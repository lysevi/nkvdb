#include <ctime>
#include <iostream>
#include <cstdlib>

#include <storage.h>
#include <logger.h>

const std::string storage_path = "exampleStorage";

size_t pagesize = 2000000;
bool enable_dyn_cache = false;
size_t cache_size=mdb::defaultcacheSize;
size_t cache_pool_size=mdb::defaultcachePoolSize;


int main(int argc, char *argv[]) {
 
 const uint64_t storage_size =
      sizeof(mdb::Page::Header) + (sizeof(mdb::Meas) * pagesize);

  mdb::Storage::Storage_ptr ds =
      mdb::Storage::Create(storage_path, storage_size);

  ds->enableCacheDynamicSize(enable_dyn_cache);
  ds->setPoolSize(cache_pool_size);
  ds->setCacheSize(cache_size);
  auto writes_count = 3000000;
  std::cout << "write " << writes_count << " values..." << std::endl;

  clock_t write_t0 = clock();
  mdb::Meas meas = mdb::Meas::empty();

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

  mdb::Meas::MeasList output;
  auto reader = ds->readInterval(0, writes_count);
  
  // or meases->readAll(&output);
  while (!reader->isEnd()) {
	  reader->readNext(&output);
  }

  clock_t read_t1 = clock();

  std::cout<<"read time :" << ((float)read_t1 - read_t0) / CLOCKS_PER_SEC<<std::endl;

  ds->Close();
}
