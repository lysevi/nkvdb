
#pragma once
#include <common/common.h>
#include <common/Meas.h>
#include <utils/LinearCache.h>
#include <utils/DataStorage.h>
#include <utils/ExternalCacheMemory.h>

namespace mdb_test {

    common::IdArray getIds();
    utils::DataStorage* makeDS();
}