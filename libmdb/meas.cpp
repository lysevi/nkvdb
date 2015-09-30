#include "meas.h"
#include "time_utils.h"
#include <stdlib.h>
#include <string.h>

using namespace mdb;

Meas::Meas() { 
    memset(this, 0, sizeof(Meas));
}

Meas Meas::empty() {
  return Meas{};
}

void Meas::readFrom(const Meas::PMeas m) {
    memcpy(this, m, sizeof(Meas));
}

bool mdb::checkPastTime(const Time t, const Time past_time) { // |current time - t| < m_past_time
  if (past_time == 0) {
    return true;
  } else {
    Time cur_t = mdb::TimeWork::CurrentUtcTime();
    auto delta = cur_t - t;

    return delta <= past_time;
  }
}
