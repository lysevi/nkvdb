Again. nkvdb is not key value database. You can`t get value by key, but you
can query data by interval or query data in time point.

for example, we have

![example 1](images/read_interval.png "readInterval example")

Horizontal lines are time-line for measurments with *id* 0-3. 

## read intervals

if you are querying in this way

```C++ 
auto reader=ds->readInterval(from,to);
nkvdb::Meas::MeasList output;
reader->readAll(&output);
```

The **output** will contain values, which marked as red circules in the above picture.

## read in time point
if you executing in this way

```C++
auto reader=ds->readInTimePoint(timepoint);
nkvdb::Meas::MeasList output;
reader->readAll(&output);
```

you will get values, which time less or equal of **from**.

![example 1](images/read_timepoint.png "readTimePoint example")
