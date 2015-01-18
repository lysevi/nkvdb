#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include "test_common.h"

BOOST_AUTO_TEST_CASE(ctor) {
    utils::DataStorage*ds=mdb_test::makeDS();
    BOOST_CHECK_EQUAL(true, true);
}
