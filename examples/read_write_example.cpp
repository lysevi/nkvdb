#include <ctime>
#include <iostream>
#include <cstdlib>

#include <storage/storage.h>
#include "storage/utils/logger.h"

const std::string storage_path = "exampleStorage";

size_t pagesize = 2000000;
bool enable_dyn_cache = false;
size_t cache_size=storage::defaultcacheSize;
size_t cache_pool_size=storage::defaultcachePoolSize;


int main(int argc, char *argv[]) {
 
 const uint64_t storage_size =
      sizeof(storage::Page::Header) + (sizeof(storage::Meas) * pagesize);

  storage::Storage::Storage_ptr ds =
      storage::Storage::Create(storage_path, storage_size);

  ds->enableCacheDynamicSize(enable_dyn_cache);
  ds->setPoolSize(cache_pool_size);
  ds->setCacheSize(cache_size);
  auto writes_count = 3000000;
  std::cout << "write " << writes_count << " values..." << std::endl;

  clock_t write_t0 = clock();
  storage::Meas meas = storage::Meas::empty();

  for (int i = 0; i < 3000000; ++i) {
    meas.value = i;
    meas.id = i % 10;
    meas.source = meas.flag = 0;
    meas.time = i;

    ds->append(meas);
  }

  clock_t write_t1 = clock();
  std::cout << "write time: " << ((float)write_t1 - write_t0) / CLOCKS_PER_SEC << std::endl;
  
  std::cout << "read all values..." << std::endl;

  clock_t read_t0 = clock();

  storage::Meas::MeasList output;
  auto reader = ds->readInterval(0, writes_count);
  
  // or meases->readAll(&output);
  while (!reader->isEnd()) {
	  reader->readNext(&output);
  }

  clock_t read_t1 = clock();

  std::cout<<"read time :" << ((float)read_t1 - read_t0) / CLOCKS_PER_SEC<<std::endl;

  ds->Close();
}
