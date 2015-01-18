
#pragma once

#include <common/Meas.h>
#include <common/common.h>

class IExternalCache {
public:
    virtual void writeValues(const common::Meas::MeasArray &meases)=0;
    virtual void readValues(const common::Meas::MeasArray &meases)=0;
};

