from setuptools import setup, Extension
import glob
import platform

with open("README.md", "r") as fh:
    long_description = fh.read()

setup(
    name="pytexture2dstudio",
    description="pytexture2dstudio",
    author="smalls",
    author_email='smalls0098@gmail.com',
    version="0.1.2",
    keywords=["etc", "etc2", "astc", "texture"],
    classifiers=[
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
        "Intended Audience :: Developers",
        "Programming Language :: Python",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Topic :: Multimedia :: Graphics",
    ],
    url="https://github.com/smalls0098/py-texture2dStudio",
    download_url="https://github.com/smalls0098/py-texture2dStudio/tarball/master",
    long_description=long_description,
    long_description_content_type="text/markdown",
    ext_modules=[
        Extension(
            "pytexture2dstudio",
            [
                *glob.glob("src/texture2dstudio/src/*.cpp"),
                # etc2comp
                *glob.glob("src/texture2dstudio/thirdparty/etc2comp/EtcLib/Etc/*.cpp"),
                *glob.glob("src/texture2dstudio/thirdparty/etc2comp/EtcLib/EtcCodec/*.cpp"),
                # lodepng
                *glob.glob("src/texture2dstudio/thirdparty/lodepng/*.cpp"),
                # astc
                *glob.glob("src/texture2dstudio/thirdparty/astc-encoder/Source/*.cpp"),
                # texture2ddecoder
                *glob.glob("src/texture2dstudio/thirdparty/texture2ddecoder/*.cpp"),

                "src/texture2dstudio/texture2d.cpp",
                "src/pylink.cpp",
            ],
            language="c++",
            include_dirs=[
                "src/texture2dstudio",
                "src/texture2dstudio/src",
                "src/texture2dstudio/thirdparty/etc2comp/EtcLib/Etc",
                "src/texture2dstudio/thirdparty/etc2comp/EtcLib/EtcCodec",
                "src/texture2dstudio/thirdparty/lodepng",
                "src/texture2dstudio/thirdparty/astc-encoder/Source",
                "src/texture2dstudio/thirdparty/texture2ddecoder",
                "src/texture2dstudio/thirdparty/texture2ddecoder/crunch",
                "src/texture2dstudio/thirdparty/texture2ddecoder/fp16",
                "src/texture2dstudio/thirdparty/texture2ddecoder/unitycrunch",
            ],
            extra_compile_args=["-std=c++11"] + (
                ["-Wno-c++11-narrowing"] if platform.system() == "Darwin" else []
            ),
        )
    ],
)
