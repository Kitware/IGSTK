/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSpatialObjectTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>

#include "igstkSpatialObject.h"

namespace igstk
{
  
namespace SpatialObjectTest
{

class DummySpatialObject : public SpatialObject
{
public:
  igstkStandardClassTraitsMacro( DummySpatialObject, SpatialObject )

  bool TestMethods()
  {
    this->RequestAddObject( NULL );        // Test with null pointer
    this->RequestSetSpatialObject( NULL ); // Test with null pointer

    Self::Pointer sibling = Self::New();   // Test with valid pointer
    this->RequestAddObject( sibling );
      
    typedef SpatialObject::SpatialObjectType SpatialObjectType;
    SpatialObjectType::Pointer so = SpatialObjectType::New();
    this->RequestSetSpatialObject( so );   // Test with valid pointer

    const Self * me1 = dynamic_cast< const Self *>( this->GetObject(  0  ) );
    if( !me1 )
      {
      std::cerr << "Error in GetObject() with valid Id" << std::endl;
      return false;
      }
    
    const Self * me2 = dynamic_cast< const Self *>( this->GetObject( 100 ) );
    if( me2 )
      {
      std::cerr << "Error in GetObject() with invalid Id" << std::endl;
      return false;
      }

    return true;
  }

protected:
  DummySpatialObject( void ):m_StateMachine(this) {};
  ~DummySpatialObject( void ) {};
  
};


} // end SpatialObjectTest namespace

} // end igstk namespace


int igstkSpatialObjectTest( int, char * [] )
{

  typedef igstk::SpatialObjectTest::DummySpatialObject ObjectType;
  ObjectType::Pointer dummyObject = ObjectType::New();

  dummyObject->TestMethods();

  std::cout << "[PASSED]" << std::endl;

  std::cout << "Test [DONE]" << std::endl;

  return EXIT_SUCCESS;
}
