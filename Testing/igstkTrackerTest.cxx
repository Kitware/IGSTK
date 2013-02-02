/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerTest.cxx
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

#include "igstkTracker.h"
#include "igstkTrackerTool.h"

#define igstkEventOccurredMacro( name, EventType ) \
  class name##EventOccurredObserver : public ::itk::Command \
  { \
  public: \
    typedef  name##EventOccurredObserver  Self; \
    typedef  ::itk::Command               Superclass;\
    typedef  ::itk::SmartPointer<Self>    Pointer;\
    itkNewMacro( Self );\
  public:\
    void Execute(itk::Object *caller, const itk::EventObject & event)\
      {\
      const itk::Object * constCaller = caller;\
      this->Execute( constCaller, event );\
      }\
    void Execute(const itk::Object *itkNotUsed(caller), const itk::EventObject & event)\
      {\
      if( EventType().CheckEvent( &event ) )\
        {\
          m_EventOccurred = true;\
        }\
      }\
    bool EventOccured() const\
      {\
      return m_EventOccurred;\
      }\
    void Reset() \
      {\
      m_EventOccurred = false; \
      }\
  protected:\
    name##EventOccurredObserver() \
      {\
      m_EventOccurred = false;\
      }\
    ~name##EventOccurredObserver() {}\
  private:\
    bool m_EventOccurred;\
  };

namespace igstk
{

namespace TrackerTest
{

class DummyTracker;

class DummyTrackerTool : public igstk::TrackerTool
{
public:
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( DummyTrackerTool, TrackerTool )

  /** The "RequestAttachToTracker" method attaches
   * the tracker tool to a tracker. */
  virtual void RequestAttachToTracker( DummyTracker * );

protected:
  DummyTrackerTool():m_StateMachine(this)
    {
    }
  ~DummyTrackerTool()
    {
    }

  /** Check if the tracker tool is configured or not. This method should
   *  be implemented in the derived classes*/
  virtual bool CheckIfTrackerToolIsConfigured( ) const { return true; }
};

class DummyTracker : public Tracker
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( DummyTracker, Tracker )

  typedef Superclass::TransformType           TransformType;
  typedef Superclass::ResultType              ResultType;

  double GetTransformValidityTime()
    {
    return this->GetValidityTime();
    }

protected:

  DummyTracker():m_StateMachine(this)
    {
    }

  ~DummyTracker()
    {
    }

  ResultType InternalOpen( void )
    {
    return SUCCESS;
    }

  ResultType InternalStartTracking( void )
    {
    return SUCCESS;
    }

  ResultType InternalReset( void )
    {
    return SUCCESS;
    }

  ResultType InternalStopTracking( void )
    {
    return SUCCESS;
    }

  ResultType InternalClose( void )
    {
    return SUCCESS;
    }

  ResultType
  VerifyTrackerToolInformation( const TrackerToolType * itkNotUsed(trackerTool) )
    {
    return SUCCESS;
    }

  ResultType
  AddTrackerToolToInternalDataContainers( const TrackerToolType * itkNotUsed(trackerTool) )
    {
    return SUCCESS;
    }

  ResultType
  RemoveTrackerToolFromInternalDataContainers(
    const TrackerToolType * itkNotUsed(trackerTool) )
    {
    return SUCCESS;
    }

  ResultType
  InternalUpdateStatus( void )
    {
    igstkLogMacro( DEBUG, "DummyTracker::InternalUpdateStatus called ...\n");

    static double x = 0;
    static double y = 0;
    static double z = 0;

    typedef TrackerToolsContainerType::const_iterator  ConstIteratorType;

    TrackerToolsContainerType trackerToolContainer =
      this->GetTrackerToolContainer();

    ConstIteratorType inputItr = trackerToolContainer.begin();
    ConstIteratorType inputEnd = trackerToolContainer.end();

    TransformType transform;

    transform.SetToIdentity( this->GetValidityTime() );

    typedef TransformType::VectorType PositionType;
    PositionType  position;
    position[0] = x;
    position[1] = y;
    position[2] = z;

    typedef TransformType::ErrorType  ErrorType;
    ErrorType errorValue = 0.5; // +/- half millimeter Uncertainty

    transform.SetTranslation( position, errorValue, this->GetValidityTime() );

    // set the raw transform in all the tracker tools
    while( inputItr != inputEnd )
      {
      this->SetTrackerToolRawTransform(
        trackerToolContainer[inputItr->first], transform );

      this->SetTrackerToolTransformUpdate(
        trackerToolContainer[inputItr->first], true );

      ++inputItr;
      }

    x += 0.1;
    y += 0.1;
    z += 0.1;

    return SUCCESS;
    }

  ResultType
  InternalThreadedUpdateStatus( void )
    {
    igstkLogMacro( DEBUG,
      "DummyTracker::InternalThreadedUpdateStatus called ...\n");
    return SUCCESS;
    }

  /** Print Self function */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const
    {
    Superclass::PrintSelf(os, indent);
    }

};


/** The "RequestAttachToTracker" method attaches
 * the tracker tool to a tracker. */
void DummyTrackerTool::RequestAttachToTracker( DummyTracker *  tracker )
{
  // This delegation is done only to enforce type matching between
  // TrackerTool and Tracker. It prevents the user from accidentally
  // mix TrackerTools and Trackers of different type;
  this->TrackerTool::RequestAttachToTracker( tracker );
}


}

}

igstkEventOccurredMacro( FailedToAttachTrackerTool,
                         igstk::TrackerToolAttachmentToTrackerErrorEvent )

int igstkTrackerTest( int, char * [] )
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::TrackerTest::DummyTracker      TrackerType;
  typedef igstk::TrackerTest::DummyTrackerTool  TrackerToolType;
  typedef TrackerToolType::TransformType        TransformType;

  FailedToAttachTrackerToolEventOccurredObserver::Pointer failedToAttachObserver =
    FailedToAttachTrackerToolEventOccurredObserver::New();

  TrackerType::Pointer tracker;

  TrackerToolType::Pointer trackerTool = TrackerToolType::New();

  trackerTool->AddObserver( igstk::TrackerToolAttachmentToTrackerErrorEvent(),
                            failedToAttachObserver );
  trackerTool->RequestConfigure();
  trackerTool->RequestAttachToTracker( tracker );

  if( !failedToAttachObserver->EventOccured() )
  {
    std::cerr << "Expected to fail the attachment of " <<
                 "tracker tool on NULL tracker" << std::endl;
    return EXIT_FAILURE;
  }

  tracker = TrackerType::New();

  tracker->RequestOpen();

  double defaultValidityTime = tracker->GetTransformValidityTime();
  std::cout << "Default validity time: " << defaultValidityTime << std::endl;

  double trackerFrequency = 1;
  double validityTimeShouldBe = (1000.0/trackerFrequency) + 20;

  tracker->RequestSetFrequency( trackerFrequency );

  double validityTimeReturned = tracker->GetTransformValidityTime();

  if( fabs( validityTimeShouldBe - validityTimeReturned ) > 1e-5 )
    {
    std::cerr << "Error in RequestSetFrequency()/GetValidityTime() "<<std::endl;
    return EXIT_FAILURE;
    }


  tracker->RequestSetFrequency( 30 );

  trackerTool->RequestAttachToTracker( tracker );

  tracker->RequestStartTracking();
  tracker->RequestStopTracking();
  tracker->RequestClose();

  std::cout << tracker << std::endl;

   return EXIT_SUCCESS;
}
