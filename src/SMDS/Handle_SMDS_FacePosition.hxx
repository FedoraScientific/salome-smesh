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

#ifndef _Handle_SMDS_FacePosition_HeaderFile
#define _Handle_SMDS_FacePosition_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif

#ifndef _Handle_SMDS_Position_HeaderFile
#include "Handle_SMDS_Position.hxx"
#endif

class Standard_Transient;
class Handle_Standard_Type;
class Handle(SMDS_Position);
class SMDS_FacePosition;
Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(SMDS_FacePosition);

class Handle(SMDS_FacePosition) : public Handle(SMDS_Position) {
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
    Handle(SMDS_FacePosition)():Handle(SMDS_Position)() {} 
    Handle(SMDS_FacePosition)(const Handle(SMDS_FacePosition)& aHandle) : Handle(SMDS_Position)(aHandle) 
     {
     }

    Handle(SMDS_FacePosition)(const SMDS_FacePosition* anItem) : Handle(SMDS_Position)((SMDS_Position *)anItem) 
     {
     }

    Handle(SMDS_FacePosition)& operator=(const Handle(SMDS_FacePosition)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(SMDS_FacePosition)& operator=(const SMDS_FacePosition* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    SMDS_FacePosition* operator->() 
     {
      return (SMDS_FacePosition *)ControlAccess();
     }

    SMDS_FacePosition* operator->() const 
     {
      return (SMDS_FacePosition *)ControlAccess();
     }

   Standard_EXPORT ~Handle(SMDS_FacePosition)();
 
   Standard_EXPORT static const Handle(SMDS_FacePosition) DownCast(const Handle(Standard_Transient)& AnObject);
};
#endif