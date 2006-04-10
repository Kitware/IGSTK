/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMR3DImageToUS3DImageRegistrationTest.cxx
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
//  Warning about: identifier was truncated to '255' characters 
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkEvents.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkObject.h"
#include "igstkUSImageReader.h"
#include "igstkMRImageReader.h"
#include "igstkMR3DImageToUS3DImageRegistration.h"

namespace MR3DImageToUS3DImageRegistrationTest
{
igstkObserverMacro(RegistrationTransform,
                   ::igstk::TransformModifiedEvent,::igstk::Transform)
igstkObserverObjectMacro(USImage,
                  ::igstk::USImageReader::ImageModifiedEvent,::igstk::USImageObject)   
igstkObserverObjectMacro(MRImage,
                  ::igstk::MRImageReader::ImageModifiedEvent,::igstk::MRImageSpatialObject)
}


int igstkMR3DImageToUS3DImageRegistrationTest( int argc, char * argv[] )
{
 /* if(argc<2)
    {
    std::cout << "Usage = " << argv[0] << " calibrationFile" << std::endl;
    return EXIT_FAILURE;
    }*/

  igstk::USImageReader::Pointer USReader = igstk::USImageReader::New();
  USReader->RequestSetDirectory("C:/Julien/Data/UltrasounReconstruction/09-Dec-04/Julien/MR/DICOM/MR3Slices");
  
  typedef MR3DImageToUS3DImageRegistrationTest::USImageObserver USImageObserverType;
  USImageObserverType::Pointer usImageObserver 
                                    = USImageObserverType::New();

  USReader->RequestReadImage();
 
  USReader->AddObserver(::igstk::USImageReader::ImageModifiedEvent(),
                            usImageObserver);

  USReader->RequestGetImage();

  if(!usImageObserver->GotUSImage())
    {
    std::cout << "No USImage!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

  igstk::MRImageReader::Pointer MRReader = igstk::MRImageReader::New();
  MRReader->RequestSetDirectory("C:/Julien/Data/UltrasounReconstruction/09-Dec-04/Julien/MR/DICOM/MR50Slices");
  MRReader->RequestReadImage();
  
  typedef MR3DImageToUS3DImageRegistrationTest::MRImageObserver MRImageObserverType;
  MRImageObserverType::Pointer mrImageObserver 
                                    = MRImageObserverType::New();
 
  MRReader->AddObserver(::igstk::MRImageReader::ImageModifiedEvent(),
                            mrImageObserver);

  MRReader->RequestGetImage();

  if(!mrImageObserver->GotMRImage())
    {
    std::cout << "No MRImage!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::Logger                   LoggerType; 
  typedef itk::StdStreamLogOutput       LogOutputType;
  LoggerType::Pointer                   logger = LoggerType::New();
  LogOutputType::Pointer                logOutput = LogOutputType::New();  

  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  igstk::MR3DImageToUS3DImageRegistration::Pointer registration = 
                              igstk::MR3DImageToUS3DImageRegistration::New();

  registration->SetLogger(logger);
  registration->RequestSetMovingMR3D(mrImageObserver->GetMRImage());
  registration->RequestSetFixedUS3D(usImageObserver->GetUSImage()); 
  registration->RequestCalculateRegistration();

  typedef MR3DImageToUS3DImageRegistrationTest::RegistrationTransformObserver 
                                            RegistrationTransformObserverType;
  RegistrationTransformObserverType::Pointer registrationTransformObserver 
                                    = RegistrationTransformObserverType::New();

  registration->AddObserver(::igstk::TransformModifiedEvent(),
                            registrationTransformObserver);
  registration->RequestGetRegistrationTransform();

  if(registrationTransformObserver->GotRegistrationTransform())
    {
    std::cout << "Got Transform! " << std::endl;
    std::cout << registrationTransformObserver->GetRegistrationTransform() 
              << std::endl;
    }
  else
    {
    std::cout << "No Transform!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  return EXIT_SUCCESS;
}
