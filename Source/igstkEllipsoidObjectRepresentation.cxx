/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkEllipsoidObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkEllipsoidObjectRepresentation.h"
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
EllipsoidObjectRepresentation::EllipsoidObjectRepresentation()
{
  // We create the ellipse spatial object
  m_EllipsoidObject = NULL;
  this->SetSpatialObject( m_EllipsoidObject );
  m_EllipsoidSource = vtkSuperquadricSource::New();
} 

/** Destructor */
EllipsoidObjectRepresentation::~EllipsoidObjectRepresentation()  
{
  if( m_EllipsoidSource )
    {
    m_EllipsoidSource->Delete();
    m_EllipsoidSource = NULL;
    }
}



/** Print Self function */
void EllipsoidObjectRepresentation::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  if(m_EllipsoidObject)
    { 
    os << indent << m_EllipsoidObject << std::endl; 
    }
}


/** Set the Ellipsoidal Spatial Object */
void EllipsoidObjectRepresentation::SetEllipsoid( const EllipsoidObjectType * ellipsoid )
{
  // We create the ellipse spatial object
  m_EllipsoidObject = ellipsoid;
  this->SetSpatialObject( m_EllipsoidObject );

  if( m_EllipsoidSource != NULL )
    {
    m_EllipsoidSource->SetCenter(0, 0, 0);

    EllipsoidObjectType::ArrayType radius = m_EllipsoidObject->GetRadius();
    m_EllipsoidSource->SetScale( radius[0], radius[1], radius[2] );
    m_EllipsoidSource->SetThetaResolution( 10 );
    m_EllipsoidSource->SetPhiResolution( 10 );
    }
} 


/** Update the visual representation in response to changes in the geometric
 * object */
void EllipsoidObjectRepresentation::UpdateRepresentationFromGeometry()
{
  EllipsoidObjectType::ArrayType radius = m_EllipsoidObject->GetRadius();
  m_EllipsoidSource->SetScale( radius[0], radius[1], radius[2] );
}


/** Create the vtk Actors */
void EllipsoidObjectRepresentation::CreateActors()
{
  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  vtkPolyDataMapper *ellipsoidMapper = vtkPolyDataMapper::New();
  vtkActor* ellipsoidActor = vtkActor::New();
  m_EllipsoidSource->SetCenter(0, 0, 0);

  ellipsoidActor->GetProperty()->SetColor(this->GetRed(),
                                          this->GetGreen(),
                                          this->GetBlue());

  ellipsoidActor->GetProperty()->SetOpacity(this->GetOpacity());

  ellipsoidMapper->SetInput( m_EllipsoidSource->GetOutput() );
  ellipsoidActor->SetMapper(ellipsoidMapper);

  // We should check if the actor doesn't exist
  this->AddActor( ellipsoidActor );
  ellipsoidMapper->Delete();
}

} // end namespace igstk

