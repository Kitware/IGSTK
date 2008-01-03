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
#include "igstkSerialCommunication.h"

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
  typedef igstk::PolarisTrackerTool           TrackerToolType;
  typedef igstk::SerialCommunication          CommunicationType;
 
  TestHelperType::Pointer tester = TestHelperType::New();

  TrackerType::Pointer tracker = TrackerType::New();

  TrackerToolType::Pointer trackerTool = TrackerToolType::New();

  const unsigned int toolPort = 0;
  trackerTool->RequestSetPort( toolPort );

  CommunicationType::Pointer communication = CommunicationType::New();
  communication->SetPortNumber( igstk::SerialCommunication::PortNumber0 );
  communication->SetParity( igstk::SerialCommunication::NoParity );
  communication->SetBaudRate( igstk::SerialCommunication::BaudRate115200 );
  communication->SetDataBits( igstk::SerialCommunication::DataBits8 );
  communication->SetStopBits( igstk::SerialCommunication::StopBits1 );
  communication->SetHardwareHandshake( 
      igstk::SerialCommunication::HandshakeOff );

  tracker->SetCommunication( communication );

  tester->SetTracker( tracker );
  tester->SetTrackerTool( trackerTool );

  tester->SetBaselineFilename( argv[1] );
  tester->SetLogOutputFilename( argv[2] );

  tester->Initialize();

  
  unsigned int numberOfPositions    = tester->GetNumberOfPositionsToTest();
  unsigned int numberOfOrientations = tester->GetNumberOfOrientationsToTest();

  std::cout << "Starting to grab Positions" << std::endl;
  while( tester->GetNumberOfPositionsCollected() <
         tester->GetNumberOfPositionsToTest()  )
    {
    tester->GrabOnePosition();
    }
  std::cout << "End of positions collection" << std::endl;
  std::cout << std::endl;

  std::cout << "Starting to grab Orientations" << std::endl;
  while( tester->GetNumberOfOrientationsCollected() < 
         tester->GetNumberOfOrientationsToTest()  )
    {
    tester->GrabOneOrientation();
    }
  std::cout << "End of orientations collection" << std::endl;
  std::cout << std::endl;


  tester->Finalize();
  tester->Report();

  return tester->GetFinalStatus();
}
