/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkScene.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
 
#ifndef __igstkScene_h 
#define __igstkScene_h 

#include "igstkObjectRepresentation.h"   
#include "igstkStateMachine.h"
 
#include <list>

namespace igstk 
{ 

/** \class SceneSpatialObject
* \brief a SceneSpatialObject has a list of SpatialObjects
*
* This class represent a SceneSpatialObject object into which one can
* plug any kind of spatial object.
*
* \sa SpatialObject
*/ 
 
class Scene : public itk::Object
{ 
 
public: 

  typedef Scene Self; 
  typedef itk::Object Superclass; 
  typedef itk::SmartPointer< Self > Pointer; 
  typedef itk::SmartPointer< const Self > ConstPointer; 
  typedef ObjectRepresentation::Pointer ObjectPointer;

  typedef std::list< ObjectPointer >        ObjectListType; 
  typedef ObjectListType::iterator          ObjectListIterator;
  typedef ObjectListType::const_iterator    ObjectListConstIterator;
     
  /** Method for defining the name of the class */ 
  TypeMacro(Scene, Object); 

  /** Method for creation through the object factory */ 
  NewMacro(Self); 

  /** Add an object to the list of children. */ 
  void RequestAddObject( ObjectRepresentation * pointer ); 
     
  /** Remove the object passed as arguments from the list of 
   *  children. May this function 
   *  should return a false value if the object to remove is 
   *  not found in the list. */ 
  void RequestRemoveObject( ObjectRepresentation * object ); 

  /** Returns a list of pointer to the children affiliated to this object.*/ 
  GetMacro( Objects, ObjectListType );

  /** Returns the number of children currently assigned to the
   * SceneSpatialObject object.*/ 
  unsigned int GetNumberOfObjects() const; 

  /** Remove all the objects in the scene */
  void RequestRemoveAllObjects();

  /** Get the last added object */
  ObjectRepresentation* GetLastAddedObject() {return m_LastAddedObject;}

  /** Get the last removed object */
  ObjectRepresentation* GetLastRemovedObject() {return m_LastRemovedObject;}

protected: 

  /** List of the children object plug to the SceneSpatialObject spatial object. */
  ObjectListType m_Objects; 

  ObjectPointer  m_LastAddedObject;
  ObjectPointer  m_LastRemovedObject;

  /** constructor */ 
  Scene(); 

  /** destructor */ 
  virtual ~Scene();

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:
    
  typedef igstk::StateMachine< Scene >               StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer   ActionType;
  typedef StateMachineType::StateType                StateType;
  typedef StateMachineType::InputType                InputType;

  FriendClassMacro( StateMachineType );

  // Actual execution method associated to the ones that can be requested from
  // the external API. Only the state machine is allowed to call the execution
  // methods below.
  void AddObject();
  void RemoveObject();
  void RemoveAllObjects();

private:

  // Arguments for methods to be invoked by the state machine.
  //
  ObjectRepresentation * m_ObjectToBeAdded;
  ObjectRepresentation * m_ObjectToBeRemoved;
  ObjectListType::iterator m_IteratorToObjectToBeRemoved;    


private:

  StateMachineType     m_StateMachine;
  
  /** Inputs to the State Machine */
  InputType            m_ValidAddObject;
  InputType            m_NullAddObject;
  InputType            m_ExistingAddObject;
  InputType            m_ValidRemoveObject;
  InputType            m_InexistingRemoveObject;
  InputType            m_NullRemoveObject;
  InputType            m_RemoveAllObjects;
  

  /** States for the State Machine */
  StateType            m_IdleState;

}; 

} // end of namespace igstk
 

#endif // __igstkScene_h 
