#mdb - not key-value database
**mdb** - is a numeric time-series database.

# Features
---
* High write speed(1.5 - 2.5 millions values per second)
* Proprietary storage format 
* Implemented as C++ library.

# build dependencies
---
* Boost 1.57.0 or higher: system, filesystem, log, interprocess, unit_test_framework(to build tests), program_options (to build benchmarks)
* cmake
* c++ 11 compiler

## build on linux
---
```shell
$ cmake .
$ make
```
##build on windows with **Microsoft Visual Studio**
---
```cmd
$ cmake -G "Visual Studio 12 2013 Win64" .
$ cmake --build .
```

## build with non system installed boost
---
```shell
$ cmake . -DBOOST_ROOT="path/to/boost/" .
$ make
```