/**
 * @file hot.h
 * @brief HOT (Hashed Octree) implementation of Tapas' tree
 */
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
#include "tapas/iterator.h"

#define DEPTH_BIT_WIDTH (3)
#define MAX_DEPTH ((1 << DEPTH_BIT_WIDTH) - 1)

namespace tapas {

// REFAACTORING: This should not be located here
typedef int KeyType;
typedef std::list<KeyType> KeyList;
typedef std::vector<KeyType> KeyVector;
typedef std::unordered_set<KeyType> KeySet;
typedef std::pair<KeyType, KeyType> KeyPair;

namespace single_node_morton_hot {

using std::unordered_map;

/** @brief A partitioning plugin class that provides Morton-curve based octree partitioning.
 */
struct SingleNodeMortonHOT {
};

template <int DIM>
struct HelperNode {
    KeyType key; // Morton key
    Vec<DIM, int> anchor;
    index_t p_index;
    index_t np;
};
    
template <class TSP>
static HelperNode<TSP::Dim>*
CreateInitialNodes(const typename TSP::BT::type *p, index_t np, 
                   const Region<TSP> &r);

static KeyType MortonKeyAppendDepth(KeyType k, int depth);

static int MortonKeyGetDepth(KeyType k);

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

template <int DIM>
KeyPair GetBodyRange(const KeyType k, const HelperNode<DIM> *hn,
                     index_t offset, index_t len);

template <int DIM>
index_t GetBodyNumber(const KeyType k, const HelperNode<DIM> *hn,
                      index_t offset, index_t len);


template <class TSP>    
class Partition;

template <class TSP> // TapasStaticParams
class Cell: public tapas::Cell<TSP> { 
    friend class Partition<TSP>;
    friend class BodyIterator<Cell>;
  public:
    typedef unordered_map<KeyType, Cell*> HashTable;
  protected:
    KeyType key_;
    HashTable *ht_;
  public:

    Cell(const Region<TSP> &region,
         index_t bid, index_t nb, KeyType key,
         HashTable *ht,
         typename TSP::BT::type *bodies,
         typename TSP::BT_ATTR *body_attrs) :
            tapas::Cell<TSP>(region, bid, nb), key_(key),
            ht_(ht), bodies_(bodies), body_attrs_(body_attrs),
            is_leaf_(true) {}
    
    typedef typename TSP::ATTR attr_type;
    typedef typename TSP::BT_ATTR body_attr_type;
    KeyType key() const { return key_; }

    bool operator==(const Cell &c) const;
    template <class T>
    bool operator==(const T &) const { return false; }
    bool IsRoot() const;
    bool IsLeaf() const;
    int nsubcells() const;
    Cell &subcell(int idx) const;
    Cell &parent() const;
#ifdef DEPRECATED
    typename TSP::BT::type &particle(index_t idx) const {
        return body(idx);
    }
#endif
    typename TSP::BT::type &body(index_t idx) const;
    BodyIterator<Cell> bodies() const;
#ifdef DEPRECATED
    typename TSP::BT_ATTR *particle_attrs() const {
        return body_attrs();
    }
#endif
    typename TSP::BT_ATTR *body_attrs() const;
    SubCellIterator<Cell> subcells() const;
  
  protected:
    typename TSP::BT_ATTR &body_attr(index_t idx) const;
    HashTable *ht() { return ht_; }
    Cell *Lookup(KeyType k) const;
    typename TSP::BT::type *bodies_;
    typename TSP::BT_ATTR *body_attrs_;
    bool is_leaf_;
    virtual void make_pure_virtual() const {}
}; // class Cell

template <class TSP> // Tapas static params
class Partition {
  private:
    const int max_nb_;
  
  public:
    Partition(unsigned max_nb): max_nb_(max_nb) {}
      
    Cell<TSP> *operator()(typename TSP::BT::type *b, index_t nb,
                          const Region<TSP> &r);
  private:
    void Refine(Cell<TSP> *c, const single_node_morton_hot::HelperNode<TSP::Dim> *hn,
                const typename TSP::BT::type *b, int cur_depth,
                KeyType cur_key) const;
}; // class Partition


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

template <class TSP>
HelperNode<TSP::Dim> *CreateInitialNodes(const typename TSP::BT::type *p,
                                         index_t np,
                                         const Region<TSP> &r) {
    const int Dim = TSP::Dim;
    typedef typename TSP::FP FP;
    typedef typename TSP::BT BT;
    
    HelperNode<Dim> *nodes = new HelperNode<TSP::Dim>[np];
    FP num_cell = 1 << MAX_DEPTH;
    Vec<Dim, FP> pitch;
    for (int d = 0; d < Dim; ++d) {
        pitch[d] = (r.max()[d] - r.min()[d]) / num_cell;
    }
    for (index_t i = 0; i < np; ++i) {
        HelperNode<Dim> &node = nodes[i];
        node.p_index = i;
        node.np = 1;
        Vec<Dim, FP> off = ParticlePosOffset<Dim, FP, BT::pos_offset>::vec((const void*)&(p[i]));
        off -= r.min();
        off /= pitch;
        for (int d = 0; d < Dim; ++d) {
            node.anchor[d] = (int)(off[d]);
            // assume maximum boundary is inclusive, i.e., a particle can be
            // right at the maximum boundary. anchor 
            if (node.anchor[d] == (1 << MAX_DEPTH)) {
                TAPAS_LOG_DEBUG() << "Particle located at max boundary." << std::endl;
                node.anchor[d]--;
            }
        }
#ifdef TAPAS_DEBUG
        assert(node.anchor >= 0);
#if 1   
        if (!(node.anchor < (1 << MAX_DEPTH))) {
            TAPAS_LOG_ERROR() << "Anchor, " << node.anchor
                              << ", exceeds the maximum depth." << std::endl
                              << "Particle at "
                              << ParticlePosOffset<Dim, FP, BT::pos_offset>::vec((const void*)&(p[i]))
                              << std::endl;
            TAPAS_DIE();
        }
#else
        assert(node.anchor < (1 << MAX_DEPTH));
#endif
#endif // TAPAS_DEBUG
    
        node.key = CalcFinestMortonKey<Dim>(node.anchor);
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
    TAPAS_ASSERT(len > 0);
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
template <int DIM>
tapas::KeyPair GetBodyRange(const tapas::KeyType k,
                            const HelperNode<DIM> *hn,
                            index_t offset,
                            index_t len) {
    TAPAS_ASSERT(len > 0);
    index_t body_begin = FindFirst(k, hn, offset, len);
    TAPAS_ASSERT(body_begin >= offset);
    TAPAS_ASSERT(body_begin < len);
    index_t body_num = GetBodyNumber(k, hn, body_begin,
                                     len-(body_begin-offset));
    return std::make_pair(body_begin, body_num);
}

template <int DIM>
tapas::index_t GetBodyNumber(const tapas::KeyType k,
                             const HelperNode<DIM> *hn,
                             index_t offset,
                             index_t len) {
    //index_t body_begin = FindFirst(k, hn, offset, len);
    if (len == 0) return 0;
    if (len == 1) return 1;
    KeyType next_key = CalcMortonKeyNext<DIM>(k);
    index_t body_end = FindFirst(next_key, hn, offset+1, len-1);
    return body_end - offset;
}

template <class TSP>
bool Cell<TSP>::operator==(const Cell &c) const {
    return key_ == c.key_;
}

template <class TSP>
bool Cell<TSP>::IsRoot() const {
    return MortonKeyGetDepth(key_) == 0;
}

template <class TSP>
bool Cell<TSP>::IsLeaf() const {
    return is_leaf_;
}

template <class TSP>
int Cell<TSP>::nsubcells() const {
    if (IsLeaf()) return 0;
    else return (1 << TSP::Dim);
}

template <class TSP>
Cell<TSP> &Cell<TSP>::subcell(int idx) const {
    KeyType k = MortonKeyChild<TSP::Dim>(key_, idx);
    return *Lookup(k);
}


template <class TSP>
Cell<TSP> *Cell<TSP>::Lookup(KeyType k) const {
    auto i = ht_->find(k);
    if (i != ht_->end()) {
        return i->second;
    } else {
        return nullptr;
    }
}

template <class TSP>
Cell<TSP> &Cell<TSP>::parent() const {
    if (IsRoot()) {
        TAPAS_LOG_ERROR() << "Trying to access parent of the root cell." << std::endl;
        TAPAS_DIE();
    }
    KeyType parent_key = MortonKeyParent<TSP::Dim>(key_);
    auto *c = Lookup(parent_key);
    if (c == nullptr) {
        TAPAS_LOG_ERROR() << "Parent (" << parent_key << ") of "
                          << "cell (" << key_ << ") not found."
                          << std::endl;
        TAPAS_DIE();
    }
    return *c;
}

template <class TSP>
typename TSP::BT::type &Cell<TSP>::body(index_t idx) const {
    return bodies_[this->bid_+idx];
}

template <class TSP>
typename TSP::BT_ATTR *Cell<TSP>::body_attrs() const {
    return body_attrs_;
}

template <class TSP>
typename TSP::BT_ATTR &Cell<TSP>::body_attr(index_t idx) const {
    return body_attrs_[this->bid_+idx];
}

template <class TSP>
SubCellIterator<Cell<TSP>> Cell<TSP>::subcells() const {
    return SubCellIterator<Cell>(*this);
}


template <class TSP>
BodyIterator<Cell<TSP>> Cell<TSP>::bodies() const {
    return BodyIterator<Cell<TSP> >(*this);
}

template <class TSP> // Tapas Static Params
Cell<TSP>*
Partition<TSP>::operator() (typename TSP::BT::type *b,
                            index_t nb,
                            const Region<TSP> &r) {
    const int Dim = TSP::Dim;
    typedef typename TSP::FP FP;
    typedef typename TSP::BT BT;
    typedef typename TSP::BT_ATTR BT_ATTR;
    typedef typename BT::type Btype;
    typedef Cell<TSP> CellT;
    
    Btype *b_work = new Btype[nb];
    HelperNode<Dim> *hn = CreateInitialNodes<TSP>(b, nb, r);

    SortNodes<Dim>(hn, nb);
    SortBodies<Dim, BT>(b, b_work, hn, nb);
    std::memcpy(b, b_work, sizeof(Btype) * nb);
    //BT_ATTR *attrs = new BT_ATTR[nb];
    BT_ATTR *attrs = (BT_ATTR*)calloc(nb, sizeof(BT_ATTR));

    KeyType root_key = 0;
    KeyPair kp = single_node_morton_hot::GetBodyRange(root_key, hn, 0, nb);
    TAPAS_LOG_DEBUG() << "Root range: offset: " << kp.first << ", "
                      << "length: " << kp.second << "\n";

    auto *ht = new typename CellT::HashTable();
    auto *root = new CellT(r, 0, nb, root_key, ht, b, attrs);
    ht->insert(std::make_pair(root_key, root));
    Refine(root, hn, b, 0, 0);
    
    return root;
}

template <class TSP>
void Partition<TSP>::Refine(Cell<TSP> *c,
                            const HelperNode<TSP::Dim> *hn,
                            const typename TSP::BT::type *b,
                            int cur_depth,
                            KeyType cur_key) const {
    const int Dim = TSP::Dim;
    typedef typename TSP::FP FP;
    typedef typename TSP::BT BT;
    
    TAPAS_LOG_INFO() << "Current depth: " << cur_depth << std::endl;
    if (c->nb() <= max_nb_) {
        TAPAS_LOG_INFO() << "Small enough cell" << std::endl;
        return;
    }
    if (cur_depth >= MAX_DEPTH) {
        TAPAS_LOG_INFO() << "Reached maximum depth" << std::endl;
        return;
    }
    KeyType child_key = MortonKeyFirstChild<Dim>(cur_key);
    index_t cur_offset = c->bid();
    index_t cur_len = c->nb();
    for (int i = 0; i < (1 << Dim); ++i) {
        TAPAS_LOG_DEBUG() << "Child key: " << child_key << std::endl; 
        index_t child_bn = GetBodyNumber(child_key, hn, cur_offset, cur_len);
        TAPAS_LOG_DEBUG() << "Range: offset: " << cur_offset << ", length: "
                          << child_bn << "\n";
        auto child_r = c->region().PartitionBSP(i);
        auto *child_cell = new Cell<TSP>(
            child_r, cur_offset, child_bn, child_key, c->ht(),
            c->bodies_, c->body_attrs_);
        c->ht()->insert(std::make_pair(child_key, child_cell));
        TAPAS_LOG_DEBUG() << "Particles: \n";
#ifdef TAPAS_DEBUG    
        tapas::debug::PrintBodies<Dim, FP, BT>(b+cur_offset, child_bn, std::cerr);
#endif    
        Refine(child_cell, hn, b, cur_depth+1, child_key);
        child_key = single_node_morton_hot::CalcMortonKeyNext<Dim>(child_key);
        cur_offset = cur_offset + child_bn;
        cur_len = cur_len - child_bn;
    }
    c->is_leaf_ = false;
}

} // namespace single_node_morton_hot

template <class TSP, class T2>
ProductIterator<CellIterator<single_node_morton_hot::Cell<TSP>>, T2>
Product(single_node_morton_hot::Cell<TSP> &c, T2 t2) {
    TAPAS_LOG_DEBUG() << "Cell-X product\n";
    typedef single_node_morton_hot::Cell<TSP> CellType;
    typedef CellIterator<CellType> CellIterType;
    return ProductIterator<CellIterType, T2>(CellIterType(c), t2);
}

template <class T1, class TSP>
ProductIterator<T1, CellIterator<single_node_morton_hot::Cell<TSP>>>
                         Product(T1 t1, single_node_morton_hot::Cell<TSP> &c) {
    TAPAS_LOG_DEBUG() << "X-Cell product\n";
    typedef single_node_morton_hot::Cell<TSP> CellType;
    typedef CellIterator<CellType> CellIterType;
    return ProductIterator<T1, CellIterType>(t1, CellIterType(c));
}

/**
 * Constructs a ProductIterator for dual tree traversal of two trees
 * \tparam int DIM
 * \tparam class FP
 * \tparam class BT
 * \tparam class BT_ATTR
 * \tparam class ATTR=tapas::NONE
 * \param c1 Root cell of the first tree
 * \param c2 Root cell of the second tree
 */ 
template <class TSP>
ProductIterator<CellIterator<single_node_morton_hot::Cell<TSP>>,
                CellIterator<single_node_morton_hot::Cell<TSP>>>
                         Product(single_node_morton_hot::Cell<TSP> &c1,
                                 single_node_morton_hot::Cell<TSP> &c2) {
    TAPAS_LOG_DEBUG() << "Cell-Cell product\n";
    typedef single_node_morton_hot::Cell<TSP> CellType;
    typedef CellIterator<CellType> CellIterType;
    return ProductIterator<CellIterType, CellIterType>(
        CellIterType(c1), CellIterType(c2));
}

/** 
 * @brief A dummy class to achieve template specialization.
 */
struct SingleNodeMortonHOT {};

template <int DIM, class FP, class BT,
          class BT_ATTR, class CELL_ATTR,
          class PartitionAlgorithm>
class Tapas;

// Specialization of Tapas for HOT (single node Morton HOT) algorithm
template <int DIM, class FP, class BT,
          class BT_ATTR, class CELL_ATTR>
class Tapas<DIM, FP, BT, BT_ATTR, CELL_ATTR, SingleNodeMortonHOT> {
    typedef TapasStaticParams<DIM, FP, BT, BT_ATTR, CELL_ATTR> TSP; // Tapas static params
  public:
    typedef tapas::Vec<2, FP> Vec2;
    typedef tapas::Vec<3, FP> Vec3;
    typedef tapas::Region<TSP> Region;  
    typedef single_node_morton_hot::Cell<TSP> Cell;
    //typedef tapas::BodyIterator<DIM, BT, BT_ATTR, Cell> BodyIterator;
    typedef tapas::BodyIterator<Cell> BodyIterator;  
    static Cell *Partition(typename BT::type *b,
                           index_t nb, const Region &r,
                           int max_nb) {
        single_node_morton_hot::Partition<TSP> part(max_nb);
        return part(b, nb, r);
    }
};

} // namespace tapas

#endif // TAPAS_HOT_
