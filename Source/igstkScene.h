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
     

  /** Method for creation through the object factory */ 
  itkTypeMacro(Self, Superclass); 
  NewMacro(Self); 

  /** Add an object to the list of children. */ 
  void AddObject( ObjectRepresentation * pointer ); 
     
  /** Remove the object passed as arguments from the list of 
   *  children. May this function 
   *  should return a false value if the object to remove is 
   *  not found in the list. */ 
  void RemoveObject( ObjectRepresentation * object ); 

  /** Returns a list of pointer to the children affiliated to this object.*/ 
  GetMacro( Objects, ObjectListType );

  /** Returns the number of children currently assigned to the SceneSpatialObject object.*/ 
  unsigned int GetNumberOfObjects() const; 

  /** Clear function : Remove all the objects in the scene */
  void Clear();

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

}; 

} // end of namespace igstk
 

#endif // __igstkScene_h 
