#include "test_common.h"

namespace mdb_test {

    common::IdArray getIds() {
        common::IdArray result{1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
        return result;
    }

    utils::DataStorage* makeDS() {
        utils::LinearCache* lcache = new utils::LinearCache(mdb_test::getIds());
        utils::ExternalCacheMemory*excache = new utils::ExternalCacheMemory();
        utils::DataStorage* result = new utils::DataStorage(lcache, excache);
        return result;
    }

}