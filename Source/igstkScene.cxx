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
  m_ViewToAddress = NULL;

  m_StateMachine.AddInput( m_ValidAddObject,     "ValidAddObject" );
  m_StateMachine.AddInput( m_NullAddObject,      "NullAddObject"  );
  m_StateMachine.AddInput( m_NullViewAddObject,  "NullViewAddObject"  );
  m_StateMachine.AddInput( m_ExistingAddObject,  "ExistingAddObject" );
  m_StateMachine.AddInput( m_ValidRemoveObject,  "ValidRemoveObject" );
  m_StateMachine.AddInput( m_NullRemoveObject,   "NullRemoveObject"  );
  m_StateMachine.AddInput( m_NullViewRemoveObject, "NullViewRemoveObject"  );
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
  m_StateMachine.AddTransition( m_IdleState, m_NullViewAddObject,  m_IdleState,  NoAction );
  m_StateMachine.AddTransition( m_IdleState, m_NullViewRemoveObject,  m_IdleState, NoAction );

  m_StateMachine.SelectInitialState( m_IdleState );

  m_StateMachine.SetReadyToRun();

}

/** Destructor */
Scene::~Scene()
{
}

/** Request for Adding an object to the Scene */
void Scene::RequestAddObject(View* view, ObjectRepresentation* pointer )
{
  m_ObjectToBeAdded = pointer;
  m_ViewToAddress = view;

  if(!view)
    {
    m_StateMachine.ProcessInput( m_NullViewAddObject );
    return;
    }

  if( !pointer )
    {
    m_StateMachine.ProcessInput( m_NullAddObject );
    return;
    }

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


/** Add an object to the Scene. This method should only be called by the state
 * machine. The state machine makes sure that this method is called with valid
 * values in the ObjectToBeAdded and the m_ViewToAddress. */
void Scene::AddObject()
{
  m_Objects.push_back( m_ObjectToBeAdded );
  this->Modified();
  
  m_ObjectToBeAdded->CreateActors();

  ObjectRepresentation::ActorsListType actors = m_ObjectToBeAdded->GetActors();
  ObjectRepresentation::ActorsListType::iterator actorIt = actors.begin();
  while(actorIt != actors.end())
    {
    m_ViewToAddress->RequestAddActor(*actorIt);
    actorIt++;
    } 
}


/** Request for removing a spatial object from the Scene */
void Scene::RequestRemoveObject( View* view, ObjectRepresentation* pointer )
{
  m_ObjectToBeRemoved = pointer;
  m_ViewToAddress = view;
  m_IteratorToObjectToBeRemoved = m_Objects.end(); 
  
  if(!view)
    {
    m_StateMachine.ProcessInput( m_NullViewRemoveObject );
    return;
    }
  
  if( !pointer )
    {
    m_StateMachine.ProcessInput( m_NullRemoveObject );
    return;
    }
  
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


/** Remove a spatial object from the Scene. This method can only be invoked by
 * the State Machine who will make sure that the content of
 * m_IteratorToObjectToBeRemoved and m_ViewToAddress are valid. */
void Scene::RemoveObject()
{
  m_Objects.erase( m_IteratorToObjectToBeRemoved );
  this->Modified();
  
  ObjectRepresentation::ActorsListType actors = m_ObjectToBeRemoved->GetActors();
  ObjectRepresentation::ActorsListType::iterator actorIt = actors.begin();
  while(actorIt != actors.end())
    {
    m_ViewToAddress->RequestRemoveActor(*actorIt);
    actorIt++;
    } 
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
    it = m_Objects.erase( it );
    this->Modified();
    
    
    //this->InvokeEvent( SceneRemoveObjectEvent() );
    }

  m_Objects.clear();
}


} // end of namespace igstk

