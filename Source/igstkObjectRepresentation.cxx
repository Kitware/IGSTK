/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkObjectRepresentation.h" 
#include "vtkMatrix4x4.h"
#include "igstkEvents.h"
#include "vtkActor.h"
#include "vtkProperty.h"

namespace igstk 
{ 

/** Constructor */
ObjectRepresentation::ObjectRepresentation()
{
  m_Color[0] = 1.0;
  m_Color[1] = 1.0;
  m_Color[2] = 1.0;
  m_Opacity = 1.0;
  m_SpatialObject = NULL;
  m_LastMTime = 0;
  m_PositionObserver    = ObserverType::New();
  m_OrientationObserver = ObserverType::New();
  m_GeometryObserver    = ObserverType::New();
  m_PositionObserver->SetCallbackFunction(    this, & ObjectRepresentation::UpdatePositionFromGeometry    );
  m_OrientationObserver->SetCallbackFunction( this, & ObjectRepresentation::UpdateOrientationFromGeometry );
  m_GeometryObserver->SetCallbackFunction(    this, & ObjectRepresentation::UpdateRepresentationFromGeometry );
} 

/** Destructor */
ObjectRepresentation::~ObjectRepresentation()  
{ 
}

/** Add an actor to the actors list */
void ObjectRepresentation::AddActor( vtkProp3D * actor )
{
  m_Actors.push_back( actor );
}

/** Empty the list of actors */
void ObjectRepresentation::DeleteActors()
{
  ActorsListType::iterator it = m_Actors.begin();
  while(it != m_Actors.end())
    {
    (*it)->Delete();
    it++;
    }

  // Reset the list of actors
  m_Actors.clear();
}

/** Has the object been modified */
bool ObjectRepresentation::IsModified() const
{
  if( m_LastMTime < this->GetMTime() )
    {
    return true;
    }
  return false;
}


/** Set the Spatial Object */
void ObjectRepresentation::SetSpatialObject( const SpatialObjectType * spatialObject )
{
  m_SpatialObject = spatialObject;
  if( m_SpatialObject )
    {
    m_SpatialObject->AddObserver( PositionModifiedEvent(),    m_PositionObserver    );
    m_SpatialObject->AddObserver( OrientationModifiedEvent(), m_OrientationObserver );
    m_SpatialObject->AddObserver( GeometryModifiedEvent(), m_GeometryObserver );
    }
}


/** Set the color */
void ObjectRepresentation::SetColor(float r, float g, float b)
{
  if(m_Color[0] == r && m_Color[1] == g && m_Color[2] == b)
    {
    return;
    }
  m_Color[0] = r;
  m_Color[1] = g;
  m_Color[2] = b;

  // Update all the actors
  ActorsListType::iterator it = m_Actors.begin();
  while(it != m_Actors.end())
    {
    static_cast<vtkActor*>(*it)->GetProperty()->SetColor(m_Color[0],
                                   m_Color[1],
                                   m_Color[2]); 
    it++;
    }

  this->Modified();
}


/** Update the object representation (i.e vtkActors). Maybe we should check also the transform
 *  modified time. */
void ObjectRepresentation::UpdateOrientationFromGeometry()
{
  SpatialObject::MatrixType itkMatrix = m_SpatialObject->GetMatrix();
  SpatialObject::VectorType offset    = m_SpatialObject->GetOffset();
 
  vtkMatrix4x4* vtkMatrix = vtkMatrix4x4::New();
  for(unsigned int i=0;i<3;i++)
  {
    for(unsigned int j=0;j<3;j++)
    {
      vtkMatrix->SetElement(i,j,itkMatrix.GetVnlMatrix().get(i,j));   
    }

    vtkMatrix->SetElement(i,3,offset[i]);
  }

  // Update all the actors
  ActorsListType::iterator it = m_Actors.begin();
  while(it != m_Actors.end())
  {  
    (*it)->SetUserMatrix(vtkMatrix);
    it++;
  }

  // Update the modified time
  m_LastMTime = this->GetMTime();
}

/** Update the object representation. This method must be overrided in every
 * derived class. */
void ObjectRepresentation::UpdateRepresentationFromGeometry()
{
  std::cerr << "If you see this message: it means that you forgot " << std::endl;
  std::cerr << "to override the method UpdateRepresentationFromGeometry()" << std::endl;
  std::cerr << "in a class deriving from ObjectRepresentation" << std::endl;
}


/** Update the object representation (i.e vtkActors). Maybe we should check also the transform
 *  modified time. */
void ObjectRepresentation::UpdatePositionFromGeometry()
{
  SpatialObject::MatrixType itkMatrix = m_SpatialObject->GetMatrix();
  SpatialObject::VectorType offset    = m_SpatialObject->GetOffset();
 
  vtkMatrix4x4* vtkMatrix = vtkMatrix4x4::New();
  for(unsigned int i=0;i<3;i++)
  {
    for(unsigned int j=0;j<3;j++)
    {
      vtkMatrix->SetElement(i,j,itkMatrix.GetVnlMatrix().get(i,j));   
    }

    vtkMatrix->SetElement(i,3,offset[i]);
  }

  // Update all the actors
  ActorsListType::iterator it = m_Actors.begin();
  while(it != m_Actors.end())
  {  
    (*it)->SetUserMatrix(vtkMatrix);
    it++;
  }

  // Update the modified time
  m_LastMTime = this->GetMTime();
}


/** Print Self function */
void ObjectRepresentation::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Color: " << m_Color[0] << " : " << m_Color[1] << " : " << m_Color[2] << std::endl;
  os << indent << "Opacity: " << m_Opacity << std::endl;
}

} // end namespace igstk

