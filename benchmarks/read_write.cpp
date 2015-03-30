#include <ctime>
#include <storage/storage.h>
#include <utils/ProcessLogger.h>

int main(int argc, char*argv[]) {
	const int meas2write = 10;
	const int write_iteration = 50;
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
			meas->value = i;
			meas->id = i%meas2write;
			meas->source = meas->flag = i%meas2write;
			meas->time = i;
			ds->append(meas);
			
		}
		clock_t write_t1 = clock();
		logger << "write time: " << ((float)write_t1 - write_t0) / CLOCKS_PER_SEC << endl;
		delete meas;
		ds = nullptr;
		auto pages = utils::ls(storage_path);
	}
	{
		storage::DataStorage::PDataStorage ds = storage::DataStorage::Open(storage_path);
		clock_t read_t0 = clock();
		for (int i = 1; i < meas2write*write_iteration; ++i) {
			auto meases = ds->readInterval(0, i);
		}
		clock_t read_t1 = clock();
		logger << "read time: " << ((float)read_t1 - read_t0) / CLOCKS_PER_SEC << endl;
	}
	utils::rm(storage_path);
}