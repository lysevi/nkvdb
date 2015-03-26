#include "Meas.h"

using namespace storage;

Meas::Meas() {
	id = time = source = flag = data = 0;
};

Meas::~Meas() {
};

Meas::PMeas Meas::empty() {
	Meas::PMeas result = std::make_shared<Meas>();
	return result;
}