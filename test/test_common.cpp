#include "test_common.h"

namespace mdb_test {

    common::IdArray getIds() {
        common::IdArray result{1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
        return result;
    }

	utils::ICache::PCache makeDS() {
		auto ids = mdb_test::getIds();
		utils::ICache::PCache lcache(new utils::LinearCache(ids));
		utils::ICache::PCache excache(new utils::MemoryCache(testCacheSz,ids));
		utils::ICache::PCache result(new utils::DataStorage(lcache, excache));
        return result;
    }

    common::Meas::MeasArray testMeases(const common::IdArray &ids) {
        common::Meas::MeasArray ma;
        ma.resize(ids.size());
        int i = 0;
        for (auto id:ids) {
			auto m = std::make_shared<common::Meas>();
            m->id = id;
            m->time = id * 10;
            m->data = id;
            m->flag = id;
            ma[i] = m;
            i++;
        }
        return ma;
    }

}