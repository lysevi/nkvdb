
#pragma once
#include <common/common.h>
#include <common/Meas.h>
#include <utils/LinearCache.h>
#include <utils/DataStorage.h>
#include <utils/ExternalCacheMemory.h>

namespace mdb_test {
	const size_t testCacheSz = 1 * 1024 * 1024; //1 mb

    common::IdArray getIds();
	utils::IExternalCache::PExternalCache makeDS();

    common::Meas::MeasArray testMeases(const common::IdArray &ids);
}