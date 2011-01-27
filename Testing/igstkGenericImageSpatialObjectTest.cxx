/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkGenericImageSpatialObjectTest.cxx
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
#pragma warning ( disable : 4786 )
#endif

#include "igstkGenericImageSpatialObject.h"

#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"


int igstkGenericImageSpatialObjectTest( int , char* [] )
{

  igstk::RealTimeClock::Initialize();


  typedef igstk::Object::LoggerType     LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  typedef igstk::GenericImageSpatialObject    ImageSpatialObjectType;
  
  ImageSpatialObjectType::Pointer genericImageSpatialObject = 
    ImageSpatialObjectType::New();

  return EXIT_SUCCESS;
}
