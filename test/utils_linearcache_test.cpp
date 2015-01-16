#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include <Utils/LinearCache.h>

common::IdList getIds() {
	common::IdList result{1,2,3,4,5,6,7,8,9,0};
	return result;
}


BOOST_AUTO_TEST_CASE(ctor) {
	common::IdList ids=getIds();
	utils::LinearCache lcache(ids);
}

BOOST_AUTO_TEST_CASE(write) {
	common::IdList ids = getIds();
	utils::LinearCache lcache(ids);
	common::Meas::PMeas pm = common::Meas::empty();
	lcache.writeValue(pm);
}