#!/usr/bin/env python

# The algorithm used below is based on the description at
# http://arborjs.org/docs/barnes-hut.

import taco
import common
import math

class BHCell(taco.Cell):
    def __init__(self, particles, region, indices=None):
        super(BHCell, self).__init__(particles, region, indices)
        self.center = taco.Vector([0.0]*common.NDIM)
        self.m = None # designates invalid status

    def create_sub_cell(self, sub_region):
        return BHCell(self.particles, sub_region)

def approximate(c):
    # This is an optional optimization. Should be taken care by the framework.
    if c.m is not None:
        return
    if c.get_num_particles() == 0:
        c.m = 0.0
        return
    elif c.get_num_particles() == 1:
        p = c.particles[iter(c.indices).next()]
        c.m = p.m
        c.center = p.pos
    if c.get_num_particles() > 1:
        center = taco.Vector([0.0]*common.NDIM)
        m = 0.0
        # sub cells have already been visited 
        for i in c.get_sub_cells():
            center = center.add(i.center.mult(i.m))
            m += i.m
        c.center = center
        c.m = m
    return

def bh_interact2(c1, c2, theta):
    if c2.get_num_particles() == 0:
        return
    elif c2.get_num_particles() == 1:
        p1 = c1.particles[iter(c1.indices).next()]
        p2 = c1.particles[iter(c2.indices).next()]
        p1.f = p1.f.add(common.compute_force(p1.pos, p2.pos, p1.m, p2.m))
        return
    else:
        # assume this has been called already
        #approximate(c2)
        p = c1.particles[iter(c1.indices).next()]
        d = math.sqrt(p.pos.distance_r2(c2.center))
        s = c2.region.r[0][1]-c2.region.r[0][0]
        if s / d < theta:
            # sufficiently far away
            p.f = p.f.add(common.compute_force(p.pos, c2.center, p.m, c2.m))
            skip_children()
    return


def taco_barnes_hut(np):
    particles = common.gen_particle_list(np)
    root_region =  taco.Region(tuple([(0.0, 1.0)]*common.NDIM))
    root_cell = BHCell(particles, root_region, set(range(np)))
    theta = 0.5
    
    #bh_interact(root_cell, root_cell, theta)

    map(approximate, traverse(root_cell), PostOrder)
    map(bh_interact2, product(root_cell.get_leaves(), traverse(root_cell)), theta)
    
    return particles

def main():
    common.run_test(taco_barnes_hut, "Barnes Hut")
    return
    
if __name__ == "__main__":
    main()


