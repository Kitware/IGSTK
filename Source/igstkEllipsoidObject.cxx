/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkEllipsoidObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkEllipsoidObject.h"
#include <vtkPolyDataMapper.h>
#include <vtkSuperquadricSource.h>
#include <vtkProperty.h>
#include <vtkActor.h>

namespace igstk
{ 

/** Constructor */
EllipsoidObject::EllipsoidObject()
{
  // We create the ellipse spatial object
  m_EllipseSpatialObject = EllipseSpatialObjectType::New();
  m_SpatialObject = m_EllipseSpatialObject;
} 

/** Destructor */
EllipsoidObject::~EllipsoidObject()  
{
}


/** Set all radii to the same radius value */
void 
EllipsoidObject::SetRadius( double radius )
{
  m_EllipseSpatialObject->SetRadius(radius);
}


void EllipsoidObject::SetRadius( double r0, double r1, double r2 )
{
  EllipseSpatialObjectType::ArrayType radius;
  radius[0] = r0;
  radius[1] = r1;
  radius[2] = r2;
  m_EllipseSpatialObject->SetRadius(radius);
}


/** Get the radii of the ellipsoid */
EllipsoidObject::ArrayType 
EllipsoidObject::GetRadius()
{
  return m_EllipseSpatialObject->GetRadius();
}


/** Print Self function */
void EllipsoidObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << m_EllipseSpatialObject << std::endl; 
}


/** Create the vtk Actors */
void EllipsoidObject::CreateActors()
{
  vtkPolyDataMapper *ellipsoidMapper = vtkPolyDataMapper::New();
  vtkActor* ellipsoid = vtkActor::New();
  vtkSuperquadricSource* ellipsoidSource = vtkSuperquadricSource::New();
  ellipsoidSource->SetCenter(0, 0, 0);
  ellipsoidSource->SetScale( m_EllipseSpatialObject->GetRadius()[0],
                             m_EllipseSpatialObject->GetRadius()[1],
                             m_EllipseSpatialObject->GetRadius()[2]);
  ellipsoidSource->SetThetaResolution( 10 );
  ellipsoidSource->SetPhiResolution( 10 );
 
  ellipsoid->GetProperty()->SetColor(this->GetRed(),
                                     this->GetGreen(),
                                     this->GetBlue());

  ellipsoid->GetProperty()->SetOpacity(this->GetOpacity());

  ellipsoidMapper->SetInput( ellipsoidSource->GetOutput() );
  ellipsoid->SetMapper(ellipsoidMapper);
  ellipsoidSource->Delete();

  // We should check if the actor doesn't exist
  this->AddActor( ellipsoid );
  ellipsoidMapper->Delete();
}

} // end namespace igstk

