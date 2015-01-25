#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include "test_common.h"
#include <utils/ICache.h>
#include <utils/MemoryCache.h>

#include <iostream>

utils::ICache::PCache makeEX() {
    utils::ICache::PCache excache(new utils::MemoryCache(mdb_test::testCacheSz, mdb_test::getIds()));
    return excache;
}

BOOST_AUTO_TEST_CASE(ctor) {
    utils::ICache::PCache ds = makeEX();
    BOOST_CHECK_EQUAL(true, true);
}

BOOST_AUTO_TEST_CASE(readWriteValues) {
    utils::ICache::PCache ds = makeEX();
    auto ids = mdb_test::getIds();
    auto tm = mdb_test::testMeases(ids);
    for(int i=0;i<100000;++i){
        ds->writeValues(tm);
    }
    
}
/*
BOOST_AUTO_TEST_CASE(readValues) {
    utils::ICache::PCache ds = makeEX();
    auto ids = mdb_test::getIds();
    auto tm = mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}

BOOST_AUTO_TEST_CASE(readValuesInterval) {
    utils::ICache::PCache ds = makeEX();
    auto ids = mdb_test::getIds();
    auto tm = mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}

BOOST_AUTO_TEST_CASE(readValuesFltr) {
    utils::ICache::PCache ds = makeEX();
    auto ids = mdb_test::getIds();
    auto tm = mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}

BOOST_AUTO_TEST_CASE(readValuesByDate) {
    utils::ICache::PCache ds = makeEX();
    auto ids = mdb_test::getIds();
    auto tm = mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}*/