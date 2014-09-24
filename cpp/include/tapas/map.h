#ifndef TAPAS_MAP_H_
#define TAPAS_MAP_H_

#include "tapas/cell.h"
#include "tapas/iterator.h"

namespace tapas {

template <class T1, class T2, class T1_Iter, class T2_Iter, class... Args>
void Map(void (*f)(T1 &, T2 &, Args...),
         ProductIterator<T1_Iter, T2_Iter> prod, Args...args) {
  TAPAS_LOG_DEBUG() << "map product iterator size: "
                    << prod.size() << std::endl;  
  for (unsigned i = 0; i < prod.size(); ++i) {
    f(prod.first(), prod.second(), args...);
    prod++;
  }
}

template <class T1, class T1_Iter, class... Args>
void Map(void (*f)(T1 &, T1 &, Args...),
         ProductIterator<T1_Iter> prod, Args...args) {
  TAPAS_LOG_DEBUG() << "map product iterator size: "
                    << prod.size() << std::endl;  
  for (unsigned i = 0; i < prod.size(); ++i) {
    f(prod.first(), prod.second(), args...);
    prod++;
  }
}


template <class T, class T_Iter, class... Args>
void Map(void (*f)(T &, Args...), T_Iter iter, Args...args) {
  TAPAS_LOG_DEBUG() << "map non-product iterator size: "
                    << iter.size() << std::endl;  
  for (unsigned i = 0; i < iter.size(); ++i) {
    f(*iter, args...);
    iter++;
  }
}

template <class T, class... Args>
void Map(void (*f)(T &, Args...), T &x, Args...args) {
  TAPAS_LOG_DEBUG() << "map non-iterator" << std::endl;
  f(x, args...);
}

} // namespace tapas

#endif // TAPAS_MAP_H_
