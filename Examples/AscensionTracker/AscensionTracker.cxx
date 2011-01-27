/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    AscensionTracker.cxx
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

// BeginLatex
// 
// This example illustrates how to use IGSTK's Tracker component to communicate 
// with Ascension trackers (FlockOfBird or 3D Guidance) and gather tool transform 
// information. These trackers are DC magnetic systems.
//
// EndLatex


#include <iostream>
#include <fstream>
#include <set>

#include "itkCommand.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkVector.h"
#include "itkVersor.h"
#include "igstkAxesObject.h"

// BeginLatex
// 
// To communicate with AscensionTracker tracking device, include the AscensionTracker
// header files \doxygen{AscensionTracker} and \doxygen{AscensionTrackerTool}
//
// EndLatex

// BeginCodeSnippet
#include "igstkAscensionTracker.h"
#include "igstkAscensionTrackerTool.h"
// EndCodeSnippet
//
#include "igstkTransform.h"

#include "igstkTransformObserver.h"

// BeginLatex
// Define a callback class to listen to events invoked by the tracker class. 
// EndLatex 
//
// BeginCodeSnippet
class AscensionTrackerCommand : public itk::Command 
{
public:
  typedef  AscensionTrackerCommand   Self;
  typedef  itk::Command                     Superclass;
  typedef  itk::SmartPointer<Self>          Pointer;
  itkNewMacro( Self );
protected:
  AscensionTrackerCommand() {};

public:
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object *  itkNotUsed(object), const itk::EventObject & event)
    {
    // don't print "CompletedEvent", only print interesting events
if (!igstk::CompletedEvent().CheckEvent(&event) &&
        !itk::DeleteEvent().CheckEvent(&event) )
      {
      std::cout << event.GetEventName() << std::endl;
      }
    }
};
//EndCodeSnippet

// BeginLatex
// 
// \doxygen{TrackerToolUpdateTransformObserver} is used for listening to updated
// transformations from \doxygen{AscensionTrackerTool} 
//
// EndLatex

// BeginCodeSnippet
class TrackerToolUpdateTransformObserver : public itk::Command 
{
public:
  typedef TrackerToolUpdateTransformObserver   Self;
  typedef itk::Command                 Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  itkNewMacro( Self );

protected:
  TrackerToolUpdateTransformObserver() {};

  typedef igstk::Transform::PointType                   PointType;

public:
  void SetTrackerTool(igstk::TrackerTool *tool)
  {
    this->m_TrackerTool = tool;
  }

  void SetWorldReference(igstk::AxesObject *reference)
  {
    this->m_WorldReference = reference;
  }

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * itkNotUsed(object), const itk::EventObject & event)
    {
      if ( igstk::TrackerToolTransformUpdateEvent().CheckEvent( & event ) )
      {
        typedef igstk::TransformObserver ObserverType;
        ObserverType::Pointer transformObserver = ObserverType::New();
        transformObserver->ObserveTransformEventsFrom( m_TrackerTool );
        transformObserver->Clear();
        
        m_TrackerTool->RequestComputeTransformTo( m_WorldReference );
        
        if ( transformObserver->GotTransform() )
        {   
            igstk::Transform transform = transformObserver->GetTransform();
            PointType point = TransformToPoint( transform );
            std::cout << "position: " << point[0] << " " << point[1] << " " << point[2] << std::endl;
        }
      }
    }

  protected:

  PointType TransformToPoint( igstk::Transform transform)
  {
    PointType point;
    for (unsigned int i=0; i<3; i++)
      {
      point[i] = transform.GetTranslation()[i];
      }
    return point;
  }

  /** tracker tool */
  igstk::TrackerTool::Pointer                           m_TrackerTool;
  /** world reference */
  igstk::AxesObject::Pointer                            m_WorldReference;
};
//EndCodeSnippet

int main( int argc, char * argv[] )
{

  igstk::RealTimeClock::Initialize();

  typedef igstk::Object::LoggerType   LoggerType;
  typedef itk::StdStreamLogOutput     LogOutputType;

  if( argc < 3 )
    {
    std::cerr << " Usage: " << argv[0] << "\t" 
                            << "Logger_Output_filename "
                            << "Serial_communication_port"
                            << std::endl;
    return EXIT_FAILURE;
    }

//BeginLatex
//Instantiate a serial communication object.
//EndLatex
//
//
//BeginCodeSnippet
  igstk::SerialCommunication::Pointer 
                     serialComm = igstk::SerialCommunication::New();
//EndCodeSnippet

//BeginLatex
//Instantiate tracker command callback object.
//EndLatex
//
//
//BeginCodeSnippet
  AscensionTrackerCommand::Pointer 
                                my_command = AscensionTrackerCommand::New();
//EndCodeSnippet

//  typedef igstk::TransformObserver   ObserverType;
 
//BeginLatex
//Setup a logger and add it to the serial communication object
//EndLatex
//
//
//BeginCodeSnippet
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

  serialComm->AddObserver( itk::AnyEvent(), my_command);
  serialComm->SetLogger( logger );
//EndCodeSnippet
//
//BeginLatex
//Setup the serial communication parameters
//EndLatex
//
//
//BeginCodeSnippet
  typedef igstk::SerialCommunication::PortNumberType PortNumberType; 
  unsigned int birdNumberIntegerValue = atoi(argv[2]);
  PortNumberType  portNumber = PortNumberType(birdNumberIntegerValue); 
  serialComm->SetPortNumber( portNumber );
  serialComm->SetParity( igstk::SerialCommunication::NoParity );
  serialComm->SetBaudRate( igstk::SerialCommunication::BaudRate115200 );
  serialComm->SetDataBits( igstk::SerialCommunication::DataBits8 );
  serialComm->SetStopBits( igstk::SerialCommunication::StopBits1 );
  serialComm->SetHardwareHandshake( igstk::SerialCommunication::HandshakeOff );
  serialComm->SetCaptureFileName( "RecordedStreamByAscensionTrackerTest.txt" );
  serialComm->SetCapture( true );

  if( !serialComm->OpenCommunication())
  {
      std::cout << "Serial port open failure\n";
      return EXIT_FAILURE;
  }
//EndCodeSnippet

//BeginLatex
//Instantiate tracker object
//EndLatex
//
//
//BeginCodeSnippet
  igstk::AscensionTracker::Pointer  tracker;
  tracker = igstk::AscensionTracker::New();
//EndCodeSnippet

//BeginLatex
//Setup tracker object: add logger, generic observer, 
//communication object and transform update
//observer
//EndLatex
//
//
//BeginCodeSnippet
  tracker->AddObserver( itk::AnyEvent(), my_command);
  tracker->SetLogger( logger );

  std::cout << "SetCommunication()" << std::endl;
  tracker->SetCommunication( serialComm );
//EndCodeSnippet
//
//BeginLatex
//Open tracker
//EndLatex
//
//
//BeginCodeSnippet
  std::cout << "RequestOpen()" << std::endl;
  tracker->RequestOpen();
//EndCodeSnippet

//
//BeginLatex
//Instantiate, configure and attach a tracker tool to the tracker. Add observer 
//to listen to events thrown by the tracker tool
//EndLatex
//
//
//BeginCodeSnippet
  typedef igstk::AscensionTrackerTool       TrackerToolType;
  TrackerToolType::Pointer trackerTool = TrackerToolType::New();
  trackerTool->SetLogger( logger );

  trackerTool->RequestSetPortNumber(1);
  trackerTool->RequestConfigure();
  trackerTool->RequestAttachToTracker( tracker );
  trackerTool->AddObserver( itk::AnyEvent(), my_command);

  TrackerToolUpdateTransformObserver::Pointer trackerToolUpdateTransformObserver = 
                                          TrackerToolUpdateTransformObserver::New();

  trackerToolUpdateTransformObserver->SetTrackerTool( trackerTool );  
//EndCodeSnippet

 //BeginLatex
 //Instantiate a world reference and set the UpdateTransformObserver to calculate transforms
 //with respect to it
 //EndLatex
 //
 //
 //BeginCodeSnippet
  igstk::AxesObject::Pointer worldReference        = igstk::AxesObject::New();
  trackerToolUpdateTransformObserver->SetWorldReference( worldReference );
  tracker->AddObserver( igstk::TrackerToolTransformUpdateEvent(), trackerToolUpdateTransformObserver );
 //EndCodeSnippet

 //
 //BeginLatex
 //Start tracking
 //EndLatex
 //
 //
 //BeginCodeSnippet
  std::cout << "Start tracking..." << std::endl;
  tracker->RequestStartTracking();
 //EndCodeSnippet

 //EndCodeSnippet
 //
 //BeginLatex
 //Stop tracking and close the communication with the tracker
 //EndLatex
 //
 //
 //BeginCodeSnippet
  std::cout << "RequestStopTracking()" << std::endl;
  tracker->RequestStopTracking();

  std::cout << "RequestClose()" << std::endl;
  tracker->RequestClose();
 
  std::cout << "CloseCommunication()" << std::endl;
  serialComm->CloseCommunication();
 //EndCodeSnippet

  std::cout << "[PASSED]" << std::endl;

  return EXIT_SUCCESS;

}
