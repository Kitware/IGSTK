/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkBasicTrackerTest.cxx
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
#include <fstream>

#include "igstkTracker.h"
#include "igstkLogger.h"

namespace igstk
{
  class TestingTracker : public igstk::Tracker
  {
    public:
      typedef TestingTracker                 Self;
      typedef itk::SmartPointer<Self>        Pointer;
      typedef itk::SmartPointer<const Self>  ConstPointer;

      igstkNewMacro(Self);
      igstkTypeMacro(TestingTracker,Tracker);

    protected:
      TestingTracker()
        {
        typedef igstk::TrackerTool  TrackerToolType;
        typedef igstk::TrackerPort  TrackerPortType;
        TrackerToolType::Pointer trackerTool = TrackerToolType::New();
        TrackerPortType::Pointer trackerPort = TrackerPortType::New();
        trackerPort->AddTool( trackerTool );
        this->AddPort( trackerPort );
        }
      ~TestingTracker() {}
  };
}


int igstkBasicTrackerTest( int, char * [] )
{

  igstk::TestingTracker::Pointer tracker = igstk::TestingTracker::New();
  igstk::Logger::Pointer logger = igstk::Logger::New();

  std::ofstream fileStream1("outputBasicTrackerTestLog1.txt");
  std::ofstream fileStream2("outputBasicTrackerTestLog2.txt");

  logger->AddOutputStream( std::cout );
  logger->AddOutputStream( fileStream1 );

  logger->SetPriorityLevel( igstk::Logger::DEBUG );

  tracker->SetLogger( logger );

  tracker->Initialize();

  tracker->StartTracking();

  tracker->UpdateStatus();

  tracker->Reset();

  tracker->StopTracking();

  std::cout << tracker->GetCurrentState() << std::endl;

  std::cout << "Testing the interaction with tools" << std::endl;
  const unsigned int toolNumber = 0;
  const unsigned int portNumber = 0;

  
  igstk::Tracker::TransformType transform;
  igstk::Tracker::TransformType::VectorType translation;
  translation[0] = 17;
  translation[1] = 19;
  translation[2] = 21;

  const double validityPeriod = 500.0; // valid for 500 milliseconds.

  std::cout << "Stored translation = " << translation << std::endl;
  transform.SetTranslation( translation, validityPeriod );
  tracker->SetToolTransform( toolNumber, portNumber, transform );

  igstk::Tracker::TransformType transform2;
  tracker->GetToolTransform( toolNumber, portNumber, transform2 );
  
  igstk::Tracker::TransformType::VectorType translation2;
  translation2 = transform2.GetTranslation();
  std::cout << "Retrieved translation = " << translation2 << std::endl;

  {
  double tolerance = 1e-5;
  for(unsigned int i=0; i<3; i++)
    {
    if( fabs( translation2[i] - translation[i] ) > tolerance )
      {
      std::cerr << "Recovered translation does not match the stored one" << std::endl;
      return EXIT_FAILURE;
      }
    }
  }

  tracker->Close();

  return EXIT_SUCCESS;
}


