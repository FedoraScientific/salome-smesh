// File generated by CPPExt (Transient)
//
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

#ifndef _Handle_SMDS_MeshEdge_HeaderFile
#define _Handle_SMDS_MeshEdge_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif

#ifndef _Handle_SMDS_MeshElement_HeaderFile
#include "Handle_SMDS_MeshElement.hxx"
#endif

class Standard_Transient;
class Handle_Standard_Type;
class Handle(SMDS_MeshElement);
class SMDS_MeshEdge;
Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(SMDS_MeshEdge);

class Handle(SMDS_MeshEdge) : public Handle(SMDS_MeshElement) {
  public:
    inline void* operator new(size_t,void* anAddress) 
      {
        return anAddress;
      }
    inline void* operator new(size_t size) 
      { 
        return Standard::Allocate(size); 
      }
    inline void  operator delete(void *anAddress) 
      { 
        if (anAddress) Standard::Free((Standard_Address&)anAddress); 
      }
//    inline void  operator delete(void *anAddress, size_t size) 
//      { 
//        if (anAddress) Standard::Free((Standard_Address&)anAddress,size); 
//      }
    Handle(SMDS_MeshEdge)():Handle(SMDS_MeshElement)() {} 
    Handle(SMDS_MeshEdge)(const Handle(SMDS_MeshEdge)& aHandle) : Handle(SMDS_MeshElement)(aHandle) 
     {
     }

    Handle(SMDS_MeshEdge)(const SMDS_MeshEdge* anItem) : Handle(SMDS_MeshElement)((SMDS_MeshElement *)anItem) 
     {
     }

    Handle(SMDS_MeshEdge)& operator=(const Handle(SMDS_MeshEdge)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(SMDS_MeshEdge)& operator=(const SMDS_MeshEdge* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    SMDS_MeshEdge* operator->() 
     {
      return (SMDS_MeshEdge *)ControlAccess();
     }

    SMDS_MeshEdge* operator->() const 
     {
      return (SMDS_MeshEdge *)ControlAccess();
     }

   Standard_EXPORT ~Handle(SMDS_MeshEdge)();
 
   Standard_EXPORT static const Handle(SMDS_MeshEdge) DownCast(const Handle(Standard_Transient)& AnObject);
};
#endif