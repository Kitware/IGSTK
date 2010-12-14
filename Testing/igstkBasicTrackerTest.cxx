/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkBasicTrackerTest.cxx
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
// Warning about: identifier was truncated to '255' characters in the 
// debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include <fstream>

#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkCylinderObject.h"

#include "igstkTracker.h"
#include "igstkTrackerTool.h"
#include "igstkRealTimeClock.h"

namespace igstk
{

class TestingTracker;

class TestingTrackerTool : public igstk::TrackerTool
{
public:
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( TestingTrackerTool, TrackerTool )

  /** The "RequestAttachToTracker" method attaches 
   * the tracker tool to a tracker. */
  virtual void RequestAttachToTracker( TestingTracker *  tracker );

protected:
  TestingTrackerTool():m_StateMachine(this)
    {
    }

  ~TestingTrackerTool()
    {
    }

  /** Check if the tracker tool is configured or not. This method should
   *  be implemented in the derived classes*/
  virtual bool CheckIfTrackerToolIsConfigured( ) const { return true; }

};
 
class TestingTracker : public igstk::Tracker
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( TestingTracker, Tracker )

  typedef Superclass::TransformType      TransformType;
  typedef TransformType::VectorType      VectorType;


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

  TestingTracker():m_StateMachine(this)
    {
    igstk::Transform::VectorType    translation;
    igstk::Transform::VersorType    rotation;
    igstk::Transform                transform;
    translation[0] = -100;
    translation[1] = -100;
    translation[2] = -100;
    rotation.SetRotationAroundZ(0.1);
    transform.SetTranslationAndRotation(translation, rotation, 0.1, 10000);
    m_Translation = translation;
    }

  ~TestingTracker() 
    {
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

    typedef TrackerToolsContainerType::const_iterator  ConstIteratorType;

    TrackerToolsContainerType trackerToolContainer = 
      this->GetTrackerToolContainer();
   
    ConstIteratorType inputItr = trackerToolContainer.begin();
    ConstIteratorType inputEnd = trackerToolContainer.end();
 
    const double validityPeriod = 500.0; // valid for 500 milliseconds.

    TransformType transform;
    TransformType::ErrorType errorValue = 0.01; // 10 microns

    while( inputItr != inputEnd )
      {
      transform.SetTranslation( m_Translation, errorValue, validityPeriod );

      // set the raw transform
      this->SetTrackerToolRawTransform( 
        trackerToolContainer[inputItr->first], transform );

      this->SetTrackerToolTransformUpdate( 
        trackerToolContainer[inputItr->first], true );

      ++inputItr;
      }

    return SUCCESS;
    }
  Tracker::ResultType InternalThreadedUpdateStatus( void )
    {
    static bool success = false;
    if( !success )
      {
      success = true;
      return FAILURE;
      }
    return SUCCESS;
    }

  Tracker::ResultType VerifyTrackerToolInformation( 
    const TrackerToolType * itkNotUsed(trackerTool) )
    {
    static bool success = false;
    if( !success )
      {
      success = true;
      return FAILURE;
      }
    return SUCCESS;
    }

  Tracker::ResultType RemoveTrackerToolFromInternalDataContainers( 
    const TrackerToolType * itkNotUsed(trackerTool) )
    {
    static bool success = false;
    if( !success )
      {
      success = true;
      return FAILURE;
      }
    return SUCCESS;
    }

  Tracker::ResultType AddTrackerToolToInternalDataContainers( 
    const TrackerToolType * itkNotUsed(trackerTool) )
    {
    static bool success = false;
    if( !success )
      {
      success = true;
      return FAILURE;
      }
    return SUCCESS;
    }

private:

  igstk::Transform::VectorType m_Translation;
 
};

/** The "RequestAttachToTracker" method attaches 
* the tracker tool to a tracker. */
void TestingTrackerTool::RequestAttachToTracker( TestingTracker *  tracker )
{
  // This delegation is done only to enforce type matching between
  // TrackerTool and Tracker. It prevents the user from accidentally 
  // mix TrackerTools and Trackers of different type;
  this->TrackerTool::RequestAttachToTracker( tracker );
}

}   // namespace igstk

int igstkBasicTrackerTest( int, char * [] )
{

  igstk::RealTimeClock::Initialize();


  igstk::TestingTracker::Pointer tracker        = igstk::TestingTracker::New();
 
  typedef igstk::TestingTrackerTool          TrackerToolType;
  TrackerToolType::Pointer    tool           = TrackerToolType::New();
  TrackerToolType::Pointer    referenceTool  = TrackerToolType::New();

  typedef igstk::Object::LoggerType             LoggerType;
  LoggerType::Pointer logger = LoggerType::New();

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
  igstk::Transform transform;

  tool->RequestConfigure();
  tool->RequestAttachToTracker( tracker );
  referenceTool->RequestAttachToTracker( tracker );

  transform = tool->GetCalibrationTransform();
  std::cout << transform << std::endl;

  translation[0] = -2;
  translation[1] = -4;
  translation[2] = -6;
  rotation.SetRotationAroundX(-1.0);

  igstk::Transform toolCalibrationTransform;
  toolCalibrationTransform.SetTranslationAndRotation(translation, rotation, 
                                                                   0.1, 10000);
  tool->SetCalibrationTransform( toolCalibrationTransform );
  toolCalibrationTransform.SetToIdentity(10000);
  toolCalibrationTransform = tool->GetCalibrationTransform();
  std::cout << toolCalibrationTransform << std::endl;

  tracker->RequestSetReferenceTool( referenceTool );

  transform = referenceTool->GetCalibrationTransform();
  std::cout << transform << std::endl;

  igstk::CylinderObject::Pointer object = igstk::CylinderObject::New();
  object->SetRadius(1.0);
  object->SetHeight(1.0);

  igstk::Transform  identityTransform;
  identityTransform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
 
  // Connect the second ellipsoid to the tracker tool.
  object->RequestSetTransformAndParent( identityTransform, tool );

  tracker->RequestOpen();  // for failure
  tracker->RequestOpen();  // for success

  tracker->RequestStartTracking(); // for failure
  tracker->RequestStartTracking(); // for success

  tracker->RequestReset(); // for failure
  tracker->RequestReset(); // for success

  tracker->RequestStartTracking();
  
  tracker->RequestStopTracking();  // for failure
  tracker->RequestClose();   // for failure of DeactivateTools
  tracker->RequestClose();   // for failure of Close
  tracker->RequestClose();

  // for testing CloseFromToolsActiveStateProcessing()
  tracker->RequestOpen();
  tracker->RequestClose();

  std::cout << "[PASSED]" << std::endl;
  return EXIT_SUCCESS;
}
