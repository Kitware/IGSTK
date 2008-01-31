/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkDefaultWidget.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkDefaultWidget_h
#define __igstkDefaultWidget_h

#if defined(_MSC_VER)
//  Warning about: identifier was truncated to '255' characters in the 
//  debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkViewProxy.h"
#include "igstkEvents.h"

namespace igstk {
  
// This is an ad-hoc Widget that emulate the actions
// of an actual GUI Widget.
class DefaultWidget 
{
public:
  typedef ::igstk::ViewProxy< DefaultWidget > ProxyType;
  typedef ::igstk::View                  ViewType;

  friend class ::igstk::ViewProxy< DefaultWidget >;

  // Constructor must connect the Widget to the Proxy.
  DefaultWidget(unsigned int width, unsigned int height):m_ProxyView(this)
    {
    this->m_Width = width;
    this->m_Height = height;
    }

  void RequestSetView( ViewType * view )
    {
    this->m_View = view;
    this->m_ProxyView.Connect( this->m_View );
    m_ProxyView.SetRenderWindowSize( 
      this->m_View, this->m_Width, this->m_Height );
    }

  // This method provides code coverage to the ProxyView
  void TestProxy()
    {
    std::cout << this->m_ProxyView.GetNameOfClass() << std::endl;
    const double px = 1.3;
    const double py = 1.8;
    this->m_ProxyView.SetPickedPointCoordinates( this->m_View, px, py );
    }

private:
  ProxyType           m_ProxyView;
  ViewType::Pointer   m_View; 

  unsigned int        m_Width;
  unsigned int        m_Height;

  // Fake methods needed to satisfy the API exposed to the ViewProxy
  void SetRenderer( vtkRenderer * ) {};
  void SetRenderWindowInteractor( vtkRenderWindowInteractor * ) {};
};

}  // end igstk namespace

#endif
