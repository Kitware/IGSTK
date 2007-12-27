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

#include "igstkView.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"


namespace igstk {


/** \class ViewProxyBase
 *
 *  A proxy class designed to access private member data 
 *  of the view class.
 *
 *
 */
class ViewProxyBase 
{
public:

protected:
  vtkRenderer *               GetRenderer( View * view );
  vtkRenderWindowInteractor * GetRenderWindowInteractor( View * view );
  void                        InitializeInteractor( View * view );
  void                        SetRenderWindowSize( View * view, int width, int height );

private:

};

} // end namespace igstk

#endif 
