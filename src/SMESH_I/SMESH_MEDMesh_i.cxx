using namespace std;
//=============================================================================
// File      : SMESH_MEDMesh_i.cxx
// Project   : SALOME
// Copyright : EDF 2001
//=============================================================================

#include "SMESH_MEDMesh_i.hxx"
#include "SMESH_Mesh_i.hxx"

#include "SMESHDS_Mesh.hxx"
#include "SMESHDS_SubMesh.hxx"

#include "SMDS_MapIteratorOfExtendedMap.hxx"
#include "SMDS_MapOfMeshElement.hxx"

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_MapOfShape.hxx>

#include "utilities.h"
#include "Utils_CorbaException.hxx"

#include "SMESH_MEDSupport_i.hxx"
#include "SMESH_MEDFamily_i.hxx"

# include "Utils_ORB_INIT.hxx"
# include "Utils_SINGLETON.hxx"
extern "C"
{
#include <stdio.h>
}


//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
// PN Est-ce un const ?
SMESH_MEDMesh_i::SMESH_MEDMesh_i()
{
  BEGIN_OF("Default Constructor SMESH_MEDMesh_i");
  END_OF("Default Constructor SMESH_MEDMesh_i");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
SMESH_MEDMesh_i::~SMESH_MEDMesh_i()
{
}
//=============================================================================
/*!
 * Constructor
 */
//=============================================================================
SMESH_MEDMesh_i::SMESH_MEDMesh_i( ::SMESH_Mesh_i* m_i ) :_meshId(""),
							 _compte(false),
							 _creeFamily(false),
							 _famIdent(0),
							 _indexElts(0), 
							 _indexEnts(0) 
{
  _mesh_i = m_i;
  _meshDS = _mesh_i->GetImpl().GetMeshDS();

  BEGIN_OF("Constructor SMESH_MEDMesh_i");
  END_OF("Constructor SMESH_MEDMesh_i");
}

//=============================================================================
/*!
 * CORBA: Accessor for Name
 */
//=============================================================================
char * SMESH_MEDMesh_i::getName() 	
  throw (SALOME::SALOME_Exception)
{
  if (_meshDS == NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
				 SALOME::INTERNAL_ERROR);

  try
    {
      // A COMPLETER PAR LE NOM DU MAILLAGE
      //return CORBA::string_dup(_mesh_i->getName().c_str());
      return CORBA::string_dup("toto");
    }
  catch(...)
    {
      MESSAGE("Exception en accedant au nom");
      THROW_SALOME_CORBA_EXCEPTION("Unable to acces Mesh C++ Object"\
				   ,SALOME::INTERNAL_ERROR);
    }
}
//=============================================================================
/*!
 * CORBA: Accessor for corbaindex cuisine interne 
 */
//=============================================================================
CORBA::Long SMESH_MEDMesh_i::getCorbaIndex() 
  throw (SALOME::SALOME_Exception)
{
  MESSAGE("Non Implemente");
  THROW_SALOME_CORBA_EXCEPTION("Unimplemented Method"\
			       ,SALOME::BAD_PARAM);
}
//=============================================================================
/*!
 * CORBA: Accessor for Space Dimension
 */
//=============================================================================
CORBA::Long SMESH_MEDMesh_i::getSpaceDimension() 
  throw (SALOME::SALOME_Exception)
{
  // PN : Il semblerait que la dimension soit fixee a 3
  if (_mesh_i == 0)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
				 SALOME::INTERNAL_ERROR);
  return 3;
}
//=============================================================================
/*!
 * CORBA: Accessor for Mesh Dimension
 */
//=============================================================================
CORBA::Long SMESH_MEDMesh_i::getMeshDimension()
  throw (SALOME::SALOME_Exception)
{
  if (_mesh_i == 0)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
				 SALOME::INTERNAL_ERROR);
  // PN : Il semblerait que la dimension soit fixee a 3
  if (_mesh_i == 0)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
				 SALOME::INTERNAL_ERROR);
  return 3;
}
//=============================================================================
/*!
 * CORBA: Accessor for Coordinates System
 */
//=============================================================================
char * SMESH_MEDMesh_i::getCoordinateSystem()
  throw (SALOME::SALOME_Exception)
{
  if (_mesh_i == 0)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
				 SALOME::INTERNAL_ERROR);
  // PN : En dur. Non encore prevu
  try
    {
      string systcoo="CARTESIEN";
      return CORBA::string_dup(systcoo.c_str());
    }
  catch(...)
    {
      MESSAGE("Exception en accedant au maillage");
      THROW_SALOME_CORBA_EXCEPTION("Unable to acces Mesh C++ Object"\
				   ,SALOME::INTERNAL_ERROR);
    }
}
//=============================================================================
/*!
 * CORBA: Accessor for Coordinates
 */
//=============================================================================
Engines::double_array * SMESH_MEDMesh_i::getCoordinates(SALOME_MED::medModeSwitch typeSwitch)
  throw (SALOME::SALOME_Exception)
{
  if (_mesh_i == 0)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
				 SALOME::INTERNAL_ERROR);
  Engines::double_array_var myseq = new Engines::double_array;
  try
    {
      // PN  : En dur
      int spaceDimension=3;
      int nbNodes=_meshDS->NbNodes();
      SCRUTE(nbNodes);
      myseq->length(nbNodes*spaceDimension);
      int i = 0;
      SMDS_MeshNodesIterator itNodes(_meshDS);
      for (;itNodes.More();itNodes.Next()) 
	{

	  const Handle(SMDS_MeshElement)& elem = itNodes.Value();
	  const Handle(SMDS_MeshNode)& node =_meshDS->GetNode(1,elem);

	  if (typeSwitch == SALOME_MED::MED_FULL_INTERLACE)
	    {
	      myseq[i*3]=node->X();
	      myseq[i*3+1]=node->Y();
	      myseq[i*3+2]=node->Z();
	      SCRUTE(myseq[i*3]);
	      SCRUTE(myseq[i*3+1]);
	      SCRUTE(myseq[i*3+2]);
	    }
	  else
	    {
	      ASSERT(typeSwitch == SALOME_MED::MED_NO_INTERLACE);
	      myseq[i]=node->X();
	      myseq[i+nbNodes]=node->Y();
	      myseq[i+(nbNodes*2)]=node->Z();
	      SCRUTE(myseq[i]);
	      SCRUTE(myseq[i+nbNodes]);
	      SCRUTE(myseq[i+(nbNodes*2)]);
	    }
	  i++;
	}
    }
  catch(...)
    {       
      MESSAGE("Exception en accedant aux coordonnees");
      THROW_SALOME_CORBA_EXCEPTION("Unable to acces Mesh C++ Object"\
				   ,SALOME::INTERNAL_ERROR);
    }
  return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for Coordinates Names
 */
//=============================================================================
Engines::string_array  * SMESH_MEDMesh_i::getCoordinatesNames() 
  throw (SALOME::SALOME_Exception)
{
  if (_mesh_i == 0)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
				 SALOME::INTERNAL_ERROR);
  Engines::string_array_var myseq = new Engines::string_array;
  try
    {
      // PN : en dur
      int spaceDimension=3;
      myseq->length(spaceDimension);
      myseq[0]=CORBA::string_dup("x");
      myseq[1]=CORBA::string_dup("y");
      myseq[2]=CORBA::string_dup("z");
    }
  catch(...)
    {       
      MESSAGE("Exception en accedant aux noms des coordonnees");
      THROW_SALOME_CORBA_EXCEPTION("Unable to acces Mesh C++ Object"\
				   ,SALOME::INTERNAL_ERROR);
    }
  return myseq._retn();

}
//=============================================================================
/*!
 * CORBA: Accessor for Coordinates Units
 */
//=============================================================================
Engines::string_array *  SMESH_MEDMesh_i::getCoordinatesUnits()
  throw (SALOME::SALOME_Exception)
{
  if (_mesh_i == 0)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
				 SALOME::INTERNAL_ERROR);
  Engines::string_array_var myseq = new Engines::string_array;
  try
    {
      // PN : en dur
      int spaceDimension=3;
      myseq->length(spaceDimension);
      myseq[0]=CORBA::string_dup("m");
      myseq[1]=CORBA::string_dup("m");
      myseq[2]=CORBA::string_dup("m");
    }
  catch(...)
    {       
      MESSAGE("Exception en accedant aux unites des coordonnees");
      THROW_SALOME_CORBA_EXCEPTION("Unable to acces Mesh C++ Object"\
				   ,SALOME::INTERNAL_ERROR);
    }
  return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for Number of Nodes
 */
//=============================================================================
CORBA::Long  SMESH_MEDMesh_i::getNumberOfNodes() 
  throw (SALOME::SALOME_Exception)
{
  if (_mesh_i == 0)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
				 SALOME::INTERNAL_ERROR);
  try
    {
      return _meshDS->NbNodes();
    }
  catch(...)
    {       
      MESSAGE("Exception en accedant au nombre de noeuds");
      THROW_SALOME_CORBA_EXCEPTION("Unable to acces Mesh C++ Object"\
				   ,SALOME::INTERNAL_ERROR);
    }
}
//=============================================================================
/*!
 * CORBA: Accessor for number of Types
 */
//=============================================================================
CORBA::Long SMESH_MEDMesh_i::getNumberOfTypes(SALOME_MED::medEntityMesh entity) 
  throw (SALOME::SALOME_Exception)
{
  if (_mesh_i == 0)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
				 SALOME::INTERNAL_ERROR);
  try
    {
      if (! _compte)
	calculeNbElts();
      int retour = 0; 
      if ( _mapNbTypes.find(entity)!=_mapNbTypes.end())
	retour=_mapNbTypes[entity];
      return retour;
    }
  catch(...)
    {       
      MESSAGE("Exception en accedant au nombre de Types");
      THROW_SALOME_CORBA_EXCEPTION("Unable to acces Mesh C++ Object"\
				   ,SALOME::INTERNAL_ERROR);
    }
}

//=============================================================================
/*!
 * CORBA: Accessor for existing geometry element types 
 * 	  Not implemented for MED_ALL_ENTITIES
 */
//=============================================================================
SALOME_MED::medGeometryElement_array * SMESH_MEDMesh_i::getTypes (SALOME_MED::medEntityMesh entity) 
  throw (SALOME::SALOME_Exception)
{
  if (_mesh_i == 0)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
				 SALOME::INTERNAL_ERROR);
  if (entity ==SALOME_MED::MED_ALL_ENTITIES)
    THROW_SALOME_CORBA_EXCEPTION("Not implemented for MED_ALL_ENTITIES",\
				 SALOME::BAD_PARAM);
  if (! _compte)
    calculeNbElts();
  SALOME_MED::medGeometryElement_array_var myseq = 
    new SALOME_MED::medGeometryElement_array;
  try
    {
      if ( _mapNbTypes.find(entity) ==_mapNbTypes.end())
	THROW_SALOME_CORBA_EXCEPTION("No Such Entity in the mesh"\
				     ,SALOME::BAD_PARAM);
      int nbTypes=_mapNbTypes[entity];

      myseq->length(nbTypes);
		
      if ( _mapIndToVectTypes.find(entity) ==_mapIndToVectTypes.end())
	THROW_SALOME_CORBA_EXCEPTION("No Such Entity in the mesh"\
				     ,SALOME::INTERNAL_ERROR);

      int index=_mapIndToVectTypes[entity];
      ASSERT( _TypesId[index].size()!=0);
      int i=0;
      vector<SALOME_MED::medGeometryElement>::iterator it;
      for (it=_TypesId[index].begin();it!=_TypesId[index].end();it++)
	{
	  myseq[i++]=*it;
	};
    }
  catch(...)
    {       
      MESSAGE("Exception en accedant aux differents types");
      THROW_SALOME_CORBA_EXCEPTION("Unable to acces Mesh C++ Object"\
				   ,SALOME::INTERNAL_ERROR);
    }
  return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Returns number of elements of type medGeometryElement
 *	  Not implemented for MED_ALL_ELEMENTS 
 * 	  implemented for MED_ALL_ENTITIES
 *
 * Dans cette implementation, il n est pas prevu de tenir compte du entity
 * qui ne doit pas pouvoir avoir deux valeurs differentes pour un geomElement 
 */
//=============================================================================
CORBA::Long SMESH_MEDMesh_i::getNumberOfElements(SALOME_MED::medEntityMesh entity, 
						 SALOME_MED::medGeometryElement geomElement)
  throw (SALOME::SALOME_Exception)
{
  if (_mesh_i == 0)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
				 SALOME::INTERNAL_ERROR);
  if (geomElement == SALOME_MED::MED_ALL_ELEMENTS)
    THROW_SALOME_CORBA_EXCEPTION("Not implemented for MED_ALL_ELEMENTS", \
				 SALOME::BAD_PARAM);
  if (! _compte)
    calculeNbElts();

  try
    {
      int retour = 0; 
      if ( _mapIndToSeqElts.find(geomElement)!=_mapIndToSeqElts.end())
	{ 
	  int index = _mapIndToSeqElts[geomElement];
	  retour=_seq_elemId[index]->length();
	}
      return retour;
    }
  catch(...)
    {
      MESSAGE("Exception en accedant au nombre d �lements");
      THROW_SALOME_CORBA_EXCEPTION("Unable to acces Mesh C++ Object"\
				   ,SALOME::INTERNAL_ERROR);
    }
}
//=============================================================================
/*!
 * CORBA: Accessor for connectivities
 */
//=============================================================================
Engines::long_array *  SMESH_MEDMesh_i::getConnectivity(SALOME_MED::medModeSwitch typeSwitch,
							SALOME_MED::medConnectivity mode, 
							SALOME_MED::medEntityMesh entity, 
							SALOME_MED::medGeometryElement geomElement)
  throw (SALOME::SALOME_Exception)
{
  if (_mesh_i == 0)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
				 SALOME::INTERNAL_ERROR);
  if (mode != SALOME_MED::MED_NODAL)
    THROW_SALOME_CORBA_EXCEPTION("Not Implemented",\
				 SALOME::BAD_PARAM);
  if (typeSwitch == SALOME_MED::MED_NO_INTERLACE)
    THROW_SALOME_CORBA_EXCEPTION("Not Yet Implemented",\
				 SALOME::BAD_PARAM);
  if (! _compte)
    calculeNbElts();

  // Faut-il renvoyer un pointeur vide ???
  if ( _mapIndToSeqElts.find(geomElement)!=_mapIndToSeqElts.end())
    THROW_SALOME_CORBA_EXCEPTION("No Such Element in the mesh"\
				 ,SALOME::BAD_PARAM);

  int index=_mapIndToSeqElts[geomElement];
  return _seq_elemId[index]._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for connectivities
 */
//=============================================================================
Engines::long_array* SMESH_MEDMesh_i::getConnectivityIndex(SALOME_MED::medConnectivity mode, 
							   SALOME_MED::medEntityMesh entity) 
  throw (SALOME::SALOME_Exception)
{
  MESSAGE("Pas Implemente dans SMESH");
  THROW_SALOME_CORBA_EXCEPTION("Unimplemented Method"\
			       ,SALOME::BAD_PARAM);
  return 0;
}
//=============================================================================
/*!
 * CORBA: Find an element corresponding to the given connectivity
 */
//=============================================================================
CORBA::Long SMESH_MEDMesh_i::getElementNumber(SALOME_MED::medConnectivity mode,
					      SALOME_MED::medEntityMesh entity,
					      SALOME_MED::medGeometryElement type,
					      const Engines::long_array & connectivity
) 
  throw (SALOME::SALOME_Exception)
{
  const char * LOC = "getElementNumber ";
  MESSAGE(LOC<<"Pas Implemente dans SMESH");
  THROW_SALOME_CORBA_EXCEPTION("Unimplemented Method"\
			       ,SALOME::BAD_PARAM);
  return -1;
}
//=============================================================================
/*!
 * CORBA: Accessor for Ascendant connectivities
 * not implemented for MED_ALL_ENTITIES and MED_MAILLE
 */
//=============================================================================
Engines::long_array* SMESH_MEDMesh_i::getReverseConnectivity(SALOME_MED::medConnectivity mode)
  throw (SALOME::SALOME_Exception)
{
  MESSAGE("Pas Implemente dans SMESH");
  THROW_SALOME_CORBA_EXCEPTION("Unimplemented Method"\
			       ,SALOME::BAD_PARAM);
  return 0;
}
//=============================================================================
/*!
 * CORBA: Accessor for connectivities
 */
//=============================================================================
Engines::long_array* SMESH_MEDMesh_i::getReverseConnectivityIndex(SALOME_MED::medConnectivity mode)
  throw (SALOME::SALOME_Exception)
{
  MESSAGE("Pas Implemente dans SMESH");
  THROW_SALOME_CORBA_EXCEPTION("Unimplemented Method"\
			       ,SALOME::BAD_PARAM);
  return 0;
}
//=============================================================================
/*!
 * CORBA: Returns number of families within the mesh
 */
//=============================================================================
CORBA::Long SMESH_MEDMesh_i::getNumberOfFamilies(SALOME_MED::medEntityMesh entity)
  throw (SALOME::SALOME_Exception)
{
  if (_creeFamily == false)
    createFamilies();
  if (_mesh_i == 0)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
				 SALOME::INTERNAL_ERROR);
  return _families.size();
}
//=============================================================================
/*!
 * CORBA: Returns number of groups within the mesh
 */
//=============================================================================
CORBA::Long SMESH_MEDMesh_i::getNumberOfGroups(SALOME_MED::medEntityMesh entity) 
  throw (SALOME::SALOME_Exception)
{
  if (_mesh_i == 0)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
				 SALOME::INTERNAL_ERROR);
  MESSAGE(" Pas d implementation des groupes dans SMESH");
  return 0;
}
//=============================================================================
/*!
 * CORBA: Returns references for families within the mesh
 */
//=============================================================================
SALOME_MED::Family_array * SMESH_MEDMesh_i::getFamilies(SALOME_MED::medEntityMesh entity) 
  throw (SALOME::SALOME_Exception)
{
  if (_creeFamily == false)
    createFamilies();
  if (_mesh_i == 0)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
				 SALOME::INTERNAL_ERROR);
  SALOME_MED::Family_array_var myseq = new SALOME_MED::Family_array;
  int nbfam= _families.size();
  myseq->length(nbfam);
  int i = 0;
  vector<SALOME_MED::FAMILY_ptr>::iterator it;
  for (it=_families.begin();it!=_families.end();it++)
    {
      myseq[i++]=*it;
    };
  return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Returns references for family i within the mesh
 */
//=============================================================================
SALOME_MED::FAMILY_ptr SMESH_MEDMesh_i::getFamily(SALOME_MED::medEntityMesh entity,
						  CORBA::Long i) 
  throw (SALOME::SALOME_Exception)
{
  if (_creeFamily == false)
    createFamilies();
  if (_mesh_i == 0)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
				 SALOME::INTERNAL_ERROR);

  SCRUTE(_families[i]->getName());
  MESSAGE ( " SMESH_MEDMesh_i::getFamily " << i )
  return _families[i];
}
//=============================================================================
/*!
 * CORBA: Returns references for groups within the mesh
 */
//=============================================================================
SALOME_MED::Group_array *  SMESH_MEDMesh_i::getGroups(SALOME_MED::medEntityMesh entity) 
  throw (SALOME::SALOME_Exception)
{
  if (_mesh_i == 0)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
				 SALOME::INTERNAL_ERROR);
  MESSAGE(" Pas d implementation des groupes dans SMESH");
  THROW_SALOME_CORBA_EXCEPTION("No group implementation"\
			       ,SALOME::BAD_PARAM);
}
//=============================================================================
/*!
 * CORBA: Returns references for group i within the mesh
 */
//=============================================================================
SALOME_MED::GROUP_ptr SMESH_MEDMesh_i::getGroup(SALOME_MED::medEntityMesh entity,
						CORBA::Long i) 
  throw (SALOME::SALOME_Exception)
{
  if (_mesh_i == 0)
    THROW_SALOME_CORBA_EXCEPTION("No associated Mesh", \
				 SALOME::INTERNAL_ERROR);
  MESSAGE(" Pas d implementation des groupes dans SMESH");
  THROW_SALOME_CORBA_EXCEPTION("No group implementation"\
			       ,SALOME::BAD_PARAM);
}
//=============================================================================
/*!
 * CORBA: 
 */
//=============================================================================
SALOME_MED::FIELD_ptr SMESH_MEDMesh_i::getVolume(SALOME_MED::SUPPORT_ptr mySupport)
  throw (SALOME::SALOME_Exception)
{
  MESSAGE("Pas Implemente dans SMESH");
  THROW_SALOME_CORBA_EXCEPTION("Unimplemented Method"\
			       ,SALOME::BAD_PARAM);
  return 0;
}
//=============================================================================
/*!
 * CORBA: 
 */
//=============================================================================
SALOME_MED::FIELD_ptr SMESH_MEDMesh_i::getArea(SALOME_MED::SUPPORT_ptr mySupport)
  throw (SALOME::SALOME_Exception)
{
  MESSAGE("Pas Implemente dans SMESH");
  THROW_SALOME_CORBA_EXCEPTION("Unimplemented Method"\
			       ,SALOME::BAD_PARAM);
  return 0;
}
//=============================================================================
/*!
 * CORBA: 
 */
//=============================================================================
SALOME_MED::FIELD_ptr SMESH_MEDMesh_i::getLength(SALOME_MED::SUPPORT_ptr mySupport)
  throw (SALOME::SALOME_Exception)
{
  MESSAGE("Pas Implemente dans SMESH");
  THROW_SALOME_CORBA_EXCEPTION("Unimplemented Method"\
			       ,SALOME::BAD_PARAM);
  return 0;
}
//=============================================================================
/*!
 * CORBA: 
 */
//=============================================================================
SALOME_MED::FIELD_ptr SMESH_MEDMesh_i::getNormal(SALOME_MED::SUPPORT_ptr mySupport)
  throw (SALOME::SALOME_Exception)
{
  MESSAGE("Pas Implemente dans SMESH");
  THROW_SALOME_CORBA_EXCEPTION("Unimplemented Method"\
			       ,SALOME::BAD_PARAM);
  return 0;
}
//=============================================================================
/*!
 * CORBA: 
 */
//=============================================================================
SALOME_MED::FIELD_ptr SMESH_MEDMesh_i::getBarycenter(SALOME_MED::SUPPORT_ptr mySupport)
  throw (SALOME::SALOME_Exception)
{
  MESSAGE("Pas Implemente dans SMESH");
  THROW_SALOME_CORBA_EXCEPTION("Unimplemented Method"\
			       ,SALOME::BAD_PARAM);
  return 0;
}
//=============================================================================
/*!
 * CORBA: 
 */
//=============================================================================
SALOME_MED::FIELD_ptr SMESH_MEDMesh_i::getNeighbourhood(SALOME_MED::SUPPORT_ptr mySupport)
  throw (SALOME::SALOME_Exception)
{
  MESSAGE("Non Implemente");
  THROW_SALOME_CORBA_EXCEPTION("Unimplemented Method"\
			       ,SALOME::BAD_PARAM);
  return 0;
}
//=============================================================================
/*!
 * CORBA: add the Mesh in the StudyManager 
 * PN Pas Implemente
 */
//=============================================================================
void SMESH_MEDMesh_i::addInStudy(SALOMEDS::Study_ptr myStudy,
				 SALOME_MED::MESH_ptr myIor)
  throw (SALOME::SALOME_Exception)
{
  BEGIN_OF("MED_Mesh_i::addInStudy");
  if ( _meshId != "" )
    {
      MESSAGE("Mesh already in Study");
      THROW_SALOME_CORBA_EXCEPTION("Mesh already in Study", \
				   SALOME::BAD_PARAM);
    };

  /*
    SALOMEDS::StudyBuilder_var myBuilder = myStudy->NewBuilder();

    // Create SComponent labelled 'MED' if it doesn't already exit
    SALOMEDS::SComponent_var medfather = myStudy->FindComponent("MED");
    if ( CORBA::is_nil(medfather) ) 
    {
    MESSAGE("Add Component MED");
    medfather = myBuilder->NewComponent("MED");
    //myBuilder->AddAttribute (medfather,SALOMEDS::Name,"MED");
    SALOMEDS::AttributeName_var aName = SALOMEDS::AttributeName::_narrow(
                      myBuilder->FindOrCreateAttribute(medfather, "AttributeName"));
    aName->SetValue("MED");

    myBuilder->DefineComponentInstance(medfather,myIor);

    } ;

    MESSAGE("Add a mesh Object under MED");
    myBuilder->NewCommand();
    SALOMEDS::SObject_var newObj = myBuilder->NewObject(medfather);

    ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
    ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting()) ;
    CORBA::ORB_var &orb = init(0,0);
    string iorStr = orb->object_to_string(myIor);
    //myBuilder->AddAttribute(newObj,SALOMEDS::IOR,iorStr.c_str());
    SALOMEDS::AttributeIOR_var aIOR = SALOMEDS::AttributeIOR::_narrow(
                      myBuilder->FindOrCreateAttribute(newObj, "AttributeIOR"));
    aIOR->SetValue(iorStr.c_str());

    //myBuilder->AddAttribute(newObj,SALOMEDS::Name,_mesh_i->getName().c_str());
    SALOMEDS::AttributeName_var aName = SALOMEDS::AttributeName::_narrow(
                      myBuilder->FindOrCreateAttribute(newObj, "AttributeName"));
    aName->SetValue(_mesh_i->getName().c_str());

    _meshId = newObj->GetID();
    myBuilder->CommitCommand();

  */
  END_OF("Mesh_i::addInStudy(SALOMEDS::Study_ptr myStudy)");
}
//=============================================================================
/*!
 * CORBA: write mesh in a med file
 */
//=============================================================================
void SMESH_MEDMesh_i::write(CORBA::Long i, const char* driverMeshName) 	     
  throw (SALOME::SALOME_Exception)
{
  MESSAGE("Non Implemente");
  THROW_SALOME_CORBA_EXCEPTION("Unimplemented Method"\
			       ,SALOME::BAD_PARAM);
}
//=============================================================================
/*!
 * CORBA: read mesh in a med file
 */
//=============================================================================
void SMESH_MEDMesh_i::read(CORBA::Long i) 	     
  throw (SALOME::SALOME_Exception)
{
  MESSAGE("Non Implemente");
  THROW_SALOME_CORBA_EXCEPTION("Unimplemented Method"\
			       ,SALOME::BAD_PARAM);
}
//=============================================================================
/*!
 * CORBA : release driver
 */
//=============================================================================
void SMESH_MEDMesh_i::rmDriver(CORBA::Long i) 	     
  throw (SALOME::SALOME_Exception)
{
  MESSAGE("Non Implemente");
  THROW_SALOME_CORBA_EXCEPTION("Unimplemented Method"\
			       ,SALOME::BAD_PARAM);
}
//=============================================================================
/*!
 * CORBA : attach driver
 */
//=============================================================================
CORBA::Long SMESH_MEDMesh_i::addDriver(SALOME_MED::medDriverTypes driverType, 
				       const char* fileName, const char* meshName)
  throw (SALOME::SALOME_Exception)
{
  MESSAGE("Non Implemente");
  THROW_SALOME_CORBA_EXCEPTION("Unimplemented Method"\
			       ,SALOME::BAD_PARAM);
  return 0;
}
//=============================================================================
/*!
 * Calcule le Nb d'elements par entite geometrique
 */
//=============================================================================
void SMESH_MEDMesh_i::calculeNbElts()
  throw (SALOME::SALOME_Exception)
{
  if (! _compte)
    {
      _compte=true;

      _mapNbTypes[SALOME_MED::MED_NODE]=1;
      // On compte les aretes MED_SEG2 ou MED_SEG3
      // On range les elements dans  les vecteurs correspondants 

      _mapIndToSeqElts[SALOME_MED::MED_SEG2]=_indexElts++;
      _mapIndToSeqElts[SALOME_MED::MED_SEG3]=_indexElts++;
      _mapIndToVectTypes[SALOME_MED::MED_EDGE]=_indexEnts++;

      int trouveSeg2=0; 
      int trouveSeg3=0; 
      SALOME_MED::medGeometryElement medElement;
      SMDS_MeshEdgesIterator itEdges(_meshDS);
      for (;itEdges.More();itEdges.Next()) 
	{
	  const Handle(SMDS_MeshElement)& elem = itEdges.Value();
	  int nb_of_nodes = elem->NbNodes();

	  switch (nb_of_nodes) 
	    {
	    case 2 : 
	      {
		medElement=SALOME_MED::MED_SEG2;
		if (trouveSeg2==0)
		  {
		    trouveSeg2=1;
		    _TypesId[SALOME_MED::MED_EDGE].push_back(SALOME_MED::MED_SEG2);

		  }
		break;
	      }
	    case 3 : 
	      {
		medElement=SALOME_MED::MED_SEG3;
		if (trouveSeg3==0)
		  {
		    trouveSeg3=1;
		    _TypesId[SALOME_MED::MED_EDGE].push_back(SALOME_MED::MED_SEG3);
		  }
		break;
	      }
	    }
	  int index=_mapIndToSeqElts[medElement];
	  SCRUTE(index);
	  // Traitement de l arete
	  int longueur=_seq_elemId[index]->length();
	  _seq_elemId[index]->length(longueur + nb_of_nodes);

	  for (int k=0; k<nb_of_nodes; k++)
	    {
	      _seq_elemId[index][longueur+k]=elem->GetConnection(k+1) +1;
	    }
	}

      _mapNbTypes[SALOME_MED::MED_EDGE]=trouveSeg2 + trouveSeg3;

      // On compte les faces MED_TRIA3, MED_HEXA8, MED_TRIA6
      // On range les elements dans  les vecteurs correspondants 
      int trouveTria3=0; 
      int trouveTria6=0; 
      int trouveQuad4=0; 

      _mapIndToSeqElts[SALOME_MED::MED_TRIA3]=_indexElts++;
      _mapIndToSeqElts[SALOME_MED::MED_TRIA6]=_indexElts++;
      _mapIndToSeqElts[SALOME_MED::MED_QUAD4]=_indexElts++;
      _mapIndToVectTypes[SALOME_MED::MED_FACE]=_indexEnts++;

      SMDS_MeshFacesIterator itFaces(_meshDS);
      for (;itFaces.More();itFaces.Next()) 
	{

	  const Handle(SMDS_MeshElement)& elem = itFaces.Value();
	  int nb_of_nodes = elem->NbNodes();

	  switch (nb_of_nodes) 
	    {
	    case 3 : 
	      {
		medElement=SALOME_MED::MED_TRIA3;
		if (trouveTria3==0)
		  {
		    trouveTria3=1;
		    _TypesId[SALOME_MED::MED_FACE].push_back(SALOME_MED::MED_TRIA3);
		  }
		break;
	      }
	    case 4 : 
	      {
		medElement=SALOME_MED::MED_QUAD4;
		if (trouveQuad4==0)
		  {
		    trouveQuad4=1;
		    _TypesId[SALOME_MED::MED_FACE].push_back(SALOME_MED::MED_QUAD4);
		  }
		break;
	      }
	    case 6 : 
	      {
		medElement=SALOME_MED::MED_TRIA6;
		if (trouveTria6==0)
		  {
		    trouveTria6=1;
		    _TypesId[SALOME_MED::MED_FACE].push_back(SALOME_MED::MED_TRIA6);
		  }
		break;
	      }
	    }
	  int index=_mapIndToSeqElts[medElement];
	  SCRUTE(index);

	  // Traitement de la face
	  // Attention La num�rotation des noeuds Med commence a 1


	  int longueur = _seq_elemId[index]->length();
	  _seq_elemId[index]->length(longueur+nb_of_nodes);

	  for (int k=0; k<nb_of_nodes; k++)
	    {
	      _seq_elemId[index][longueur+k]=elem->GetConnection(k+1) +1;
	    }

	}
      _mapNbTypes[SALOME_MED::MED_FACE]=trouveTria3 + trouveTria6 + trouveQuad4;

      _mapIndToSeqElts[SALOME_MED::MED_HEXA8]=_indexElts++;
      _mapIndToVectTypes[SALOME_MED::MED_CELL]=_indexEnts++;
      int index=_mapIndToSeqElts[medElement];

      int trouveHexa8=0;
      SMDS_MeshVolumesIterator itVolumes(_meshDS);
      for (;itVolumes.More();itVolumes.Next()) 
	{
	  const Handle(SMDS_MeshElement)& elem = itVolumes.Value();

	  int nb_of_nodes = elem->NbNodes();
	  medElement=SALOME_MED::MED_HEXA8;
	  ASSERT(nb_of_nodes=8);

	  if (trouveHexa8 == 0)
	    {
	      trouveHexa8=1;
	      _TypesId[SALOME_MED::MED_CELL].push_back(SALOME_MED::MED_HEXA8);
	    };
	  // Traitement de la maille
	  int longueur=_seq_elemId[index]->length();
	  _seq_elemId[index]->length(longueur+nb_of_nodes);

	  for (int k=0; k<nb_of_nodes; k++)
	    {
	      _seq_elemId[index][longueur+k]=elem->GetConnection(k+1) +1;
	    }

        }

      _mapNbTypes[SALOME_MED::MED_CELL]=trouveHexa8;
      _mapNbTypes[SALOME_MED::MED_ALL_ENTITIES]
	=trouveHexa8 + trouveTria3 + trouveTria6 + trouveQuad4 + trouveSeg2 + trouveSeg3 ;
    } /* fin du _compte */
};
//=============================================================================
/*!
 * Creation des familles
 */
//=============================================================================
void SMESH_MEDMesh_i::createFamilies()
  throw (SALOME::SALOME_Exception)
{
  string famDes=("Je ne sais pas");
  string famName0="Famille_";
  string famName;
  char numero[10];

  if ( _creeFamily == false)
    {
      _creeFamily = true;
      SMESH_subMesh_i* subMeshServant;

      map<int, SMESH_subMesh_i*>::iterator it;
      for (it = _mesh_i->_mapSubMesh_i.begin(); it !=  _mesh_i->_mapSubMesh_i.end(); it++) {
	SMESH_subMesh_i* submesh_i = (*it).second;
	int famIdent = (*it).first;

	ASSERT (famIdent<999999999);
	sprintf(numero,"%d\n",famIdent);
	famName=famName0 + numero;

	SMESH_MEDFamily_i * famservant= new SMESH_MEDFamily_i(famIdent, submesh_i,
							      famName, famDes, SALOME_MED::MED_NODE);
	SALOME_MED::FAMILY_ptr famille = 
	  SALOME_MED::FAMILY::_narrow(famservant->POA_SALOME_MED::FAMILY::_this());
	_families.push_back(famille);
      }
    } /* fin du _compte */
};
