// File      : SMDS_VolumeTool.cxx
// Created   : Tue Jul 13 12:22:13 2004
// Author    : Edward AGAPOV (eap)
// Copyright : Open CASCADE

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "SMDS_VolumeTool.hxx"

#include "SMDS_MeshElement.hxx"
#include "SMDS_MeshNode.hxx"
#include "SMDS_PolyhedralVolumeOfNodes.hxx"

#include "utilities.h"

#include <map>
#include <float.h>
#include <math.h>

using namespace std;

// ======================================================
// Node indices in faces depending on volume orientation
// making most faces normals external
// ======================================================

/*
//           N3
//           +
//          /|\
//         / | \
//        /  |  \
//    N0 +---|---+ N1                TETRAHEDRON
//       \   |   /
//        \  |  /
//         \ | /
//          \|/
//           +
//           N2
*/
static int Tetra_F [4][4] = { // FORWARD == EXTERNAL
  { 0, 1, 2, 0 },              // All faces have external normals
  { 0, 3, 1, 0 },
  { 1, 3, 2, 1 },
  { 0, 2, 3, 0 }}; 
static int Tetra_R [4][4] = { // REVERSED
  { 0, 1, 2, 0 },             // All faces but a bottom have external normals
  { 0, 1, 3, 0 },
  { 1, 2, 3, 1 },
  { 0, 3, 2, 0 }};
static int Tetra_RE [4][4] = { // REVERSED -> FORWARD (EXTERNAL)
  { 0, 2, 1, 0 },              // All faces have external normals
  { 0, 1, 3, 0 },
  { 1, 2, 3, 1 },
  { 0, 3, 2, 0 }};
static int Tetra_nbN [] = { 3, 3, 3, 3 };

//
//     PYRAMID
//
static int Pyramid_F [5][5] = { // FORWARD == EXTERNAL
  { 0, 1, 2, 3, 0 },            // All faces have external normals
  { 0, 4, 1, 0, 4 },
  { 1, 4, 2, 1, 4 },
  { 2, 4, 3, 2, 4 },
  { 3, 4, 0, 3, 4 }}; 
static int Pyramid_R [5][5] = { // REVERSED
  { 0, 1, 2, 3, 0 },            // All faces but a bottom have external normals
  { 0, 1, 4, 0, 4 },
  { 1, 2, 4, 1, 4 },
  { 2, 3, 4, 2, 4 },
  { 3, 0, 4, 3, 4 }}; 
static int Pyramid_RE [5][5] = { // REVERSED -> FORWARD (EXTERNAL)
  { 0, 3, 2, 1, 0 },             // All faces but a bottom have external normals
  { 0, 1, 4, 0, 4 },
  { 1, 2, 4, 1, 4 },
  { 2, 3, 4, 2, 4 },
  { 3, 0, 4, 3, 4 }}; 
static int Pyramid_nbN [] = { 4, 3, 3, 3, 3 };

/*   
//            + N4
//           /|\
//          / | \
//         /  |  \
//        /   |   \
//    N3 +---------+ N5
//       |    |    |
//       |    + N1 |
//       |   / \   |                PENTAHEDRON
//       |  /   \  |
//       | /     \ |
//       |/       \|
//    N0 +---------+ N2
*/
static int Penta_F [5][5] = { // FORWARD
  { 0, 1, 2, 0, 0 },          // Top face has an internal normal, other - external
  { 3, 4, 5, 3, 3 },          // 0 is bottom, 1 is top face
  { 0, 2, 5, 3, 0 },
  { 1, 4, 5, 2, 1 },
  { 0, 3, 4, 1, 0 }}; 
static int Penta_R [5][5] = { // REVERSED
  { 0, 1, 2, 0, 0 },          // Bottom face has an internal normal, other - external
  { 3, 4, 5, 3, 3 },          // 0 is bottom, 1 is top face
  { 0, 3, 5, 2, 0 },
  { 1, 2, 5, 4, 1 },
  { 0, 1, 4, 3, 0 }}; 
static int Penta_FE [5][5] = { // FORWARD -> EXTERNAL
  { 0, 1, 2, 0, 0 },
  { 3, 5, 4, 3, 3 },
  { 0, 2, 5, 3, 0 },
  { 1, 4, 5, 2, 1 },
  { 0, 3, 4, 1, 0 }}; 
static int Penta_RE [5][5] = { // REVERSED -> EXTERNAL
  { 0, 2, 1, 0, 0 },
  { 3, 4, 5, 3, 3 },
  { 0, 3, 5, 2, 0 },
  { 1, 2, 5, 4, 1 },
  { 0, 1, 4, 3, 0 }}; 
static int Penta_nbN [] = { 3, 3, 4, 4, 4 };

/*
//         N5+----------+N6
//          /|         /|
//         / |        / |
//        /  |       /  |
//     N4+----------+N7 |
//       |   |      |   |           HEXAHEDRON
//       |   |      |   |
//       |   |      |   |
//       | N1+------|---+N2
//       |  /       |  /
//       | /        | /
//       |/         |/
//     N0+----------+N3
*/
static int Hexa_F [6][5] = { // FORWARD
  { 0, 1, 2, 3, 0 },         // opposite faces are neighbouring,
  { 4, 5, 6, 7, 4 },         // odd face(1,3,5) normal is internal, even(0,2,4) - external
  { 1, 0, 4, 5, 1 },         // same index nodes of opposite faces are linked
  { 2, 3, 7, 6, 2 }, 
  { 0, 3, 7, 4, 0 }, 
  { 1, 2, 6, 5, 1 }};
// static int Hexa_R [6][5] = { // REVERSED
//   { 0, 3, 2, 1, 0 },         // opposite faces are neighbouring,
//   { 4, 7, 6, 5, 4 },         // odd face(1,3,5) normal is external, even(0,2,4) - internal
//   { 1, 5, 4, 0, 1 },         // same index nodes of opposite faces are linked
//   { 2, 6, 7, 3, 2 }, 
//   { 0, 4, 7, 3, 0 }, 
//   { 1, 5, 6, 2, 1 }};
static int Hexa_FE [6][5] = { // FORWARD -> EXTERNAL
  { 0, 1, 2, 3, 0 } ,         // opposite faces are neighbouring,
  { 4, 7, 6, 5, 4 },          // all face normals are external,
  { 0, 4, 5, 1, 0 },          // links in opposite faces: 0-0, 1-3, 2-2, 3-1
  { 3, 2, 6, 7, 3 }, 
  { 0, 3, 7, 4, 0 },
  { 1, 5, 6, 2, 1 }};
static int Hexa_RE [6][5] = { // REVERSED -> EXTERNAL
  { 0, 3, 2, 1, 0 },          // opposite faces are neighbouring,
  { 4, 5, 6, 7, 4 },          // all face normals are external,
  { 0, 1, 5, 4, 0 },          // links in opposite faces: 0-0, 1-3, 2-2, 3-1
  { 3, 7, 6, 2, 3 }, 
  { 0, 4, 7, 3, 0 },
  { 1, 2, 6, 5, 1 }};
static int Hexa_nbN [] = { 4, 4, 4, 4, 4, 4 };

// ========================================================
// to perform some calculations without linkage to CASCADE
// ========================================================
struct XYZ {
  double x;
  double y;
  double z;
  XYZ()                               { x = 0; y = 0; z = 0; }
  XYZ( double X, double Y, double Z ) { x = X; y = Y; z = Z; }
  XYZ( const XYZ& other )             { x = other.x; y = other.y; z = other.z; }
  XYZ( const SMDS_MeshNode* n )       { x = n->X(); y = n->Y(); z = n->Z(); }
  XYZ operator-( const XYZ& other );
  XYZ Crossed( const XYZ& other );
  double Dot( const XYZ& other );
  double Magnitude();
};
XYZ XYZ::operator-( const XYZ& Right ) {
  return XYZ(x - Right.x, y - Right.y, z - Right.z);
}
XYZ XYZ::Crossed( const XYZ& Right ) {
  return XYZ (y * Right.z - z * Right.y,
              z * Right.x - x * Right.z,
              x * Right.y - y * Right.x);
}
double XYZ::Dot( const XYZ& Other ) {
  return(x * Other.x + y * Other.y + z * Other.z);
}
double XYZ::Magnitude() {
  return sqrt (x * x + y * y + z * z);
}

//=======================================================================
//function : SMDS_VolumeTool
//purpose  : 
//=======================================================================

SMDS_VolumeTool::SMDS_VolumeTool ()
     : myVolume( 0 ),
       myPolyedre( 0 ),
       myVolForward( true ),
       myNbFaces( 0 ),
       myVolumeNbNodes( 0 ),
       myVolumeNodes( NULL ),
       myExternalFaces( false ),
       myFaceNbNodes( 0 ),
       myCurFace( -1 ),
       myFaceNodeIndices( NULL ),
       myFaceNodes( NULL )
{
}

//=======================================================================
//function : SMDS_VolumeTool
//purpose  : 
//=======================================================================

SMDS_VolumeTool::SMDS_VolumeTool (const SMDS_MeshElement* theVolume)
     : myVolume( 0 ),
       myPolyedre( 0 ),
       myVolForward( true ),
       myNbFaces( 0 ),
       myVolumeNbNodes( 0 ),
       myVolumeNodes( NULL ),
       myExternalFaces( false ),
       myFaceNbNodes( 0 ),
       myCurFace( -1 ),
       myFaceNodeIndices( NULL ),
       myFaceNodes( NULL )
{
  Set( theVolume );
}

//=======================================================================
//function : SMDS_VolumeTool
//purpose  : 
//=======================================================================

SMDS_VolumeTool::~SMDS_VolumeTool()
{
  if (myVolumeNodes != NULL) {
    delete [] myVolumeNodes;
    myVolumeNodes = NULL;
  }
  if (myFaceNodes != NULL) {
    delete [] myFaceNodes;
    myFaceNodes = NULL;
  }
}

//=======================================================================
//function : SetVolume
//purpose  : Set volume to iterate on
//=======================================================================

bool SMDS_VolumeTool::Set (const SMDS_MeshElement* theVolume)
{
  myVolume = 0;
  myPolyedre = 0;

  myVolForward = true;
  myNbFaces = 0;
  myVolumeNbNodes = 0;
  if (myVolumeNodes != NULL) {
    delete [] myVolumeNodes;
    myVolumeNodes = NULL;
  }

  myExternalFaces = false;
  myFaceNbNodes = 0;

  myCurFace = -1;
  myFaceNodeIndices = NULL;
  if (myFaceNodes != NULL) {
    delete [] myFaceNodes;
    myFaceNodes = NULL;
  }

  if ( theVolume && theVolume->GetType() == SMDSAbs_Volume )
  {
    myVolume = theVolume;

    myNbFaces = theVolume->NbFaces();
    myVolumeNbNodes = theVolume->NbNodes();

    // set volume nodes
    int iNode = 0;
    myVolumeNodes = new const SMDS_MeshNode* [myVolumeNbNodes];
    SMDS_ElemIteratorPtr nodeIt = myVolume->nodesIterator();
    while ( nodeIt->more() ) {
      myVolumeNodes[ iNode++ ] = static_cast<const SMDS_MeshNode*>( nodeIt->next() );
    }

    if (myVolume->IsPoly()) {
      myPolyedre = static_cast<const SMDS_PolyhedralVolumeOfNodes*>( myVolume );
      if (!myPolyedre) {
        MESSAGE("Warning: bad volumic element");
        return false;
      }
    } else {
      switch ( myVolumeNbNodes ) {
      case 4:
      case 5:
      case 6:
      case 8: {
        // define volume orientation
        XYZ botNormal;
        GetFaceNormal( 0, botNormal.x, botNormal.y, botNormal.z );
        const SMDS_MeshNode* topNode = myVolumeNodes[ myVolumeNbNodes - 1 ];
        const SMDS_MeshNode* botNode = myVolumeNodes[ 0 ];
        XYZ upDir (topNode->X() - botNode->X(),
                   topNode->Y() - botNode->Y(),
                   topNode->Z() - botNode->Z() );
        myVolForward = ( botNormal.Dot( upDir ) < 0 );
        break;
      }
      default:
        break;
      }
    }
  }
  return ( myVolume != 0 );
}

//=======================================================================
//function : Inverse
//purpose  : Inverse volume
//=======================================================================

#define SWAP_NODES(nodes,i1,i2)           \
{                                         \
  const SMDS_MeshNode* tmp = nodes[ i1 ]; \
  nodes[ i1 ] = nodes[ i2 ];              \
  nodes[ i2 ] = tmp;                      \
}
void SMDS_VolumeTool::Inverse ()
{
  if ( !myVolume ) return;

  if (myVolume->IsPoly()) {
    MESSAGE("Warning: attempt to inverse polyhedral volume");
    return;
  }

  myVolForward = !myVolForward;
  myCurFace = -1;

  // inverse top and bottom faces
  switch ( myVolumeNbNodes ) {
  case 4:
    SWAP_NODES( myVolumeNodes, 1, 2 );
    break;
  case 5:
    SWAP_NODES( myVolumeNodes, 1, 3 );
    break;
  case 6:
    SWAP_NODES( myVolumeNodes, 1, 2 );
    SWAP_NODES( myVolumeNodes, 4, 5 );
    break;
  case 8:
    SWAP_NODES( myVolumeNodes, 1, 3 );
    SWAP_NODES( myVolumeNodes, 5, 7 );
    break;
  default:;
  }
}

//=======================================================================
//function : GetVolumeType
//purpose  : 
//=======================================================================

SMDS_VolumeTool::VolumeType SMDS_VolumeTool::GetVolumeType() const
{
  if ( myPolyedre )
    return POLYHEDA;

  if ( myVolume ) {
    static const VolumeType types[] = {
      TETRA,     // myVolumeNbNodes = 4
      PYRAM,     // myVolumeNbNodes = 5
      PENTA,     // myVolumeNbNodes = 6
      UNKNOWN,   // myVolumeNbNodes = 7
      HEXA       // myVolumeNbNodes = 8
    };
    return types[ myVolumeNbNodes - 4 ];
  }

  return UNKNOWN;
}

//=======================================================================
//function : getTetraVolume
//purpose  : 
//=======================================================================

static double getTetraVolume(const SMDS_MeshNode* n1,
                             const SMDS_MeshNode* n2,
                             const SMDS_MeshNode* n3,
                             const SMDS_MeshNode* n4)
{
  double X1 = n1->X();
  double Y1 = n1->Y();
  double Z1 = n1->Z();

  double X2 = n2->X();
  double Y2 = n2->Y();
  double Z2 = n2->Z();

  double X3 = n3->X();
  double Y3 = n3->Y();
  double Z3 = n3->Z();

  double X4 = n4->X();
  double Y4 = n4->Y();
  double Z4 = n4->Z();

  double Q1 = -(X1-X2)*(Y3*Z4-Y4*Z3);
  double Q2 =  (X1-X3)*(Y2*Z4-Y4*Z2);
  double R1 = -(X1-X4)*(Y2*Z3-Y3*Z2);
  double R2 = -(X2-X3)*(Y1*Z4-Y4*Z1);
  double S1 =  (X2-X4)*(Y1*Z3-Y3*Z1);
  double S2 = -(X3-X4)*(Y1*Z2-Y2*Z1);

  return (Q1+Q2+R1+R2+S1+S2)/6.0;
}

//=======================================================================
//function : GetSize
//purpose  : Return element volume
//=======================================================================

double SMDS_VolumeTool::GetSize() const
{
  double V = 0.;
  if ( !myVolume )
    return 0.;

  if ( myVolume->IsPoly() )
  {
    if ( !myPolyedre )
      return 0.;

    // split a polyhedron into tetrahedrons

    SMDS_VolumeTool* me = const_cast< SMDS_VolumeTool* > ( this );
    XYZ baryCenter;
    me->GetBaryCenter(baryCenter.x, baryCenter.y, baryCenter.z);
    SMDS_MeshNode bcNode ( baryCenter.x, baryCenter.y, baryCenter.z );

    for ( int f = 0; f < NbFaces(); ++f )
    {
      bool externalFace = me->IsFaceExternal( f ); // it calls setFace()
      for ( int n = 2; n < myFaceNbNodes; ++n )
      {
        double Vn = getTetraVolume( myFaceNodes[ 0 ],
                                    myFaceNodes[ n-1 ],
                                    myFaceNodes[ n ],
                                    & bcNode );
///         cout <<"++++   " << Vn << "   nodes " <<myFaceNodes[ 0 ]->GetID() << " " <<myFaceNodes[ n-1 ]->GetID() << " " <<myFaceNodes[ n ]->GetID() << "        < " << V << endl;
        V += externalFace ? -Vn : Vn;
      }
    }
  }
  else 
  {
    const static int ind[] = {
      0, 1, 3, 6, 11 };
    const static int vtab[][4] = {
      // tetrahedron
      { 0, 1, 2, 3 },
      // pyramid
      { 0, 1, 3, 4 },
      { 1, 2, 3, 4 },
      // pentahedron
      { 0, 1, 2, 3 },
      { 1, 5, 3, 4 },
      { 1, 5, 2, 3 },
      // hexahedron
      { 1, 4, 3, 0 },
      { 4, 1, 6, 5 },
      { 1, 3, 6, 2 },
      { 4, 6, 3, 7 },
      { 1, 4, 6, 3 }
    };

    int type = GetVolumeType();
    int n1 = ind[type];
    int n2 = ind[type+1];

    for (int i = n1; i <  n2; i++)
    {
      V -= getTetraVolume( myVolumeNodes[ vtab[i][0] ],
                           myVolumeNodes[ vtab[i][1] ],
                           myVolumeNodes[ vtab[i][2] ],
                           myVolumeNodes[ vtab[i][3] ]);
    }
  }
  return V;
}

//=======================================================================
//function : GetBaryCenter
//purpose  : 
//=======================================================================

bool SMDS_VolumeTool::GetBaryCenter(double & X, double & Y, double & Z) const
{
  X = Y = Z = 0.;
  if ( !myVolume )
    return false;

  for ( int i = 0; i < myVolumeNbNodes; i++ ) {
    X += myVolumeNodes[ i ]->X();
    Y += myVolumeNodes[ i ]->Y();
    Z += myVolumeNodes[ i ]->Z();
  }
  X /= myVolumeNbNodes;
  Y /= myVolumeNbNodes;
  Z /= myVolumeNbNodes;

  return true;
}

//=======================================================================
//function : SetExternalNormal
//purpose  : Node order will be so that faces normals are external
//=======================================================================

void SMDS_VolumeTool::SetExternalNormal ()
{
  myExternalFaces = true;
  myCurFace = -1;
}

//=======================================================================
//function : NbFaceNodes
//purpose  : Return number of nodes in the array of face nodes
//=======================================================================

int SMDS_VolumeTool::NbFaceNodes( int faceIndex )
{
    if ( !setFace( faceIndex ))
      return 0;
    return myFaceNbNodes;
}

//=======================================================================
//function : GetFaceNodes
//purpose  : Return pointer to the array of face nodes.
//           To comfort link iteration, the array
//           length == NbFaceNodes( faceIndex ) + 1 and
//           the last node == the first one.
//=======================================================================

const SMDS_MeshNode** SMDS_VolumeTool::GetFaceNodes( int faceIndex )
{
  if ( !setFace( faceIndex ))
    return 0;
  return myFaceNodes;
}

//=======================================================================
//function : GetFaceNodesIndices
//purpose  : Return pointer to the array of face nodes indices
//           To comfort link iteration, the array
//           length == NbFaceNodes( faceIndex ) + 1 and
//           the last node index == the first one.
//=======================================================================

const int* SMDS_VolumeTool::GetFaceNodesIndices( int faceIndex )
{
  if (myVolume->IsPoly()) {
    MESSAGE("Warning: attempt to obtain FaceNodesIndices of polyhedral volume");
    return NULL;
  }
  if ( !setFace( faceIndex ))
    return 0;
  return myFaceNodeIndices;
}

//=======================================================================
//function : GetFaceNodes
//purpose  : Return a set of face nodes.
//=======================================================================

bool SMDS_VolumeTool::GetFaceNodes (int                        faceIndex,
                                    set<const SMDS_MeshNode*>& theFaceNodes )
{
  if ( !setFace( faceIndex ))
    return false;

  theFaceNodes.clear();
  int iNode, nbNode = myFaceNbNodes;
  for ( iNode = 0; iNode < nbNode; iNode++ )
    theFaceNodes.insert( myFaceNodes[ iNode ]);

  return true;
}

//=======================================================================
//function : IsFaceExternal
//purpose  : Check normal orientation of a returned face
//=======================================================================

bool SMDS_VolumeTool::IsFaceExternal( int faceIndex )
{
  if ( myExternalFaces || !myVolume )
    return true;

  if (myVolume->IsPoly()) {
    XYZ aNormal, baryCenter, p0 (myPolyedre->GetFaceNode(faceIndex + 1, 1));
    GetFaceNormal(faceIndex, aNormal.x, aNormal.y, aNormal.z);
    GetBaryCenter(baryCenter.x, baryCenter.y, baryCenter.z);
    XYZ insideVec (baryCenter - p0);
    if (insideVec.Dot(aNormal) > 0)
      return false;
    return true;
  }

  switch ( myVolumeNbNodes ) {
  case 4:
  case 5:
    // only the bottom of a reversed tetrahedron can be internal
    return ( myVolForward || faceIndex != 0 );
  case 6:
    // in a forward pentahedron, the top is internal, in a reversed one - bottom
    return ( myVolForward ? faceIndex != 1 : faceIndex != 0 );
  case 8: {
    // in a forward hexahedron, even face normal is external, odd - internal
    bool odd = faceIndex % 2;
    return ( myVolForward ? !odd : odd );
  }
  default:;
  }
  return false;
}

//=======================================================================
//function : GetFaceNormal
//purpose  : Return a normal to a face
//=======================================================================

bool SMDS_VolumeTool::GetFaceNormal (int faceIndex, double & X, double & Y, double & Z)
{
  if ( !setFace( faceIndex ))
    return false;

  XYZ p1 ( myFaceNodes[0] );
  XYZ p2 ( myFaceNodes[1] );
  XYZ p3 ( myFaceNodes[2] );
  XYZ aVec12( p2 - p1 );
  XYZ aVec13( p3 - p1 );
  XYZ cross = aVec12.Crossed( aVec13 );

  if ( myFaceNbNodes == 4 ) {
    XYZ p4 ( myFaceNodes[3] );
    XYZ aVec14( p4 - p1 );
    XYZ cross2 = aVec13.Crossed( aVec14 );
    cross.x += cross2.x;
    cross.y += cross2.y;
    cross.z += cross2.z;    
  }

  double size = cross.Magnitude();
  if ( size <= DBL_MIN )
    return false;

  X = cross.x / size;
  Y = cross.y / size;
  Z = cross.z / size;

  return true;
}

//=======================================================================
//function : GetFaceArea
//purpose  : Return face area
//=======================================================================

double SMDS_VolumeTool::GetFaceArea( int faceIndex )
{
  if (myVolume->IsPoly()) {
    MESSAGE("Warning: attempt to obtain area of a face of polyhedral volume");
    return 0;
  }

  if ( !setFace( faceIndex ))
    return 0;

  XYZ p1 ( myFaceNodes[0] );
  XYZ p2 ( myFaceNodes[1] );
  XYZ p3 ( myFaceNodes[2] );
  XYZ aVec12( p2 - p1 );
  XYZ aVec13( p3 - p1 );
  double area = aVec12.Crossed( aVec13 ).Magnitude() * 0.5;

  if ( myFaceNbNodes == 4 ) {
    XYZ p4 ( myFaceNodes[3] );
    XYZ aVec14( p4 - p1 );
    area += aVec14.Crossed( aVec13 ).Magnitude() * 0.5;
  }
  return area;
}

//=======================================================================
//function : GetOppFaceIndex
//purpose  : Return index of the opposite face if it exists, else -1.
//=======================================================================

int SMDS_VolumeTool::GetOppFaceIndex( int faceIndex ) const
{
  int ind = -1;
  if (myVolume->IsPoly()) {
    MESSAGE("Warning: attempt to obtain opposite face on polyhedral volume");
    return ind;
  }

  if ( faceIndex >= 0 && faceIndex < NbFaces() ) {
    switch ( myVolumeNbNodes ) {
    case 6:
      if ( faceIndex == 0 || faceIndex == 1 )
        ind = 1 - faceIndex;
        break;
    case 8:
      ind = faceIndex + ( faceIndex % 2 ? -1 : 1 );
      break;
    default:;
    }
  }
  return ind;
}

//=======================================================================
//function : IsLinked
//purpose  : return true if theNode1 is linked with theNode2
//=======================================================================

bool SMDS_VolumeTool::IsLinked (const SMDS_MeshNode* theNode1,
                                const SMDS_MeshNode* theNode2) const
{
  if ( !myVolume )
    return false;

  if (myVolume->IsPoly()) {
    if (!myPolyedre) {
      MESSAGE("Warning: bad volumic element");
      return false;
    }
    bool isLinked = false;
    int iface;
    for (iface = 1; iface <= myNbFaces && !isLinked; iface++) {
      int inode, nbFaceNodes = myPolyedre->NbFaceNodes(iface);

      for (inode = 1; inode <= nbFaceNodes && !isLinked; inode++) {
        const SMDS_MeshNode* curNode = myPolyedre->GetFaceNode(iface, inode);

        if (curNode == theNode1 || curNode == theNode2) {
          int inextnode = (inode == nbFaceNodes) ? 1 : inode + 1;
          const SMDS_MeshNode* nextNode = myPolyedre->GetFaceNode(iface, inextnode);

          if ((curNode == theNode1 && nextNode == theNode2) ||
              (curNode == theNode2 && nextNode == theNode1)) {
            isLinked = true;
          }
        }
      }
    }
    return isLinked;
  }

  // find nodes indices
  int i1 = -1, i2 = -1;
  for ( int i = 0; i < myVolumeNbNodes; i++ ) {
    if ( myVolumeNodes[ i ] == theNode1 )
      i1 = i;
    else if ( myVolumeNodes[ i ] == theNode2 )
      i2 = i;
  }
  return IsLinked( i1, i2 );
}

//=======================================================================
//function : IsLinked
//purpose  : return true if the node with theNode1Index is linked
//           with the node with theNode2Index
//=======================================================================

bool SMDS_VolumeTool::IsLinked (const int theNode1Index,
                                const int theNode2Index) const
{
  if (myVolume->IsPoly()) {
    return IsLinked(myVolumeNodes[theNode1Index], myVolumeNodes[theNode2Index]);
  }

  int minInd = theNode1Index < theNode2Index ? theNode1Index : theNode2Index;
  int maxInd = theNode1Index < theNode2Index ? theNode2Index : theNode1Index;

  if ( minInd < 0 || maxInd > myVolumeNbNodes - 1 || maxInd == minInd )
    return false;

  switch ( myVolumeNbNodes ) {
  case 4:
    return true;
  case 5:
    if ( maxInd == 4 )
      return true;
    switch ( maxInd - minInd ) {
    case 1:
    case 3: return true;
    default:;
    }
    break;
  case 6:
    switch ( maxInd - minInd ) {
    case 1: return minInd != 2;
    case 2: return minInd == 0 || minInd == 3;
    case 3: return true;
    default:;
    }
    break;
  case 8:
    switch ( maxInd - minInd ) {
    case 1: return minInd != 3;
    case 3: return minInd == 0 || minInd == 4;
    case 4: return true;
    default:;
    }
    break;
  default:;
  }
  return false;
}

//=======================================================================
//function : GetNodeIndex
//purpose  : Return an index of theNode
//=======================================================================

int SMDS_VolumeTool::GetNodeIndex(const SMDS_MeshNode* theNode) const
{
  if ( myVolume ) {
    for ( int i = 0; i < myVolumeNbNodes; i++ ) {
      if ( myVolumeNodes[ i ] == theNode )
        return i;
    }
  }
  return -1;
}

//=======================================================================
//function : IsFreeFace
//purpose  : check that only one volume is build on the face nodes
//=======================================================================

bool SMDS_VolumeTool::IsFreeFace( int faceIndex )
{
  const int free = true;

  if (!setFace( faceIndex ))
    return !free;

  const SMDS_MeshNode** nodes = GetFaceNodes( faceIndex );
  int nbFaceNodes = myFaceNbNodes;

  // evaluate nb of face nodes shared by other volume
  int maxNbShared = -1;
  typedef map< const SMDS_MeshElement*, int > TElemIntMap;
  TElemIntMap volNbShared;
  TElemIntMap::iterator vNbIt;
  for ( int iNode = 0; iNode < nbFaceNodes; iNode++ )
  {
    const SMDS_MeshNode* n = nodes[ iNode ];
    SMDS_ElemIteratorPtr eIt = n->GetInverseElementIterator();
    while ( eIt->more() ) {
      const SMDS_MeshElement* elem = eIt->next();
      if ( elem != myVolume && elem->GetType() == SMDSAbs_Volume ) {
        int nbShared = 1;
        vNbIt = volNbShared.find( elem );
        if ( vNbIt == volNbShared.end() )
          volNbShared.insert ( TElemIntMap::value_type( elem, nbShared ));
        else
          nbShared = ++(*vNbIt).second;
        if ( nbShared > maxNbShared )
          maxNbShared = nbShared;
      }
    }
  }
  if ( maxNbShared < 3 )
    return free; // is free

  // find volumes laying on the opposite side of the face
  // and sharing all nodes
  XYZ intNormal; // internal normal
  GetFaceNormal( faceIndex, intNormal.x, intNormal.y, intNormal.z );
  if ( IsFaceExternal( faceIndex ))
    intNormal = XYZ( -intNormal.x, -intNormal.y, -intNormal.z );
  XYZ p0 ( nodes[0] ), baryCenter;
  for ( vNbIt = volNbShared.begin(); vNbIt != volNbShared.end(); vNbIt++ )
  {
    int nbShared = (*vNbIt).second;
    if ( nbShared >= 3 ) {
      SMDS_VolumeTool volume( (*vNbIt).first );
      volume.GetBaryCenter( baryCenter.x, baryCenter.y, baryCenter.z );
      XYZ intNormal2( baryCenter - p0 );
      if ( intNormal.Dot( intNormal2 ) < 0 )
        continue; // opposite side
    }
    // remove a volume from volNbShared map
    volNbShared.erase( vNbIt );
  }
  // here volNbShared contains only volumes laying on the
  // opposite side of the face
  if ( volNbShared.empty() )
    return free; // is free

  // check if the whole area of a face is shared
  bool isShared[] = { false, false, false, false }; // 4 triangle parts of a quadrangle
  for ( vNbIt = volNbShared.begin(); vNbIt != volNbShared.end(); vNbIt++ )
  {
    SMDS_VolumeTool volume( (*vNbIt).first );
    bool prevLinkShared = false;
    int nbSharedLinks = 0;
    for ( int iNode = 0; iNode < nbFaceNodes; iNode++ )
    {
      bool linkShared = volume.IsLinked( nodes[ iNode ], nodes[ iNode + 1] );
      if ( linkShared )
        nbSharedLinks++;
      if ( linkShared && prevLinkShared &&
          volume.IsLinked( nodes[ iNode - 1 ], nodes[ iNode + 1] ))
        isShared[ iNode ] = true;
      prevLinkShared = linkShared;
    }
    if ( nbSharedLinks == nbFaceNodes )
      return !free; // is not free
    if ( nbFaceNodes == 4 ) {
      // check traingle parts 1 & 3
      if ( isShared[1] && isShared[3] )
        return !free; // is not free
      // check triangle parts 0 & 2;
      // 0 part could not be checked in the loop; check it here
      if ( isShared[2] && prevLinkShared &&
          volume.IsLinked( nodes[ 0 ], nodes[ 1 ] ) &&
          volume.IsLinked( nodes[ 1 ], nodes[ 3 ] ) )
        return !free; // is not free
    }
  }
  return free;
}

//=======================================================================
//function : GetFaceIndex
//purpose  : Return index of a face formed by theFaceNodes
//=======================================================================

int SMDS_VolumeTool::GetFaceIndex( const set<const SMDS_MeshNode*>& theFaceNodes )
{
  for ( int iFace = 0; iFace < myNbFaces; iFace++ ) {
    const SMDS_MeshNode** nodes = GetFaceNodes( iFace );
    int nbFaceNodes = NbFaceNodes( iFace );
    set<const SMDS_MeshNode*> nodeSet;
    for ( int iNode = 0; iNode < nbFaceNodes; iNode++ )
      nodeSet.insert( nodes[ iNode ] );
    if ( theFaceNodes == nodeSet )
      return iFace;
  }
  return -1;
}

//=======================================================================
//function : GetFaceIndex
//purpose  : Return index of a face formed by theFaceNodes
//=======================================================================

/*int SMDS_VolumeTool::GetFaceIndex( const set<int>& theFaceNodesIndices )
{
  for ( int iFace = 0; iFace < myNbFaces; iFace++ ) {
    const int* nodes = GetFaceNodesIndices( iFace );
    int nbFaceNodes = NbFaceNodes( iFace );
    set<int> nodeSet;
    for ( int iNode = 0; iNode < nbFaceNodes; iNode++ )
      nodeSet.insert( nodes[ iNode ] );
    if ( theFaceNodesIndices == nodeSet )
      return iFace;
  }
  return -1;
}*/

//=======================================================================
//function : setFace
//purpose  : 
//=======================================================================

bool SMDS_VolumeTool::setFace( int faceIndex )
{
  if ( !myVolume )
    return false;

  if ( myCurFace == faceIndex )
    return true;

  myCurFace = -1;

  if ( faceIndex < 0 || faceIndex >= NbFaces() )
    return false;

  if (myFaceNodes != NULL) {
    delete [] myFaceNodes;
    myFaceNodes = NULL;
  }

  if (myVolume->IsPoly()) {
    if (!myPolyedre) {
      MESSAGE("Warning: bad volumic element");
      return false;
    }

    // check orientation
    bool isGoodOri = true;
    if (myExternalFaces)
      isGoodOri = IsFaceExternal( faceIndex );

    // set face nodes
    int iNode;
    myFaceNbNodes = myPolyedre->NbFaceNodes(faceIndex + 1);
    myFaceNodes = new const SMDS_MeshNode* [myFaceNbNodes + 1];
    if (isGoodOri) {
      for ( iNode = 0; iNode < myFaceNbNodes; iNode++ )
        myFaceNodes[ iNode ] = myPolyedre->GetFaceNode(faceIndex + 1, iNode + 1);
    } else {
      for ( iNode = 0; iNode < myFaceNbNodes; iNode++ )
        myFaceNodes[ iNode ] = myPolyedre->GetFaceNode(faceIndex + 1, myFaceNbNodes - iNode);
    }
    myFaceNodes[ myFaceNbNodes ] = myFaceNodes[ 0 ]; // last = first

  } else {
    // choose face node indices
    switch ( myVolumeNbNodes ) {
    case 4:
      myFaceNbNodes = Tetra_nbN[ faceIndex ];
      if ( myExternalFaces )
        myFaceNodeIndices = myVolForward ? Tetra_F[ faceIndex ] : Tetra_RE[ faceIndex ];
      else
        myFaceNodeIndices = myVolForward ? Tetra_F[ faceIndex ] : Tetra_R[ faceIndex ];
      break;
    case 5:
      myFaceNbNodes = Pyramid_nbN[ faceIndex ];
      if ( myExternalFaces )
        myFaceNodeIndices = myVolForward ? Pyramid_F[ faceIndex ] : Pyramid_RE[ faceIndex ];
      else
        myFaceNodeIndices = myVolForward ? Pyramid_F[ faceIndex ] : Pyramid_R[ faceIndex ];
      break;
    case 6:
      myFaceNbNodes = Penta_nbN[ faceIndex ];
      if ( myExternalFaces )
        myFaceNodeIndices = myVolForward ? Penta_FE[ faceIndex ] : Penta_RE[ faceIndex ];
      else
        myFaceNodeIndices = myVolForward ? Penta_F[ faceIndex ] : Penta_R[ faceIndex ];
      break;
    case 8:
      myFaceNbNodes = Hexa_nbN[ faceIndex ];
      if ( myExternalFaces )
        myFaceNodeIndices = myVolForward ? Hexa_FE[ faceIndex ] : Hexa_RE[ faceIndex ];
      else
        myFaceNodeIndices = Hexa_F[ faceIndex ];
      break;
    default:
      return false;
    }

    // set face nodes
    myFaceNodes = new const SMDS_MeshNode* [myFaceNbNodes + 1];
    for ( int iNode = 0; iNode < myFaceNbNodes; iNode++ )
      myFaceNodes[ iNode ] = myVolumeNodes[ myFaceNodeIndices[ iNode ]];
    myFaceNodes[ myFaceNbNodes ] = myFaceNodes[ 0 ];
  }

  myCurFace = faceIndex;

  return true;
}

//=======================================================================
//function : GetType
//purpose  : return VolumeType by nb of nodes in a volume
//=======================================================================

SMDS_VolumeTool::VolumeType SMDS_VolumeTool::GetType(int nbNodes)
{
  switch ( nbNodes ) {
  case 4: return TETRA;
  case 5: return PYRAM;
  case 6: return PENTA;
  case 8: return HEXA;
  default:return UNKNOWN;
  }
}

//=======================================================================
//function : NbFaces
//purpose  : return nb of faces by volume type
//=======================================================================

int SMDS_VolumeTool::NbFaces( VolumeType type )
{
  switch ( type ) {
  case TETRA: return 4;
  case PYRAM: return 5;
  case PENTA: return 5;
  case HEXA : return 6;
  default:    return 0;
  }
}

//=======================================================================
//function : GetFaceNodesIndices
//purpose  : Return the array of face nodes indices
//           To comfort link iteration, the array
//           length == NbFaceNodes( faceIndex ) + 1 and
//           the last node index == the first one.
//=======================================================================

const int* SMDS_VolumeTool::GetFaceNodesIndices(VolumeType type,
                                                int        faceIndex,
                                                bool       external)
{
  switch ( type ) {
  case TETRA: return Tetra_F[ faceIndex ];
  case PYRAM: return Pyramid_F[ faceIndex ];
  case PENTA: return external ? Penta_FE[ faceIndex ] : Penta_F[ faceIndex ];
  case HEXA:  return external ? Hexa_FE[ faceIndex ] : Hexa_F[ faceIndex ];
  default:;
  }
  return 0;
}

//=======================================================================
//function : NbFaceNodes
//purpose  : Return number of nodes in the array of face nodes
//=======================================================================

int SMDS_VolumeTool::NbFaceNodes(VolumeType type,
                                 int        faceIndex )
{
  switch ( type ) {
  case TETRA: return Tetra_nbN[ faceIndex ];
  case PYRAM: return Pyramid_nbN[ faceIndex ];
  case PENTA: return Penta_nbN[ faceIndex ];
  case HEXA:  return Hexa_nbN[ faceIndex ];
  default:;
  }
  return 0;
}

