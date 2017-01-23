# Matrix-matrix multiplication on DFE

## Description
A set of DFE algorithms for matrix-matrix multiplication written as part of master thesis.

There are four different DFE algorithms - Basic, Transposed, Tiled and PagingAlg with PagingAlg generaly being the fastest. First three have also pipelined variants using DFEVector construct ("Vec" at the end of the folder) and variants using LMem ("LMem" at the end of the folder).

For running time comparison there is also simple CPE version for matrix-matrix multiplication. 

Example tests are in the bench folder.


## Requirements
DFE algorithms are implemented for and need to be run on [Maxeler platform] (https://www.maxeler.com/solutions/).

Some implementations need to include [Maxeler standard library] (https://github.com/maxeler/maxpower).

## MIT Licence
Repository is open source under the MIT license. See [LICENSE](LICENSE) for details.


