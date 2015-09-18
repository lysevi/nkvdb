#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include "test_common.h"
#include <storage/page.h>
#include <storage/storage.h>
#include <storage/config.h>
#include <storage/time.h>
#include "storage/utils/logger.h"
#include "storage/utils/utils.h"

#include <iterator>
#include <list>
#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>

using namespace storage;
/*
BOOST_AUTO_TEST_CASE(StorageReadInterval) {
    const int meas2write = 5;
    const uint64_t storage_size = sizeof(storage::Page::Header) + (sizeof(storage::Meas) * meas2write);
    const std::string storage_path = mdb_test::storage_path + "storageIO";
    auto ds=storage::Storage::Create(storage_path,storage_size);

    storage::Meas m;
    {
        m.id=1; m.time=1;
        ds->append(m);
        m.id=2;m.time=2;
        ds->append(m);

        m.id=4;m.time=4;
        ds->append(m);
        m.id=5;m.time=5;
        ds->append(m);
        m.id=55;m.time=5;
        ds->append(m);

        auto reader=ds->readInterval(3,5);
        storage::Meas::MeasList output{};
        reader->readAll(&output);
        BOOST_CHECK_EQUAL(output.size(),size_t(3));
    }
    {
        m.id=1; m.time=6;
        ds->append(m);
        m.id=2;m.time=7;
        ds->append(m);

        m.id=4;m.time=9;
        ds->append(m);
        m.id=5;m.time=10;
        ds->append(m);
        m.id=6;m.time=10;
        ds->append(m);

        auto reader=ds->readInterval(IdArray{1,2,4,5,55},0,0,8,10);
        storage::Meas::MeasList output{};
        reader->readAll(&output);
        BOOST_CHECK_EQUAL(output.size(),size_t(5));
    }
    ds->Close();
    utils::rm(storage_path);
}
*/