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
//
#ifndef _SMDS_SpacePosition_HeaderFile
#define _SMDS_SpacePosition_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Handle_SMDS_SpacePosition_HeaderFile
#include "Handle_SMDS_SpacePosition.hxx"
#endif

#ifndef _gp_Pnt_HeaderFile
#include <gp_Pnt.hxx>
#endif
#ifndef _SMDS_Position_HeaderFile
#include "SMDS_Position.hxx"
#endif
#ifndef _Standard_Real_HeaderFile
#include <Standard_Real.hxx>
#endif
class gp_Pnt;


class SMDS_SpacePosition : public SMDS_Position {

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
 // Methods PUBLIC
 // 
Standard_EXPORT SMDS_SpacePosition();
Standard_EXPORT SMDS_SpacePosition(const Standard_Real x,const Standard_Real y,const Standard_Real z);
Standard_EXPORT SMDS_SpacePosition(const gp_Pnt& aCoords);
Standard_EXPORT virtual  gp_Pnt Coords() const;
Standard_EXPORT inline   void SetCoords(const Standard_Real x,const Standard_Real y,const Standard_Real z) ;
Standard_EXPORT inline   void SetCoords(const gp_Pnt& aCoords) ;
Standard_EXPORT ~SMDS_SpacePosition();




 // Type management
 //
 Standard_EXPORT friend Handle_Standard_Type& SMDS_SpacePosition_Type_();
 Standard_EXPORT const Handle(Standard_Type)& DynamicType() const;
 Standard_EXPORT Standard_Boolean	       IsKind(const Handle(Standard_Type)&) const;

protected:

 // Methods PROTECTED
 // 


 // Fields PROTECTED
 //


private: 

 // Methods PRIVATE
 // 


 // Fields PRIVATE
 //
gp_Pnt myCoords;


};


#include "SMDS_SpacePosition.lxx"



// other inline functions and methods (like "C++: function call" methods)
//


#endif