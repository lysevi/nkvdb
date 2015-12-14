#include "meas.h"
#include "time_utils.h"
#include <stdlib.h>
#include <string.h>

using namespace nkvdb;

Meas::Meas() { 
    memset(this, 0, sizeof(Meas));
}

Meas Meas::empty() {
  return Meas{};
}

void Meas::readFrom(const Meas::PMeas m) {
    memcpy(this, m, sizeof(Meas));
}

bool nkvdb::checkPastTime(const Time t, const Time past_time) { // |current time - t| < m_past_time
  if (past_time == 0) {
    return true;
  } else {
    Time cur_t = nkvdb::TimeWork::CurrentUtcTime();
    auto delta = cur_t - t;

    return delta <= past_time;
  }
}


void nkvdb::Reader::readAll(Meas::MeasList*output) {
    while (!isEnd()) {
        this->readNext(output);
    }
}


Reader_ptr nkvdb::MetaStorage::readInterval(Time from, Time to){
    static IdArray empty{};
    return this->readInterval(empty,0,0,from,to);
}

Reader_ptr nkvdb::MetaStorage::readInTimePoint(Time time_point){
    static IdArray empty{};
    return this->readInTimePoint(empty,0,0,time_point);
}

append_result nkvdb::MetaStorage::append(const Meas::MeasArray& ma) 
{
	if (ma.size() > 0) {
		auto pm = (nkvdb::Meas::PMeas)ma.data();
		return this->append(pm, ma.size());
	} else {
		return append_result::empty();
	}
}

append_result nkvdb::MetaStorage::append(const Meas::MeasList& ml) {
	if (ml.size() > 0) {
		append_result result{};
		for (auto &m:ml) {
			auto res = this->append(m);
			result =result + res;
		}
		return result;
	} else {
		return append_result::empty();
	}

}
