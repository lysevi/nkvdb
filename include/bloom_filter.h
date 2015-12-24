#pragma once

namespace nkvdb {

template<typename T>
struct Bloom{
    static T empty(){
        return T{};
    }

    static T  add(const T&fltr, const T&val){
        return fltr|val;
    }

    static bool check(const T&fltr, const T&val){
        return (fltr&val)==val;
    }
};

}
