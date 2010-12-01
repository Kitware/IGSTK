/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTrackerToolObserverToOpenIGTLinkRelayTest.cxx
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


#include "igstkRealTimeClock.h"
#include "igstkSystemInformation.h"
#include "igstkSerialCommunication.h"
#include "igstkAuroraTracker.h"
#include "igstkSimulatedTrackerTool.h"
#include "igstkTrackerToolObserverToOpenIGTLinkRelay.h"


int igstkAuroraTrackerToolObserverToOpenIGTLinkRelayTest( int argc, char * argv [] )
{

  if( argc < 6 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " hostname portnumber numberOfTransformsToSend auroraPortNumber framesPerSecond" << std::endl;
    return EXIT_FAILURE;
    }

  igstk::RealTimeClock::Initialize();

  typedef igstk::AuroraTracker                         TrackerType;
  typedef igstk::AuroraTrackerTool                     TrackerToolType;
  typedef igstk::TrackerToolObserverToOpenIGTLinkRelay       ObserverType;

  TrackerType::Pointer      tracker      = TrackerType::New();
  TrackerToolType::Pointer  trackerTool  = TrackerToolType::New();
  ObserverType::Pointer     toolObserver = ObserverType::New();

  igstk::SerialCommunication::Pointer 
                     serialComm = igstk::SerialCommunication::New();

  typedef igstk::SerialCommunication::PortNumberType PortNumberType; 
  unsigned int portNumberIntegerValue = atoi(argv[4]);
  PortNumberType  polarisPortNumber = PortNumberType(portNumberIntegerValue); 
  serialComm->SetPortNumber( polarisPortNumber );
  serialComm->SetParity( igstk::SerialCommunication::NoParity );
  serialComm->SetBaudRate( igstk::SerialCommunication::BaudRate115200 );
  serialComm->SetDataBits( igstk::SerialCommunication::DataBits8 );
  serialComm->SetStopBits( igstk::SerialCommunication::StopBits1 );
  serialComm->SetHardwareHandshake( igstk::SerialCommunication::HandshakeOff );

  serialComm->SetCaptureFileName( "RecordedStreamByAuroraTrackerTest.txt" );
  serialComm->SetCapture( true );

  serialComm->OpenCommunication();

  tracker->SetCommunication( serialComm );

  double frequency = atof( argv[5] ); 
  tracker->RequestSetFrequency( frequency );
  tracker->RequestOpen();

  trackerTool->RequestSelect6DOFTrackerTool();
  trackerTool->RequestSetPortNumber( portNumberIntegerValue );
  trackerTool->RequestConfigure();

  trackerTool->RequestAttachToTracker( tracker );

  toolObserver->RequestSetTrackerTool( trackerTool );
  toolObserver->RequestSetHostName( argv[1] );
  toolObserver->RequestSetPort( atoi( argv[2] ) );
  toolObserver->RequestStart();

  tracker->RequestStartTracking();

  const unsigned int numberOfTransformsToSend = atoi( argv[3] );

  for( unsigned int i = 0; i < numberOfTransformsToSend; i++ )
    {
    igstk::PulseGenerator::Sleep(10);
    igstk::PulseGenerator::CheckTimeouts();
    }

  tracker->RequestStopTracking();
  tracker->RequestReset();
  tracker->RequestClose();

  return EXIT_SUCCESS;
}
