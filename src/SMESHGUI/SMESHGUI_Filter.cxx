//  SMESHGUI_PredicateFilter : Filters for VTK viewer
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
//  File   : SMESHGUI_Filter.cxx
//  Author : Sergey LITONIN
//  Module : SMESH

#include "SMESHGUI_Filter.h"
#include "SMESHGUI_Utils.h"
#include "SMDS_Mesh.hxx"
#include "SMDS_MeshElement.hxx"
#include "SMDSAbs_ElementType.hxx"

#include <vtkCell.h>

#include <gp_Vec.hxx>
#include <Precision.hxx>
#include "SMESH_Actor.h"
#include "SMESHGUI.h"

IMPLEMENT_STANDARD_HANDLE(SMESHGUI_Filter, VTKViewer_Filter)
IMPLEMENT_STANDARD_RTTIEXT(SMESHGUI_Filter, VTKViewer_Filter)

IMPLEMENT_STANDARD_HANDLE(SMESHGUI_PredicateFilter, SMESHGUI_Filter)
IMPLEMENT_STANDARD_RTTIEXT(SMESHGUI_PredicateFilter, SMESHGUI_Filter)

IMPLEMENT_STANDARD_HANDLE(SMESHGUI_QuadrangleFilter, SMESHGUI_Filter)
IMPLEMENT_STANDARD_RTTIEXT(SMESHGUI_QuadrangleFilter, SMESHGUI_Filter)

IMPLEMENT_STANDARD_HANDLE(SMESHGUI_TriangleFilter, SMESHGUI_Filter)
IMPLEMENT_STANDARD_RTTIEXT(SMESHGUI_TriangleFilter, SMESHGUI_Filter)

/*
  Class       : SMESHGUI_PredicateFilter
  Description : Selection filter for VTK viewer. This class aggregate object
                of SMESH_Predicate class and uses it for verification of criterion
*/

//=======================================================================
// name    : SMESHGUI_PredicateFilter::SMESHGUI_PredicateFilter
// Purpose : Constructor
//=======================================================================
SMESHGUI_PredicateFilter::SMESHGUI_PredicateFilter()
{
}

SMESHGUI_PredicateFilter::~SMESHGUI_PredicateFilter()
{
}

//=======================================================================
// name    : SMESHGUI_PredicateFilter::IsValid
// Purpose : Verify whether entry id satisfies to criterion of the filter
//=======================================================================
bool SMESHGUI_PredicateFilter::IsValid( const int theCellId ) const
{
  if ( myActor == 0 || myPred->_is_nil() )
    return false;

  SMESH_Actor* anActor = ( SMESH_Actor* )myActor;
  if ( anActor->GetObject() == 0 )
    return false;
  
  SMDS_Mesh* aMesh = anActor->GetObject()->GetMesh();
  SMESH::ElementType anElemType = myPred->GetElementType();
  int aMeshId = anElemType == SMESH::NODE ? anActor->GetNodeObjId( theCellId )
                                          : anActor->GetElemObjId( theCellId );

  // if type of element != type of predicate return true because
  // this predicate is not intended for filtering sush elements
  const SMDS_MeshElement* anElem = anElemType == SMESH::NODE ? aMesh->FindNode( aMeshId )
                                                             : aMesh->FindElement( aMeshId );
  if ( anElem != 0 && anElem->GetType() != (SMDSAbs_ElementType)myPred->GetElementType() )
    return true;
  
  return myPred->IsSatisfy( aMeshId );
}

//=======================================================================
// name    : SMESHGUI_PredicateFilter::IsValid
// Purpose : Verify whether entry id satisfies to criterion of the filter
//=======================================================================
bool SMESHGUI_PredicateFilter::IsObjValid( const int theObjId ) const
{
  if ( myActor == 0 || myPred->_is_nil() )
    return false;

  SMESH_Actor* anActor = ( SMESH_Actor* )myActor;
  if ( anActor->GetObject() == 0 )
    return false;
  
  SMDS_Mesh* aMesh = anActor->GetObject()->GetMesh();
  SMESH::ElementType anElemType = myPred->GetElementType();
  
  // if type of element != type of predicate return true because
  // this predicate is not intended for filtering sush elements
  const SMDS_MeshElement* anElem = anElemType == SMESH::NODE ? aMesh->FindNode( theObjId )
                                                             : aMesh->FindElement( theObjId );
  if ( anElem != 0 && anElem->GetType() != (SMDSAbs_ElementType)myPred->GetElementType() )
    return true;

  return myPred->IsSatisfy( theObjId );
}

//=======================================================================
// name    : SMESHGUI_PredicateFilter::IsNodeFilter
// Purpose : Returns true if filter is intended for nodes
//=======================================================================
bool SMESHGUI_PredicateFilter::IsNodeFilter() const
{
  return GetId() == SMESHGUI_NodeFilter;
}

//=======================================================================
// name    : SMESHGUI_PredicateFilter::SetPredicate
// Purpose : Set new pridicate to the filter
//=======================================================================
void SMESHGUI_PredicateFilter::SetPredicate( SMESH::Predicate_ptr thePred )
{
  myPred = SMESH::Predicate::_duplicate( thePred );
}

//=======================================================================
// name    : SMESHGUI_PredicateFilter::SetActor
// Purpose : Set new actor
//=======================================================================
void SMESHGUI_PredicateFilter::SetActor( SALOME_Actor* theActor )
{
  if ( myActor == theActor )
    return;
  SMESHGUI_Filter::SetActor( theActor );

  if ( myActor != 0 && !myPred->_is_nil() )
  {
    Handle(SALOME_InteractiveObject) anIO = myActor->getIO();
    if ( !anIO.IsNull() )
    {
      SMESH::SMESH_Mesh_var aMesh = SMESH::IObjectToInterface<SMESH::SMESH_Mesh>(anIO);
      if(!aMesh->_is_nil())
        myPred->SetMesh(aMesh);
    }
  }
}

//=======================================================================
// name    : SMESHGUI_PredicateFilter::SetActor
// Purpose : Get ID of the filter. Must return value from SMESHGUI_FilterType
//           enumeration. All filters must have different ids
//=======================================================================
int SMESHGUI_PredicateFilter::GetId() const
{
  if      ( myPred->GetElementType() == SMESH::NODE   ) return SMESHGUI_NodeFilter;
  else if ( myPred->GetElementType() == SMESH::EDGE   ) return SMESHGUI_EdgeFilter;
  else if ( myPred->GetElementType() == SMESH::FACE   ) return SMESHGUI_FaceFilter;
  else if ( myPred->GetElementType() == SMESH::VOLUME ) return SMESHGUI_VolumeFilter;
  else if ( myPred->GetElementType() == SMESH::ALL    ) return SMESHGUI_AllElementsFilter;
  else                                                  return SMESHGUI_UnknownFilter;
}


/*
  Class       : SMESHGUI_QuadrangleFilter
  Description : Verify whether selected cell is quadranle
*/

//=======================================================================
// name    : SMESHGUI_QuadrangleFilter::SMESHGUI_QuadrangleFilter
// Purpose : Constructor
//=======================================================================
SMESHGUI_QuadrangleFilter::SMESHGUI_QuadrangleFilter()
: SMESHGUI_Filter()
{
}

SMESHGUI_QuadrangleFilter::~SMESHGUI_QuadrangleFilter()
{
}

//=======================================================================
// name    : SMESHGUI_QuadrangleFilter::IsValid
// Purpose : Verify whether selected cell is quadranle
//=======================================================================
bool SMESHGUI_QuadrangleFilter::IsValid( const int theCellId ) const
{
  if ( myActor == 0 )
    return false;

  SMESH_Actor* anActor = ( SMESH_Actor* )myActor;
  if ( anActor->GetObject() == 0 )
    return false;
  
  SMDS_Mesh* aMesh = anActor->GetObject()->GetMesh();
  const SMDS_MeshElement* anElem = aMesh->FindElement( anActor->GetElemObjId( theCellId ) );
  
  return anElem != 0 ? anElem->GetType() != SMDSAbs_Face || anElem->NbNodes() == 4 : false;
}

//=======================================================================
// name    : SMESHGUI_QuadrangleFilter::IsValid
// Purpose : Verify whether selected cell is quadranle
//=======================================================================
bool SMESHGUI_QuadrangleFilter::IsObjValid( const int theObjId ) const
{
  if ( myActor == 0 )
    return false;

  SMESH_Actor* anActor = ( SMESH_Actor* )myActor;
  if ( anActor->GetObject() == 0 )
    return false;
  
  SMDS_Mesh* aMesh = anActor->GetObject()->GetMesh();
  const SMDS_MeshElement* anElem = aMesh->FindElement( theObjId );

  return anElem != 0 ? anElem->GetType() != SMDSAbs_Face || anElem->NbNodes() == 4 : false;
}

//=======================================================================
// name    : SMESHGUI_QuadrangleFilter::SetActor
// Purpose : Get ID of the filter. Must return value from SMESHGUI_FilterType
//           enumeration. All filters must have different ids
//=======================================================================
int SMESHGUI_QuadrangleFilter::GetId() const
{
  return SMESHGUI_QuadFilter;
}

//=======================================================================
// name    : SMESHGUI_QuadrangleFilter::IsNodeFilter
// Purpose : Returns true if filter is intended for nodes
//=======================================================================
bool SMESHGUI_QuadrangleFilter::IsNodeFilter() const
{
  return false;
}


/*
  Class       : SMESHGUI_TriangleFilter
  Description : Verify whether selected cell is triangle
*/


//=======================================================================
// name    : SMESHGUI_TriangleFilter::SMESHGUI_TriangleFilter
// Purpose : Constructor
//=======================================================================
SMESHGUI_TriangleFilter::SMESHGUI_TriangleFilter()
: SMESHGUI_Filter()
{
}

SMESHGUI_TriangleFilter::~SMESHGUI_TriangleFilter()
{
}

//=======================================================================
// name    : SMESHGUI_TriangleFilter::IsValid
// Purpose : Verify whether selected cell is triangle
//=======================================================================
bool SMESHGUI_TriangleFilter::IsValid( const int theCellId ) const
{
  if ( myActor == 0 )
    return false;

  SMESH_Actor* anActor = ( SMESH_Actor* )myActor;
  if ( anActor->GetObject() == 0 )
    return false;
  
  SMDS_Mesh* aMesh = anActor->GetObject()->GetMesh();
  const SMDS_MeshElement* anElem = aMesh->FindElement( anActor->GetElemObjId( theCellId ) );

  return anElem != 0 ? anElem->GetType() != SMDSAbs_Face || anElem->NbNodes() == 3 : false;
}

//=======================================================================
// name    : SMESHGUI_TriangleFilter::IsValid
// Purpose : Verify whether selected cell is triangle
//=======================================================================
bool SMESHGUI_TriangleFilter::IsObjValid( const int theObjId ) const
{
  if ( myActor == 0 )
    return false;

  SMESH_Actor* anActor = ( SMESH_Actor* )myActor;
  if ( anActor->GetObject() == 0 )
    return false;
  
  SMDS_Mesh* aMesh = anActor->GetObject()->GetMesh();
  const SMDS_MeshElement* anElem = aMesh->FindElement( theObjId );

  return anElem != 0 ? anElem->GetType() != SMDSAbs_Face || anElem->NbNodes() == 3 : false;
}

//=======================================================================
// name    : SMESHGUI_TriangleFilter::SetActor
// Purpose : Get ID of the filter. Must return value from SMESHGUI_FilterType
//           enumeration. All filters must have different ids
//=======================================================================
int SMESHGUI_TriangleFilter::GetId() const
{
  return SMESHGUI_TriaFilter;
}

//=======================================================================
// name    : SMESHGUI_TriangleFilter::IsNodeFilter
// Purpose : Returns true if filter is intended for nodes
//=======================================================================
bool SMESHGUI_TriangleFilter::IsNodeFilter() const
{
  return false;
}










