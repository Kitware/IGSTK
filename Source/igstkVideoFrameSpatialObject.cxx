/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoFrameSpatialObject.cxx
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
#include "igstkVideoFrameSpatialObject.h"

#include "vtkImageData.h"

namespace igstk
{ 

VideoFrameSpatialObject::VideoFrameSpatialObject() :
  m_StateMachine(this)
{
  igstkLogMacro( DEBUG, "VideoFrameSpatialObject Constructor called ....\n" );
  
  VideoFrameSpatialObjectType::Pointer dummy = VideoFrameSpatialObjectType::New();
  this->RequestSetInternalSpatialObject(dummy);

  m_Width = 0;
  m_Height = 0;
  
  m_PixelSizeX = 0;
  m_PixelSizeY = 0;

  m_StateMachine.SetReadyToRun();
} 

VideoFrameSpatialObject::~VideoFrameSpatialObject()  
{
  igstkLogMacro( DEBUG, "VideoFrameSpatialObject Destructor called ....\n" );
}

void
VideoFrameSpatialObject::Initialize()  
{
  igstkLogMacro( DEBUG, "VideoFrameSpatialObject::Initialize called ....\n" );

  m_ImageData = vtkImageData::New();
  m_ImageData->SetScalarTypeToUnsignedChar();
  m_ImageData->SetSpacing (m_PixelSizeX, m_PixelSizeY, 1.0);
  m_ImageData->SetNumberOfScalarComponents( 1 );
  m_ImageData->SetExtent(0, m_Width - 1,
                         0, m_Height - 1, 0, 0);

  m_ImageData->AllocateScalars();
}

bool
VideoFrameSpatialObject::IsEmpty() const
{
  igstkLogMacro( DEBUG, "VideoFrameSpatialObject::IsEmpty called ....\n" );

  int npoints = m_ImageData->GetNumberOfPoints();

  if (npoints == 0)
    return true;
  else
    return false;
}

void
VideoFrameSpatialObject::RequestGetVTKImage() const
{
  igstkLogMacro( DEBUG, "VideoFrameSpatialObject::GetImageData called ....\n");

  VTKImageModifiedEvent  event;
  event.Set( m_ImageData );
  this->InvokeEvent( event );
}

void *
VideoFrameSpatialObject::GetImagePtr()
{
  igstkLogMacro( DEBUG, "VideoFrameSpatialObject::GetImagePtr called ....\n");

  return m_ImageData->GetScalarPointer();
}

} // end namespace igstk
