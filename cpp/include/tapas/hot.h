#ifndef TAPAS_HOT_
#define TAPAS_HOT_

#include "tapas/stdcbug.h"

#include <cstdlib>
#include <algorithm>
#include <list>
#include <vector>
#include <unordered_set>
#include <utility>
#include <iostream>

#include "tapas/cell.h"
#include "tapas/bitarith.h"
#include "tapas/logging.h"
#include "tapas/debug_util.h"

#define CELL_TEMPLATE_PARAMS \
  int DIM, class FP, class BT, class BT_ATTR, class ATTR=float
#define CELL_TEMPLATE_PARAMS_NO_DEF \
  int DIM, class FP, class BT, class BT_ATTR, class ATTR
#define CELL_TEMPLATE_ARGS \
  DIM, FP, BT, BT_ATTR, ATTR
#define CELL Cell<CELL_TEMPLATE_ARGS>

namespace tapas {

// REFAACTORING: This should not be located here
typedef int KeyType;
typedef std::list<KeyType> KeyList;
typedef std::vector<KeyType> KeyVector;
typedef std::unordered_set<KeyType> KeySet;
typedef std::pair<KeyType, KeyType> KeyPair;

template <class T>
void PrintKeys(const T &s, std::ostream &os) {
  tapas::StringJoin sj;
  for (auto k: s) {
    sj << k;
  }
  os << "Key set: " << sj << std::endl;
}

namespace hot {

template <int DIM>
struct HelperNode {
  KeyType key; // Morton key
  Vec<DIM, int> anchor;
  index_t p_index;
  index_t np;
};

template <int DIM, class FP, class BT>
HelperNode<DIM> *CreateInitialNodes(const typename BT::type *p, index_t np, 
                                    const Region<DIM, FP> &r,
                                    const int max_depth);

KeyType MortonKeyAppendDepth(KeyType k, int depth, const int depth_bit_width);

int MortonKeyGetDepth(KeyType k, const int depth_bit_width);

KeyType MortonKeyRemoveDepth(KeyType k, const int depth_bit_width);

int MortonKeyIncrementDepth(KeyType k, int inc, const int depth_bit_width);

template <int DIM>
bool MortonKeyIsDescendant(KeyType asc, KeyType dsc, const int max_depth,
                           const int depth_bit_width);

template <int DIM>
KeyType CalcMortonKey(const Vec<DIM, int> &anchor, const int max_depth,
                      const int depth_bit_width);
template <int DIM>
KeyType CalcMortonKeyNext(const KeyType k, const int max_depth,
                          const int depth_bit_width);

template <int DIM>
KeyType MortonKeyFirstChild(const KeyType k, const int max_depth,
                            const int depth_bit_width);


template <int DIM, class T>
void AppendChildren(KeyType k, T &s, const int max_depth, const int depth_bit_width);

template <int DIM>
void SortNodes(HelperNode<DIM> *nodes, index_t n);

template <int DIM, class BT>
void SortBodies(const typename BT::type *b, typename BT::type *sorted,
                const tapas::hot::HelperNode<DIM> *nodes,
                tapas::index_t nb);

template <int DIM>
KeyType FindFinestAncestor(KeyType x, KeyType y, const int max_depth,
                           const int depth_bit_width);

template <int DIM>
void CompleteRegion(KeyType x, KeyType y, KeyVector &s, const int max_depth);

template <int DIM>
index_t FindFirst(const KeyType k, const HelperNode<DIM> *hn,
                  const index_t offset, const index_t len);

template <int DIM, class BT>
KeyPair GetBodyRange(const KeyType k, const HelperNode<DIM> *hn,
                     const BT *b, index_t offset,
                     index_t len, int max_depth,
                     int depth_bit_width);

template <int DIM, class BT>
index_t GetBodyNumber(const KeyType k, const HelperNode<DIM> *hn,
                      const BT *b, index_t offset,
                      index_t len, int max_depth,
                      int depth_bit_width);

template <CELL_TEMPLATE_PARAMS>
class Cell: public tapas::Cell<CELL_TEMPLATE_ARGS> {
  KeyType key_;
 public:
  Cell(const Region<DIM, FP> &region, index_t bid, index_t nb,
       KeyType key):
      tapas::Cell<CELL_TEMPLATE_ARGS>(region, bid, nb), key_(key) {}
};

template <CELL_TEMPLATE_PARAMS>    
class Partition {
 private:
  const int max_nb_;
  const int max_depth_;
  const int depth_bit_width_;
  
 public:
  Partition(int max_nb, int max_depth):
      max_nb_(max_nb), max_depth_(max_depth),
      depth_bit_width_(CalcMinBitLen(max_depth)) {}
  Cell<CELL_TEMPLATE_ARGS> *operator()(typename BT::type *b, index_t nb,
                                       const Region<DIM, FP> &r);
 private:
  void Refine(CELL *c, const hot::HelperNode<DIM> *hn,
              const typename BT::type *b, int cur_depth,
              KeyType cur_key) const;
  
}; // class PartitionHOT

} // namespace hot
} // namespace tapas

inline
tapas::KeyType tapas::hot::MortonKeyAppendDepth(tapas::KeyType k, int depth,
                                                const int depth_bit_width) {
  k = (k << depth_bit_width) | depth;
  return k;
}

inline
int tapas::hot::MortonKeyGetDepth(tapas::KeyType k,
                                  const int depth_bit_width) {
  return k & ((1 << depth_bit_width) - 1);
}

inline
tapas::KeyType tapas::hot::MortonKeyRemoveDepth(tapas::KeyType k,
                                                const int depth_bit_width) {
  return k >> depth_bit_width;
}

inline
int tapas::hot::MortonKeyIncrementDepth(tapas::KeyType k, int inc,
                                        const int depth_bit_width) {
  int depth = MortonKeyGetDepth(k, depth_bit_width);
  ++depth;
  k = MortonKeyRemoveDepth(k, depth_bit_width);
  return MortonKeyAppendDepth(k, depth, depth_bit_width);
}

template <int DIM>
bool tapas::hot::MortonKeyIsDescendant(tapas::KeyType asc, tapas::KeyType dsc,
                                       const int max_depth, const int depth_bit_width) {
  int depth = MortonKeyGetDepth(asc, depth_bit_width);
  if (depth >= MortonKeyGetDepth(dsc, depth_bit_width)) return false;
  int s = (max_depth - depth) * DIM + depth_bit_width;
  asc >>= s;
  dsc >>= s;
  return asc == dsc;
}


template <int DIM>
tapas::KeyType tapas::hot::CalcMortonKey(const tapas::Vec<DIM, int> &anchor,
                                         const int max_depth,
                                         const int depth_bit_width) {
  KeyType k = 0;
  int mask = 1 << (max_depth - 1);
  for (int i = 0; i < max_depth; ++i) {
    for (int d = DIM-1; d >= 0; --d) {
      k = (k << 1) | ((anchor[d] & mask) >> (max_depth - i - 1));
    }
    mask >>= 1;
  }
  return MortonKeyAppendDepth(k, max_depth, depth_bit_width);
}

template <int DIM, class FP, class BT>
tapas::hot::HelperNode<DIM> *tapas::hot::CreateInitialNodes(
    const typename BT::type *p, index_t np,
    const Region<DIM, FP> &r,
    const int max_depth) {

  HelperNode<DIM> *nodes = new HelperNode<DIM>[np];
  FP num_cell = 1 << max_depth;
  Vec<DIM, FP> pitch;
  const int depth_bit_width = CalcMinBitLen(max_depth);
  for (int d = 0; d < DIM; ++d) {
    pitch[d] = (r.max()[d] - r.min()[d]) / num_cell;
  }
  for (index_t i = 0; i < np; ++i) {
    HelperNode<DIM> &node = nodes[i];
    node.p_index = i;
    node.np = 1;
    Vec<DIM, FP> off = ParticlePosOffset<DIM, FP, BT::pos_offset>::vec((const void*)&(p[i]));
    off -= r.min();
    off /= pitch;
    for (int d = 0; d < DIM; ++d) {
      node.anchor[d] = (int)(off[d]);
    }
#ifdef TAPAS_DEBUG
    assert(node.anchor >= 0);
    assert(node.anchor < (1 << max_depth));
#endif // TAPAS_DEBUG
    
    node.key = CalcMortonKey<DIM>(node.anchor, max_depth, depth_bit_width);
  }
  
  return nodes;
}

template <int DIM>
void tapas::hot::SortNodes(tapas::hot::HelperNode<DIM> *nodes, index_t n) {
  std::qsort(nodes, n, sizeof(HelperNode<DIM>),
             [] (const void *x, const void *y) {
               return static_cast<const HelperNode<DIM>*>(x)->key -
                   static_cast<const HelperNode<DIM>*>(y)->key;
             });
}

template <int DIM, class BT>
void tapas::hot::SortBodies(const typename BT::type *b, typename BT::type *sorted,
                            const tapas::hot::HelperNode<DIM> *sorted_nodes,
                            tapas::index_t nb) {
  for (index_t i = 0; i < nb; ++i) {
    sorted[i] = b[sorted_nodes[i].p_index];
  }
}

template <int DIM>
tapas::KeyType tapas::hot::FindFinestAncestor(tapas::KeyType x,
                                              tapas::KeyType y,
                                              const int max_depth,
                                              const int depth_bit_width) {
  int min_depth = std::min(MortonKeyGetDepth(x, depth_bit_width),
                           MortonKeyGetDepth(y, depth_bit_width));
  x >>= depth_bit_width;
  y >>= depth_bit_width;
  KeyType a = ~(x ^ y);
  int common_depth = 0;
  for (; common_depth < min_depth; ++common_depth) {
    KeyType t = (a >> (max_depth - common_depth -1) * DIM) & ((1 << DIM) - 1);
    if (t != ((1 << DIM) -1)) break;
  }
  int common_bit_len = common_depth * DIM;
  KeyType mask = ((1 << common_bit_len) - 1) << (max_depth * DIM - common_bit_len);
  return MortonKeyAppendDepth(x & mask, common_depth, depth_bit_width);
}

template <int DIM, class T>
void tapas::hot::AppendChildren(tapas::KeyType x, T &s,
                                const int max_depth, const int depth_bit_width) {
  int x_depth = MortonKeyGetDepth(x, depth_bit_width);
  int c_depth = x_depth + 1;
  if (c_depth > max_depth) return;
  x = MortonKeyIncrementDepth(x, 1, depth_bit_width);
  for (int i = 0; i < (1 << DIM); ++i) {
    int child_key = (i << ((max_depth - c_depth) * DIM + depth_bit_width));
    s.push_back(x | child_key);
    std::cout << "Adding child " << (x | child_key) << std::endl;
  }
}

// Note this doesn't return a valid morton key when the incremented
// key overflows the overall region, but should be fine for
// FindBodyRange method.
template <int DIM>
tapas::KeyType tapas::hot::CalcMortonKeyNext(const KeyType k,
                                             const int max_depth,
                                             const int depth_bit_width) {
  int d = MortonKeyGetDepth(k, depth_bit_width);
  KeyType inc = 1 << (DIM * (max_depth - d) + depth_bit_width);
  return k + inc;
}

template <int DIM>
tapas::KeyType tapas::hot::MortonKeyFirstChild(const KeyType k, const int max_depth,
                                               const int depth_bit_width) {
#ifdef TAPAS_DEBUG
  KeyType t = MortonKeyRemoveDepth(k, depth_bit_width);
  t = t & ~(~((KeyType)0) << (DIM * (max_depth - MortonKeyGetDepth(k, depth_bit_width))));
  assert(t == 0);
#endif  
  return MortonKeyIncrementDepth(k, 1, depth_bit_width);
}


template <int DIM>
void tapas::hot::CompleteRegion(tapas::KeyType x, tapas::KeyType y,
                                tapas::KeyVector &s,
                                const int max_depth) {
  int depth_bit_width = CalcMinBitLen(max_depth);
  KeyType fa = FindFinestAncestor<DIM>(x, y, max_depth, depth_bit_width);
  KeyList w;
  AppendChildren<DIM>(fa, w, max_depth, depth_bit_width);
  tapas::PrintKeys(w, std::cout);
  while (w.size() > 0) {
    KeyType k = w.front();
    w.pop_front();
    std::cout << "visiting " << k << std::endl;
    if ((k > x && k < y) && !MortonKeyIsDescendant<DIM>(k, y, max_depth,
                                                        depth_bit_width)) {
      s.push_back(k);
      std::cout << "Adding " << k << " to output set" << std::endl;
    } else if (MortonKeyIsDescendant<DIM>(k, x, max_depth, depth_bit_width) ||
               MortonKeyIsDescendant<DIM>(k, y, max_depth, depth_bit_width)) {
      std::cout << "Adding children of " << k << " to work set" << std::endl;      
      AppendChildren<DIM>(k, w, max_depth, depth_bit_width);

    }
  }
  std::sort(std::begin(s), std::end(s));
}

template <int DIM>
tapas::index_t tapas::hot::FindFirst(const KeyType k,
                                     const HelperNode<DIM> *hn,
                                     const index_t offset,
                                     const index_t len) {
  // Assume hn is softed by key
  // Searches the first element that is grether or equal to the key.
  // Returns len if not found, i.e., all elements are less than the
  // key.
  index_t pivot = len / 2;
  index_t i = offset + pivot;
  index_t cur;
  if (hn[i].key < k) {
    cur = offset + len;
    index_t rem_len = len - pivot - 1;
    if (rem_len > 0) {
      cur = std::min(cur, FindFirst(k, hn, i + 1, rem_len));
    }
  } else { // (hn[i].key >= k) {
    cur = i;
    index_t rem_len = pivot;
    if (rem_len > 0) {
      cur = std::min(cur, FindFirst(k, hn, offset, rem_len));
    }
  }
  return cur;
}


// Returns the range of bodies that are included in the cell specified
// by the given key. 
template <int DIM, class BT>
tapas::KeyPair tapas::hot::GetBodyRange(const tapas::KeyType k,
                                        const tapas::hot::HelperNode<DIM> *hn,
                                        const BT *b,
                                        index_t offset,
                                        index_t len,
                                        int max_depth,
                                        int depth_bit_width) {
  index_t body_begin = FindFirst(k, hn, offset, len);
  index_t body_num = GetBodyNumber(k, hn, b, body_begin,
                                   len-(body_begin-offset),
                                   max_depth, depth_bit_width);
  return std::make_pair(body_begin, body_num);
}

template <int DIM, class BT>
tapas::index_t tapas::hot::GetBodyNumber(const tapas::KeyType k,
                                         const tapas::hot::HelperNode<DIM> *hn,
                                         const BT *b,
                                         index_t offset,
                                         index_t len,
                                         int max_depth,
                                         int depth_bit_width) {
  //index_t body_begin = FindFirst(k, hn, offset, len);
  KeyType next_key = CalcMortonKeyNext<DIM>(k, max_depth, depth_bit_width);
  index_t body_end = FindFirst(next_key, hn, offset+1, len-1);
  return body_end - offset;
}

template <CELL_TEMPLATE_PARAMS_NO_DEF>
tapas::hot::Cell<CELL_TEMPLATE_ARGS> *tapas::hot::Partition<CELL_TEMPLATE_ARGS>::operator()(
    typename BT::type *b, index_t nb,
    const Region<DIM, FP> &r) {

  typename BT::type *b_work = new typename BT::type[nb];
  //const int depth_bit_width = CalcMinBitLen(max_depth);
  HelperNode<DIM> *hn =
      CreateInitialNodes<DIM, FP, BT>(b, nb, r, max_depth_);

  SortNodes<DIM>(hn, nb);
  SortBodies<DIM, BT>(b, b_work, hn, nb);
  std::memcpy(b, b_work, sizeof(BT) * nb);

  KeyType root_key = 0;
  KeyPair kp = hot::GetBodyRange(root_key, hn, b, 0, nb, max_depth_, depth_bit_width_);
  TAPAS_LOG_DEBUG() << "Root range: offset: " << kp.first << ", length: " << kp.second << "\n";

  auto *root = new Cell<CELL_TEMPLATE_ARGS>(r, 0, nb, root_key);
  Refine(root, hn, b, 0, 0);
  
  return root;
}

template <CELL_TEMPLATE_PARAMS_NO_DEF>
void tapas::hot::Partition<CELL_TEMPLATE_ARGS>::Refine(CELL *c,
                                                       const HelperNode<DIM> *hn,
                                                       const typename BT::type *b,
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
  KeyType child_key = MortonKeyFirstChild<DIM>(cur_key, max_depth_, depth_bit_width_);
  index_t cur_offset = c->bid();
  index_t cur_len = c->nb();
  for (int i = 0; i < (1 << DIM); ++i) {
    TAPAS_LOG_DEBUG() << "Child key: " << child_key << std::endl; 
    index_t child_bn = GetBodyNumber(child_key, hn, b, cur_offset, cur_len,
                                          max_depth_, depth_bit_width_);
    TAPAS_LOG_DEBUG() << "Range: offset: " << cur_offset << ", length: "
                      << child_bn << "\n";
    auto child_r = c->region().PartitionBSP(i);
    auto *child_cell = new Cell<CELL_TEMPLATE_ARGS>(
        child_r, cur_offset, child_bn, child_key);
    TAPAS_LOG_DEBUG() << "Particles: \n";
    tapas::debug::PrintBodies<DIM, FP, BT>(b+cur_offset, child_bn, std::cerr);
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

#endif // TAPAS_HOT_
