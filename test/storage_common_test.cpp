#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>

#include <bloom_filter.h>

BOOST_AUTO_TEST_CASE(BloomTest) {
    auto u8_fltr=nkvdb::Bloom<uint8_t>::create(111);
    BOOST_CHECK_EQUAL(u8_fltr.fltr, char{111});
    BOOST_CHECK_EQUAL(sizeof(u8_fltr.fltr), sizeof(uint8_t));

    u8_fltr=nkvdb::Bloom<uint8_t>::empty();

    BOOST_CHECK_EQUAL(u8_fltr.fltr, char{0});

    u8_fltr.add(uint8_t{1});
    u8_fltr.add(uint8_t{2});

    BOOST_CHECK(u8_fltr.check(uint8_t{1}));
    BOOST_CHECK(u8_fltr.check(uint8_t{2}));
    BOOST_CHECK(u8_fltr.check(uint8_t{3}));
    BOOST_CHECK(!u8_fltr.check(uint8_t{4}));
}
