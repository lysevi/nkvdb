#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include "test_common.h"

BOOST_AUTO_TEST_CASE(ctor) {
    utils::DataStorage*ds=mdb_test::makeDS();
    BOOST_CHECK_EQUAL(true, true);
}

BOOST_AUTO_TEST_CASE(writeValues) {
    utils::DataStorage*ds=mdb_test::makeDS();
    auto ids=mdb_test::getIds();
    auto tm=mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}


BOOST_AUTO_TEST_CASE(readValues) {
    utils::DataStorage*ds=mdb_test::makeDS();
    auto ids=mdb_test::getIds();
    auto tm=mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}

BOOST_AUTO_TEST_CASE(readValuesInterval) {
    utils::DataStorage*ds=mdb_test::makeDS();
    auto ids=mdb_test::getIds();
    auto tm=mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}

BOOST_AUTO_TEST_CASE(readValuesFltr) {
    utils::DataStorage*ds=mdb_test::makeDS();
    auto ids=mdb_test::getIds();
    auto tm=mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}

BOOST_AUTO_TEST_CASE(readValuesByDate) {
    utils::DataStorage*ds=mdb_test::makeDS();
    auto ids=mdb_test::getIds();
    auto tm=mdb_test::testMeases(ids);
    BOOST_CHECK_EQUAL(true, false);
}