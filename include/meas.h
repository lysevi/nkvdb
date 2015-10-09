#pragma once

#include <memory>
#include <vector>
#include <list>
#include <ctime>
#include <set>

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
}
