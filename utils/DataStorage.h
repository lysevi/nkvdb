
#pragma once
#include "LinearCache.h"
#include "IExternalCache.h"
#include <boost/noncopyable.hpp>

namespace utils {

    class DataStorage {
    public:
        DataStorage(LinearCache *lcache,IExternalCache*ecache);
        virtual ~DataStorage();
    private:
        LinearCache   *m_lcache;
        IExternalCache*m_ecache;
    };

}