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
//  File   : SMESH_MaxElementArea_i.cxx
//  Author : Paul RASCLE, EDF
//  Module : SMESH
//  $Header$

using namespace std;
using namespace std;
#include "SMESH_MaxElementArea_i.hxx"
#include "SMESH_Gen.hxx"
#include "SMESH_HypothesisFactory.hxx"

#include "Utils_CorbaException.hxx"
#include "utilities.h"

//=============================================================================
/*!
 *  Constructor: 
 *  _name is related to the class name: prefix = SMESH_ ; suffix = _i .
 */
//=============================================================================

SMESH_MaxElementArea_i::SMESH_MaxElementArea_i(const char* anHyp,
					       int studyId,
					       ::SMESH_Gen* genImpl)
{
  MESSAGE("SMESH_MaxElementArea_i::SMESH_MaxElementArea_i");
  _impl = new ::SMESH_MaxElementArea(genImpl->_hypothesisFactory.GetANewId(),
				     studyId,
				     genImpl);
  _baseImpl = _impl;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

SMESH_MaxElementArea_i::~SMESH_MaxElementArea_i()
{
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

void SMESH_MaxElementArea_i::SetMaxElementArea(CORBA::Double area)
  throw (SALOME::SALOME_Exception)
{
  ASSERT(_impl);
  try
    {
      _impl->SetMaxArea(area);
    }
  catch (SALOME_Exception& S_ex)
    {
      THROW_SALOME_CORBA_EXCEPTION(S_ex.what(), \
				   SALOME::BAD_PARAM);
    }
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

CORBA::Double SMESH_MaxElementArea_i::GetMaxElementArea()
{
  ASSERT(_impl);
  return _impl->GetMaxArea();
}

