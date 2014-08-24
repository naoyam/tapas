#!/usr/bin/env python

import taco
import common
import math

class FMMCell(taco.Cell):
    def __init__(self, particles, region, indices=None, is_root=False):
        super(FMMCell, self).__init__(particles, region, indices, is_root)
        self.m = None # multipole expansion
        self.l = None # local expansion

    def create_sub_cell(self, sub_region):
        return FMMCell(self.particles, sub_region, parent=self)

# Dummy functions
def p2p(c):
    return

def m2m(c):
    return

def m2l(c1, c2):
    return
    
def l2l(c):
    return

def l2p(c):
    return
    
def fmm_p2m(c):
    if c.is_leaf():
        # approximate the leaf cell for long-range force calculation
        # TODO
        c.m = p2m(c)
    else:
        for i in c.get_sub_cells():
            fmm_bottomup(i)
        c.m = m2m(c)
    return

def get_grand_parent(c):
    p = c.get_parent()
    if p is not None:
        return p.get_parent()
    else:
        return None

        
def fmm_m2l(c1, c2):
    c1_grand_parent = get_grand_parent(c1)
    c2_grand_parent = get_grand_parent(c2)
    if c1_grand_parent != None and c2_grand_parent != None and \
       c1_grand_parent == c2_grand_parent:
        # reflect long-range interaction force mutually
        f = m2l(c1, c2)
        c1.l += f
        c2.l += -f # actual M2L force calculation look probably different
        for i, j in taco.product(c1.get_sub_cells(), c2.get_sub_cells()):
            fmm_m2l(i, j)
    elif c1_grand_parent is None:
        assert c2_grand_parent is None
        # c1 and c2 are the root cell or both of them do not have grand parents
        for i, j in taco.product(c1.get_sub_cells(), c2.get_sub_cells()):
            fmm_m2l(i, j)

            
def fmm_l2l(c):
    p = c.get_parent()
    if p is not None:
        c.l += l2l(c, p)
    if c.is_leaf():
        l2p(c)
    else:
        for i in c.get_sub_cells():
            fmm_l2l()

# Dummy            
def cell_distance(c1, c2):
    return None
    
def fmm_p2p(c1, c2, p2p_cutoff):
    if cell_distance(c1, c2) < p2p_cutoff:
        if c1.is_leaf() and c2.is_leaf:
            # update both c1 and c2
            p2p(c1, c2)
        elif not c1.is_leaf():
            for i in c1.get_sub_cells():
                fmm_p2p(i, c2, p2p_cutoff)
            return
        elif not c2.is_leaf():
            for i in c2.get_sub_cells():
                fmm_p2p(c1, i, p2p_cutoff)
            return
    return
        

def taco_fmm(np):
    particles = common.gen_particle_list(np)
    root_region =  taco.Region(tuple([(0.0, 1.0)]*common.NDIM))
    root_cell = FMMCell(particles, root_region, set(range(np)), is_root=True)

    # P2M and M2M
    fmm_p2m(root_cell)

    # M2L
    fmm_m2l(root_cell, root_cell)

    # L2L and L2P
    fmm_l2l(root_cell)

    # short-range direct force
    fmm_p2p(root_cell, root_cell)
    

    return particles

def main():
    common.run_test(taco_barnes_hut, "FMM")
    return
    
if __name__ == "__main__":
    main()


