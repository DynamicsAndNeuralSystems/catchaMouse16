from distutils.core import setup, Extension
import os

sourceDir = "../C/src/"

sourceFileList = [sourceDir+file for file in os.listdir(sourceDir) if file.endswith(".c") and not 'main' in file]; #  and not (file == "sampen.c" or file == "run_features.c")];

# the c++ extension module
extension_mod = Extension("catchaMouse16_C", 
	sources=["catchaMouse16_wrap_P3.c"] + sourceFileList, 
	include_dirs=[sourceDir], libraries=["gsl", "gslcblas"])

# setup(name = "catch22", ext_modules=[extension_mod], packages=['catch22'])

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
