using namespace std;
// File generated by CPPExt (Transient)
//                     Copyright (C) 1991,1995 by
//  
//                      MATRA DATAVISION, FRANCE
//  
// This software is furnished in accordance with the terms and conditions
// of the contract and with the inclusion of the above copyright notice.
// This software or any other copy thereof may not be provided or otherwise
// be made available to any other person. No title to an ownership of the
// software is hereby transferred.
//  
// At the termination of the contract, the software and all copies of this
// software must be deleted.
//
#include "SMDS_HSequenceOfMesh.hxx"

#ifndef _Standard_TypeMismatch_HeaderFile
#include <Standard_TypeMismatch.hxx>
#endif

#ifndef _Standard_NoSuchObject_HeaderFile
#include <Standard_NoSuchObject.hxx>
#endif
#ifndef _Standard_OutOfRange_HeaderFile
#include <Standard_OutOfRange.hxx>
#endif
#ifndef _SMDS_Mesh_HeaderFile
#include "SMDS_Mesh.hxx"
#endif
#ifndef _SMDS_SequenceOfMesh_HeaderFile
#include "SMDS_SequenceOfMesh.hxx"
#endif
SMDS_HSequenceOfMesh::~SMDS_HSequenceOfMesh() {}
 


Standard_EXPORT Handle_Standard_Type& SMDS_HSequenceOfMesh_Type_()
{

    static Handle_Standard_Type aType1 = STANDARD_TYPE(MMgt_TShared);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(Standard_Transient);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("SMDS_HSequenceOfMesh",
			                                 sizeof(SMDS_HSequenceOfMesh),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}


// DownCast method
//   allow safe downcasting
//
const Handle(SMDS_HSequenceOfMesh) Handle(SMDS_HSequenceOfMesh)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(SMDS_HSequenceOfMesh) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(SMDS_HSequenceOfMesh))) {
       _anOtherObject = Handle(SMDS_HSequenceOfMesh)((Handle(SMDS_HSequenceOfMesh)&)AnObject);
     }
  }

  return _anOtherObject ;
}
const Handle(Standard_Type)& SMDS_HSequenceOfMesh::DynamicType() const 
{ 
  return STANDARD_TYPE(SMDS_HSequenceOfMesh) ; 
}
Standard_Boolean SMDS_HSequenceOfMesh::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(SMDS_HSequenceOfMesh) == AType || MMgt_TShared::IsKind(AType)); 
}
Handle_SMDS_HSequenceOfMesh::~Handle_SMDS_HSequenceOfMesh() {}
#define Item Handle_SMDS_Mesh
#define Item_hxx "SMDS_Mesh.hxx"
#define TheSequence SMDS_SequenceOfMesh
#define TheSequence_hxx "SMDS_SequenceOfMesh.hxx"
#define TCollection_HSequence SMDS_HSequenceOfMesh
#define TCollection_HSequence_hxx "SMDS_HSequenceOfMesh.hxx"
#define Handle_TCollection_HSequence Handle_SMDS_HSequenceOfMesh
#define TCollection_HSequence_Type_() SMDS_HSequenceOfMesh_Type_()
#include <TCollection_HSequence.gxx>
