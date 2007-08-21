/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkViewProxy.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkViewProxy_h
#define __igstkViewProxy_h

#include "igstkViewNew.h"
#include "igstkViewProxyBase.h"

namespace igstk {


/** \class ViewProxy
 *
 *
 */

template < class T>
class ViewProxy : public ViewProxyBase 
{
public:

  typedef ViewProxy          Self;
  typedef ViewProxyBase      Superclass;

  igstkTypeMacro( ViewProxy, ViewProxyBase );

  ViewProxy( )
    {
    this->m_Widget = NULL;
    }

  ViewProxy( T * t)
    {
    this->m_Widget = t;
    }

  void Connect ( ViewNew * view )
    {
    vtkRenderer * renderer =
        ViewProxyBase::GetRenderer( view );

    vtkRenderWindowInteractor * interactor =
        ViewProxyBase::GetRenderWindowInteractor( view );

    ::itk::Object::Pointer reporter =
        ViewProxyBase::GetReporter( view );

    this->m_Widget->SetRenderer( renderer ); 
    this->m_Widget->SetRenderWindowInteractor( interactor ); 
    this->m_Widget->SetReporter ( reporter );

    ViewProxyBase::InitializeInteractor( view );
    }  

  void SetRenderWindowSize( ViewNew * view, int width, int height )
    {
    ViewProxyBase::SetRenderWindowSize( view, width, height );

    }


protected:

private:
    T  * m_Widget;

};

} // end namespace igstk

#endif 
