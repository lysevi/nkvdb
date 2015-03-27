#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include "../storage/Meas.h"
#include "../storage/Page.h"

BOOST_AUTO_TEST_CASE(MeasEmpty) {
	storage::Meas::PMeas pm = storage::Meas::empty();

	BOOST_CHECK_EQUAL(pm->data, storage::Value(0));
	BOOST_CHECK_EQUAL(pm->flag, storage::Flag(0));
	BOOST_CHECK_EQUAL(pm->id,   storage::Id(0));
	BOOST_CHECK_EQUAL(pm->source, storage::Flag(0));
	BOOST_CHECK_EQUAL(pm->time,   storage::Time(0));
}

BOOST_AUTO_TEST_CASE(CreateOpenStoragePage) {
	const int sizeInMb = 10;
	{
		Page::PPage created = Page::Create("test_page.db", sizeInMb);
	}
	{
		Page::PPage openned = Page::Open("test_page.db");
		
		BOOST_CHECK_EQUAL(openned->sizeMb(), sizeInMb);
	}
}

