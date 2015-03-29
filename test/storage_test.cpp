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
        Page::PPage created = Page::Create("test_page.db", mdb_test::sizeInMb10);
        BOOST_CHECK(!created->isFull());
    }
    {
        Page::PPage openned = Page::Open("test_page.db");

        BOOST_CHECK_EQUAL(openned->sizeMb(), mdb_test::sizeInMb10);
        BOOST_CHECK(!openned->isFull());
    }
}

BOOST_AUTO_TEST_CASE(PageIO) {
    const int TestableMeasCount = 10000;
    {
        {
            Page::PPage storage = Page::Create("test_page.db", mdb_test::sizeInMb10);
        }
        const int flagValue = 1;
        const int srcValue = 2;
        const int timeValue = 3;
        const int zeroTimeValue = 1;

        for (int i = 0; i < TestableMeasCount; ++i) {
            Page::PPage storage = Page::Open("test_page.db");
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

        Page::PPage storage = Page::Open("test_page.db");

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
    const std::string path="dstorage";
    {
        storage::DataStorage::PDataStorage ds=storage::DataStorage::Create(path);
        BOOST_CHECK(boost::filesystem::exists(path));
        BOOST_CHECK(boost::filesystem::is_directory(path));

        std::list<boost::filesystem::path> pages=utils::ls(path);
        BOOST_CHECK_EQUAL(pages.size(),1);

        ds=storage::DataStorage::Create(path);
        BOOST_CHECK(boost::filesystem::exists(path));
        BOOST_CHECK(boost::filesystem::is_directory(path));
        pages=utils::ls(path);
        BOOST_CHECK_EQUAL(pages.size(),1);
    }
    {
        storage::DataStorage::PDataStorage ds=storage::DataStorage::Open(path);
    }
}
