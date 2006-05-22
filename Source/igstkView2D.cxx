/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkView2D.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkView2D.h"

#include "vtkInteractorStyleImage.h"

namespace igstk {

/** Constructor */
View2D::View2D( int x, int y, int w, int h, const char *l ) : View(x,y,w,h,l)
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


/** Main FLTK event handler */
int View2D::handle( int event ) 
{
  return View::handle( event );
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
      m_Camera->SetViewUp (     0,  0,  1 );
      break;
      }
    case Coronal:
      {
      position[1] -= distanceToFocalPoint;
      m_Camera->SetViewUp (     0,  0,  1 );
      break;
      }
    case Axial:
      {
      position[2] -= distanceToFocalPoint;
      m_Camera->SetViewUp (     0,  -1,  0 );
      break;
      }
    }

  m_Camera->SetPosition (   position );
  m_Camera->SetFocalPoint ( focalPoint );
  m_Camera->SetClippingRange( 0.1, 100000 );
  m_Camera->ParallelProjectionOn();
}


} // end namespace igstk
