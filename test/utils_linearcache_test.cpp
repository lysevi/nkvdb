#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include <utils/LinearCache.h>
#include <iostream>
#include "test_common.h"

BOOST_AUTO_TEST_CASE(ctor) {
    common::IdArray ids = mdb_test::getIds();
    utils::LinearCache lcache(ids);

    BOOST_CHECK_EQUAL(lcache.size(), ids.size());
}
BOOST_AUTO_TEST_CASE(read_write) {
    common::IdArray ids = mdb_test::getIds();
    utils::LinearCache lcache(ids);
    auto tm=mdb_test::testMeases(ids);
    for (auto &m:tm) {
        lcache.writeValue(m);
    }

    for (auto id:ids) {
        common::Meas m = lcache.readValue(id);

        BOOST_CHECK_EQUAL(m.id, id);
        BOOST_CHECK_EQUAL(m.time, id * 10);
        BOOST_CHECK_EQUAL(m.data, id);
    }

    auto meases = lcache.readValues(ids);

    BOOST_CHECK_EQUAL(meases.size(), ids.size());

    for (size_t i = 0; i < meases.size(); ++i) {
        BOOST_CHECK_EQUAL(meases[i].id, ids[i]);
        BOOST_CHECK_EQUAL(meases[i].time, ids[i]*10);
        BOOST_CHECK_EQUAL(meases[i].data, ids[i]);
    }
}

BOOST_AUTO_TEST_CASE(read_write_group) {
    common::IdArray ids = mdb_test::getIds();
    utils::LinearCache lcache(ids);
    auto tm=mdb_test::testMeases(ids);
    lcache.writeValues(tm);

    auto meases = lcache.readValues(ids);

    BOOST_CHECK_EQUAL(meases.size(), ids.size());

    for (size_t i = 0; i < meases.size(); ++i) {
        BOOST_CHECK_EQUAL(meases[i].id, ids[i]);
        BOOST_CHECK_EQUAL(meases[i].time, ids[i]*10);
        BOOST_CHECK_EQUAL(meases[i].data, ids[i]);
    }
}

BOOST_AUTO_TEST_CASE(read_flg) {
    common::IdArray ids = mdb_test::getIds();
    utils::LinearCache lcache(ids);
    auto tm=mdb_test::testMeases(ids);
    lcache.writeValues(tm);

    auto meases = lcache.readValuesFltr(ids,0,(common::Flag)ids[0]);

    BOOST_CHECK_EQUAL(meases.size(),(size_t)1);

    BOOST_CHECK_EQUAL(meases[0].id,ids[0]);
}