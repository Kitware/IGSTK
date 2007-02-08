/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkBasicTrackerTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters in the 
// debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include <fstream>

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkCylinderObject.h"

#include "igstkTracker.h"
#include "igstkRealTimeClock.h"

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

  bool TestThreadingEnableMethods()
    {
    this->SetThreadingEnabled(true);
    if( !this->GetThreadingEnabled() )
      {
      return false;
      }
    this->SetThreadingEnabled(false);
    if( this->GetThreadingEnabled() )
      {
      return false;
      }
    return true; 
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
    igstk::Transform::VectorType   translation;
    igstk::Transform::VersorType rotation;
    igstk::Transform             transform;
    translation[0] = -100;
    translation[1] = -100;
    translation[2] = -100;
    rotation.SetRotationAroundZ(0.1);
    transform.SetTranslationAndRotation(translation, rotation, 0.1, 10000);
    SetToolTransform(1, 0, transform);
    m_Translation = translation;
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

  igstk::RealTimeClock::Initialize();


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

  // Exercise the "ThreadingEnabling" method"
  bool status = tracker->TestThreadingEnableMethods();
  if( status == false )
    {
    std::cerr << " ERROR: inconsitency between ";
    std::cerr << "SetThreadingEnabled() and GetThreadingEnabled() " 
              << std::endl; 
    return EXIT_FAILURE;
    }

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
  toolCalibrationTransform.SetTranslationAndRotation(translation, rotation, 
                                                                   0.1, 10000);
  tracker->SetToolCalibrationTransform(0, 0, toolCalibrationTransform);
  toolCalibrationTransform.SetToIdentity(10000);
  toolCalibrationTransform = tracker->GetToolCalibrationTransform(0, 0);
  std::cout << toolCalibrationTransform << std::endl;

  tracker->SetReferenceTool(false, 1, 0);
  tracker->SetReferenceTool(true, 1, 0);
  unsigned int refTool;
  unsigned int refPort;
  bool bApplyRefTool;
  bApplyRefTool = tracker->GetReferenceTool(refPort, refTool);
  std::cout << "ReferenceTool flag : " << bApplyRefTool << "\t port: " 
            << refPort << ", tool: " << refTool << std::endl;
  if( bApplyRefTool != true  ||  refTool != 0  ||  refPort != 1 )
    {
    std::cerr << "SetReferenceTool() or GetReferenceTool() doesn't"
              << " work correctly!" << std::endl;
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

  tracker->RequestOpen();  // for failure
  tracker->RequestOpen();  // for success

  tracker->RequestInitialize();  // for failure
  tracker->RequestInitialize();  // for success

  tracker->RequestStartTracking(); // for failure
  tracker->RequestStartTracking(); // for success

  tracker->RequestUpdateStatus();  // for failure
  tracker->RequestUpdateStatus();  // for success

  tracker->RequestUpdateStatus();

  tracker->RequestReset(); // for failure
  tracker->RequestReset(); // for success

  tracker->RequestInitialize();
  tracker->RequestStartTracking();
  
  tracker->RequestStopTracking();  // for failure
  tracker->RequestClose();   // for failure of DeactivateTools
  tracker->RequestClose();   // for failure of Close
  tracker->RequestClose();

  // for testing CloseFromToolsActiveStateProcessing()
  tracker->RequestOpen();
  tracker->RequestInitialize();
  tracker->RequestClose();

  // covering the base tracker's internal functions
  igstk::Tracker::Pointer basetracker = igstk::Tracker::New();
  basetracker->SetLogger( logger );

  basetracker->RequestOpen();

  basetracker->RequestInitialize();

  basetracker->RequestStartTracking();

  basetracker->RequestUpdateStatus();
  basetracker->RequestReset();
  basetracker->RequestInitialize();
  basetracker->RequestStartTracking();
  basetracker->RequestStopTracking();
  basetracker->RequestClose();

  std::cout << "[PASSED]" << std::endl;
  return EXIT_SUCCESS;
}
