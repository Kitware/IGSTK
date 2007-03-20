/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkQView2D.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkQView2D.h"

#include "vtkInteractorStyleImage.h"

namespace igstk {

/** Constructor */
//QView2D::QView2D( int x, int y, int w, int h, const char *l ) : QView(x,y,w,h,l)

#if QT_VERSION < 0x040000
  //! constructor for Qt 3
  QView2D::QView2D(QWidget* parent, const char* name, Qt::WFlags f)
#else
  //! constructor for Qt 4
  QView2D::QView2D(QWidget* parent, Qt::WFlags f)
#endif
{
  vtkInteractorStyleImage * interactorStyle = vtkInteractorStyleImage::New();
  this->SetInteractorStyle( interactorStyle );
  interactorStyle->Delete();

  // initialize the orientation to be axial
  this->m_Orientation = Axial;
}

/** Destructor */
QView2D::~QView2D()
{
  this->SetInteractorStyle( NULL );
}


/** Main FLTK event handler */
/*
int QView2D::handle( int event ) 
{
  return QView::handle( event );
}
*/

/** Print object information */
void QView2D::PrintSelf( std::ostream& os, ::itk::Indent indent ) const
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Orientation type " << m_Orientation << std::endl;  
}


/** Select the orientation of the View */
void QView2D::RequestSetOrientation( const OrientationType & orientation )
{
  igstkLogMacro( DEBUG, "igstk::QView2D::RequestSetOrientation called ....\n");

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
