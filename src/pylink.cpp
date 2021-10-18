#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include "texture2d.h"


static PyObject *_CompressEtc1(PyObject *self, PyObject *args)
{
    // define vars
    uint8_t *data;
    size_t data_size;
    int mipmap, fEffort, jobs, header;

    if (!PyArg_ParseTuple(args, "y#iiii", &data, &data_size, &mipmap, &fEffort, &jobs, &header))
        return NULL;

    uint8_t *out = nullptr;
    size_t outsize = 0;

    if (CompressEtc1(data, data_size, mipmap, fEffort, jobs, header, &out, &outsize) == 0) {
        return NULL;
    }

    PyObject *res = Py_BuildValue("y#", out, outsize);
    free(out);
    return res;
}

static PyObject *_CompressEtc2RGB(PyObject *self, PyObject *args)
{
    //uint8_t *src, size_t size, int mipmap, float fEffort, int jobs, int header,
    //            uint8_t **dst, size_t *filesize

    // define vars
    uint8_t *data;
    size_t data_size;
    int mipmap, fEffort, jobs, header;

    if (!PyArg_ParseTuple(args, "y#iiii", &data, &data_size, &mipmap, &fEffort, &jobs, &header))
        return NULL;

    uint8_t *out = nullptr;
    size_t outsize = 0;

    if (CompressEtc2RGB(data, data_size, mipmap, fEffort, jobs, header, &out, &outsize) == 0) {
        return NULL;
    }

    PyObject *res = Py_BuildValue("y#", out, outsize);
    free(out);
    return res;
}

static PyObject *_CompressEtc2RGBA(PyObject *self, PyObject *args)
{
    //uint8_t *src, size_t size, int mipmap, float fEffort, int jobs, int header,
    //            uint8_t **dst, size_t *filesize

    // define vars
    uint8_t *data;
    size_t data_size;
    int mipmap, fEffort, jobs, header;

    if (!PyArg_ParseTuple(args, "y#iiii", &data, &data_size, &mipmap, &fEffort, &jobs, &header))
        return NULL;

    uint8_t *out = nullptr;
    size_t outsize = 0;

    if (CompressEtc2RGBA(data, data_size, mipmap, fEffort, jobs, header, &out, &outsize) == 0) {
        return NULL;
    }

    PyObject *res = Py_BuildValue("y#", out, outsize);
    free(out);
    return res;
}

static PyObject *_CompressEtc1WithFile(PyObject *self, PyObject *args)
{
    // define vars
    char *input;
    char *output;
    int mipmap, fEffort, jobs;
    if (!PyArg_ParseTuple(args, "ssiii", &input, &output, &mipmap, &fEffort, &jobs))
        return NULL;
    int result = CompressEtc1WithFile(input, output, mipmap, fEffort, jobs);
    PyObject *res = Py_BuildValue("i", result);
    return res;
}

static PyObject *_CompressEtc2RGBWithFile(PyObject *self, PyObject *args)
{
    // define vars
    char *input;
    char *output;
    int mipmap, fEffort, jobs;
    if (!PyArg_ParseTuple(args, "ssiii", &input, &output, &mipmap, &fEffort, &jobs))
        return NULL;
    int result = CompressEtc2RGBWithFile(input, output, mipmap, fEffort, jobs);
    PyObject *res = Py_BuildValue("i", result);
    return res;
}

static PyObject *_CompressEtc2RGBAWithFile(PyObject *self, PyObject *args)
{
    // define vars
    char *input;
    char *output;
    int mipmap, fEffort, jobs;
    if (!PyArg_ParseTuple(args, "ssiii", &input, &output, &mipmap, &fEffort, &jobs))
        return NULL;
    int result = CompressEtc2RGBAWithFile(input, output, mipmap, fEffort, jobs);
    PyObject *res = Py_BuildValue("i", result);
    return res;
}


// ====================astc compress


static PyObject *_CompressAstc(PyObject *self, PyObject *args)
{
    // define vars
    uint8_t *data;
    size_t data_size;
    int fEffort, block_x, block_y, block_z, header;

    if (!PyArg_ParseTuple(args, "y#iiiii", &data, &data_size, &fEffort, &block_x, &block_y, &block_z, &header))
        return NULL;

    uint8_t *out = nullptr;
    size_t outsize = 0;

    if (CompressAstc(data, data_size, fEffort, block_x, block_y, block_z, header, &out, &outsize) == 0) {
        return NULL;
    }

    PyObject *res = Py_BuildValue("y#", out, outsize);
    free(out);
    return res;
}

static PyObject *_CompressAstcWithFile(PyObject *self, PyObject *args)
{
    // define vars
    char *input;
    char *output;
    int fEffort, block_x, block_y, block_z;

    if (!PyArg_ParseTuple(args, "ssiiii", &input, &output, &fEffort, &block_x, &block_y, &block_z))
        return NULL;

    int result = CompressAstcWithFile(input, output, fEffort, block_x, block_y, block_z);
    PyObject *res = Py_BuildValue("i", result);
    return res;
}


// ================ decode


static PyObject *_DecompressEtc1(PyObject *self, PyObject *args)
{
    // define vars
    uint8_t *data;
    size_t data_size;
    int w, h;
    if (!PyArg_ParseTuple(args, "y#ii", &data, &data_size, &w, &h))
        return NULL;
    uint32_t *out = nullptr;
    size_t outsize = 0;
    if (DecompressEtc1(data, w, h, &out, &outsize) == 0) {
        return NULL;
    }
    PyObject *res = Py_BuildValue("y#", out, outsize);
    free(out);
    return res;
}

static PyObject *_DecompressEtc2(PyObject *self, PyObject *args)
{
    // define vars
    uint8_t *data;
    size_t data_size;
    int w, h;
    if (!PyArg_ParseTuple(args, "y#ii", &data, &data_size, &w, &h))
        return NULL;
    uint32_t *out = nullptr;
    size_t outsize = 0;
    if (DecompressEtc2(data, w, h, &out, &outsize) == 0) {
        return NULL;
    }
    PyObject *res = Py_BuildValue("y#", out, outsize);
    free(out);
    return res;
}

static PyObject *_DecompressEtc2a1(PyObject *self, PyObject *args)
{
    // define vars
    uint8_t *data;
    size_t data_size;
    int w, h;
    if (!PyArg_ParseTuple(args, "y#ii", &data, &data_size, &w, &h))
        return NULL;
    uint32_t *out = nullptr;
    size_t outsize = 0;
    if (DecompressEtc2a1(data, w, h, &out, &outsize) == 0) {
        return NULL;
    }
    PyObject *res = Py_BuildValue("y#", out, outsize);
    free(out);
    return res;
}

static PyObject *_DecompressEtc2a8(PyObject *self, PyObject *args)
{
    // define vars
    uint8_t *data;
    size_t data_size;
    int w, h;
    if (!PyArg_ParseTuple(args, "y#ii", &data, &data_size, &w, &h))
        return NULL;
    uint32_t *out = nullptr;
    size_t outsize = 0;
    if (DecompressEtc2a8(data, w, h, &out, &outsize) == 0) {
        return NULL;
    }
    PyObject *res = Py_BuildValue("y#", out, outsize);
    free(out);
    return res;
}

static PyObject *_DecompressAstc(PyObject *self, PyObject *args)
{
    // define vars
    uint8_t *data;
    size_t data_size;
    int w, h, block_width, block_height;
    if (!PyArg_ParseTuple(args, "y#iiii", &data, &data_size, &w, &h, &block_width, &block_height))
        return NULL;
    uint32_t *out = nullptr;
    size_t outsize = 0;
    if (DecompressAstc(data, w, h, block_width, block_height, &out, &outsize) == 0) {
        return NULL;
    }
    PyObject *res = Py_BuildValue("y#", out, outsize);
    free(out);
    return res;
}

static PyObject *_DecompressEtc1ToFile(PyObject *self, PyObject *args)
{
    // define vars
    uint8_t *data;
    size_t data_size;
    char *output;
    int w, h;
    if (!PyArg_ParseTuple(args, "y#sii", &data, &data_size, &output, &w, &h))
        return NULL;
    int result = DecompressEtc1ToFile(data, w, h, output);
    PyObject *res = Py_BuildValue("i", result);
    return res;
}

static PyObject *_DecompressEtc2ToFile(PyObject *self, PyObject *args)
{
    // define vars
    uint8_t *data;
    size_t data_size;
    char *output;
    int w, h;
    if (!PyArg_ParseTuple(args, "y#sii", &data, &data_size, &output, &w, &h))
        return NULL;
    int result = DecompressEtc2ToFile(data, w, h, output);
    PyObject *res = Py_BuildValue("i", result);
    return res;
}

static PyObject *_DecompressEtc2a1ToFile(PyObject *self, PyObject *args)
{
    // define vars
    uint8_t *data;
    size_t data_size;
    char *output;
    int w, h;
    if (!PyArg_ParseTuple(args, "y#sii", &data, &data_size, &output, &w, &h))
        return NULL;
    int result = DecompressEtc2a1ToFile(data, w, h, output);
    PyObject *res = Py_BuildValue("i", result);
    return res;
}

static PyObject *_DecompressEtc2a8ToFile(PyObject *self, PyObject *args)
{
    // define vars
    uint8_t *data;
    size_t data_size;
    char *output;
    int w, h;
    if (!PyArg_ParseTuple(args, "y#sii", &data, &data_size, &output, &w, &h))
        return NULL;
    int result = DecompressEtc2a8ToFile(data, w, h, output);
    PyObject *res = Py_BuildValue("i", result);
    return res;
}

static PyObject *_DecompressAstcToFile(PyObject *self, PyObject *args)
{
    // define vars
    uint8_t *data;
    size_t data_size;
    char *output;
    int w, h, block_width, block_height;
    if (!PyArg_ParseTuple(args, "y#siiii", &data, &data_size, &output, &w, &h, &block_width, &block_height))
        return NULL;
    int result = DecompressAstcToFile(data, w, h, block_width, block_height, output);
    PyObject *res = Py_BuildValue("i", result);
    return res;
}

/*
 *************************************************
 *
 * python connection
 *
 ************************************************
*/

// Exported methods are collected in a table
static struct PyMethodDef method_table[] = {
    {"CompressEtc1",
     (PyCFunction)_CompressEtc1,
     METH_VARARGS,
     "bytes data, size_t data_size, int mipmap, int fEffort, int jobs, int header"},
     {"CompressEtc2RGB",
     (PyCFunction)_CompressEtc2RGB,
     METH_VARARGS,
     "bytes data, size_t data_size, int mipmap, int fEffort, int jobs, int header"},
     {"CompressEtc2RGBA",
     (PyCFunction)_CompressEtc2RGBA,
     METH_VARARGS,
     "bytes data, size_t data_size, int mipmap, int fEffort, int jobs, int header"},
     {"CompressEtc1WithFile",
     (PyCFunction)_CompressEtc1WithFile,
     METH_VARARGS,
     "string inputfile, string outputfile, size_t data_size, int mipmap, int fEffort, int jobs"},
     {"CompressEtc2RGBWithFile",
     (PyCFunction)_CompressEtc2RGBWithFile,
     METH_VARARGS,
     "string inputfile, string outputfile, size_t data_size, int mipmap, int fEffort, int jobs"},
     {"CompressEtc2RGBAWithFile",
     (PyCFunction)_CompressEtc2RGBAWithFile,
     METH_VARARGS,
     "string inputfile, string outputfile, size_t data_size, int mipmap, int fEffort, int jobs"},
     {"CompressAstc",
     (PyCFunction)_CompressAstc,
     METH_VARARGS,
     "bytes data, size_t data_size, int fEffort, int block_x, int block_y, int block_z, int header"},
     {"CompressAstcWithFile",
     (PyCFunction)_CompressAstcWithFile,
     METH_VARARGS,
     "string inputfile, string outputfile, int fEffort, int block_x, int block_y, int block_z"},
     {"DecompressEtc1",
     (PyCFunction)_DecompressEtc1,
     METH_VARARGS,
     "bytes data, size_t data_size, int w, int h"},
     {"DecompressEtc2",
     (PyCFunction)_DecompressEtc2,
     METH_VARARGS,
     "bytes data, size_t data_size, int w, int h"},
      {"DecompressEtc2a1",
     (PyCFunction)_DecompressEtc2a1,
     METH_VARARGS,
     "bytes data, size_t data_size, int w, int h"},
     {"DecompressEtc2a8",
     (PyCFunction)_DecompressEtc2a8,
     METH_VARARGS,
     "bytes data, size_t data_size, int w, int h"},
     {"DecompressEtc1ToFile",
     (PyCFunction)_DecompressEtc1ToFile,
     METH_VARARGS,
     "string inputfile, string outputfile, int w, int h"},
     {"DecompressEtc2ToFile",
     (PyCFunction)_DecompressEtc2ToFile,
     METH_VARARGS,
     "string inputfile, string outputfile, int w, int h"},
     {"DecompressEtc2a1ToFile",
     (PyCFunction)_DecompressEtc2a1ToFile,
     METH_VARARGS,
     "string inputfile, string outputfile, int w, int h"},
     {"DecompressEtc2a8ToFile",
     (PyCFunction)_DecompressEtc2a8ToFile,
     METH_VARARGS,
     "string inputfile, string outputfile, int w, int h"},
      {"DecompressAstc",
     (PyCFunction)_DecompressAstc,
     METH_VARARGS,
     "bytes data, size_t data_size, int w, int h, int block_width, int block_height"},
     {"DecompressAstcToFile",
     (PyCFunction)_DecompressAstcToFile,
     METH_VARARGS,
     "string inputfile, string outputfile, int w, int h, int block_width, int block_height"},
    {NULL,
     NULL,
     0,
     NULL} // Sentinel value ending the table
};

// A struct contains the definition of a module
static PyModuleDef pytexture2dstudio_module = {
    PyModuleDef_HEAD_INIT,
    "pytexture2dstudio", // Module name
    "pytexture2dstudio",
    -1, // Optional size of the module state memory
    method_table,
    NULL, // Optional slot definitions
    NULL, // Optional traversal function
    NULL, // Optional clear function
    NULL  // Optional module deallocation function
};


// The module init function
PyMODINIT_FUNC PyInit_pytexture2dstudio(void)
{
    return PyModule_Create(&pytexture2dstudio_module);
}