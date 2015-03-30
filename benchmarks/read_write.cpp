#include <ctime>
#include <iostream>
#include <storage/storage.h>
#include <utils/ProcessLogger.h>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int meas2write = 10;
int write_iteration = 50;
bool write_only = false;
bool verbose = false;

int main(int argc, char*argv[]) {
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("mc", po::value<int>(), "measurment count")
		("ic", po::value<int>(), "iteration count")
		("write-only",  "don`t run readInterval")
		("verbose", "verbose ouput")
		;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 1;
	}
	
	if (vm.count("write-only")) {
		write_only = true;
	}

	if (vm.count("verbose")) {
		verbose = true;
	}

	if (vm.count("mc"))
		meas2write = vm["mc"].as<int>();
	if (vm.count("ic"))
		write_iteration = vm["ic"].as<int>();
	
	const uint64_t storage_size = sizeof(storage::Page::Header) + (sizeof(storage::Meas)*meas2write);
	const std::string storage_path = "benchmarkStorage";
	logger << "storage page size: " << storage_size << endl
		<< "meas count: " << meas2write << endl
		<< "write iterations: " << write_iteration << endl;

	{
		storage::DataStorage::PDataStorage ds = storage::DataStorage::Create(storage_path, storage_size);
		clock_t write_t0=clock();
		storage::Meas::PMeas meas = storage::Meas::empty();
		for (int i = 0; i < (meas2write*write_iteration); ++i) {
			clock_t verb_t0 = clock();
			
			meas->value = i;
			meas->id = i%meas2write;
			meas->source = meas->flag = i%meas2write;
			meas->time = i;
			
			ds->append(meas);
			clock_t verb_t1 = clock();
			if (verbose) {
				logger << "write[" << i << "]: " << ((float)verb_t1 - verb_t0) / CLOCKS_PER_SEC << endl;
			}
			
		}
		clock_t write_t1 = clock();
		logger << "write time: " << ((float)write_t1 - write_t0) / CLOCKS_PER_SEC << endl;
		delete meas;
		ds = nullptr;
		auto pages = utils::ls(storage_path);
	}
	if (!write_only){
		storage::DataStorage::PDataStorage ds = storage::DataStorage::Open(storage_path);
		clock_t read_t0 = clock();
		for (int i = 1; i < meas2write*write_iteration; ++i) {
			clock_t verb_t0 = clock();
			
			auto meases = ds->readInterval(0, i);

			clock_t verb_t1 = clock();
			if (verbose) {
				logger << "read[" << i << "]: " << ((float)verb_t1 - verb_t0) / CLOCKS_PER_SEC << endl;
			}
		}
		clock_t read_t1 = clock();
		logger << "read time: " << ((float)read_t1 - read_t0) / CLOCKS_PER_SEC << endl;
	}
	utils::rm(storage_path);
}