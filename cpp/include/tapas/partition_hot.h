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

template <CELL_TEMPLATE_PARAMS>    
class PartitionHOT {
 private:
  const int max_nb_;
  const int max_depth_;
  const int depth_bit_width_;
  
 public:
  PartitionHOT(int max_nb, int max_depth):
      max_nb_(max_nb), max_depth_(max_depth),
      depth_bit_width_(CalcMinBitLen(max_depth)) {}
  Cell<CELL_TEMPLATE_ARGS> *operator()(BT *b, index_t nb,
                                       const Region<DIM, FP> &r);
 private:
  void Refine(CELL *c,
              const hot::HelperNode<DIM> *hn,
              const BT *b,
              int cur_depth,
              KeyType cur_key) const;

  
}; // class PartitionHOT


} // namespace tapas

template <CELL_TEMPLATE_PARAMS_NO_DEF>
tapas::Cell<CELL_TEMPLATE_ARGS> *tapas::PartitionHOT<CELL_TEMPLATE_ARGS>::operator()(
    BT *b, index_t nb,
    const Region<DIM, FP> &r) {

  BT *b_work = new BT[nb];
  //const int depth_bit_width = CalcMinBitLen(max_depth);
  hot::HelperNode<DIM> *hn =
      hot::CreateInitialNodes<DIM, FP, BT, POS_OFFSET>(
          b, nb, r, max_depth_);

  hot::SortNodes<DIM>(hn, nb);
  hot::SortBodies<DIM, BT>(b, b_work, hn, nb);
  std::memcpy(b, b_work, sizeof(BT) * nb);

  KeyType root_key = 0;
  KeyPair kp = hot::GetBodyRange(root_key, hn, b, 0, nb, max_depth_, depth_bit_width_);
  TAPAS_LOG_DEBUG() << "Root range: offset: " << kp.first << ", length: " << kp.second << "\n";

  auto *root = new Cell<CELL_TEMPLATE_ARGS>(root_key, r, 0, nb);
  Refine(root, hn, b, 0, 0);
  
  return root;
}

template <CELL_TEMPLATE_PARAMS_NO_DEF>
void tapas::PartitionHOT<CELL_TEMPLATE_ARGS>::Refine(CELL *c,
                                                     const hot::HelperNode<DIM> *hn,
                                                     const BT *b,
                                                     int cur_depth,
                                                     KeyType cur_key) const {
  TAPAS_LOG_DEBUG() << "Current depth: " << cur_depth << std::endl;
  if (c->nb() <= max_nb_) {
    TAPAS_LOG_DEBUG() << "Small enough cell" << std::endl;
    return;
  }
  if (cur_depth > max_depth_) {
    TAPAS_LOG_DEBUG() << "Reached maximum depth" << std::endl;
    return;
  }
  KeyType child_key = hot::MortonKeyFirstChild<DIM>(cur_key, max_depth_, depth_bit_width_);
  index_t cur_offset = c->bid();
  index_t cur_len = c->nb();
  for (int i = 0; i < (1 << DIM); ++i) {
    TAPAS_LOG_DEBUG() << "Child key: " << child_key << std::endl; 
    index_t child_bn = hot::GetBodyNumber(child_key, hn, b, cur_offset, cur_len,
                                          max_depth_, depth_bit_width_);
    TAPAS_LOG_DEBUG() << "Range: offset: " << cur_offset << ", length: "
                      << child_bn << "\n";
    auto child_r = c->region().PartitionBSP(i);
    auto *child_cell = new Cell<CELL_TEMPLATE_ARGS>(
        child_key, child_r, cur_offset, child_bn);
    TAPAS_LOG_DEBUG() << "Particles: \n";
    tapas::debug::PrintBodies<DIM, FP, BT, POS_OFFSET>(b+cur_offset, child_bn, std::cerr);
    Refine(child_cell, hn, b, cur_depth+1, child_key);
    child_key = hot::CalcMortonKeyNext<DIM>(child_key, max_depth_, depth_bit_width_);
    cur_offset = cur_offset + child_bn;
    cur_len = cur_len - child_bn;
  }
}

#undef CELL_TEMPLATE_PARAMS
#undef CELL_TEMPLATE_PARAMS_NO_DEF
#undef CELL_TEMPLATE_ARGS
#undef CELL

#endif // TAPAS_PARTITION_H_
