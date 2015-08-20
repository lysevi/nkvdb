#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include "test_common.h"
#include <storage/Meas.h>
#include <storage/Page.h>
#include <storage/storage.h>
#include <utils/ProcessLogger.h>
#include <utils/utils.h>

#include <iterator>
#include <list>
using namespace storage;

BOOST_AUTO_TEST_CASE(MeasEmpty) {
    storage::Meas::PMeas pm = storage::Meas::empty();

    BOOST_CHECK_EQUAL(pm->value, storage::Value(0));
    BOOST_CHECK_EQUAL(pm->flag, storage::Flag(0));
    BOOST_CHECK_EQUAL(pm->id, storage::Id(0));
    BOOST_CHECK_EQUAL(pm->source, storage::Flag(0));
    BOOST_CHECK_EQUAL(pm->time, storage::Time(0));
    delete pm;
}


BOOST_AUTO_TEST_CASE(StorageCreateOpen){
	{
		{
			
			storage::DataStorage::PDataStorage ds = storage::DataStorage::Create(mdb_test::storage_path);
			BOOST_CHECK(boost::filesystem::exists(mdb_test::storage_path));
			BOOST_CHECK(boost::filesystem::is_directory(mdb_test::storage_path));

			std::list<boost::filesystem::path> pages = utils::ls(mdb_test::storage_path);
			BOOST_CHECK_EQUAL(pages.size(), 1);
			ds = nullptr;

			ds = storage::DataStorage::Create(mdb_test::storage_path);
			BOOST_CHECK(boost::filesystem::exists(mdb_test::storage_path));
			BOOST_CHECK(boost::filesystem::is_directory(mdb_test::storage_path));
			pages = utils::ls(mdb_test::storage_path);
			BOOST_CHECK_EQUAL(pages.size(), 1);
		}
		{
			storage::DataStorage::PDataStorage ds = storage::DataStorage::Open(mdb_test::storage_path);
		}
	}
    utils::rm(mdb_test::storage_path);
}

BOOST_AUTO_TEST_CASE(StorageIO){
    const int meas2write=10;
    const int write_iteration=10;
    const uint64_t storage_size=sizeof(storage::Page::Header)+(sizeof(storage::Meas)*meas2write);
	const std::string storage_path = mdb_test::storage_path + "storageIO";
	{
		storage::DataStorage::PDataStorage ds = storage::DataStorage::Create(storage_path, storage_size);

		storage::Meas::PMeas meas = storage::Meas::empty();
        storage::Time end_it= (meas2write*write_iteration);
        for (storage::Time i = 0; i < end_it; ++i) {
			if (end_it - i != 0) {
				auto meases = ds->readInterval(i, end_it - i);
				BOOST_CHECK(meases.size() == 0);
			}

			meas->value = i;
			meas->id = i;
			meas->source = meas->flag = i%meas2write;
			meas->time = i;
			ds->append(meas);

			
			auto meases = ds->readInterval(0, end_it);

			for (storage::Time j = 0; j < i; ++j) {
				bool isExists = false;
				for (storage::Meas m : meases) {
					if (m.id == j) {
						isExists = true;
						break;
					}
				}
				if (!isExists) {
					int a = 3;
				}
				BOOST_CHECK(isExists);
				
			}
		}
		delete meas;
		ds = nullptr;
		auto pages = utils::ls(storage_path);
		BOOST_CHECK_EQUAL(pages.size(), write_iteration);
	}
	{
		storage::DataStorage::PDataStorage ds = storage::DataStorage::Open(storage_path);
		
		for (int i = 1; i < meas2write*write_iteration; i += (meas2write*write_iteration) / 100) {
			clock_t verb_t0 = clock();
			storage::Time to = i*((meas2write*write_iteration) / 100);
			auto meases = ds->readInterval(0, to);

			BOOST_CHECK_EQUAL(meases.size(), to + 1);

			for (storage::Meas m : meases) {
				BOOST_CHECK(utils::inInterval<storage::Time>(0, to, m.time));

				BOOST_CHECK(m.time<=to);
			}
		}
	}
    utils::rm(storage_path);
}

BOOST_AUTO_TEST_CASE(StorageIOArrays) {
	const int meas2write = 10;
	const int write_iteration = 10;
	const uint64_t storage_size = sizeof(storage::Page::Header) + (sizeof(storage::Meas)*meas2write);
	const std::string storage_path = mdb_test::storage_path + "storageIO";

	{
		storage::DataStorage::PDataStorage ds = storage::DataStorage::Create(storage_path, storage_size);

		size_t arr_size = meas2write * write_iteration;
		storage::Meas::PMeas array = new storage::Meas[arr_size];
		for (int i = 0; i < arr_size; ++i) {
			array[i].id = i;
			array[i].time = i;
		}
		ds->append(array, arr_size);
		delete[] array;
		
		auto interval = ds->readInterval(0, arr_size);
		BOOST_CHECK_EQUAL(interval.size(), arr_size);
		for (auto m : interval) {
			BOOST_CHECK(utils::inInterval<storage::Time>(0, arr_size, m.time));
		}

		for (int i = 0; i < arr_size; ++i) {
			bool isExists = false;
			for (auto m : interval) {
				if (m.id == i) {
					isExists = true;
					break;
				}
			}
			BOOST_CHECK(isExists);
		}
		ds->Close();

		auto pages = utils::ls(storage_path);
		BOOST_CHECK_EQUAL(pages.size(), write_iteration);
	}
	utils::rm(storage_path);
}