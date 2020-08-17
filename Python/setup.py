from distutils.core import setup, Extension
import os

sourceDir = "../src/"

sourceFileList = [sourceDir+file for file in os.listdir(sourceDir) if file.endswith(".c") and not 'main' in file]; #  and not (file == "sampen.c" or file == "run_features.c")];

# the c++ extension module
extension_mod = Extension("catchaMouse16_C", 
	sources=["catchaMouse16_wrap.c"] + sourceFileList, 
	include_dirs=[sourceDir])

# setup(name = "catchaMouse16", ext_modules=[extension_mod])

setup(
name="catchaMouse16",
version="0.1.0",
author="Imran Alam",
url="https://github.com/chlubba/catch22",#change
description="CAnonical Time-series Features for mouse fMRI, see description and license on GitHub.",
ext_modules=[extension_mod],
packages=['catchaMouse16'],
classifiers=[
    "Programming Language :: Python :: 3",
    "License :: OSI Approved :: GNU General Public License v3 (GPLv3)",
    "Operating System :: OS Independent",
    ],
)
