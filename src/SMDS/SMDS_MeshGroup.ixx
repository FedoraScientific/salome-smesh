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
#include "SMDS_MeshGroup.jxx"

#ifndef _Standard_TypeMismatch_HeaderFile
#include "Standard_TypeMismatch.hxx"
#endif

SMDS_MeshGroup::~SMDS_MeshGroup() {}
 


Standard_EXPORT Handle_Standard_Type& SMDS_MeshGroup_Type_()
{

    static Handle_Standard_Type aType1 = STANDARD_TYPE(SMDS_MeshObject);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(SMDS_MeshObject);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("SMDS_MeshGroup",
			                                 sizeof(SMDS_MeshGroup),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}


// DownCast method
//   allow safe downcasting
//
const Handle(SMDS_MeshGroup) Handle(SMDS_MeshGroup)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(SMDS_MeshGroup) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(SMDS_MeshGroup))) {
       _anOtherObject = Handle(SMDS_MeshGroup)((Handle(SMDS_MeshGroup)&)AnObject);
     }
  }

  return _anOtherObject ;
}
const Handle(Standard_Type)& SMDS_MeshGroup::DynamicType() const 
{ 
  return STANDARD_TYPE(SMDS_MeshGroup) ; 
}
Standard_Boolean SMDS_MeshGroup::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(SMDS_MeshGroup) == AType || SMDS_MeshObject::IsKind(AType)); 
}
Handle_SMDS_MeshGroup::~Handle_SMDS_MeshGroup() {}
