#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include <Utils/Utils.hpp>


BOOST_AUTO_TEST_CASE(testSimple) {
    BOOST_CHECK_EQUAL(true, true);
}