#include "meas.h"
#include "time.h"
#include <stdlib.h>
#include <string.h>

using namespace storage;

Meas::Meas() { 
    memset(this, 0, sizeof(Meas));
}

Meas::PMeas Meas::empty() {
  Meas::PMeas result = new Meas();
  return result;
}

void Meas::readFrom(const Meas::PMeas m) {
    memcpy(this, m, sizeof(Meas));
}

bool storage::checkPastTime(const Time t, const Time past_time) { // |current time - t| < m_past_time
  if (past_time == 0) {
    return true;
  } else {
    Time cur_t = storage::TimeWork::CurrentUtcTime();
    auto delta = std::abs(cur_t - t);

    return delta <= past_time;
  }
}
