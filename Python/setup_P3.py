import os
import shlex
import subprocess

from setuptools import setup, Extension

sourceDir = "../C/src/"

sourceFileList = [sourceDir+file for file in os.listdir(sourceDir) if file.endswith(".c") and not 'main' in file]


def gsl_flags():
    """Find GSL's include and library directories with gsl-config, if it is on the PATH.

    Otherwise rely on the compiler's default search paths (and CFLAGS/LDFLAGS).
    """
    try:
        cflags = subprocess.check_output(["gsl-config", "--cflags"], text=True)
        libs = subprocess.check_output(["gsl-config", "--libs"], text=True)
    except (OSError, subprocess.CalledProcessError):
        return [], []
    include_dirs = [flag[2:] for flag in shlex.split(cflags) if flag.startswith("-I")]
    library_dirs = [flag[2:] for flag in shlex.split(libs) if flag.startswith("-L")]
    return include_dirs, library_dirs


gslIncludeDirs, gslLibraryDirs = gsl_flags()

# the C extension module
extension_mod = Extension("catchaMouse16_C",
	sources=["catchaMouse16_wrap_P3.c"] + sourceFileList,
	include_dirs=[sourceDir] + gslIncludeDirs,
	library_dirs=gslLibraryDirs,
	extra_link_args=["-Wl,-rpath," + d for d in gslLibraryDirs],
	libraries=["gsl", "gslcblas"])

setup(
name="catchaMouse16",
version="0.1.0",
author="Imran Alam",
url="https://github.com/NeuralSystemsAndSignals/catchaMouse16",
description="CAnonical Time-series Features for mouse fMRI, see description and license on GitHub.",
ext_modules=[extension_mod],
packages=['catchaMouse16'],
classifiers=[
    "Programming Language :: Python :: 3",
    "License :: OSI Approved :: GNU General Public License v3 (GPLv3)",
    "Operating System :: OS Independent",
    ],
)
