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
//  File   : StdMeshers_StartEndLength_i.cxx
//           Moved here from SMESH_LocalLength_i.cxx
//  Author : Paul RASCLE, EDF
//  Module : SMESH
//  $Header$

using namespace std;
#include "StdMeshers_StartEndLength_i.hxx"
#include "SMESH_Gen.hxx"

#include "Utils_CorbaException.hxx"
#include "utilities.h"

//=============================================================================
/*!
 *  StdMeshers_StartEndLength_i::StdMeshers_StartEndLength_i
 *
 *  Constructor
 */
//=============================================================================

StdMeshers_StartEndLength_i::StdMeshers_StartEndLength_i( PortableServer::POA_ptr thePOA,
                                                         int                     theStudyId,
                                                         ::SMESH_Gen*            theGenImpl )
     : SALOME::GenericObj_i( thePOA ), 
       SMESH_Hypothesis_i( thePOA )
{
  MESSAGE( "StdMeshers_StartEndLength_i::StdMeshers_StartEndLength_i" );
  myBaseImpl = new ::StdMeshers_StartEndLength( theGenImpl->GetANewId(),
                                               theStudyId,
                                               theGenImpl );
}

//=============================================================================
/*!
 *  StdMeshers_StartEndLength_i::~StdMeshers_StartEndLength_i
 *
 *  Destructor
 */
//=============================================================================

StdMeshers_StartEndLength_i::~StdMeshers_StartEndLength_i()
{
  MESSAGE( "StdMeshers_StartEndLength_i::~StdMeshers_StartEndLength_i" );
}

//=============================================================================
/*!
 *  StdMeshers_StartEndLength_i::SetLength
 *
 *  Set length
 */
//=============================================================================

void StdMeshers_StartEndLength_i::SetLength(CORBA::Double theLength,
                                            CORBA::Boolean theIsStart )
     throw ( SALOME::SALOME_Exception )
{
  MESSAGE( "StdMeshers_StartEndLength_i::SetLength" );
  ASSERT( myBaseImpl );
  try {
    this->GetImpl()->SetLength( theLength, theIsStart );
  }
  catch ( SALOME_Exception& S_ex ) {
    THROW_SALOME_CORBA_EXCEPTION( S_ex.what(),
				  SALOME::BAD_PARAM );
  }
}

//=============================================================================
/*!
 *  StdMeshers_StartEndLength_i::GetLength
 *
 *  Get length
 */
//=============================================================================

CORBA::Double StdMeshers_StartEndLength_i::GetLength( CORBA::Boolean theIsStart)
{
  MESSAGE( "StdMeshers_StartEndLength_i::GetLength" );
  ASSERT( myBaseImpl );
  return this->GetImpl()->GetLength( theIsStart );
}

//=============================================================================
/*!
 *  StdMeshers_StartEndLength_i::GetImpl
 *
 *  Get implementation
 */
//=============================================================================

::StdMeshers_StartEndLength* StdMeshers_StartEndLength_i::GetImpl()
{
  MESSAGE( "StdMeshers_StartEndLength_i::GetImpl" );
  return ( ::StdMeshers_StartEndLength* )myBaseImpl;
}