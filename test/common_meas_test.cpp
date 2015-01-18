#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>

#include <common/Meas.h>


BOOST_AUTO_TEST_CASE(MeasEmpty) {
	//common::Meas::PMeas pm = common::Meas::empty();
	common::Meas m;
	BOOST_CHECK_EQUAL(m.data, common::Value(0));
	BOOST_CHECK_EQUAL(m.flag, common::Flag(0));
	BOOST_CHECK_EQUAL(m.id,   common::Id(0));
	BOOST_CHECK_EQUAL(m.source, common::Flag(0));
	BOOST_CHECK_EQUAL(m.time, common::Time(0));
}