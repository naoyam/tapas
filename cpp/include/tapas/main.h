#ifndef TAPAS_MAIN_H_
#define TAPAS_MAIN_H_

#include "tapas/common.h"

//!\todo this code should be written in user's code
#include "hot.h"

namespace tapas {

template <class BT, int POS_OFFSET>
class BodyInfo {
  public:
    typedef BT type;
    static const int pos_offset = POS_OFFSET;
};

// Generic Definition of the main class of Tapas framework
template <int DIM, class FP, class BT,
          class BT_ATTR, class CELL_ATTR,
          class PartitionAlgorithm>
class Tapas {};

class SingleNodeMortonHOT { };

// Specialization of Tapas for HOT (single node Morton HOT) algorithm
template <int DIM, class FP, class BT,
          class BT_ATTR, class CELL_ATTR>
class Tapas<DIM, FP, BT, BT_ATTR, CELL_ATTR, SingleNodeMortonHOT> {
    typedef TapasStaticParams<DIM, FP, BT, BT_ATTR, CELL_ATTR> TSP; // Tapas static params
  public:
    typedef tapas::Vec<2, FP> Vec2;
    typedef tapas::Vec<3, FP> Vec3;
    typedef tapas::Region<TSP> Region;  
    typedef tapas::hot::Cell<TSP> Cell;
    //typedef tapas::BodyIterator<DIM, BT, BT_ATTR, Cell> BodyIterator;
    typedef tapas::BodyIterator<Cell> BodyIterator;  
    static Cell *Partition(typename BT::type *b,
                           index_t nb, const Region &r,
                           int max_nb) {
        tapas::hot::Partition<TSP> part(max_nb);
        return part(b, nb, r);
    }
};

} // namespace tapas


#endif // TAPAS_MAIN_H_ 
