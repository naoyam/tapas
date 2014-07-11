import itertools
import pdb
import sys

def product(s1, s2):
    # itertools.product(s1, s2) does not work since it can include
    # duplicated pairs
    prod  = []
    # covered_region_pair is a set of region pairs that are already
    # inserted to the product list
    covered_region_pair = set()
    for i in s1:
        for j in s2:
            if (not (i.region, j.region) in covered_region_pair) and \
               (not (j.region, i.region) in covered_region_pair):
                prod.append((i, j))
                covered_region_pair.add((i.region, j.region))
    return prod


class Vector(object):
    def __init__(self, v=None):
        if type(v) is Vector:
            self.v = v.v[:]
        elif v is None:
            self.v = []            
        else:
            self.v = v
            
    def __len__(self):
        return len(self.v)

    def __iter__(self):
        return iter(self.v)

    def __getitem__(self, index):
        return self.v[index]
        
    def __setitem__(self, index, v):
        self.v[index] = v
        return

    def append(self, v):
        self.v.append(v)

    def mult(self, x):
        if type(x) is Vector:
            return Vector([i * j for i, j in zip(self, x)])
        else:
            return Vector([i * x for i in self.v])

    def add(self, x):
        if type(x) is Vector:
            return Vector([i + j for i, j in zip(self, x)])
        else:
            return Vector([i + x for i in self.v])

    def distance_r2(self, other):
        return reduce(lambda x, y: x + y,
                      ((x - y) ** 2 for x, y
                       in itertools.izip(self.v, other.v)))
        
    
class Particle(object):
    def __init__(self,  pos):
        self.pos = Vector(pos)

    def __str__(self):
        return "particle: " + ", ".join(str(i) for i in self.pos)
    pass

    def distance_r2(self, other):
        return self.pos.distance_r2(other.pos)

class Region(object):
    def __init__(self, min_max):
        assert(type(min_max) == tuple)
        self.r = min_max

    def partition(self):
        x = itertools.product(
            *[[(r[0], r[0]+(r[1]-r[0])/2.0),
             (r[0]+(r[1]-r[0])/2.0, r[1])]
                 for r in self.r])
        children = [Region(i) for i in x]
        return children

    def contains(self, particle):
        return all(p >= self.r[i][0] and p < self.r[i][1]
                   for i, p in enumerate(particle.pos))

    def __str__(self):
        return "region: " + str(self.r)

    def __eq__(self, other):
        return self.r == other.r

    def __ne__(self, other):
        return not(self == other)

    def __hash__(self):
        return hash(self.r)
        
class Cell(object):
    def __init__(self, particles, region, indices=None):
        self.particles = particles
        self.region = region
        self.indices = indices
        if self.indices is None:
            self.indices = set()
        self.sub_cells = None
        pass

    def __str__(self):
        return "cell {" + str(self.region) + ", indices: " + \
            str(self.indices) + "}"

    def create_sub_cell(self, sub_region):
        return Cell(self.particles, sub_region)

    # partition a cell to equally divided 8 sub cells
    def _partition(self):
        sub_regions = self.region.partition()
        sub_cells = []        
        for c in sub_regions:
            sub_cells.append(self.create_sub_cell(c))

        for index in self.indices:
            for c in sub_cells:
                if c.region.contains(self.particles[index]):
                    c.indices.add(index)

        self.sub_cells = sub_cells

    def get_sub_cells(self):
        if self.sub_cells is None:
            self._partition()
        return self.sub_cells
        

    def get_num_particles(self):
        return len(self.indices)
    
