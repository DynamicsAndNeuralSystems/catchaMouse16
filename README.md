# *catchaMouse16*: CAnonical Time-series CHaracteristics for Mouse fMRI

This is a collection of 16 time series features contained in the hctsa toolbox coded in C. Features were selected by their classification performance across a collection of mouse fMRI time-series classification problems. (according to the *[op_importance](https://github.com/imraniac/op_importance)* pipeline)

For information on the full set of over 7000 features, see the following (open) publications:

* B.D. Fulcher and N.S. Jones. [_hctsa_: A computational framework for automated time-series phenotyping using massive feature extraction](http://www.cell.com/cell-systems/fulltext/S2405-4712\(17\)30438-6). *Cell Systems* **5**, 527 (2017).
* B.D. Fulcher, M.A. Little, N.S. Jones [Highly comparative time-series analysis: the empirical structure of time series and their methods](http://rsif.royalsocietypublishing.org/content/10/83/20130048.full). *J. Roy. Soc. Interface* **10**, 83 (2013).

# Raw C

## Installation of additional libraries

Install GSL library using brew (for Mac Users):
```
$ brew install gsl
```
If there is a permission denied error, try this and install again:
```
sudo chown -R $(whoami) $(brew --prefix)/*
```
Check if it is installed correctly with command
```
gsl-config --cflags --libs-without-cblas
```
It should print the following
```
-I/usr/local/Cellar/gsl/2.6/include
-L/usr/local/Cellar/gsl/2.6/lib -lgsl
```

## Compilation

To compile, run the makefile inside C folder
```
make
```

Then run the executable file (make sure you have provided the csv or text file name containing the time series data)
```
./run_feat
```
