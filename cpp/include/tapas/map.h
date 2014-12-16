#ifndef TAPAS_MAP_H_
#define TAPAS_MAP_H_

#include "tapas/cell.h"
#include "tapas/iterator.h"

#ifdef MTHREAD
# ifndef TO_MTHREAD_NATIVE
#  define TO_MTHREAD_NATIVE 1
# endif
# include <tpswitch/tpswitch.h>
#endif

namespace tapas {

/**
 * Map function f over product of two iterators
 */
template <class Funct, class T1_Iter, class T2_Iter, class... Args>
void Map(Funct f, ProductIterator<T1_Iter, T2_Iter> prod, Args...args) {
  TAPAS_LOG_DEBUG() << "map product iterator size: "
                    << prod.size() << std::endl;
#if MTHREAD
  mk_task_group;
  for (int i = 0; i < prod.size(); i++) {
    create_taskA( f(prod.first(), prod.second(), args...) );
    prod++;
  }
  wait_tasks;
#else
  for (index_t i = 0; i < prod.size(); ++i) {
    f(prod.first(), prod.second(), args...);
    prod++;
  }
#endif
}
  
template <class Funct, class T1_Iter, class... Args>
void Map(Funct f, ProductIterator<T1_Iter> prod, Args...args) {
  TAPAS_LOG_DEBUG() << "map product iterator size: "
                    << prod.size() << std::endl;
#ifdef MTHREAD
  mk_task_group;
  for (int i = 0; i < prod.size(); i++) {
    create_taskA( f(prod.first(), prod.second(), args...) );
    prod++;
  }
  wait_tasks;
#else
  for (int i = 0; i < prod.size(); ++i) {
    f(prod.first(), prod.second(), args...);
    prod++;
  }
#endif
}

template <class Funct, class T, class... Args>
void Map(Funct f, SubCellIterator<T> iter, Args...args) {
  TAPAS_LOG_DEBUG() << "map non-product subcell iterator size: "
                    << iter.size() << std::endl;
#ifdef MTHREAD
  mk_task_group;
  for (int i = 0; i < iter.size(); i++) {
    create_task0(f(*iter, args...));
    iter++;
  }
  wait_tasks;
#else
  for (int i = 0; i < iter.size(); ++i) {
    f(*iter, args...);
    iter++;
  }
#endif
}
  
template <class Funct, class T, class... Args>
void Map(Funct f, BodyIterator<T> iter, Args...args) {
  TAPAS_LOG_DEBUG() << "map non-product body iterator size: "
                    << iter.size() << std::endl;  
  for (int i = 0; i < iter.size(); ++i) {
    f(*iter, args...);
    iter++;
  }
}

template <class Funct, class T, class... Args>
void Map(Funct f, T &x, Args...args) {
  TAPAS_LOG_DEBUG() << "map non-iterator" << std::endl;
  f(x, args...);
}

} // namespace tapas

#endif // TAPAS_MAP_H_
