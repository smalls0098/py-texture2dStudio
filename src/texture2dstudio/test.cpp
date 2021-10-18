//
// Created by smalls on 2021/8/18.
//
typedef unsigned int astcenc_operation;


/** @brief Stage bit indicating we need to load an uncompressed image. */
static const unsigned int ASTCENC_STAGE_LD_NCOMP = 1 << 2;

/** @brief Stage bit indicating we need compress an image. */
static const unsigned int ASTCENC_STAGE_COMPRESS = 1 << 4;

/** @brief Stage bit indicating we need to store a compressed image. */
static const unsigned int ASTCENC_STAGE_ST_COMP = 1 << 1;

/** @brief Operation indicating the user wants to compress and store an image. */
static const astcenc_operation ASTCENC_OP_COMPRESS =
        ASTCENC_STAGE_LD_NCOMP |
        ASTCENC_STAGE_COMPRESS |
        ASTCENC_STAGE_ST_COMP;

/**
 * @brief Image preprocesing tasks prior to encoding.
 */
enum astcenc_preprocess {
    /** @brief No image preprocessing. */
    ASTCENC_PP_NONE = 0,
    /** @brief Normal vector unit-length normalization. */
    ASTCENC_PP_NORMALIZE,
    /** @brief Color data alpha premultiplication. */
    ASTCENC_PP_PREMULTIPLY
};


static int init_astcenc_config(
        astcenc_profile profile,
        astcenc_operation operation,
        astc_compressed_image &comp_image,
        astcenc_preprocess &preprocess,
        astcenc_config &config
) {
    unsigned int block_x = 0;
    unsigned int block_y = 0;
    unsigned int block_z = 1;

    float quality = 0.0f;
    preprocess = ASTCENC_PP_NONE;

    // parse the command line's encoding options.
    if (operation & ASTCENC_STAGE_COMPRESS) {
        int cnt2D, cnt3D;
        sscanf("8x8", "%ux%u%nx%u%n",
               &block_x, &block_y, &cnt2D, &block_z, &cnt3D);
    }

    unsigned int flags = 0;

    flags |= ASTCENC_FLG_SELF_DECOMPRESS_ONLY;

    astcenc_error status = astcenc_config_init(profile, block_x, block_y, block_z,
                                               quality, flags, &config);

    if (status == ASTCENC_ERR_BAD_BLOCK_SIZE) {
        printf("ERROR: Block size '%s' is invalid\n", "8x8");
        return 1;
    } else if (status == ASTCENC_ERR_BAD_CPU_ISA) {
        printf("ERROR: Required SIMD ISA support missing on this CPU\n");
        return 1;
    } else if (status == ASTCENC_ERR_BAD_CPU_FLOAT) {
        printf("ERROR: astcenc must not be compiled with -ffast-math\n");
        return 1;
    } else if (status != ASTCENC_SUCCESS) {
        printf("ERROR: Init config failed with %s\n", astcenc_get_error_string(status));
        return 1;
    }

    return 0;
}

/**
 * @brief Utility to generate a slice file name from a pattern.
 *
 * Convert "foo/bar.png" in to "foo/bar_<slice>.png"
 *
 * @param basename The base pattern; must contain a file extension.
 * @param index    The slice index.
 * @param error    Set to true on success, false on error (no extension found).
 *
 * @return The slice file name.
 */
static std::string get_slice_filename(
        const std::string &basename,
        unsigned int index,
        bool &error
) {
    size_t sep = basename.find_last_of(".");
    if (sep == std::string::npos) {
        error = true;
        return "";
    }

    std::string base = basename.substr(0, sep);
    std::string ext = basename.substr(sep);
    std::string name = base + "_" + std::to_string(index) + ext;
    error = false;
    return name;
}


/**
 * @brief Load a non-astc image file from memory.
 *
 * @param filename            The file to load, or a pattern for array loads.
 * @param dim_z               The number of slices to load.
 * @param y_flip              Should this image be Y flipped?
 * @param[out] is_hdr         Is the loaded image HDR?
 * @param[out] component_count The number of components in the loaded image.
 *
 * @return The astc image file, or nullptr on error.
 */
static astcenc_image *load_uncomp_file(
        const char *filename,
        unsigned int dim_z,
        bool y_flip,
        bool &is_hdr,
        unsigned int &component_count
) {
    astcenc_image *image = nullptr;

    // For a 2D image just load the image directly
    if (dim_z == 1) {
        image = load_ncimage(filename, y_flip, is_hdr, component_count);
    } else {
        bool slice_is_hdr;
        unsigned int slice_component_count;
        astcenc_image *slice = nullptr;
        std::vector < astcenc_image * > slices;

        // For a 3D image load an array of slices
        for (unsigned int image_index = 0; image_index < dim_z; image_index++) {
            bool error;
            std::string slice_name = get_slice_filename(filename, image_index, error);
            if (error) {
                printf("ERROR: Image pattern does not contain file extension: %s\n", filename);
                break;
            }

            slice = load_ncimage(slice_name.c_str(), y_flip,
                                 slice_is_hdr, slice_component_count);
            if (!slice) {
                break;
            }

            slices.push_back(slice);

            // Check it is not a 3D image
            if (slice->dim_z != 1) {
                printf("ERROR: Image arrays do not support 3D sources: %s\n", slice_name.c_str());
                break;
            }

            // Check slices are consistent with each other
            if (image_index != 0) {
                if ((is_hdr != slice_is_hdr) || (component_count != slice_component_count)) {
                    printf("ERROR: Image array[0] and [%d] are different formats\n", image_index);
                    break;
                }

                if ((slices[0]->dim_x != slice->dim_x) ||
                    (slices[0]->dim_y != slice->dim_y) ||
                    (slices[0]->dim_z != slice->dim_z)) {
                    printf("ERROR: Image array[0] and [%d] are different dimensions\n", image_index);
                    break;
                }
            } else {
                is_hdr = slice_is_hdr;
                component_count = slice_component_count;
            }
        }

        // If all slices loaded correctly then repack them into a single image
        if (slices.size() == dim_z) {
            unsigned int dim_x = slices[0]->dim_x;
            unsigned int dim_y = slices[0]->dim_y;
            int bitness = is_hdr ? 16 : 8;
            int slice_size = dim_x * dim_y;

            image = alloc_image(bitness, dim_x, dim_y, dim_z);

            // Combine 2D source images into one 3D image
            for (unsigned int z = 0; z < dim_z; z++) {
                if (image->data_type == ASTCENC_TYPE_U8) {
                    uint8_t *data8 = static_cast<uint8_t *>(image->data[z]);
                    uint8_t *data8src = static_cast<uint8_t *>(slices[z]->data[0]);
                    size_t copy_size = slice_size * 4 * sizeof(uint8_t);
                    memcpy(data8, data8src, copy_size);
                } else if (image->data_type == ASTCENC_TYPE_F16) {
                    uint16_t *data16 = static_cast<uint16_t *>(image->data[z]);
                    uint16_t *data16src = static_cast<uint16_t *>(slices[z]->data[0]);
                    size_t copy_size = slice_size * 4 * sizeof(uint16_t);
                    memcpy(data16, data16src, copy_size);
                } else // if (image->data_type == ASTCENC_TYPE_F32)
                {
                    assert(image->data_type == ASTCENC_TYPE_F32);
                    float *data32 = static_cast<float *>(image->data[z]);
                    float *data32src = static_cast<float *>(slices[z]->data[0]);
                    size_t copy_size = slice_size * 4 * sizeof(float);
                    memcpy(data32, data32src, copy_size);
                }
            }
        }

        for (auto &i : slices) {
            free_image(i);
        }
    }

    return image;
}

/**
 * @brief Compression workload definition for worker threads.
 */
struct compression_workload {
    astcenc_context *context;
    astcenc_image *image;
    astcenc_swizzle swizzle;
    uint8_t *data_out;
    size_t data_len;
    astcenc_error error;
};


int test() {
    double start_time = get_time();
    //    compAstc();
    astcenc_operation operation = ASTCENC_OP_COMPRESS;
    astcenc_profile profile = ASTCENC_PRF_LDR;

    int error;
    std::string input_filename = "E:\\git\\texture2dstudio\\tests\\img.png";
    std::string output_filename = "E:\\speedm\\0801\\demo\\example.astc";

    astc_compressed_image image_comp{};

    astcenc_config config{};
    astcenc_preprocess preprocess;
    error = init_astcenc_config(profile, operation, image_comp, preprocess, config);
    if (error) {
        return 1;
    }

    // Initialize cli_config_options with default values
    cli_config_options cli_config{1, 1, false, false, -10, 10,
                                  {ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A},
                                  {ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A}};

    astcenc_image *image_uncomp_in = nullptr;
    unsigned int image_uncomp_in_component_count = 0;
    bool image_uncomp_in_is_hdr = false;
    astcenc_image *image_decomp_out = nullptr;

    // TODO: Handle RAII resources so they get freed when out of scope
    astcenc_error codec_status;
    astcenc_context *codec_context;

    codec_status = astcenc_context_alloc(&config, cli_config.thread_count, &codec_context);
    if (codec_status != ASTCENC_SUCCESS) {
        printf("ERROR: Codec context alloc failed: %s\n", astcenc_get_error_string(codec_status));
        return 1;
    }

    FILE *fd = fopen(input_filename.c_str(), "rb");
    assert(fd);
    size_t size;
    fseek(fd, 0, SEEK_END);
    size = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    uint8_t *in = new uint8_t[size];
    fread(in, size, 1, fd);
    fclose(fd);

    int dim_x, dim_y;
    uint8_t *data = data = stbi_load_from_memory(in, size, &dim_x, &dim_y, nullptr, STBI_rgb_alpha);

    image_uncomp_in = astc_img_from_unorm8x4_array(data,
                                                   dim_x,
                                                   dim_y,
                                                   false);

    image_uncomp_in = load_uncomp_file(
            input_filename.c_str(), cli_config.array_size, cli_config.y_flip,
            image_uncomp_in_is_hdr, image_uncomp_in_component_count);
    if (!image_uncomp_in) {
        printf("ERROR: Failed to load uncompressed image file\n");
        return 1;
    }

    if (!cli_config.silentmode) {
        printf("Source image\n");
        printf("============\n\n");
        printf("    Source:                     %s\n", input_filename.c_str());
        printf("    Color profile:              %s\n", image_uncomp_in_is_hdr ? "HDR" : "LDR");
        if (image_uncomp_in->dim_z > 1) {
            printf("    Dimensions:                 3D, %ux%ux%u\n",
                   image_uncomp_in->dim_x, image_uncomp_in->dim_y, image_uncomp_in->dim_z);
        } else {
            printf("    Dimensions:                 2D, %ux%u\n",
                   image_uncomp_in->dim_x, image_uncomp_in->dim_y);
        }
        printf("    Components:                 %d\n\n", image_uncomp_in_component_count);
    }

    double start_coding_time = get_time();

    double image_size = 0.0;
    if (image_uncomp_in) {
        image_size = (double) image_uncomp_in->dim_x *
                     (double) image_uncomp_in->dim_y *
                     (double) image_uncomp_in->dim_z;
    } else {
        image_size = (double) image_comp.dim_x *
                     (double) image_comp.dim_y *
                     (double) image_comp.dim_z;
    }

    unsigned int blocks_x = (image_uncomp_in->dim_x + config.block_x - 1) / config.block_x;
    unsigned int blocks_y = (image_uncomp_in->dim_y + config.block_y - 1) / config.block_y;
    unsigned int blocks_z = (image_uncomp_in->dim_z + config.block_z - 1) / config.block_z;
    size_t buffer_size = blocks_x * blocks_y * blocks_z * 16;
    uint8_t *buffer = new uint8_t[buffer_size];

    compression_workload work;
    work.context = codec_context;
    work.image = image_uncomp_in;
    work.swizzle = cli_config.swz_encode;
    work.data_out = buffer;
    work.data_len = buffer_size;
    work.error = ASTCENC_SUCCESS;

    work.error = astcenc_compress_image(
            work.context, work.image, &work.swizzle,
            work.data_out, work.data_len, 0);

    if (work.error != ASTCENC_SUCCESS) {
        printf("ERROR: Codec compress failed: %s\n", astcenc_get_error_string(work.error));
        return 1;
    }

    image_comp.block_x = config.block_x;
    image_comp.block_y = config.block_y;
    image_comp.block_z = config.block_z;
    image_comp.dim_x = image_uncomp_in->dim_x;
    image_comp.dim_y = image_uncomp_in->dim_y;
    image_comp.dim_z = image_uncomp_in->dim_z;
    image_comp.data = buffer;
    image_comp.data_len = buffer_size;



    /* ============================================================================
	        ASTC compressed file loading
       ============================================================================ */
    struct astc_header {
        uint8_t magic[4];
        uint8_t block_x;
        uint8_t block_y;
        uint8_t block_z;
        uint8_t dim_x[3];            // dims = dim[0] + (dim[1] << 8) + (dim[2] << 16)
        uint8_t dim_y[3];            // Sizes are given in texels;
        uint8_t dim_z[3];            // block count is inferred
    };

    static const uint32_t ASTC_MAGIC_ID = 0x5CA1AB13;

    astc_header hdr;
    hdr.magic[0] = ASTC_MAGIC_ID & 0xFF;
    hdr.magic[1] = (ASTC_MAGIC_ID >> 8) & 0xFF;
    hdr.magic[2] = (ASTC_MAGIC_ID >> 16) & 0xFF;
    hdr.magic[3] = (ASTC_MAGIC_ID >> 24) & 0xFF;

    hdr.block_x = static_cast<uint8_t>(image_comp.block_x);
    hdr.block_y = static_cast<uint8_t>(image_comp.block_y);
    hdr.block_z = static_cast<uint8_t>(image_comp.block_z);

    hdr.dim_x[0] = image_comp.dim_x & 0xFF;
    hdr.dim_x[1] = (image_comp.dim_x >> 8) & 0xFF;
    hdr.dim_x[2] = (image_comp.dim_x >> 16) & 0xFF;

    hdr.dim_y[0] = image_comp.dim_y & 0xFF;
    hdr.dim_y[1] = (image_comp.dim_y >> 8) & 0xFF;
    hdr.dim_y[2] = (image_comp.dim_y >> 16) & 0xFF;

    hdr.dim_z[0] = image_comp.dim_z & 0xFF;
    hdr.dim_z[1] = (image_comp.dim_z >> 8) & 0xFF;
    hdr.dim_z[2] = (image_comp.dim_z >> 16) & 0xFF;

    size_t offset = 0;
    auto *buf = (uint8_t *) malloc(sizeof(astc_header) + image_comp.data_len * sizeof(uint8_t));
    memcpy(&buf[offset], &hdr, sizeof(astc_header));
    offset += sizeof(astc_header);
    memcpy(&buf[offset], image_comp.data, image_comp.data_len);
    offset += image_comp.data_len;

    FILE *ofd = fopen(output_filename.c_str(), "wb");
    assert(ofd);
    fseek(ofd, 0, SEEK_SET);
    fwrite(buf, offset, 1, ofd);
    fclose(ofd);

    //    error = store_cimage(image_comp, output_filename.c_str());
    //    if (error) {
    //        printf("ERROR: Failed to store compressed image\n");
    //        return 1;
    //    }

    free_image(image_uncomp_in);
    free_image(image_decomp_out);
    astcenc_context_free(codec_context);

    delete[] image_comp.data;

    double end_coding_time = get_time();

    double end_time = get_time();
    double tex_rate = image_size / (end_coding_time - start_coding_time);
    tex_rate = tex_rate / 1000000.0;

    printf("Performance metrics\n");
    printf("===================\n\n");
    printf("    Total time:                %8.4f s\n", end_time - start_time);
    printf("    Coding time:               %8.4f s\n", end_coding_time - start_coding_time);
    printf("    Coding rate:               %8.4f MT/s\n", tex_rate);

}