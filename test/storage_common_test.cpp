#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>

#include <bloom_filter.h>

BOOST_AUTO_TEST_CASE(BloomTest) {
    typedef uint8_t u8_fltr_t;

    auto u8_fltr=nkvdb::bloom_empty<u8_fltr_t>();

    BOOST_CHECK_EQUAL(u8_fltr, uint8_t{0});

	u8_fltr = nkvdb::bloom_add(u8_fltr, uint8_t{ 1 });
	u8_fltr = nkvdb::bloom_add(u8_fltr, uint8_t{ 2 });

	BOOST_CHECK(nkvdb::bloom_check(u8_fltr, uint8_t{ 1 }));
	BOOST_CHECK(nkvdb::bloom_check(u8_fltr, uint8_t{ 2 }));
	BOOST_CHECK(nkvdb::bloom_check(u8_fltr, uint8_t{ 3 }));
	BOOST_CHECK(!nkvdb::bloom_check(u8_fltr, uint8_t{ 4 }));
	BOOST_CHECK(!nkvdb::bloom_check(u8_fltr, uint8_t{ 5 }));
}
