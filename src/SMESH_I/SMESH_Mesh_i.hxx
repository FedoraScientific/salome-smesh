//  SMESH SMESH_I : idl implementation based on 'SMESH' unit's calsses
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
//  File   : SMESH_Mesh_i.hxx
//  Author : Paul RASCLE, EDF
//  Module : SMESH
//  $Header$

#ifndef _SMESH_MESH_I_HXX_
#define _SMESH_MESH_I_HXX_

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SMESH_Mesh)
#include CORBA_SERVER_HEADER(SMESH_Group)
#include CORBA_SERVER_HEADER(SMESH_Hypothesis)
#include CORBA_CLIENT_HEADER(GEOM_Gen)
#include CORBA_CLIENT_HEADER(GEOM_Shape)
#include CORBA_CLIENT_HEADER(MED)

class SMESH_Gen_i;
class SMESH_Group_i;

#include "SMESH_Hypothesis.hxx"
#include "SMESH_Mesh.hxx"
#include "SMESH_subMesh_i.hxx"
#include "SMESH_subMesh.hxx"
#include "SMESH_topo.hxx"

#include <map>

#include "SALOME_GenericObj_i.hh"

class SMESH_Mesh_i:
  public virtual POA_SMESH::SMESH_Mesh,
  public virtual SALOME::GenericObj_i
{
public:
  SMESH_Mesh_i();
  SMESH_Mesh_i( PortableServer::POA_ptr thePOA,
                SMESH_Gen_i*            myGen_i,
	        CORBA::Long             studyId );

  virtual ~SMESH_Mesh_i();

  // --- CORBA
  void SetShape( GEOM::GEOM_Shape_ptr theShape )
    throw (SALOME::SALOME_Exception);

  SMESH::Hypothesis_Status AddHypothesis(GEOM::GEOM_Shape_ptr aSubShape,
                                         SMESH::SMESH_Hypothesis_ptr anHyp)
    throw (SALOME::SALOME_Exception);

  SMESH::Hypothesis_Status RemoveHypothesis(GEOM::GEOM_Shape_ptr aSubShape,
                                            SMESH::SMESH_Hypothesis_ptr anHyp)
    throw (SALOME::SALOME_Exception);

  SMESH::ListOfHypothesis* GetHypothesisList(GEOM::GEOM_Shape_ptr aSubShape)
    throw (SALOME::SALOME_Exception);

  SMESH::SMESH_subMesh_ptr GetSubMesh(GEOM::GEOM_Shape_ptr aSubShape, const char* theName)
    throw (SALOME::SALOME_Exception);

  void RemoveSubMesh( SMESH::SMESH_subMesh_ptr theSubMesh )
    throw (SALOME::SALOME_Exception);

  SMESH::SMESH_Group_ptr CreateGroup( SMESH::ElementType theElemType, const char* theName )
    throw (SALOME::SALOME_Exception);
  
  void RemoveGroup( SMESH::SMESH_Group_ptr theGroup )
    throw (SALOME::SALOME_Exception);

//    SMESH::string_array* GetLog(CORBA::Boolean clearAfterGet)
//      throw (SALOME::SALOME_Exception);

  SMESH::log_array* GetLog(CORBA::Boolean clearAfterGet)
    throw (SALOME::SALOME_Exception);

  SMESH::SMESH_MeshEditor_ptr GetMeshEditor();

  void ClearLog()
    throw (SALOME::SALOME_Exception);

  CORBA::Long GetId()
    throw (SALOME::SALOME_Exception);

  CORBA::Long GetStudyId()
    throw (SALOME::SALOME_Exception);

  // --- C++ interface

  void SetImpl(::SMESH_Mesh* impl);
  ::SMESH_Mesh& GetImpl();         // :: force no namespace here

  SMESH_Gen_i* GetGen() { return _gen_i; }
  
  /*!
   * consult DriverMED_R_SMESHDS_Mesh::ReadStatus for returned value
   */
  SMESH::DriverMED_ReadStatus ImportMEDFile( const char* theFileName, const char* theMeshName )
    throw (SALOME::SALOME_Exception);

  void ExportMED( const char* file, CORBA::Boolean auto_groups )
    throw (SALOME::SALOME_Exception);
  void ExportDAT( const char* file )
    throw (SALOME::SALOME_Exception);
  void ExportUNV( const char* file )
    throw (SALOME::SALOME_Exception);

  SALOME_MED::MESH_ptr GetMEDMesh()
    throw (SALOME::SALOME_Exception);
  
  CORBA::Long NbNodes()
    throw (SALOME::SALOME_Exception);
  
  CORBA::Long NbEdges()
    throw (SALOME::SALOME_Exception);
  
  CORBA::Long NbFaces()
    throw (SALOME::SALOME_Exception);

  CORBA::Long NbTriangles()
    throw (SALOME::SALOME_Exception);

  CORBA::Long NbQuadrangles()
    throw (SALOME::SALOME_Exception);
  
  CORBA::Long NbVolumes()
    throw (SALOME::SALOME_Exception);

  CORBA::Long NbTetras()
    throw (SALOME::SALOME_Exception);

  CORBA::Long NbHexas()
    throw (SALOME::SALOME_Exception);
  
  CORBA::Long NbPyramids()
    throw (SALOME::SALOME_Exception);
  
  CORBA::Long NbPrisms()
    throw (SALOME::SALOME_Exception);
  
  CORBA::Long NbSubMesh()
    throw (SALOME::SALOME_Exception);

  char* Dump();
  
  // Internal methods not available through CORBA
  // They are called by corresponding interface methods
  SMESH_Hypothesis::Hypothesis_Status addHypothesis(GEOM::GEOM_Shape_ptr aSubShape,
                                                    SMESH::SMESH_Hypothesis_ptr anHyp);

  SMESH_Hypothesis::Hypothesis_Status removeHypothesis(GEOM::GEOM_Shape_ptr aSubShape,
                                                       SMESH::SMESH_Hypothesis_ptr anHyp);
  
  bool setShape( GEOM::GEOM_Shape_ptr theShape );

  int importMEDFile( const char* theFileName, const char* theMeshName );

  SMESH::SMESH_subMesh_ptr createSubMesh( GEOM::GEOM_Shape_ptr theSubShape );

  void removeSubMesh( SMESH::SMESH_subMesh_ptr theSubMesh, GEOM::GEOM_Shape_ptr theSubShape );

  SMESH::SMESH_Group_ptr createGroup( SMESH::ElementType theElemType, const char* theName );

  void removeGroup( const int theId );

  map<int, SMESH_subMesh_i*> _mapSubMesh_i; //NRI
  map<int, ::SMESH_subMesh*> _mapSubMesh;   //NRI

private:
  static int myIdGenerator;
  ::SMESH_Mesh* _impl;  // :: force no namespace here
  SMESH_Gen_i* _gen_i;
  int _id;          // id given by creator (unique within the creator instance)
  int _studyId;
  map<int, SMESH::SMESH_subMesh_ptr>    _mapSubMeshIor;
  map<int, SMESH::SMESH_Group_ptr>      _mapGroups;
  map<int, SMESH::SMESH_Hypothesis_ptr> _mapHypo;
};

#endif

