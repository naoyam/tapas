#ifndef TAPAS_MAP_H_
#define TAPAS_MAP_H_

#include "thread.h"
#include "tapas/cell.h"
#include "tapas/iterator.h"

namespace tapas {

const int THRESHOLD = 1;

template<class T>
struct ClassCheck { static const bool value = false; };

template<class CellType>
struct ClassCheck<BodyIterator<CellType>> { static const bool value = true; };

template<class T1_Iter, class T2_Iter>
struct AllowMutual {
    static bool value(T1_Iter t1, T2_Iter t2) {
        return false;
    }
};

template<class T1_Iter>
struct AllowMutual<T1_Iter, T1_Iter> {
    static bool value(T1_Iter iter1, T1_Iter iter2) {
        return iter1.AllowMutualInteraction(iter2);
    }
};

template<class T1_Iter, class T2_Iter, class Funct, class...Args>
static void product_map(T1_Iter iter1, int beg1, int end1,
                        T2_Iter iter2, int beg2, int end2,
                        Funct f, Args... args) {
    assert(beg1 < end1 && beg2 < end2);
    if (end1 - beg1 <= THRESHOLD || end2 - beg2 <= THRESHOLD) {
        for(int i = beg1; i < end1; i++) {
            for(int j = beg2; j < end2; j++) {
                bool am = AllowMutual<T1_Iter, T2_Iter>::value(iter1, iter2);
                if ((am && i <= j) || !am) {
                    f(*(iter1+i), *(iter2+j), args...);
                }
            }
        }
    } else {
        int mid1 = (end1 + beg1) / 2;
        int mid2 = (end2 + beg2) / 2;
        // run (beg1,mid1) x (beg2,mid2) and (mid1,end1) x (mid2,end2) in parallel
        {
            mk_task_group;
            create_taskA(product_map(iter1, beg1, mid1, iter2, beg2, mid2, f, args...));
            create_taskA(product_map(iter1, mid1, end1, iter2, mid2, end2, f, args...));
            wait_tasks;
        }
        {
            mk_task_group;
            create_taskA(product_map(iter1, beg1, mid1, iter2, mid2, end2, f, args...));
            create_taskA(product_map(iter1, mid1, end1, iter2, beg2, mid2, f, args...));
            wait_tasks;
        }
    }
}

#define USE_NEW_PRODUCT_MAP

/**
 * Map function f over product of two iterators
 */
template <class Funct, class T1_Iter, class T2_Iter, class... Args>
void Map(Funct f, ProductIterator<T1_Iter, T2_Iter> prod, Args...args) {
  TAPAS_LOG_DEBUG() << "map product iterator size: "
                    << prod.size() << std::endl;
#ifdef USE_NEW_PRODUCT_MAP
  product_map(prod.t1_, 0, prod.t1_.size(),
              prod.t2_, 0, prod.t2_.size(),
              f, args...);
#else
  for (index_t i = 0; i < prod.size(); ++i) {
      f(prod.first(), prod.second(), args...);
      prod++;
  }
#endif
}
  
template <class Funct, class T1_Iter, class...Args>
void Map(Funct f, ProductIterator<T1_Iter> prod, Args...args) {
  TAPAS_LOG_DEBUG() << "map product iterator size: "
                    << prod.size() << std::endl;
#ifdef USE_NEW_PRODUCT_MAP
  product_map(prod.t1_, 0, prod.t1_.size(),
              prod.t2_, 0, prod.t2_.size(),
              f, args...);
#else
  for (index_t i = 0; i < prod.size(); ++i) {
      f(prod.first(), prod.second(), args...);
      prod++;
  }
#endif
}

template <class Funct, class T, class... Args>
void Map(Funct f, SubCellIterator<T> iter, Args...args) {
    TAPAS_LOG_DEBUG() << "map non-product subcell iterator size: "
                      << iter.size() << std::endl;
    mk_task_group;
    for (int i = 0; i < iter.size(); i++) {
        create_task0(f(*iter, args...));
        iter++;
    }
    wait_tasks;
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
