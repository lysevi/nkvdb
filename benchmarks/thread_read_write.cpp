#include <ctime>
#include <iostream>
#include <cstdlib>
#include <thread>

#include <storage/storage.h>
#include <utils/logger.h>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

const std::string storage_path = "threadBenchmarkStorage";

int meas2write = 10;
int pagesize = 1000000;
int thread_count = 10;
bool verbose = false;
bool dont_remove = false;
bool enable_dyn_cache = false;
size_t cache_size=storage::defaultcacheSize;
size_t cache_pool_size=storage::defaultcachePoolSize;
storage::DataStorage::PDataStorage ds = nullptr;

void makeStorage() {
   logger("makeStorage mc:" << meas2write << " dyn_cache: " << (enable_dyn_cache ? "true" : "false"));

  const uint64_t storage_size =
      sizeof(storage::Page::Header) + (sizeof(storage::Meas) * pagesize);

  ds = storage::DataStorage::Create(storage_path, storage_size);

  ds->enableCacheDynamicSize(enable_dyn_cache);
  ds->setPoolSize(cache_pool_size);
  ds->setCacheSize(cache_size);
  
  
  
}

void writer(int writeCount) {
	storage::Meas::PMeas meas = storage::Meas::empty();

	for (int i = 0; i < writeCount; ++i) {
		meas->value = i % meas2write;
		meas->id = i % meas2write;
		meas->source = meas->flag = i % meas2write;
		meas->time = i;

		ds->append(*meas);
	}
	delete meas;
}


int main(int argc, char *argv[]) {
  po::options_description desc("Allowed options");
  desc.add_options()("help", "produce help message")(
      "mc", po::value<int>(&meas2write)->default_value(meas2write), "measurment count")
      ("dyncache",po::value<bool>(&enable_dyn_cache)->default_value(enable_dyn_cache),"enable dynamic cache")
      ("cache-size",po::value<size_t>(&cache_size)->default_value(cache_size),"cache size")
      ("cache-pool-size",po::value<size_t>(&cache_pool_size)->default_value(cache_pool_size),"cache pool size")
	  ("thread-count", po::value<int>(&thread_count)->default_value(thread_count), "write thread count")
      ("page-size",po::value<int>(&pagesize)->default_value(pagesize),"page size")
      ("verbose", "verbose ouput")
      ("dont-remove", "dont remove created storage");

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
  } catch (std::exception &ex) {
    logger("Error: " << ex.what());
    exit(1);
  }
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }

  if (vm.count("verbose")) {
    verbose = true;
  }

  if (vm.count("dont-remove")) {
    dont_remove = true;
  }

  /*std::list<std::thread> writers;
  for (int i = 0; i < thread_count; i++) {
	  std::thread t{ writer, 1000 };
	  writers.push_back(t);
  }

  for (auto cur_t:writers) {
	  cur_t.join();
  }*/
  makeStorage();
  if (!dont_remove) {
	  utils::rm(storage_path);
  }
}
