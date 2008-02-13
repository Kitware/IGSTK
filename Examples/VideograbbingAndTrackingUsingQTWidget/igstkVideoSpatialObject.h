/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoSpatialObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

  Made by SINTEF Health Research - Medical technology:
  http://www.sintef.no/medtech

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkVideoSpatialObject_h
#define __igstkVideoSpatialObject_h

#include "igstkMacros.h"
#include "igstkSpatialObject.h"

namespace igstk
{


class VideoSpatialObject 
: public SpatialObject
{

public:
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( VideoSpatialObject, SpatialObject )
  
  typedef itk::SpatialObject<3>          VideoSpatialObjectType;
  
protected:
  VideoSpatialObject( void );
  ~VideoSpatialObject( void );
  
private:
};

} // end namespace igstk

#endif // __igstkVideoSpatialObject_h
