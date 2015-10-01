#include <ctime>
#include <iostream>
#include <cstdlib>

#include <mdb.h>

const std::string storage_path = "exampleStorage";

bool enable_dyn_cache = false;
size_t cache_size=mdb::defaultcacheSize;
size_t cache_pool_size=mdb::defaultcachePoolSize;


int main(int argc, char *argv[]) {
 
  mdb::Storage::Storage_ptr ds =  mdb::Storage::Create(storage_path);

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
  
  reader->readAll(&output);
  
  clock_t read_t1 = clock();

  std::cout<<"read time :" << ((float)read_t1 - read_t0) / CLOCKS_PER_SEC<<std::endl;

  ds->Close();
}
