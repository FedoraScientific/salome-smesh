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

#include "SMDS_FaceOfEdges.hxx"
#include "SMDS_IteratorOfElements.hxx"
#include "SMDS_MeshNode.hxx"

//=======================================================================
//function : NbEdges
//purpose  : 
//=======================================================================

int SMDS_FaceOfEdges::NbEdges() const
{
	return myEdges.size();
}

int SMDS_FaceOfEdges::NbFaces() const
{
	return 1;
}
//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

void SMDS_FaceOfEdges::Print(ostream & OS) const
{
	OS << "face <" << GetID() << " > : ";
	int i;
	for (i = 0; i < NbEdges() - 1; i++) OS << myEdges[i] << ",";
	OS << myEdges[i] << ") " << endl;
}

SMDSAbs_ElementType SMDS_FaceOfEdges::GetType() const
{
	return SMDSAbs_Face;
}

class SMDS_FaceOfEdges_MyIterator:public SMDS_ElemIterator
{
  const vector<const SMDS_MeshEdge*>& mySet;
  int index;
 public:
  SMDS_FaceOfEdges_MyIterator(const vector<const SMDS_MeshEdge*>& s):
    mySet(s),index(0) {}

  bool more()
  {
    return index<mySet.size();
  }

  const SMDS_MeshElement* next()
  {
    index++;
    return mySet[index-1];
  }	
};
SMDS_ElemIteratorPtr SMDS_FaceOfEdges::
	elementsIterator(SMDSAbs_ElementType type) const
{
	switch(type)
	{
	case SMDSAbs_Face:
          return SMDS_MeshElement::elementsIterator(SMDSAbs_Face);
	case SMDSAbs_Edge:
          return SMDS_ElemIteratorPtr(new SMDS_FaceOfEdges_MyIterator(myEdges));
	default:
          return SMDS_ElemIteratorPtr
            (new SMDS_IteratorOfElements
             (this,type, SMDS_ElemIteratorPtr(new SMDS_FaceOfEdges_MyIterator(myEdges))));
	}
}

SMDS_FaceOfEdges::SMDS_FaceOfEdges(const SMDS_MeshEdge* edge1,
                                   const SMDS_MeshEdge* edge2,
                                   const SMDS_MeshEdge* edge3)
{
	myEdges.resize(3);
	myEdges[0]=edge1;
	myEdges[1]=edge2;
	myEdges[2]=edge3;
}

SMDS_FaceOfEdges::SMDS_FaceOfEdges(const SMDS_MeshEdge* edge1,
                                   const SMDS_MeshEdge* edge2,
                                   const SMDS_MeshEdge* edge3,
                                   const SMDS_MeshEdge* edge4)
{
	myEdges.resize(4);
	myEdges[0]=edge1;
	myEdges[1]=edge2;
	myEdges[2]=edge3;
	myEdges[3]=edge4;	
}

/*bool operator<(const SMDS_FaceOfEdges& f1, const SMDS_FaceOfEdges& f2)
{
	set<SMDS_MeshNode> set1,set2;
	SMDS_ElemIteratorPtr it;
	const SMDS_MeshNode * n;

	it=f1.nodesIterator();

	while(it->more())
	{
		n=static_cast<const SMDS_MeshNode *>(it->next());
		set1.insert(*n);
	}

	delete it;
	it=f2.nodesIterator();
	
	while(it->more())
	{	
		n=static_cast<const SMDS_MeshNode *>(it->next());
		set2.insert(*n);
	}

	delete it;
	return set1<set2;	

}*/

