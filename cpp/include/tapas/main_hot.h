#ifndef TAPAS_MAIN_HOT_H_
#define TAPAS_MAIN_HOT_H_

#include "tapas/main.h"
#include "tapas/hot.h"

namespace tapas {

class HOT {};

template <int DIM, class FP, class BT,
          class BT_ATTR, class CELL_ATTR>
class Tapas<DIM, FP, BT, BT_ATTR, CELL_ATTR, HOT> {
 public:
  typedef tapas::hot::Cell<DIM, FP, BT, BT_ATTR> Cell;
  typedef tapas::BodyIterator<DIM, FP, BT, BT_ATTR> BodyIterator;
  static Cell *Partition(typename BT::type *b,
                         index_t nb, const Region<DIM, FP> &r,
                         int max_nb) {
    tapas::hot::Partition<DIM, FP, BT, BT_ATTR> part(max_nb);
    return part(b, nb, r);
  }
};



} // namespace tapas

#endif // TAPAS_MAIN_HOT_H_
