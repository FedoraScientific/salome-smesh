using namespace std;
//=============================================================================
// File      : SMESH_MEDSupport_i.cxx
// Project   : SALOME
// Copyright : EDF 2002
//=============================================================================

#include "SMESH_MEDSupport_i.hxx"
#include "utilities.h"
#include "Utils_CorbaException.hxx"

#include <TopoDS_Iterator.hxx>
#include "SMESHDS_Mesh.hxx"
#include "SMDS_MapIteratorOfExtendedMap.hxx"

#include "SMESH_subMesh.hxx"
#include "SMESH_Mesh_i.hxx"
#include "SMESH_subMesh_i.hxx"

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
SMESH_MEDSupport_i::SMESH_MEDSupport_i()
{
  BEGIN_OF("Default Constructor SMESH_MEDSupport_i");
  END_OF("Default Constructor SMESH_MEDSupport_i");
}

//=============================================================================
/*!
 * Constructor
 */
//=============================================================================
SMESH_MEDSupport_i::SMESH_MEDSupport_i(SMESH_subMesh_i* sm,
				       string name, 
				       string description,
				       SALOME_MED::medEntityMesh entity) :
  _subMesh_i(sm),		
  _name(name),
  _description(description),
  _entity(entity),
  _seqNumber(false),
  _seqLength(0)
{
  BEGIN_OF("Constructor SMESH_MEDSupport_i");

  _meshDS = _subMesh_i->_mesh_i->GetImpl().GetMeshDS();

  int subMeshId = _subMesh_i->GetId();

  MESSAGE ( " subMeshId " << subMeshId )
  if (_subMesh_i->_mesh_i->_mapSubMesh.find(subMeshId) != _subMesh_i->_mesh_i->_mapSubMesh.end()) {
    ::SMESH_subMesh* subMesh = _subMesh_i->_mesh_i->_mapSubMesh[subMeshId];
    
    _subMeshDS = subMesh->GetSubMeshDS();
  }

  if ( _entity == SALOME_MED::MED_NODE) 
    {
      _numberOfGeometricType = 1 ;
      _geometricType         = new SALOME_MED::medGeometryElement[1] ;
      _geometricType[0]      = SALOME_MED::MED_NONE ;
    }
  else
    {
      MESSAGE("Pas implemente dans cette version");
      THROW_SALOME_CORBA_EXCEPTION("Seules les familles de noeuds sont implementees ", \
				   SALOME::BAD_PARAM);
    }

  END_OF("Constructor SMESH_MEDSupport_i");
}
//=============================================================================
/*!
 * Constructor
 */
//=============================================================================
SMESH_MEDSupport_i::SMESH_MEDSupport_i(const SMESH_MEDSupport_i &s) :
  _subMesh_i(s._subMesh_i),
  _name(s._name),
  _description(s._description),
  _entity(s._entity),
  _seqNumber(false),
  _seqLength(0)
{
  BEGIN_OF("Constructor SMESH_MEDSupport_i");

  _meshDS = _subMesh_i->_mesh_i->GetImpl().GetMeshDS();

  int subMeshId = _subMesh_i->GetId();
  if (_subMesh_i->_mesh_i->_mapSubMesh.find(subMeshId) != _subMesh_i->_mesh_i->_mapSubMesh.end()) {
    ::SMESH_subMesh* subMesh = _subMesh_i->_mesh_i->_mapSubMesh[subMeshId];
    _subMeshDS = subMesh->GetSubMeshDS();
  }

  END_OF("Constructor SMESH_MEDSupport_i");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================

SMESH_MEDSupport_i::~SMESH_MEDSupport_i()
{
}
//=============================================================================
/*!
 * CORBA: Accessor for Corba Index 
 */
//=============================================================================

CORBA::Long SMESH_MEDSupport_i::getCorbaIndex()
  throw (SALOME::SALOME_Exception)
{
  if (_subMeshDS==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				 SALOME::INTERNAL_ERROR);
  MESSAGE("Not implemented for SMESH_i");
  THROW_SALOME_CORBA_EXCEPTION("Not Implemented ", \
			       SALOME::BAD_PARAM);

}

//=============================================================================
/*!
 * CORBA: Accessor for Name 
 */
//=============================================================================

char * SMESH_MEDSupport_i::getName() 	
  throw (SALOME::SALOME_Exception)
{
  if (_subMeshDS.IsNull())
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				 SALOME::INTERNAL_ERROR);
  return CORBA::string_dup(_name.c_str());

}

//=============================================================================
/*!
 * CORBA: Accessor for Description 
 */
//=============================================================================

char*  SMESH_MEDSupport_i::getDescription()
  throw (SALOME::SALOME_Exception)
{
  if (_subMeshDS.IsNull())
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				 SALOME::INTERNAL_ERROR);
  return CORBA::string_dup(_description.c_str());
}

//=============================================================================
/*!
 * CORBA: Accessor for Mesh 
 */
//=============================================================================

SALOME_MED::MESH_ptr SMESH_MEDSupport_i::getMesh()
  throw (SALOME::SALOME_Exception)
{
  if (_subMeshDS.IsNull())
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				 SALOME::INTERNAL_ERROR);
  
  return _subMesh_i->_mesh_i->GetMEDMesh();
}

//=============================================================================
/*!
 * CORBA: boolean indicating if support concerns all elements 
 */
//=============================================================================

CORBA::Boolean SMESH_MEDSupport_i::isOnAllElements()
  throw (SALOME::SALOME_Exception)
{
  if (_subMeshDS.IsNull())
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				 SALOME::INTERNAL_ERROR);
  if (_seqNumber == false)
  {
    if (_entity != SALOME_MED::MED_NONE)
    {
  	int i = 0;
 	const SMDS_MapOfMeshElement& myNodesMap = _subMeshDS->GetNodes();
  	for (SMDS_MapIteratorOfExtendedMap it(myNodesMap); it.More(); it.Next())
        {
      		const Handle(SMDS_MeshElement)& elem = myNodesMap.FindID(it.Key()->GetID());
      		const Handle(SMDS_MeshNode)& node = _meshDS->GetNode(1,elem);
		i ++;
	}
	_seqLength=i;
	_seqNumber=true;
    }
    else
    {
      MESSAGE("Only Node Families are implemented ");
      THROW_SALOME_CORBA_EXCEPTION("Not implemented Yet ", \
                                   SALOME::BAD_PARAM);
    }
   }
  try
    {
     if (_seqLength == _meshDS->NbNodes())
       _isOnAllElements=true;
     else
       _isOnAllElements=false;
  }
  catch(...)
  {
      MESSAGE("unable to acces related Mesh");
      THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
				   SALOME::INTERNAL_ERROR);
   };
  return _isOnAllElements;
}

//=============================================================================
/*!
 * CORBA: Accessor for type of support's entity 
 */
//=============================================================================

SALOME_MED::medEntityMesh SMESH_MEDSupport_i::getEntity() 
  throw (SALOME::SALOME_Exception)
{
  if (_subMeshDS.IsNull())
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				 SALOME::INTERNAL_ERROR);
  return _entity;
}

//=============================================================================
/*!
 * CORBA: Accessor for types of geometry elements 
 */
//=============================================================================

SALOME_MED::medGeometryElement_array * SMESH_MEDSupport_i::getTypes() 
  throw (SALOME::SALOME_Exception)
{
  if (_subMeshDS.IsNull())
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				 SALOME::INTERNAL_ERROR);
  SALOME_MED::medGeometryElement_array_var myseq = new SALOME_MED::medGeometryElement_array;
  try
    {
      int mySeqLength = _numberOfGeometricType;
      myseq->length(mySeqLength);
      for (int i=0;i<mySeqLength;i++)
	{
	  myseq[i]=_geometricType[i];
	}
    }
  catch(...)
    {
      MESSAGE("Exception lors de la recherche des differents types");
      THROW_SALOME_CORBA_EXCEPTION("Unable to acces Support Types", \
				   SALOME::INTERNAL_ERROR);
    }
  return myseq._retn();
}

//=============================================================================
/*!
 * CORBA: Number of different types of geometry elements
 *        existing in the support
 */
//=============================================================================
CORBA::Long SMESH_MEDSupport_i::getNumberOfElements(SALOME_MED::medGeometryElement geomElement) 
  throw (SALOME::SALOME_Exception)
{
  if (_subMeshDS.IsNull())
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				 SALOME::INTERNAL_ERROR);
  return  _numberOfGeometricType;

}

//=============================================================================
/*!
 * CORBA: get Nodes 
 */
//=============================================================================

Engines::long_array *  SMESH_MEDSupport_i::getNumber(SALOME_MED::medGeometryElement geomElement) 
  throw (SALOME::SALOME_Exception)
{
  if (_subMeshDS.IsNull())
    THROW_SALOME_CORBA_EXCEPTION("No associated Support", \
				 SALOME::INTERNAL_ERROR);


  // A changer s'il ne s agit plus seulement de famille de noeuds
  if (geomElement != SALOME_MED::MED_NONE)
    THROW_SALOME_CORBA_EXCEPTION("Not implemented", \
				 SALOME::BAD_PARAM);

  Engines::long_array_var myseq= new Engines::long_array;

  int i =0;
  const SMDS_MapOfMeshElement& myNodesMap = _subMeshDS->GetNodes();
  for (SMDS_MapIteratorOfExtendedMap it(myNodesMap); it.More(); it.Next())
    {
      const Handle(SMDS_MeshElement)& elem = myNodesMap.FindID(it.Key()->GetID());
      const Handle(SMDS_MeshNode)& node = _meshDS->GetNode(1,elem);
      myseq->length(i+1);
      myseq[i]=node->GetID();
SCRUTE(node->GetID());
SCRUTE(myseq[i]);
      i++;
    };

SCRUTE(myseq->length());
MESSAGE("yyyyy");
  return myseq._retn();
	
}
//=============================================================================
/*!
 * CORBA: Global Nodes Index (optionnaly designed by the user)
 * CORBA:  ??????????????????????????????
 */
//=============================================================================

Engines::long_array *  SMESH_MEDSupport_i::getNumberIndex()
  throw (SALOME::SALOME_Exception)
{
  MESSAGE("Not implemented for SMESH_i");
  THROW_SALOME_CORBA_EXCEPTION("Not Implemented", \
			       SALOME::BAD_PARAM);
  return 0;
}

//=============================================================================
/*!
 * CORBA: Array containing indexes for elements included in the support  
 */
//=============================================================================

CORBA::Long SMESH_MEDSupport_i::getNumberOfGaussPoints(SALOME_MED::medGeometryElement geomElement)
  throw (SALOME::SALOME_Exception)
{
  MESSAGE("Not implemented for SMESH_i");
  return 0;
}
