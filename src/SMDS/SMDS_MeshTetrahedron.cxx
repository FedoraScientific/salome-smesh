using namespace std;
// File:	SMDS_MeshTetrahedron.cxx
// Created:	Wed Jan 23 17:02:34 2002
// Author:	Jean-Michel BOULCOURT
//		<jmb@coulox.paris1.matra-dtv.fr>


#include "SMDS_MeshTetrahedron.ixx"
#include "Standard_ConstructionError.hxx"

//=======================================================================
//function : SMDS_MeshTetrahedron
//purpose  : Tetrahedra
//=======================================================================

SMDS_MeshTetrahedron::SMDS_MeshTetrahedron(const Standard_Integer ID,
					   const Standard_Integer idnode1, 
					   const Standard_Integer idnode2, 
					   const Standard_Integer idnode3,
					   const Standard_Integer idnode4):SMDS_MeshVolume(ID,4)
{
  SetConnections(idnode1,idnode2,idnode3,idnode4);
  ComputeKey();
}


//=======================================================================
//function : SetConnections
//purpose  : Tetrahedra
//=======================================================================
void SMDS_MeshTetrahedron::SetConnections(const Standard_Integer idnode1, 
					  const Standard_Integer idnode2,
					  const Standard_Integer idnode3,
					  const Standard_Integer idnode4)
{
  Standard_Integer idmin = (idnode1 < idnode2 ? idnode1 : idnode2);
  idmin = (idmin < idnode3 ? idmin : idnode3);
  idmin = (idmin < idnode4 ? idmin : idnode4);

  myNodes[0] = idmin;
  if (idmin == idnode1) {
    myNodes[1] = idnode2;
    myNodes[2] = idnode3;
    myNodes[3] = idnode4;
  } else if (idmin == idnode2) {
    myNodes[1] = idnode3;
    myNodes[2] = idnode4;
    myNodes[3] = idnode1;
  } else if (idmin == idnode3) {
    myNodes[1] = idnode4;
    myNodes[2] = idnode1;
    myNodes[3] = idnode2;
  } else {
    myNodes[1] = idnode1;
    myNodes[2] = idnode2;
    myNodes[3] = idnode3;
  }

}


//=======================================================================
//function : NodesOfFace
//purpose  : returns the rank node in mynodes. Useful to extract faces from volume
//=======================================================================
Standard_Integer SMDS_MeshTetrahedron::NodesOfFace(const Standard_Integer rankface, 
						   const Standard_Integer ranknode)
{
  static Standard_Integer facenode[4][3] = {
    {0,1,2},
    {0,3,1},
    {1,3,2},
    {0,2,3}
  };

  return facenode[rankface-1][ranknode-1];
}


//=======================================================================
//function : NodesOfEdge
//purpose  : returns the rank node in mynodes. Useful to extract edges from volume
//=======================================================================
Standard_Integer SMDS_MeshTetrahedron::NodesOfEdge(const Standard_Integer rankedge, 
						   const Standard_Integer ranknode) const
{
  static Standard_Integer faceedge[6][2] = {
    {0,1},
    {1,2},
    {0,2},
    {0,3},
    {1,3},
    {2,3}
  };

  return faceedge[rankedge-1][ranknode-1];
}


//=======================================================================
//function : GetFaceDefinedByNodes
//purpose  : 
//=======================================================================
void SMDS_MeshTetrahedron::GetFaceDefinedByNodes(const Standard_Integer rank, 
						 const Standard_Address idnode,
						 Standard_Integer& nb) const
{
  Standard_Integer *ptr;
  nb = 3;
  ptr = (Standard_Integer *)idnode;
  ptr[0] = myNodes[NodesOfFace(rank,1)];
  ptr[1] = myNodes[NodesOfFace(rank,2)];
  ptr[2] = myNodes[NodesOfFace(rank,3)];

}

//=======================================================================
//function : GetEdgeDefinedByNodes
//purpose  : 
//=======================================================================
void SMDS_MeshTetrahedron::GetEdgeDefinedByNodes(const Standard_Integer rank, 
					    Standard_Integer& idnode1,
					    Standard_Integer& idnode2) const
{
  idnode1 = myNodes[NodesOfEdge(rank,1)];
  idnode2 = myNodes[NodesOfEdge(rank,2)];
}
