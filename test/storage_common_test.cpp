#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>

#include <bloom_filter.h>

BOOST_AUTO_TEST_CASE(BloomTest) {
    typedef nkvdb::Bloom<uint8_t> u8_fltr_t;

    auto u8_fltr=u8_fltr_t::empty();

    BOOST_CHECK_EQUAL(u8_fltr, uint8_t{0});

    u8_fltr=u8_fltr_t::add(u8_fltr,uint8_t{1});
    u8_fltr=u8_fltr_t::add(u8_fltr,uint8_t{2});

    BOOST_CHECK(u8_fltr_t::check(u8_fltr,uint8_t{1}));
    BOOST_CHECK(u8_fltr_t::check(u8_fltr,uint8_t{2}));
    BOOST_CHECK(u8_fltr_t::check(u8_fltr,uint8_t{3}));
    BOOST_CHECK(!u8_fltr_t::check(u8_fltr,uint8_t{4}));
    BOOST_CHECK(!u8_fltr_t::check(u8_fltr,uint8_t{5}));
}
