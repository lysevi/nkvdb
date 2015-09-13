#include <ctime>
#include <iostream>
#include <cstdlib>
#include <thread>
#include <atomic>

#include <storage/storage.h>
#include "storage/utils/logger.h"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

const std::string storage_path = "threadBenchmarkStorage";

int meas2write = 10;
int pagesize = 1000000;
int thread_count = 5;
int iteration_count = 1000000;
bool verbose = false;
bool dont_remove = false;
bool enable_dyn_cache = false;
size_t cache_size = storage::defaultcacheSize;
size_t cache_pool_size = storage::defaultcachePoolSize;
storage::Storage::Storage_ptr ds = nullptr;

std::atomic_long append_count{ 0 };
bool stop_info = false;

void makeStorage() {
	logger("makeStorage mc:" << meas2write << " dyn_cache: " << (enable_dyn_cache ? "true" : "false"));

	const uint64_t storage_size =
		sizeof(storage::Page::Header) + (sizeof(storage::Meas) * pagesize);

	ds = storage::Storage::Create(storage_path, storage_size);

	ds->enableCacheDynamicSize(enable_dyn_cache);
	ds->setPoolSize(cache_pool_size);
	ds->setCacheSize(cache_size);
}

void writer(int writeCount) {
    storage::Meas meas = storage::Meas::empty();

	for (int i = 0; i < writeCount; ++i) {
        meas.value = i % meas2write;
        meas.id = i % meas2write;
        meas.source = meas.flag = i % meas2write;
        meas.time = i;

        ds->append(meas);
		append_count++;
	}
}

void show_info() {
	clock_t t0 = clock();
	auto all_writes = thread_count*iteration_count;
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(300));

		clock_t t1 = clock();
		auto writes_per_sec = append_count.load() / double((t1 - t0) / CLOCKS_PER_SEC);
		std::cout << "\r>> " << writes_per_sec << "/sec progress:" << (100 * append_count) / all_writes << '%';
		std::cout.flush();
		if (stop_info) {
			break;
		}
	}
}

int main(int argc, char *argv[]) {
    po::options_description desc("Multithread IO benchmark.\n Allowed options");
	desc.add_options()("help", "produce help message")(
		"mc", po::value<int>(&meas2write)->default_value(meas2write), "measurment count")
		("dyncache", po::value<bool>(&enable_dyn_cache)->default_value(enable_dyn_cache), "enable dynamic cache")
		("cache-size", po::value<size_t>(&cache_size)->default_value(cache_size), "cache size")
		("cache-pool-size", po::value<size_t>(&cache_pool_size)->default_value(cache_pool_size), "cache pool size")
		("thread-count", po::value<int>(&thread_count)->default_value(thread_count), "write thread count")
		("page-size", po::value<int>(&pagesize)->default_value(pagesize), "page size")
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

	makeStorage();
	std::thread info_thred(show_info);

    logger("threads count: "<<thread_count);

	std::vector<std::thread> writers(thread_count);
	size_t pos = 0;
	for (int i = 0; i < thread_count; i++) {
		std::thread t{ writer, iteration_count };
		writers[pos++] = std::move(t);
	}

	pos = 0;
	for (int i = 0; i < thread_count; i++) {
		std::thread t = std::move(writers[pos++]);
		t.join();
	}

	stop_info = true;
	info_thred.join();

	ds->Close();
	if (!dont_remove) {
		utils::rm(storage_path);
	}
}
