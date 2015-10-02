#include <ctime>
#include <iostream>
#include <cstdlib>

#include <nkvdb.h>

const std::string storage_path = "exampleStorage";

bool enable_dyn_cache = false;
size_t cache_size=nkvdb::defaultcacheSize;
size_t cache_pool_size=nkvdb::defaultcachePoolSize;


int main(int argc, char *argv[]) {
 
  nkvdb::Storage::Storage_ptr ds =  nkvdb::Storage::Create(storage_path);

  ds->enableCacheDynamicSize(enable_dyn_cache);
  ds->setPoolSize(cache_pool_size);
  ds->setCacheSize(cache_size);
  auto writes_count = 3000000;
  std::cout << "write " << writes_count << " values..." << std::endl;

  clock_t write_t0 = clock();
  nkvdb::Meas meas = nkvdb::Meas::empty();

  auto start_time=time(0);
  for (int i = 0; i < 3000000; ++i) {
    meas.value = i;
    meas.id = i % 10;
    meas.source = meas.flag = 0;
    meas.time = time(0);

    ds->append(meas);
  }

  clock_t write_t1 = clock();
  std::cout << "write time: " << ((float)write_t1 - write_t0) / CLOCKS_PER_SEC << std::endl;
  
  std::cout << "read all values..." << std::endl;

  clock_t read_t0 = clock();

  nkvdb::Meas::MeasList output;
  auto reader = ds->readInterval(start_time, time(0));
  
  reader->readAll(&output);
  clock_t read_t1 = clock();

  std::cout<<"read time: " << ((float)read_t1 - read_t0) / CLOCKS_PER_SEC<<std::endl;

  ds->Close();
}
