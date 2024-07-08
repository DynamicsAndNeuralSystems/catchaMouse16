<p align="center"><img src="img/catch_a_mouse16_vecorized.svg" alt="catch22 logo" height="220"/></p>

<h1 align="center"><em>catchaMouse16</em>: CAnonical Time-series CHaracteristics for Mouse fMRI</h1>

<p align="center">
 	<!-- <a href="https://zenodo.org/badge/latestdoi/146194807"><img src="https://zenodo.org/badge/146194807.svg" height="20"/></a> -->
    <a href="https://www.gnu.org/licenses/gpl-3.0"><img src="https://img.shields.io/badge/License-GPLv3-blue.svg" height="20"/></a>
 	<a href="https://twitter.com/compTimeSeries"><img src="https://img.shields.io/twitter/url/https/twitter.com/compTimeSeries.svg?style=social&label=Follow%20%40compTimeSeries" height="20"/></a>
</p>



> This is a collection of 16 time series features contained in the hctsa toolbox coded in C that can be run from Python, MATLAB and C, licensed under the [GNU GPL v3 license](http://www.gnu.org/licenses/gpl-3.0.html) (or later). Features were selected from the total set of [hctsa](https://github.com/benfulcher/hctsa/) features by their classification performance across a collection of mouse fMRI time-series classification problems. See the [pipeline and paper specific repo](https://github.com/DynamicsAndNeuralSystems/Catchamouse16_paper_code) for details.

For information on the full set of over 7000 features, see the following (open) publications:

* B.D. Fulcher and N.S. Jones. [_hctsa_: A computational framework for automated time-series phenotyping using massive feature extraction](http://www.cell.com/cell-systems/fulltext/S2405-4712\(17\)30438-6). *Cell Systems* **5**, 527 (2017).
* B.D. Fulcher, M.A. Little, N.S. Jones [Highly comparative time-series analysis: the empirical structure of time series and their methods](http://rsif.royalsocietypublishing.org/content/10/83/20130048.full). *J. Roy. Soc. Interface* **10**, 83 (2013).

The pipeline to reproduce to create the catchaMouse16 feature set is an adaptation of the general pipeline from C.H. Lubba, S.S. Sethi, P. Knaute, S.R. Schultz, B.D. Fulcher, N.S. Jones. [_catch22_: CAnonical Time-series CHaracteristics](https://doi.org/10.1007/s10618-019-00647-x). *Data Mining and Knowledge Discovery* (2019) for a specific fMRI mouse data set.


## [&#x1F4D2;&#x1F4D8;&#x1F4D7;___catchaMouse16_ documentation__](https://time-series-features.gitbook.io/time-series-analysis-tools/feature-subsets/catchamouse16)

There is [comprehensive documentation](https://time-series-features.gitbook.io/time-series-analysis-tools/feature-subsets/catchamouse16) for _catchaMouse16_, including:

- Installation instructions (across C, python, and Matlab)
- Information about the theory behind and behavior of each of the features,
- A list of publications that have used the _catchaMouse16_ feature set or specific time series pipeline
- And more :yum:

## Acknowledgement :+1:

If you use this software, please read and cite this open-access article:

- &#x1F4D7; PREPINT OF THE PAPER.

## Performance Summary

Summary of the performance of the _catch22_ feature set across 93 classification problems, and a comparison to the [_hctsa_ feature set](https://github.com/benfulcher/hctsa) (cf. Fig. 4 from [our paper][LINK TO BE ADDED]:

![](img/PerformanceComparisonFig3.png)


# Contribution

Please feel free to ask any query in the Issue section. Any suggestions or improvements are welcomed through Pull Requests.
