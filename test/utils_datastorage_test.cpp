#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include "test_common.h"

BOOST_AUTO_TEST_CASE(ctor) {
	utils::ICache::PCache ds = mdb_test::makeDS();
    BOOST_CHECK_EQUAL(true, true);
}

BOOST_AUTO_TEST_CASE(writeValues) {
	utils::ICache::PCache ds = mdb_test::makeDS();
    auto ids=mdb_test::getIds();
    auto tm=mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}


BOOST_AUTO_TEST_CASE(readValues) {
	utils::ICache::PCache ds = mdb_test::makeDS();
    auto ids=mdb_test::getIds();
    auto tm=mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}

BOOST_AUTO_TEST_CASE(readValuesInterval) {
	utils::ICache::PCache ds = mdb_test::makeDS();
    auto ids=mdb_test::getIds();
    auto tm=mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}

BOOST_AUTO_TEST_CASE(readValuesFltr) {
	utils::ICache::PCache ds = mdb_test::makeDS();
    auto ids=mdb_test::getIds();
    auto tm=mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}

BOOST_AUTO_TEST_CASE(readValuesByDate) {
	utils::ICache::PCache ds = mdb_test::makeDS();
    auto ids=mdb_test::getIds();
    auto tm=mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}