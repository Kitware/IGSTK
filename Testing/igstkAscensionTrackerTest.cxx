/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkAscensionTrackerTest.cxx
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
#include "igstkSerialCommunicationSimulator.h"
#include "igstkAscensionTracker.h"
#include "igstkAscensionTrackerTool.h"
#include "igstkTransform.h"
#include "igstkTransformObserver.h"
#include "igstkAxesObject.h"

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

  void Execute(const itk::Object * object, const itk::EventObject & event)
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
    for (int i=0; i<3; i++)
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

class AscensionTrackerTestCommand : public itk::Command 
{
public:
  typedef  AscensionTrackerTestCommand   Self;
  typedef  itk::Command                 Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  itkNewMacro( Self );
protected:
  AscensionTrackerTestCommand() {};

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

/** This program tests single bird tracker tool */
int igstkAscensionTrackerTest( int argc, char * argv[] )
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


  igstk::AscensionTrackerTool::Pointer tool = igstk::AscensionTrackerTool::New();

  igstk::SerialCommunication::Pointer 
                     serialComm = igstk::SerialCommunication::New();

  AscensionTrackerTestCommand::Pointer 
                                my_command = AscensionTrackerTestCommand::New();

  typedef igstk::TransformObserver   ObserverType;
 
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

  typedef igstk::SerialCommunication::PortNumberType PortNumberType; 
  unsigned int fobNumberIntegerValue = atoi(argv[2]);
  PortNumberType  fobPortNumber = PortNumberType(fobNumberIntegerValue); 
  serialComm->SetPortNumber( fobPortNumber );
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

  igstk::AscensionTracker::Pointer  tracker;

  tracker = igstk::AscensionTracker::New();

  tracker->AddObserver( itk::AnyEvent(), my_command);

  tracker->SetLogger( logger );

  std::cout << "SetCommunication()" << std::endl;
  tracker->SetCommunication( serialComm );

  std::cout << "RequestOpen()" << std::endl;
  tracker->RequestOpen();

  typedef igstk::AscensionTrackerTool       TrackerToolType;
  typedef TrackerToolType::TransformType    TransformType;

  // instantiate and attach tracker tool  
  TrackerToolType::Pointer trackerTool = TrackerToolType::New();
  trackerTool->SetLogger( logger );

  //Configure
  trackerTool->RequestSetPortNumber(1);
  trackerTool->RequestConfigure();
  //Attach to the tracker
  trackerTool->RequestAttachToTracker( tracker );
  //Add observer to listen to events thrown by the tracker tool
  trackerTool->AddObserver( itk::AnyEvent(), my_command);

  TrackerToolUpdateTransformObserver::Pointer trackerToolUpdateTransformObserver = 
                                          TrackerToolUpdateTransformObserver::New();

  trackerToolUpdateTransformObserver->SetTrackerTool( trackerTool );  

  // Instantiate a world reference
  igstk::AxesObject::Pointer worldReference        = igstk::AxesObject::New();

  trackerToolUpdateTransformObserver->SetWorldReference( worldReference );

  tracker->AddObserver( igstk::TrackerToolTransformUpdateEvent(), trackerToolUpdateTransformObserver );

  std::cout << "Start tracking..." << std::endl;
  tracker->RequestStartTracking();

  std::cout << "RequestStopTracking()" << std::endl;
  tracker->RequestStopTracking();

  std::cout << "RequestClose()" << std::endl;
  tracker->RequestClose();

  std::cout << "CloseCommunication()" << std::endl;
  serialComm->CloseCommunication();

  std::cout << "[PASSED]" << std::endl;

  return EXIT_SUCCESS;
}
