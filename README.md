#This project is abandoned. Use [dariadb](https://github.com/lysevi/dariadb)

[![Build Status](https://travis-ci.org/lysevi/nkvdb.svg?branch=master)](https://travis-ci.org/lysevi/nkvdb)
[![Join the chat at https://gitter.im/lysevi/nkvdb](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/lysevi/nkvdb?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

#nkvdb - not key-value database
**nkvdb** - is a numeric time-series database.

# Features
* High write speed(1.5 - 2.5 millions values per second)
* Proprietary storage format 
* Implemented as C++ library.

# Dependencies
* Boost 1.53.0 or higher: system, filesystem, interprocess, unit_test_framework(to build tests), program_options (to build benchmarks)
* cmake
* c++ 11 compiler

# Example
## Open, read, write
```C++
#include <nkvdb.h>

int main(int argc, char *argv[]) {
    auto writes_count = 3000000;
    {
        auto ds = nkvdb::Storage::Create("path/to/storage");

        nkvdb::Meas meas = nkvdb::Meas::empty();

        for (int i = 0; i < 3000000; ++i) {
            meas.setValue(i);
            meas.id = i % 10;
            meas.source = meas.flag = 0;
            meas.time = i;

            ds->append(meas);
        }
    }
    {
        auto ds = nkvdb::Storage::Open("path/to/storage");
        //reading
        nkvdb::Meas::MeasList output;
        auto reader = ds->readInterval(0, writes_count);

        // or just call reader->readAll(&output);
        while (!reader->isEnd()) {
            reader->readNext(&output);
        }
    }
}
```

# Build
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
$ cmake  -DBOOST_ROOT="path/to/boost/" .
$ make
```
