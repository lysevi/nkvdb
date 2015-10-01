#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include "test_common.h"

#include <meas.h>
#include <utils.h>
#include <cache.h>
#include <logger.h>

#include <iterator>
#include <list>
using namespace nkvdb;

BOOST_AUTO_TEST_CASE(CacheIO) {
  const size_t TestableMeasCount = 10000;
  {
    const int flagValue = 1;
    const int srcValue = 2;
    nkvdb::Cache c(TestableMeasCount - 1);

    for (size_t i = 0; i < TestableMeasCount - 1; ++i) {
      auto newMeas = nkvdb::Meas::empty();
      newMeas.value = i;
      newMeas.id = i;
      newMeas.flag = flagValue;
      newMeas.source = srcValue;
      newMeas.time = i;
      auto wrt_res = c.append(newMeas, 0);
      BOOST_CHECK_EQUAL(wrt_res.writed, size_t(1));
    }
    auto newMeas = nkvdb::Meas::empty();
    auto wrt_res = c.append(newMeas, 0);

    BOOST_CHECK_EQUAL(wrt_res.writed, size_t(0));
    BOOST_CHECK_EQUAL(c.append(newMeas, 1).ignored, size_t(1));

    auto interval = c.readInterval(0, TestableMeasCount);
    BOOST_CHECK_EQUAL(interval.size(), TestableMeasCount - 1);
    for (auto m : interval) {
      BOOST_CHECK(
          utils::inInterval<nkvdb::Time>(0, TestableMeasCount - 1, m.time));
    }

    for (size_t i = 0; i < TestableMeasCount - 1; ++i) {
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
    for (size_t i = 0; i < TestableMeasCount - 1; ++i) {
      bool isExists = false;
      for (uint64_t j = 0; j < c.size(); ++j) {
        if (output_array[j].id == (uint64_t)i) {
          isExists = true;
          break;
        }
      }
      BOOST_CHECK(isExists);
    }
  }
}

BOOST_AUTO_TEST_CASE(CacheResize) {
  const int TestableMeasCount = 10000;
  {
    nkvdb::Cache c(TestableMeasCount - 1);

    c.setSize(10);
    BOOST_CHECK_EQUAL(c.size(), size_t(10));
  }
}

BOOST_AUTO_TEST_CASE(CachePoolChecks) {
  nkvdb::CachePool pool(2, 100);

  BOOST_CHECK(pool.haveCache());

  auto c1 = pool.getCache();
  c1->sync_begin();
  auto c2 = pool.getCache();
  c2->sync_begin();

  BOOST_CHECK(!pool.haveCache());
  c1->sync_complete();

  BOOST_CHECK(pool.haveCache());
}
