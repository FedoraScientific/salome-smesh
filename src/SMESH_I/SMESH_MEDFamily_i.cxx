using namespace std;
//=============================================================================
// File      : SMESH_MEDFamily_i.cxx
// Project   : SALOME
// Copyright : EDF 2001
//=============================================================================

#include "SMESH_MEDFamily_i.hxx"
#include "utilities.h"
#include "Utils_CorbaException.hxx"

//=============================================================================
/*!
 * Default constructor
 */
//=============================================================================
SMESH_MEDFamily_i::SMESH_MEDFamily_i()
{
  BEGIN_OF("Default Constructor SMESH_MEDFamily_i");
  END_OF("Default Constructor SMESH_MEDFamily_i");
}
//=============================================================================
/*!
 * constructor par recopie
 */
//=============================================================================
SMESH_MEDFamily_i::SMESH_MEDFamily_i(const SMESH_MEDFamily_i & f): 
  SMESH_MEDSupport_i(f._subMesh_i,f._name,f._description,f._entity),
  _subMesh_i(f._subMesh_i),
  _identifier(f._identifier),
  _numberOfAttribute(f._numberOfAttribute),
  _numberOfGroup(f._numberOfGroup), 
  _attributeValue(f._attributeValue)
{
  BEGIN_OF("Copy Constructor SMESH_MEDFamily_i");
  _attributeDescription=new string[_numberOfAttribute];
  for (int i=0;i<_numberOfAttribute;i++) {
    _attributeDescription[i]=f._attributeDescription[i];
  };
  _groupName=new string[_numberOfGroup];
  for (int i=0;i<_numberOfAttribute;i++) {
    _groupName[i]=f._groupName[i];
  };
  END_OF("Copy Constructor SMESH_MEDFamily_i");
}
//=============================================================================
/*!
 * Destructor
 */
//=============================================================================
SMESH_MEDFamily_i::~SMESH_MEDFamily_i()
{
}
//=============================================================================
/*!
 * Constructor
 */
//=============================================================================
SMESH_MEDFamily_i::SMESH_MEDFamily_i(int identifier, SMESH_subMesh_i* sm,
				     string name, string description, SALOME_MED::medEntityMesh entity): 
  SMESH_MEDSupport_i( sm, name, description, entity ),
  
  _subMesh_i(sm),
  _identifier(identifier),
  _numberOfAttribute(0),
  _attributeIdentifier((int*)NULL),
  _attributeValue((int*)NULL), 
  _attributeDescription((string*)NULL),
  _numberOfGroup(0), 
  _groupName((string*)NULL)
{
  BEGIN_OF("Constructor SMESH_MEDFamily_i");
  END_OF("Constructor SMESH_MEDFamily_i");
}
//=============================================================================
/*!
 * CORBA: Accessor for Family's Identifier
 */
//=============================================================================

CORBA::Long SMESH_MEDFamily_i::getIdentifier()      
throw (SALOME::SALOME_Exception)
{
  if (_subMeshDS==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
				 SALOME::INTERNAL_ERROR); 
  return _identifier;
  
}
//=============================================================================
/*!
 * CORBA: Accessor for number of attributes
 */
//=============================================================================
CORBA::Long SMESH_MEDFamily_i::getNumberOfAttributes() 
throw (SALOME::SALOME_Exception)
{
  if (_subMeshDS==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
				 SALOME::INTERNAL_ERROR);
  return _numberOfAttribute;
}
//=============================================================================
/*!
 * CORBA: Accessor for attributes identifiers
 */
//=============================================================================
Engines::long_array*  SMESH_MEDFamily_i::getAttributesIdentifiers() 
throw (SALOME::SALOME_Exception)
{
  if (_subMeshDS==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
				 SALOME::INTERNAL_ERROR);
  if (_numberOfAttribute == 0)
    {
      MESSAGE("Les familles SMESH n ont pas d attribut");
      THROW_SALOME_CORBA_EXCEPTION("No attributes"\
				   ,SALOME::BAD_PARAM);
    };
  
  Engines::long_array_var myseq= new Engines::long_array;
  myseq->length(_numberOfAttribute);
  for (int i=0;i<_numberOfAttribute;i++)
    {
      myseq[i]=_attributeIdentifier[i];
    };
  return myseq._retn();

}
//=============================================================================
/*!
 * CORBA: Accessor for attribute identifier I
 */
//=============================================================================
CORBA::Long SMESH_MEDFamily_i::getAttributeIdentifier(CORBA::Long i) 
  throw (SALOME::SALOME_Exception)
{    
  if (_subMeshDS==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
				 SALOME::INTERNAL_ERROR);
  MESSAGE("Les familles SMESH n ont pas d attribut");
  THROW_SALOME_CORBA_EXCEPTION("No attributes"\
			       ,SALOME::BAD_PARAM);
  if (_numberOfAttribute = 0)
    {
      MESSAGE("Les familles SMESH n ont pas d attribut");
      THROW_SALOME_CORBA_EXCEPTION("No attributes"\
				   ,SALOME::BAD_PARAM);
    };
  ASSERT (i <= _numberOfAttribute);
  return _attributeIdentifier[i];
}
//=============================================================================
/*!
 * CORBA: Accessor for attributes values
 */
//=============================================================================
Engines::long_array*  SMESH_MEDFamily_i::getAttributesValues() 
  throw (SALOME::SALOME_Exception)
{
  if (_subMeshDS==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
				 SALOME::INTERNAL_ERROR);

  if (_numberOfAttribute == 0)
    {
      MESSAGE("Les familles SMESH n ont pas d attribut");
      THROW_SALOME_CORBA_EXCEPTION("No attributes"\
				   ,SALOME::BAD_PARAM);
    };

  Engines::long_array_var myseq= new Engines::long_array;
  myseq->length(_numberOfAttribute);
  for (int i=0;i<_numberOfAttribute;i++)
    {
      myseq[i]=_attributeValue[i];
    };
  return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for attribute value I
 */
//=============================================================================
CORBA::Long  SMESH_MEDFamily_i::getAttributeValue(CORBA::Long i) 
  throw (SALOME::SALOME_Exception)
{   
  if (_subMeshDS==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
				 SALOME::INTERNAL_ERROR);
  if (_numberOfAttribute = 0)
    {
      MESSAGE("Les familles SMESH n ont pas d attribut");
      THROW_SALOME_CORBA_EXCEPTION("No attributes"\
				   ,SALOME::BAD_PARAM);
    }

  ASSERT (i <= _numberOfAttribute);
  return _attributeValue[i];
}
//=============================================================================
/*!
 * CORBA: Accessor for attributes desriptions
 */
//=============================================================================
Engines::string_array * SMESH_MEDFamily_i::getAttributesDescriptions() 
  throw (SALOME::SALOME_Exception)
{
  if (_subMeshDS==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
				 SALOME::INTERNAL_ERROR);
  if (_numberOfAttribute = 0)
    {
      MESSAGE("Les familles SMESH n ont pas d attribut");
      THROW_SALOME_CORBA_EXCEPTION("No attributes"\
				   ,SALOME::BAD_PARAM);
    }
  Engines::string_array_var myseq = new Engines::string_array;
  for (int i=0;i<_numberOfAttribute;i++)
    {
      myseq[i]=CORBA::string_dup(_attributeDescription[i].c_str());
    }
  return myseq._retn();
}
//=============================================================================
/*!
 * CORBA: Accessor for attribute description i
 */
//=============================================================================
char *  SMESH_MEDFamily_i::getAttributeDescription( CORBA::Long i) 
  throw (SALOME::SALOME_Exception)
{   
  if (_subMeshDS==NULL)
    THROW_SALOME_CORBA_EXCEPTION("No associated Family",\
				 SALOME::INTERNAL_ERROR);
  if (_numberOfAttribute = 0)
    {
      MESSAGE("Les familles SMESH n ont pas d attribut");
      THROW_SALOME_CORBA_EXCEPTION("No attributes"\
				   ,SALOME::BAD_PARAM);
    }
  ASSERT (i <= _numberOfAttribute);
  return CORBA::string_dup(_attributeDescription[i].c_str());
}