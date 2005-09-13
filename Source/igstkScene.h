 /*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkScene.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkScene_h 
#define __igstkScene_h 

#include "igstkStateMachine.h"
#include "igstkView.h"
 
#include <list>

namespace igstk 
{ 

/** \class Scene
* \brief a Scene has a list of SpatialObjects
*
* This class represent a Scene object into which one can
* plug any kind of spatial object representation.
*
* \deprecated
*
* \sa SpatialObject
* \sa View
* \sa View2D
* \sa View3D
*/ 
 
class Scene : public itk::Object
{ 
 
public: 

  typedef Scene Self; 
  typedef itk::Object Superclass; 
  typedef itk::SmartPointer< Self > Pointer; 
  typedef itk::SmartPointer< const Self > ConstPointer; 

  // Object representation types
  typedef View::ObjectPointer               ObjectPointer;
  typedef View::ObjectListType              ObjectListType; 
  typedef View::ObjectListIterator          ObjectListIterator;
  typedef View::ObjectListConstIterator     ObjectListConstIterator;
     
  /** Method for defining the name of the class */ 
  igstkTypeMacro(Scene, Object); 

  /** Method for creation through the object factory */ 
  igstkNewMacro(Self); 

  /** Add an object representation to the list of children and associate it
   * with a specific view. */ 
  void RequestAddObject( View* view, ObjectRepresentation* object ); 
     
  /** Remove the object passed as arguments from the list of children, only if
   * it is associated to a particular view. */ 
  void RequestRemoveObject( View* view, ObjectRepresentation* object ); 

  /** Remove all the objects in the scene */
  void RequestRemoveAllObjects();

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

protected: 

  /** constructor */ 
  Scene(); 

  /** destructor */ 
  virtual ~Scene();

  /** Print the object informations in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:
    
  // Actual execution method associated to the ones that can be requested from
  // the external API. Only the state machine is allowed to call the execution
  // methods below.
  void AddObject();
  void RemoveObject();
  void RemoveAllObjects();

private:

  /** List of the children object plug to the Scene spatial object. */
  ObjectListType m_Objects; 

  // Arguments for methods to be invoked by the state machine.
  //
  ObjectRepresentation::Pointer m_ObjectToBeAdded;
  ObjectRepresentation::Pointer m_ObjectToBeRemoved;
  ObjectListType::iterator      m_IteratorToObjectToBeRemoved;
  View*                         m_ViewToAddress;

private:

  /** Inputs to the State Machine */
  InputType            m_ValidAddObject;
  InputType            m_NullAddObject;
  InputType            m_NullViewAddObject;
  InputType            m_ExistingAddObject;
  InputType            m_ValidRemoveObject;
  InputType            m_InexistingRemoveObject;
  InputType            m_NullRemoveObject;
  InputType            m_NullViewRemoveObject;
  InputType            m_RemoveAllObjects;
  

  /** States for the State Machine */
  StateType            m_IdleState;

}; 

} // end of namespace igstk
 

#endif // __igstkScene_h 
