/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// Disabling warning C4355: 'this' : used in base member initializer list
#if defined(_MSC_VER)
#pragma warning ( disable : 4355 )
#endif

#include "igstkObject.h"


namespace igstk
{


Object::Object()
{
  m_Logger = LoggerType::New();
}

Object::~Object()
{
  this->RemoveFromObservedObjects();
}


Object::LoggerType * 
Object::GetLogger() const 
{ 
  return m_Logger; 
} 


void 
Object::SetLogger( LoggerType * logger) 
{ 
  m_Logger = logger; 
}


void 
Object::RemoveObserver( unsigned long tag ) const
{ 
  Object * nonConstObject = const_cast< Object * >( this );
  nonConstObject->Superclass::RemoveObserver( tag );
}


void 
Object::RegisterObservedObject( const ::igstk::Object * object, unsigned long tag )
{ 
  this->m_ObservedObjectsContainer.push_back( object );
  this->m_ObserverTagsContainer.push_back( tag );
}


void 
Object::RemoveFromObservedObjects()
{
  ObservedObjectsContainer::iterator objectItr = this->m_ObservedObjectsContainer.begin();
  ObserverTagContainer::iterator itr = this->m_ObserverTagsContainer.begin();
  ObserverTagContainer::iterator end = this->m_ObserverTagsContainer.end();
  while( itr != end )
    {
    (*objectItr)->RemoveObserver( *itr );
    ++itr;
    ++objectItr;
    }
}

/** Print Self function */
void Object::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

}
