#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include "test_common.h"
#include <storage/page.h>
#include <storage/storage.h>
#include <storage/config.h>
#include <storage/time.h>
#include <utils/logger.h>
#include <utils/utils.h>

#include <iterator>
#include <list>
#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>

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

BOOST_AUTO_TEST_CASE(StorageCreateOpen) {
  {
    {
      storage::DataStorage::PDataStorage ds =
          storage::DataStorage::Create(mdb_test::storage_path);
      BOOST_CHECK(boost::filesystem::exists(mdb_test::storage_path));
      BOOST_CHECK(boost::filesystem::is_directory(mdb_test::storage_path));

      std::list<boost::filesystem::path> pages =
          utils::ls(mdb_test::storage_path);
      BOOST_CHECK_EQUAL(pages.size(), (size_t)1);
      ds = nullptr;

      ds = storage::DataStorage::Create(mdb_test::storage_path);
      BOOST_CHECK(boost::filesystem::exists(mdb_test::storage_path));
      BOOST_CHECK(boost::filesystem::is_directory(mdb_test::storage_path));
      pages = utils::ls(mdb_test::storage_path);
      BOOST_CHECK_EQUAL(pages.size(), (size_t)1);
    }
    {
      storage::DataStorage::PDataStorage ds =
          storage::DataStorage::Open(mdb_test::storage_path);
    }
  }
  utils::rm(mdb_test::storage_path);
}

BOOST_AUTO_TEST_CASE(StorageIO) {
  const int meas2write = 10;
  const int write_iteration = 10;
  const uint64_t storage_size =
      sizeof(storage::Page::Header) + (sizeof(storage::Meas) * meas2write);
  const std::string storage_path = mdb_test::storage_path + "storageIO";
  {
    storage::DataStorage::PDataStorage ds =
        storage::DataStorage::Create(storage_path, storage_size);

    storage::Meas::PMeas meas = storage::Meas::empty();
    storage::Time end_it = (meas2write * write_iteration);
    for (storage::Time i = 0; i < end_it; ++i) {
      /*if (end_it - i != 0) {
              auto meases = ds->readInterval(i, end_it - i);
              BOOST_CHECK(meases.size() == 0);
      }*/

      meas->value = i;
      meas->id = i;
      meas->source = meas->flag = i % meas2write;
      meas->time = i;
      ds->append(*meas);

      auto meases = ds->readInterval(0, end_it);

      for (storage::Time j = 0; j < i; ++j) {
        bool isExists = false;
        for (storage::Meas m : meases) {
          if (m.id == static_cast<storage::Id>(j)) {
            isExists = true;
            break;
          }
        }
        BOOST_CHECK(isExists);
      }
    }
    delete meas;
    ds = nullptr;
    auto pages = utils::ls(storage_path);
    BOOST_CHECK_EQUAL(pages.size(), (size_t)(write_iteration * 2));
  }
  {
    storage::DataStorage::PDataStorage ds =
        storage::DataStorage::Open(storage_path);

    for (int i = 1; i < meas2write * write_iteration;i += (meas2write * write_iteration) / 100) {
      storage::Time to = i * ((meas2write * write_iteration) / 100);
      auto meases = ds->readInterval(0, to);

      BOOST_CHECK_EQUAL(meases.size(), (size_t)(to + 1));

      for (storage::Meas m : meases) {
        BOOST_CHECK(utils::inInterval<storage::Time>(0, to, m.time));

        BOOST_CHECK(m.time <= to);
      }
    }
  }
  utils::rm(storage_path);
}

BOOST_AUTO_TEST_CASE(StorageIOArrays) {
  const int meas2write = 10;
  const size_t write_iteration = 10;
  const uint64_t storage_size =
      sizeof(storage::Page::Header) + (sizeof(storage::Meas) * meas2write);
  const std::string storage_path = mdb_test::storage_path + "storageIO";

  {
    storage::DataStorage::PDataStorage ds =
        storage::DataStorage::Create(storage_path, storage_size);

    size_t arr_size = meas2write * write_iteration;
    storage::Meas::PMeas array = new storage::Meas[arr_size];
    for (size_t i = 0; i < arr_size; ++i) {
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

    for (size_t i = 0; i < arr_size; ++i) {
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
    BOOST_CHECK_EQUAL(pages.size(), write_iteration * 2);
  }
  utils::rm(storage_path);
}

BOOST_AUTO_TEST_CASE(StorageIORealTime) {
  const int meas2write = 10;
  const int write_iteration = 10;
  const uint64_t storage_size =
      sizeof(storage::Page::Header) + (sizeof(storage::Meas) * meas2write);
  const std::string storage_path = mdb_test::storage_path + "storageIO";

  {
    storage::DataStorage::PDataStorage ds =
        storage::DataStorage::Create(storage_path, storage_size);

    ds->setPastTime(storage::TimeWork::fromDuration(std::chrono::seconds(2)));

    size_t arr_size = meas2write * write_iteration;

    storage::Meas::PMeas array = new storage::Meas[arr_size];

    auto cur_utc_time=storage::TimeWork::CurrentUtcTime();
    for (size_t i = 0; i < arr_size; ++i) {

      array[i].id = i;
      array[i].time = cur_utc_time;
    }
    ds->append(array, arr_size);

    auto interval = ds->readInterval(array[0].time, array[arr_size - 1].time);
    BOOST_CHECK_EQUAL(interval.size(), arr_size);
    for (auto m : interval) {
      BOOST_CHECK(utils::inInterval<storage::Time>(
          array[0].time, array[arr_size - 1].time, m.time));
    }

    for (size_t i = 0; i < arr_size; ++i) {
      bool isExists = false;
      for (auto m : interval) {
        if (m.id == i) {
          isExists = true;
          break;
        }
      }
      BOOST_CHECK(isExists);
    }

    ds->setPastTime(1);

    auto wrt_res = ds->append(array, arr_size);
    BOOST_CHECK_EQUAL(wrt_res.writed, wrt_res.ignored);

    delete[] array;
    ds->Close();
  }
  utils::rm(storage_path);
}


BOOST_AUTO_TEST_CASE(StorageCurvalues) {
	auto test_storage = [](IdArray query, std::map<storage::Id, storage::Meas> id2meas, storage::DataStorage::PDataStorage ds){
		auto curValues = ds->curValues(query);
		BOOST_CHECK_EQUAL(curValues.size(), id2meas.size());
		for (auto v : curValues) {
			if (id2meas.find(v.id) == id2meas.end()) {
				BOOST_ERROR("id2meas.find(v.id) == id2meas.end()");
			} else {
				id2meas.erase(v.id);
			}
		}
		BOOST_CHECK(id2meas.size() == 0);
	};

	const int meas2write = 10;
	const size_t write_iteration = 10;
	const uint64_t storage_size =
		sizeof(storage::Page::Header) + (sizeof(storage::Meas) * meas2write);
	const std::string storage_path = mdb_test::storage_path + "storageIO";
	
	std::map<storage::Id, storage::Meas> id2meas;
	IdArray query{};
	{
		{
			storage::DataStorage::PDataStorage ds =	storage::DataStorage::Create(storage_path, storage_size);

			size_t arr_size = meas2write * write_iteration;

			

			storage::Meas::PMeas array = new storage::Meas[arr_size];
			for (size_t i = 0; i < arr_size; ++i) {
				array[i].id = i%meas2write;
				array[i].time = i;
				id2meas[array[i].id] = array[i];
			}
			ds->append(array, arr_size);
			delete[] array;

			
			query.resize(id2meas.size());
			size_t pos = 0;
			for (auto kv : id2meas) {
				query[pos] = kv.first;
				pos++;
			}

			test_storage(query, id2meas, ds);
		}
		{
			storage::DataStorage::PDataStorage ds =	storage::DataStorage::Open(storage_path);
			ds->loadCurValues(query);
			test_storage(query, id2meas, ds);
		}
	}
	utils::rm(storage_path);
}