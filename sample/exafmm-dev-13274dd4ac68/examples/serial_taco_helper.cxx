
template <typename TACO_vector, typename EXAFMM_vector>
TACO_vector &asn(TACO_vector &x, const EXAFMM_vector &y) {
  for (int i = 0; i < (int)(sizeof(TACO_vector)/sizeof(real_t)); ++i) {
    x.x[i] = y[i];
  }
  return x;
}

static taco_particle *LoadBodiesToParticles(Bodies &bodies) {
  taco_particle *p = (taco_particle*)malloc(sizeof(taco_particle) * bodies.size());
  int i = 0;
  for (B_iter it = bodies.begin(); it != bodies.end(); ++it) {
    asn(p[i].TRG, it->TRG);
  }
  return p;
}

static TACO_region3r &asn(TACO_region3r &x, const Bounds &y) {
  asn(x.min, y.Xmin);
  asn(x.max, y.Xmax);
  return x;
}

// TODO
static void fmm_p2m(taco_cell * const c) {
  if (TACO_is_leaf(c)) {
    c->m = p2m(c);
  } else {
    map(fmm_p2m, TACO_get_subcells(c));
    c->m = m2m(c);
  }
}
