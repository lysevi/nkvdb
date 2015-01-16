#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>

#include <common/Meas.h>


BOOST_AUTO_TEST_CASE(MeasEmpty) {
	common::Meas::PMeas pm = common::Meas::empty();
	
	BOOST_CHECK_EQUAL(pm->data==nullptr, true);
	BOOST_CHECK_EQUAL(pm->flag, 0);
	BOOST_CHECK_EQUAL(pm->id,   0);
	BOOST_CHECK_EQUAL(pm->size, 0);
	BOOST_CHECK_EQUAL(pm->source, 0);
	BOOST_CHECK_EQUAL(pm->time, 0);
}