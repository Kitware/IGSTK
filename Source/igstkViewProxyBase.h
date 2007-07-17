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

#include "igstkViewNew.h"
#include "vtkRenderer.h"

namespace igstk {


/** \class ViewProxyBase
 *
 *
 */
class ViewProxyBase 
{
public:

protected:
  vtkRenderer * Connect( ViewNew * view );

private:

};

} // end namespace igstk

#endif 
