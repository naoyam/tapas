#ifndef TAPAS_PARTITION_H_
#define TAPAS_PARTITION_H_

#include "tapas/common.h"
#include "tapas/basic_types.h"
#include "tapas/tapas.h"

#define CELL_TEMPLATE_PARAMS \
  int DIM, class FP, class BT, int POS_OFFSET, class BT_ATTR, class ATTR=float
#define CELL_TEMPLATE_PARAMS_NO_DEF \
  int DIM, class FP, class BT, int POS_OFFSET, class BT_ATTR, class ATTR
#define CELL_TEMPLATE_ARGS \
  DIM, FP, BT, POS_OFFSET, BT_ATTR, ATTR

namespace tapas {

template <CELL_TEMPLATE_PARAMS>    
class Partition {
 public:
Cell<CELL_TEMPLATE_ARGS> *operator()(BT *b, index_t nb,
                                     const Region<DIM, FP> &r,
                                     int max_nb) {
  return NULL;
}
}; // class Partition

} // namespace tapas

#undef CELL_TEMPLATE_PARAMS
#undef CELL_TEMPLATE_PARAMS_NO_DEF
#undef CELL_TEMPLATE_ARGS

#endif // TAPAS_PARTITION_H_
