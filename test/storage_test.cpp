#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include "test_common.h"
#include <page.h>
#include <storage.h>
#include <time_utils.h>
#include <logger.h>
#include <utils.h>

#include <iterator>
#include <list>
#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>

using namespace mdb;

BOOST_AUTO_TEST_CASE(MeasEmpty) {
  mdb::Meas pm = mdb::Meas::empty();

  BOOST_CHECK_EQUAL(pm.value, mdb::Value(0));
  BOOST_CHECK_EQUAL(pm.flag, mdb::Flag(0));
  BOOST_CHECK_EQUAL(pm.id, mdb::Id(0));
  BOOST_CHECK_EQUAL(pm.source, mdb::Flag(0));
  BOOST_CHECK_EQUAL(pm.time, mdb::Time(0));
}

BOOST_AUTO_TEST_CASE(StorageCreateOpen) {
  {
    {
      mdb::Storage::Storage_ptr ds = mdb::Storage::Create(mdb_test::storage_path);
      BOOST_CHECK(boost::filesystem::exists(mdb_test::storage_path));
      BOOST_CHECK(boost::filesystem::is_directory(mdb_test::storage_path));

      std::list<boost::filesystem::path> pages = utils::ls(mdb_test::storage_path);
      BOOST_CHECK_EQUAL(pages.size(), (size_t)2);
      ds->Close();

      ds = mdb::Storage::Create(mdb_test::storage_path);
      BOOST_CHECK(boost::filesystem::exists(mdb_test::storage_path));
      BOOST_CHECK(boost::filesystem::is_directory(mdb_test::storage_path));
      pages = utils::ls(mdb_test::storage_path);
      BOOST_CHECK_EQUAL(pages.size(), (size_t)2);
      ds->Close();
    }
    {
      mdb::Storage::Storage_ptr ds =  mdb::Storage::Open(mdb_test::storage_path);
      ds=nullptr;
    }
  }
  utils::rm(mdb_test::storage_path);
}

BOOST_AUTO_TEST_CASE(StorageIO) {
  const int meas2write = 10;
  const int write_iteration = 10;
  const uint64_t storage_size = sizeof(mdb::Page::Header) + (sizeof(mdb::Meas) * meas2write);
  const std::string storage_path = mdb_test::storage_path + "storageIO";


  {
    mdb::Storage::Storage_ptr ds =
        mdb::Storage::Create(storage_path, storage_size);

    mdb::Meas meas = mdb::Meas::empty();
    mdb::Time end_it = (meas2write * write_iteration);
    for (mdb::Time i = 0; i < end_it; ++i) {
      meas.value = i;
      meas.id = i;
      meas.source = meas.flag = i % meas2write;
      meas.time = i;
      ds->append(meas);

      Meas::MeasList meases{};
      auto reader = ds->readInterval(0, end_it);
      reader->readAll(&meases);


      for (mdb::Time j = 0; j < i; ++j) {
        bool isExists = false;
        for (mdb::Meas m : meases) {
          if (m.id == static_cast<mdb::Id>(j)) {
            isExists = true;
            break;
          }
        }
        BOOST_CHECK(isExists);
      }
    }
	ds->Close();
    ds = nullptr;
    auto pages = utils::ls(storage_path);
    BOOST_CHECK_EQUAL(pages.size(), (size_t)(write_iteration * 3));
  }
  {
    mdb::Storage::Storage_ptr ds =
        mdb::Storage::Open(storage_path);

    for (int i = 1; i < meas2write * write_iteration;i += (meas2write * write_iteration) / 100) {
      mdb::Time to = i * ((meas2write * write_iteration) / 100);

      Meas::MeasList meases{};
      auto reader = ds->readInterval(0, to);
      reader->readAll(&meases);

      BOOST_CHECK_EQUAL(meases.size(), (size_t)(to + 1));

      for (mdb::Meas m : meases) {
        BOOST_CHECK(utils::inInterval<mdb::Time>(0, to, m.time));

        BOOST_CHECK(m.time <= to);
      }
    }
	ds->Close();
  }
 
  utils::rm(storage_path);
  
 {/// readinterval with not exists data. reading from WriteWindow.
      mdb::Storage::Storage_ptr ds = mdb::Storage::Create(storage_path, storage_size);

      mdb::Meas meas = mdb::Meas::empty();
      mdb::Time end_it = (meas2write * write_iteration);
      mdb::Time queryFrom = 20;
      mdb::Time queryFrom2 = 22;
      mdb::Id   Id = 1;
      for (mdb::Time i = 0; i < end_it; ++i) {
		  if (i == queryFrom) {
			  Id = 2;
			  continue;
		  }

          if (i == queryFrom2) {
              continue;
          }

		  meas.value = i;
		  meas.id = Id;
		  meas.source = meas.flag = i % meas2write;
		  meas.time = i;
		  ds->append(meas);
	  }
	  ds->setPastTime(end_it);

	  Meas::MeasList meases{};
      auto queryIds = IdArray{ 1, 2, 3 };
	  
	  auto reader = ds->readInterval(queryIds,0,0,queryFrom, end_it);
	  reader->readAll(&meases);

	  // [..max] from [min]
      meases.erase(std::remove_if(meases.begin(), meases.end(), [queryFrom](const mdb::Meas&m){return m.time > queryFrom; }), meases.end());
	  
      BOOST_CHECK_EQUAL(meases.size(), size_t(1));
      BOOST_CHECK_EQUAL(meases.front().id, mdb::Id(1));

	  // [..max] [min..from]
      meases.clear();
	  reader = ds->readInterval(IdArray{1,2},0,0,queryFrom2, end_it);
      reader->readAll(&meases);

      meases.erase(std::remove_if(meases.begin(), meases.end(), [queryFrom2](const mdb::Meas&m){return m.time > queryFrom2; }), meases.end());
	  
      BOOST_CHECK_EQUAL(meases.size(), size_t(1));
      BOOST_CHECK_EQUAL(meases.front().id, mdb::Id(1));

	  ds->Close();
	  utils::rm(storage_path);
  }
}

BOOST_AUTO_TEST_CASE(StorageIOArrays) {
  const int meas2write = 10;
  const size_t write_iteration = 10;
  const uint64_t storage_size =
      sizeof(mdb::Page::Header) + (sizeof(mdb::Meas) * meas2write);
  const std::string storage_path = mdb_test::storage_path + "storageIO";

  {
    mdb::Storage::Storage_ptr ds =
        mdb::Storage::Create(storage_path, storage_size);

    size_t arr_size = meas2write * write_iteration;
    mdb::Meas::PMeas array = new mdb::Meas[arr_size];
    for (size_t i = 0; i < arr_size; ++i) {
      array[i].id = i;
      array[i].time = i;
    }
    ds->append(array, arr_size);
    delete[] array;

    Meas::MeasList interval{};
    auto reader = ds->readInterval(0, arr_size);
    reader->readAll(&interval);

    BOOST_CHECK_EQUAL(interval.size(), arr_size);
    for (auto m : interval) {
      BOOST_CHECK(utils::inInterval<mdb::Time>(0, arr_size, m.time));
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
    BOOST_CHECK_EQUAL(pages.size(), write_iteration * 3);
  }
  utils::rm(storage_path);
}

BOOST_AUTO_TEST_CASE(StorageIORealTime) {
  const int meas2write = 10;
  const int write_iteration = 10;
  const uint64_t storage_size =
      sizeof(mdb::Page::Header) + (sizeof(mdb::Meas) * meas2write);
  const std::string storage_path = mdb_test::storage_path + "storageIO";

  {
    mdb::Storage::Storage_ptr ds =
        mdb::Storage::Create(storage_path, storage_size);

    ds->setPastTime(mdb::TimeWork::fromDuration(std::chrono::seconds(2)));

    size_t arr_size = meas2write * write_iteration;

    mdb::Meas::PMeas array = new mdb::Meas[arr_size];

    auto cur_utc_time=mdb::TimeWork::CurrentUtcTime();
    for (size_t i = 0; i < arr_size; ++i) {
      array[i].id = i;
      array[i].time = cur_utc_time;
    }
    ds->append(array, arr_size);


    Meas::MeasList interval {};
    auto reader = ds->readInterval(array[0].time, array[arr_size - 1].time);
    reader->readAll(&interval);

    BOOST_CHECK_EQUAL(interval.size(), arr_size);
    for (auto m : interval) {
      BOOST_CHECK(utils::inInterval<mdb::Time>(array[0].time, array[arr_size - 1].time, m.time));
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
    auto test_storage = [](IdArray query, std::map<mdb::Id, mdb::Meas> id2meas, mdb::Storage::Storage_ptr ds){
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
        sizeof(mdb::Page::Header) + (sizeof(mdb::Meas) * meas2write);
	const std::string storage_path = mdb_test::storage_path + "storageIO";
	
    std::map<mdb::Id, mdb::Meas> id2meas;
	IdArray query{};
	{
		{
            mdb::Storage::Storage_ptr ds =	mdb::Storage::Create(storage_path, storage_size);

			size_t arr_size = meas2write * write_iteration;

			

            mdb::Meas::PMeas array = new mdb::Meas[arr_size];
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
            mdb::Storage::Storage_ptr ds =	mdb::Storage::Open(storage_path);
			ds->loadCurValues(query);
			test_storage(query, id2meas, ds);
		}
	}
    {
        /// check query with not found result
        mdb::Storage::Storage_ptr ds =	mdb::Storage::Open(storage_path);
        IdArray query={0,1,meas2write+2,meas2write+3};
        auto notFound=ds->loadCurValues(query);
        BOOST_CHECK_EQUAL(notFound.size(),size_t(2));
    }
	utils::rm(storage_path);
}


BOOST_AUTO_TEST_CASE(StorageReadTwoTimesParallel) {
    const int meas2write = 10;
    const size_t write_iteration = 10;
    const uint64_t storage_size =
            sizeof(mdb::Page::Header) + (sizeof(mdb::Meas) * meas2write);
    const std::string storage_path = mdb_test::storage_path + "storageIO";

    {
        mdb::Storage::Storage_ptr ds =
                mdb::Storage::Create(storage_path, storage_size);

        size_t arr_size = meas2write * write_iteration;
        mdb::Meas::PMeas array = new mdb::Meas[arr_size];
        for (size_t i = 0; i < arr_size; ++i) {
            array[i].id = i;
            array[i].time = i;
        }
        ds->append(array, arr_size);
        delete[] array;

        Meas::MeasList interval{};
        auto reader = ds->readInterval(0, arr_size);

        Meas::MeasList interval2{};
        auto reader2 = ds->readInterval(0, arr_size);

        while(!reader->isEnd()){
            reader->readNext(&interval);
        }

        while(!reader2->isEnd()){
            reader2->readNext(&interval2);
        }

        BOOST_CHECK_EQUAL(interval.size(), arr_size);
        BOOST_CHECK_EQUAL(interval2.size(), arr_size);
    }
    utils::rm(storage_path);
}
