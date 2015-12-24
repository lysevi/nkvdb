#pragma once

namespace nkvdb {

template<typename T>
struct Bloom{
    T fltr;
    static Bloom create(const T&fltr){
        Bloom res{};
        res.fltr=fltr;
        return res;
    }

    static Bloom empty(){
        Bloom res{};
        res.fltr=T{};
        return res;
    }

    void  add(const T&val){
        fltr|=val;
    }

    bool check(const T&val){
        return (fltr&val)==val;
    }
};

}
