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
	std::string index = "";
	{
		{
			Page::PPage storage = Page::Create(mdb_test::test_page_name, mdb_test::sizeInMb10);
		}
		const int flagValue = 1;
		const int srcValue = 2;

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
		index = storage->index_fileName();
	}

	utils::rm(mdb_test::test_page_name);
	utils::rm(index);
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
	auto index = page->index_fileName();
	page->close();

	utils::rm(mdb_test::test_page_name);
	utils::rm(index);
}


BOOST_AUTO_TEST_CASE(AppendMany) {
	const size_t pageSize = sizeof(storage::Page::Header) + sizeof(storage::Meas) * 10;
	Page::PPage page = Page::Create(mdb_test::test_page_name, pageSize);

	size_t arr_size = 15;
	storage::Meas::PMeas array = new storage::Meas[arr_size];
	for (size_t i = 0; i < arr_size; ++i) {
		array[i].id = i;
		array[i].time = i;
	}
	size_t writed = page->append(array, arr_size);
	delete[] array;
	BOOST_CHECK_EQUAL(writed, 10);

	for (size_t i = 0; i < writed; ++i) {
		storage::Meas readed;
		page->read(&readed, i);
		BOOST_CHECK_EQUAL(readed.id, i);
	}

	BOOST_CHECK_EQUAL(page->minTime(), 0);
	BOOST_CHECK_EQUAL(page->maxTime(), arr_size - 1);
	auto index = page->index_fileName();
	page->close();
	utils::rm(mdb_test::test_page_name);
	utils::rm(index);
}

BOOST_AUTO_TEST_CASE(PagereadIntervalFltr) {
	const int TestableMeasCount = 1000;
	std::string index = "";
	{
		{
			Page::PPage storage = Page::Create(mdb_test::test_page_name, mdb_test::sizeInMb10);
		}

		Page::PPage storage = Page::Open(mdb_test::test_page_name);
		for (int i = 0; i < TestableMeasCount; ++i) {

			auto newMeas = storage::Meas::empty();
			newMeas->value = i;
			newMeas->id = i % 10;
			newMeas->flag = (storage::Flag) (i % 5);
			newMeas->source = (storage::Flag) (i % 5);
			newMeas->time = i;
			storage->append(newMeas);
			delete newMeas;
		}

		{
			IdArray ids = { 1, 2, 3 };
			auto readRes = storage->readInterval(ids, 3, 3, 0, TestableMeasCount);

			BOOST_CHECK(readRes.size() != 0);
			for (auto it = readRes.cbegin(); it != readRes.cend(); ++it) {

				BOOST_CHECK(it->id != 4 && it->id != 5);
				BOOST_CHECK(it->flag == 3);
				BOOST_CHECK(it->source == 3);
				BOOST_CHECK(it->time <= TestableMeasCount && it->time >= 0);
			}
		}

		{
			IdArray ids = { 1, 2, 3, 4, 5 };
			auto readRes = storage->readInterval(ids, 0, 0, 0, TestableMeasCount);

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
				BOOST_CHECK(it->time <= TestableMeasCount && it->time >= 0);
			}

			BOOST_CHECK(haveSource);
			BOOST_CHECK(haveFlag);
			index = storage->index_fileName();
		}
	}

	utils::rm(mdb_test::test_page_name);
	utils::rm(index);
}