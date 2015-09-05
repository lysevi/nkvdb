#include "Meas.h"
#include "time.h"

using namespace storage;

Meas::Meas() {
    id = time = source = flag = value = 0;
}


Meas::PMeas Meas::empty() {
    Meas::PMeas result = new Meas();
	return result;
}

void Meas::readFrom(const Meas::PMeas m){
    id=m->id;
    time=m->time;
    source=m->source;
    flag=m->flag;
    value=m->value;
}



bool storage::checkPastTime(const Time t, const Time past_time) { // |current time - t| < m_past_time
	if (past_time == 0) {
		return true;
	} else {
        Time cur_t = storage::TimeWork::CurrentUtcTime();
        auto delta=std::abs(cur_t - t);
        return  delta<=past_time;
	}
}
