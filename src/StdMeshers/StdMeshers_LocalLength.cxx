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
//  File   : StdMeshers_LocalLength.cxx
//           Moved here from SMESH_LocalLength.cxx
//  Author : Paul RASCLE, EDF
//  Module : SMESH
//  $Header$

using namespace std;
#include "StdMeshers_LocalLength.hxx"
#include "utilities.h"

//=============================================================================
/*!
 *  
 */
//=============================================================================

StdMeshers_LocalLength::StdMeshers_LocalLength(int hypId, int studyId,
	SMESH_Gen * gen):SMESH_Hypothesis(hypId, studyId, gen)
{
	_length = 1.;
	_name = "LocalLength";
//   SCRUTE(_name);
//   SCRUTE(&_name);
        _param_algo_dim = 1; // is used by SMESH_Regular_1D
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

StdMeshers_LocalLength::~StdMeshers_LocalLength()
{
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

void StdMeshers_LocalLength::SetLength(double length) throw(SALOME_Exception)
{
	double oldLength = _length;
	if (length <= 0)
		throw SALOME_Exception(LOCALIZED("length must be positive"));
	_length = length;
	if (oldLength != _length)
		NotifySubMeshesHypothesisModification();
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

double StdMeshers_LocalLength::GetLength() const
{
	return _length;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

ostream & StdMeshers_LocalLength::SaveTo(ostream & save)
{
  save << this->_length;
  return save;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

istream & StdMeshers_LocalLength::LoadFrom(istream & load)
{
  bool isOK = true;
  double a;
  isOK = (load >> a);
  if (isOK)
    this->_length = a;
  else
    load.clear(ios::badbit | load.rdstate());
  return load;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

ostream & operator <<(ostream & save, StdMeshers_LocalLength & hyp)
{
  return hyp.SaveTo( save );
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

istream & operator >>(istream & load, StdMeshers_LocalLength & hyp)
{
  return hyp.LoadFrom( load );
}