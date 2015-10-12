#nkvdb is a not key value database.

specific read model can found [here](read_model.md)
- [Measurment](#measurment)
- [Creating storage](#creating-storage)
- [Openning storage](#openning-storage)
  - [Options](#storage-options)
- [Writing data](#writing-data)
- [Reading](#reading)

## Measurment
Is a struct:
```C++
struc Meas
  Id id;
  Time time;
  Flag source;
  Flag flag;
  Value value;
};
```

## Creating storage 
```C++
    auto ds =  nkvdb::Storage::Create(storage_path);
```

ds - is shared ptr, and if you forget call ds->Close(), it will be called automaticaly.
**if storage_path already exists, ngvdb will remove storage_path and create new.**

## Openning storage
```C++
    auto ds = nkvdb::Storage::Open(storage_path);
```
### storage options
#### cache size;
The default cache size is nkvdb::defaultcacheSize. But you can change it by this code.

```C++
    ds->setCacheSize(cache_size);
```
#### cache pool
nkvdb uses cache pools for fast writing speed. The default cache pools size is nkvdb::defaultcachePoolSize). To change its size use method **setPoolSize**.
```C++
    ds->setPoolSize(cache_pool_size); 
```

## Writing data
### Write one measurment
```C++
    nkvdb::Meas meas = nkvdb::Meas::empty();
    meas.id=1;
    meas.time=1;
    ds->append(meas);
```
### Write arrays
```C++
    nkvdb::Meas::PMeas array = new nkvdb::Meas[arr_size];
    for (size_t i = 0; i < arr_size; ++i) {
      array[i].id = i;
      array[i].time = i;
    }
    ds->append(array, arr_size);
    delete[] array;
```

## Reading
### Read time interval
```C++
    auto reader=ds->readInterval(3,5);
    nkvdb::Meas::MeasList output{};
    reader->readAll(&output);
```
### Read time with filteration.
This query return measurments with **id**={1,2,3}, **source**=777, **flag**=111, 
and with **time** from 10 to 30
```C++
    auto queryIds = IdArray{ 1, 2, 3 };

    nkvdb::Time from=10, to 30;
    nkvdb::Flag source=777;
    nkvdb::Flag flag=111;
    auto reader = ds->readInterval(queryIds,source,flag,from, to);
    nkvdb::Meas::MeasList output{};
    reader->readAll(&output);
```
### Read value in time point
Return all unique values (unique by id) from an inteval (тут надо дописать какой интервал, так должно быть понятнее) with time less than tpoint.
```C++
nkvdb::Time tpoint=...
auto reader=ds->readInTimePoint(tpoint);
nkvdb::Meas::MeasList output{};
reader->readAll(&output);
```
