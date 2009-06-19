/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoFrameRepresentationTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include "igstkLandmark3DRegistration.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkObject.h"
#include "itkCommand.h"
#include "itkMacro.h"
#include "igstkEvents.h"
#include "igstkVideoFrameRepresentation.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkAxesObject.h"
#include "igstkAxesObjectRepresentation.h"

#ifdef ConnectObjectToRepresentationMacro
#undef ConnectObjectToRepresentationMacro
#endif

#define ConnectObjectToRepresentationMacro( object, representation ) \
  representation->RequestSetVideoFrameSpatialObject( object );

#include <iostream>
#include "igstkLandmark3DRegistration.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkObject.h"
#include "itkCommand.h"
#include "itkMacro.h"
#include "igstkEvents.h"
#include "igstkVideoFrameSpatialObject.h"
#include "igstkVideoFrameRepresentation.h"
#include "igstkSpatialObjectTestHelper.h"

#define VIEW_3D_REFRESH_RATE 25
#define VIDEOIMAGER_DEFAULT_REFRESH_RATE 25

int igstkVideoFrameRepresentationTest( int argv, char * argc[] )
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput     LogOutputType;

  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  logger->SetPriorityLevel( itk::Logger::CRITICAL );

  LogOutputType::Pointer logOutput = LogOutputType::New();

  std::ofstream outputLogFile;
  if( argv > 1 )
    {
    outputLogFile.open( argc[1] );
    logOutput->SetStream( outputLogFile );
    logger->AddLogOutput( logOutput );
    }

  /** Instantiate the world reference */
  igstk::AxesObject::Pointer m_WorldReference;
  m_WorldReference = igstk::AxesObject::New();

  typedef igstk::VideoFrameSpatialObject<unsigned char, 1 >
                                             VideoFrameSpatialObjectType;
  VideoFrameSpatialObjectType::Pointer      m_VideoFrame;
  m_VideoFrame = VideoFrameSpatialObjectType::New();
  m_VideoFrame->SetWidth(256);
  m_VideoFrame->SetHeight(256);
  m_VideoFrame->SetPixelSizeX(1);
  m_VideoFrame->SetPixelSizeY(1);
  m_VideoFrame->SetNumberOfScalarComponents(1);
  m_VideoFrame->Initialize();

  igstk::Transform identity;
  identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  m_VideoFrame->RequestSetTransformAndParent( identity, m_WorldReference );
  
  typedef igstk::VideoFrameRepresentation<VideoFrameSpatialObjectType>
                                            VideoFrameRepresentationType;

  VideoFrameRepresentationType::Pointer m_VideoFrameRepresentationForVideoView;

  m_VideoFrameRepresentationForVideoView = VideoFrameRepresentationType::New();

  m_VideoFrameRepresentationForVideoView->
                       RequestSetVideoFrameSpatialObject( m_VideoFrame );

  return EXIT_SUCCESS; 
}
