#include "Meas.h"

using namespace storage;

Meas::Meas() {
    id = time = source = flag = value = 0;
};


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
