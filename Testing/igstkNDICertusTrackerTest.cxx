/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkNDICertusTrackerTest.cxx
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

#include <iostream>
#include <fstream>
#include <set>

#include "itkCommand.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkVector.h"
#include "itkVersor.h"

#include "igstkSystemInformation.h"
#include "igstkSerialCommunication.h"
#include "igstkNDICertusTracker.h"
#include "igstkNDICertusTrackerTool.h"
#include "igstkAuroraTrackerTool.h"
#include "igstkTransform.h"
#include "igstkTransformObserver.h"

class NDICertusTrackerTestCommand : public itk::Command 
{
public:
  typedef  NDICertusTrackerTestCommand   Self;
  typedef  itk::Command                Superclass;
  typedef itk::SmartPointer<Self>      Pointer;
  itkNewMacro( Self );
protected:
  NDICertusTrackerTestCommand() {};

public:
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
    // don't print "CompletedEvent", only print interesting events
    if (!igstk::CompletedEvent().CheckEvent(&event) &&
        !itk::DeleteEvent().CheckEvent(&event) )
      {
      std::cout << event.GetEventName() << std::endl;
      }
    }
};

int igstkNDICertusTrackerTest( int argc, char * argv[] )
{

  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType     LoggerType;
  typedef itk::StdStreamLogOutput       LogOutputType;
  typedef igstk::TransformObserver      ObserverType;

  if( argc < 4 )
    {
    std::cerr << " Usage: " << argv[0] << "\t" 
                            << "Logger_Output_filename "
                            << "Rig_filename "
							<< "Setup_filename"
                            << std::endl;
    return EXIT_FAILURE;
    }

  std::string rigidBodyFile = argv[2];
  
  std::string CertusSetupFile = argv[3];
  
  igstk::NDICertusTrackerTool::Pointer tool = igstk::NDICertusTrackerTool::New();

  
  NDICertusTrackerTestCommand::Pointer 
                                my_command = NDICertusTrackerTestCommand::New();

  std::string filename = argv[1];
    
  std::cout << "Logger output saved here:\n";
  std::cout << filename << "\n"; 

  std::ofstream loggerFile;
  loggerFile.open( filename.c_str() );
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();  
  logOutput->SetStream( loggerFile );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG);

  //Instantiate the tracker
  igstk::NDICertusTracker::Pointer  tracker;

  tracker = igstk::NDICertusTracker::New();

  tracker->AddObserver( itk::AnyEvent(), my_command);

  tracker->SetLogger( logger );

  //Set the ini file for the certus system
  tracker->SetIniFileName(CertusSetupFile.c_str());

  //Set the number of rigid bodies (tools)
  tracker->rigidBodyStatus.lnRigidBodies = 1;

  //Configure the rigid body - this is much easier using igstk::TrackerController and igstk::TrackerConfiguration classes
  tracker->rigidBodyDescrArray[0].lnStartMarker = 1;
  tracker->rigidBodyDescrArray[0].lnNumberOfMarkers = 4;
  strcpy(tracker->rigidBodyDescrArray[0].szName, rigidBodyFile.c_str());

  std::cout << "RequestOpen()" << std::endl;
  tracker->RequestOpen();

  typedef igstk::NDICertusTrackerTool         TrackerToolType;
  typedef TrackerToolType::TransformType    TransformType;

  // instantiate and attach wired tracker tool  
  TrackerToolType::Pointer trackerTool = TrackerToolType::New();
  trackerTool->SetLogger( logger );
  //Add observer to listen to events throw by the tracker tool
  trackerTool->AddObserver( itk::AnyEvent(), my_command);
  
  //Configure
  trackerTool->RequestSetRigidBodyName(rigidBodyFile.c_str());
  trackerTool->RequestConfigure();

  //Attach to the tracker
  trackerTool->RequestAttachToTracker( tracker );
 
  //Add observer to listen to transform events 
  ObserverType::Pointer coordSystemAObserver = ObserverType::New();
  coordSystemAObserver->ObserveTransformEventsFrom( trackerTool );

  // instantiate and attempt to attach aurora tracker tool. This attempt
  // will fail since one is not allowed to attach aurora tracker tool to a 
  // polaris tracker
  std::cout << "Instantiate aurora tracker tool: " << std::endl;
  typedef igstk::AuroraTrackerTool         AuroraTrackerToolType;
  AuroraTrackerToolType::Pointer trackerTool3 = AuroraTrackerToolType::New();
  trackerTool3->SetLogger( logger );
  //Add observer to listen to events throw by the tracker tool
  trackerTool3->AddObserver( itk::AnyEvent(), my_command);
  //Select wireless tracker tool
  trackerTool3->RequestSelect6DOFTrackerTool();
  //Configure
  trackerTool3->RequestConfigure();
  //Attach to the tracker
  trackerTool3->RequestAttachToTracker( tracker );

  //start tracking 
  tracker->RequestStartTracking();

  typedef igstk::Transform            TransformType;
  typedef ::itk::Vector<double, 3>    VectorType;
  typedef ::itk::Versor<double>       VersorType;


  for(unsigned int i=0; i<100; i++)
    {
    igstk::PulseGenerator::CheckTimeouts();

    TransformType             transform;
    VectorType                position;

    coordSystemAObserver->Clear();
    trackerTool->RequestGetTransformToParent();
    if (coordSystemAObserver->GotTransform())
      {
      transform = coordSystemAObserver->GetTransform();
      if ( transform.IsValidNow() ) 
        {
        position = transform.GetTranslation();
        std::cout << "Trackertool :" 
                << trackerTool->GetTrackerToolIdentifier() 
                << "\t\t  Position = (" << position[0]
                << "," << position[1] << "," << position[2]
                << ")" << std::endl;
        }
      }

    }
  
  std::cout << "RequestStopTracking()" << std::endl;
  tracker->RequestStopTracking();


  // restart tracking
  tracker->RequestStartTracking();

  for(unsigned int i=0; i<100; i++)
    {
    igstk::PulseGenerator::CheckTimeouts();

    TransformType             transform;
    VectorType                position;

    coordSystemAObserver->Clear();
    trackerTool->RequestGetTransformToParent();
    if (coordSystemAObserver->GotTransform())
      {
      transform = coordSystemAObserver->GetTransform();
      if ( transform.IsValidNow() ) 
        {
        position = transform.GetTranslation();
        std::cout << "Trackertool :" 
                << trackerTool->GetTrackerToolIdentifier() 
                << "\t\t  Position = (" << position[0]
                << "," << position[1] << "," << position[2]
                << ")" << std::endl;
        }
      }
    }
  
  std::cout << "RequestStopTracking()" << std::endl;
  tracker->RequestStopTracking();

  std::cout << "RequestClose()" << std::endl;
  tracker->RequestClose();


  return EXIT_SUCCESS;
}
