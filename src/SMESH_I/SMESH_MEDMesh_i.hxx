//=============================================================================
// File      : SMESH_MEDMesh_i.hxx
// Project   : SALOME
// Copyright : EDF 2001
//=============================================================================

#ifndef _MED_SMESH_MESH_I_HXX_
#define _MED_SMESH_MESH_I_HXX_

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MED)
#include <string>
#include <vector>
#include <map>

#include "SMESHDS_Mesh.hxx"

#include "SMDS_MeshElement.hxx"
#include "SMDS_MeshNode.hxx"
#include "SMDS_MeshFacesIterator.hxx"
#include "SMDS_MeshEdgesIterator.hxx"
#include "SMDS_MeshNodesIterator.hxx"
#include "SMDS_MeshVolumesIterator.hxx"

#define MED_NBR_GEOMETRIE_MAILLE 15
#define MED_NBR_TYPE 5


class SMESH_Mesh_i;

class SMESH_MEDMesh_i:
  public POA_SALOME_MED::MESH,
  public PortableServer::RefCountServantBase
{
public :

private :

protected:
  // C++ object containing values
  ::SMESH_Mesh_i*       _mesh_i;
  Handle (SMESHDS_Mesh) _meshDS;
  
  string                _meshId;
  bool	                _compte;
  bool	                _creeFamily;
  int		        _indexElts; 
  int		        _indexEnts; 
  int		        _famIdent;

  map <SALOME_MED::medGeometryElement,int> _mapIndToSeqElts;
  Engines::long_array_var _seq_elemId[MED_NBR_GEOMETRIE_MAILLE];

  map <SALOME_MED::medEntityMesh,int> _mapNbTypes;
  map <SALOME_MED::medEntityMesh,int> _mapIndToVectTypes;
  vector<SALOME_MED::medGeometryElement> _TypesId[MED_NBR_GEOMETRIE_MAILLE];

  vector<SALOME_MED::FAMILY_ptr>   _families;
public:

  // Constructors and associated internal methods
  SMESH_MEDMesh_i();
  SMESH_MEDMesh_i(SMESH_Mesh_i* m);
  ~SMESH_MEDMesh_i();

  // IDL Methods 
  char * 	getName() 		throw (SALOME::SALOME_Exception);
  CORBA::Long   getSpaceDimension() 	throw (SALOME::SALOME_Exception);
  CORBA::Long   getMeshDimension() 	throw (SALOME::SALOME_Exception);

  char * 	  	  getCoordinateSystem() 
    throw (SALOME::SALOME_Exception);
  Engines::double_array* getCoordinates(SALOME_MED::medModeSwitch typeSwitch)
    throw (SALOME::SALOME_Exception);
  Engines::string_array* getCoordinatesNames() 
    throw (SALOME::SALOME_Exception);
  Engines::string_array* getCoordinatesUnits()
    throw (SALOME::SALOME_Exception); 
  CORBA::Long            getNumberOfNodes() 
    throw (SALOME::SALOME_Exception);

  CORBA::Long            getNumberOfTypes       (SALOME_MED::medEntityMesh entity) 
    throw   (SALOME::SALOME_Exception);
  SALOME_MED::medGeometryElement_array* getTypes(SALOME_MED::medEntityMesh entity)
    throw   (SALOME::SALOME_Exception);

  CORBA::Long 	   getNumberOfElements (SALOME_MED::medEntityMesh entity,
					SALOME_MED::medGeometryElement geomElement) 
    throw (SALOME::SALOME_Exception);
  Engines::long_array*   getConnectivity     (SALOME_MED::medModeSwitch typeSwitch, 
					      SALOME_MED::medConnectivity mode, 
					      SALOME_MED::medEntityMesh entity, 
					      SALOME_MED::medGeometryElement geomElement)
    throw (SALOME::SALOME_Exception);
  Engines::long_array*   getConnectivityIndex(SALOME_MED::medConnectivity mode, 
					      SALOME_MED::medEntityMesh entity) 
    throw (SALOME::SALOME_Exception);

  CORBA::Long getElementNumber(SALOME_MED::medConnectivity mode, 
			       SALOME_MED::medEntityMesh entity, 
			       SALOME_MED::medGeometryElement type, 
			       const Engines::long_array & connectivity)
    throw (SALOME::SALOME_Exception);

  Engines::long_array*   getReverseConnectivity(SALOME_MED::medConnectivity mode)

    throw  (SALOME::SALOME_Exception);
  Engines::long_array*   getReverseConnectivityIndex(SALOME_MED::medConnectivity mode)
    throw        (SALOME::SALOME_Exception);

  // Family and Group
  CORBA::Long        getNumberOfFamilies (SALOME_MED::medEntityMesh entity)
    throw (SALOME::SALOME_Exception);
  CORBA::Long        getNumberOfGroups   (SALOME_MED::medEntityMesh entity) 
    throw (SALOME::SALOME_Exception);
  SALOME_MED::Family_array* getFamilies  (SALOME_MED::medEntityMesh entity) 
    throw (SALOME::SALOME_Exception);
  SALOME_MED::FAMILY_ptr    getFamily    (SALOME_MED::medEntityMesh entity,
					  CORBA::Long i) 
    throw (SALOME::SALOME_Exception);
  SALOME_MED::Group_array*  getGroups    (SALOME_MED::medEntityMesh entity) 
    throw (SALOME::SALOME_Exception);
  SALOME_MED::GROUP_ptr     getGroup     (SALOME_MED::medEntityMesh entity, 
					  CORBA::Long i) 
    throw (SALOME::SALOME_Exception);
  // 
  SALOME_MED::FIELD_ptr getVolume        (SALOME_MED::SUPPORT_ptr mySupport)
    throw (SALOME::SALOME_Exception);
  SALOME_MED::FIELD_ptr getArea          (SALOME_MED::SUPPORT_ptr mySupport)          
    throw (SALOME::SALOME_Exception);
  SALOME_MED::FIELD_ptr getLength        (SALOME_MED::SUPPORT_ptr mySupport)        
    throw (SALOME::SALOME_Exception);
  SALOME_MED::FIELD_ptr getNormal        (SALOME_MED::SUPPORT_ptr mySupport)        
    throw (SALOME::SALOME_Exception);
  SALOME_MED::FIELD_ptr getBarycenter    (SALOME_MED::SUPPORT_ptr mySupport)    
    throw (SALOME::SALOME_Exception);
  SALOME_MED::FIELD_ptr getNeighbourhood (SALOME_MED::SUPPORT_ptr mySupport) 
    throw (SALOME::SALOME_Exception);


  // Others
  void        addInStudy (SALOMEDS::Study_ptr myStudy, 
			  SALOME_MED::MESH_ptr myIor) 
    throw (SALOME::SALOME_Exception);
  CORBA::Long addDriver  (SALOME_MED::medDriverTypes driverType, 
			  const char* fileName, const char* meshName)
    throw (SALOME::SALOME_Exception);
  void        rmDriver   (CORBA::Long i)	
    throw (SALOME::SALOME_Exception);
  void        read       (CORBA::Long i)
    throw (SALOME::SALOME_Exception);
  void        write      (CORBA::Long i, const char* driverMeshName)
    throw (SALOME::SALOME_Exception);

  //					Cuisine interne
  CORBA::Long getCorbaIndex() throw (SALOME::SALOME_Exception);

  //
  void		calculeNbElts()	  throw (SALOME::SALOME_Exception);
  void		createFamilies()  throw (SALOME::SALOME_Exception);
};

#endif /* _MED_MESH_I_HXX_ */