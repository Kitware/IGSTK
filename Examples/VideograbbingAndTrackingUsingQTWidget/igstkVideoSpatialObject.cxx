/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoSpatialObject.cxx
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
#include "igstkVideoSpatialObject.h"

namespace igstk
{ 

VideoSpatialObject::VideoSpatialObject() :
  m_StateMachine(this)
{
  igstkLogMacro( DEBUG, "VideoSpatialObject Constructor called ....\n" );
  
  VideoSpatialObjectType::Pointer dummy = VideoSpatialObjectType::New();
  this->RequestSetInternalSpatialObject(dummy);
} 

VideoSpatialObject::~VideoSpatialObject()  
{
  igstkLogMacro( DEBUG, "VideoSpatialObject Destructor called ....\n" );
}

} // end namespace igstk
