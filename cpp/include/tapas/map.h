#ifndef TAPAS_MAP_H_
#define TAPAS_MAP_H_

#include "tapas/cell.h"

#define CELL_TEMPLATE_PARAMS \
  int DIM, class FP, class BT, int POS_OFFSET, class BT_ATTR, class ATTR=float
#define CELL_TEMPLATE_PARAMS_NO_DEF \
  int DIM, class FP, class BT, int POS_OFFSET, class BT_ATTR, class ATTR
#define CELL_TEMPLATE_ARGS \
  DIM, FP, BT, POS_OFFSET, BT_ATTR, ATTR


namespace tapas {

template <CELL_TEMPLATE_PARAMS, class T1, class T2, class... Args>
void Map(void (*f)(Cell<CELL_TEMPLATE_ARGS>&, Cell<CELL_TEMPLATE_ARGS>&, Args...), ProductIterator<T1, T2> prod, Args...args) {
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
#if 0
template <CELL_TEMPLATE_PARAMS, class... Args>
void Map(void (*f)(const PT &p1, PT_ATTR &a1, const PT &p2, PT_ATTR &a2, Args...),
         ProductIterator<ParticleIterator<CELL_TEMPLATE_ARGS>, ParticleIterator<CELL_TEMPLATE_ARGS> >
         iter, Args...args) {
  for (unsigned i = 0; i < iter.size(); ++i) {
    f(iter.first(), iter.attr_first(), iter.second(), iter.attr_second(), args...);
    ++iter;
  }
}

template <CELL_TEMPLATE_PARAMS, class... Args>
void Map(void (*f)(const PT &p1, PT_ATTR &a1, const PT &p2, Args...),
         ProductIterator<ParticleIterator<CELL_TEMPLATE_ARGS>, ParticleIterator<CELL_TEMPLATE_ARGS> >
         iter, Args...args) {
  for (unsigned i = 0; i < iter.size(); ++i) {
    f(iter.first(), iter.attr_first(), iter.second(), args...);
    ++iter;
  }
}
#else
template <CELL_TEMPLATE_PARAMS, class... Args>
void Map(void (*f)(ParticleIterator<CELL_TEMPLATE_ARGS> &p1, Args...),
         ParticleIterator<CELL_TEMPLATE_ARGS> iter, Args...args) {
  for (index_t i = 0; i < iter.size(); ++i) {
    f(iter, args...);
    ++iter;
  }
}
template <CELL_TEMPLATE_PARAMS, class... Args>
void Map(void (*f)(ParticleIterator<CELL_TEMPLATE_ARGS> &p1,
                   ParticleIterator<CELL_TEMPLATE_ARGS> &p2, Args...),
         ProductIterator<ParticleIterator<CELL_TEMPLATE_ARGS>, ParticleIterator<CELL_TEMPLATE_ARGS> >
         iter, Args...args) {
  for (unsigned i = 0; i < iter.size(); ++i) {
    f(iter.first(), iter.second(), args...);
    ++iter;
  }
}
#endif

#if 0
template <CELL_TEMPLATE_PARAMS, class... Args>
void Map(void (*f)(const PT &p1, PT_ATTR &a1, Args...),
         ParticleIterator<CELL_TEMPLATE_ARGS> iter, Args...args) {
  for (unsigned i = 0; i < iter.size(); ++i) {
    f(iter.first(), iter.attr_first(), args...);
    ++iter;
  }
}

template <CELL_TEMPLATE_PARAMS, class... Args>
void Map(void (*f)(const PT &p1, Args...),
         ParticleIterator<CELL_TEMPLATE_ARGS> iter, Args...args) {
  for (unsigned i = 0; i < iter.size(); ++i) {
    f(iter.first(), args...);
    ++iter;
  }
}
#endif

} // namespace tapas

#undef CELL_TEMPLATE_PARAMS
#undef CELL_TEMPLATE_PARAMS_NO_DEF
#undef CELL_TEMPLATE_ARGS

#endif // TAPAS_MAP_H_
