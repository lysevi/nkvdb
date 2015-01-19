#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include "test_common.h"
#include <utils/ExternalCacheMemory.h>

const size_t testCacheSz = 160 * 1024 * 1024; //160 mb

utils::ExternalCacheMemory* makeEX() {
	utils::ExternalCacheMemory*excache = new utils::ExternalCacheMemory(testCacheSz,mdb_test::getIds());
	return excache;
}

BOOST_AUTO_TEST_CASE(ctor) {
    utils::ExternalCacheMemory*ds=makeEX();
    BOOST_CHECK_EQUAL(true, true);
}

BOOST_AUTO_TEST_CASE(writeValues) {
	utils::ExternalCacheMemory*ds = makeEX();
    auto ids=mdb_test::getIds();
    auto tm=mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}


BOOST_AUTO_TEST_CASE(readValues) {
	utils::ExternalCacheMemory*ds = makeEX();
    auto ids=mdb_test::getIds();
    auto tm=mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}

BOOST_AUTO_TEST_CASE(readValuesInterval) {
	utils::ExternalCacheMemory*ds = makeEX();
    auto ids=mdb_test::getIds();
    auto tm=mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}

BOOST_AUTO_TEST_CASE(readValuesFltr) {
	utils::ExternalCacheMemory*ds = makeEX();
    auto ids=mdb_test::getIds();
    auto tm=mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}

BOOST_AUTO_TEST_CASE(readValuesByDate) {
	utils::ExternalCacheMemory*ds = makeEX();
    auto ids=mdb_test::getIds();
    auto tm=mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}