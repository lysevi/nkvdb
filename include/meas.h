#pragma once

#include <memory>
#include <vector>
#include <list>
#include <ctime>
#include <set>
#include "common.h"

namespace nkvdb {
typedef uint64_t Time;
typedef uint64_t Id;
typedef uint64_t Flag;
typedef uint64_t Value;
typedef std::vector<Id> IdArray;
typedef std::set<nkvdb::Id> IdSet;

struct Meas {
    typedef Meas *PMeas;
    typedef std::vector<Meas> MeasArray;
    typedef std::list<Meas> MeasList;

    Meas();
    void readFrom(const Meas::PMeas m);
    static Meas empty();

    Id id;
    Time time;
    Flag source;
    Flag flag;
    Value value;
};

bool checkPastTime(const Time t, const Time past_time); // |current time - t| < past_time

class Reader
{
public:
    virtual bool isEnd() const=0;
    virtual void readNext(Meas::MeasList*output)=0;
    virtual void readAll(Meas::MeasList*output);
};

typedef std::shared_ptr<Reader> Reader_ptr;
class MetaStorage{
public:
	virtual ~MetaStorage() = default;
    /// min time of writed meas
    virtual Time minTime()=0;
    /// max time of writed meas
    virtual Time maxTime()=0;

    virtual append_result append(const Meas& value)=0;
	virtual append_result append(const Meas::MeasArray& ma);
	virtual append_result append(const Meas::MeasList& ml);
    virtual append_result append(const Meas::PMeas begin, const size_t size)=0;

    virtual Reader_ptr readInterval(Time from, Time to);
    virtual Reader_ptr readInterval(const IdArray &ids, nkvdb::Flag source, nkvdb::Flag flag, Time from, Time to)=0;

    virtual Reader_ptr readInTimePoint(Time time_point);
    virtual Reader_ptr readInTimePoint(const IdArray &ids, nkvdb::Flag source, nkvdb::Flag flag, Time time_point)=0;

};
}
