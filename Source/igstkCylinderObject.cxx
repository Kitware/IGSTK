/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkCylinderObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkCylinderObject.h"
#include <vtkPolyDataMapper.h>
#include <vtkCylinderSource.h>
#include <vtkProperty.h>
#include <vtkActor.h>

namespace igstk
{ 

/** Constructor */
CylinderObject::CylinderObject()
{
  // We create the ellipse spatial object
  m_CylinderSpatialObject = CylinderSOType::New();
  m_SpatialObject = m_CylinderSpatialObject;
} 

/** Destructor */
CylinderObject::~CylinderObject()  
{
}

/** Print Self function */
void CylinderObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

/** Set the radius of the cylinder */
void CylinderObject::SetRadius(double radius)
{
  m_CylinderSpatialObject->SetRadius(radius);
}

/** Get the radius of the cylinder */
double CylinderObject::GetRadius()
{
  return m_CylinderSpatialObject->GetRadius();
}

/** Set the Height of the cylinder */
void CylinderObject::SetHeight(double height)
{
  m_CylinderSpatialObject->SetHeight(height);
}

/** Get the Height of the cylinder */
double CylinderObject::GetHeight()
{
  return m_CylinderSpatialObject->GetHeight();
}


/** Create the vtk Actors */
void CylinderObject::CreateActors()
{
  vtkPolyDataMapper *cylinderMapper = vtkPolyDataMapper::New();
  vtkActor* cylinder = vtkActor::New();
  vtkCylinderSource* cylinderSource = vtkCylinderSource::New();
  cylinderSource->SetCenter(0, 0, 0);
  cylinderSource->SetRadius(m_CylinderSpatialObject->GetRadius());
  cylinderSource->SetHeight(m_CylinderSpatialObject->GetHeight());
  cylinderSource->SetResolution(10);
 
  cylinder->GetProperty()->SetColor(this->GetRed(),
                                     this->GetGreen(),
                                     this->GetBlue()); 
        
  cylinder->GetProperty()->SetOpacity(this->GetOpacity()); 
  cylinderMapper->SetInput(cylinderSource->GetOutput());
  cylinder->SetMapper( cylinderMapper );
  cylinderSource->Delete();

  // We should check if the actor doesn't exist
  this->AddActor( cylinder );
  cylinderMapper->Delete();
}

} // end namespace igstk

