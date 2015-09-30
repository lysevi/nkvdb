#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include "test_common.h"
#include <page.h>
#include <storage.h>
#include <logger.h>
#include <utils.h>

#include <iterator>
#include <list>
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

using namespace mdb;

std::atomic<int> threads_count(0);
const int meas2write = 10;
const int write_iteration = 10;
size_t arr_size = meas2write * write_iteration;

void writer(mdb::Storage::Storage_ptr ds) {
	threads_count++;
    mdb::Meas meas = mdb::Meas::empty();
	for (size_t i = 0; i < arr_size; ++i) {
        meas.value = i;
        meas.id = i;
        meas.source = meas.flag = i % meas2write;
        meas.time = i;
        ds->append(meas);
	}
}

BOOST_AUTO_TEST_CASE(StorageIOArrays) {

  const uint64_t storage_size =
      sizeof(mdb::Page::Header) + (sizeof(mdb::Meas) * meas2write);
  const std::string storage_path = mdb_test::storage_path + "storageIO";

  {
    mdb::Storage::Storage_ptr ds =
        mdb::Storage::Create(storage_path, storage_size);

    std::thread t1(writer, ds);
    std::thread t2(writer, ds);
    std::thread t3(writer, ds);
    std::thread t4(writer, ds);
    std::thread t5(writer, ds);

    t5.join();
    t4.join();
    t3.join();
    t2.join();
    t1.join();

    Meas::MeasList meases {};
    auto reader = ds->readInterval(0, arr_size);
    while(!reader->isEnd()){
        reader->readNext(&meases);
    }

    BOOST_CHECK_EQUAL(meases.size(), arr_size * 5);

    for (size_t i = 0; i < arr_size; ++i) {
      int count = threads_count;
      for (auto m : meases) {
        if (m.id == i) {
          count--;
        }
      }
      BOOST_CHECK_EQUAL(count, 0);
    }
  }
  utils::rm(storage_path);
}
