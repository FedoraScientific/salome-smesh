//  SMESH SMESHDS : management of mesh data and SMESH document
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
//  File   : SMESHDS_CommandType.hxx
//  Module : SMESH

#ifndef _SMESHDS_CommandType_HeaderFile
#define _SMESHDS_CommandType_HeaderFile

//#include <Standard_PrimitiveTypes.hxx>

enum SMESHDS_CommandType { 
 SMESHDS_AddNode,
 SMESHDS_AddEdge,
 SMESHDS_AddTriangle,
 SMESHDS_AddQuadrangle,
 SMESHDS_AddTetrahedron,
 SMESHDS_AddPyramid,
 SMESHDS_AddPrism,
 SMESHDS_AddHexahedron,
 SMESHDS_RemoveNode,
 SMESHDS_RemoveElement,
 SMESHDS_MoveNode,
 SMESHDS_ChangeElementNodes,
 SMESHDS_Renumber
};


#endif
