#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include "test_common.h"
#include <storage/Meas.h>
#include <utils/utils.h>
#include <storage/cache.h>
#include <utils/ProcessLogger.h>
#include <utils/utils.h>

#include <iterator>
#include <list>
using namespace storage;

BOOST_AUTO_TEST_CASE(CacheIO) {
	const int TestableMeasCount = 10000;
	{
		const int flagValue = 1;
		const int srcValue = 2;
		storage::Cache c(TestableMeasCount - 1);
		
		for (int i = 0; i < TestableMeasCount-1; ++i) {
			auto newMeas = storage::Meas::empty();
			newMeas->value = i;
			newMeas->id = i;
			newMeas->flag = flagValue;
			newMeas->source = srcValue;
			newMeas->time = i;
			bool flag=c.append(*newMeas);
			BOOST_CHECK_EQUAL(flag, true);
			delete newMeas;
		}
		auto newMeas = storage::Meas::empty();
		bool flag=c.append(*newMeas);
		delete newMeas;
		BOOST_CHECK_EQUAL(flag, false);

		auto interval = c.readInterval(0, TestableMeasCount); 
		BOOST_CHECK_EQUAL(interval.size(), TestableMeasCount-1);
		for (auto m : interval) {
			BOOST_CHECK(utils::inInterval<storage::Time>(0, TestableMeasCount - 1, m.time));
		}

		for (int i = 0; i < TestableMeasCount - 1; ++i) {
			bool isExists = false;
			for (auto m : interval) {
                if (m.id == (uint64_t)i) {
					isExists = true;
					break;
				}
			}
			BOOST_CHECK(isExists);
		}


        auto output_array = c.asArray();
		for (int i = 0; i < TestableMeasCount - 1; ++i) {
			bool isExists = false;
            for (uint64_t j = 0; j < c.size() ; ++j) {
                if (output_array[j].id == (uint64_t)i) {
					isExists = true;
					break;
				}
			}
			BOOST_CHECK(isExists);
		}

	}
}

BOOST_AUTO_TEST_CASE(CachePoolChecks){
    storage::CachePool pool(2,100);

    BOOST_CHECK(pool.haveCache());

    auto c1 = pool.getCache();
    c1->sync_begin();
    auto c2 = pool.getCache();
    c2->sync_begin();

    BOOST_CHECK(!pool.haveCache());
    c1->sync_complete();

    BOOST_CHECK(pool.haveCache());
}
