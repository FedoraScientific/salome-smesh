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
//  File   : SMESH_Filter_i.cxx
//  Author : Alexey Petrov, OCC
//  Module : SMESH


#include "SMESH_Filter_i.hxx"

#include "SMESH_Gen_i.hxx"

#include "SMDS_Mesh.hxx"
#include "SMDS_MeshNode.hxx"
#include "SMDS_MeshElement.hxx"

#include "SMESHDS_Mesh.hxx"

#include <LDOM_Document.hxx>
#include <LDOM_Element.hxx>
#include <LDOM_Node.hxx>
#include <LDOMString.hxx>
#include <LDOMParser.hxx>
#include <LDOM_XmlWriter.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListOfReal.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_SequenceOfHAsciiString.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>
#include <Precision.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <TopExp_Explorer.hxx>
#include <OSD_Path.hxx>
#include <OSD_File.hxx>

using namespace SMESH;
using namespace SMESH::Controls;

/*
  Class       : BelongToGeom
  Description : Predicate for verifying whether entiy belong to
                specified geometrical support
*/

Controls::BelongToGeom::BelongToGeom()
: myMeshDS(NULL),
  myType(SMDSAbs_All)
{}

void Controls::BelongToGeom::SetMesh( SMDS_Mesh* theMesh )
{
  myMeshDS = dynamic_cast<SMESHDS_Mesh*>(theMesh);
}

void Controls::BelongToGeom::SetGeom( const TopoDS_Shape& theShape )
{
  myShape = theShape;
}

static bool IsContains( SMESHDS_Mesh*           theMeshDS,
                        const TopoDS_Shape&     theShape,
                        const SMDS_MeshElement* theElem,
                        TopAbs_ShapeEnum        theFindShapeEnum,
                        TopAbs_ShapeEnum        theAvoidShapeEnum = TopAbs_SHAPE )
{
  TopExp_Explorer anExp( theShape,theFindShapeEnum,theAvoidShapeEnum );
  
  while( anExp.More() )
  {
    const TopoDS_Shape& aShape = anExp.Current();
    if( SMESHDS_SubMesh* aSubMesh = theMeshDS->MeshElements( aShape ) ){
      if( aSubMesh->Contains( theElem ) )
        return true;
    }
    anExp.Next();
  }
  return false;
}

bool Controls::BelongToGeom::IsSatisfy( long theId )
{
  if ( myMeshDS == 0 || myShape.IsNull() )
    return false;

  if( myType == SMDSAbs_Node )
  {
    if( const SMDS_MeshNode* aNode = myMeshDS->FindNode( theId ) )
    {
      const SMDS_PositionPtr& aPosition = aNode->GetPosition();
      SMDS_TypeOfPosition aTypeOfPosition = aPosition->GetTypeOfPosition();
      switch( aTypeOfPosition )
      {
      case SMDS_TOP_VERTEX : return IsContains( myMeshDS,myShape,aNode,TopAbs_VERTEX );
      case SMDS_TOP_EDGE   : return IsContains( myMeshDS,myShape,aNode,TopAbs_EDGE );
      case SMDS_TOP_FACE   : return IsContains( myMeshDS,myShape,aNode,TopAbs_FACE );
      case SMDS_TOP_3DSPACE: return IsContains( myMeshDS,myShape,aNode,TopAbs_SHELL );
      }
    }
  }
  else
  {
    if( const SMDS_MeshElement* anElem = myMeshDS->FindElement( theId ) )
    {
      if( myType == SMDSAbs_All )
      {
        return IsContains( myMeshDS,myShape,anElem,TopAbs_EDGE ) ||
               IsContains( myMeshDS,myShape,anElem,TopAbs_FACE ) ||
               IsContains( myMeshDS,myShape,anElem,TopAbs_SHELL )||
               IsContains( myMeshDS,myShape,anElem,TopAbs_SOLID );
      }
      else if( myType == anElem->GetType() )
      {
        switch( myType )
        {
        case SMDSAbs_Edge  : return IsContains( myMeshDS,myShape,anElem,TopAbs_EDGE );
        case SMDSAbs_Face  : return IsContains( myMeshDS,myShape,anElem,TopAbs_FACE );
        case SMDSAbs_Volume: return IsContains( myMeshDS,myShape,anElem,TopAbs_SHELL )||
                                    IsContains( myMeshDS,myShape,anElem,TopAbs_SOLID );
        }
      }
    }
  }
    
  return false;
}

void Controls::BelongToGeom::SetType( SMDSAbs_ElementType theType )
{
  myType = theType;
}

SMDSAbs_ElementType Controls::BelongToGeom::GetType() const
{
  return myType;
}

TopoDS_Shape Controls::BelongToGeom::GetShape()
{
  return myShape;
}

SMESHDS_Mesh* Controls::BelongToGeom::GetMeshDS()
{
  return myMeshDS;
}

/*
  Class       : LyingOnGeom
  Description : Predicate for verifying whether entiy lying or partially lying on
                specified geometrical support
*/

Controls::LyingOnGeom::LyingOnGeom()
: myMeshDS(NULL),
  myType(SMDSAbs_All)
{}

void Controls::LyingOnGeom::SetMesh( SMDS_Mesh* theMesh )
{
 myMeshDS = dynamic_cast<SMESHDS_Mesh*>(theMesh);
}

void Controls::LyingOnGeom::SetGeom( const TopoDS_Shape& theShape )
{
  myShape = theShape;
}

bool Controls::LyingOnGeom::IsSatisfy( long theId )
{
  if ( myMeshDS == 0 || myShape.IsNull() )
    return false;

  if( myType == SMDSAbs_Node )
  {
    if( const SMDS_MeshNode* aNode = myMeshDS->FindNode( theId ) )
    {
      const SMDS_PositionPtr& aPosition = aNode->GetPosition();
      SMDS_TypeOfPosition aTypeOfPosition = aPosition->GetTypeOfPosition();
      switch( aTypeOfPosition )
      {
      case SMDS_TOP_VERTEX : return IsContains( myMeshDS,myShape,aNode,TopAbs_VERTEX );
      case SMDS_TOP_EDGE   : return IsContains( myMeshDS,myShape,aNode,TopAbs_EDGE );
      case SMDS_TOP_FACE   : return IsContains( myMeshDS,myShape,aNode,TopAbs_FACE );
      case SMDS_TOP_3DSPACE: return IsContains( myMeshDS,myShape,aNode,TopAbs_SHELL );
      }
    }
  }
  else
  {
    if( const SMDS_MeshElement* anElem = myMeshDS->FindElement( theId ) )
    {
      if( myType == SMDSAbs_All )
      {
        return Contains( myMeshDS,myShape,anElem,TopAbs_EDGE ) ||
               Contains( myMeshDS,myShape,anElem,TopAbs_FACE ) ||
               Contains( myMeshDS,myShape,anElem,TopAbs_SHELL )||
               Contains( myMeshDS,myShape,anElem,TopAbs_SOLID );
      }
      else if( myType == anElem->GetType() )
      {
        switch( myType )
        {
        case SMDSAbs_Edge  : return Contains( myMeshDS,myShape,anElem,TopAbs_EDGE );
        case SMDSAbs_Face  : return Contains( myMeshDS,myShape,anElem,TopAbs_FACE );
        case SMDSAbs_Volume: return Contains( myMeshDS,myShape,anElem,TopAbs_SHELL )||
                                    Contains( myMeshDS,myShape,anElem,TopAbs_SOLID );
        }
      }
    }
  }
    
  return false;
}

void Controls::LyingOnGeom::SetType( SMDSAbs_ElementType theType )
{
  myType = theType;
}

SMDSAbs_ElementType Controls::LyingOnGeom::GetType() const
{
  return myType;
}

TopoDS_Shape Controls::LyingOnGeom::GetShape()
{
  return myShape;
}

SMESHDS_Mesh* Controls::LyingOnGeom::GetMeshDS()
{
  return myMeshDS;
}

bool Controls::LyingOnGeom::Contains( SMESHDS_Mesh*           theMeshDS,
				      const TopoDS_Shape&     theShape,
				      const SMDS_MeshElement* theElem,
				      TopAbs_ShapeEnum        theFindShapeEnum,
				      TopAbs_ShapeEnum        theAvoidShapeEnum )
{
  if (IsContains(theMeshDS, theShape, theElem, theFindShapeEnum, theAvoidShapeEnum))
    return true;
  
  TopTools_IndexedMapOfShape aSubShapes;
  TopExp::MapShapes( theShape, aSubShapes );
  
  for (int i = 1; i <= aSubShapes.Extent(); i++)
    {
      const TopoDS_Shape& aShape = aSubShapes.FindKey(i);
      
      if( SMESHDS_SubMesh* aSubMesh = theMeshDS->MeshElements( aShape ) ){
	if( aSubMesh->Contains( theElem ) )
	  return true;
	
	SMDS_NodeIteratorPtr aNodeIt = aSubMesh->GetNodes();
	while ( aNodeIt->more() )
	  {
	    const SMDS_MeshNode* aNode = static_cast<const SMDS_MeshNode*>(aNodeIt->next());
	    SMDS_ElemIteratorPtr anElemIt = aNode->GetInverseElementIterator();
	    while ( anElemIt->more() )
	      {
		const SMDS_MeshElement* anElement = static_cast<const SMDS_MeshElement*>(anElemIt->next());
		if (anElement == theElem)
		  return true;
	      }
	  }
      }
    }
  return false;
}


/*
                            AUXILIARY METHODS
*/

static inline SMDS_Mesh* MeshPtr2SMDSMesh( SMESH_Mesh_ptr theMesh )
{
  SMESH_Mesh_i* anImplPtr = 
    dynamic_cast<SMESH_Mesh_i*>( SMESH_Gen_i::GetServant( theMesh ).in() );
  return anImplPtr ? anImplPtr->GetImpl().GetMeshDS() : 0;
}

static inline SMESH::long_array* toArray( const TColStd_ListOfInteger& aList )
{
  SMESH::long_array_var anArray = new SMESH::long_array;
  anArray->length( aList.Extent() );
  TColStd_ListIteratorOfListOfInteger anIter( aList );
  int i = 0;
  for( ; anIter.More(); anIter.Next() )
    anArray[ i++ ] = anIter.Value();

  return anArray._retn();
}

static inline SMESH::double_array* toArray( const TColStd_ListOfReal& aList )
{
  SMESH::double_array_var anArray = new SMESH::double_array;
  anArray->length( aList.Extent() );
  TColStd_ListIteratorOfListOfReal anIter( aList );
  int i = 0;
  for( ; anIter.More(); anIter.Next() )
    anArray[ i++ ] = anIter.Value();

  return anArray._retn();
}

static SMESH::Filter::Criterion createCriterion()
{
  SMESH::Filter::Criterion aCriterion;

  aCriterion.Type          = FT_Undefined;
  aCriterion.Compare       = FT_Undefined;
  aCriterion.Threshold     = 0;
  aCriterion.UnaryOp       = FT_Undefined;
  aCriterion.BinaryOp      = FT_Undefined;
  aCriterion.ThresholdStr  = "";
  aCriterion.Tolerance     = Precision::Confusion();
  aCriterion.TypeOfElement = SMESH::ALL;
  aCriterion.Precision     = -1;

  return aCriterion;
}

static TopoDS_Shape getShapeByName( const char* theName )
{
  if ( theName != 0 )
  {
    SMESH_Gen_i* aSMESHGen = SMESH_Gen_i::GetSMESHGen();
    SALOMEDS::Study_ptr aStudy = aSMESHGen->GetCurrentStudy();
    if ( aStudy != 0 )
    {
      SALOMEDS::Study::ListOfSObject_var aList =
        aStudy->FindObjectByName( theName, "GEOM" );
      if ( aList->length() > 0 )
      {
        GEOM::GEOM_Object_var aGeomObj = GEOM::GEOM_Object::_narrow( aList[ 0 ]->GetObject() );
        if ( !aGeomObj->_is_nil() )
        {
	  GEOM::GEOM_Gen_var aGEOMGen = SMESH_Gen_i::GetGeomEngine();
          TopoDS_Shape aLocShape = aSMESHGen->GetShapeReader()->GetShape( aGEOMGen, aGeomObj );
          return aLocShape;
        }
      }
    }
  }
  return TopoDS_Shape();
}



/*
                                FUNCTORS
*/

/*
  Class       : Functor_i
  Description : An abstact class for all functors 
*/
Functor_i::Functor_i(): 
  SALOME::GenericObj_i( SMESH_Gen_i::GetPOA() )
{
  SMESH_Gen_i::GetPOA()->activate_object( this );
}

void Functor_i::SetMesh( SMESH_Mesh_ptr theMesh )
{
  myFunctorPtr->SetMesh( MeshPtr2SMDSMesh( theMesh ) );
  INFOS("Functor_i::SetMesh~");
}

ElementType Functor_i::GetElementType()
{
  return ( ElementType )myFunctorPtr->GetType();
}


/*
  Class       : NumericalFunctor_i
  Description : Base class for numerical functors
*/
CORBA::Double NumericalFunctor_i::GetValue( CORBA::Long theId )
{
  return myNumericalFunctorPtr->GetValue( theId );
}

void NumericalFunctor_i::SetPrecision( CORBA::Long thePrecision )
{
  myNumericalFunctorPtr->SetPrecision( thePrecision );
}

CORBA::Long NumericalFunctor_i::GetPrecision()
{
 return myNumericalFunctorPtr->GetPrecision();
}

Controls::NumericalFunctorPtr NumericalFunctor_i::GetNumericalFunctor()
{
  return myNumericalFunctorPtr;
}


/*
  Class       : SMESH_MinimumAngle
  Description : Functor for calculation of minimum angle
*/
MinimumAngle_i::MinimumAngle_i()
{
  myNumericalFunctorPtr.reset( new Controls::MinimumAngle() );
  myFunctorPtr = myNumericalFunctorPtr;
}

FunctorType MinimumAngle_i::GetFunctorType()
{
  return SMESH::FT_MinimumAngle;
}


/*
  Class       : AspectRatio
  Description : Functor for calculating aspect ratio
*/
AspectRatio_i::AspectRatio_i()
{
  myNumericalFunctorPtr.reset( new Controls::AspectRatio() );
  myFunctorPtr = myNumericalFunctorPtr;
}

FunctorType AspectRatio_i::GetFunctorType()
{
  return SMESH::FT_AspectRatio;
}


/*
  Class       : AspectRatio3D
  Description : Functor for calculating aspect ratio 3D
*/
AspectRatio3D_i::AspectRatio3D_i()
{
  myNumericalFunctorPtr.reset( new Controls::AspectRatio3D() );
  myFunctorPtr = myNumericalFunctorPtr;
}

FunctorType AspectRatio3D_i::GetFunctorType()
{
  return SMESH::FT_AspectRatio3D;
}


/*
  Class       : Warping_i
  Description : Functor for calculating warping
*/
Warping_i::Warping_i()
{
  myNumericalFunctorPtr.reset( new Controls::Warping() );
  myFunctorPtr = myNumericalFunctorPtr;
}

FunctorType Warping_i::GetFunctorType()
{
  return SMESH::FT_Warping;
}


/*
  Class       : Taper_i
  Description : Functor for calculating taper
*/
Taper_i::Taper_i()
{
  myNumericalFunctorPtr.reset( new Controls::Taper() );
  myFunctorPtr = myNumericalFunctorPtr;
}

FunctorType Taper_i::GetFunctorType()
{
  return SMESH::FT_Taper;
}


/*
  Class       : Skew_i
  Description : Functor for calculating skew in degrees
*/
Skew_i::Skew_i()
{
  myNumericalFunctorPtr.reset( new Controls::Skew() );
  myFunctorPtr = myNumericalFunctorPtr;
}

FunctorType Skew_i::GetFunctorType()
{
  return SMESH::FT_Skew;
}

/*
  Class       : Area_i
  Description : Functor for calculating area
*/
Area_i::Area_i()
{
  myNumericalFunctorPtr.reset( new Controls::Area() );
  myFunctorPtr = myNumericalFunctorPtr;
}

FunctorType Area_i::GetFunctorType()
{
  return SMESH::FT_Area;
}

/*
  Class       : Length_i
  Description : Functor for calculating length off edge
*/
Length_i::Length_i()
{
  myNumericalFunctorPtr.reset( new Controls::Length() );
  myFunctorPtr = myNumericalFunctorPtr;
}

FunctorType Length_i::GetFunctorType()
{
  return SMESH::FT_Length;
}

/*
  Class       : Length2D_i
  Description : Functor for calculating length of edge
*/
Length2D_i::Length2D_i()
{
  myNumericalFunctorPtr.reset( new Controls::Length2D() );
  myFunctorPtr = myNumericalFunctorPtr;
}

FunctorType Length2D_i::GetFunctorType()
{
  return SMESH::FT_Length2D;
}

SMESH::Length2D::Values* Length2D_i::GetValues()
{
  INFOS("Length2D_i::GetValues");
  SMESH::Controls::Length2D::TValues aValues;
  myLength2DPtr->GetValues( aValues );
  
  long i = 0, iEnd = aValues.size();

  SMESH::Length2D::Values_var aResult = new SMESH::Length2D::Values(iEnd);

  SMESH::Controls::Length2D::TValues::const_iterator anIter;
  for ( anIter = aValues.begin() ; anIter != aValues.end(); anIter++, i++ )
  {
    const SMESH::Controls::Length2D::Value&  aVal = *anIter;
    SMESH::Length2D::Value &aValue = aResult[ i ];
    
    aValue.myLength = aVal.myLength;
    aValue.myPnt1 = aVal.myPntId[ 0 ];
    aValue.myPnt2 = aVal.myPntId[ 1 ];
   
  }

  INFOS("Length2D_i::GetValuess~");
  return aResult._retn();
}

/*
  Class       : MultiConnection_i
  Description : Functor for calculating number of faces conneted to the edge
*/
MultiConnection_i::MultiConnection_i()
{
  myNumericalFunctorPtr.reset( new Controls::MultiConnection() );
  myFunctorPtr = myNumericalFunctorPtr;
}

FunctorType MultiConnection_i::GetFunctorType()
{
  return SMESH::FT_MultiConnection;
}

/*
  Class       : MultiConnection2D_i
  Description : Functor for calculating number of faces conneted to the edge
*/
MultiConnection2D_i::MultiConnection2D_i()
{
  myNumericalFunctorPtr.reset( new Controls::MultiConnection2D() );
  myFunctorPtr = myNumericalFunctorPtr;
}

FunctorType MultiConnection2D_i::GetFunctorType()
{
  return SMESH::FT_MultiConnection2D;
}

SMESH::MultiConnection2D::Values* MultiConnection2D_i::GetValues()
{
  INFOS("MultiConnection2D_i::GetValues");
  SMESH::Controls::MultiConnection2D::MValues aValues;
  myMulticonnection2DPtr->GetValues( aValues );
  
  long i = 0, iEnd = aValues.size();

  SMESH::MultiConnection2D::Values_var aResult = new SMESH::MultiConnection2D::Values(iEnd);

  SMESH::Controls::MultiConnection2D::MValues::const_iterator anIter;
  for ( anIter = aValues.begin() ; anIter != aValues.end(); anIter++, i++ )
  {
    const SMESH::Controls::MultiConnection2D::Value&  aVal = (*anIter).first;
    SMESH::MultiConnection2D::Value &aValue = aResult[ i ];
    
    aValue.myPnt1 = aVal.myPntId[ 0 ];
    aValue.myPnt2 = aVal.myPntId[ 1 ];
    aValue.myNbConnects = (*anIter).second;
   
  }

  INFOS("Multiconnection2D_i::GetValuess~");
  return aResult._retn();
}

/*
                            PREDICATES
*/


/*
  Class       : Predicate_i
  Description : Base class for all predicates
*/
CORBA::Boolean Predicate_i::IsSatisfy( CORBA::Long theId )
{
  return myPredicatePtr->IsSatisfy( theId );
}

Controls::PredicatePtr Predicate_i::GetPredicate()
{
  return myPredicatePtr;
}

/*
  Class       : BadOrientedVolume_i
  Description : Verify whether a mesh volume is incorrectly oriented from
                the point of view of MED convention
*/
BadOrientedVolume_i::BadOrientedVolume_i()
{
  Controls::PredicatePtr control( new Controls::BadOrientedVolume() );
  myFunctorPtr = myPredicatePtr = control;
};

FunctorType BadOrientedVolume_i::GetFunctorType()
{
  return SMESH::FT_BadOrientedVolume;
}

/*
  Class       : BelongToGeom_i
  Description : Predicate for selection on geometrical support
*/
BelongToGeom_i::BelongToGeom_i()
{
  myBelongToGeomPtr.reset( new Controls::BelongToGeom() );
  myFunctorPtr = myPredicatePtr = myBelongToGeomPtr;
  myShapeName = 0;
}

BelongToGeom_i::~BelongToGeom_i()
{
  delete myShapeName;
}

void BelongToGeom_i::SetGeom( GEOM::GEOM_Object_ptr theGeom )
{
  if ( theGeom->_is_nil() )
    return;
  SMESH_Gen_i* aSMESHGen = SMESH_Gen_i::GetSMESHGen();
  GEOM::GEOM_Gen_var aGEOMGen = SMESH_Gen_i::GetGeomEngine();
  TopoDS_Shape aLocShape = aSMESHGen->GetShapeReader()->GetShape( aGEOMGen, theGeom );
  myBelongToGeomPtr->SetGeom( aLocShape );
}

void BelongToGeom_i::SetGeom( const TopoDS_Shape& theShape )
{
  myBelongToGeomPtr->SetGeom( theShape );
}

void BelongToGeom_i::SetElementType(ElementType theType){
  myBelongToGeomPtr->SetType(SMDSAbs_ElementType(theType));
}

FunctorType BelongToGeom_i::GetFunctorType()
{
  return SMESH::FT_BelongToGeom;
}

void BelongToGeom_i::SetShapeName( const char* theName )
{
  delete myShapeName;
  myShapeName = strdup( theName );
  myBelongToGeomPtr->SetGeom( getShapeByName( myShapeName ) );
}

char* BelongToGeom_i::GetShapeName()
{
  return CORBA::string_dup( myShapeName );
}

/*
  Class       : BelongToSurface_i
  Description : Predicate for selection on geometrical support
*/
BelongToSurface_i::BelongToSurface_i( const Handle(Standard_Type)& theSurfaceType )
{
  myElementsOnSurfacePtr.reset( new Controls::ElementsOnSurface() );
  myFunctorPtr = myPredicatePtr = myElementsOnSurfacePtr;
  myShapeName = 0;
  mySurfaceType = theSurfaceType;
}

BelongToSurface_i::~BelongToSurface_i()
{
  delete myShapeName;
}

void BelongToSurface_i::SetSurface( GEOM::GEOM_Object_ptr theGeom, ElementType theType )
{
  if ( theGeom->_is_nil() )
    return;
  SMESH_Gen_i* aSMESHGen = SMESH_Gen_i::GetSMESHGen();
  GEOM::GEOM_Gen_var aGEOMGen = SMESH_Gen_i::GetGeomEngine();
  TopoDS_Shape aLocShape = aSMESHGen->GetShapeReader()->GetShape( aGEOMGen, theGeom );

  if ( aLocShape.ShapeType() == TopAbs_FACE )
  {
    Handle(Geom_Surface) aSurf = BRep_Tool::Surface( TopoDS::Face( aLocShape ) );
    if ( !aSurf.IsNull() && aSurf->DynamicType() == mySurfaceType )
    {
      myElementsOnSurfacePtr->SetSurface( aLocShape, (SMDSAbs_ElementType)theType );
      return;
    }
  }

  myElementsOnSurfacePtr->SetSurface( TopoDS_Shape(), (SMDSAbs_ElementType)theType );
}

void BelongToSurface_i::SetShapeName( const char* theName, ElementType theType )
{
  delete myShapeName;
  myShapeName = strdup( theName );
  myElementsOnSurfacePtr->SetSurface( getShapeByName( myShapeName ), (SMDSAbs_ElementType)theType );
}

char* BelongToSurface_i::GetShapeName()
{
  return CORBA::string_dup( myShapeName );
}

void BelongToSurface_i::SetTolerance( CORBA::Double theToler )
{
  myElementsOnSurfacePtr->SetTolerance( theToler );
}

CORBA::Double BelongToSurface_i::GetTolerance()
{
  return myElementsOnSurfacePtr->GetTolerance();
}

/*
  Class       : BelongToPlane_i
  Description : Verify whether mesh element lie in pointed Geom planar object
*/

BelongToPlane_i::BelongToPlane_i()
: BelongToSurface_i( STANDARD_TYPE( Geom_Plane ) )
{
}

void BelongToPlane_i::SetPlane( GEOM::GEOM_Object_ptr theGeom, ElementType theType )
{
  BelongToSurface_i::SetSurface( theGeom, theType );
}

FunctorType BelongToPlane_i::GetFunctorType()
{
  return FT_BelongToPlane;
}

/*
  Class       : BelongToCylinder_i
  Description : Verify whether mesh element lie in pointed Geom planar object
*/

BelongToCylinder_i::BelongToCylinder_i()
: BelongToSurface_i( STANDARD_TYPE( Geom_CylindricalSurface ) )
{
}

void BelongToCylinder_i::SetCylinder( GEOM::GEOM_Object_ptr theGeom, ElementType theType )
{
  BelongToSurface_i::SetSurface( theGeom, theType );
}

FunctorType BelongToCylinder_i::GetFunctorType()
{
  return FT_BelongToCylinder;
}

/*
  Class       : LyingOnGeom_i
  Description : Predicate for selection on geometrical support
*/
LyingOnGeom_i::LyingOnGeom_i()
{
  myLyingOnGeomPtr.reset( new Controls::LyingOnGeom() );
  myFunctorPtr = myPredicatePtr = myLyingOnGeomPtr;
  myShapeName = 0;
}

LyingOnGeom_i::~LyingOnGeom_i()
{
  delete myShapeName;
}

void LyingOnGeom_i::SetGeom( GEOM::GEOM_Object_ptr theGeom )
{
  if ( theGeom->_is_nil() )
    return;
  SMESH_Gen_i* aSMESHGen = SMESH_Gen_i::GetSMESHGen();
  GEOM::GEOM_Gen_var aGEOMGen = SMESH_Gen_i::GetGeomEngine();
  TopoDS_Shape aLocShape = aSMESHGen->GetShapeReader()->GetShape( aGEOMGen, theGeom );
  myLyingOnGeomPtr->SetGeom( aLocShape );
}

void LyingOnGeom_i::SetGeom( const TopoDS_Shape& theShape )
{
  myLyingOnGeomPtr->SetGeom( theShape );
}

void LyingOnGeom_i::SetElementType(ElementType theType){
  myLyingOnGeomPtr->SetType(SMDSAbs_ElementType(theType));
}

FunctorType LyingOnGeom_i::GetFunctorType()
{
  return SMESH::FT_LyingOnGeom;
}

void LyingOnGeom_i::SetShapeName( const char* theName )
{
  delete myShapeName;
  myShapeName = strdup( theName );
  myLyingOnGeomPtr->SetGeom( getShapeByName( myShapeName ) );
}

char* LyingOnGeom_i::GetShapeName()
{
  return CORBA::string_dup( myShapeName );
}

/*
  Class       : FreeBorders_i
  Description : Predicate for free borders
*/
FreeBorders_i::FreeBorders_i()
{
  myPredicatePtr.reset(new Controls::FreeBorders());
  myFunctorPtr = myPredicatePtr;
}

FunctorType FreeBorders_i::GetFunctorType()
{
  return SMESH::FT_FreeBorders;
}

/*
  Class       : FreeEdges_i
  Description : Predicate for free borders
*/
FreeEdges_i::FreeEdges_i()
: myFreeEdgesPtr( new Controls::FreeEdges() )
{
  myFunctorPtr = myPredicatePtr = myFreeEdgesPtr;
}

SMESH::FreeEdges::Borders* FreeEdges_i::GetBorders()
{
  INFOS("FreeEdges_i::GetBorders");
  SMESH::Controls::FreeEdges::TBorders aBorders;
  myFreeEdgesPtr->GetBoreders( aBorders );
  
  long i = 0, iEnd = aBorders.size();

  SMESH::FreeEdges::Borders_var aResult = new SMESH::FreeEdges::Borders(iEnd);

  SMESH::Controls::FreeEdges::TBorders::const_iterator anIter;
  for ( anIter = aBorders.begin() ; anIter != aBorders.end(); anIter++, i++ )
  {
    const SMESH::Controls::FreeEdges::Border&  aBord = *anIter;
    SMESH::FreeEdges::Border &aBorder = aResult[ i ];
    
    aBorder.myElemId = aBord.myElemId;
    aBorder.myPnt1 = aBord.myPntId[ 0 ];
    aBorder.myPnt2 = aBord.myPntId[ 1 ];
  }

  INFOS("FreeEdges_i::GetBorders~");
  return aResult._retn();
}

FunctorType FreeEdges_i::GetFunctorType()
{
  return SMESH::FT_FreeEdges;
}

/*
  Class       : RangeOfIds_i
  Description : Predicate for Range of Ids.
                Range may be specified with two ways.
                1. Using AddToRange method
                2. With SetRangeStr method. Parameter of this method is a string
                   like as "1,2,3,50-60,63,67,70-"
*/

RangeOfIds_i::RangeOfIds_i()
{
  myRangeOfIdsPtr.reset( new Controls::RangeOfIds() );
  myFunctorPtr = myPredicatePtr = myRangeOfIdsPtr;
}

void RangeOfIds_i::SetRange( const SMESH::long_array& theIds )
{
  CORBA::Long iEnd = theIds.length();
  for ( CORBA::Long i = 0; i < iEnd; i++ )
    myRangeOfIdsPtr->AddToRange( theIds[ i ] );
}

CORBA::Boolean RangeOfIds_i::SetRangeStr( const char* theRange )
{
  return myRangeOfIdsPtr->SetRangeStr(
    TCollection_AsciiString( (Standard_CString)theRange ) );
}

char* RangeOfIds_i::GetRangeStr()
{
  TCollection_AsciiString aStr;
  myRangeOfIdsPtr->GetRangeStr( aStr );
  return CORBA::string_dup( aStr.ToCString() );
}

void RangeOfIds_i::SetElementType( ElementType theType )
{
  myRangeOfIdsPtr->SetType( SMDSAbs_ElementType( theType ) );
}

FunctorType RangeOfIds_i::GetFunctorType()
{
  return SMESH::FT_RangeOfIds;
}

/*
  Class       : Comparator_i
  Description : Base class for comparators
*/
Comparator_i::Comparator_i():
  myNumericalFunctor( NULL )
{}

Comparator_i::~Comparator_i()
{
  if ( myNumericalFunctor )
    myNumericalFunctor->Destroy();
}

void Comparator_i::SetMargin( CORBA::Double theValue )
{
  myComparatorPtr->SetMargin( theValue );
}

CORBA::Double Comparator_i::GetMargin()
{
  return myComparatorPtr->GetMargin();
}

void Comparator_i::SetNumFunctor( NumericalFunctor_ptr theFunct )
{
  if ( myNumericalFunctor )
    myNumericalFunctor->Destroy();

  myNumericalFunctor = dynamic_cast<NumericalFunctor_i*>( SMESH_Gen_i::GetServant( theFunct ).in() );

  if ( myNumericalFunctor )
  {
    myComparatorPtr->SetNumFunctor( myNumericalFunctor->GetNumericalFunctor() );
    myNumericalFunctor->Register();
  }
}

Controls::ComparatorPtr Comparator_i::GetComparator()
{
  return myComparatorPtr;
}

NumericalFunctor_i* Comparator_i::GetNumFunctor_i()
{
  return myNumericalFunctor;
}


/*
  Class       : LessThan_i
  Description : Comparator "<"
*/
LessThan_i::LessThan_i()
{
  myComparatorPtr.reset( new Controls::LessThan() );
  myFunctorPtr = myPredicatePtr = myComparatorPtr;
}

FunctorType LessThan_i::GetFunctorType()
{
  return SMESH::FT_LessThan;
}


/*
  Class       : MoreThan_i
  Description : Comparator ">"
*/
MoreThan_i::MoreThan_i()
{
  myComparatorPtr.reset( new Controls::MoreThan() );
  myFunctorPtr = myPredicatePtr = myComparatorPtr;
}

FunctorType MoreThan_i::GetFunctorType()
{
  return SMESH::FT_MoreThan;
}


/*
  Class       : EqualTo_i
  Description : Comparator "="
*/
EqualTo_i::EqualTo_i()
: myEqualToPtr( new Controls::EqualTo() )
{
  myFunctorPtr = myPredicatePtr = myComparatorPtr = myEqualToPtr;
}

void EqualTo_i::SetTolerance( CORBA::Double theToler )
{
  myEqualToPtr->SetTolerance( theToler );
}

CORBA::Double EqualTo_i::GetTolerance()
{
  return myEqualToPtr->GetTolerance();
}

FunctorType EqualTo_i::GetFunctorType()
{
  return SMESH::FT_EqualTo;
}

/*
  Class       : LogicalNOT_i
  Description : Logical NOT predicate
*/
LogicalNOT_i::LogicalNOT_i()
: myPredicate( NULL ),
  myLogicalNOTPtr( new Controls::LogicalNOT() )
{
  myFunctorPtr = myPredicatePtr = myLogicalNOTPtr;
}

LogicalNOT_i::~LogicalNOT_i()
{
  if ( myPredicate )
    myPredicate->Destroy();
}

void LogicalNOT_i::SetPredicate( Predicate_ptr thePred )
{
  if ( myPredicate )
    myPredicate->Destroy();

  myPredicate = dynamic_cast<Predicate_i*>( SMESH_Gen_i::GetServant( thePred ).in() );

  if ( myPredicate ){
    myLogicalNOTPtr->SetPredicate(myPredicate->GetPredicate());
    myPredicate->Register();
  }
}

FunctorType LogicalNOT_i::GetFunctorType()
{
  return SMESH::FT_LogicalNOT;
}

Predicate_i* LogicalNOT_i::GetPredicate_i()
{
  return myPredicate;
}


/*
  Class       : LogicalBinary_i
  Description : Base class for binary logical predicate
*/
LogicalBinary_i::LogicalBinary_i()
: myPredicate1( NULL ),
  myPredicate2( NULL )
{}

LogicalBinary_i::~LogicalBinary_i()
{
  if ( myPredicate1 )
    myPredicate1->Destroy();

  if ( myPredicate2 )
    myPredicate2->Destroy();
}

void LogicalBinary_i::SetMesh( SMESH_Mesh_ptr theMesh )
{
  if ( myPredicate1 )
    myPredicate1->SetMesh( theMesh );

  if ( myPredicate2 )
    myPredicate2->SetMesh( theMesh );
}

void LogicalBinary_i::SetPredicate1( Predicate_ptr thePredicate )
{
  if ( myPredicate1 )
    myPredicate1->Destroy();

  myPredicate1 = dynamic_cast<Predicate_i*>( SMESH_Gen_i::GetServant( thePredicate ).in() );

  if ( myPredicate1 ){
    myLogicalBinaryPtr->SetPredicate1(myPredicate1->GetPredicate());
    myPredicate1->Register();
  }
}

void LogicalBinary_i::SetPredicate2( Predicate_ptr thePredicate )
{
  if ( myPredicate2 )
    myPredicate2->Destroy();

  myPredicate2 = dynamic_cast<Predicate_i*>( SMESH_Gen_i::GetServant( thePredicate ).in() );

  if ( myPredicate2 ){
    myLogicalBinaryPtr->SetPredicate2(myPredicate2->GetPredicate());
    myPredicate2->Register();
  }
}

Controls::LogicalBinaryPtr LogicalBinary_i::GetLogicalBinary()
{
  return myLogicalBinaryPtr;
}

Predicate_i* LogicalBinary_i::GetPredicate1_i()
{
  return myPredicate1;
}
Predicate_i* LogicalBinary_i::GetPredicate2_i()
{
  return myPredicate2;
}


/*
  Class       : LogicalAND_i
  Description : Logical AND
*/
LogicalAND_i::LogicalAND_i()
{
  myLogicalBinaryPtr.reset( new Controls::LogicalAND() );
  myFunctorPtr = myPredicatePtr = myLogicalBinaryPtr;
}

FunctorType LogicalAND_i::GetFunctorType()
{
  return SMESH::FT_LogicalAND;
}


/*
  Class       : LogicalOR_i
  Description : Logical OR
*/
LogicalOR_i::LogicalOR_i()
{
  myLogicalBinaryPtr.reset( new Controls::LogicalOR() );
  myFunctorPtr = myPredicatePtr = myLogicalBinaryPtr;
}

FunctorType LogicalOR_i::GetFunctorType()
{
  return SMESH::FT_LogicalOR;
}


/*
                            FILTER MANAGER
*/

FilterManager_i::FilterManager_i()
: SALOME::GenericObj_i( SMESH_Gen_i::GetPOA() )
{
  SMESH_Gen_i::GetPOA()->activate_object( this );
}

MinimumAngle_ptr FilterManager_i::CreateMinimumAngle()
{
  SMESH::MinimumAngle_i* aServant = new SMESH::MinimumAngle_i();
  SMESH::MinimumAngle_var anObj = aServant->_this();
  return anObj._retn();
}


AspectRatio_ptr FilterManager_i::CreateAspectRatio()
{
  SMESH::AspectRatio_i* aServant = new SMESH::AspectRatio_i();
  SMESH::AspectRatio_var anObj = aServant->_this();
  return anObj._retn();
}


AspectRatio3D_ptr FilterManager_i::CreateAspectRatio3D()
{
  SMESH::AspectRatio3D_i* aServant = new SMESH::AspectRatio3D_i();
  SMESH::AspectRatio3D_var anObj = aServant->_this();
  return anObj._retn();
}


Warping_ptr FilterManager_i::CreateWarping()
{
  SMESH::Warping_i* aServant = new SMESH::Warping_i();
  SMESH::Warping_var anObj = aServant->_this();
  return anObj._retn();
}


Taper_ptr FilterManager_i::CreateTaper()
{
  SMESH::Taper_i* aServant = new SMESH::Taper_i();
  SMESH::Taper_var anObj = aServant->_this();
  return anObj._retn();
}


Skew_ptr FilterManager_i::CreateSkew()
{
  SMESH::Skew_i* aServant = new SMESH::Skew_i();
  SMESH::Skew_var anObj = aServant->_this();
  return anObj._retn();
}


Area_ptr FilterManager_i::CreateArea()
{
  SMESH::Area_i* aServant = new SMESH::Area_i();
  SMESH::Area_var anObj = aServant->_this();
  return anObj._retn();
}


Length_ptr FilterManager_i::CreateLength()
{
  SMESH::Length_i* aServant = new SMESH::Length_i();
  SMESH::Length_var anObj = aServant->_this();
  return anObj._retn();
}

Length2D_ptr FilterManager_i::CreateLength2D()
{
  SMESH::Length2D_i* aServant = new SMESH::Length2D_i();
  SMESH::Length2D_var anObj = aServant->_this();
  return anObj._retn();
}

MultiConnection_ptr FilterManager_i::CreateMultiConnection()
{
  SMESH::MultiConnection_i* aServant = new SMESH::MultiConnection_i();
  SMESH::MultiConnection_var anObj = aServant->_this();
  return anObj._retn();
}

MultiConnection2D_ptr FilterManager_i::CreateMultiConnection2D()
{
  SMESH::MultiConnection2D_i* aServant = new SMESH::MultiConnection2D_i();
  SMESH::MultiConnection2D_var anObj = aServant->_this();
  return anObj._retn();
}

BelongToGeom_ptr FilterManager_i::CreateBelongToGeom()
{
  SMESH::BelongToGeom_i* aServant = new SMESH::BelongToGeom_i();
  SMESH::BelongToGeom_var anObj = aServant->_this();
  return anObj._retn();
}

BelongToPlane_ptr FilterManager_i::CreateBelongToPlane()
{
  SMESH::BelongToPlane_i* aServant = new SMESH::BelongToPlane_i();
  SMESH::BelongToPlane_var anObj = aServant->_this();
  return anObj._retn();
}

BelongToCylinder_ptr FilterManager_i::CreateBelongToCylinder()
{
  SMESH::BelongToCylinder_i* aServant = new SMESH::BelongToCylinder_i();
  SMESH::BelongToCylinder_var anObj = aServant->_this();
  return anObj._retn();
}

LyingOnGeom_ptr FilterManager_i::CreateLyingOnGeom()
{
  SMESH::LyingOnGeom_i* aServant = new SMESH::LyingOnGeom_i();
  SMESH::LyingOnGeom_var anObj = aServant->_this();
  return anObj._retn();
}

FreeBorders_ptr FilterManager_i::CreateFreeBorders()
{
  SMESH::FreeBorders_i* aServant = new SMESH::FreeBorders_i();
  SMESH::FreeBorders_var anObj = aServant->_this();
  return anObj._retn();
}

FreeEdges_ptr FilterManager_i::CreateFreeEdges()
{
  SMESH::FreeEdges_i* aServant = new SMESH::FreeEdges_i();
  SMESH::FreeEdges_var anObj = aServant->_this();
  return anObj._retn();
}

RangeOfIds_ptr FilterManager_i::CreateRangeOfIds()
{
  SMESH::RangeOfIds_i* aServant = new SMESH::RangeOfIds_i();
  SMESH::RangeOfIds_var anObj = aServant->_this();
  return anObj._retn();
}

BadOrientedVolume_ptr FilterManager_i::CreateBadOrientedVolume()
{
  SMESH::BadOrientedVolume_i* aServant = new SMESH::BadOrientedVolume_i();
  SMESH::BadOrientedVolume_var anObj = aServant->_this();
  return anObj._retn();
}

LessThan_ptr FilterManager_i::CreateLessThan()
{
  SMESH::LessThan_i* aServant = new SMESH::LessThan_i();
  SMESH::LessThan_var anObj = aServant->_this();
  return anObj._retn();
}


MoreThan_ptr FilterManager_i::CreateMoreThan()
{
  SMESH::MoreThan_i* aServant = new SMESH::MoreThan_i();
  SMESH::MoreThan_var anObj = aServant->_this();
  return anObj._retn();
}

EqualTo_ptr FilterManager_i::CreateEqualTo()
{
  SMESH::EqualTo_i* aServant = new SMESH::EqualTo_i();
  SMESH::EqualTo_var anObj = aServant->_this();
  return anObj._retn();
}


LogicalNOT_ptr FilterManager_i::CreateLogicalNOT()
{
  SMESH::LogicalNOT_i* aServant = new SMESH::LogicalNOT_i();
  SMESH::LogicalNOT_var anObj = aServant->_this();
  return anObj._retn();
}


LogicalAND_ptr FilterManager_i::CreateLogicalAND()
{
  SMESH::LogicalAND_i* aServant = new SMESH::LogicalAND_i();
  SMESH::LogicalAND_var anObj = aServant->_this();
  return anObj._retn();
}


LogicalOR_ptr FilterManager_i::CreateLogicalOR()
{
  SMESH::LogicalOR_i* aServant = new SMESH::LogicalOR_i();
  SMESH::LogicalOR_var anObj = aServant->_this();
  return anObj._retn();
}

Filter_ptr FilterManager_i::CreateFilter()
{
  SMESH::Filter_i* aServant = new SMESH::Filter_i();
  SMESH::Filter_var anObj = aServant->_this();
  return anObj._retn();
}

FilterLibrary_ptr FilterManager_i::LoadLibrary( const char* aFileName )
{
  SMESH::FilterLibrary_i* aServant = new SMESH::FilterLibrary_i( aFileName );
  SMESH::FilterLibrary_var anObj = aServant->_this();
  return anObj._retn();
}

FilterLibrary_ptr FilterManager_i::CreateLibrary()
{
  SMESH::FilterLibrary_i* aServant = new SMESH::FilterLibrary_i();
  SMESH::FilterLibrary_var anObj = aServant->_this();
  return anObj._retn();
}

CORBA::Boolean FilterManager_i::DeleteLibrary( const char* aFileName )
{
  return remove( aFileName ) ? false : true;
}

//=============================================================================
/*!
 *  SMESH_Gen_i::CreateFilterManager
 *
 *  Create filter manager
 */
//=============================================================================

SMESH::FilterManager_ptr SMESH_Gen_i::CreateFilterManager()
{
  SMESH::FilterManager_i* aFilter = new SMESH::FilterManager_i();
  SMESH::FilterManager_var anObj = aFilter->_this();
  return anObj._retn();
}


/*
                              FILTER
*/

//=======================================================================
// name    : Filter_i::Filter_i
// Purpose : Constructor
//=======================================================================
Filter_i::Filter_i()
: myPredicate( NULL )
{}

//=======================================================================
// name    : Filter_i::~Filter_i
// Purpose : Destructor
//=======================================================================
Filter_i::~Filter_i()
{
  if ( myPredicate )
    myPredicate->Destroy();
}

//=======================================================================
// name    : Filter_i::SetPredicate
// Purpose : Set predicate
//=======================================================================
void Filter_i::SetPredicate( Predicate_ptr thePredicate )
{
  if ( myPredicate )
    myPredicate->Destroy();

  myPredicate = dynamic_cast<Predicate_i*>( SMESH_Gen_i::GetServant( thePredicate ).in() );

  if ( myPredicate )
  {
    myFilter.SetPredicate( myPredicate->GetPredicate() );
    myPredicate->Register();
  }
}

//=======================================================================
// name    : Filter_i::GetElementType
// Purpose : Get entity type
//=======================================================================
SMESH::ElementType Filter_i::GetElementType()
{
  return myPredicate != 0 ? myPredicate->GetElementType() : SMESH::ALL;
}

//=======================================================================
// name    : Filter_i::SetMesh
// Purpose : Set mesh
//=======================================================================
void Filter_i::SetMesh( SMESH_Mesh_ptr theMesh )
{
  if ( myPredicate )
    myPredicate->SetMesh( theMesh );
}

//=======================================================================
// name    : Filter_i::GetElementsId
// Purpose : Get ids of entities
//=======================================================================
SMESH::long_array* Filter_i::GetElementsId( SMESH_Mesh_ptr theMesh )
{
  SMDS_Mesh* aMesh = MeshPtr2SMDSMesh(theMesh);
  Controls::Filter::TIdSequence aSequence = myFilter.GetElementsId(aMesh);

  SMESH::long_array_var anArray = new SMESH::long_array;
  long i = 0, iEnd = aSequence.size();

  anArray->length( iEnd );
  for ( ; i < iEnd; i++ )
    anArray[ i ] = aSequence[i];

  return anArray._retn();
}

//=======================================================================
// name    : getCriteria
// Purpose : Retrieve criterions from predicate
//=======================================================================
static inline bool getCriteria( Predicate_i*                thePred,
                                SMESH::Filter::Criteria_out theCriteria )
{
  int aFType = thePred->GetFunctorType();

  switch ( aFType )
  {
  case FT_FreeBorders:
  case FT_FreeEdges:
    {
      CORBA::ULong i = theCriteria->length();
      theCriteria->length( i + 1 );

      theCriteria[ i ] = createCriterion();

      theCriteria[ i ].Type = aFType;
      theCriteria[ i ].TypeOfElement = thePred->GetElementType();
      return true;
    }
  case FT_BelongToGeom:
    {
      BelongToGeom_i* aPred = dynamic_cast<BelongToGeom_i*>( thePred );

      CORBA::ULong i = theCriteria->length();
      theCriteria->length( i + 1 );

      theCriteria[ i ] = createCriterion();

      theCriteria[ i ].Type          = FT_BelongToGeom;
      theCriteria[ i ].ThresholdStr  = aPred->GetShapeName();
      theCriteria[ i ].TypeOfElement = aPred->GetElementType();

      return true;
    }
  case FT_BelongToPlane:
  case FT_BelongToCylinder:
    {
      BelongToSurface_i* aPred = dynamic_cast<BelongToSurface_i*>( thePred );

      CORBA::ULong i = theCriteria->length();
      theCriteria->length( i + 1 );

      theCriteria[ i ] = createCriterion();

      theCriteria[ i ].Type          = aFType;
      theCriteria[ i ].ThresholdStr  = aPred->GetShapeName();
      theCriteria[ i ].TypeOfElement = aPred->GetElementType();
      theCriteria[ i ].Tolerance     = aPred->GetTolerance();

      return true;
    }
   case FT_LyingOnGeom:
    {
      LyingOnGeom_i* aPred = dynamic_cast<LyingOnGeom_i*>( thePred );

      CORBA::ULong i = theCriteria->length();
      theCriteria->length( i + 1 );

      theCriteria[ i ] = createCriterion();

      theCriteria[ i ].Type          = FT_LyingOnGeom;
      theCriteria[ i ].ThresholdStr  = aPred->GetShapeName();
      theCriteria[ i ].TypeOfElement = aPred->GetElementType();

      return true;
    }
  case FT_RangeOfIds:
    {
      RangeOfIds_i* aPred = dynamic_cast<RangeOfIds_i*>( thePred );

      CORBA::ULong i = theCriteria->length();
      theCriteria->length( i + 1 );

      theCriteria[ i ] = createCriterion();

      theCriteria[ i ].Type          = FT_RangeOfIds;
      theCriteria[ i ].ThresholdStr  = aPred->GetRangeStr();
      theCriteria[ i ].TypeOfElement = aPred->GetElementType();

      return true;
    }
  case FT_BadOrientedVolume:
    {
      BadOrientedVolume_i* aPred = dynamic_cast<BadOrientedVolume_i*>( thePred );

      CORBA::ULong i = theCriteria->length();
      theCriteria->length( i + 1 );

      theCriteria[ i ] = createCriterion();

      theCriteria[ i ].Type          = FT_BadOrientedVolume;
      theCriteria[ i ].TypeOfElement = aPred->GetElementType();

      return true;
    }
  case FT_LessThan:
  case FT_MoreThan:
  case FT_EqualTo:
    {
      Comparator_i* aCompar = dynamic_cast<Comparator_i*>( thePred );

      CORBA::ULong i = theCriteria->length();
      theCriteria->length( i + 1 );

      theCriteria[ i ] = createCriterion();

      theCriteria[ i ].Type      = aCompar->GetNumFunctor_i()->GetFunctorType();
      theCriteria[ i ].Compare   = aFType;
      theCriteria[ i ].Threshold = aCompar->GetMargin();
      theCriteria[ i ].TypeOfElement = aCompar->GetElementType();

      if ( aFType == FT_EqualTo )
      {
        EqualTo_i* aCompar = dynamic_cast<EqualTo_i*>( thePred );
        theCriteria[ i ].Tolerance = aCompar->GetTolerance();
      }
    }
    return true;

  case FT_LogicalNOT:
    {
      Predicate_i* aPred = ( dynamic_cast<LogicalNOT_i*>( thePred ) )->GetPredicate_i();
      getCriteria( aPred, theCriteria );
      theCriteria[ theCriteria->length() - 1 ].UnaryOp = FT_LogicalNOT;
    }
    return true;

  case FT_LogicalAND:
  case FT_LogicalOR:
    {
      Predicate_i* aPred1 = ( dynamic_cast<LogicalBinary_i*>( thePred ) )->GetPredicate1_i();
      Predicate_i* aPred2 = ( dynamic_cast<LogicalBinary_i*>( thePred ) )->GetPredicate2_i();
      if ( !getCriteria( aPred1, theCriteria ) )
        return false;
      theCriteria[ theCriteria->length() - 1 ].BinaryOp = aFType;
      return getCriteria( aPred2, theCriteria );
    }

  case FT_Undefined:
    return false;
  default:
    return false;
  }
}

//=======================================================================
// name    : Filter_i::GetCriteria
// Purpose : Retrieve criterions from predicate
//=======================================================================
CORBA::Boolean Filter_i::GetCriteria( SMESH::Filter::Criteria_out theCriteria )
{
  theCriteria = new SMESH::Filter::Criteria;
  return myPredicate != 0 ? getCriteria( myPredicate, theCriteria ) : true;
}

//=======================================================================
// name    : Filter_i::SetCriteria
// Purpose : Create new predicate and set criterions in it
//=======================================================================
CORBA::Boolean Filter_i::SetCriteria( const SMESH::Filter::Criteria& theCriteria )
{
  if ( myPredicate != 0 )
    myPredicate->Destroy();

    SMESH::FilterManager_i* aFilter = new SMESH::FilterManager_i();
    FilterManager_ptr aFilterMgr = aFilter->_this();

  // CREATE two lists ( PREDICATES  and LOG OP )

  // Criterion
  std::list<SMESH::Predicate_ptr> aPredicates;
  std::list<int>                  aBinaries;
  for ( int i = 0, n = theCriteria.length(); i < n; i++ )
  {
    int         aCriterion    = theCriteria[ i ].Type;
    int         aCompare      = theCriteria[ i ].Compare;
    double      aThreshold    = theCriteria[ i ].Threshold;
    int         aUnary        = theCriteria[ i ].UnaryOp;
    int         aBinary       = theCriteria[ i ].BinaryOp;
    double      aTolerance    = theCriteria[ i ].Tolerance;
    const char* aThresholdStr = theCriteria[ i ].ThresholdStr;
    ElementType aTypeOfElem   = theCriteria[ i ].TypeOfElement;
    long        aPrecision    = theCriteria[ i ].Precision;
    
    SMESH::Predicate_ptr aPredicate = SMESH::Predicate::_nil();
    SMESH::NumericalFunctor_ptr aFunctor = SMESH::NumericalFunctor::_nil();

    switch ( aCriterion )
    {
      // Functors
      
      case SMESH::FT_MultiConnection:
        aFunctor = aFilterMgr->CreateMultiConnection();
        break;
      case SMESH::FT_MultiConnection2D:
	aFunctor = aFilterMgr->CreateMultiConnection2D();
	break;
      case SMESH::FT_Length:
        aFunctor = aFilterMgr->CreateLength();
        break;
      case SMESH::FT_Length2D:
        aFunctor = aFilterMgr->CreateLength2D();
        break;
      case SMESH::FT_AspectRatio:
        aFunctor = aFilterMgr->CreateAspectRatio();
        break;
      case SMESH::FT_AspectRatio3D:
        aFunctor = aFilterMgr->CreateAspectRatio3D();
        break;
      case SMESH::FT_Warping:
        aFunctor = aFilterMgr->CreateWarping();
        break;
      case SMESH::FT_MinimumAngle:
        aFunctor = aFilterMgr->CreateMinimumAngle();
        break;
      case SMESH::FT_Taper:
        aFunctor = aFilterMgr->CreateTaper();
        break;
      case SMESH::FT_Skew:
        aFunctor = aFilterMgr->CreateSkew();
        break;
      case SMESH::FT_Area:
        aFunctor = aFilterMgr->CreateArea();
        break;

      // Predicates

      case SMESH::FT_FreeBorders:
        aPredicate = aFilterMgr->CreateFreeBorders();
        break;
      case SMESH::FT_FreeEdges:
        aPredicate = aFilterMgr->CreateFreeEdges();
        break;
      case SMESH::FT_BelongToGeom:
        {
          SMESH::BelongToGeom_ptr tmpPred = aFilterMgr->CreateBelongToGeom();
          tmpPred->SetElementType( aTypeOfElem );
          tmpPred->SetShapeName( aThresholdStr );
          aPredicate = tmpPred;
        }
        break;
      case SMESH::FT_BelongToPlane:
      case SMESH::FT_BelongToCylinder:
        {
          SMESH::BelongToSurface_ptr tmpPred;
          if ( aCriterion == SMESH::FT_BelongToPlane )
            tmpPred = aFilterMgr->CreateBelongToPlane();
          else
            tmpPred = aFilterMgr->CreateBelongToCylinder();
          tmpPred->SetShapeName( aThresholdStr, aTypeOfElem );
          tmpPred->SetTolerance( aTolerance );
          aPredicate = tmpPred;
        }
        break;
      case SMESH::FT_LyingOnGeom:
        {
          SMESH::LyingOnGeom_ptr tmpPred = aFilterMgr->CreateLyingOnGeom();
          tmpPred->SetElementType( aTypeOfElem );
          tmpPred->SetShapeName( aThresholdStr );
          aPredicate = tmpPred;
        }
        break; 
      case SMESH::FT_RangeOfIds:
        {
          SMESH::RangeOfIds_ptr tmpPred = aFilterMgr->CreateRangeOfIds();
          tmpPred->SetRangeStr( aThresholdStr );
          tmpPred->SetElementType( aTypeOfElem );
          aPredicate = tmpPred;
        }
        break;
      case SMESH::FT_BadOrientedVolume:
        {
          aPredicate = aFilterMgr->CreateBadOrientedVolume();
        }
        break;
              
      default:
        continue;
    }

    // Comparator
    if ( !aFunctor->_is_nil() && aPredicate->_is_nil() )
    {
      SMESH::Comparator_ptr aComparator = SMESH::Comparator::_nil();

      if ( aCompare == SMESH::FT_LessThan )
        aComparator = aFilterMgr->CreateLessThan();
      else if ( aCompare == SMESH::FT_MoreThan )
        aComparator = aFilterMgr->CreateMoreThan();
      else if ( aCompare == SMESH::FT_EqualTo )
        aComparator = aFilterMgr->CreateEqualTo();
      else
        continue;

      aComparator->SetNumFunctor( aFunctor );
      aComparator->SetMargin( aThreshold );

      if ( aCompare == FT_EqualTo )
      {
        SMESH::EqualTo_var anEqualTo = SMESH::EqualTo::_narrow( aComparator );
        anEqualTo->SetTolerance( aTolerance );
      }

      aPredicate = aComparator;

      aFunctor->SetPrecision( aPrecision );
    }

    // Logical not
    if ( aUnary == FT_LogicalNOT )
    {
      SMESH::LogicalNOT_ptr aNotPred = aFilterMgr->CreateLogicalNOT();
      aNotPred->SetPredicate( aPredicate );
      aPredicate = aNotPred;
    }

    // logical op
    aPredicates.push_back( aPredicate );
    aBinaries.push_back( aBinary );

  } // end of for

  // CREATE ONE PREDICATE FROM PREVIOUSLY CREATED MAP

  // combine all "AND" operations

  std::list<SMESH::Predicate_ptr> aResList;

  std::list<SMESH::Predicate_ptr>::iterator aPredIter;
  std::list<int>::iterator                  aBinaryIter;

  SMESH::Predicate_ptr aPrevPredicate = SMESH::Predicate::_nil();
  int aPrevBinary = SMESH::FT_Undefined;

  for ( aPredIter = aPredicates.begin(), aBinaryIter = aBinaries.begin();
        aPredIter != aPredicates.end() && aBinaryIter != aBinaries.end();
        ++aPredIter, ++aBinaryIter )
  {
    int aCurrBinary = *aBinaryIter;

    SMESH::Predicate_ptr aCurrPred = SMESH::Predicate::_nil();

    if ( aPrevBinary == SMESH::FT_LogicalAND )
    {

      SMESH::LogicalBinary_ptr aBinaryPred = aFilterMgr->CreateLogicalAND();
      aBinaryPred->SetPredicate1( aPrevPredicate );
      aBinaryPred->SetPredicate2( *aPredIter );
      aCurrPred = aBinaryPred;
    }
    else
      aCurrPred = *aPredIter;

    if ( aCurrBinary != SMESH::FT_LogicalAND )
      aResList.push_back( aCurrPred );

    aPrevPredicate = aCurrPred;
    aPrevBinary = aCurrBinary;
  }

  // combine all "OR" operations

  SMESH::Predicate_ptr aResPredicate = SMESH::Predicate::_nil();

  if ( aResList.size() == 1 )
    aResPredicate = *aResList.begin();
  else if ( aResList.size() > 1 )
  {
    std::list<SMESH::Predicate_ptr>::iterator anIter = aResList.begin();
    aResPredicate = *anIter;
    anIter++;
    for ( ; anIter != aResList.end(); ++anIter )
    {
      SMESH::LogicalBinary_ptr aBinaryPred = aFilterMgr->CreateLogicalOR();
      aBinaryPred->SetPredicate1( aResPredicate );
      aBinaryPred->SetPredicate2( *anIter );
      aResPredicate = aBinaryPred;
    }
  }

  SetPredicate( aResPredicate );

  return !aResPredicate->_is_nil();
}

//=======================================================================
// name    : Filter_i::GetPredicate_i
// Purpose : Get implementation of predicate
//=======================================================================
Predicate_i* Filter_i::GetPredicate_i()
{
  return myPredicate;
}

//=======================================================================
// name    : Filter_i::GetPredicate
// Purpose : Get predicate
//=======================================================================
Predicate_ptr Filter_i::GetPredicate()
{
  if ( myPredicate == 0 )
    return SMESH::Predicate::_nil();
  else
  {
    SMESH::Predicate_var anObj = myPredicate->_this();
    return anObj._retn();
  }
}

/*
                            FILTER LIBRARY
*/

#define ATTR_TYPE          "type"
#define ATTR_COMPARE       "compare"
#define ATTR_THRESHOLD     "threshold"
#define ATTR_UNARY         "unary"
#define ATTR_BINARY        "binary"
#define ATTR_THRESHOLD_STR "threshold_str"
#define ATTR_TOLERANCE     "tolerance"
#define ATTR_ELEMENT_TYPE  "ElementType"

//=======================================================================
// name    : toString
// Purpose : Convert bool to LDOMString
//=======================================================================
static inline LDOMString toString( const bool val )
{
  return val ? "logical not" : "";
}

//=======================================================================
// name    : toBool
// Purpose : Convert LDOMString to bool
//=======================================================================
static inline bool toBool( const LDOMString& theStr )
{
  return theStr.equals( "logical not" );
}

//=======================================================================
// name    : toString
// Purpose : Convert double to LDOMString
//=======================================================================
static inline LDOMString toString( const double val )
{
  char a[ 255 ];
  sprintf( a, "%e", val );
  return LDOMString( a );
}

//=======================================================================
// name    : toDouble
// Purpose : Convert LDOMString to double
//=======================================================================
static inline double toDouble( const LDOMString& theStr )
{
  return atof( theStr.GetString() );
}

//=======================================================================
// name    : toString
// Purpose : Convert functor type to LDOMString
//=======================================================================
static inline LDOMString toString( const long theType )
{
  switch ( theType )
  {
    case FT_AspectRatio     : return "Aspect ratio";
    case FT_Warping         : return "Warping";
    case FT_MinimumAngle    : return "Minimum angle";
    case FT_Taper           : return "Taper";
    case FT_Skew            : return "Skew";
    case FT_Area            : return "Area";
    case FT_BelongToGeom    : return "Belong to Geom";
    case FT_BelongToPlane   : return "Belong to Plane";
    case FT_BelongToCylinder: return "Belong to Cylinder";
    case FT_LyingOnGeom     : return "Lying on Geom";
    case FT_BadOrientedVolume: return "Bad Oriented Volume";
    case FT_RangeOfIds      : return "Range of IDs";
    case FT_FreeBorders     : return "Free borders";
    case FT_FreeEdges       : return "Free edges";
    case FT_MultiConnection : return "Borders at multi-connections";
    case FT_MultiConnection2D: return "Borders at multi-connections 2D";
    case FT_Length          : return "Length";
    case FT_Length2D        : return "Length2D";
    case FT_LessThan        : return "Less than";
    case FT_MoreThan        : return "More than";
    case FT_EqualTo         : return "Equal to";
    case FT_LogicalNOT      : return "Not";
    case FT_LogicalAND      : return "And";
    case FT_LogicalOR       : return "Or";
    case FT_Undefined       : return "";
    default                 : return "";
  }
}

//=======================================================================
// name    : toFunctorType
// Purpose : Convert LDOMString to functor type
//=======================================================================
static inline SMESH::FunctorType toFunctorType( const LDOMString& theStr )
{
  if      ( theStr.equals( "Aspect ratio"                 ) ) return FT_AspectRatio;
  else if ( theStr.equals( "Warping"                      ) ) return FT_Warping;
  else if ( theStr.equals( "Minimum angle"                ) ) return FT_MinimumAngle;
  else if ( theStr.equals( "Taper"                        ) ) return FT_Taper;
  else if ( theStr.equals( "Skew"                         ) ) return FT_Skew;
  else if ( theStr.equals( "Area"                         ) ) return FT_Area;
  else if ( theStr.equals( "Belong to Geom"               ) ) return FT_BelongToGeom;
  else if ( theStr.equals( "Belong to Plane"              ) ) return FT_BelongToPlane;
  else if ( theStr.equals( "Belong to Cylinder"           ) ) return FT_BelongToCylinder;
  else if ( theStr.equals( "Lying on Geom"                ) ) return FT_LyingOnGeom;
  else if ( theStr.equals( "Free borders"                 ) ) return FT_FreeBorders;
  else if ( theStr.equals( "Free edges"                   ) ) return FT_FreeEdges;
  else if ( theStr.equals( "Borders at multi-connections" ) ) return FT_MultiConnection;
  //  else if ( theStr.equals( "Borders at multi-connections 2D" ) ) return FT_MultiConnection2D;
  else if ( theStr.equals( "Length"                       ) ) return FT_Length;
  //  else if ( theStr.equals( "Length2D"                     ) ) return FT_Length2D;
  else if ( theStr.equals( "Range of IDs"                 ) ) return FT_RangeOfIds;
  else if ( theStr.equals( "Bad Oriented Volume"          ) ) return FT_BadOrientedVolume;
  else if ( theStr.equals( "Less than"                    ) ) return FT_LessThan;
  else if ( theStr.equals( "More than"                    ) ) return FT_MoreThan;
  else if ( theStr.equals( "Equal to"                     ) ) return FT_EqualTo;
  else if ( theStr.equals( "Not"                          ) ) return FT_LogicalNOT;
  else if ( theStr.equals( "And"                          ) ) return FT_LogicalAND;
  else if ( theStr.equals( "Or"                           ) ) return FT_LogicalOR;
  else if ( theStr.equals( ""                             ) ) return FT_Undefined;
  else  return FT_Undefined;
}

//=======================================================================
// name    : toFunctorType
// Purpose : Convert LDOMString to value of ElementType enumeration
//=======================================================================
static inline SMESH::ElementType toElementType( const LDOMString& theStr )
{
  if      ( theStr.equals( "NODE"   ) ) return SMESH::NODE;
  else if ( theStr.equals( "EDGE"   ) ) return SMESH::EDGE;
  else if ( theStr.equals( "FACE"   ) ) return SMESH::FACE;
  else if ( theStr.equals( "VOLUME" ) ) return SMESH::VOLUME;
  else                                  return SMESH::ALL;
}

//=======================================================================
// name    : toString
// Purpose : Convert ElementType to string
//=======================================================================
static inline LDOMString toString( const SMESH::ElementType theType )
{
  switch ( theType )
  {
    case SMESH::NODE   : return "NODE";
    case SMESH::EDGE   : return "EDGE";
    case SMESH::FACE   : return "FACE";
    case SMESH::VOLUME : return "VOLUME";
    case SMESH::ALL    : return "ALL";
    default            : return "";
  }
}

//=======================================================================
// name    : findFilter
// Purpose : Find filter in document
//=======================================================================
static LDOM_Element findFilter( const char* theFilterName,
                                const LDOM_Document& theDoc,
                                LDOM_Node* theParent = 0 )
{
  LDOM_Element aRootElement = theDoc.getDocumentElement();
  if ( aRootElement.isNull() || !aRootElement.hasChildNodes() )
    return LDOM_Element();

  for ( LDOM_Node aTypeNode = aRootElement.getFirstChild();
        !aTypeNode.isNull(); aTypeNode = aTypeNode.getNextSibling() )
  {
    for ( LDOM_Node aFilter = aTypeNode.getFirstChild();
          !aFilter.isNull(); aFilter = aFilter.getNextSibling() )
    {
      LDOM_Element* anElem = ( LDOM_Element* )&aFilter;
      if ( anElem->getTagName().equals( LDOMString( "filter" ) ) &&
           anElem->getAttribute( "name" ).equals( LDOMString( theFilterName ) ) )
      {
        if ( theParent != 0  )
          *theParent = aTypeNode;
        return (LDOM_Element&)aFilter;
      }
    }
  }
  return LDOM_Element();
}

//=======================================================================
// name    : getSectionName
// Purpose : Get name of section of filters
//=======================================================================
static const char* getSectionName( const ElementType theType )
{
  switch ( theType )
  {
    case SMESH::NODE   : return "Filters for nodes";
    case SMESH::EDGE   : return "Filters for edges";
    case SMESH::FACE   : return "Filters for faces";
    case SMESH::VOLUME : return "Filters for volumes";
    case SMESH::ALL    : return "Filters for elements";
    default            : return "";
  }
}

//=======================================================================
// name    : getSection
// Purpose : Create section for filters corresponding to the entity type
//=======================================================================
static LDOM_Node getSection( const ElementType theType,
                             LDOM_Document&    theDoc,
                             const bool        toCreate = false )
{
  LDOM_Element aRootElement = theDoc.getDocumentElement();
  if ( aRootElement.isNull() )
    return LDOM_Node();

  // Find section
  bool anExist = false;
  const char* aSectionName = getSectionName( theType );
  if ( strcmp( aSectionName, "" ) == 0 )
    return LDOM_Node();
  
  LDOM_NodeList aSections = theDoc.getElementsByTagName( "section" );
  LDOM_Node aNode;
  for ( int i = 0, n = aSections.getLength(); i < n; i++ )
  {
    aNode = aSections.item( i );
    LDOM_Element& anItem = ( LDOM_Element& )aNode;
    if ( anItem.getAttribute( "name" ).equals( LDOMString( aSectionName ) ) )
    {
      anExist = true;
      break;
    }
  }

  // Create new section if necessary
  if ( !anExist )
  {
    if ( toCreate )
    {
      LDOM_Element aNewItem = theDoc.createElement( "section" );
      aNewItem.setAttribute( "name", aSectionName );
      aRootElement.appendChild( aNewItem );
      return aNewItem;
    }
    else
      return LDOM_Node();
  }
  return
    aNode;
}

//=======================================================================
// name    : createFilterItem
// Purpose : Create filter item or LDOM document
//=======================================================================
static LDOM_Element createFilterItem( const char*       theName,
                                      SMESH::Filter_ptr theFilter,
                                      LDOM_Document&    theDoc )
{
  // create new filter in document
  LDOM_Element aFilterItem = theDoc.createElement( "filter" );
  aFilterItem.setAttribute( "name", theName );

  // save filter criterions
  SMESH::Filter::Criteria_var aCriteria = new SMESH::Filter::Criteria;

  if ( !theFilter->GetCriteria( aCriteria ) )
    return LDOM_Element();

  for ( CORBA::ULong i = 0, n = aCriteria->length(); i < n; i++ )
  {
    LDOM_Element aCriterionItem = theDoc.createElement( "criterion" );

    aCriterionItem.setAttribute( ATTR_TYPE         , toString( aCriteria[ i ].Type      ) );
    aCriterionItem.setAttribute( ATTR_COMPARE      , toString( aCriteria[ i ].Compare   ) );
    aCriterionItem.setAttribute( ATTR_THRESHOLD    , toString( aCriteria[ i ].Threshold ) );
    aCriterionItem.setAttribute( ATTR_UNARY        , toString( aCriteria[ i ].UnaryOp   ) );
    aCriterionItem.setAttribute( ATTR_BINARY       , toString( aCriteria[ i ].BinaryOp  ) );
    
    aCriterionItem.setAttribute( ATTR_THRESHOLD_STR, (const char*)aCriteria[ i ].ThresholdStr );
    aCriterionItem.setAttribute( ATTR_TOLERANCE    , toString( aCriteria[ i ].Tolerance ) );
    aCriterionItem.setAttribute( ATTR_ELEMENT_TYPE ,
      toString( (SMESH::ElementType)aCriteria[ i ].TypeOfElement ) );

    aFilterItem.appendChild( aCriterionItem );
  }

  return aFilterItem;
}

//=======================================================================
// name    : FilterLibrary_i::FilterLibrary_i
// Purpose : Constructor
//=======================================================================
FilterLibrary_i::FilterLibrary_i( const char* theFileName )
{
  myFileName = strdup( theFileName );
  SMESH::FilterManager_i* aFilterMgr = new SMESH::FilterManager_i();
  myFilterMgr = aFilterMgr->_this();

  LDOMParser aParser;

  // Try to use existing library file
  bool anExists = false;
  if ( !aParser.parse( myFileName ) )
  {
    myDoc = aParser.getDocument();
    anExists = true;
  }
  // Create a new XML document if it doesn't exist
  else
    myDoc = LDOM_Document::createDocument( LDOMString() );

  LDOM_Element aRootElement = myDoc.getDocumentElement();
  if ( aRootElement.isNull() )
  {
    // If the existing document is empty --> try to create a new one
    if ( anExists )
      myDoc = LDOM_Document::createDocument( LDOMString() );
  }
}

//=======================================================================
// name    : FilterLibrary_i::FilterLibrary_i
// Purpose : Constructor
//=======================================================================
FilterLibrary_i::FilterLibrary_i()
{
  myFileName = 0;
  SMESH::FilterManager_i* aFilter = new SMESH::FilterManager_i();
  myFilterMgr = aFilter->_this();

  myDoc = LDOM_Document::createDocument( LDOMString() );
}

FilterLibrary_i::~FilterLibrary_i()
{
  delete myFileName;
}

//=======================================================================
// name    : FilterLibrary_i::Copy
// Purpose : Create filter and initialize it with values from library
//=======================================================================
Filter_ptr FilterLibrary_i::Copy( const char* theFilterName )
{
  Filter_ptr aRes;
  LDOM_Node aFilter = findFilter( theFilterName, myDoc );

  if ( aFilter.isNull() )
    return aRes;

  std::list<SMESH::Filter::Criterion> aCriteria;
    
  for ( LDOM_Node aCritNode = aFilter.getFirstChild();
        !aCritNode.isNull() ; aCritNode = aCritNode.getNextSibling() )
  {
    LDOM_Element* aCrit = (LDOM_Element*)&aCritNode;

    const char* aTypeStr      = aCrit->getAttribute( ATTR_TYPE          ).GetString();
    const char* aCompareStr   = aCrit->getAttribute( ATTR_COMPARE       ).GetString();
    const char* aUnaryStr     = aCrit->getAttribute( ATTR_UNARY         ).GetString();
    const char* aBinaryStr    = aCrit->getAttribute( ATTR_BINARY        ).GetString();
    const char* anElemTypeStr = aCrit->getAttribute( ATTR_ELEMENT_TYPE  ).GetString();
    
    SMESH::Filter::Criterion aCriterion = createCriterion();

    aCriterion.Type          = toFunctorType( aTypeStr );
    aCriterion.Compare       = toFunctorType( aCompareStr );
    aCriterion.UnaryOp       = toFunctorType( aUnaryStr );
    aCriterion.BinaryOp      = toFunctorType( aBinaryStr );
    
    aCriterion.TypeOfElement = toElementType( anElemTypeStr );

    LDOMString str = aCrit->getAttribute( ATTR_THRESHOLD );
    int val = 0;
    aCriterion.Threshold = str.Type() == LDOMBasicString::LDOM_Integer && str.GetInteger( val )
      ? val : atof( str.GetString() );

    str = aCrit->getAttribute( ATTR_TOLERANCE );
    aCriterion.Tolerance = str.Type() == LDOMBasicString::LDOM_Integer && str.GetInteger( val )
      ? val : atof( str.GetString() );

    str = aCrit->getAttribute( ATTR_THRESHOLD_STR );
    if ( str.Type() == LDOMBasicString::LDOM_Integer && str.GetInteger( val ) )
    {
      char a[ 255 ];
      sprintf( a, "%d", val );
      aCriterion.ThresholdStr = strdup( a );
    }
    else
      aCriterion.ThresholdStr = str.GetString();

    aCriteria.push_back( aCriterion );
  }

  SMESH::Filter::Criteria_var aCriteriaVar = new SMESH::Filter::Criteria;
  aCriteriaVar->length( aCriteria.size() );
  
  CORBA::ULong i = 0;
  std::list<SMESH::Filter::Criterion>::iterator anIter = aCriteria.begin();
  
  for( ; anIter != aCriteria.end(); ++anIter )
    aCriteriaVar[ i++ ] = *anIter;

  aRes = myFilterMgr->CreateFilter();
  aRes->SetCriteria( aCriteriaVar.inout() );

  return aRes;
}

//=======================================================================
// name    : FilterLibrary_i::SetFileName
// Purpose : Set file name for library
//=======================================================================
void FilterLibrary_i::SetFileName( const char* theFileName )
{
  delete myFileName;
  myFileName = strdup( theFileName );
}

//=======================================================================
// name    : FilterLibrary_i::GetFileName
// Purpose : Get file name of library
//=======================================================================
char* FilterLibrary_i::GetFileName()
{
  return CORBA::string_dup( myFileName );
}

//=======================================================================
// name    : FilterLibrary_i::Add
// Purpose : Add new filter to library
//=======================================================================
CORBA::Boolean FilterLibrary_i::Add( const char* theFilterName, Filter_ptr theFilter )
{
  // if filter already in library or entry filter is null do nothing
  LDOM_Node aFilterNode = findFilter( theFilterName, myDoc );
  if ( !aFilterNode.isNull() || theFilter->_is_nil() )
    return false;

  // get section corresponding to the filter type
  ElementType anEntType = theFilter->GetElementType();

  LDOM_Node aSection = getSection( anEntType, myDoc, true );
  if ( aSection.isNull() )
    return false;

  // create filter item
  LDOM_Element aFilterItem = createFilterItem( theFilterName, theFilter, myDoc );
  if ( aFilterItem.isNull() )
    return false;
  else
  {
    aSection.appendChild( aFilterItem );
    return true;
  }
}

//=======================================================================
// name    : FilterLibrary_i::Add
// Purpose : Add new filter to library
//=======================================================================
CORBA::Boolean FilterLibrary_i::AddEmpty( const char* theFilterName, ElementType theType )
{
  // if filter already in library or entry filter is null do nothing
  LDOM_Node aFilterNode = findFilter( theFilterName, myDoc );
  if ( !aFilterNode.isNull() )
    return false;

  LDOM_Node aSection = getSection( theType, myDoc, true );
  if ( aSection.isNull() )
    return false;

  // create filter item
  Filter_var aFilter = myFilterMgr->CreateFilter();
  
  LDOM_Element aFilterItem = createFilterItem( theFilterName, aFilter, myDoc );
  if ( aFilterItem.isNull() )
    return false;
  else
  {
    aSection.appendChild( aFilterItem );
    return true;
  }
}

//=======================================================================
// name    : FilterLibrary_i::Delete
// Purpose : Delete filter from library
//=======================================================================
CORBA::Boolean FilterLibrary_i::Delete ( const char* theFilterName )
{
  LDOM_Node aParentNode;
  LDOM_Node aFilterNode = findFilter( theFilterName, myDoc, &aParentNode );
  if ( aFilterNode.isNull() || aParentNode.isNull() )
    return false;

  aParentNode.removeChild( aFilterNode );
  return true;
}

//=======================================================================
// name      : FilterLibrary_i::Replace 
// Purpose   : Replace existing filter with entry filter.
// IMPORTANT : If filter does not exist it is not created
//=======================================================================
CORBA::Boolean FilterLibrary_i::Replace( const char* theFilterName,
                                         const char* theNewName,                 
                                         Filter_ptr  theFilter )
{
  LDOM_Element aFilterItem = findFilter( theFilterName, myDoc );
  if ( aFilterItem.isNull() || theFilter->_is_nil() )
    return false;

  LDOM_Element aNewItem = createFilterItem( theNewName, theFilter, myDoc );
  if ( aNewItem.isNull() )
    return false;
  else                                                                                          
  {
    aFilterItem.ReplaceElement( aNewItem );
    return true;
  }
}

//=======================================================================
// name    : FilterLibrary_i::Save
// Purpose : Save library on disk
//=======================================================================
CORBA::Boolean FilterLibrary_i::Save()
{
  if ( myFileName == 0 || strlen( myFileName ) == 0 )
    return false;
          
  FILE* aOutFile = fopen( myFileName, "wt" );
  if ( !aOutFile )
    return false;

  LDOM_XmlWriter aWriter( aOutFile );
  aWriter.SetIndentation( 2 );
  aWriter << myDoc;
  fclose( aOutFile );

  return true;
}

//=======================================================================
// name    : FilterLibrary_i::SaveAs
// Purpose : Save library on disk
//=======================================================================
CORBA::Boolean FilterLibrary_i::SaveAs( const char* aFileName )
{
  myFileName = strdup ( aFileName );
  return Save();
}

//=======================================================================
// name    : FilterLibrary_i::IsPresent
// Purpose : Verify whether filter is in library
//=======================================================================
CORBA::Boolean FilterLibrary_i::IsPresent( const char* theFilterName )
{
  return !findFilter( theFilterName, myDoc ).isNull();
}

//=======================================================================
// name    : FilterLibrary_i::NbFilters
// Purpose : Return amount of filters in library
//=======================================================================
CORBA::Long FilterLibrary_i::NbFilters( ElementType theType )
{
  string_array_var aNames = GetNames( theType );
  return aNames->length();
}

//=======================================================================
// name    : FilterLibrary_i::GetNames
// Purpose : Get names of filters from library
//=======================================================================
string_array* FilterLibrary_i::GetNames( ElementType theType )
{
  string_array_var anArray = new string_array;
  TColStd_SequenceOfHAsciiString aSeq;

  LDOM_Node aSection = getSection( theType, myDoc, false );

  if ( !aSection.isNull() )
  {
    for ( LDOM_Node aFilter = aSection.getFirstChild();
          !aFilter.isNull(); aFilter = aFilter.getNextSibling() )
    {
      LDOM_Element& anElem = ( LDOM_Element& )aFilter;
      aSeq.Append( new TCollection_HAsciiString(
         (Standard_CString)anElem.getAttribute( "name" ).GetString() ) );
    }
  }

  anArray->length( aSeq.Length() );
  for ( int i = 1, n = aSeq.Length(); i <= n; i++ )
    anArray[ i - 1 ] = CORBA::string_dup( aSeq( i )->ToCString() );

  return anArray._retn();
}

//=======================================================================
// name    : FilterLibrary_i::GetAllNames
// Purpose : Get names of filters from library
//=======================================================================
string_array* FilterLibrary_i::GetAllNames()
{
  string_array_var aResArray = new string_array;
  for ( int type = SMESH::ALL; type <= SMESH::VOLUME; type++ )
  {
    SMESH::string_array_var aNames = GetNames( (SMESH::ElementType)type );

    int aPrevLength = aResArray->length();
    aResArray->length( aPrevLength + aNames->length() );
    for ( int i = 0, n = aNames->length(); i < n; i++ )
      aResArray[ aPrevLength + i ] = aNames[ i ];
  }

  return aResArray._retn();
}