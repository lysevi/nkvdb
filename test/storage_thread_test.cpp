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
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
using namespace storage;

std::atomic<int> threads_count(0);
const int meas2write = 10;
const int write_iteration = 10;
size_t arr_size = meas2write * write_iteration;

void writer(storage::DataStorage::PDataStorage ds) {
  threads_count++;
  storage::Meas::PMeas meas = storage::Meas::empty();
  for (size_t i = 0; i < arr_size; ++i) {
    meas->value = i;
    meas->id = i;
    meas->source = meas->flag = i % meas2write;
    meas->time = i;
    ds->append(meas);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  delete meas;
}

BOOST_AUTO_TEST_CASE(StorageIOArrays) {

  const uint64_t storage_size =
      sizeof(storage::Page::Header) + (sizeof(storage::Meas) * meas2write);
  const std::string storage_path = mdb_test::storage_path + "storageIO";

  {
    storage::DataStorage::PDataStorage ds =
        storage::DataStorage::Create(storage_path, storage_size);

    std::thread t1(writer, ds);
    std::thread t2(writer, ds);
    std::thread t3(writer, ds);
    std::thread t4(writer, ds);
    std::thread t5(writer, ds);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();

    auto meases = ds->readInterval(0, arr_size);

    BOOST_CHECK(meases.size() == arr_size * 5);

    for (size_t i = 0; i < arr_size; ++i) {
      int count = threads_count;
      for (auto m : meases) {
        if (m.id == i) {
          count--;
        }
      }
      BOOST_CHECK(count == 0);
    }
  }
  utils::rm(storage_path);
}
