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
//  File   : StdMeshers_AutomaticLength.hxx
//  Author : Edward AGAPOV, OCC
//  Module : SMESH
//  $Header$

#ifndef _SMESH_AutomaticLength_HXX_
#define _SMESH_AutomaticLength_HXX_

#include "SMESH_Hypothesis.hxx"
#include "Utils_SALOME_Exception.hxx"

#include <map>

class SMESH_Mesh;
class TopoDS_Shape;
class TopoDS_TShape;

/*!
 * \brief 1D Hypothesis to compute segment length free of thinking
 *
 * It computes segment length basing on max shape size to shortest edge length ratio:
 * S = S0 * f(L/Lmin) where f(x) = 1 + (2/Pi * 7 * atan(x/5) )
 */

class StdMeshers_AutomaticLength:public SMESH_Hypothesis
{
public:
  StdMeshers_AutomaticLength(int hypId, int studyId, SMESH_Gen * gen);
  virtual ~ StdMeshers_AutomaticLength();

  double GetLength(const SMESH_Mesh* aMesh, const TopoDS_Shape& anEdge)
    throw(SALOME_Exception);

  virtual std::ostream & SaveTo(std::ostream & save);
  virtual std::istream & LoadFrom(std::istream & load);
  friend std::ostream & operator <<(std::ostream & save, StdMeshers_AutomaticLength & hyp);
  friend std::istream & operator >>(std::istream & load, StdMeshers_AutomaticLength & hyp);

protected:
  std::map<const TopoDS_TShape*, double> _TShapeToLength;
  const SMESH_Mesh* _mesh;
};

#endif
