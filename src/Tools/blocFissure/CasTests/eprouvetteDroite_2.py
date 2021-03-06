# -*- coding: utf-8 -*-

import os
from blocFissure import gmu
from blocFissure.gmu.geomsmesh import geompy, smesh

import math
import GEOM
import SALOMEDS
import SMESH
#import StdMeshers
#import GHS3DPlugin
#import NETGENPlugin
import logging

from eprouvetteDroite import eprouvetteDroite

from blocFissure.gmu.triedreBase import triedreBase
from blocFissure.gmu.genereMeshCalculZoneDefaut import genereMeshCalculZoneDefaut
from blocFissure.gmu.creeZoneDefautDansObjetSain import creeZoneDefautDansObjetSain
from blocFissure.gmu.insereFissureGenerale import insereFissureGenerale

O, OX, OY, OZ = triedreBase()

class eprouvetteDroite_2(eprouvetteDroite):
  """
  problème de fissure plane coupant 3 faces (éprouvette), débouches non normaux, objet plan
  """

  nomProbleme = "eprouvetteDroite2"

  # ---------------------------------------------------------------------------
  def genereShapeFissure( self, geometriesSaines, geomParams, shapeFissureParams):
    logging.info("genereShapeFissure %s", self.nomCas)

    lgInfluence = shapeFissureParams['lgInfluence']

    shellFiss = geompy.ImportFile(os.path.join(gmu.pathBloc, "materielCasTests/EprouvetteDroiteFiss_2.brep"), "BREP")
    fondFiss = geompy.CreateGroup(shellFiss, geompy.ShapeType["EDGE"])
    geompy.UnionIDs(fondFiss, [10])
    geompy.addToStudy( shellFiss, 'shellFiss' )
    geompy.addToStudyInFather( shellFiss, fondFiss, 'fondFiss' )


    coordsNoeudsFissure = genereMeshCalculZoneDefaut(shellFiss, 5 ,10)

    centre = None
    return [shellFiss, centre, lgInfluence, coordsNoeudsFissure, fondFiss]

  # ---------------------------------------------------------------------------
  def setReferencesMaillageFissure(self):
    self.referencesMaillageFissure = dict(Entity_Quad_Pyramid    = 396,
                                          Entity_Quad_Triangle   = 1084,
                                          Entity_Quad_Edge       = 510,
                                          Entity_Quad_Penta      = 96,
                                          Entity_Quad_Hexa       = 9504,
                                          Entity_Node            = 55482,
                                          Entity_Quad_Tetra      = 7545,
                                          Entity_Quad_Quadrangle = 3724)

