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

#ifndef _Handle_SMESHDS_ListNodeOfListOfCommand_HeaderFile
#define _Handle_SMESHDS_ListNodeOfListOfCommand_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif

#ifndef _Handle_TCollection_MapNode_HeaderFile
#include <Handle_TCollection_MapNode.hxx>
#endif

class Standard_Transient;
class Handle_Standard_Type;
class Handle(TCollection_MapNode);
class SMESHDS_ListNodeOfListOfCommand;
Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(SMESHDS_ListNodeOfListOfCommand);

class Handle(SMESHDS_ListNodeOfListOfCommand) : public Handle(TCollection_MapNode) {
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
    Handle(SMESHDS_ListNodeOfListOfCommand)():Handle(TCollection_MapNode)() {} 
    Handle(SMESHDS_ListNodeOfListOfCommand)(const Handle(SMESHDS_ListNodeOfListOfCommand)& aHandle) : Handle(TCollection_MapNode)(aHandle) 
     {
     }

    Handle(SMESHDS_ListNodeOfListOfCommand)(const SMESHDS_ListNodeOfListOfCommand* anItem) : Handle(TCollection_MapNode)((TCollection_MapNode *)anItem) 
     {
     }

    Handle(SMESHDS_ListNodeOfListOfCommand)& operator=(const Handle(SMESHDS_ListNodeOfListOfCommand)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(SMESHDS_ListNodeOfListOfCommand)& operator=(const SMESHDS_ListNodeOfListOfCommand* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    SMESHDS_ListNodeOfListOfCommand* operator->() 
     {
      return (SMESHDS_ListNodeOfListOfCommand *)ControlAccess();
     }

    SMESHDS_ListNodeOfListOfCommand* operator->() const 
     {
      return (SMESHDS_ListNodeOfListOfCommand *)ControlAccess();
     }

   Standard_EXPORT ~Handle(SMESHDS_ListNodeOfListOfCommand)();
 
   Standard_EXPORT static const Handle(SMESHDS_ListNodeOfListOfCommand) DownCast(const Handle(Standard_Transient)& AnObject);
};
#endif