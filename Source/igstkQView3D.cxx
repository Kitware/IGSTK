/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkQView3D.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkQView3D.h"

#include "vtkInteractorStyleTrackballCamera.h"

namespace igstk {

/** Constructor */
#if QT_VERSION < 0x040000
  //! constructor for Qt 3
  QView3D::QView3D(QWidget* parent, const char* name, Qt::WFlags f)
    : QView(parent, name, f)
{
#else
  //! constructor for Qt 4
  QView3D::QView3D(QWidget* parent, Qt::WFlags f)
  : QView(parent, f)
{
#endif
  vtkInteractorStyleTrackballCamera * interactorStyle = 
                                     vtkInteractorStyleTrackballCamera::New();
  this->SetInteractorStyle( interactorStyle );
  interactorStyle->Delete();
}

/** Destructor */
QView3D::~QView3D()
{
  this->SetInteractorStyle( NULL );
}

/** Main FLTK event handler */
/*
int QView3D::handle( int event ) 
{
  return QView::handle( event );
}
*/

/** Print object information */
void QView3D::PrintSelf( std::ostream& os, ::itk::Indent indent ) const
{
  this->Superclass::PrintSelf(os,indent);
}


} // end namespace igstk
