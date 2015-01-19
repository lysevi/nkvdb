#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>

#include <common/Meas.h>


BOOST_AUTO_TEST_CASE(MeasEmpty) {
	common::Meas::PMeas pm = common::Meas::empty();

	BOOST_CHECK_EQUAL(pm->data, common::Value(0));
	BOOST_CHECK_EQUAL(pm->flag, common::Flag(0));
	BOOST_CHECK_EQUAL(pm->id,   common::Id(0));
	BOOST_CHECK_EQUAL(pm->source, common::Flag(0));
	BOOST_CHECK_EQUAL(pm->time,   common::Time(0));
}