/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkScene.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkScene.h"
#include <algorithm>
#include <igstkEvents.h>

namespace igstk
{
  
/** Constructor */
Scene::Scene():m_StateMachine(this)
{
  m_LastAddedObject = 0;
  m_LastRemovedObject = 0;

  m_StateMachine.AddInput( m_ValidAddObject,     "ValidAddObject" );
  m_StateMachine.AddInput( m_NullAddObject,      "NullAddObject"  );
  m_StateMachine.AddInput( m_ExistingAddObject,  "ExistingAddObject" );
  m_StateMachine.AddInput( m_ValidRemoveObject,  "ValidRemoveObject" );
  m_StateMachine.AddInput( m_NullRemoveObject,   "NullRemoveObject"  );
  m_StateMachine.AddInput( m_RemoveAllObjects,   "RemoveAllObjects"  );
  m_StateMachine.AddInput( m_InexistingRemoveObject,  "InexistingRemoveObject" );

  m_StateMachine.AddState( m_IdleState,      "IdleState"     );

  const ActionType NoAction = 0;

  m_StateMachine.AddTransition( m_IdleState, m_ValidAddObject, m_IdleState,  & Scene::AddObject );
  m_StateMachine.AddTransition( m_IdleState, m_NullAddObject,  m_IdleState,  NoAction );
  m_StateMachine.AddTransition( m_IdleState, m_ExistingAddObject,  m_IdleState,  NoAction );
  m_StateMachine.AddTransition( m_IdleState, m_ValidRemoveObject, m_IdleState,  & Scene::RemoveObject );
  m_StateMachine.AddTransition( m_IdleState, m_NullRemoveObject,  m_IdleState,          NoAction );
  m_StateMachine.AddTransition( m_IdleState, m_InexistingRemoveObject,  m_IdleState,    NoAction );
  m_StateMachine.AddTransition( m_IdleState, m_RemoveAllObjects, m_IdleState,  & Scene::RemoveAllObjects );

  m_StateMachine.SelectInitialState( m_IdleState );

  m_StateMachine.SetReadyToRun();
}

/** Destructor */
Scene::~Scene()
{
}

/** Request for Adding an object to the Scene */
void Scene::RequestAddObject(ObjectRepresentation * pointer )
{
  m_ObjectToBeAdded = pointer;
  if( !pointer )
    {
    m_StateMachine.ProcessInput( m_NullAddObject );
    }
  else
    {
    ObjectListType::iterator it =    
      std::find(m_Objects.begin(),m_Objects.end(),pointer);
    if( it != m_Objects.end() )
      {
      m_StateMachine.ProcessInput( m_ExistingAddObject );
      }
    else
      {
      m_StateMachine.ProcessInput( m_ValidAddObject );
      }
    }
}


/** Add an object to the Scene. This method should only be called by the state
 * machine. */
void Scene::AddObject()
{
  m_Objects.push_back( m_ObjectToBeAdded );
  this->Modified();
  m_LastAddedObject = m_ObjectToBeAdded;
  this->InvokeEvent( SceneAddObjectEvent() );
}


/** Request for removing a spatial object from the Scene */
void Scene::RequestRemoveObject( ObjectRepresentation * pointer )
{
  m_ObjectToBeRemoved = pointer;
  m_IteratorToObjectToBeRemoved = m_Objects.end(); 
  if( !pointer )
    {
    m_StateMachine.ProcessInput( m_NullRemoveObject );
    }
  else
    {
    m_IteratorToObjectToBeRemoved =
      std::find(m_Objects.begin(),m_Objects.end(),pointer);
    if( m_IteratorToObjectToBeRemoved == m_Objects.end() )
      {
      m_StateMachine.ProcessInput( m_InexistingRemoveObject );
      }
    else
      {
      m_StateMachine.ProcessInput( m_ValidRemoveObject );
      }
    }
}


/** Remove a spatial object from the Scene */
void Scene::RemoveObject()
{
  m_Objects.erase( m_IteratorToObjectToBeRemoved );
  m_LastRemovedObject = m_ObjectToBeRemoved;
  this->Modified();
  this->InvokeEvent( SceneRemoveObjectEvent() );
}


/** Return the number of objects in the Scene */
unsigned int 
Scene::GetNumberOfObjects() const
{
  return m_Objects.size();
} 

/** Print the object */
void Scene::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "Number of objects: " 
     << m_Objects.size() << std::endl;
  os << indent << "List of objects: ";

  ObjectListType::const_iterator it = m_Objects.begin();
  ObjectListType::const_iterator itEnd = m_Objects.end();

  while(it != itEnd)
    {
    os << "[" << (*it) << "] ";
    it++;
    }
  os << std::endl;

  Superclass::PrintSelf(os, indent);
}

/** Request for removing all the objects in the scene */
void Scene::RequestRemoveAllObjects()
{
  m_StateMachine.ProcessInput( m_RemoveAllObjects );
}


/** Remove all the objects in the scene */
void Scene::RemoveAllObjects()
{
  ObjectListType::iterator it = m_Objects.begin();

  while( it != m_Objects.end() )
    {
    m_Objects.erase( it );
    m_LastRemovedObject = *it;
    this->Modified();
    this->InvokeEvent( SceneRemoveObjectEvent() );
    }

  m_Objects.clear();
}


} // end of namespace igstk

