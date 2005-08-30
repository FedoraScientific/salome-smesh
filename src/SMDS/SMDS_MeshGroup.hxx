//  SMESH SMDS : implementaion of Salome mesh data structure
//
//  Copyright (C) 2003  OPEN CASCADE
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
//  See http://www.opencascade.org or email : webmaster@opencascade.org 
//
//
//
//  File   : SMDS_MeshGroup.hxx
//  Module : SMESH

#ifndef _SMDS_MeshGroup_HeaderFile
#define _SMDS_MeshGroup_HeaderFile

#include "SMDS_Mesh.hxx"
#include <set>

//#ifdef WNT
//#include <SALOME_WNT.hxx>
//#else
//#define SALOME_WNT_EXPORT
//#endif

#if defined WNT && defined WIN32 && defined SMDS_EXPORTS
#define SMDS_WNT_EXPORT __declspec( dllexport )
#else
#define SMDS_WNT_EXPORT
#endif

class SMDS_WNT_EXPORT SMDS_MeshGroup:public SMDS_MeshObject
{
  public:
	SMDS_MeshGroup(const SMDS_Mesh * theMesh,
                       const SMDSAbs_ElementType theType = SMDSAbs_All);
	const SMDS_MeshGroup * AddSubGroup
                      (const SMDSAbs_ElementType theType = SMDSAbs_All);
	virtual bool RemoveSubGroup(const SMDS_MeshGroup* theGroup);
	virtual bool RemoveFromParent();

        const SMDS_Mesh* GetMesh() const { return myMesh; }

        void SetType (const SMDSAbs_ElementType theType);
        void Clear();
	void Add(const SMDS_MeshElement * theElem);
	void Remove(const SMDS_MeshElement * theElem);
	bool IsEmpty() const { return myElements.empty(); }
	int Extent() const { return myElements.size(); }

        SMDSAbs_ElementType GetType() const { return myType; }

	bool Contains(const SMDS_MeshElement * theElem) const;

        void InitIterator() const
        { const_cast<TIterator&>(myIterator) = myElements.begin(); }

        bool More() const { return myIterator != myElements.end(); }

        const SMDS_MeshElement* Next() const
        { return *(const_cast<TIterator&>(myIterator))++; }

  private:
	SMDS_MeshGroup(SMDS_MeshGroup* theParent,
                       const SMDSAbs_ElementType theType = SMDSAbs_All);

        typedef std::set<const SMDS_MeshElement *>::const_iterator TIterator;
	const SMDS_Mesh *                       myMesh;
	SMDSAbs_ElementType                     myType;
	std::set<const SMDS_MeshElement *>      myElements;
	SMDS_MeshGroup *                        myParent;
	std::list<const SMDS_MeshGroup*>        myChildren;
        TIterator                               myIterator;
};
#endif
