#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include "../storage/Meas.h"

BOOST_AUTO_TEST_CASE(MeasEmpty) {
	storage::Meas::PMeas pm = storage::Meas::empty();

	BOOST_CHECK_EQUAL(pm->data, storage::Value(0));
	BOOST_CHECK_EQUAL(pm->flag, storage::Flag(0));
	BOOST_CHECK_EQUAL(pm->id,   storage::Id(0));
	BOOST_CHECK_EQUAL(pm->source, storage::Flag(0));
	BOOST_CHECK_EQUAL(pm->time,   storage::Time(0));
}