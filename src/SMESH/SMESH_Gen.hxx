//  SMESH SMESH : implementaion of SMESH idl descriptions
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : SMESH_Gen.hxx
//  Author : Paul RASCLE, EDF
//  Module : SMESH
//  $Header$

#ifndef _SMESH_GEN_HXX_
#define _SMESH_GEN_HXX_

#include "Utils_SALOME_Exception.hxx"

#include "SMESH_Hypothesis.hxx"
#include "SMESH_Algo.hxx"
#include "SMESH_1D_Algo.hxx"
#include "SMESH_2D_Algo.hxx"
#include "SMESH_3D_Algo.hxx"
#include "SMESH_Mesh.hxx"

#include "SMESHDS_Document.hxx"

#include <TopoDS_Shape.hxx>

#include <map>


typedef struct studyContextStruct
{
	std::map < int, SMESH_Hypothesis * >mapHypothesis;
	  std::map < int, SMESH_Mesh * >mapMesh;
	  SMESHDS_Document * myDocument;
} StudyContextStruct;

class SMESH_Gen
{
 public:
  SMESH_Gen();
  ~SMESH_Gen();

//  SMESH_Hypothesis *CreateHypothesis(const char *anHyp, int studyId)
//    throw(SALOME_Exception);
  SMESH_Mesh* CreateMesh(int studyId)
    throw(SALOME_Exception);
  bool Compute(::SMESH_Mesh & aMesh, const TopoDS_Shape & aShape);

  bool CheckAlgoState(SMESH_Mesh& aMesh, const TopoDS_Shape& aShape);
  // notify on bad state of attached algos, return false
  // if Compute() would fail because of some algo bad state


  StudyContextStruct *GetStudyContext(int studyId);

  static int GetShapeDim(const TopAbs_ShapeEnum & aShapeType);
  static int GetShapeDim(const TopoDS_Shape & aShape)
  { return GetShapeDim( aShape.ShapeType() ); }
  SMESH_Algo* GetAlgo(SMESH_Mesh & aMesh, const TopoDS_Shape & aShape);
  static bool IsGlobalAlgo(const SMESH_Algo* theAlgo, SMESH_Mesh& aMesh);

  // inherited methods from SALOMEDS::Driver

  void Save(int studyId, const char *aUrlOfFile);
  void Load(int studyId, const char *aUrlOfFile);
  void Close(int studyId);
  const char *ComponentDataType();

  const char *IORToLocalPersistentID(const char *IORString, bool & IsAFile);
  const char *LocalPersistentIDToIOR(const char *aLocalPersistentID);

  int GetANewId();

  std::map < int, SMESH_Algo * >_mapAlgo;
  std::map < int, SMESH_1D_Algo * >_map1D_Algo;
  std::map < int, SMESH_2D_Algo * >_map2D_Algo;
  std::map < int, SMESH_3D_Algo * >_map3D_Algo;

 private:

  int _localId;				// unique Id of created objects, within SMESH_Gen entity
  std::map < int, StudyContextStruct * >_mapStudyContext;

  // hypotheses managing
  int _hypId;
};

#endif
