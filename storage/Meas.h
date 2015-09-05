#pragma once

#include <memory>
#include <vector>
#include <list>
#include <ctime>
#include <utils/utils.h>

using namespace utils;

namespace storage {
typedef clock_t Time;
typedef uint64_t Id;
typedef uint64_t Flag;
typedef uint64_t Value;
typedef std::vector<Id> IdArray;

struct Meas {
  typedef Meas *PMeas;
  typedef std::vector<Meas> MeasArray;
  typedef std::list<Meas> MeasList;

  Meas();
  void readFrom(const Meas::PMeas m);
  static Meas::PMeas empty();

  Id id;
  Time time;
  Flag source;
  Flag flag;
  Value value;
};

bool checkPastTime(const Time t,
                   const Time past_time); // |current time - t| < past_time
}
