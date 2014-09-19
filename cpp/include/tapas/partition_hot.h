#ifndef TAPAS_PARTITION_HOT_H_
#define TAPAS_PARTITION_HOT_H_

#include "tapas/common.h"
#include "tapas/basic_types.h"
#include "tapas/partition.h" 
#include "tapas/tapas.h" // class Cell
#include "tapas/hot.h" // needs refactoring
#include "tapas/logging.h"
#include "tapas/debug_util.h"

#define CELL_TEMPLATE_PARAMS \
  int DIM, class FP, class BT, int POS_OFFSET, class BT_ATTR, class ATTR=float
#define CELL_TEMPLATE_PARAMS_NO_DEF \
  int DIM, class FP, class BT, int POS_OFFSET, class BT_ATTR, class ATTR
#define CELL_TEMPLATE_ARGS \
  DIM, FP, BT, POS_OFFSET, BT_ATTR, ATTR

#define CELL Cell<CELL_TEMPLATE_ARGS>

namespace tapas {


} // namespace tapas


#undef CELL_TEMPLATE_PARAMS
#undef CELL_TEMPLATE_PARAMS_NO_DEF
#undef CELL_TEMPLATE_ARGS
#undef CELL

#endif // TAPAS_PARTITION_H_
