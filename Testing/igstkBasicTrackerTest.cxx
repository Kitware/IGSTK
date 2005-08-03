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

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkCylinderObject.h"

#include "igstkTracker.h"

namespace igstk
{
  class TestingTracker : public igstk::Tracker
  {
    public:
      typedef TestingTracker                 Self;
      typedef itk::SmartPointer<Self>        Pointer;
      typedef itk::SmartPointer<const Self>  ConstPointer;
      typedef igstk::Tracker::TransformType  TransformType;
      typedef TransformType::VectorType      VectorType;

      igstkNewMacro(Self);
      igstkTypeMacro(TestingTracker,Tracker);

      void SetTranslation(VectorType &translation)
        {
        m_Translation = translation;
        }

    protected:

      TestingTracker()
        {
        typedef TrackerTool  TrackerToolType;
        typedef TrackerPort  TrackerPortType;
        TrackerToolType::Pointer trackerTool = TrackerToolType::New();
        TrackerPortType::Pointer trackerPort = TrackerPortType::New();
        trackerPort->AddTool( trackerTool );
        this->AddPort( trackerPort );

        TrackerToolType::Pointer trackerTool2 = TrackerToolType::New();
        TrackerPortType::Pointer trackerPort2 = TrackerPortType::New();
        trackerPort2->AddTool( trackerTool2 );
        this->AddPort( trackerPort2 );
        igstk::Transform::VectorType translation;
        igstk::Transform::VersorType rotation;
        igstk::Transform transform;
        translation[0] = -100;
        translation[1] = -100;
        translation[2] = -100;
        rotation.SetRotationAroundZ(0.1);
        transform.SetTranslationAndRotation(translation, rotation, 0.1, 10000);
        SetToolTransform(1, 0, transform);
        }

      ~TestingTracker() 
        {
        this->ClearPorts();
        }

      Tracker::ResultType InternalOpen( void )
        {
        static bool success = false;
        if( !success )
          {
          success = true;
          return FAILURE;
          }
        return SUCCESS;
        }

      Tracker::ResultType InternalClose( void )
        {
        static bool success = false;
        if( !success )
          {
          success = true;
          return FAILURE;
          }
        return SUCCESS;
        }

      Tracker::ResultType InternalReset( void )
        {
        static bool success = false;
        if( !success )
          {
          success = true;
          return FAILURE;
          }
        return SUCCESS;
        }

      Tracker::ResultType InternalActivateTools( void )
        {
        static bool success = false;
        if( !success )
          {
          success = true;
          return FAILURE;
          }
        return SUCCESS;
        }

      Tracker::ResultType InternalDeactivateTools( void )
        {
        static bool success = false;
        if( !success )
          {
          success = true;
          return FAILURE;
          }
        return SUCCESS;
        }

      Tracker::ResultType InternalStartTracking( void )
        {
        static bool success = false;
        if( !success )
          {
          success = true;
          return FAILURE;
          }
        return SUCCESS;
        }

      Tracker::ResultType InternalStopTracking( void )
        {
        static bool success = false;
        if( !success )
          {
          success = true;
          return FAILURE;
          }
        return SUCCESS;
        }

      Tracker::ResultType InternalUpdateStatus( void )
        {
        static bool success = false;
        if( !success )
          {
          success = true;
          return FAILURE;
          }

        const unsigned int toolNumber = 0;
        const unsigned int portNumber = 0;
        const double validityPeriod = 500.0; // valid for 500 milliseconds.

        TransformType transform;
        TransformType::ErrorType errorValue = 0.01; // 10 microns

        transform.SetTranslation( m_Translation, errorValue, validityPeriod );
        this->SetToolTransform( toolNumber, portNumber, transform );

        return SUCCESS;
        }

    private:

      igstk::Transform::VectorType m_Translation;
 
  };
}   // namespace igstk

int igstkBasicTrackerTest( int, char * [] )
{

  igstk::TestingTracker::Pointer tracker = igstk::TestingTracker::New();
  itk::Logger::Pointer logger = itk::Logger::New();

  // create the outputs for the logger
  itk::StdStreamLogOutput::Pointer fileLogOutput1 = 
    itk::StdStreamLogOutput::New();
  itk::StdStreamLogOutput::Pointer consoleLogOutput =
    itk::StdStreamLogOutput::New();

  std::ofstream fileStream1("outputBasicTrackerTestLog1.txt");
  fileLogOutput1->SetStream( fileStream1 );
  consoleLogOutput->SetStream( std::cout );

  logger->SetPriorityLevel( itk::Logger::DEBUG );

  logger->AddLogOutput( fileLogOutput1 );
  logger->AddLogOutput( consoleLogOutput );

  tracker->SetLogger( logger );

  std::cout << tracker << std::endl;

  igstk::Transform::VectorType translation;
  igstk::Transform::VersorType rotation;
  igstk::Transform transform, patientTransform, toolCalibrationTransform;
  tracker->GetToolTransform(0, 0, transform);
  std::cout << transform << std::endl;

  translation[0] = 1;
  translation[1] = 2;
  translation[2] = 3;
  rotation.SetRotationAroundX(1.0);
  patientTransform.SetTranslationAndRotation(translation, rotation, 0.1, 10000);
  tracker->SetPatientTransform(patientTransform);
  patientTransform.SetToIdentity(10000);
  patientTransform = tracker->GetPatientTransform();
  std::cout << patientTransform << std::endl;

  translation[0] = -2;
  translation[1] = -4;
  translation[2] = -6;
  rotation.SetRotationAroundX(-1.0);
  toolCalibrationTransform.SetTranslationAndRotation(translation, rotation, 0.1, 10000);
  tracker->SetToolCalibrationTransform(toolCalibrationTransform);
  toolCalibrationTransform.SetToIdentity(10000);
  toolCalibrationTransform = tracker->GetToolCalibrationTransform();
  std::cout << toolCalibrationTransform << std::endl;

  tracker->SetReferenceTool(true, 1, 0);
  unsigned int refTool;
  unsigned int refPort;
  bool bApplyRefTool;
  bApplyRefTool = tracker->GetReferenceTool(refPort, refTool);
  if( bApplyRefTool != true  ||  refTool != 0  ||  refPort != 1 )
    {
    std::cerr << "SetReferenceTool() or GetReferenceTool() doesn't work correctly!" << std::endl;
    return EXIT_FAILURE;
    }

  tracker->GetToolTransform(0, 0, transform);
  std::cout << transform << std::endl;

  igstk::CylinderObject::Pointer object = igstk::CylinderObject::New();
  object->SetRadius(1.0);
  object->SetHeight(1.0);
  const unsigned int toolNumber = 0;
  const unsigned int portNumber = 0;
  tracker->AttachObjectToTrackerTool(portNumber, toolNumber, object);

  tracker->Open();  // for failure
  tracker->Open();  // for success

  tracker->Initialize();  // for failure
  tracker->Initialize();  // for success

  tracker->StartTracking(); // for failure
  tracker->StartTracking(); // for success

  tracker->UpdateStatus();  // for failure
  tracker->UpdateStatus();  // for success

  tracker->UpdateStatus();

  tracker->Reset(); // for failure
  tracker->Reset(); // for success

  tracker->StopTracking();  // for failure
  tracker->Close();   // for failure of DeactivateTools
  tracker->Close();   // for failure of Close
  tracker->Close();

  // for testing CloseFromToolsActiveStateProcessing()
  tracker->Open();
  tracker->Initialize();
  tracker->Close();

  // covering the base tracker's internal functions
  igstk::Tracker::Pointer basetracker = igstk::Tracker::New();
  basetracker->SetLogger( logger );

  basetracker->Open();

  basetracker->Initialize();

  basetracker->StartTracking();

  basetracker->UpdateStatus();
  basetracker->Reset();

  basetracker->StopTracking();
  basetracker->Close();

  std::cout << "[PASSED]" << std::endl;
  return EXIT_SUCCESS;
}

