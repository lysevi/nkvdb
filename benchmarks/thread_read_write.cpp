#include <ctime>
#include <iostream>
#include <cstdlib>
#include <thread>
#include <atomic>

#include <storage.h>
#include <logger.h>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

const std::string storage_path = "threadBenchmarkStorage";

size_t meas2write = 10;
size_t pagesize = 1000000;
size_t thread_count = 5;
size_t iteration_count = 1000000;
size_t read_iteration_count=10;
bool verbose = false;
bool dont_remove = false;
bool enable_dyn_cache = false;
size_t cache_size = nkvdb::defaultcacheSize;
size_t cache_pool_size = nkvdb::defaultcachePoolSize;
nkvdb::Storage::Storage_ptr ds = nullptr;

std::atomic_long append_count{ 0 };
std::atomic_long reads_count{ 0 };
bool stop_info = false;

void makeStorage() {
	logger("makeStorage mc:" << meas2write << " dyn_cache: " << (enable_dyn_cache ? "true" : "false"));

    const uint64_t storage_size = nkvdb::Page::calc_size(pagesize);

    ds = nkvdb::Storage::Create(storage_path, storage_size);

	ds->enableCacheDynamicSize(enable_dyn_cache);
	ds->setPoolSize(cache_pool_size);
	ds->setCacheSize(cache_size);
}

void writer(int writeCount) {
    nkvdb::Meas meas = nkvdb::Meas::empty();

	for (int i = 0; i < writeCount; ++i) {
        meas.setValue(i);
        meas.id = i % meas2write;
        meas.source = meas.flag = i % meas2write;
        meas.time = i;

        ds->append(meas);
		append_count++;
	}
}

void reader(int num,nkvdb::Time from, nkvdb::Time to) {
    auto read_window = (to - from) / read_iteration_count;

	for (size_t i = 0; i < read_iteration_count; i++) {
		auto reader = ds->readInterval(read_window*(i+1), read_window*(i+2));
        nkvdb::Meas::MeasList output;
		reader->readAll(&output);
		reads_count++;
	}
}

void show_info() {
	clock_t t0 = clock();
	auto all_writes = thread_count*iteration_count;
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(300));

		clock_t t1 = clock();
		auto writes_per_sec = append_count.load() / double((t1 - t0) / CLOCKS_PER_SEC);
		std::cout << "\rwrites: " << writes_per_sec << "/sec progress:" << (100 * append_count) / all_writes << "%             ";
		std::cout.flush();
		if (stop_info) {
			break;
		}
	}
	std::cout << "\n";
}


void show_reads_info() {
	clock_t t0 = clock();
	float all_reads = thread_count*read_iteration_count;
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(300));

		clock_t t1 = clock();
		auto reads_per_sec = reads_count.load() / double((t1 - t0) / CLOCKS_PER_SEC);
		std::cout << "\rreads: " << reads_per_sec << "/sec progress:" << (100 * reads_count) / all_reads << "%             ";
		std::cout.flush();
		if (stop_info) {
			break;
		}
	}
}

int main(int argc, char *argv[]) {
    po::options_description desc("Multithread IO benchmark.\n Allowed options");
	desc.add_options()("help", "produce help message")(
        "mc", po::value<size_t>(&meas2write)->default_value(meas2write), "measurment count")
		("dyncache", po::value<bool>(&enable_dyn_cache)->default_value(enable_dyn_cache), "enable dynamic cache")
		("cache-size", po::value<size_t>(&cache_size)->default_value(cache_size), "cache size")
		("cache-pool-size", po::value<size_t>(&cache_pool_size)->default_value(cache_pool_size), "cache pool size")
        ("thread-count", po::value<size_t>(&thread_count)->default_value(thread_count), "write thread count")
        ("page-size", po::value<size_t>(&pagesize)->default_value(pagesize), "page size")
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
	std::thread info_thread(show_info);

    logger("threads count: "<<thread_count);
	/// writers
	std::vector<std::thread> writers(thread_count);
	size_t pos = 0;
    for (size_t i = 0; i < thread_count; i++) {
		std::thread t{ writer, iteration_count };
		writers[pos++] = std::move(t);
	}

	pos = 0;
    for (size_t i = 0; i < thread_count; i++) {
		std::thread t = std::move(writers[pos++]);
		t.join();
	}

	stop_info = true;
	info_thread.join();

	/// readers
	stop_info = false;
	std::thread read_info_thread(show_reads_info);
	std::vector<std::thread> readers(thread_count);
	pos = 0;
	auto reads_per_thread = (iteration_count / ((float)thread_count));
    for (size_t i = 0; i < thread_count; i++) {
		std::thread t{ reader,i, reads_per_thread*i, reads_per_thread*(i+1) };
		readers[pos++] = std::move(t);
	}

	pos = 0;
    for (size_t i = 0; i < thread_count; i++) {
		std::thread t = std::move(readers[pos++]);
		t.join();
	}
	stop_info = true;
	read_info_thread.join();

    ds=nullptr;
	if (!dont_remove) {
        nkvdb::utils::rm(storage_path);
	}
}
