/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkView2D.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkView2D.h"
#include "vtkInteractorStyleImage.h"

namespace igstk {

/** Constructor */
View2D::View2D() : m_StateMachine(this)
{
  vtkInteractorStyleImage * interactorStyle = vtkInteractorStyleImage::New();
  this->SetInteractorStyle( interactorStyle );
  interactorStyle->Delete();

  // initialize the orientation to be axial
  this->m_Orientation = Axial;
}

/** Destructor */
View2D::~View2D()
{
  this->SetInteractorStyle( NULL );
}

/** Print object information */
void View2D::PrintSelf( std::ostream& os, ::itk::Indent indent ) const
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Orientation type " << m_Orientation << std::endl;  
}


/** Select the orientation of the View */
void View2D::RequestSetOrientation( const OrientationType & orientation )
{
  igstkLogMacro( DEBUG, "igstk::View2D::RequestSetOrientation called ....\n");

  m_Orientation = orientation;

  double focalPoint[3];
  double position[3];

  for ( unsigned int cc = 0; cc < 3; cc++)
    {
    focalPoint[cc] = 0.0;
    position[cc]   = focalPoint[cc];
    }

  const double distanceToFocalPoint = 1000;

  switch( m_Orientation )
    {
    case Sagittal:
      {
      position[0] += distanceToFocalPoint;
      this->SetCameraViewUp (     0,  0,  1 );
      break;
      }
    case Coronal:
      {
      position[1] -= distanceToFocalPoint;
      this->SetCameraViewUp (  0,  0,  1 );
      break;
      }
    case Axial:
      {
      position[2] -= distanceToFocalPoint;
      this->SetCameraViewUp (     0,  -1,  0 );
      break;
      }
    }

  this->m_Camera->SetPosition (   position[0], position[1], position[2] );
  this->m_Camera->SetFocalPoint( focalPoint[0], focalPoint[1], focalPoint[2] );
  this->m_Camera->SetClippingRange( 0.1, 100000 );
  this->m_Camera->SetParallelProjection( true );
}

void View2D::SetCameraFocalPoint( double x, double y, double z )
{
  igstkLogMacro( DEBUG, "igstkView::SetCameraFocalPoint(...) called ...\n");

  double oldFocalPoint[3];
  double position[3];
  double offset[3];
  
  this->m_Camera->GetFocalPoint(oldFocalPoint);
  this->m_Camera->GetPosition(position);

  offset[0] = x - oldFocalPoint[0];
  offset[1] = y - oldFocalPoint[1];
  offset[2] = z - oldFocalPoint[2];

  for ( unsigned int cc = 0; cc < 3; cc++)
  {
    position[cc]   += offset[cc];
  }


  this->m_Camera->SetPosition(   position[0], position[1], position[2] );
  this->m_Camera->SetFocalPoint( x, y, z );
}

} // end namespace igstk
