#ifndef TAPAS_MAP_H_
#define TAPAS_MAP_H_

#include "tapas/cell.h"
#include "tapas/iterator.h"

#define CELL_TEMPLATE_PARAMS \
  int DIM, class FP, class BT, class BT_ATTR, class ATTR=tapas::NONE
#define CELL_TEMPLATE_PARAMS_NO_DEF \
  int DIM, class FP, class BT, class BT_ATTR, class ATTR
#define CELL_TEMPLATE_ARGS \
  DIM, FP, BT, BT_ATTR, ATTR
#define BODY_ITERATOR_ARGS DIM, BT, BT_ATTR, CELL
#define BODY_ITERATOR BodyIterator<BODY_ITERATOR_ARGS>

namespace tapas {

template <CELL_TEMPLATE_PARAMS, class T1, class T2, class... Args>
void Map(void (*f)(Cell<CELL_TEMPLATE_ARGS>&, Cell<CELL_TEMPLATE_ARGS>&, Args...),
         ProductIterator<T1, T2> prod, Args...args) {
  for (unsigned i = 0; i < prod.size(); ++i) {
    f(prod.first(), prod.second(), args...);
    ++prod;
  }
}

template <class CellType, class T1, class T2, class... Args>
void Map(void (*f)(CellType &, CellType &, Args...),
         ProductIterator<T1, T2> prod, Args...args) {
  for (unsigned i = 0; i < prod.size(); ++i) {
    f(prod.first(), prod.second(), args...);
    ++prod;
  }
}

template <CELL_TEMPLATE_PARAMS, class T, class... Args>
void Map(void (*f)(Cell<CELL_TEMPLATE_ARGS>&, Args...), T iter, Args...args) {
  for (unsigned i = 0; i < iter.size(); ++i) {
    f(*iter, args...);
    ++iter;
  }
}

template <CELL_TEMPLATE_PARAMS, class CELL, class... Args>
void Map(void (*f)(BODY_ITERATOR &p1, Args...),
         BODY_ITERATOR iter, Args...args) {
  for (index_t i = 0; i < iter.size(); ++i) {
    f(iter, args...);
    ++iter;
  }
}
template <CELL_TEMPLATE_PARAMS, class CELL, class... Args>
void Map(void (*f)(BODY_ITERATOR &p1, BODY_ITERATOR &p2, Args...),
         ProductIterator<BODY_ITERATOR, BODY_ITERATOR >
         iter, Args...args) {
  for (unsigned i = 0; i < iter.size(); ++i) {
    f(iter.first(), iter.second(), args...);
    ++iter;
  }
}

} // namespace tapas

#undef CELL_TEMPLATE_PARAMS
#undef CELL_TEMPLATE_PARAMS_NO_DEF
#undef CELL_TEMPLATE_ARGS
#undef BODY_ITERATOR_ARGS
#undef BODY_ITERATOR

#endif // TAPAS_MAP_H_
