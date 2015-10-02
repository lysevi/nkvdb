# storage format
storage is a set of pages. each pages represent by 3 files:

![page](images/nkvdb_format.png) 

**.page** file contains measurements in order, which send to write. no sorting, no merging, no compressing.
All for fast write speed.

Before write to page, nkvdb store data to cache. Cache is just an array and write speed to it is very fast.
When array is full (max size of array **nkvdb::defaultcacheSize**), the new portion of data will send to async writer, 
and for new data nkvdb will take new cache from cache pool.

## Async writer

nkvdb operate page file as mapped file. nkvdb just take full cache and just write it to the mapped file.
And here nkvdb write to .pagei index info for future read queries.

## When page os full.
When page is full, to .pagew file write last values for each **id**. thats do for reads data in time point and may save time for scanning pages to past.
