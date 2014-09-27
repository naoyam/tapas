
int numM2L;
int numP2P;

template <int DIM, class FP> inline
tapas::Vec<DIM, FP> &asn(tapas::Vec<DIM, FP> &dst, const vec<DIM, FP> &src) {
  for (int i = 0; i < DIM; ++i) {
    dst[i] = src[i];
  }
  return dst;
}
template <int DIM, class FP> inline
vec<DIM, FP> &asn(vec<DIM, FP> &dst, const tapas::Vec<DIM, FP> &src) {
  for (int i = 0; i < DIM; ++i) {
    dst[i] = src[i];
  }
  return dst;
}

static Region &asn(Region &x, const Bounds &y) {
  asn(x.min(), y.Xmin);
  asn(x.max(), y.Xmax);
  return x;
}

// UpDownPass::upwardPass
static inline void FMM_P2M(Tapas::Cell &c, real_t theta) {
  if (!c.IsLeaf()) {
    tapas::Map(FMM_P2M, c.subcells(), theta);
  }
  c.attr().M = 0;
  c.attr().L = 0;
  if (c.IsLeaf()) {
    tapas_kernel::P2M(c);
  } else {
    tapas_kernel::M2M(c);
  }
  for (int i = 0; i < 3; ++i) {
    c.attr().R = std::max(c.width(i), c.attr().R);
  }
  c.attr().R = c.attr().R / 2 * 1.00001; // see bounds2box func
  c.attr().R /= theta;
}

static inline void FMM_L2P(Tapas::Cell &c) {
  if (c.nb() == 0) return;
  if (!c.IsRoot()) tapas_kernel::L2L(c);
  if (c.IsLeaf()) {
    tapas::Map(tapas_kernel::L2P, c.bodies());
  } else {
    tapas::Map(FMM_L2P, c.subcells());
  }
}


static void FMM_M2L(Tapas::Cell &Ci, Tapas::Cell &Cj, int mutual, int nspawn);

static inline void tapas_splitCell(Tapas::Cell &Ci, Tapas::Cell &Cj, int mutual, int nspawn) {
  if (Cj.IsLeaf()) {
    assert(!Ci.IsLeaf());                                   //  Make sure Ci is not leaf
    //for (C_iter ci=Ci0+Ci->ICHILD; ci!=Ci0+Ci->ICHILD+Ci->NCHILD; ci++) {// Loop over Ci's children
    //traverse(ci, Cj, Xperiodic, mutual, remote);            //   Traverse a single pair of cells
    //}                                                         //
    //End loop over Ci's children
    tapas::Map(FMM_M2L, tapas::Product(Ci.subcells(), Cj), mutual, nspawn);
  } else if (Ci.IsLeaf()) {                               // Else if Ci is leaf
    assert(!Cj.IsLeaf());                                   //  Make sure Cj is not leaf
    //for (C_iter cj=Cj0+Cj->ICHILD; cj!=Cj0+Cj->ICHILD+Cj->NCHILD; cj++) {// Loop over Cj's children
    //traverse(Ci, cj, Xperiodic, mutual, remote);            //   Traverse a single pair of cells
    //}                                                         //
    //End loop over Cj's children
    tapas::Map(FMM_M2L, tapas::Product(Ci, Cj.subcells()), mutual, nspawn);
  } else if (Ci.nb() + Cj.nb() >= (tapas::index_t)nspawn || (mutual && Ci == Cj)) {// Else if cells are still large
    //TraverseRange traverseRange(this, Ci0+Ci->ICHILD, Ci0+Ci->ICHILD+Ci->NCHILD,// Instantiate recursive functor
    //Cj0+Cj->ICHILD, Cj0+Cj->ICHILD+Cj->NCHILD, Xperiodic, mutual, remote);
    //traverseRange();                                          //
    //Traverse for range of cell pairs
    tapas::Map(FMM_M2L, tapas::Product(Ci.subcells(), Cj.subcells()), mutual, nspawn);
  } else if (Ci.attr().R >= Cj.attr().R) {                                // Else if Ci is larger than Cj
    //for (C_iter ci=Ci0+Ci->ICHILD; ci!=Ci0+Ci->ICHILD+Ci->NCHILD; ci++) {// Loop over Ci's children
    //traverse(ci, Cj, Xperiodic, mutual, remote);            //   Traverse a single pair of cells
    //}                                                         //  End loop over Ci's children
    tapas::Map(FMM_M2L, tapas::Product(Ci.subcells(), Cj), mutual, nspawn);
  } else {                                                    // Else if Cj is larger than Ci
    //for (C_iter cj=Cj0+Cj->ICHILD; cj!=Cj0+Cj->ICHILD+Cj->NCHILD; cj++) {// Loop over Cj's children
    // traverse(Ci, cj, Xperiodic, mutual, remote);            //   Traverse a single pair of cells
    //}                                                         //  End loop over Cj's children
    tapas::Map(FMM_M2L, tapas::Product(Ci, Cj.subcells()), mutual, nspawn);
  }
}

static inline void FMM_M2L(Tapas::Cell &Ci, Tapas::Cell &Cj, int mutual, int nspawn) {
  //static inline void FMM_M2L(TapasCell &Ci, TapasCell &Cj, int
  //mutual) {
  if (Ci.nb() == 0 || Cj.nb() == 0) return;
  vec3 dX;
  asn(dX, Ci.center() - Cj.center());
  real_t R2 = norm(dX);
  vec3 Xperiodic = 0; // dummy; periodic not ported
  if (R2 > (Ci.attr().R+Cj.attr().R) * (Ci.attr().R+Cj.attr().R)) {
    //std::cerr << "R2 Y: " << R2 << " vs " << (Ci.attr().R+Cj.attr().R) * (Ci.attr().R+Cj.attr().R) << std::endl;
  } else {
    //std::cerr << "R2: " << R2 << " vs " << (Ci.attr().R+Cj.attr().R) * (Ci.attr().R+Cj.attr().R) << std::endl;
  }
  if (R2 > (Ci.attr().R+Cj.attr().R) * (Ci.attr().R+Cj.attr().R)) {                   // If distance is far enough
    //std::cerr << "M2L approx\n";
    numM2L++;
    tapas_kernel::M2L(Ci, Cj, Xperiodic, mutual);                   //  M2L kernel
  } else if (Ci.IsLeaf() && Cj.IsLeaf()) {            // Else if both cells are bodies
    tapas::Map(tapas_kernel::P2P, tapas::Product(Ci.bodies(), Cj.bodies()),
               Xperiodic);
    numP2P++;
  } else {                                                    // Else if cells are close but not bodies
    tapas_splitCell(Ci, Cj, mutual, nspawn);             //  Split cell and call function recursively for child
  }                                                           // End if for multipole acceptance
}

static inline void CopyBackResult(Bodies &body, const kvec4 *trg,
                                  int num_bodies) {
  // greedy ad-hoc copying
  for (int i = 0; i < num_bodies; ++i) {
    body[i].TRG = trg[i];
  }
}
