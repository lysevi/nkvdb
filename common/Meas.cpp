#include "Meas.h"

using namespace common;

Meas::Meas() {
	id = time = source = flag = size = 0;
	data = nullptr;
};

Meas::~Meas() {
};

Meas::PMeas Meas::empty() {
	Meas::PMeas result = std::make_shared<Meas>();
	return result;
}