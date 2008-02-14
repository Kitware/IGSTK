/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkDefaultWidget.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkDefaultWidget.h"
#include "vtkRenderWindow.h"

namespace igstk
{

DefaultWidget
::DefaultWidget(unsigned int width, unsigned int height):m_ProxyView(this)
{
  this->m_Width = width;
  this->m_Height = height;
}

void
DefaultWidget
::RequestSetView( ViewType * view )
{
  this->m_View = view;
  this->m_ProxyView.Connect( this->m_View );
  m_ProxyView.SetRenderWindowSize( 
    this->m_View, this->m_Width, this->m_Height );
}

void
DefaultWidget
::TestProxy()
{
  std::cout << this->m_ProxyView.GetNameOfClass() << std::endl;
  const double px = 1.3;
  const double py = 1.8;
  this->m_ProxyView.SetPickedPointCoordinates( this->m_View, px, py );
}

void 
DefaultWidget
::SetRenderer( vtkRenderer * )
{
}

void
DefaultWidget
::SetRenderWindowInteractor( vtkRenderWindowInteractor * iren ) 
{
  vtkRenderWindow* renWin = iren->GetRenderWindow();
  if (renWin != NULL)
    {
    renWin->BordersOn();
    renWin->SetWindowName( "IGSTK DefaultWidget" );
    }
}

} // end namespace igstk
