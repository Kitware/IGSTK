/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkRenderWindowInteractor.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkRenderWindowInteractor_h
#define __igstkRenderWindowInteractor_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "vtkRenderWindowInteractor.h"

#include "igstkMacros.h"

namespace igstk
{


class RenderWindowInteractor : public vtkRenderWindowInteractor
{
public:

  static RenderWindowInteractor * New();

  vtkTypeRevisionMacro(RenderWindowInteractor,vtkRenderWindowInteractor);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void Initialize();


protected:
  RenderWindowInteractor();
  virtual ~RenderWindowInteractor();

private:
  RenderWindowInteractor(const RenderWindowInteractor&);  // Not implemented.
  void operator=(const RenderWindowInteractor&);  // Not implemented.
};


} // end namespace igstk


#endif
