#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include "test_common.h"
#include <meas.h>
#include <page.h>
#include <storage.h>
#include <logger.h>
#include <utils.h>
#include <exception.h>

#include <iterator>
#include <list>
using namespace nkvdb;

BOOST_AUTO_TEST_CASE(PageCreateOpen) {
    {
        Page::Page_ptr created =
                Page::Create(nkvdb_test::test_page_name, nkvdb_test::sizeInMb10);
        BOOST_CHECK(!created->isFull());
        created->close();
    }
    {
        Page::Page_ptr openned = Page::Open(nkvdb_test::test_page_name);

        BOOST_CHECK_EQUAL(openned->size(), nkvdb_test::sizeInMb10);
        BOOST_CHECK(!openned->isFull());
        openned->close();
    }

    utils::rm(nkvdb_test::test_page_name);
}

BOOST_AUTO_TEST_CASE(PageOpenTwice) {
  
	Page::Page_ptr created =  Page::Create(nkvdb_test::test_page_name, nkvdb_test::sizeInMb10);
    BOOST_CHECK(!created->isFull());
	created->close();
  
  {
    Page::Page_ptr openned = Page::Open(nkvdb_test::test_page_name);

    BOOST_CHECK_THROW(Page::Open(nkvdb_test::test_page_name), utils::Exception);

    openned->close();
  }
 utils::rm(nkvdb_test::test_page_name);
}
/**/
BOOST_AUTO_TEST_CASE(PageIO) {
  const size_t TestableMeasCount = 100;
  std::string index = "";
  std::string wname ="";
  {
    {
      Page::Page_ptr storage = Page::Create(nkvdb_test::test_page_name, nkvdb_test::sizeInMb10);
      storage->close();
    }
    const nkvdb::Flag flagValue = 1;
    const nkvdb::Flag srcValue = 2;

    for (size_t i = 0; i < TestableMeasCount; ++i) {
      Page::Page_ptr storage = Page::Open(nkvdb_test::test_page_name);
      auto newMeas = nkvdb::Meas::empty();
      newMeas.setValue(i);
      newMeas.size=sizeof(i);
      newMeas.id = i;
      newMeas.flag = flagValue;
      newMeas.source = srcValue;
      newMeas.time = i;
      storage->append(newMeas);
      storage->close();
    }

    Page::Page_ptr storage = Page::Open(nkvdb_test::test_page_name);

    BOOST_CHECK_EQUAL(storage->minTime(), nkvdb::Time(0));
    BOOST_CHECK_EQUAL(storage->maxTime(), static_cast<nkvdb::Time>(TestableMeasCount - 1));

    auto newMeas = nkvdb::Meas::empty();
    for (size_t i = 0; i < TestableMeasCount; ++i) {
      bool readState = storage->read(&newMeas, i);

      BOOST_CHECK_EQUAL(readState, true);
      //BOOST_CHECK_EQUAL(newMeas.readValue<size_t>(), i);
      BOOST_CHECK_EQUAL(newMeas.id, i);
      BOOST_CHECK_EQUAL(newMeas.flag, flagValue);
      BOOST_CHECK_EQUAL(newMeas.source, srcValue);

      BOOST_CHECK_EQUAL(newMeas.time, (nkvdb::Time)i);
    }
    BOOST_CHECK(!storage->isFull());

	BOOST_CHECK_EQUAL(storage->maxTime(), (nkvdb::Time)TestableMeasCount - 1);
	BOOST_CHECK_EQUAL(storage->minTime(), nkvdb::Time(0));
	BOOST_CHECK_EQUAL(storage->size(), storage->size());
    index = dynamic_cast<nkvdb::Page*>(storage.get())->index_fileName();
	wname = dynamic_cast<nkvdb::Page*>(storage.get())->writewindow_fileName();
    storage->close();
  }

  utils::rm(nkvdb_test::test_page_name);
  utils::rm(index);
  utils::rm(wname);
}

BOOST_AUTO_TEST_CASE(Capacity) {
  const size_t pageSize =
      sizeof(nkvdb::Page::Header) + sizeof(nkvdb::Meas) * 10;
  Page::Page_ptr page = Page::Create(nkvdb_test::test_page_name, pageSize);
  BOOST_CHECK_EQUAL(page->capacity(), (size_t)10);

  auto newMeas = nkvdb::Meas::empty();
  page->append(newMeas);

  BOOST_CHECK_EQUAL(page->capacity(), (size_t)9);
  page->append(newMeas);

  BOOST_CHECK_EQUAL(page->capacity(), (size_t)8);
  auto index = dynamic_cast<nkvdb::Page*>(page.get())->index_fileName();
  auto wname = dynamic_cast<nkvdb::Page*>(page.get())->writewindow_fileName();
  page->close();

  utils::rm(nkvdb_test::test_page_name);
  utils::rm(index);
   utils::rm(wname);
}

BOOST_AUTO_TEST_CASE(AppendMany) {
  const size_t pageSize =Page::calc_size<10>();
  Page::Page_ptr page = Page::Create(nkvdb_test::test_page_name, pageSize);

  size_t arr_size = 15;
  nkvdb::Meas::PMeas array = new nkvdb::Meas[arr_size];
  for (size_t i = 0; i < arr_size; ++i) {
    array[i].id = i;
    array[i].time = i;
    array[i].setValue(i);
  }
  size_t writed = page->append(array, arr_size).writed;
  delete[] array;
  BOOST_CHECK_EQUAL(writed, (size_t)10);

  for (size_t i = 0; i < writed; ++i) {
    nkvdb::Meas readed;
    page->read(&readed, i);
    BOOST_CHECK_EQUAL(readed.id, i);
  }

  BOOST_CHECK_EQUAL(page->minTime(), nkvdb::Time(0));
  BOOST_CHECK_EQUAL(page->maxTime(), (nkvdb::Time)writed-1);
  auto index = page->index_fileName();
  auto wname=page->writewindow_fileName();
  page->close();
  utils::rm(nkvdb_test::test_page_name);
  utils::rm(index);
  utils::rm(wname);
}

BOOST_AUTO_TEST_CASE(PagereadInterval) {
  const int TestableMeasCount = 1000;
  std::string index = "";
  std::string wname = "";
  {
    {
      Page::Page_ptr storage = Page::Create(nkvdb_test::test_page_name, nkvdb_test::sizeInMb10);
      wname=storage->writewindow_fileName();
      storage->close();
    }

    Page::Page_ptr storage = Page::Open(nkvdb_test::test_page_name);
    for (int i = 0; i < TestableMeasCount; ++i) {

      auto newMeas = nkvdb::Meas::empty();
      newMeas.setValue(i);
      newMeas.id = i % 10;
      newMeas.flag = (nkvdb::Flag)(i % 5);
      newMeas.source = (nkvdb::Flag)(i % 5);
      newMeas.time = i;
      storage->append(newMeas);
    }

    {
      IdArray ids = {1, 2, 3};
	  Meas::MeasList readRes;
	  
      auto reader=storage->readInterval(ids, 3, 3, 0, TestableMeasCount);
	  reader->readAll(&readRes);
      reader=nullptr;

      BOOST_CHECK(readRes.size() != 0);
      for (auto it = readRes.cbegin(); it != readRes.cend(); ++it) {

        BOOST_CHECK(it->id != 4 && it->id != 5);
        BOOST_CHECK(it->flag == 3);
        BOOST_CHECK(it->source == 3);
        BOOST_CHECK(it->time <= TestableMeasCount);
      }
    }

    {
      IdArray ids = {1, 2, 3, 4, 5};
	  Meas::MeasList readRes;
      auto reader=storage->readInterval(ids, 0, 0, 0, TestableMeasCount);
	  reader->readAll(&readRes);

      reader=nullptr;
      BOOST_CHECK(readRes.size() != 0);
      bool haveFlag = false;
      bool haveSource = false;
      for (auto it = readRes.cbegin(); it != readRes.cend(); ++it) {

        if (it->source > 3) {
          haveSource = true;
        }
        if (it->flag > 3) {
          haveFlag = true;
        }
        BOOST_CHECK(it->time <= TestableMeasCount);
      }

      BOOST_CHECK(haveSource);
      BOOST_CHECK(haveFlag);
      index = storage->index_fileName();
    }
    storage->close();
  }

  utils::rm(nkvdb_test::test_page_name);
  utils::rm(index);
  utils::rm(wname);
}

BOOST_AUTO_TEST_CASE(DynSize) {
  const size_t pageSize =Page::calc_size<10>();
  Page::Page_ptr page = Page::Create(nkvdb_test::test_page_name, pageSize);

  size_t arr_size = 10;
  nkvdb::Meas::PMeas array = new nkvdb::Meas[arr_size];
  float small=10;
  double big =11;

  for (size_t i = 0; i < arr_size; ++i) {
    array[i].id = i;
    array[i].time = i;
    if(i%2){
        array[i].setValue(small);
    }
    else{
        array[i].setValue(big);
    }
  }
  size_t writed = page->append(array, arr_size).writed;
  delete[] array;
  BOOST_CHECK_EQUAL(writed, (size_t)10);

  for (size_t i = 0; i < writed; ++i) {
    nkvdb::Meas readed;
    page->read(&readed, i);
    BOOST_CHECK_EQUAL(readed.id, i);
    if(i%2){
		auto v = readed.readValue<float>();
        BOOST_CHECK_EQUAL(v, small);
    }
    else{
		auto v = readed.readValue<double>();
        BOOST_CHECK_EQUAL(v, big);
    }
  }

  BOOST_CHECK_EQUAL(page->minTime(), nkvdb::Time(0));
  BOOST_CHECK_EQUAL(page->maxTime(), (nkvdb::Time)writed-1);
  auto index = page->index_fileName();
  auto wname=page->writewindow_fileName();
  page->close();
  utils::rm(nkvdb_test::test_page_name);
  utils::rm(index);
  utils::rm(wname);
}

BOOST_AUTO_TEST_CASE(PageCommonTest) {
	utils::rm("test_page.dbi");
	utils::rm("test_page.dbw");
    auto sz100=nkvdb::Page::calc_size<100>();
    Page::Page_ptr created = Page::Create(nkvdb_test::test_page_name, sz100);
    auto iname=created->index_fileName();
    auto wname=created->writewindow_fileName();
    nkvdb_test::storage_test_io(created.get(),0,100,1);

    created->close();


    utils::rm(nkvdb_test::test_page_name);
    utils::rm(iname);
    utils::rm(wname);
}
