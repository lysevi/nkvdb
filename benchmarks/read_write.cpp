#include <ctime>
#include <iostream>
#include <cstdlib>
#include <storage/storage.h>
#include <utils/ProcessLogger.h>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

const std::string storage_path = "benchmarkStorage";

int meas2write = 10;
int write_iteration = 50;
bool write_only = false;
bool read_all = false;
bool verbose = false;
bool random_read=false;
int  random_read_step= 10;
bool dont_remove = false;

int main(int argc, char*argv[]) {

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("mc", po::value<int>(&meas2write)->default_value(meas2write), "measurment count")
		("ic", po::value<int>(&write_iteration)->default_value(write_iteration), "iteration count")
		("write-only",  "don`t run readInterval")
		("read-all", "bench with read from 0 to mc*ic")
		("verbose", "verbose ouput")
		("rand-read", "read random intervals")
		("rand-read-step", po::value<int>(&random_read_step)->default_value(random_read_step), "step count of random reads")
		("dont-remove", "dont remove created storage")
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

	if (vm.count("read-all")) {
		read_all = true;
	}

	if (vm.count("rand-read")) {
		random_read= true;
	}

	if (vm.count("verbose")) {
		verbose = true;
	}

	if (vm.count("dont-remove")) {
		dont_remove = true;
	}

	const uint64_t storage_size = sizeof(storage::Page::Header) + (sizeof(storage::Meas)*meas2write);
	
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
	if (read_all)
	{
		storage::DataStorage::PDataStorage ds = storage::DataStorage::Open(storage_path);
		clock_t read_t0 = clock();
		auto meases = ds->readInterval(0, meas2write*write_iteration);
		clock_t read_t1 = clock();
		logger << "read all[" << 0 << ":" << meas2write*write_iteration << "]: " << ((float)read_t1 - read_t0) / CLOCKS_PER_SEC << " cnt:" << meases.size() << endl;
		
	}
	if (random_read)
	{
		storage::DataStorage::PDataStorage ds = storage::DataStorage::Open(storage_path);
		auto step_size = (write_iteration*meas2write) / random_read_step;
		storage::Time from = 0;
		for (int st = 0; st < random_read_step; ++st) {
			storage::Time to = step_size*st; +step_size;
			clock_t read_t0 = clock();
			auto meases = ds->readInterval(from, to);
			clock_t read_t1 = clock();
			logger << "random read [" << from << ":" << to << "]: " << ((float)read_t1 - read_t0) / CLOCKS_PER_SEC << " cnt:" << meases.size() << endl;
		}
		
	}
	if (!write_only){
		storage::DataStorage::PDataStorage ds = storage::DataStorage::Open(storage_path);
		clock_t read_t0 = clock();
		for (int i = 1; i < meas2write*write_iteration; i += (meas2write*write_iteration)/100) {
			clock_t verb_t0 = clock();
			storage::Time to = i*((meas2write*write_iteration)/100);
			auto meases = ds->readInterval(0, to);

			clock_t verb_t1 = clock();
			if (verbose) {
				logger << "read["<<0<<":"<< to << "]: " << ((float)verb_t1 - verb_t0) / CLOCKS_PER_SEC <<" cnt:"<<meases.size()<< endl;
			}
		}
		clock_t read_t1 = clock();
		logger << "read time: " << ((float)read_t1 - read_t0) / CLOCKS_PER_SEC << endl;
	}
	if (!dont_remove)
		utils::rm(storage_path);
}