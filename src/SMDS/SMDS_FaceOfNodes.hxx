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

#ifndef _SMDS_FaceOfNodes_HeaderFile
#define _SMDS_FaceOfNodes_HeaderFile

#include <iostream>
#include "SMDS_MeshFace.hxx"
#include "SMDS_MeshNode.hxx"
#include "SMDS_Iterator.hxx"

class SMDS_FaceOfNodes:public SMDS_MeshFace
{
  public:
	void Print(ostream & OS) const;
	SMDS_FaceOfNodes(SMDS_MeshNode* node1, SMDS_MeshNode* node2,
		SMDS_MeshNode* node3);
	SMDS_FaceOfNodes(SMDS_MeshNode* node1, SMDS_MeshNode* node2,
		SMDS_MeshNode* node3, SMDS_MeshNode* node4);
		
	int NbEdges() const;
	int NbFaces() const;
	int NbNodes() const;
  protected:
  	SMDS_Iterator<const SMDS_MeshElement *> *
		elementsIterator(SMDSAbs_ElementType type) const;

  private:
	vector<const SMDS_MeshNode*> myNodes;

};

#endif
