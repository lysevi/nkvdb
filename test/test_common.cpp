#include "test_common.h"

namespace mdb_test {

    common::IdArray getIds() {
        common::IdArray result{1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
        return result;
    }

    utils::DataStorage* makeDS() {
		auto ids = mdb_test::getIds();
        utils::LinearCache* lcache = new utils::LinearCache(ids);
        utils::ExternalCacheMemory*excache = new utils::ExternalCacheMemory(100*1024*1024,ids);
        utils::DataStorage* result = new utils::DataStorage(lcache, excache);
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