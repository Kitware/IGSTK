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
#include <algorithm>

namespace igstk
{

Object::Object()
{
  m_Logger = LoggerType::New();
  m_ObservedObjectDeleteReceptor = DeleteEventCommandType::New();
  m_ObservedObjectDeleteReceptor->SetCallbackFunction( 
    this, &igstk::Object::ObservedObjectDeleteProcessing );
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
Object::RegisterObservedObject( 
  const ObservedObjectType * object, unsigned long tag )
{ 
  /** Add a delete callback if the object isn't being observed yet. */
  ObservedObjectTagPairObjectMatchPredicate objectPointerEquality( object );

  ObservedObjectPairContainer::iterator findResult =
     std::find_if( m_ObservedObjectPairContainer.begin(),
                   m_ObservedObjectPairContainer.end(),
                   objectPointerEquality );

  if ( findResult == m_ObservedObjectPairContainer.end() )
    {
    /* We are not observing the object yet. Add a delete event callback.
     */
    unsigned long delTag = 
      object->AddObserver( 
        itk::DeleteEvent(), m_ObservedObjectDeleteReceptor );

    ObservedObjectTagPair delTagPair;
    delTagPair.first = object;
    delTagPair.second = delTag;
    m_ObservedObjectPairContainer.push_back( delTagPair );
    }

  // Keep track of the object we're observing and the observer tag.  
  ObservedObjectTagPair objTagPair;
  objTagPair.first = object;
  objTagPair.second = tag;
  m_ObservedObjectPairContainer.push_back( objTagPair );
}


void 
Object::RemoveFromObservedObjects()
{
  ObservedObjectPairContainer::iterator itr = 
    this->m_ObservedObjectPairContainer.begin();

  ObservedObjectPairContainer::iterator end = 
    this->m_ObservedObjectPairContainer.end();

  while( itr != end )
    {
    const ObservedObjectType* obj = (*itr).first;
    obj->RemoveObserver( (*itr).second );
    ++itr;
    }
}

/** Print Self function */
void Object::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

void Object::ObservedObjectDeleteProcessing( 
  const ::itk::Object* caller, const EventType& event )
{
  ::itk::DeleteEvent deleteEvent;
  if ( deleteEvent.CheckEvent( &event ) == true )
    {
    // Setup a predicate that is true if the object in the pair == caller.
    ObservedObjectTagPairObjectMatchPredicate objectIsCaller( caller );

    // Remove the object that is being deleted from the list
    // containing the objects we observe. We use this list 
    // when we're deleted to remove observers we've put in place.
    m_ObservedObjectPairContainer.remove_if( objectIsCaller );
    }
}

}
