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
//  File   : StdMeshers_Quadrangle_2D_i.cxx
//           Moved here from SMESH_Quadrangle_2D_i.cxx
//  Author : Paul RASCLE, EDF
//  Module : SMESH
//  $Header$

using namespace std;
#include "StdMeshers_Quadrangle_2D_i.hxx"
#include "SMESH_Gen.hxx"

#include "Utils_CorbaException.hxx"
#include "utilities.h"

//=============================================================================
/*!
 *  StdMeshers_Quadrangle_2D_i::StdMeshers_Quadrangle_2D_i
 *
 *  Constructor
 */
//=============================================================================

StdMeshers_Quadrangle_2D_i::StdMeshers_Quadrangle_2D_i( PortableServer::POA_ptr thePOA,
					      int                     theStudyId,
					      ::SMESH_Gen*            theGenImpl )
     : SALOME::GenericObj_i( thePOA ), 
       SMESH_Hypothesis_i( thePOA ), 
       SMESH_Algo_i( thePOA ),
       SMESH_2D_Algo_i( thePOA )
{
  MESSAGE( "StdMeshers_Quadrangle_2D_i::StdMeshers_Quadrangle_2D_i" );
  myBaseImpl = new ::StdMeshers_Quadrangle_2D( theGenImpl->GetANewId(),
					  theStudyId,
					  theGenImpl );
}

//=============================================================================
/*!
 *  StdMeshers_Quadrangle_2D_i::~StdMeshers_Quadrangle_2D_i
 *
 *  Destructor
 *  
 */
//=============================================================================

StdMeshers_Quadrangle_2D_i::~StdMeshers_Quadrangle_2D_i()
{
  MESSAGE( "StdMeshers_Quadrangle_2D_i::~StdMeshers_Quadrangle_2D_i" );
}

//=============================================================================
/*!
 *  StdMeshers_Quadrangle_2D_i::GetImpl
 *
 *  Get implementation
 */
//=============================================================================

::StdMeshers_Quadrangle_2D* StdMeshers_Quadrangle_2D_i::GetImpl()
{
  MESSAGE( "StdMeshers_Quadrangle_2D_i::GetImpl" );
  return ( ::StdMeshers_Quadrangle_2D* )myBaseImpl;
}
