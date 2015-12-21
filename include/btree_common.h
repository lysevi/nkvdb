#pragma once
#include <utility>
#include <iostream>
namespace trees{
    template<class T>
    void insert_to_array(T *array, size_t sz,size_t insert_pos,const T &value){ //sz => len(array), cap => capacity(array)
        for(auto i=sz-1;i>insert_pos;i--){
//            if(i==sz-1){
//                break;
//            }
            std::swap(array[i],array[i-1]);

            /*for(size_t pos=0;pos<sz;pos++){
                std::cout<<" * "<<array[pos];
            }
            std::cout<<std::endl;*/

        }
        array[insert_pos]=value;
//        for(size_t pos=0;pos<sz;pos++){
//            std::cout<<" * "<<array[pos];
//        }
//        std::cout<<std::endl;
    }
}
