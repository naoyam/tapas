

template <int DIM, class FP> inline
taco::Vec<DIM, FP> &asn(taco::Vec<DIM, FP> &x, const vec<DIM, FP> &y) {
  for (int i = 0; i < DIM; ++i) {
    x[i] = y[i];
  }
  return x;
}

static Region &asn(Region &x, const Bounds &y) {
  asn(x.min(), y.Xmin);
  asn(x.max(), y.Xmax);
  return x;
}

// UpDownPass::upwardPass
static inline void FMM_P2M(TacoCell &c, real_t theta) {
  if (!c.IsLeaf()) {
    taco::Map(FMM_P2M, c.subcells(), theta);
  }
  c.attr().M = 0;
  c.attr().L = 0;
  if (c.IsLeaf()) {
    taco_kernel::P2M(c);
  } else {
    taco_kernel::M2M(c);
  }
  c.attr().R /= theta;
}
