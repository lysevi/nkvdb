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
int read_step = 100;
int pagesize = 1000000;
bool write_only = false;
bool verbose = false;
bool dont_remove = false;

void makeAndWrite(int mc, int ic) {
	logger << "makeAndWrite mc:" << mc << " ic:" << ic << endl;

	const uint64_t storage_size = sizeof(storage::Page::Header) + (sizeof(storage::Meas)*pagesize);

	storage::DataStorage::PDataStorage ds = storage::DataStorage::Create(storage_path, storage_size);
	clock_t write_t0 = clock();
	storage::Meas::PMeas meas = storage::Meas::empty();

	for (int i = 0; i < ic; ++i) {
		meas->value = i%mc;
		meas->id = i%mc;
		meas->source = meas->flag = i%mc;
		meas->time = i;

		ds->append(meas);
	}

	clock_t write_t1 = clock();
	logger << "write time: " << ((float)write_t1 - write_t0) / CLOCKS_PER_SEC << endl;
	delete meas;
	ds = nullptr;
	utils::rm(storage_path);
}

int main(int argc, char*argv[]) {

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("mc", po::value<int>(&meas2write)->default_value(meas2write), "measurment count")
		("ic", po::value<int>(&write_iteration)->default_value(write_iteration), "iteration count")
		("read-step", po::value<int>(&read_step)->default_value(read_step), "read step on read benchmark")
		("pSize", po::value<int>(&pagesize)->default_value(pagesize), "meas count in page")
		("write-only", "don`t run readInterval")
		("verbose", "verbose ouput")
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

	makeAndWrite(meas2write, 1000000);

	makeAndWrite(meas2write, 2000000);
	makeAndWrite(meas2write, 3000000);
	if (!write_only) {
		{
			storage::DataStorage::PDataStorage ds = storage::DataStorage::Create(storage_path, storage_size);
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
			delete meas;
			ds = nullptr;
			auto pages = utils::ls(storage_path);
		}


		{
			storage::DataStorage::PDataStorage ds = storage::DataStorage::Open(storage_path);
			clock_t read_t0 = clock();
			for (int i = 0; i < meas2write*write_iteration; i += read_step) {
				clock_t verb_t0 = clock();
				storage::Time from = i;
				storage::Time to = i + read_step;
				auto meases = ds->readInterval(from, to);

				clock_t verb_t1 = clock();
				if (verbose) {
					logger << "read[" << from << ":" << to << "]: " << ((float)verb_t1 - verb_t0) / CLOCKS_PER_SEC << " cnt:" << meases.size() << endl;
				}
			}
			clock_t read_t1 = clock();
			logger << "read time: " << ((float)read_t1 - read_t0) / CLOCKS_PER_SEC << endl;
		}
		if (!dont_remove)
			utils::rm(storage_path);
	}
}
