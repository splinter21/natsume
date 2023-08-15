import os
import subprocess
import sys
from distutils.errors import DistutilsExecError
from distutils.spawn import spawn
from distutils.version import LooseVersion
from glob import glob
from itertools import chain
from os.path import exists, join
from subprocess import run

import numpy as np
import setuptools.command.build_py
import setuptools.command.develop
from setuptools import Extension, find_packages, setup


# Reference: https://github.com/r9y9/pyopenjtalk/blob/master/setup.py

version = "0.1.1"
min_cython_ver = "0.21.0"

try:
    import Cython

    ver = Cython.__version__
    _CYTHON_INSTALLED = ver >= LooseVersion(min_cython_ver)
except ImportError:
    _CYTHON_INSTALLED = False

try:
    if not _CYTHON_INSTALLED:
        raise ImportError("No supported version of Cython installed.")
    from Cython.Distutils import build_ext

    cython = True
except ImportError:
    cython = False


if cython:
    ext = ".pyx"
    cmdclass = {"build_ext": build_ext}
else:
    ext = ".cpp"
    cmdclass = {}
    if not os.path.exists(join("natsume", "openjtalk" + ext)):
        raise RuntimeError("Cython is required to generate C++ code")

# open_jtalk sources
src_top = join("libs", "open_jtalk", "src")

if not exists(join(src_top, "mecab", "src", "config.h")):
    cwd = os.getcwd()
    build_dir = join(src_top, "build")
    os.makedirs(build_dir, exist_ok=True)
    os.chdir(build_dir)

    r = run(["cmake", "..", "-DHTS_ENGINE_INCLUDE_DIR=.", "-DHTS_ENGINE_LIB=dummy"])
    r.check_returncode()
    os.chdir(cwd)

all_src = []
include_dirs = []

for s in [
    "mecab/src",
    "mecab2njd",
    "njd",
    "njd_set_accent_phrase",
    "njd_set_accent_type",
    "njd_set_digit",
    "njd_set_long_vowel",
    "njd_set_pronunciation",
    "njd_set_unvoiced_vowel",
    "text2mecab",
]:
    all_src += glob(join(src_top, s, "*.c"))
    all_src += glob(join(src_top, s, "*.cpp"))
    include_dirs.append(join(os.getcwd(), src_top, s))

# Extension for OpenJTalk frontend
ext_modules = [
    Extension(
        name="natsume.openjtalk",
        sources=[join("natsume", "openjtalk" + ext)] + all_src,
        include_dirs=[np.get_include()] + include_dirs,
        extra_compile_args=[],
        extra_link_args=[],
        language="c++",
        define_macros=[
            ("HAVE_CONFIG_H", None),
            ("DIC_VERSION", 102),
            ("MECAB_DEFAULT_RC", '"dummy"'),
            ("PACKAGE", '"open_jtalk"'),
            ("VERSION", '"1.10"'),
            ("CHARSET_UTF_8", None),
        ]
    )
]


cwd = os.path.dirname(os.path.abspath(__file__))

class build_py(setuptools.command.build_py.build_py):
    def run(self):
        self.create_version_file()
        setuptools.command.build_py.build_py.run(self)

    @staticmethod
    def create_version_file():
        global version, cwd
        print("-- Building version " + version)
        version_path = os.path.join(cwd, "natsume", "version.py")
        with open(version_path, "w") as f:
            f.write("__version__ = '{}'\n".format(version))

class develop(setuptools.command.develop.develop):
    def run(self):
        build_py.create_version_file()
        setuptools.command.develop.develop.run(self)


cmdclass["build_py"] = build_py
cmdclass["develop"] = develop


with open("README.md", "r", encoding="utf8") as fd:
    long_description = fd.read()

setup(
    name="natsume",
    version=version,
    description="A Japanese text frontend processing toolkit",
    long_description=long_description,
    long_description_content_type="text/markdown",
    author="Francis Hu",
    author_email="franciskomizu@gmail.com",
    url="https://github.com/Francis-Komizu/natsume",
    license="GPL",
    packages=find_packages(),
    ext_modules=ext_modules,
    cmdclass=cmdclass,
    install_requires=[
        "numpy >= 1.20.0",
        "cython >= " + min_cython_ver + ",<3.0.0",
        "six",
        "tqdm",
    ]
)