# -*- coding: iso-8859-1 -*-

import sys
import salome

salome.salome_init()
theStudy = salome.myStudy

import salome_notebook
notebook = salome_notebook.notebook

import os
from blocFissure import gmu

###
### GEOM component
###

import GEOM
from salome.geom import geomBuilder
import math
import SALOMEDS


geompy = geomBuilder.New(theStudy)
O = geompy.MakeVertex(0, 0, 0)
OX = geompy.MakeVectorDXDYDZ(1, 0, 0)
OY = geompy.MakeVectorDXDYDZ(0, 1, 0)
OZ = geompy.MakeVectorDXDYDZ(0, 0, 1)
geomObj_1 = geompy.MakeMarker(0, 0, 0, 1, 0, 0, 0, 1, 0)
geomObj_2 = geompy.MakeMarker(0, 0, 0, 1, 0, 0, 0, 1, 0)
sk = geompy.Sketcher2D()
sk.addPoint(0.000000, 0.000000)
sk.addSegmentAbsolute(10.000000, 0.000000)
sk.addSegmentAbsolute(10.000000, 98.750000)
sk.addArcAngleRadiusLength(0, -1.250000, 90.000000)
sk.addSegmentAbsolute(15.000000, 100.000000)
sk.addSegmentAbsolute(15.000000, 120.000000)
sk.addSegmentAbsolute(9.945000, 120.000000)
sk.addSegmentAbsolute(9.945000, 108.000000)
sk.addSegmentAbsolute(0.000000, 105.000000)
sk.close()
Sketch_1 = sk.wire(geomObj_2)
Face_1 = geompy.MakeFaceWires([Sketch_1], 1)
Vertex_1 = geompy.MakeVertex(15, 108, 0)
Vertex_2 = geompy.MakeVertex(0, 94, 0)
Vertex_3 = geompy.MakeVertex(10, 94, 0)
Face_1_vertex_17 = geompy.GetSubShape(Face_1, [17])
Line_1 = geompy.MakeLineTwoPnt(Vertex_1, Face_1_vertex_17)
Face_1_vertex_9 = geompy.GetSubShape(Face_1, [9])
Line_2 = geompy.MakeLineTwoPnt(Face_1_vertex_17, Face_1_vertex_9)
Face_1_vertex_19 = geompy.GetSubShape(Face_1, [19])
Face_1_vertex_7 = geompy.GetSubShape(Face_1, [7])
Line_3 = geompy.MakeLineTwoPnt(Face_1_vertex_19, Face_1_vertex_7)
Line_4 = geompy.MakeLineTwoPnt(Vertex_2, Vertex_3)
coupe_vis = geompy.MakePartition([Face_1], [Line_1, Line_2, Line_3, Line_4], [], [], geompy.ShapeType["FACE"], 0, [], 0)
[tige, section, tige_haute, rond, tete, section_tete] = geompy.Propagate(coupe_vis)
conge = geompy.CreateGroup(coupe_vis, geompy.ShapeType["EDGE"])
geompy.UnionIDs(conge, [21])
appui = geompy.CreateGroup(coupe_vis, geompy.ShapeType["EDGE"])
geompy.UnionIDs(appui, [37])
p_imp = geompy.CreateGroup(coupe_vis, geompy.ShapeType["EDGE"])
geompy.UnionIDs(p_imp, [4])

Vertex_4 = geompy.MakeVertex(11.25, 98.75, 0)
Vertex_5 = geompy.MakeVertexWithRef(Vertex_4, -0.5, 0.5, 0)
Vertex_6 = geompy.MakeVertexWithRef(Vertex_4, -5, 5, 0)
Line_5 = geompy.MakeLineTwoPnt(Vertex_5, Vertex_6)
Partition_1 = geompy.MakePartition([Line_5], [conge], [], [], geompy.ShapeType["EDGE"], 0, [], 1)
[Vertex_7] = geompy.SubShapes(Partition_1, [4])
Vertex_8 = geompy.MakeVertexWithRef(Vertex_7, -1.1, 1.1, 0)
generatrice = geompy.MakeLineTwoPnt(Vertex_5, Vertex_8)
Revolution_1 = geompy.MakeRevolution2Ways(generatrice, OY, 60*math.pi/180.0)
Partition_2 = geompy.MakePartition([Revolution_1], [conge], [], [], geompy.ShapeType["FACE"], 0, [], 1)
Partition_2_vertex_11 = geompy.GetSubShape(Partition_2, [11])
Plane_1 = geompy.MakePlane(Partition_2_vertex_11, OY, 2000)
Partition_3 = geompy.MakePartition([Revolution_1], [Plane_1], [], [], geompy.ShapeType["FACE"], 0, [], 0)
Vertex_9 = geompy.MakeVertex(0, 99.633883, 8.977320000000001)
Vertex_10 = geompy.MakeVertex(0, 99.633883, -8.977320000000001)
Vertex_11 = geompy.MakeVertexWithRef(Vertex_9, 0, 0, -1)
Vertex11x = geompy.MakeVertexWithRef(Vertex_11, 1, 0, 0)
Line_11x = geompy.MakeLineTwoPnt(Vertex_11, Vertex11x)
Vertex_12 = geompy.MakeVertexWithRef(Vertex_10, 0, 0, 1)
Vertex12x = geompy.MakeVertexWithRef(Vertex_12, 1, 0, 0)
Line_12x = geompy.MakeLineTwoPnt(Vertex_12, Vertex12x)
Vertex_13 = geompy.MakeVertexWithRef(Vertex_11, 0, 1, 0)
Vertex_14 = geompy.MakeVertexWithRef(Vertex_12, 0, 1, 0)
Vertex_15 = geompy.MakeRotation(Vertex_9, Line_11x, 30*math.pi/180.0)
Vertex_16 = geompy.MakeRotation(Vertex_10, Line_12x, -30*math.pi/180.0)
Arc_1 = geompy.MakeArc(Vertex_15, Vertex_9, Vertex_13,False)
Arc_2 = geompy.MakeArc(Vertex_14, Vertex_10, Vertex_16,False)
Line_6 = geompy.MakeLineTwoPnt(Vertex_13, Vertex_14)
Line_8 = geompy.MakeLineTwoPnt(Vertex_16, Vertex_15)
Wire_1 = geompy.MakeWire([Arc_1, Arc_2, Line_6, Line_8], 1e-07)
Face_2 = geompy.MakeFaceWires([Wire_1], 1)
Extrusion_1 = geompy.MakePrismVecH(Face_2, OX, 15)
Revolution_2 = geompy.MakeRevolution2Ways(generatrice, OY, 65*math.pi/180.0)
Fissure = geompy.MakeCommonList([Extrusion_1, Revolution_2], True)
fondFiss = geompy.CreateGroup(Fissure, geompy.ShapeType["EDGE"])
geompy.UnionIDs(fondFiss, [9, 7, 4])
geompy.Export(Fissure, os.path.join(gmu.pathBloc, "materielCasTests/visFiss.brep"), "BREP")

geompy.addToStudy( O, 'O' )
geompy.addToStudy( OX, 'OX' )
geompy.addToStudy( OY, 'OY' )
geompy.addToStudy( OZ, 'OZ' )
geompy.addToStudy( Sketch_1, 'Sketch_1' )
geompy.addToStudy( Face_1, 'Face_1' )
geompy.addToStudy( Vertex_1, 'Vertex_1' )
geompy.addToStudy( Vertex_2, 'Vertex_2' )
geompy.addToStudy( Vertex_3, 'Vertex_3' )
geompy.addToStudyInFather( Face_1, Face_1_vertex_17, 'Face_1:vertex_17' )
geompy.addToStudy( Line_1, 'Line_1' )
geompy.addToStudyInFather( Face_1, Face_1_vertex_9, 'Face_1:vertex_9' )
geompy.addToStudy( Line_2, 'Line_2' )
geompy.addToStudyInFather( Face_1, Face_1_vertex_19, 'Face_1:vertex_19' )
geompy.addToStudyInFather( Face_1, Face_1_vertex_7, 'Face_1:vertex_7' )
geompy.addToStudy( Line_3, 'Line_3' )
geompy.addToStudy( Line_4, 'Line_4' )
geompy.addToStudy( coupe_vis, 'coupe_vis' )
geompy.addToStudyInFather( coupe_vis, tige, 'tige' )
geompy.addToStudyInFather( coupe_vis, section, 'section' )
geompy.addToStudyInFather( coupe_vis, tige_haute, 'tige_haute' )
geompy.addToStudyInFather( coupe_vis, rond, 'rond' )
geompy.addToStudyInFather( coupe_vis, tete, 'tete' )
geompy.addToStudyInFather( coupe_vis, section_tete, 'section_tete' )
geompy.addToStudyInFather( coupe_vis, conge, 'conge' )
geompy.addToStudyInFather( coupe_vis, appui, 'appui' )
geompy.addToStudyInFather( coupe_vis, p_imp, 'p_imp' )

geompy.addToStudy( Vertex_4, 'Vertex_4' )
geompy.addToStudy( Vertex_6, 'Vertex_6' )
geompy.addToStudy( Vertex_5, 'Vertex_5' )
geompy.addToStudy( Line_5, 'Line_5' )
geompy.addToStudy( Partition_1, 'Partition_1' )
geompy.addToStudyInFather( Partition_1, Vertex_7, 'Vertex_7' )
geompy.addToStudy( Vertex_8, 'Vertex_8' )
geompy.addToStudy( generatrice, 'generatrice' )
geompy.addToStudy( Revolution_1, 'Revolution_1' )
geompy.addToStudy( Partition_2, 'Partition_2' )
geompy.addToStudyInFather( Partition_2, Partition_2_vertex_11, 'Partition_2:vertex_11' )
geompy.addToStudy( Plane_1, 'Plane_1' )
geompy.addToStudy( Partition_3, 'Partition_3' )
geompy.addToStudy( Vertex_9, 'Vertex_9' )
geompy.addToStudy( Vertex_10, 'Vertex_10' )
geompy.addToStudy( Vertex_11, 'Vertex_11' )
geompy.addToStudy( Vertex_12, 'Vertex_12' )
geompy.addToStudy( Vertex_13, 'Vertex_13' )
geompy.addToStudy( Vertex_14, 'Vertex_14' )
geompy.addToStudy( Vertex_15, 'Vertex_15' )
geompy.addToStudy( Vertex_16, 'Vertex_16' )
geompy.addToStudy( Arc_1, 'Arc_1' )
geompy.addToStudy( Arc_2, 'Arc_2' )
geompy.addToStudy( Line_6, 'Line_6' )
geompy.addToStudy( Line_8, 'Line_8' )
geompy.addToStudy( Wire_1, 'Wire_1' )
geompy.addToStudy( Face_2, 'Face_2' )
geompy.addToStudy( Extrusion_1, 'Extrusion_1' )
geompy.addToStudy( Revolution_2, 'Revolution_2' )
geompy.addToStudy( Fissure, 'Fissure' )
geompy.addToStudyInFather( Fissure, fondFiss, 'fondFiss' )

###
### SMESH component
###

import  SMESH, SALOMEDS
from salome.smesh import smeshBuilder

smesh = smeshBuilder.New(theStudy)
coupe_vis_1 = smesh.Mesh(coupe_vis)
Regular_1D = coupe_vis_1.Segment()
Nb_Segments_1 = Regular_1D.NumberOfSegments(10)
Nb_Segments_1.SetDistrType( 0 )
Quadrangle_2D = coupe_vis_1.Quadrangle(algo=smeshBuilder.QUADRANGLE)
isDone = coupe_vis_1.Compute()
Nb_Segments_2 = smesh.CreateHypothesis('NumberOfSegments')
Nb_Segments_2.SetNumberOfSegments( 30 )
Nb_Segments_2.SetDistrType( 0 )
status = coupe_vis_1.AddHypothesis(Regular_1D,tige)
status = coupe_vis_1.AddHypothesis(Nb_Segments_2,tige)
isDone = coupe_vis_1.Compute()
Nb_Segments_3 = smesh.CreateHypothesis('NumberOfSegments')
Nb_Segments_3.SetNumberOfSegments( 10 )
Nb_Segments_3.SetScaleFactor( 3 )
Nb_Segments_3.SetReversedEdges( [  ] )
Nb_Segments_3.SetObjectEntry( "0:1:1:14" )
status = coupe_vis_1.AddHypothesis(Regular_1D,section)
status = coupe_vis_1.AddHypothesis(Nb_Segments_3,section)
coupe_vis_1.Clear()
isDone = coupe_vis_1.Compute()
coupe_vis_1.Clear()
Nb_Segments_3.SetNumberOfSegments( 10 )
Nb_Segments_3.SetDistrType( 1 )
Nb_Segments_3.SetScaleFactor( 3 )
Nb_Segments_3.SetReversedEdges( [ 23, 4, 9, 16 ] )
Nb_Segments_3.SetObjectEntry( "0:1:1:14" )
isDone = coupe_vis_1.Compute()
tige_1 = coupe_vis_1.GroupOnGeom(tige,'tige',SMESH.EDGE)
section_1 = coupe_vis_1.GroupOnGeom(section,'section',SMESH.EDGE)
tige_haute_1 = coupe_vis_1.GroupOnGeom(tige_haute,'tige_haute',SMESH.EDGE)
rond_1 = coupe_vis_1.GroupOnGeom(rond,'rond',SMESH.EDGE)
tete_1 = coupe_vis_1.GroupOnGeom(tete,'tete',SMESH.EDGE)
section_tete_1 = coupe_vis_1.GroupOnGeom(section_tete,'section_tete',SMESH.EDGE)
conge_1 = coupe_vis_1.GroupOnGeom(conge,'conge',SMESH.EDGE)
appui_1 = coupe_vis_1.GroupOnGeom(appui,'appui',SMESH.EDGE)
p_imp_1 = coupe_vis_1.GroupOnGeom(p_imp,'p_imp',SMESH.EDGE)
isDone = coupe_vis_1.SplitQuad( [ 691 ], 1 )
visHex80 = smesh.CopyMesh( coupe_vis_1, 'visHex80', 1, 0)
[ tige_2, section_2, tige_haute_2, rond_2, tete_2, section_tete_2, conge_2, appui_2, p_imp_2 ] = visHex80.GetGroups()
[ tige_rotated, section_rotated, tige_haute_rotated, rond_rotated, tete_rotated, section_tete_rotated, conge_rotated, appui_rotated, p_imp_rotated, tige_top, section_top, tige_haute_top, rond_top, tete_top, section_tete_top, conge_top, appui_top, p_imp_top ] = visHex80.RotationSweepObject2D( visHex80, SMESH.AxisStruct( 0, 0, 0, 0, 10, 0 ), math.pi/40, 40, 1e-05 ,True)
[ tige_2, section_2, tige_haute_2, rond_2, tete_2, section_tete_2, conge_2, appui_2, p_imp_2, tige_rotated, tige_top, section_rotated, section_top, tige_haute_rotated, tige_haute_top, rond_rotated, rond_top, tete_rotated, tete_top, section_tete_rotated, section_tete_top, conge_rotated, conge_top, appui_rotated, appui_top, p_imp_rotated, p_imp_top ] = visHex80.GetGroups()
[ tige_mirrored, section_mirrored, tige_haute_mirrored, rond_mirrored, tete_mirrored, section_tete_mirrored, conge_mirrored, appui_mirrored, p_imp_mirrored, tige_rotated_mirrored, tige_top_mirrored, section_rotated_mirrored, section_top_mirrored, tige_haute_rotated_mirrored, tige_haute_top_mirrored, rond_rotated_mirrored, rond_top_mirrored, tete_rotated_mirrored, tete_top_mirrored, section_tete_rotated_mirrored, section_tete_top_mirrored, conge_rotated_mirrored, conge_top_mirrored, appui_rotated_mirrored, appui_top_mirrored, p_imp_rotated_mirrored, p_imp_top_mirrored ] = visHex80.MirrorObject( visHex80, SMESH.AxisStruct( 0, 0, 0, 0, 0, 10 ), SMESH.SMESH_MeshEditor.PLANE ,True,True)
[ tige_2, section_2, tige_haute_2, rond_2, tete_2, section_tete_2, conge_2, appui_2, p_imp_2, tige_rotated, tige_top, section_rotated, section_top, tige_haute_rotated, tige_haute_top, rond_rotated, rond_top, tete_rotated, tete_top, section_tete_rotated, section_tete_top, conge_rotated, conge_top, appui_rotated, appui_top, p_imp_rotated, p_imp_top, tige_mirrored, section_mirrored, tige_haute_mirrored, rond_mirrored, tete_mirrored, section_tete_mirrored, conge_mirrored, appui_mirrored, p_imp_mirrored, tige_rotated_mirrored, tige_top_mirrored, section_rotated_mirrored, section_top_mirrored, tige_haute_rotated_mirrored, tige_haute_top_mirrored, rond_rotated_mirrored, rond_top_mirrored, tete_rotated_mirrored, tete_top_mirrored, section_tete_rotated_mirrored, section_tete_top_mirrored, conge_rotated_mirrored, conge_top_mirrored, appui_rotated_mirrored, appui_top_mirrored, p_imp_rotated_mirrored, p_imp_top_mirrored ] = visHex80.GetGroups()
coincident_nodes_on_part = visHex80.FindCoincidentNodesOnPart( visHex80, 1e-05, [  ] )
visHex80.MergeNodes(coincident_nodes_on_part)
equal_elements = visHex80.FindEqualElements( visHex80 )
visHex80.MergeElements(equal_elements)
[ tige_2, section_2, tige_haute_2, rond_2, tete_2, section_tete_2, conge_2, appui_2, p_imp_2, tige_rotated, tige_top, section_rotated, section_top, tige_haute_rotated, tige_haute_top, rond_rotated, rond_top, tete_rotated, tete_top, section_tete_rotated, section_tete_top, conge_rotated, conge_top, appui_rotated, appui_top, p_imp_rotated, p_imp_top, tige_mirrored, section_mirrored, tige_haute_mirrored, rond_mirrored, tete_mirrored, section_tete_mirrored, conge_mirrored, appui_mirrored, p_imp_mirrored, tige_rotated_mirrored, tige_top_mirrored, section_rotated_mirrored, section_top_mirrored, tige_haute_rotated_mirrored, tige_haute_top_mirrored, rond_rotated_mirrored, rond_top_mirrored, tete_rotated_mirrored, tete_top_mirrored, section_tete_rotated_mirrored, section_tete_top_mirrored, conge_rotated_mirrored, conge_top_mirrored, appui_rotated_mirrored, appui_top_mirrored, p_imp_rotated_mirrored, p_imp_top_mirrored ] = visHex80.GetGroups()
SubMesh_1 = coupe_vis_1.GetSubMesh( tige, 'SubMesh_1' )
SubMesh_2 = coupe_vis_1.GetSubMesh( section, 'SubMesh_2' )
visHex80.ExportMED( os.path.join(gmu.pathBloc, "materielCasTests/visSain.med"), 0, SMESH.MED_V2_2, 1 )


## Set names of Mesh objects
smesh.SetName(appui_rotated, 'appui_rotated')
smesh.SetName(p_imp_rotated, 'p_imp_rotated')
smesh.SetName(section_tete_rotated, 'section_tete_rotated')
smesh.SetName(conge_rotated, 'conge_rotated')
smesh.SetName(rond_rotated, 'rond_rotated')
smesh.SetName(tete_rotated, 'tete_rotated')
smesh.SetName(section_rotated, 'section_rotated')
smesh.SetName(tige_haute_rotated, 'tige_haute_rotated')
smesh.SetName(tige_rotated, 'tige_rotated')
smesh.SetName(SubMesh_2, 'SubMesh_2')
smesh.SetName(SubMesh_1, 'SubMesh_1')
smesh.SetName(Regular_1D.GetAlgorithm(), 'Regular_1D')
smesh.SetName(Quadrangle_2D.GetAlgorithm(), 'Quadrangle_2D')
smesh.SetName(appui_1, 'appui')
smesh.SetName(p_imp_1, 'p_imp')
smesh.SetName(coupe_vis_1.GetMesh(), 'coupe_vis')
smesh.SetName(visHex80.GetMesh(), 'visHex80')
smesh.SetName(tige_1, 'tige')
smesh.SetName(p_imp_2, 'p_imp')
smesh.SetName(section_1, 'section')
smesh.SetName(appui_2, 'appui')
smesh.SetName(tige_haute_1, 'tige_haute')
smesh.SetName(conge_2, 'conge')
smesh.SetName(rond_1, 'rond')
smesh.SetName(section_tete_2, 'section_tete')
smesh.SetName(tete_1, 'tete')
smesh.SetName(tete_2, 'tete')
smesh.SetName(section_tete_1, 'section_tete')
smesh.SetName(rond_2, 'rond')
smesh.SetName(conge_1, 'conge')
smesh.SetName(tige_haute_2, 'tige_haute')
smesh.SetName(section_2, 'section')
smesh.SetName(tige_2, 'tige')
smesh.SetName(p_imp_top, 'p_imp_top')
smesh.SetName(Nb_Segments_3, 'Nb. Segments_3')
smesh.SetName(conge_top, 'conge_top')
smesh.SetName(Nb_Segments_2, 'Nb. Segments_2')
smesh.SetName(appui_top, 'appui_top')
smesh.SetName(Nb_Segments_1, 'Nb. Segments_1')
smesh.SetName(tete_top, 'tete_top')
smesh.SetName(section_tete_top, 'section_tete_top')
smesh.SetName(tige_haute_top, 'tige_haute_top')
smesh.SetName(rond_top, 'rond_top')
smesh.SetName(tige_top, 'tige_top')
smesh.SetName(section_top, 'section_top')

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser(1)
