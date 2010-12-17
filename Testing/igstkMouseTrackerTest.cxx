/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMouseTrackerTest.cxx
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

#include <FL/Fl.H>
#include <FL/Fl_Window.H>

#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "igstkMouseTracker.h"
#include "igstkMouseTrackerTool.h"

#include "igstkTransform.h"
#include "igstkTransformObserver.h"


class MouseTrackerTestCommand : public itk::Command 
{
public:
  typedef MouseTrackerTestCommand     Self;
  typedef itk::Command                Superclass;
  typedef itk::SmartPointer<Self>     Pointer;
  itkNewMacro( Self );
protected:
  MouseTrackerTestCommand() {};

public:
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * itkNotUsed(object), const itk::EventObject & event)
    {
    // don't print "CompletedEvent", only print interesting events
    if (!igstk::CompletedEvent().CheckEvent(&event) &&
        !itk::DeleteEvent().CheckEvent(&event) )
      {
      std::cout << event.GetEventName() << std::endl;
      }
    }
};

  
int igstkMouseTrackerTest( int, char * [] )
{

  igstk::RealTimeClock::Initialize();

  typedef igstk::Transform             TransformType;
  typedef igstk::MouseTracker          MouseTrackerType;
  typedef igstk::Object::LoggerType    LoggerType;
  typedef itk::StdStreamLogOutput      LogOutputType;
  typedef igstk::TransformObserver     ObserverType;

  // logger object created for logging mouse activities
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();  
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  MouseTrackerTestCommand::Pointer 
                                my_command = MouseTrackerTestCommand::New();

 
  MouseTrackerType::Pointer tracker = MouseTrackerType::New();
  tracker->SetLogger( logger );

  // Test SetScaleFactor()/GetScaleFactor() methods
  const double scale = 1.0;
  tracker->SetScaleFactor( scale );
  const double scaleReturned = tracker->GetScaleFactor();
  if( fabs( scale - scaleReturned ) > 1e-5 )
    {
    std::cerr << "Error in SetScaleFactor()/GetScaleFactor() " << std::endl;
    return EXIT_FAILURE;
    }

  tracker->Print( std::cout );
  tracker->RequestOpen();

  typedef igstk::MouseTrackerTool           TrackerToolType;
  typedef TrackerToolType::TransformType    TransformType;

  // instantiate and attach wired tracker tool  
  TrackerToolType::Pointer trackerTool = TrackerToolType::New();
  trackerTool->SetLogger( logger );
  std::string mouseName = "PS/s";
  trackerTool->RequestSetMouseName( mouseName );
  if ( mouseName != trackerTool->GetMouseName() )
    {
    std::cerr << "MouseName did not match!" << std::endl;
    return EXIT_FAILURE;
    }
  //Configure
  trackerTool->RequestConfigure();
  //Attach to the tracker
  trackerTool->RequestAttachToTracker( tracker );
  //Add observer to listen to events throw by the tracker tool
  trackerTool->AddObserver( itk::AnyEvent(), my_command);
  ObserverType::Pointer coordSystemAObserver = ObserverType::New();
  coordSystemAObserver->ObserveTransformEventsFrom( trackerTool );

  tracker->RequestStartTracking();

  typedef igstk::Transform            TransformType;
  typedef ::itk::Vector<double, 3>    VectorType;
  typedef ::itk::Versor<double>       VersorType;


  for(unsigned int i=0; i<20; i++)
    {
    TransformType             transform;
    VectorType                position;

    coordSystemAObserver->Clear();
    trackerTool->RequestGetTransformToParent();
    if (coordSystemAObserver->GotTransform())
      {
      transform = coordSystemAObserver->GetTransform();
      position = transform.GetTranslation();
      std::cout << "Mouse :" 
              << trackerTool->GetTrackerToolIdentifier() 
              << "\t\t  Position = (" << position[0]
              << "," << position[1] << "," << position[2]
              << ")" << std::endl;
      }
    }
  
  tracker->RequestStopTracking();
  
  tracker->RequestClose();

  //coverage
  tracker->RequestReset();

  return EXIT_SUCCESS;
}
