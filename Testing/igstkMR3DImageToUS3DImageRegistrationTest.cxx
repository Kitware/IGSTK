/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMR3DImageToUS3DImageRegistrationTest.cxx
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
//  Warning about: identifier was truncated to '255' characters 
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkEvents.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkObject.h"
#include "igstkUSImageReader.h"
#include "igstkMRImageReader.h"
#include "igstkMR3DImageToUS3DImageRegistration.h"
#include "igstkUltrasoundImageSimulator.h"
#include "igstkTransformObserver.h"

namespace MR3DImageToUS3DImageRegistrationTest
{
igstkObserverObjectMacro(USImage,::igstk::USImageReader::ImageModifiedEvent,
                         ::igstk::USImageObject)   
igstkObserverObjectMacro(MRImage,
    ::igstk::MRImageReader::ImageModifiedEvent,::igstk::MRImageSpatialObject)


// Simulate an US from the MR image
typedef igstk::UltrasoundImageSimulator<igstk::MRImageSpatialObject> 
                                                              USSimulatorType;
typedef USSimulatorType::ImageModifiedEvent        USImageModifiedEventType;
  
igstkObserverObjectMacro(SimulatedUSImage,
                         USImageModifiedEventType,
                         igstk::USImageObject)


}


int igstkMR3DImageToUS3DImageRegistrationTest( int argc, char * argv[] )
{
  if(argc<2)
    {
    std::cout << "Usage = " << argv[0] << " directory" << std::endl;
    return EXIT_FAILURE;
    }

  igstk::MRImageReader::Pointer MRReader = igstk::MRImageReader::New();
  MRReader->RequestSetDirectory(argv[1]);
  MRReader->RequestReadImage();

  typedef MR3DImageToUS3DImageRegistrationTest::MRImageObserver 
                                                        MRImageObserverType;
  MRImageObserverType::Pointer mrImageObserver = MRImageObserverType::New();
 
  MRReader->AddObserver(::igstk::MRImageReader::ImageModifiedEvent(),
                            mrImageObserver);

  MRReader->RequestGetImage();

  if(!mrImageObserver->GotMRImage())
    {
    std::cout << "No MRImage!" << std::endl;
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }

  // Simulate an US from the MR image
  typedef MR3DImageToUS3DImageRegistrationTest::USSimulatorType USSimulatorType;
  USSimulatorType::Pointer usSimulator = USSimulatorType::New();

  usSimulator->RequestSetImageGeometricModel(mrImageObserver->GetMRImage());

  typedef igstk::Transform TransformType;
  TransformType usTransform;
  TransformType::VectorType translation;
  translation.Fill(0);
  translation[0] = 3;
  translation[1] = 2;
  translation[2] = 1;
  usTransform.SetTranslation(translation,0,10000);

  usSimulator->RequestSetTransform(usTransform);
  usSimulator->RequestReslice();

  usSimulator->Print(std::cout);

  typedef MR3DImageToUS3DImageRegistrationTest::SimulatedUSImageObserver
                                                     SimulatedUSImageObserver;
  SimulatedUSImageObserver::Pointer usImageObserver 
                                            = SimulatedUSImageObserver::New();
  usSimulator->AddObserver(USSimulatorType::ImageModifiedEvent(),
                             usImageObserver);

  usSimulator->RequestGetImage();

  if(!usImageObserver->GotSimulatedUSImage())
    {
    std::cout << "No USImage!" << std::endl;
    return EXIT_FAILURE;
    }

  // Print the USImage
  igstk::USImageObject::Pointer usImage = 
                                       usImageObserver->GetSimulatedUSImage();

  typedef igstk::Logger                   LoggerType; 
  typedef itk::StdStreamLogOutput         LogOutputType;
  LoggerType::Pointer                     logger = LoggerType::New();
  LogOutputType::Pointer                  logOutput = LogOutputType::New();  

  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  igstk::MR3DImageToUS3DImageRegistration::Pointer registration = 
                              igstk::MR3DImageToUS3DImageRegistration::New();


  igstk::Transform initialTransform;
  typedef igstk::Transform::VectorType VectorType;

  // Initial transformation is equal to the final transformation
  // in order to ensure convergence
  VectorType regTranslation;
  regTranslation.Fill(0);
  regTranslation[0] = 3;
  regTranslation[1] = 2;
  regTranslation[2] = 1;
  initialTransform.SetTranslation(regTranslation,100000,0);

  //coverage stuff
  registration->SetLogger( logger );
  registration->Print( std::cout ); 
  registration->RequestGetRegistrationTransform(); 
  registration->RequestReset();

  registration->RequestSetMovingMR3D(mrImageObserver->GetMRImage());
  registration->RequestSetFixedUS3D(usImageObserver->GetSimulatedUSImage());
  registration->SetInitialTransform(initialTransform);
  registration->RequestCalculateRegistration();

  igstk::TransformObserver::Pointer registrationTransformObserver = 
    igstk::TransformObserver::New();

  registrationTransformObserver->ObserveTransformEventsFrom( registration );
  registrationTransformObserver->Clear();

  registration->RequestGetRegistrationTransform();

  if( registrationTransformObserver->GotTransform() )
    {
    igstk::Transform final = registrationTransformObserver->GetTransform();
    VectorType finalT = final.GetTranslation();
    finalT += initialTransform.GetTranslation();

    TransformType::VectorType errorVector;
    errorVector = finalT - translation;

    if(fabs(errorVector[0])>1 || fabs(errorVector[1])>1 || 
       fabs(errorVector[2])>1)
      {
      std::cout << "[FAILED] : " << std::endl;
      std::cout << "Final transform = " << finalT << std::endl;
      return EXIT_FAILURE;
      }
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
