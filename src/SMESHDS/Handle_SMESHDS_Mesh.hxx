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

#ifndef _Handle_SMESHDS_Mesh_HeaderFile
#define _Handle_SMESHDS_Mesh_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif

#ifndef _Handle_SMDS_Mesh_HeaderFile
#include "Handle_SMDS_Mesh.hxx"
#endif

class Standard_Transient;
class Handle_Standard_Type;
class Handle(SMDS_Mesh);
class SMESHDS_Mesh;
Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(SMESHDS_Mesh);

class Handle(SMESHDS_Mesh) : public Handle(SMDS_Mesh) {
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
    Handle(SMESHDS_Mesh)():Handle(SMDS_Mesh)() {} 
    Handle(SMESHDS_Mesh)(const Handle(SMESHDS_Mesh)& aHandle) : Handle(SMDS_Mesh)(aHandle) 
     {
     }

    Handle(SMESHDS_Mesh)(const SMESHDS_Mesh* anItem) : Handle(SMDS_Mesh)((SMDS_Mesh *)anItem) 
     {
     }

    Handle(SMESHDS_Mesh)& operator=(const Handle(SMESHDS_Mesh)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(SMESHDS_Mesh)& operator=(const SMESHDS_Mesh* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    SMESHDS_Mesh* operator->() 
     {
      return (SMESHDS_Mesh *)ControlAccess();
     }

    SMESHDS_Mesh* operator->() const 
     {
      return (SMESHDS_Mesh *)ControlAccess();
     }

   Standard_EXPORT ~Handle(SMESHDS_Mesh)();
 
   Standard_EXPORT static const Handle(SMESHDS_Mesh) DownCast(const Handle(Standard_Transient)& AnObject);
};
#endif