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
  vtkRenderWindowInteractor::SetInteractorStyle( vtkInteractorStyleImage::New() );
}

/** Destructor */
View2D::~View2D()
{
}


/** Main FLTK event handler */
int View2D::handle( int event ) 
{
  return View::handle( event );
}


/** Print object information */
void View2D::PrintSelf( std::ostream& os, ::itk::Indent indent )
{
  this->Superclass::PrintSelf(os,indent);
}


} // end namespace igstk
