[![Build Status](https://travis-ci.org/lysevi/mdb.svg?branch=master)](https://travis-ci.org/lysevi/mdb)

#mdb - not key-value database
**mdb** - is a numeric time-series database.

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
#include <mdb.h>

int main(int argc, char *argv[]) {
	auto writes_count = 3000000;
	{
		auto ds = mdb::Storage::Create("path/to/storage");

		mdb::Meas meas = mdb::Meas::empty();

		for (int i = 0; i < 3000000; ++i) {
			meas.value = i;
			meas.id = i % 10;
			meas.source = meas.flag = 0;
			meas.time = i;

			ds->append(meas);
		}
		ds->Close();
	}
	{
		auto ds = mdb::Storage::Open("path/to/storage");
		//reading
		mdb::Meas::MeasList output;
		auto reader = ds->readInterval(0, writes_count);

		// or just call reader->readAll(&output);
		while (!reader->isEnd()) {
			reader->readNext(&output);
		}
		ds->Close();
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
$ cmake . -DBOOST_ROOT="path/to/boost/" .
$ make
```
