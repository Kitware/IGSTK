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

namespace igstk
{
  
/** Constructor */
Scene::Scene()
{
}

/** Destructor */
Scene::~Scene()
{
}

/** Add an  object to the Scene */
void Scene::AddObject(ObjectRepresentation * pointer )
{
  m_Objects.push_back( pointer );
  this->Modified();
}

/** Remove a spatial object from the Scene */
void Scene::RemoveObject( ObjectRepresentation * pointer )
{
  ObjectListType::iterator it;    
  it = std::find(m_Objects.begin(),m_Objects.end(),pointer);

  if( it != m_Objects.end() )
    {
    if( *it == pointer )
      {
      m_Objects.erase( it );
      this->Modified();
      }
    }
  else
    { 
    //throw an exception object to let user know that he tried to remove an object
    // which is not in the list of the children.
    }
}


/** Return the children list */
Scene::ObjectListType * Scene::GetObjects( unsigned int depth, char * name )
{
  ObjectListType * newList = new ObjectListType;
  ObjectListType::const_iterator it = m_Objects.begin();
  ObjectListType::const_iterator itEnd = m_Objects.end();

  while(it != itEnd)
    {
    if(name == NULL || strstr(typeid(**it).name(), name))
      {
      newList->push_back(*it);
      }
    it++;
    }

  return newList;
}

/** Return the number of objects in the Scene */
unsigned int Scene::GetNumberOfObjects( unsigned int depth, char * name )
{
  ObjectListType::iterator it = m_Objects.begin();
  ObjectListType::iterator itEnd = m_Objects.end();

  unsigned int cnt = 0;
  while(it != itEnd)
    {
    if(name == NULL || strstr(typeid(**it).name(), name))
      {
      cnt++;
      }
    it++;
    }

  it = m_Objects.begin();
  itEnd = m_Objects.end();
  if( depth > 0 )
    {
    while(it != itEnd)
      {
      cnt += (*it).GetPointer()->GetNumberOfChildren( depth-1, name );
      it++;
      }
    }
  return cnt;
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

/** Clear function : Remove all the objects in the scene */
void Scene::Clear()
{
  m_Objects.clear();
}


} // end of namespace igstk

