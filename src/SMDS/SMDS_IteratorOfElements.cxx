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

#include "SMDS_IteratorOfElements.hxx"

bool SMDS_IteratorOfElements::subMore()
{
	if((t2Iterator==NULL)||(!t2Iterator->more()))
	{
		if(t1Iterator->more())
		{
			if(t2Iterator!=NULL) delete t2Iterator;
			t2Iterator=t1Iterator->next()->elementsIterator(myType);
			return subMore();
		}
		else return false;
	}
	else return true;
}

const SMDS_MeshElement * SMDS_IteratorOfElements::subNext()
{
	if((t2Iterator==NULL)||(!t2Iterator->more()))
	{
		if(t1Iterator->more())
		{
			if(t2Iterator!=NULL) delete t2Iterator;
			t2Iterator=t1Iterator->next()->elementsIterator(myType);
		}
	}
	return t2Iterator->next();
}

/////////////////////////////////////////////////////////////////////////////
/// Create an iterator which look for elements of type type which are linked 
/// to the element element. it is the iterator to get connectivity of element
//////////////////////////////////////////////////////////////////////////////
SMDS_IteratorOfElements::SMDS_IteratorOfElements(const SMDS_MeshElement * element,
	SMDSAbs_ElementType type, SMDS_Iterator<const SMDS_MeshElement *>* it)
	:t1Iterator(it), t2Iterator(NULL), myType(type), myElement(element),
	myProxyElement(NULL)
{
	while(subMore())
		alreadyReturnedElements.insert(subNext());
	itAlreadyReturned= alreadyReturnedElements.begin();
	switch(myElement->GetType())
	{
	case SMDSAbs_Node: 
	case SMDSAbs_Edge: myReverseIteration=true; break;
	case SMDSAbs_Face: myReverseIteration=(type==SMDSAbs_Volume); break;
	default: myReverseIteration=false;
	}	
}

bool SMDS_IteratorOfElements::more()
{
	if(myProxyElement==NULL)
	{
		while(itAlreadyReturned!=alreadyReturnedElements.end())
		{
			myProxyElement=*itAlreadyReturned;
			itAlreadyReturned++;	

			if(myReverseIteration)
			{
				SMDS_Iterator<const SMDS_MeshElement*> * it=
					myProxyElement->elementsIterator(myElement->GetType());
				while(it->more())
				{				
					if(it->next()==myElement) return true;
				}
			}
			else return true;
		}
		myProxyElement=NULL;
		return false;
	}
	else return true;
}

const SMDS_MeshElement * SMDS_IteratorOfElements::next()
{
	more();
	const SMDS_MeshElement *e=myProxyElement;
	myProxyElement=NULL;
	return e;
}

SMDS_IteratorOfElements::~SMDS_IteratorOfElements()
{
	delete t1Iterator;
	if(t2Iterator!=NULL) delete t2Iterator;
}