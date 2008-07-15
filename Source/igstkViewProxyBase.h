/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkViewProxyBase.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkViewProxyBase_h
#define __igstkViewProxyBase_h

class vtkRenderer;
class vtkRenderWindowInteractor;

namespace igstk {

class View;

/** \class ViewProxyBase
 *
 *  \brief A base class for widget type templated proxy classes. 
 *
 *  A proxy class designed to access private member data of 
 *  the view class. This class is a friend class of the View
 *  class.
 *
 * \sa ViewProxy
 * \ingroup View
 *
 */
class ViewProxyBase 
{
public:

protected:
  /** Get the VTK renderer */ 
  vtkRenderer *               GetRenderer( View * view );

  /** Get the VTK RenderWindow interactor */
  vtkRenderWindowInteractor * GetRenderWindowInteractor( View * view );

  /** Initialize VTK RenderWindow interactor */
  void InitializeInteractor( View * view );

  /** Set VTK RenderWindow Size */
  void SetRenderWindowSize( View * view, int width, int height );

  /** Set PickedPoint coordinates */
  void SetPickedPointCoordinates( View * view, 
                                  double xPickedPoint ,
                                  double yPickedPoint );
private:

};

} // end namespace igstk

#endif 
