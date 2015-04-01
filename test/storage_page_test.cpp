#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include "test_common.h"
#include <storage/Meas.h>
#include <storage/Page.h>
#include <storage/storage.h>
#include <utils/ProcessLogger.h>
#include <utils/utils.h>

#include <iterator>
#include <list>
using namespace storage;

BOOST_AUTO_TEST_CASE(PageCreateOpen) {
	{
		Page::PPage created = Page::Create(mdb_test::test_page_name, mdb_test::sizeInMb10);
		BOOST_CHECK(!created->isFull());
	}
	{
		Page::PPage openned = Page::Open(mdb_test::test_page_name);

		BOOST_CHECK_EQUAL(openned->size(), mdb_test::sizeInMb10);
		BOOST_CHECK(!openned->isFull());
	}
}

BOOST_AUTO_TEST_CASE(PageIO) {
	const int TestableMeasCount = 10000;
	{
		{
			Page::PPage storage = Page::Create(mdb_test::test_page_name, mdb_test::sizeInMb10);
		}
		const int flagValue = 1;
		const int srcValue = 2;
		const int timeValue = 3;

		for (int i = 0; i < TestableMeasCount; ++i) {
			Page::PPage storage = Page::Open(mdb_test::test_page_name);
			auto newMeas = storage::Meas::empty();
			newMeas->value = i;
			newMeas->id = i;
			newMeas->flag = flagValue;
			newMeas->source = srcValue;
			newMeas->time = i;
			storage->append(newMeas);
			delete newMeas;
		}

		Page::PPage storage = Page::Open(mdb_test::test_page_name);

		BOOST_CHECK_EQUAL(storage->minTime(), 0);
		BOOST_CHECK_EQUAL(storage->maxTime(), TestableMeasCount - 1);

		auto newMeas = storage::Meas::empty();
		for (int i = 0; i < TestableMeasCount; ++i) {
			bool readState = storage->read(newMeas, i);

			BOOST_CHECK_EQUAL(readState, true);
			BOOST_CHECK_EQUAL(newMeas->value, i);
			BOOST_CHECK_EQUAL(newMeas->id, i);
			BOOST_CHECK_EQUAL(newMeas->flag, flagValue);
			BOOST_CHECK_EQUAL(newMeas->source, srcValue);

			BOOST_CHECK_EQUAL(newMeas->time, i);
		}
		BOOST_CHECK(!storage->isFull());
		delete newMeas;

		auto hdr = storage->getHeader();
		BOOST_CHECK_EQUAL(hdr.maxTime, TestableMeasCount - 1);
		BOOST_CHECK_EQUAL(hdr.minTime, 0);
		BOOST_CHECK_EQUAL(hdr.size, storage->size());
	}
}

BOOST_AUTO_TEST_CASE(Capacity) {
	const size_t pageSize = sizeof(storage::Page::Header) + sizeof(storage::Meas) * 10;
	Page::PPage page = Page::Create(mdb_test::test_page_name, pageSize);
	BOOST_CHECK_EQUAL(page->capacity(), 10);
	
	auto newMeas = storage::Meas::empty();
	page->append(newMeas);
	
	BOOST_CHECK_EQUAL(page->capacity(), 9);
	page->append(newMeas);
	delete newMeas;
	BOOST_CHECK_EQUAL(page->capacity(), 8);
}


BOOST_AUTO_TEST_CASE(AppendMany) {
	const size_t pageSize = sizeof(storage::Page::Header) + sizeof(storage::Meas) * 10;
	Page::PPage page = Page::Create(mdb_test::test_page_name, pageSize);
	
	size_t arr_size = 15;
	storage::Meas::PMeas array = new storage::Meas[arr_size];
	for (int i = 0; i < arr_size; ++i) {
		array[i].id = i;
	}
	size_t writed = page->append(array, arr_size);
	delete[] array;
	BOOST_CHECK_EQUAL(writed, 10);

	for (int i = 0; i < writed; ++i) {
		storage::Meas readed;
		page->read(&readed, i);
		BOOST_CHECK_EQUAL(readed.id, i);
	}
}