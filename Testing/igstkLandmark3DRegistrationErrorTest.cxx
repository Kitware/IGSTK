/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmark3DRegistrationErrorTest.cxx
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
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include "igstkLandmark3DRegistrationError.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkMacro.h"

int igstkLandmark3DRegistrationErrorTest( int argv, char * argc[] )
{
    std::cout << "Testing igstk::Landmark3DRegistration" << std::endl;

    typedef itk::Logger                   LoggerType;
    typedef itk::StdStreamLogOutput       LogOutputType;
    
    typedef igstk::Landmark3DRegistrationError     
                              Landmark3DRegistrationErrorType;
    typedef igstk::Landmark3DRegistrationError::LandmarkPointContainerType
                              LandmarkPointContainerType;
    typedef igstk::Landmark3DRegistrationError::LandmarkPointType 
                              LandmarkPointType;
    
    Landmark3DRegistrationErrorType::Pointer landmarkRegisterError = 
                                        Landmark3DRegistrationErrorType::New();    

    LandmarkPointContainerType  ipointcontainer;
    LandmarkPointContainerType  tpointcontainer;
    LandmarkPointType           LandmarkPoint;
    
    // logger object
    LoggerType::Pointer   logger = LoggerType::New();
    LogOutputType::Pointer logOutput = LogOutputType::New();
    logOutput->SetStream( std::cout );
    logger->AddLogOutput( logOutput );
    logger->SetPriorityLevel( itk::Logger::DEBUG );
    landmarkRegisterError->SetLogger( logger );

    // Add 1st landmark
    LandmarkPoint[0] =  25.0;
    LandmarkPoint[1] =  1.0;
    LandmarkPoint[2] =  15.0;
    ipointcontainer.push_back( LandmarkPoint );

    // Add 2nd landmark
    LandmarkPoint[0] =  15.0;
    LandmarkPoint[1] =  21.0;
    LandmarkPoint[2] =  17.0;
    ipointcontainer.push_back( LandmarkPoint );
    
    // Add 3d landmark
    LandmarkPoint[0] =  14.0;
    LandmarkPoint[1] =  25.0;
    LandmarkPoint[2] =  11.0;
    ipointcontainer.push_back( LandmarkPoint );
 
    // Add 4th landmark
    LandmarkPoint[0] =  10.0;
    LandmarkPoint[1] =  11.0;
    LandmarkPoint[2] =  8.0;
    ipointcontainer.push_back( LandmarkPoint );

    landmarkRegisterError->SetImageLandmarks( ipointcontainer );

    landmarkRegisterError->Print(std::cout);
    
   return EXIT_SUCCESS;
}


