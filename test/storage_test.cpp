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

BOOST_AUTO_TEST_CASE(PageCreateOpen) {
    {
        Page::PPage created = Page::Create(mdb_test::test_page_name, mdb_test::sizeInMb10);
        BOOST_CHECK(!created->isFull());
    }
    {
        Page::PPage openned = Page::Open(mdb_test::test_page_name);

        BOOST_CHECK_EQUAL(openned->size(), mdb_test::sizeInMb10);
        BOOST_CHECK(!openned->isFull());
    }
}

BOOST_AUTO_TEST_CASE(PageIO) {
    const int TestableMeasCount = 10000;
    {
        {
            Page::PPage storage = Page::Create(mdb_test::test_page_name, mdb_test::sizeInMb10);
        }
        const int flagValue = 1;
        const int srcValue = 2;
        const int timeValue = 3;
        const int zeroTimeValue = 1;

        for (int i = 0; i < TestableMeasCount; ++i) {
            Page::PPage storage = Page::Open(mdb_test::test_page_name);
            auto newMeas = storage::Meas::empty();
            newMeas->value = i;
            newMeas->id = i;
            newMeas->flag = flagValue;
            newMeas->source = srcValue;
            if (i == 0) {
                newMeas->time = zeroTimeValue;
            } else {
                newMeas->time = i;
            }
            storage->append(newMeas);
            delete newMeas;
        }

        Page::PPage storage = Page::Open(mdb_test::test_page_name);

        BOOST_CHECK_EQUAL(storage->minTime(), zeroTimeValue);
        BOOST_CHECK_EQUAL(storage->maxTime(), TestableMeasCount-1);

        auto newMeas = storage::Meas::empty();
        for (int i = 0; i < TestableMeasCount; ++i) {
            bool readState = storage->read(newMeas, i);

            BOOST_CHECK_EQUAL(readState, true);
            BOOST_CHECK_EQUAL(newMeas->value, i);
            BOOST_CHECK_EQUAL(newMeas->id, i);
            BOOST_CHECK_EQUAL(newMeas->flag, flagValue);
            BOOST_CHECK_EQUAL(newMeas->source, srcValue);

            if(i==0){
                BOOST_CHECK_EQUAL(newMeas->time, zeroTimeValue);
            }else{
                BOOST_CHECK_EQUAL(newMeas->time, i);
            }
        }
        BOOST_CHECK(!storage->isFull());
        delete newMeas;

        auto hdr=storage->getHeader();
        BOOST_CHECK_EQUAL(hdr.maxTime,TestableMeasCount-1);
        BOOST_CHECK_EQUAL(hdr.minTime,zeroTimeValue);
        BOOST_CHECK_EQUAL(hdr.size,storage->size());
    }
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
    const int meas2write=100;
    const int write_iteration=10;
    const uint64_t storage_size=sizeof(storage::Page::Header)+(sizeof(storage::Meas)*meas2write);
	const std::string storage_path = mdb_test::storage_path + "storageIO";
	{
		storage::DataStorage::PDataStorage ds = storage::DataStorage::Create(storage_path, storage_size);

		storage::Meas::PMeas meas = storage::Meas::empty();
		for (int i = 0; i < (meas2write*write_iteration); ++i) {
			meas->value = i;
			meas->id = i%meas2write;
			meas->source = meas->flag = i%meas2write;
			ds->append(meas);
		}
		delete meas;
		ds = nullptr;
		auto pages = utils::ls(storage_path);
		BOOST_CHECK_EQUAL(pages.size(), write_iteration);
	}
    utils::rm(storage_path);
}
