#include "t.h"

template<typename T> 
void Test<T>::set_t(T t) {
    t_ = t;
}

template<typename T> 
T Test<T>::get_t() {
    return t_;
}




