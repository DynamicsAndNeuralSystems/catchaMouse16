# *catchaMouse16*: CAnonical Time-series CHaracteristics for Mouse fMRI

This is a collection of 16 time series features contained in the hctsa toolbox coded in C. Features were selected by their classification performance across a collection of mouse fMRI time-series classification problems. (according to the *[op_importance](https://github.com/imraniac/op_importance)* pipeline)

For information on the full set of over 7000 features, see the following (open) publications:

* B.D. Fulcher and N.S. Jones. [_hctsa_: A computational framework for automated time-series phenotyping using massive feature extraction](http://www.cell.com/cell-systems/fulltext/S2405-4712\(17\)30438-6). *Cell Systems* **5**, 527 (2017).
* B.D. Fulcher, M.A. Little, N.S. Jones [Highly comparative time-series analysis: the empirical structure of time series and their methods](http://rsif.royalsocietypublishing.org/content/10/83/20130048.full). *J. Roy. Soc. Interface* **10**, 83 (2013).



# Prerequisite: Installation of additional libraries

Install GSL library using brew (for Mac Users):
```
$ brew install gsl
```
If there is a permission denied error, try this and install again:
```
$ sudo chown -R $(whoami) $(brew --prefix)/*
```
Check if it is installed correctly with this command (it should print the path of the library)
```
$ gsl-config --cflags --libs-without-cblas
```

In Linux, GSL can be simply installed via
```
$ sudo apt-get install libgsl-dev
```

# Using the *catchaMouse16*-features from C, Matlab and Python

The features are efficiently implemented in C and it can also be used in Matlab and Python. Currently it has been tested only on OS X and Linux platforms.

## Raw C

### Compilation

To compile, simply run the makefile inside the 'C' folder using
```
$ make
```

### Usage
Then run the executable file (make sure you have provided the correct csv or text file name containing the time series data)
```
$ ./run_feat <infile> <outfile>
```

The outfile is optional. If not provided then it will print in stdout.

Each line of the output corresponds to a feature with the following comma-separated entries: `feature value`, `name` and `execution time` 

## Matlab

### Compilation
Go to the ‘Matlab' directory and run _mexAll_ command in Matlab application
```
mexAll
```
This will create a bunch of Matlab executable files with name *catchaMouse16_<feature_name>*.

### Usage
Now you can test it by run the *catchaMouse16_all* script 
```
ts_data = randn(100,1) % column vector
catchaMouse16_all(ts_data)
```
This will return a feature vector.

Or you can also call the features individually, for example “AC_nl_035” :
```
catchaMouse16_AC_nl_035(ts_data)
```

## Python

The wrapper needs to be build with linked C library before being importable into some python code. Installation procedure for Python 2 and Python 3 are given below. (If there is a permission denial try with 'sudo')

### Install in Python 2
Copy the following command and run it inside the 'Python' directory
```
$ python setup.py build
$ python setup.py install
```
To test this module execute the test python script:
```
$ python test_features.py
```
### Install in Python 3
Copy the following command and run it inside the 'Python' directory
```
$ python3 setup_P3.py build
$ python3 setup_P3.py install
```
To test this module execute the test python script:
```
$ python3 test_features.py
```

### Usage
Now you can import *catchaMouse16* in your code and call the individual features

```
import catchaMouse16
ts_data = np.rand(100,1)
catchaMouse16.SC_FluctAnal_2_dfa_50_2_logi_r2_se2(ts_data)
```

Or get the feature vector using
```
from catchaMouse16 import catchaMouse16_all
features = catchaMouse16_all(ts_data)
```

# Contribution

Please feel free to ask any query in the Issue section. Any suggestions or improvements are welcomed through Pull Requests.
