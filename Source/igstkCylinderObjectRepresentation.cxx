/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkCylinderObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkCylinderObjectRepresentation.h"
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include "igstkEvents.h"


namespace igstk
{ 

/** Constructor */
CylinderObjectRepresentation::CylinderObjectRepresentation()
{
  // We create the ellipse spatial object
  m_CylinderSpatialObject = NULL;
  this->SetSpatialObject( m_CylinderSpatialObject );
  m_CylinderSource = vtkCylinderSource::New();
  m_GeometryObserver       = ObserverType::New();
} 

/** Destructor */
CylinderObjectRepresentation::~CylinderObjectRepresentation()  
{
  if( m_CylinderSource )
    {
    m_CylinderSource->Delete();
    m_CylinderSource = NULL;
    }
}

/** Set the Cylindrical Spatial Object */
void CylinderObjectRepresentation::SetCylinder( const CylinderSpatialObjectType * cylinder )
{
  // We create the ellipse spatial object
  m_CylinderSpatialObject = cylinder;
  this->SetSpatialObject( m_CylinderSpatialObject );

  if( cylinder != NULL )
    {
    m_CylinderSource->SetCenter(0, 0, 0);
    m_CylinderSource->SetRadius(m_CylinderSpatialObject->GetRadius());
    m_CylinderSource->SetHeight(m_CylinderSpatialObject->GetHeight());
    m_CylinderSource->SetResolution(10);
   
    m_CylinderSpatialObject->AddObserver( GeometryModifiedEvent(), m_GeometryObserver );
    }

  m_GeometryObserver->SetCallbackFunction( this, & CylinderObjectRepresentation::UpdateRepresentationFromGeometry );
} 


/** Print Self function */
void CylinderObjectRepresentation::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

/** Update the visual representation in response to changes in the geometric
 * object */
void CylinderObjectRepresentation::UpdateRepresentationFromGeometry()
{
  m_CylinderSource->SetRadius(m_CylinderSpatialObject->GetRadius());
  m_CylinderSource->SetHeight(m_CylinderSpatialObject->GetHeight());
}

/** Create the vtk Actors */
void CylinderObjectRepresentation::CreateActors()
{
  vtkPolyDataMapper *cylinderMapper = vtkPolyDataMapper::New();
  vtkActor* cylinderActor = vtkActor::New();

  cylinderActor->GetProperty()->SetColor(this->GetRed(),
                                         this->GetGreen(),
                                         this->GetBlue()); 
        
  cylinderActor->GetProperty()->SetOpacity(this->GetOpacity()); 
  cylinderMapper->SetInput(m_CylinderSource->GetOutput());
  cylinderActor->SetMapper( cylinderMapper );

  // We should check if the actor doesn't exist
  this->AddActor( cylinderActor );
  cylinderMapper->Delete();
}

} // end namespace igstk

