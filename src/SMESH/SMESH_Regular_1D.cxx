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
//  File   : SMESH_Regular_1D.cxx
//  Author : Paul RASCLE, EDF
//  Module : SMESH
//  $Header$

using namespace std;
using namespace std;
#include "SMESH_Regular_1D.hxx"
#include "SMESH_Gen.hxx"
#include "SMESH_Mesh.hxx"

#include "SMESH_LocalLength.hxx"
#include "SMESH_NumberOfSegments.hxx"

#include "SMESHDS_ListOfPtrHypothesis.hxx"
#include "SMESHDS_ListIteratorOfListOfPtrHypothesis.hxx"
#include "SMDS_MeshElement.hxx"
#include "SMDS_MeshNode.hxx"
#include "SMDS_EdgePosition.hxx"

#include "utilities.h"

#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <BRep_Tool.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GCPnts_UniformAbscissa.hxx>

#include <string>
#include <algorithm>

//=============================================================================
/*!
 *  
 */
//=============================================================================

SMESH_Regular_1D::SMESH_Regular_1D(int hypId, int studyId, SMESH_Gen* gen)
  : SMESH_1D_Algo(hypId, studyId, gen)
{
  MESSAGE("SMESH_Regular_1D::SMESH_Regular_1D");
  _name = "Regular_1D";
  //  _shapeType = TopAbs_EDGE;
  _shapeType = (1<<TopAbs_EDGE);
  _compatibleHypothesis.push_back("LocalLength");
  _compatibleHypothesis.push_back("NumberOfSegments");

  _localLength = 0;
  _numberOfSegments = 0;
  _hypLocalLength = NULL;
  _hypNumberOfSegments = NULL;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

SMESH_Regular_1D::~SMESH_Regular_1D()
{
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

ostream & SMESH_Regular_1D::SaveTo(ostream & save)
{
  return save << this;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

istream & SMESH_Regular_1D::LoadFrom(istream & load)
{
  return load >> (*this);
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

ostream& operator << (ostream & save, SMESH_Regular_1D & hyp)
{
  return save;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

istream& operator >> (istream & load, SMESH_Regular_1D & hyp)
{
  return load;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

bool SMESH_Regular_1D::CheckHypothesis(SMESH_Mesh& aMesh,
				       const TopoDS_Shape& aShape)
{
  //MESSAGE("SMESH_Regular_1D::CheckHypothesis");

  list<SMESHDS_Hypothesis*>::const_iterator itl;
  SMESHDS_Hypothesis* theHyp;

  const list<SMESHDS_Hypothesis*>& hyps = GetUsedHypothesis(aMesh, aShape);
  int nbHyp = hyps.size();
  if (nbHyp != 1) return false;  // only one compatible hypothesis allowed

  itl = hyps.begin();
  theHyp = (*itl);

  string hypName = theHyp->GetName();
  int hypId = theHyp->GetID();
  //SCRUTE(hypName);

  bool isOk = false;

  if (hypName == "LocalLength")
    {
      _hypLocalLength = dynamic_cast<SMESH_LocalLength*> (theHyp);
      ASSERT(_hypLocalLength);
      _localLength = _hypLocalLength->GetLength();
      _numberOfSegments = 0;
      isOk =true;
    }

  if (hypName == "NumberOfSegments")
    {
      _hypNumberOfSegments = dynamic_cast<SMESH_NumberOfSegments*> (theHyp);
      ASSERT(_hypNumberOfSegments);
      _numberOfSegments = _hypNumberOfSegments->GetNumberOfSegments();
      _scaleFactor = _hypNumberOfSegments->GetScaleFactor();
      _localLength = 0;
      isOk = true;
    }

  //SCRUTE(_localLength);
  //SCRUTE(_numberOfSegments);

  return isOk;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================

bool SMESH_Regular_1D::Compute(SMESH_Mesh& aMesh,
			       const TopoDS_Shape& aShape)
{
  //MESSAGE("SMESH_Regular_1D::Compute");

  const Handle(SMESHDS_Mesh)& meshDS = aMesh.GetMeshDS();
  SMESH_subMesh* theSubMesh = aMesh.GetSubMesh(aShape);

  const TopoDS_Edge& EE = TopoDS::Edge(aShape);
  TopoDS_Edge E = TopoDS::Edge(EE.Oriented(TopAbs_FORWARD));

  double f,l;
  Handle(Geom_Curve) Curve = BRep_Tool::Curve(E,f,l); 

  TopoDS_Vertex VFirst, VLast;
  TopExp::Vertices(E, VFirst, VLast); // Vfirst corresponds to f and Vlast to l

  double length = EdgeLength(E);
  //SCRUTE(length);

  double eltSize = 1;
//   if (_localLength > 0) eltSize = _localLength;
  if (_localLength > 0)
    {
      double nbseg = ceil(length/_localLength); // integer sup
      if (nbseg <=0) nbseg = 1;                 // degenerated edge
      eltSize = length/nbseg;
    }
  else
    {
      ASSERT(_numberOfSegments> 0);
      eltSize = length/_numberOfSegments;
    }
  
  ASSERT(!VFirst.IsNull());
  SMESH_subMesh* firstSubMesh = aMesh.GetSubMesh(VFirst);
  const TColStd_ListOfInteger& lidf
    = firstSubMesh->GetSubMeshDS()->GetIDNodes();
  int idFirst= lidf.First();
  //SCRUTE(idFirst);
  
  ASSERT(!VLast.IsNull());
  SMESH_subMesh* lastSubMesh = aMesh.GetSubMesh(VLast);
  const TColStd_ListOfInteger& lidl
    = lastSubMesh->GetSubMeshDS()->GetIDNodes();
  int idLast= lidl.First();
  //SCRUTE(idLast);

  if (!Curve.IsNull())
    {
      GeomAdaptor_Curve C3d(Curve);
      GCPnts_UniformAbscissa Discret(C3d,eltSize,f,l);
      int NbPoints = Discret.NbPoints();
      //MESSAGE("nb points on edge : "<<NbPoints);

      // edge extrema (indexes : 1 & NbPoints) already in SMDS (TopoDS_Vertex)
      // only internal nodes receive an edge position with param on curve

      int idPrev = idFirst;
      for (int i=2; i<NbPoints; i++)
	{
	  double param = Discret.Parameter(i);

	  if(_numberOfSegments > 1)
	    {
	      double epsilon = 0.001;
	      if( fabs(_scaleFactor-1.0) > epsilon )
		{
		  double alpha = pow(_scaleFactor, 1.0/(_numberOfSegments-1) );
		  double d = length*(1-pow(alpha,i-1))/(1-pow(alpha,_numberOfSegments));
		  param = d;
		}
	    }

	  gp_Pnt P = Curve->Value(param);
      
	  //Add the Node in the DataStructure
	  int nodeId = meshDS->AddNode(P.X(), P.Y(), P.Z());
 	  //MESSAGE("point "<<nodeId<<" "<<P.X()<<" "<<P.Y()<<" "<<P.Z()<<" - "<<i<<" "<<param);
 	  Handle (SMDS_MeshElement) elt = meshDS->FindNode(nodeId);
 	  Handle (SMDS_MeshNode) node = meshDS->GetNode(1, elt);
 	  meshDS->SetNodeOnEdge(node, E);

	  // **** edgePosition associe au point = param. 
// 	  Handle (SMDS_EdgePosition) epos
// 	    = new SMDS_EdgePosition(theSubMesh->GetId(),param); // non, deja cree
// 	  node->SetPosition(epos);
	  Handle (SMDS_EdgePosition) epos
	    = Handle (SMDS_EdgePosition)::DownCast(node->GetPosition());
	  epos->SetUParameter(param);

	  int edgeId = meshDS->AddEdge(idPrev, nodeId);
	  elt = meshDS->FindElement(edgeId);
	  meshDS->SetMeshElementOnShape(elt, E);
	  idPrev = nodeId;
	}
      int edgeId = meshDS->AddEdge(idPrev, idLast);
      Handle (SMDS_MeshElement) elt = meshDS->FindElement(edgeId);
      meshDS->SetMeshElementOnShape(elt, E);
    }
  else
    {
//       MESSAGE ("Edge Degeneree non traitee --- arret");
//       ASSERT(0);
      if (BRep_Tool::Degenerated(E))
	{
	  // Edge is a degenerated Edge : We put n = 5 points on the edge.
	  int NbPoints = 5;
	  BRep_Tool::Range(E,f,l);
      	  double du = (l-f)/(NbPoints-1);
	  MESSAGE("************* Degenerated edge! *****************");
      
	  TopoDS_Vertex V1,V2;
	  TopExp::Vertices (E,V1,V2);
	  gp_Pnt P = BRep_Tool::Pnt(V1);
      
	  int idPrev = idFirst;
	  for (int i=2; i<NbPoints; i++)
	    {
	      double param = f + (i-1)*du;
	      //Add the Node in the DataStructure
	      int nodeId = meshDS->AddNode(P.X(), P.Y(), P.Z());
	      //MESSAGE("point "<<nodeId<<" "<<P.X()<<" "<<P.Y()<<" "<<P.Z()<<" - "<<i<<" "<<param);

	      Handle (SMDS_MeshElement) elt = meshDS->FindNode(nodeId);
	      Handle (SMDS_MeshNode) node = meshDS->GetNode(1, elt);
	      meshDS->SetNodeOnEdge(node, E);
	      
// 	      Handle (SMDS_EdgePosition) epos
// 		= new SMDS_EdgePosition(theSubMesh->GetId(),param);
// 	      node->SetPosition(epos);
	      Handle (SMDS_EdgePosition) epos
		= Handle (SMDS_EdgePosition)::DownCast(node->GetPosition());
	      epos->SetUParameter(param);
	      
	      int edgeId = meshDS->AddEdge(idPrev, nodeId);
	      elt = meshDS->FindElement(edgeId);
	      meshDS->SetMeshElementOnShape(elt, E);
	      idPrev = nodeId;
	    }
	  int edgeId = meshDS->AddEdge(idPrev, idLast);
	  Handle (SMDS_MeshElement) elt = meshDS->FindElement(edgeId);
	  meshDS->SetMeshElementOnShape(elt, E);
	}
      else ASSERT(0);
    }
  return true;
}
