//  SMESH SMESH : implementaion of SMESH idl descriptions
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
//  File   : SMESH_HypoFilter.hxx
//  Module : SMESH
//  $Header$


#ifndef SMESH_HypoFilter_HeaderFile
#define SMESH_HypoFilter_HeaderFile

// ===========================
// Filter of SMESH_Hypothesis
// ===========================

#include <list>
#include <string>

class SMESH_HypoFilter;
class SMESH_Hypothesis;
class TopoDS_Shape;

class SMESH_HypoPredicate {
 public:
  virtual bool IsOk(const SMESH_Hypothesis* aHyp,
                    const TopoDS_Shape&     aShape) const = 0;
  // check aHyp or/and aShape it is assigned to
  virtual ~SMESH_HypoPredicate() {}
 private:
  int _logical_op;
  friend class SMESH_HypoFilter;
};

class SMESH_HypoFilter: public SMESH_HypoPredicate
{
 public:
  // Create and add predicates.
  // Added predicates will be destroyed by filter when it dies
  SMESH_HypoFilter();
  SMESH_HypoFilter( SMESH_HypoPredicate* aPredicate, bool notNagate = true );
  // notNagate==false means !aPredicate->IsOk()
  void Init  ( SMESH_HypoPredicate* aPredicate, bool notNagate = true );
  void And   ( SMESH_HypoPredicate* aPredicate );
  void AndNot( SMESH_HypoPredicate* aPredicate );
  void Or    ( SMESH_HypoPredicate* aPredicate );
  void OrNot ( SMESH_HypoPredicate* aPredicate );

  // Create predicates
  static SMESH_HypoPredicate* IsAlgo();
  static SMESH_HypoPredicate* IsApplicableTo(const TopoDS_Shape& theMainShape);
  static SMESH_HypoPredicate* Is(const SMESH_Hypothesis* theHypo);
  static SMESH_HypoPredicate* IsGlobal(const TopoDS_Shape& theMainShape);
  static SMESH_HypoPredicate* HasName(const std::string & theName);
  static SMESH_HypoPredicate* HasDim(const int theDim);
  static SMESH_HypoPredicate* HasType(const int theHypType);

  bool IsOk (const SMESH_Hypothesis* aHyp,
             const TopoDS_Shape&     aShape) const;
  // check aHyp or/and aShape it is assigned to

  ~SMESH_HypoFilter();


 protected:
  // fields

  std::list<SMESH_HypoPredicate*> myPredicates;

  // private methods

  enum Logical { AND, AND_NOT, OR, OR_NOT };
  enum Comparison { EQUAL, NOT_EQUAL, MORE, LESS };

  SMESH_HypoFilter(const SMESH_HypoFilter& other){}

  void add( Logical bool_op, SMESH_HypoPredicate* pred )
  {
    if ( pred ) {
      pred->_logical_op = bool_op;
      myPredicates.push_back( pred );
    }
  }

  // predicates implementation

  template <typename TValue>
    struct templPredicate: public SMESH_HypoPredicate {
      Comparison _comp;
      TValue     _val;
      virtual TValue Value(const SMESH_Hypothesis* aHyp) const = 0;
      virtual bool IsOk(const SMESH_Hypothesis* aHyp, const TopoDS_Shape& ) const
      {
        if      ( _comp == EQUAL )     return _val == Value( aHyp );
        else if ( _comp == NOT_EQUAL ) return _val != Value( aHyp );
        else if ( _comp == MORE )      return _val < Value( aHyp );
        else                           return _val > Value( aHyp );
      }
    };

  struct NamePredicate : public SMESH_HypoPredicate {
    std::string _name;
    NamePredicate( std::string name ): _name(name){}
    bool IsOk(const SMESH_Hypothesis* aHyp,
              const TopoDS_Shape&     aShape) const;
  };
  
  struct TypePredicate : public templPredicate< int > {
    TypePredicate( Comparison comp, int hypType )
    { _comp = comp; _val = hypType; }
    int Value( const SMESH_Hypothesis* aHyp ) const;
  };
  
  struct DimPredicate : public templPredicate< int > {
    DimPredicate( Comparison comp, int dim )
    { _comp = comp; _val = dim; }
    int Value( const SMESH_Hypothesis* aHyp ) const;
  };
  
  struct ApplicablePredicate : public SMESH_HypoPredicate {
    int _shapeType;
    ApplicablePredicate( const TopoDS_Shape& theShape );
    bool IsOk(const SMESH_Hypothesis* aHyp,
              const TopoDS_Shape&     aShape) const;
  };
        
  struct InstancePredicate : public SMESH_HypoPredicate {
    const SMESH_Hypothesis* _hypo;
    InstancePredicate( const SMESH_Hypothesis* hypo ):_hypo(hypo){}
    bool IsOk(const SMESH_Hypothesis* aHyp,
              const TopoDS_Shape&     aShape) const;
  };
        
  struct IsGlobalPredicate : public SMESH_HypoPredicate {
    const TopoDS_Shape& _mainShape;
    IsGlobalPredicate( const TopoDS_Shape& mainShape ):_mainShape(mainShape){}
    bool IsOk(const SMESH_Hypothesis* aHyp,
              const TopoDS_Shape&     aShape) const;
  };
        
};


#endif