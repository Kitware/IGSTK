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
  m_TubeSO = TubeSOType::New();
  this->GetSpatialObjects().push_back(m_TubeSO);
} 

/** Destructor */
CylinderObject::~CylinderObject()  
{
}

/** Set the radius value */
void CylinderObject::SetRadius(double radius)
{
  m_Radius = radius;
}

/** Get the radius */
double CylinderObject::GetRadius()
{
  return m_Radius;
}

/** Set the height value */
void CylinderObject::SetHeight(double height)
{
  m_Height = height;
}

/** Get the height */
double CylinderObject::GetHeight()
{
  return m_Height;
}

/** Print Self function */
void CylinderObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Radius = " << m_Radius << std::endl;
  os << indent << "Height = " << m_Height << std::endl;
}


/** Create the vtk Actors */
void CylinderObject::CreateVTKActors()
{
  vtkPolyDataMapper *cylinderMapper = vtkPolyDataMapper::New();
  vtkActor* cylinder = vtkActor::New();
  vtkCylinderSource* cylinderSource = vtkCylinderSource::New();
  cylinderSource->SetCenter(0, 0, 0);
  cylinderSource->SetRadius(m_Radius);
  cylinderSource->SetHeight(m_Height);
  cylinderSource->SetResolution(10);
 
  cylinder->GetProperty()->SetColor(this->GetProperty()->GetRed(),
                                     this->GetProperty()->GetGreen(),
                                     this->GetProperty()->GetBlue()); 
        
  cylinder->GetProperty()->SetOpacity(this->GetProperty()->GetOpacity()); 
  cylinderMapper->SetInput(cylinderSource->GetOutput());
  cylinder->SetMapper(cylinderMapper);
  cylinderSource->Delete();

  // We should check if the actor doesn't exist
  this->GetVTKActors().push_back(cylinder);
  cylinderMapper->Delete();
}

} // end namespace igstk

