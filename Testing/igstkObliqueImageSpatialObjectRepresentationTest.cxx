/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkObliqueImageSpatialObjectRepresentationTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "igstkObliqueImageSpatialObjectRepresentation.h"
#include "igstkImageSpatialObject.h"
#include "igstkView2D.h"
#include "igstkCTImageReader.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkEvents.h"

int igstkObliqueImageSpatialObjectRepresentationTest( int argc , char * argv [] )
{

  igstk::RealTimeClock::Initialize();

  typedef short    PixelType;
  const unsigned int Dimension = 3;

  typedef igstk::ImageSpatialObject< 
                                PixelType, 
                                Dimension 
                                       > ImageSpatialObjectType;
  
  typedef igstk::ObliqueImageSpatialObjectRepresentation< 
                                  ImageSpatialObjectType 
                                                      >   RepresentationType;

  RepresentationType::Pointer  representation = RepresentationType::New();

  typedef itk::Logger              LoggerType;
  typedef itk::StdStreamLogOutput  LogOutputType;
  
  
  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  return EXIT_SUCCESS;
}

