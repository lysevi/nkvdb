#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>

#include <common/Meas.h>


BOOST_AUTO_TEST_CASE(MeasEmpty) {
	//common::Meas::PMeas pm = common::Meas::empty();
	common::Meas m;
	BOOST_CHECK_EQUAL(m.data, 0);
	BOOST_CHECK_EQUAL(m.flag, 0);
	BOOST_CHECK_EQUAL(m.id,   0);
	BOOST_CHECK_EQUAL(m.source, 0);
	BOOST_CHECK_EQUAL(m.time, 0);
}