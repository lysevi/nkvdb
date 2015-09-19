#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include "test_common.h"
#include <libmdb/page.h>
#include <libmdb/storage.h>
#include <libmdb/config.h>
#include <libmdb/time.h>
#include <libmdb/utils/logger.h>
#include <libmdb/utils/utils.h>

#include <iterator>
#include <list>
#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>

using namespace mdb;

BOOST_AUTO_TEST_CASE(StorageReadInterval) {
    const int meas2write = 5;
    const uint64_t storage_size = sizeof(mdb::Page::Header) + (sizeof(mdb::Meas) * meas2write);
    const std::string storage_path = mdb_test::storage_path + "storageIO";
    auto ds=mdb::Storage::Create(storage_path,storage_size);

    mdb::Meas m;
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

		{
			auto tp_reader = ds->readInTimePoint(6);
            mdb::Meas::MeasList output_in_point{};
			tp_reader->readAll(&output_in_point);

			BOOST_CHECK_EQUAL(output_in_point.size(), size_t(5));
		}
		{
			
			auto tp_reader = ds->readInTimePoint(3);
            mdb::Meas::MeasList output_in_point{};
			tp_reader->readAll(&output_in_point);

			BOOST_CHECK_EQUAL(output_in_point.size(), size_t(2));
			for (auto v : output_in_point) {
				BOOST_CHECK(v.time<=3);
			}
		}
		auto reader=ds->readInterval(3,5);
        mdb::Meas::MeasList output{};
        reader->readAll(&output);
        BOOST_CHECK_EQUAL(output.size(),size_t(5));
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
		{

			auto tp_reader = ds->readInTimePoint(8);
            mdb::Meas::MeasList output_in_point{};
			tp_reader->readAll(&output_in_point);

			BOOST_CHECK_EQUAL(output_in_point.size(), size_t(5));
			for (auto v : output_in_point) {
				BOOST_CHECK(v.time <= 8);
			}
		}

        auto reader=ds->readInterval(IdArray{1,2,4,5,55},0,0,8,10);
        mdb::Meas::MeasList output{};
        reader->readAll(&output);
        BOOST_CHECK_EQUAL(output.size(),size_t(7));
    }
    ds->Close();
    utils::rm(storage_path);
}
