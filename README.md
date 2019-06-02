# Template engine for TKOM
## Dependencies
* cmake (>=3.7)
* make
* boost_test

## Building
In the main directory:
```
cmake .
```
Then to build project:
```
make tgen
```
Or to build tests:
```
make tkom_tests
```
## Usage
```
./tgen INFILE [JSONFILE]
```
Where JSONFILE is optional file with json data, and INFILE is input file. Output is returned as STDOUT, so to generate output file use:
```
./tgen INFILE [JSONFILE] > OUTFILE
```
Where OUTFILE is filename of output file.
