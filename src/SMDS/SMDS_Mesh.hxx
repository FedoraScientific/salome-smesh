//  SMESH SMDS : implementaion of Salome mesh data structure
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
//  File   : SMDS_Mesh.hxx
//  Module : SMESH

#ifndef _SMDS_Mesh_HeaderFile
#define _SMDS_Mesh_HeaderFile

#include "SMDS_MeshNode.hxx"
#include "SMDS_MeshEdge.hxx"
#include "SMDS_MeshFace.hxx"
#include "SMDS_MeshVolume.hxx"
#include "SMDS_MeshElementIDFactory.hxx"
#include "SMDS_Iterator.hxx"

#include <set>
#include <list>

class SMDSControl_BoundaryEdges;

class SMDS_Mesh:public SMDS_MeshObject
{

  public:

	SMDS_Mesh();

	SMDS_Iterator<const SMDS_MeshNode *> * nodesIterator() const;
	SMDS_Iterator<const SMDS_MeshEdge *> * edgesIterator() const;
	SMDS_Iterator<const SMDS_MeshFace *> * facesIterator() const;
	SMDS_Iterator<const SMDS_MeshVolume *> * volumesIterator() const;

	SMDS_Mesh *AddSubMesh();
	virtual SMDS_MeshNode* AddNode(double x, double y, double z);
	virtual SMDS_MeshNode* AddNodeWithID(double x, double y, double z, int ID);

	virtual SMDS_MeshEdge* AddEdge(int idnode1, int idnode2);
	virtual SMDS_MeshEdge* AddEdgeWithID(int idnode1, int idnode2, int ID);
	virtual SMDS_MeshFace* AddFace(int idnode1, int idnode2, int idnode3);
	virtual SMDS_MeshFace* AddFaceWithID(int idnode1, int idnode2, int idnode3,
		int ID);
	virtual SMDS_MeshFace* AddFace(int idnode1, int idnode2, int idnode3,
		int idnode4);
	virtual SMDS_MeshFace* AddFaceWithID(int idnode1, int idnode2, int idnode3,
		int idnode4, int ID);
	virtual SMDS_MeshVolume* AddVolume(int idnode1, int idnode2, int idnode3,
		int idnode4);
	virtual SMDS_MeshVolume* AddVolumeWithID(int idnode1, int idnode2,
		int idnode3, int idnode4, int ID);
	virtual SMDS_MeshVolume* AddVolume(int idnode1, int idnode2, int idnode3,
		int idnode4, int idnode5);
	virtual SMDS_MeshVolume* AddVolumeWithID(int idnode1, int idnode2, int idnode3,
		int idnode4, int idnode5, int ID);
	virtual SMDS_MeshVolume* AddVolume(int idnode1, int idnode2, int idnode3,
		int idnode4, int idnode5, int idnode6);
	virtual SMDS_MeshVolume* AddVolumeWithID(int idnode1, int idnode2,
		int idnode3, int idnode4, int idnode5, int idnode6, int ID);
	virtual SMDS_MeshVolume* AddVolume(int idnode1, int idnode2, int idnode3,
		int idnode4, int idnode5, int idnode6, int idnode7, int idnode8);
	virtual SMDS_MeshVolume* AddVolumeWithID(int idnode1, int idnode2, int idnode3,
		int idnode4, int idnode5, int idnode6, int idnode7, int idnode8,
		int ID);
	virtual void RemoveNode(int IDnode);
	virtual void RemoveEdge(int idnode1, int idnode2);
	virtual void RemoveFace(int idnode1, int idnode2, int idnode3);
	virtual void RemoveFace(int idnode1, int idnode2, int idnode3, int idnode4);
	virtual void RemoveElement(int IDelem, const bool removenodes = false);
	void RemoveElement(const SMDS_MeshElement * elem, const bool removenodes =
		false);
    bool RemoveNode(const SMDS_MeshNode * node);	//Remove a node and elements which need this node
	void RemoveEdge(const SMDS_MeshEdge * edge);
	void RemoveFace(const SMDS_MeshFace * face);
	void RemoveVolume(const SMDS_MeshVolume * volume);

	virtual bool RemoveFromParent();
	virtual bool RemoveSubMesh(const SMDS_Mesh * aMesh);
	
	const SMDS_MeshNode *FindNode(int idnode) const;
	const SMDS_MeshEdge *FindEdge(int idnode1, int idnode2) const;
	const SMDS_MeshFace *FindFace(int idnode1, int idnode2, int idnode3) const;
	const SMDS_MeshFace *FindFace(int idnode1, int idnode2, int idnode3, int idnode4) const;
	const SMDS_MeshElement *FindElement(int IDelem) const;
	
	int NbNodes() const;
	int NbEdges() const;
	int NbFaces() const;
	int NbVolumes() const;
	int NbSubMesh() const;
	void DumpNodes() const;
	void DumpEdges() const;
	void DumpFaces() const;
	void DumpVolumes() const;
	void DebugStats() const;
	SMDS_Mesh *boundaryFaces();
	SMDS_Mesh *boundaryEdges();
	 ~SMDS_Mesh();
    bool hasConstructionEdges();
    bool hasConstructionFaces();
	bool hasInverseElements();
    void setConstructionEdges(bool);
    void setConstructionFaces(bool);
	void setInverseElements(bool);

  private:
    SMDS_Mesh(SMDS_Mesh * parent);
    SMDS_MeshFace * createTriangle(SMDS_MeshNode * node1,
		SMDS_MeshNode * node2, SMDS_MeshNode * node3);
	SMDS_MeshFace * createQuadrangle(SMDS_MeshNode * node1,
		SMDS_MeshNode * node2, SMDS_MeshNode * node3, SMDS_MeshNode * node4);
	void removeElementDependencies(SMDS_MeshElement * element);
	const SMDS_MeshEdge* FindEdge(const SMDS_MeshNode * n1,
		const SMDS_MeshNode * n2) const;
	SMDS_MeshEdge* FindEdgeOrCreate(const SMDS_MeshNode * n1,
		const SMDS_MeshNode * n2);
	SMDS_MeshFace* FindFaceOrCreate(
		const SMDS_MeshNode *n1,
		const SMDS_MeshNode *n2,
		const SMDS_MeshNode *n3);
	const SMDS_MeshFace* FindFace(
		const SMDS_MeshNode *n1,
		const SMDS_MeshNode *n2,
		const SMDS_MeshNode *n3,
		const SMDS_MeshNode *n4) const;
	SMDS_MeshFace* FindFaceOrCreate(
		const SMDS_MeshNode *n1,
		const SMDS_MeshNode *n2,
		const SMDS_MeshNode *n3,
		const SMDS_MeshNode *n4);

	// Fields PRIVATE
	typedef set<SMDS_MeshNode *> SetOfNodes;
	typedef set<SMDS_MeshEdge *> SetOfEdges;
	typedef set<SMDS_MeshFace *> SetOfFaces;
	typedef set<SMDS_MeshVolume *> SetOfVolumes;

	SetOfNodes myNodes;
	SetOfEdges myEdges;
	SetOfFaces myFaces;
	SetOfVolumes myVolumes;
	SMDS_Mesh *myParent;
	list<SMDS_Mesh *> myChildren;
	SMDS_MeshElementIDFactory *myNodeIDFactory;
	SMDS_MeshElementIDFactory *myElementIDFactory;

    bool myHasConstructionEdges;
    bool myHasConstructionFaces;
	bool myHasInverseElements;
};

#endif
