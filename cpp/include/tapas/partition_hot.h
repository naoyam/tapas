#ifndef TAPAS_PARTITION_HOT_H_
#define TAPAS_PARTITION_HOT_H_

#include "tapas/common.h"
#include "tapas/basic_types.h"
#include "tapas/partition.h" 
#include "tapas/tapas.h" // class Cell
#include "tapas/hot.h" // needs refactoring
#include "tapas/logging.h"

#define CELL_TEMPLATE_PARAMS \
  int DIM, class FP, class BT, int POS_OFFSET, class BT_ATTR, class ATTR=float
#define CELL_TEMPLATE_PARAMS_NO_DEF \
  int DIM, class FP, class BT, int POS_OFFSET, class BT_ATTR, class ATTR
#define CELL_TEMPLATE_ARGS \
  DIM, FP, BT, POS_OFFSET, BT_ATTR, ATTR

#define CELL Cell<CELL_TEMPLATE_ARGS>

namespace tapas {

template <CELL_TEMPLATE_PARAMS>    
class PartitionHOT {
 public:
Cell<CELL_TEMPLATE_ARGS> *operator()(BT *b, index_t nb,
                                     const Region<DIM, FP> &r,
                                     int max_nb, int max_depth);
  
 private:
  void Refine(CELL *c, const int max_nb, const int max_depth,
              const int cur_depth) const;
}; // class PartitionHOT

} // namespace tapas

template <CELL_TEMPLATE_PARAMS_NO_DEF>
tapas::Cell<CELL_TEMPLATE_ARGS> *tapas::PartitionHOT<CELL_TEMPLATE_ARGS>::operator()(
    BT *b, index_t nb,
    const Region<DIM, FP> &r,
    int max_nb, int max_depth) {

  BT *b_work = new BT[nb];
  const int depth_bit_width = CalcMinBitLen(max_depth);
  hot::HelperNode<DIM> *hn =
      hot::CreateInitialNodes<DIM, FP, BT, POS_OFFSET>(
          b, nb, r, max_depth);

  hot::SortNodes<DIM>(hn, nb);
  hot::SortBodies<DIM, BT>(b, b_work, hn, nb);
  std::memcpy(b, b_work, sizeof(BT) * nb);

  KeyType root_key = 0;
  KeyPair kp = hot::FindBodyRange(root_key, hn, b, 0, nb, max_depth, depth_bit_width);
  TAPAS_LOG_DEBUG() << "Root range: offset: " << kp.first << ", length: " << kp.second << "\n";

  auto *root = new Cell<CELL_TEMPLATE_ARGS>(r, 0, nb);
  Refine(root, max_nb, max_depth, 0);
  
  return root;
}

template <CELL_TEMPLATE_PARAMS_NO_DEF>
void tapas::PartitionHOT<CELL_TEMPLATE_ARGS>::Refine(CELL *c,
                                                     const int max_nb,
                                                     const int max_depth,
                                                     const int cur_depth) const {
  if (c->nb() <= max_nb) {
    TAPAS_LOG_DEBUG() << "Small enough cell" << std::endl;
    return;
  }
  if (cur_depth > max_depth) {
    TAPAS_LOG_DEBUG() << "Reached maximum depth" << std::endl;
    return;
  }
  
}

#undef CELL_TEMPLATE_PARAMS
#undef CELL_TEMPLATE_PARAMS_NO_DEF
#undef CELL_TEMPLATE_ARGS
#undef CELL

#endif // TAPAS_PARTITION_H_
