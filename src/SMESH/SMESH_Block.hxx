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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com

// File      : SMESH_Block.hxx
// Created   : Tue Nov 30 12:42:18 2004
// Author    : Edward AGAPOV (eap)


#ifndef SMESH_Block_HeaderFile
#define SMESH_Block_HeaderFile

#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedMapOfOrientedShape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>

#include <ostream>
#include <vector>

class SMDS_MeshVolume;
class SMDS_MeshNode;

// =========================================================
// class calculating coordinates of 3D points by normalized
// parameters inside the block and vice versa
// =========================================================

class SMESH_Block: public math_FunctionSetWithDerivatives
{
 public:
  enum TShapeID { // ids of the block sub-shapes
    ID_NONE = 0,

    ID_V000 = 1, ID_V100, ID_V010, ID_V110, ID_V001, ID_V101, ID_V011, ID_V111,

    ID_Ex00, ID_Ex10, ID_Ex01, ID_Ex11,
    ID_E0y0, ID_E1y0, ID_E0y1, ID_E1y1,
    ID_E00z, ID_E10z, ID_E01z, ID_E11z,

    ID_Fxy0, ID_Fxy1, ID_Fx0z, ID_Fx1z, ID_F0yz, ID_F1yz,

    ID_Shell
    };


 public: // methods about ids of the block sub-shapes

  static int NbVertices()  { return  8; }
  static int NbEdges()     { return 12; }
  static int NbFaces()     { return  6; }
  static int NbSubShapes() { return ID_Shell; }
  // to avoid magic numbers when allocating memory for subshapes

  static inline bool IsVertexID( int theShapeID )
  { return ( theShapeID >= ID_V000 && theShapeID <= ID_V111 ); }

  static inline bool IsEdgeID( int theShapeID )
  { return ( theShapeID >= ID_Ex00 && theShapeID <= ID_E11z ); }

  static inline bool IsFaceID( int theShapeID )
  { return ( theShapeID >= ID_Fxy0 && theShapeID <= ID_F1yz ); }

  static int ShapeIndex( int theShapeID )
  {
    if ( IsVertexID( theShapeID )) return theShapeID - ID_V000;
    if ( IsEdgeID( theShapeID ))   return theShapeID - ID_Ex00;
    if ( IsFaceID( theShapeID ))   return theShapeID - ID_Fxy0;
    return 0;
  }
  // return index [0-...] for each type of sub-shapes,
  // for example :
  // ShapeIndex( ID_Ex00 ) == 0
  // ShapeIndex( ID_Ex10 ) == 1

  static void GetFaceEdgesIDs (const int faceID, vector< int >& edgeVec );
  // return edges IDs of a face in the order u0, u1, 0v, 1v

  static void GetEdgeVertexIDs (const int edgeID, vector< int >& vertexVec );
  // return vertex IDs of an edge

  static int GetCoordIndOnEdge (const int theEdgeID)
  { return (theEdgeID < ID_E0y0) ? 1 : (theEdgeID < ID_E00z) ? 2 : 3; }
  // return an index of a coordinate which varies along the edge

  static double* GetShapeCoef (const int theShapeID);
  // for theShapeID( TShapeID ), returns 3 coefficients used
  // to compute an addition of an on-theShape point to coordinates
  // of an in-shell point. If an in-shell point has parameters (Px,Py,Pz),
  // then the addition of a point P is computed as P*kx*ky*kz and ki is
  // defined by the returned coef like this:
  // ki = (coef[i] == 0) ? 1 : (coef[i] < 0) ? 1 - Pi : Pi

  static int GetShapeIDByParams ( const gp_XYZ& theParams );
  // define an id of the block sub-shape by point parameters

  static ostream& DumpShapeID (const int theBlockShapeID, ostream& stream);
  // DEBUG: dump an id of a block sub-shape


 public: // initialization

  SMESH_Block (): myNbIterations(0), mySumDist(0.) {}

  bool LoadBlockShapes(const TopoDS_Shell&         theShell,
                       const TopoDS_Vertex&        theVertex000,
                       const TopoDS_Vertex&        theVertex001,
                       TopTools_IndexedMapOfOrientedShape& theShapeIDMap );
  // add sub-shapes of theBlock to theShapeIDMap so that they get
  // IDs acoording to enum TShapeID

  bool LoadMeshBlock(const SMDS_MeshVolume*        theVolume,
                     const int                     theNode000Index,
                     const int                     theNode001Index,
                     vector<const SMDS_MeshNode*>& theOrderedNodes);
  // prepare to work with theVolume and
  // return nodes in the order of TShapeID enum


 public: // define coordinates by parameters

  bool VertexPoint( const int theVertexID, gp_XYZ& thePoint ) const {
    if ( !IsVertexID( theVertexID ))           return false;
    thePoint = myPnt[ theVertexID - ID_V000 ]; return true;
  }
  // return vertex coordinates, parameters are defined by theVertexID

  bool EdgePoint( const int theEdgeID, const gp_XYZ& theParams, gp_XYZ& thePoint ) const {
    if ( !IsEdgeID( theEdgeID ))                                 return false;
    thePoint = myEdge[ theEdgeID - ID_Ex00 ].Point( theParams ); return true;
  }
  // return coordinates of a point on edge

  bool FacePoint( const int theFaceID, const gp_XYZ& theParams, gp_XYZ& thePoint ) const {
    if ( !IsFaceID ( theFaceID ))                                return false;
    thePoint = myFace[ theFaceID - ID_Fxy0 ].Point( theParams ); return true;
  }
  // return coordinates of a point on face

  bool ShellPoint( const gp_XYZ& theParams, gp_XYZ& thePoint ) const;
  // return coordinates of a point in shell

  static bool ShellPoint(const gp_XYZ&         theParams,
                         const vector<gp_XYZ>& thePointOnShape,
                         gp_XYZ&               thePoint );
  // computes coordinates of a point in shell by points on sub-shapes;
  // thePointOnShape[ subShapeID ] must be a point on a subShape;
  // thePointOnShape.size() == ID_Shell, thePointOnShape[0] not used


 public: // define parameters by coordinates

  bool ComputeParameters (const gp_Pnt& thePoint,
                          gp_XYZ&       theParams,
                          const int     theShapeID = ID_Shell);
  // compute point parameters in the block.
  // Note: for edges, it is better to use EdgeParameters()

  bool VertexParameters(const int theVertexID, gp_XYZ& theParams);
  // return parameters of a vertex given by TShapeID

  bool EdgeParameters(const int theEdgeID, const double theU, gp_XYZ& theParams);
  // return parameters of a point given by theU on edge


 public: // services

  static bool IsForwardEdge (const TopoDS_Edge &                 theEdge,
                             TopTools_IndexedMapOfOrientedShape& theShapeIDMap) {
    int v1ID = theShapeIDMap.FindIndex( TopExp::FirstVertex( theEdge ).Oriented( TopAbs_FORWARD ));
    int v2ID = theShapeIDMap.FindIndex( TopExp::LastVertex( theEdge ).Oriented( TopAbs_FORWARD ));
    return ( v1ID < v2ID );
  }
  // Return true if an in-block parameter increases along theEdge curve

  static void Swap(double& a, double& b) { double tmp = a; a = b; b = tmp; }


 public:
  // methods of math_FunctionSetWithDerivatives used internally
  // to define parameters by coordinates
  Standard_Integer NbVariables() const;
  Standard_Integer NbEquations() const;
  Standard_Boolean Value(const math_Vector& X,math_Vector& F) ;
  Standard_Boolean Derivatives(const math_Vector& X,math_Matrix& D) ;
  Standard_Boolean Values(const math_Vector& X,math_Vector& F,math_Matrix& D) ;
  Standard_Integer GetStateNumber ();

 private:

  struct TEdge {
    int                myCoordInd;
    double             myFirst;
    double             myLast;
    Handle(Geom_Curve) myC3d;
    gp_Trsf            myTrsf;
    double GetU( const gp_XYZ& theParams ) const;
    gp_XYZ Point( const gp_XYZ& theParams ) const;
    // if mesh volume
    gp_XYZ             myNodes[2];
  };

  struct TFace {
    // 4 edges in the order u0, u1, 0v, 1v
    int                  myCoordInd[ 4 ];
    double               myFirst   [ 4 ];
    double               myLast    [ 4 ];
    Handle(Geom2d_Curve) myC2d     [ 4 ];
    // 4 corner points in the order 00, 10, 11, 01
    gp_XY                myCorner  [ 4 ];
    // surface
    Handle(Geom_Surface) myS;
    gp_Trsf              myTrsf;
    gp_XY  GetUV( const gp_XYZ& theParams ) const;
    gp_XYZ Point( const gp_XYZ& theParams ) const;
    int GetUInd() const { return myCoordInd[ 0 ]; }
    int GetVInd() const { return myCoordInd[ 2 ]; }
    void GetCoefs( int i, const gp_XYZ& theParams, double& eCoef, double& vCoef ) const;
    // if mesh volume
    gp_XYZ               myNodes[4];
  };

  TopoDS_Shell myShell;
  // geometry in the order as in TShapeID:
  // 8 vertices
  gp_XYZ myPnt[ 8 ];
  // 12 edges
  TEdge  myEdge[ 12 ];
  // 6 faces
  TFace  myFace[ 6 ];

  // for param computation

  int      myFaceIndex;
  double   myFaceParam;
  int      myNbIterations;
  double   mySumDist;

  gp_XYZ   myPoint; // the given point
  gp_XYZ   myParam; // the best parameters guess
  double   myValues[ 4 ]; // values computed at myParam

  typedef pair<gp_XYZ,gp_XYZ> TxyzPair;
  TxyzPair my3x3x3GridNodes[ 27 ];
  bool     myGridComputed;
};


#endif
