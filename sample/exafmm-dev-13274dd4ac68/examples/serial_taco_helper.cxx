
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

#if 0
// TODO
// UpDownPass::upwardPass
static void fmm_p2m(taco_cell * const c, real_t theta) {
  if (!TACO_is_leaf(c)) {
    map(fmm_p2m, TACO_get_subcells(c));
  }
  c->M = 0;
  C->L = 0;
  if (TACO_is_leaf(c)) {
    kernel::P2M(c);
  } else {
    kernel::M2M(c);
  }
  C->R /= theta;
}

#endif
