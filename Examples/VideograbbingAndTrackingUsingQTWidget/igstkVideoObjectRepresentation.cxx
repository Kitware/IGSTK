/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoObjectRepresentation.cxx
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
#include "igstkVideoObjectRepresentation.h"

#include <vtkProperty.h>
#include <vtkImageActor.h>
#include <vtkTexture.h>
#include <vtkActor.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>

namespace igstk
{

VideoObjectRepresentation::VideoObjectRepresentation(void) :
  m_StateMachine(this),
  m_ImageData(NULL),
  m_ImageActor(NULL),
  m_PlaneSource(NULL),
  m_VideoMapper(NULL),
  m_VideoTexture(NULL)
{
  igstkLogMacro( DEBUG, "VideoObjectRepresentation constructor called ...\n");
  
  m_StateMachine.SetReadyToRun();
}

VideoObjectRepresentation::~VideoObjectRepresentation(void)
{
  igstkLogMacro( DEBUG, "VideoObjectRepresentation destructor called ...\n");
}

/** Set the Video Spatial Object */
void VideoObjectRepresentation
::RequestSetVideoSpatialObject( const VideoSpatialObject * spatialObject )
{
  igstkLogMacro( DEBUG,  "VideoObjectRepresentation::"
                 "RequestSetVideoSpatialObject called ....\n" );
  
  this->RequestSetSpatialObject( spatialObject );
}

void VideoObjectRepresentation::CreateActors()
{
  igstkLogMacro( DEBUG, "igstk::VideoObjectRepresentation"
                 "::CreateActors called...\n");
  
  m_VideoGrabber = QuickTimeGrabber::New();
  
  unsigned int m_VideoOutputFormat = 8;
  VideoGrabber::VideoDimensions m_VideoOutputDimensions;
  m_VideoOutputDimensions.m_Height = 576;
  m_VideoOutputDimensions.m_Width = 768;
  
  // Set paremeters for testing
  m_VideoGrabber->RequestSetVideoBufferSize(100);
  m_VideoGrabber->RequestSetVideoOutputDimensions(m_VideoOutputDimensions.
                                                  m_Height, 
                                                  m_VideoOutputDimensions.
                                                  m_Width);
  m_VideoGrabber->RequestSetVideoOutputFormat(m_VideoOutputFormat);
  
  m_ImageData = vtkImageData::New();
  m_ImageData->SetScalarTypeToUnsignedChar();
  m_ImageData->SetNumberOfScalarComponents(m_VideoOutputFormat / 8);
  m_ImageData->SetExtent(0, m_VideoOutputDimensions.m_Width - 1,
                         0, m_VideoOutputDimensions.m_Height - 1, 0, 0);
  m_ImageData->AllocateScalars();

  m_VideoGrabber->SetVideoMemoryPtr(m_ImageData->GetScalarPointer());
  
  // To avoid duplicates we clean the previous actors
  this->DeleteActors();
    
  m_ImageActor = vtkActor::New();
  m_ImageActor->SetPosition(0,0,0);
  m_ImageActor->SetOrientation(0,0,0);
  m_ImageActor->GetProperty()->SetOpacity(1.0);
  
  this->AddActor( m_ImageActor );
  
  m_VideoTexture = vtkTexture::New();
  m_VideoTexture->SetInput(m_ImageData);
  
  m_PlaneSource = vtkPlaneSource::New();
  m_PlaneSource->SetPoint1(200.0, 0.0, 0.0);
  m_PlaneSource->SetPoint2(0.0, 200.0 , 0.0);
  m_VideoMapper = vtkPolyDataMapper::New();
  m_VideoMapper->SetInput(m_PlaneSource->GetOutput());
  
  m_ImageActor->SetTexture(m_VideoTexture);
  m_ImageActor->SetMapper(m_VideoMapper);
    
  // Start grabbing
  m_VideoGrabber->RequestInitialize();
  m_VideoGrabber->RequestOpen();
  
  m_StateMachine.ProcessInputs(); 
}


/** Start video grabbing */
void VideoObjectRepresentation::StartGrabbing()
{
  m_VideoGrabber->RequestStartGrabbing();
}

/** Stop video grabbing */
void VideoObjectRepresentation::StopGrabbing()
{
  m_VideoGrabber->RequestStopGrabbing();
}

void VideoObjectRepresentation::UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoObjectRepresentation"
                 "::UpdateRepresentationProcessing called...\n");
 
  // Allow grabber time to process video stream
  m_VideoGrabber->ProcessVideo();
  m_ImageData->Modified();
}
} // namespace
