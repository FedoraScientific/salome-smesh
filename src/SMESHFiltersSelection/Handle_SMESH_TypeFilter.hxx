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

#ifndef _Handle_SMESH_TypeFilter_HeaderFile
#define _Handle_SMESH_TypeFilter_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif

#ifndef _Handle_SALOME_Filter_HeaderFile
#include "Handle_SALOME_Filter.hxx"
#endif

class Standard_Transient;
class Handle_Standard_Type;
class Handle(SALOME_Filter);
class SMESH_TypeFilter;
Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(SMESH_TypeFilter);

class Handle(SMESH_TypeFilter) : public Handle(SALOME_Filter) {
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
    Handle(SMESH_TypeFilter)():Handle(SALOME_Filter)() {} 
    Handle(SMESH_TypeFilter)(const Handle(SMESH_TypeFilter)& aHandle) : Handle(SALOME_Filter)(aHandle) 
     {
     }

    Handle(SMESH_TypeFilter)(const SMESH_TypeFilter* anItem) : Handle(SALOME_Filter)((SALOME_Filter *)anItem) 
     {
     }

    Handle(SMESH_TypeFilter)& operator=(const Handle(SMESH_TypeFilter)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(SMESH_TypeFilter)& operator=(const SMESH_TypeFilter* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    SMESH_TypeFilter* operator->() 
     {
      return (SMESH_TypeFilter *)ControlAccess();
     }

    SMESH_TypeFilter* operator->() const 
     {
      return (SMESH_TypeFilter *)ControlAccess();
     }

   Standard_EXPORT ~Handle(SMESH_TypeFilter)();
 
   Standard_EXPORT static const Handle(SMESH_TypeFilter) DownCast(const Handle(Standard_Transient)& AnObject);
};
#endif