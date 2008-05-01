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

#include "igstkView.h"
#include "igstkViewProxyBase.h"

namespace igstk {


/** \class ViewProxy
 *
 *  \brief Link widget classes with the view class.
 *
 *  This class is a a proxy class designed to access private member
 *  data of the view class and pass the values to widget classes. This 
 *  class is templated over widget type.
 *
 * \sa View
 *
 * \ingroup View
 * \ingroup Object
 */

template < class WidgetType>
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

  ViewProxy( WidgetType * widget )
    {
    this->m_Widget = widget;
    }

  virtual ~ViewProxy() {}
    
  /** Connect the widget with the view */
  void Connect ( View * view )
    {
    vtkRenderer * renderer =
        ViewProxyBase::GetRenderer( view );

    vtkRenderWindowInteractor * interactor =
        ViewProxyBase::GetRenderWindowInteractor( view );

    this->m_Widget->SetRenderer( renderer ); 
    this->m_Widget->SetRenderWindowInteractor( interactor ); 
     
    // There is no need to call Widget->RequestSetView( view ) 
    // because this Connect() method is normally called as a 
    // consequence of calling Widget->RequestSetView(). In other
    // words, the view is already set in the Widget by the time
    // the Connect method is called.

    ViewProxyBase::InitializeInteractor( view );
    }  

  /** Set the RenderWindow size */
  void SetRenderWindowSize( View * view, int width, int height )
    {
    ViewProxyBase::SetRenderWindowSize( view, width, height );

    }

  /** Set the PickedPoint coordinates */
  void SetPickedPointCoordinates( View * view, double x, double y ) 
    {
    ViewProxyBase::SetPickedPointCoordinates( view, x, y );
    }


protected:

private:
  WidgetType  * m_Widget;

};

} // end namespace igstk

#endif 
