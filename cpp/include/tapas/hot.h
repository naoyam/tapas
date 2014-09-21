#ifndef TAPAS_HOT_
#define TAPAS_HOT_

#include "tapas/stdcbug.h"

#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <list>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <iostream>

#include "tapas/cell.h"
#include "tapas/bitarith.h"
#include "tapas/logging.h"
#include "tapas/debug_util.h"

#define CELL_TEMPLATE_PARAMS \
  int DIM, class FP, class BT, class BT_ATTR, class ATTR=tapas::NONE
#define CELL_TEMPLATE_PARAMS_NO_DEF \
  int DIM, class FP, class BT, class BT_ATTR, class ATTR
#define CELL_TEMPLATE_ARGS \
  DIM, FP, BT, BT_ATTR, ATTR
#define CELL Cell<CELL_TEMPLATE_ARGS>

#define DEPTH_BIT_WIDTH (3)
#define MAX_DEPTH ((1 << DEPTH_BIT_WIDTH) - 1)

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

using std::unordered_map;

template <int DIM>
struct HelperNode {
  KeyType key; // Morton key
  Vec<DIM, int> anchor;
  index_t p_index;
  index_t np;
};



template <int DIM, class FP, class BT>
HelperNode<DIM> *CreateInitialNodes(const typename BT::type *p, index_t np, 
                                    const Region<DIM, FP> &r);

KeyType MortonKeyAppendDepth(KeyType k, int depth);

int MortonKeyGetDepth(KeyType k);

KeyType MortonKeyRemoveDepth(KeyType k);

int MortonKeyIncrementDepth(KeyType k, int inc);

template <int DIM>
bool MortonKeyIsDescendant(KeyType asc, KeyType dsc);

template <int DIM>
KeyType CalcFinestMortonKey(const Vec<DIM, int> &anchor);
                      
template <int DIM>
KeyType CalcMortonKeyNext(KeyType k);

template <int DIM>
KeyType MortonKeyClearDescendants(KeyType k);

template <int DIM>
KeyType MortonKeyParent(KeyType k);

template <int DIM>
KeyType MortonKeyFirstChild(KeyType k);

template <int DIM>
KeyType MortonKeyChild(KeyType k, int child_idx);


template <int DIM, class T>
void AppendChildren(KeyType k, T &s);

template <int DIM>
void SortNodes(HelperNode<DIM> *nodes, index_t n);

template <int DIM, class BT>
void SortBodies(const typename BT::type *b, typename BT::type *sorted,
                const HelperNode<DIM> *nodes,
                tapas::index_t nb);

template <int DIM>
KeyType FindFinestAncestor(KeyType x, KeyType y);

template <int DIM>
void CompleteRegion(KeyType x, KeyType y, KeyVector &s);

template <int DIM>
index_t FindFirst(const KeyType k, const HelperNode<DIM> *hn,
                  const index_t offset, const index_t len);

template <int DIM, class BT>
KeyPair GetBodyRange(const KeyType k, const HelperNode<DIM> *hn,
                     const BT *b, index_t offset,
                     index_t len);

template <int DIM, class BT>
index_t GetBodyNumber(const KeyType k, const HelperNode<DIM> *hn,
                      const BT *b, index_t offset,
                      index_t len);


template <CELL_TEMPLATE_PARAMS_NO_DEF>    
class Partition;

template <CELL_TEMPLATE_PARAMS>
class Cell: public tapas::Cell<CELL_TEMPLATE_ARGS> {
  friend class Partition<CELL_TEMPLATE_ARGS>;
  friend class BodyIterator<DIM, BT, BT_ATTR, Cell>;
 public:
  typedef unordered_map<KeyType, Cell*> HashTable;
 protected:
  KeyType key_;
  HashTable ht_;
 public:

  Cell(const Region<DIM, FP> &region, index_t bid, index_t nb,
       KeyType key, HashTable &ht,
       typename BT::type *bodies, BT_ATTR *body_attrs):
      tapas::Cell<CELL_TEMPLATE_ARGS>(region, bid, nb), key_(key),
      ht_(ht), bodies_(bodies), body_attrs_(body_attrs),
      is_leaf_(true) {}

  typedef Cell value_type;
  typedef ATTR attr_type;
  Cell &operator*() {
    return *this;
  }
  const Cell &operator++() const {
    return *this;
  }

  
  KeyType key() const { return key_; }

  bool IsRoot() const;
  bool IsLeaf() const;
  int nsubcells() const;
  Cell &subcell(int idx) const;
  Cell &parent() const;
#ifdef DEPRECATED
  typename BT::type &particle(index_t idx) const {
    return body(idx);
  }
#endif
  typename BT::type &body(index_t idx) const;
  BodyIterator<DIM, BT, BT_ATTR, Cell> bodies() const;
#ifdef DEPRECATED
  BT_ATTR *particle_attrs() const {
    return body_attrs();
  }
#endif
  BT_ATTR *body_attrs() const;
  SubCellIterator<DIM, Cell> subcells() const;
  
 protected:
  BT_ATTR &attr(index_t idx) const;
  HashTable &ht() { return ht_; }
  Cell *Lookup(KeyType k) const;
  typename BT::type *bodies_;
  BT_ATTR *body_attrs_;
  bool is_leaf_;  
}; // class Cell

template <CELL_TEMPLATE_PARAMS>    
class Partition {
 private:
  const unsigned max_nb_;
  
 public:
  Partition(unsigned max_nb): max_nb_(max_nb) {}
      
  Cell<CELL_TEMPLATE_ARGS> *operator()(typename BT::type *b, index_t nb,
                                       const Region<DIM, FP> &r);
 private:
  void Refine(CELL *c, const hot::HelperNode<DIM> *hn,
              const typename BT::type *b, int cur_depth,
              KeyType cur_key) const;
  
}; // class PartitionHOT


inline
tapas::KeyType MortonKeyAppendDepth(tapas::KeyType k, int depth) {
  k = (k << DEPTH_BIT_WIDTH) | depth;
  return k;
}

inline
int MortonKeyGetDepth(tapas::KeyType k) {
  return k & ((1 << DEPTH_BIT_WIDTH) - 1);
}

inline
tapas::KeyType MortonKeyRemoveDepth(tapas::KeyType k) {
  return k >> DEPTH_BIT_WIDTH;
}

inline
int MortonKeyIncrementDepth(tapas::KeyType k, int inc) {
  int depth = MortonKeyGetDepth(k);
  depth += inc;
#ifdef TAPAS_DEBUG
  if (depth > MAX_DEPTH) {
    TAPAS_LOG_ERROR() << "Exceeded the maximum allowable depth: " << MAX_DEPTH << std::endl;
    TAPAS_DIE();
  }
#endif  
  k = MortonKeyRemoveDepth(k);
  return MortonKeyAppendDepth(k, depth);
}

template <int DIM>
bool MortonKeyIsDescendant(tapas::KeyType asc, tapas::KeyType dsc) {
  int depth = MortonKeyGetDepth(asc);
  if (depth >= MortonKeyGetDepth(dsc)) return false;
  int s = (MAX_DEPTH - depth) * DIM + DEPTH_BIT_WIDTH;
  asc >>= s;
  dsc >>= s;
  return asc == dsc;
}


template <int DIM>
tapas::KeyType CalcFinestMortonKey(const tapas::Vec<DIM, int> &anchor) {
  KeyType k = 0;
  int mask = 1 << (MAX_DEPTH - 1);
  for (int i = 0; i < MAX_DEPTH; ++i) {
    for (int d = DIM-1; d >= 0; --d) {
      k = (k << 1) | ((anchor[d] & mask) >> (MAX_DEPTH - i - 1));
    }
    mask >>= 1;
  }
  return MortonKeyAppendDepth(k, MAX_DEPTH);
}

template <int DIM, class FP, class BT>
HelperNode<DIM> *CreateInitialNodes(
    const typename BT::type *p, index_t np,
    const Region<DIM, FP> &r) {
  HelperNode<DIM> *nodes = new HelperNode<DIM>[np];
  FP num_cell = 1 << MAX_DEPTH;
  Vec<DIM, FP> pitch;
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
    assert(node.anchor < (1 << MAX_DEPTH));
#endif // TAPAS_DEBUG
    
    node.key = CalcFinestMortonKey<DIM>(node.anchor);
  }
  
  return nodes;
}

template <int DIM>
void SortNodes(HelperNode<DIM> *nodes, index_t n) {
  std::qsort(nodes, n, sizeof(HelperNode<DIM>),
             [] (const void *x, const void *y) {
               return static_cast<const HelperNode<DIM>*>(x)->key -
                   static_cast<const HelperNode<DIM>*>(y)->key;
             });
}

template <int DIM, class BT>
void SortBodies(const typename BT::type *b, typename BT::type *sorted,
                            const HelperNode<DIM> *sorted_nodes,
                            tapas::index_t nb) {
  for (index_t i = 0; i < nb; ++i) {
    sorted[i] = b[sorted_nodes[i].p_index];
  }
}

template <int DIM>
tapas::KeyType FindFinestAncestor(tapas::KeyType x,
                                              tapas::KeyType y) {
  int min_depth = std::min(MortonKeyGetDepth(x),
                           MortonKeyGetDepth(y));
  x = MortonKeyRemoveDepth(x);
  y = MortonKeyRemoveDepth(y);
  KeyType a = ~(x ^ y);
  int common_depth = 0;
  for (; common_depth < min_depth; ++common_depth) {
    KeyType t = (a >> (MAX_DEPTH - common_depth -1) * DIM) & ((1 << DIM) - 1);
    if (t != ((1 << DIM) -1)) break;
  }
  int common_bit_len = common_depth * DIM;
  KeyType mask = ((1 << common_bit_len) - 1) << (MAX_DEPTH * DIM - common_bit_len);
  return MortonKeyAppendDepth(x & mask, common_depth);
}

template <int DIM, class T>
void AppendChildren(tapas::KeyType x, T &s) {
  int x_depth = MortonKeyGetDepth(x);
  int c_depth = x_depth + 1;
  if (c_depth > MAX_DEPTH) return;
  x = MortonKeyIncrementDepth(x, 1);
  for (int i = 0; i < (1 << DIM); ++i) {
    int child_key = (i << ((MAX_DEPTH - c_depth) * DIM + DEPTH_BIT_WIDTH));
    s.push_back(x | child_key);
    TAPAS_LOG_DEBUG() << "Adding child " << (x | child_key) << std::endl;
  }
}

// Note this doesn't return a valid morton key when the incremented
// key overflows the overall region, but should be fine for
// FindBodyRange method.
template <int DIM>
tapas::KeyType CalcMortonKeyNext(KeyType k) {
  int d = MortonKeyGetDepth(k);
  KeyType inc = 1 << (DIM * (MAX_DEPTH - d) + DEPTH_BIT_WIDTH);
  return k + inc;
}

template <int DIM>
tapas::KeyType MortonKeyClearDescendants(KeyType k) {
  int d = MortonKeyGetDepth(k);
  KeyType m = ~(((1 << ((MAX_DEPTH - d) * DIM)) - 1) << DEPTH_BIT_WIDTH);
  return k & m;
}

template <int DIM>
tapas::KeyType MortonKeyParent(KeyType k) {
  int d = MortonKeyGetDepth(k);  
  if (d == 0) return k;
  k = MortonKeyIncrementDepth(k, -1);
  return MortonKeyClearDescendants<DIM>(k);
}


template <int DIM>
tapas::KeyType MortonKeyFirstChild(KeyType k) {
#ifdef TAPAS_DEBUG
  KeyType t = MortonKeyRemoveDepth(k);
  t = t & ~(~((KeyType)0) << (DIM * (MAX_DEPTH - MortonKeyGetDepth(k))));
  assert(t == 0);
#endif  
  return MortonKeyIncrementDepth(k, 1);
}

template <int DIM>
tapas::KeyType MortonKeyChild(KeyType k, int child_idx) {
  TAPAS_ASSERT(child_idx < (1 << DIM));
  k = MortonKeyIncrementDepth(k, 1);
  int d = MortonKeyGetDepth(k);
  return k | (child_idx << ((MAX_DEPTH - d) * DIM + DEPTH_BIT_WIDTH));
}

template <int DIM>
void CompleteRegion(tapas::KeyType x, tapas::KeyType y,
                                tapas::KeyVector &s) {
  KeyType fa = FindFinestAncestor<DIM>(x, y);
  KeyList w;
  AppendChildren<DIM>(fa, w);
  tapas::PrintKeys(w, std::cout);
  while (w.size() > 0) {
    KeyType k = w.front();
    w.pop_front();
    TAPAS_LOG_DEBUG() << "visiting " << k << std::endl;
    if ((k > x && k < y) && !MortonKeyIsDescendant<DIM>(k, y)) {
      s.push_back(k);
      TAPAS_LOG_DEBUG() << "Adding " << k << " to output set" << std::endl;
    } else if (MortonKeyIsDescendant<DIM>(k, x) ||
               MortonKeyIsDescendant<DIM>(k, y)) {
      TAPAS_LOG_DEBUG() << "Adding children of " << k << " to work set" << std::endl;
      AppendChildren<DIM>(k, w);

    }
  }
  std::sort(std::begin(s), std::end(s));
}

template <int DIM>
tapas::index_t FindFirst(const KeyType k,
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
tapas::KeyPair GetBodyRange(const tapas::KeyType k,
                                        const HelperNode<DIM> *hn,
                                        const BT *b, index_t offset,
                                        index_t len) {
  index_t body_begin = FindFirst(k, hn, offset, len);
  index_t body_num = GetBodyNumber(k, hn, b, body_begin,
                                   len-(body_begin-offset));
  return std::make_pair(body_begin, body_num);
}

template <int DIM, class BT>
tapas::index_t GetBodyNumber(const tapas::KeyType k,
                                         const HelperNode<DIM> *hn,
                                         const BT *b,
                                         index_t offset,
                                         index_t len) {
  //index_t body_begin = FindFirst(k, hn, offset, len);
  KeyType next_key = CalcMortonKeyNext<DIM>(k);
  index_t body_end = FindFirst(next_key, hn, offset+1, len-1);
  return body_end - offset;
}

template <CELL_TEMPLATE_PARAMS_NO_DEF>
bool Cell<CELL_TEMPLATE_ARGS>::IsRoot() const {
  return MortonKeyGetDepth(key_) == 0;
}

template <CELL_TEMPLATE_PARAMS_NO_DEF>
bool Cell<CELL_TEMPLATE_ARGS>::IsLeaf() const {
  return is_leaf_;
}

template <CELL_TEMPLATE_PARAMS_NO_DEF>
int Cell<CELL_TEMPLATE_ARGS>::nsubcells() const {
  if (IsLeaf()) return 0;
  else return (1 << DIM);
}

template <CELL_TEMPLATE_PARAMS_NO_DEF>
Cell<CELL_TEMPLATE_ARGS> &Cell<CELL_TEMPLATE_ARGS>::subcell(int idx) const {
  KeyType k = MortonKeyChild<DIM>(key_, idx);
  return *Lookup(k);
}


template <CELL_TEMPLATE_PARAMS_NO_DEF>
Cell<CELL_TEMPLATE_ARGS> *Cell<CELL_TEMPLATE_ARGS>::Lookup(
    KeyType k) const {
  auto i = ht_.find(k);
  if (i != ht_.end()) {
    return i->second;
  } else {
    return NULL;
  }
}

template <CELL_TEMPLATE_PARAMS_NO_DEF>
Cell<CELL_TEMPLATE_ARGS> &Cell<CELL_TEMPLATE_ARGS>::parent() const {
  if (IsRoot()) {
    TAPAS_LOG_ERROR() << "Trying to access parent of the root cell." << std::endl;
    TAPAS_DIE();
  }
  KeyType *parent_key = MortonKeyParent<DIM>(key_);
  auto *c = Lookup(parent_key);
  if (c == NULL) {
    TAPAS_LOG_ERROR() << "Parent (" << parent_key << ") of cell (" << key_ << ") not found."
                      << std::endl;
    TAPAS_DIE();
  }
  return *c;
}

template <CELL_TEMPLATE_PARAMS_NO_DEF>
typename BT::type &Cell<CELL_TEMPLATE_ARGS>::body(index_t idx) const {
  return bodies_[this->bid_+idx];
}

template <CELL_TEMPLATE_PARAMS_NO_DEF>
BT_ATTR *Cell<CELL_TEMPLATE_ARGS>::body_attrs() const {
  return body_attrs_;
}

template <CELL_TEMPLATE_PARAMS_NO_DEF>
BT_ATTR &Cell<CELL_TEMPLATE_ARGS>::attr(index_t idx) const {
  return body_attrs_[this->bid_+idx];
}

template <CELL_TEMPLATE_PARAMS_NO_DEF>
SubCellIterator<DIM, Cell<CELL_TEMPLATE_ARGS> > Cell<CELL_TEMPLATE_ARGS>::
subcells() const {
  return SubCellIterator<DIM, Cell>(*this);
}


template <CELL_TEMPLATE_PARAMS_NO_DEF>
BodyIterator<DIM, BT, BT_ATTR, Cell<CELL_TEMPLATE_ARGS> > Cell<CELL_TEMPLATE_ARGS>::
bodies() const {
  return BodyIterator<DIM, BT, BT_ATTR, Cell<CELL_TEMPLATE_ARGS> >(*this);
}

template <CELL_TEMPLATE_PARAMS_NO_DEF>
Cell<CELL_TEMPLATE_ARGS> *Partition<CELL_TEMPLATE_ARGS>::operator()(
    typename BT::type *b, index_t nb,
    const Region<DIM, FP> &r) {
  typedef Cell<CELL_TEMPLATE_ARGS> CELL_T;
  
  typename BT::type *b_work = new typename BT::type[nb];
  HelperNode<DIM> *hn = CreateInitialNodes<DIM, FP, BT>(b, nb, r);

  SortNodes<DIM>(hn, nb);
  SortBodies<DIM, BT>(b, b_work, hn, nb);
  std::memcpy(b, b_work, sizeof(BT) * nb);
  BT_ATTR *attrs = new BT_ATTR[nb];

  KeyType root_key = 0;
  KeyPair kp = hot::GetBodyRange(root_key, hn, b, 0, nb);
  TAPAS_LOG_DEBUG() << "Root range: offset: " << kp.first << ", length: " << kp.second << "\n";

  typename CELL_T::HashTable *ht = new typename CELL_T::HashTable();
  //auto *root = new Cell<CELL_TEMPLATE_ARGS>(r, 0, nb, root_key);
  auto *root = new CELL_T(r, 0, nb, root_key, *ht, b, attrs);
  ht->insert(std::make_pair(root_key, root));
  Refine(root, hn, b, 0, 0);
  
  return root;
}


template <CELL_TEMPLATE_PARAMS_NO_DEF>
void Partition<CELL_TEMPLATE_ARGS>::Refine(CELL *c,
                                           const HelperNode<DIM> *hn,
                                           const typename BT::type *b,
                                           int cur_depth,
                                           KeyType cur_key) const {
  TAPAS_LOG_DEBUG() << "Current depth: " << cur_depth << std::endl;
  if (c->nb() <= max_nb_) {
    TAPAS_LOG_DEBUG() << "Small enough cell" << std::endl;
    return;
  }
  if (cur_depth >= MAX_DEPTH) {
    TAPAS_LOG_DEBUG() << "Reached maximum depth" << std::endl;
    return;
  }
  KeyType child_key = MortonKeyFirstChild<DIM>(cur_key);
  index_t cur_offset = c->bid();
  index_t cur_len = c->nb();
  for (int i = 0; i < (1 << DIM); ++i) {
    TAPAS_LOG_DEBUG() << "Child key: " << child_key << std::endl; 
    index_t child_bn = GetBodyNumber(child_key, hn, b, cur_offset, cur_len);
    TAPAS_LOG_DEBUG() << "Range: offset: " << cur_offset << ", length: "
                      << child_bn << "\n";
    auto child_r = c->region().PartitionBSP(i);
    auto *child_cell = new Cell<CELL_TEMPLATE_ARGS>(
        child_r, cur_offset, child_bn, child_key, c->ht(),
        c->bodies_, c->body_attrs_);
    c->ht().insert(std::make_pair(child_key, child_cell));
    TAPAS_LOG_DEBUG() << "Particles: \n";
    tapas::debug::PrintBodies<DIM, FP, BT>(b+cur_offset, child_bn, std::cerr);
    Refine(child_cell, hn, b, cur_depth+1, child_key);
    child_key = hot::CalcMortonKeyNext<DIM>(child_key);
    cur_offset = cur_offset + child_bn;
    cur_len = cur_len - child_bn;
  }
  c->is_leaf_ = false;
}


} // namespace hot
} // namespace tapas

#undef CELL_TEMPLATE_PARAMS
#undef CELL_TEMPLATE_PARAMS_NO_DEF
#undef CELL_TEMPLATE_ARGS
#undef CELL

#endif // TAPAS_HOT_
