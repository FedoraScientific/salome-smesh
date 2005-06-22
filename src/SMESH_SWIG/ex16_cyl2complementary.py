# CEA/LGLS 2004-2005, Francis KLOSS (OCC)
# =======================================

from geompy import *

import smesh

# Geometry
# ========

# Create the hexahedrical block geometry of a holed parallelepipede.
# The hole has a T form composed by 2 cylinders with different radius, and their axis are normal.
# This piece is meshed in hexahedrical.

# Values
# ------

gx = 0
gy = 0
gz = 0

g_dx = 250
g_dy = 200
g_dz = 150

g_rayonGrand = 70
g_rayonPetit = 50

g_trim = 1000

# The parallelepipede
# -------------------

p_boite = MakeBox(gx-g_dx, gy-g_dy, gz-g_dz,  gx+g_dx, gy+g_dy, gz+g_dz)

# The great cylinder
# ------------------

g_base = MakeVertex(gx-g_dx, gy, gz)
g_dir  = MakeVectorDXDYDZ(1, 0, 0)
g_cyl  = MakeCylinder(g_base, g_dir, g_rayonGrand, g_dx*2)

# The first hole
# --------------

b_boite = MakeCut(p_boite , g_cyl)

# Partitioning
# ------------

p_base = MakeVertex(gx, gy, gz)

p_tools = []

p_tools.append(MakePlane(p_base, MakeVectorDXDYDZ(0,  1   , 0   ), g_trim))
p_tools.append(MakePlane(p_base, MakeVectorDXDYDZ(0,  g_dz, g_dy), g_trim))
p_tools.append(MakePlane(p_base, MakeVectorDXDYDZ(0, -g_dz, g_dy), g_trim))

p_tools.append(MakePlane(MakeVertex(gx-g_rayonPetit, gy, gz), g_dir, g_trim))
p_tools.append(MakePlane(MakeVertex(gx+g_rayonPetit, gy, gz), g_dir, g_trim))

p_piece = MakePartition([b_boite], p_tools, [], [], ShapeType["SOLID"])

# The small cylinder
# ------------------

c_cyl = MakeCylinder(p_base, MakeVectorDXDYDZ(0, 0, 1), g_rayonPetit, g_dz)

# The second hole
# ---------------

d_element = SubShapeAllSorted(p_piece, ShapeType["SOLID"])

d_element[ 8] = MakeCut(d_element[ 8], c_cyl)
d_element[10] = MakeCut(d_element[10], c_cyl)

# Compound
# --------

piece = RemoveExtraEdges(MakeCompound(d_element))

# Add piece in study
# ------------------

piece_id = addToStudy(piece, "ex16_cyl2complementary")

# Meshing
# =======

# Create a hexahedral mesh
# ------------------------

hexa = smesh.Mesh(piece, "ex16_cyl2complementary:hexa")

algo = hexa.Segment()
algo.NumberOfSegments(12)

hexa.Quadrangle()

hexa.Hexahedron()

# Define local hypothesis
# -----------------------

def local(x, y, z, d):
    edge = GetEdgeNearPoint(piece, MakeVertex(x, y, z))
    algo = hexa.Segment(edge)
    algo.NumberOfSegments(d)
    algo.Propagation()

local(gx     , gy+g_dy, gz+g_dz, 7)
local(gx+g_dx, gy+g_dy, gz     , 21)
local(gx+g_dx, gy-g_dy, gz     , 21)

# Mesh calculus
# -------------

hexa.Compute()