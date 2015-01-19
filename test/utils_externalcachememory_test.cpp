#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include "test_common.h"
#include <utils/IExternalCache.h>
#include <utils/ExternalCacheMemory.h>

utils::IExternalCache::PExternalCache makeEX() {
	utils::IExternalCache::PExternalCache excache(new utils::ExternalCacheMemory(mdb_test::testCacheSz, mdb_test::getIds()));
	return excache;
}

BOOST_AUTO_TEST_CASE(ctor) {
	utils::IExternalCache::PExternalCache ds = makeEX();
    BOOST_CHECK_EQUAL(true, true);
}

BOOST_AUTO_TEST_CASE(writeValues) {
	utils::IExternalCache::PExternalCache ds = makeEX();
    auto ids=mdb_test::getIds();
    auto tm=mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}


BOOST_AUTO_TEST_CASE(readValues) {
	utils::IExternalCache::PExternalCache ds = makeEX();
    auto ids=mdb_test::getIds();
    auto tm=mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}

BOOST_AUTO_TEST_CASE(readValuesInterval) {
	utils::IExternalCache::PExternalCache ds = makeEX();
    auto ids=mdb_test::getIds();
    auto tm=mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}

BOOST_AUTO_TEST_CASE(readValuesFltr) {
	utils::IExternalCache::PExternalCache ds = makeEX();
    auto ids=mdb_test::getIds();
    auto tm=mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}

BOOST_AUTO_TEST_CASE(readValuesByDate) {
	utils::IExternalCache::PExternalCache ds = makeEX();
    auto ids=mdb_test::getIds();
    auto tm=mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}