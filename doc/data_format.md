# storage format
storage is a set of pages. each pages represent by 3 files:

![page](images/nkvdb_format.png) 

**.page** file contains measurements in order, which send to write. no sorting, no merging, no compressing.
All for fast write speed.

Before write to page, nkvdb store data to cache. cache just a array, and write speed to them its very fast.
When array is full (max size of array **nkvdb::defaultcacheSize**), 
data sends to async writer, 
and for new data nkvdb take new cache from cache pool.

## Async writer

nkvdb operate page file as mapped file. nkvdb just take full cache and just them to that map.
And here nkvdb write to .pagei index info for future read queries.

## When page os full.
When page is full, to .pagew file write last values for each **id**. thats do for reads data in time point and may save time for scanning pages to past.