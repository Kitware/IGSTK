/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisHardwareTest.cxx
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

#include "igstkTrackerHardwareTestHelper.h"
#include "igstkPolarisTracker.h"

#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForPosix.h"
#endif

int igstkPolarisHardwareTest( int argc, char* argv[] )
{

  if( argc < 3 )
    {
    std::cerr << "Missing arguments" << std::endl;
    std::cerr << "Usage : " << std::endl;
    std::cerr << argv[0] << "inputBaselineFile.txt logOutputFile.txt " << std::endl;
    return EXIT_FAILURE;
    }

  typedef igstk::TrackerHardwareTestHelper    TestHelperType;
  typedef igstk::PolarisTracker               TrackerType;

 #ifdef WIN32
  typedef igstk::SerialCommunicationForWindows  CommunicationType;
#else
  typedef igstk::SerialCommunicationForPosix    CommunicationType;
#endif
 
  TestHelperType::Pointer tester = TestHelperType::New();

  TrackerType::Pointer tracker = TrackerType::New();

  CommunicationType::Pointer communication = CommunicationType::New();
  communication->SetPortNumber( igstk::SerialCommunication::PortNumber0 );
  communication->SetParity( igstk::SerialCommunication::NoParity );
  communication->SetBaudRate( igstk::SerialCommunication::BaudRate9600 );
  communication->SetDataBits( igstk::SerialCommunication::DataBits8 );
  communication->SetStopBits( igstk::SerialCommunication::StopBits1 );
  communication->SetHardwareHandshake( 
      igstk::SerialCommunication::HandshakeOff );

  tracker->SetCommunication( communication );

  tester->SetTracker( tracker );
  tester->SetBaselineFilename( argv[1] );
  tester->SetLogOutputFilename( argv[2] );
  tester->Initialize();
  tester->Execute();
  tester->Report();

  return tester->GetFinalStatus();
}
