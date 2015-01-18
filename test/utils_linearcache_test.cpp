#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include <utils/LinearCache.h>

common::IdArray getIds() {
	common::IdArray result{1,2,3,4,5,6,7,8,9,0};
	return result;
}


BOOST_AUTO_TEST_CASE(ctor) {
	common::IdArray ids=getIds();
	utils::LinearCache lcache(ids);

	BOOST_CHECK_EQUAL(lcache.size(), ids.size());
}

BOOST_AUTO_TEST_CASE(read_write) {
	common::IdArray ids = getIds();
	utils::LinearCache lcache(ids);
	for (auto id : ids) {
		common::Meas m;
		m.id = id;
		m.time = id*10;
		m.data = id;
		lcache.writeValue(m);
	}

	for (auto id : ids) {
		common::Meas m=lcache.readValue(id);

		BOOST_CHECK_EQUAL(m.id, id);
		BOOST_CHECK_EQUAL(m.time, id*10);
		BOOST_CHECK_EQUAL(m.data, id);
	}

        auto meases=lcache.readValues(ids);

        BOOST_CHECK_EQUAL(meases.size(), ids.size());

        for(size_t i=0;i<meases.size();++i){
            BOOST_CHECK_EQUAL(meases[i].id, ids[i]);
	    BOOST_CHECK_EQUAL(meases[i].time, ids[i]*10);
	    BOOST_CHECK_EQUAL(meases[i].data, ids[i]);
        }
}
