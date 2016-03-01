#pragma once

namespace nkvdb {

	template<typename T>
	static T bloom_empty() {
        return T{};
    }

	template<typename T>
	static T  bloom_add(const T&fltr, const T&val) {
        return fltr|val;
    }

	template<typename T>
	static bool bloom_check(const T&fltr, const T&val) {
        return (fltr&val)==val;
    }


}
