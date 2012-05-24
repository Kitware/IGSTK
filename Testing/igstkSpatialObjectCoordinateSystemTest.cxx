/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSpatialObjectCoordinateSystemTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkSpatialObject.h"
#include "igstkEllipsoidObject.h"
#include "igstkTimeStamp.h"
#include "igstkTransform.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "igstkView3D.h"
#include "igstkCoordinateSystemDelegator.h"
#include "igstkTracker.h"

namespace igstk
{
namespace SpatialObjectCoordinateSystemTest
{

class DummyTracker : public Tracker
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( DummyTracker, Tracker )

  typedef Superclass::TransformType           TransformType;
  typedef Superclass::ResultType              ResultType;


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
  RemoveTrackerToolFromInternalDataContainers
  ( const TrackerToolType * itkNotUsed(trackerTool) )
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

    TrackerToolsContainerType 
            trackerToolContainer = this->GetTrackerToolContainer();
   
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

class CoordinateSystemObserver : public ::itk::Command
{
public:
  typedef igstk::CoordinateSystemTransformToEvent  EventType;
  typedef igstk::CoordinateSystemTransformToResult PayloadType;

  /** Standard class typedefs. */
  typedef CoordinateSystemObserver         Self;
  typedef ::itk::Command                   Superclass;
  typedef ::itk::SmartPointer<Self>        Pointer;
  typedef ::itk::SmartPointer<const Self>  ConstPointer;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(CoordinateSystemObserver, ::itk::Command);
  itkNewMacro(CoordinateSystemObserver);

  typedef ::igstk::Transform              TransformType;

  CoordinateSystemObserver()
    {
    m_GotPayload = false;
    m_Payload.Clear();
    }

  ~CoordinateSystemObserver()
    {
    m_GotPayload = false;
    m_Payload.Clear();
    }

  void ClearPayload()
    {
    m_GotPayload = false;
    m_Payload.Clear();
    }

  void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & event)
    {
    this->ClearPayload();
    if( EventType().CheckEvent( &event ) )
      {
      const EventType * transformEvent = 
                dynamic_cast< const EventType *>( &event );
      if( transformEvent )
        {
        m_Payload = transformEvent->Get();
        m_GotPayload = true;
        }
      }
    else
      {
      std::cout << "Got unexpected event : " << std::endl;
      event.Print(std::cout);
      }
    }

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    this->Execute(static_cast<const itk::Object*>(caller), event);
    }

  bool GotPayload() const
    {
    return m_GotPayload;
    }

  const PayloadType & GetPayload() const
    {
    return m_Payload;
    }

  const TransformType & GetTransform() const
    {
    return m_Payload.GetTransform();
    }

protected:

  TransformType m_Transform;
  PayloadType   m_Payload;
  bool          m_GotPayload;

};
}
}


int igstkSpatialObjectCoordinateSystemTest(int , char* [])
{
  typedef igstk::EllipsoidObject              EllipsoidObjectType;
  typedef igstk::EllipsoidObject::Pointer     EllipsoidObjectPointer;

  typedef igstk::SpatialObjectCoordinateSystemTest::CoordinateSystemObserver 
                                                                ObserverType;

  typedef ObserverType::EventType             CoordinateSystemEventType;
  typedef igstk::Object::LoggerType           LoggerType;
  typedef itk::StdStreamLogOutput             LogOutputType;
  
  const double tol = 1e-15;

  igstk::TimeStamp::TimePeriodType aReallyLongTime = 
                                igstk::TimeStamp::GetLongestPossibleTime();

  igstk::Transform::ErrorType errorVal = 1e-15;

  LoggerType::Pointer logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );

  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( LoggerType::DEBUG );

  int testStatus = EXIT_SUCCESS;

  ObserverType::Pointer ellipsoid1Observer = ObserverType::New();

  EllipsoidObjectPointer ellipsoid1 = EllipsoidObjectType::New();
  ellipsoid1->AddObserver( CoordinateSystemEventType(), ellipsoid1Observer );

  ObserverType::Pointer ellipsoid2Observer = ObserverType::New();
  EllipsoidObjectPointer ellipsoid2 = EllipsoidObjectType::New();
  ellipsoid2->AddObserver( CoordinateSystemEventType(), ellipsoid2Observer );
  ellipsoid2->SetLogger( logger );

  igstk::Transform transformE2ToE1;
  igstk::Transform::VectorType translationE2ToE1;
  translationE2ToE1[0] = 33.7452;
  translationE2ToE1[1] = 42.6762;
  translationE2ToE1[2] = 19.7840;
  transformE2ToE1.SetTranslation(translationE2ToE1, 
                                 errorVal, 
                                 aReallyLongTime);

  /** Need to pass the bare pointer for compiler to figure
   *  out the template type. 
   */
  ellipsoid2->RequestSetTransformAndParent(transformE2ToE1, 
                                           ellipsoid1);

  igstk::Transform getTE1ToE2;

  std::cout << "Testing ellipsoid1->RequestComputeTransformTo(ellipsoid2) : ";
  ellipsoid1->RequestComputeTransformTo( ellipsoid2 );
  
  if( ellipsoid1Observer->GotPayload() )
    {
    getTE1ToE2 = ellipsoid1Observer->GetTransform();

    if (getTE1ToE2.IsNumericallyEquivalent( transformE2ToE1.GetInverse(), tol )
                                                                    == false)
      {
      testStatus = EXIT_FAILURE;
      std::cout << "FAILED!" << std::endl;
      }
    else
      {
      std::cout << "passed." << std::endl;  
      } 
    }
  else
    {
    testStatus = EXIT_FAILURE;
    std::cout << "FAILED! [NO EVENT]" << std::endl;
    }

  std::cout << "Testing ellipsoid2->RequestComputeTransformTo(ellipsoid1) : ";
  igstk::Transform getTE2ToE1;
  ellipsoid2->RequestComputeTransformTo( ellipsoid1 );

  if( ellipsoid2Observer->GotPayload() )
    {
    getTE2ToE1 = ellipsoid2Observer->GetTransform();

    if (getTE2ToE1.IsNumericallyEquivalent( transformE2ToE1, tol ) == false)
      {
      testStatus = EXIT_FAILURE;
      std::cout << "FAILED!" << std::endl;
      }
    else
      {
      std::cout << "passed." << std::endl;
      }
    }
  else
    {
    testStatus = EXIT_FAILURE;
    std::cout << "FAILED! [NO EVENT]" << std::endl;
    }

  ellipsoid2->RequestGetTransformToParent();

  ellipsoid1->SetLogger( logger );
  ellipsoid1->RequestGetTransformToParent();

  typedef igstk::View3D ViewType;

  igstk::Transform identity;
  identity.SetToIdentity(igstk::TimeStamp::GetLongestPossibleTime());

  ObserverType::Pointer viewObserver = ObserverType::New();
  ViewType::Pointer view = ViewType::New();
  view->AddObserver( CoordinateSystemEventType(), viewObserver );
  igstk::Transform viewToParent = identity;
  view->RequestSetTransformAndParent( viewToParent, ellipsoid1 );

  std::cout << "Testing view->RequestGetTransformToParent() : ";
  view->RequestGetTransformToParent();
  if (viewObserver->GotPayload() == true )
    {
    igstk::Transform viewToParentObs = viewObserver->GetTransform();
    if (viewToParentObs.IsNumericallyEquivalent( viewToParent, tol ) == true)
      {
      std::cout << "passed." << std::endl;
      }
    else
      {
      testStatus = EXIT_FAILURE;
      std::cout << "FAILED! [Numeric equivalency test]" << std::endl;
      }
    }
  else
    {
    testStatus = EXIT_FAILURE;
    std::cout << "FAILED! [No event]" << std::endl;
    }

  std::cout << "Testing view->RequestComputeTransformTo( ellipsoid2 ) : ";
  view->RequestComputeTransformTo( ellipsoid2 );
  if (viewObserver->GotPayload() == true )
    {
    igstk::Transform viewToEllipsoid2Obs = viewObserver->GetTransform();
    igstk::Transform viewToEllipsoid2 = 
                      igstk::Transform::TransformCompose(
                              transformE2ToE1.GetInverse(), viewToParent);

    if (viewToEllipsoid2Obs.IsNumericallyEquivalent( 
                          viewToEllipsoid2, tol ) == true)
      {
      std::cout << "passed." << std::endl;
      }
    else
      {
      testStatus = EXIT_FAILURE;
      std::cout << "FAILED! [Numeric equivalency test]" << std::endl;
      }
    }
  else
    {
    testStatus = EXIT_FAILURE;
    std::cout << "FAILED! [No event]" << std::endl;
    }
  igstk::Friends::CoordinateSystemHelper::GetCoordinateSystem( ellipsoid2 );

  ObserverType::Pointer trackerObserver = ObserverType::New();
  typedef igstk::SpatialObjectCoordinateSystemTest::DummyTracker  TrackerType;
  TrackerType::Pointer tracker = TrackerType::New();
  tracker->AddObserver( CoordinateSystemEventType(), trackerObserver );

  igstk::Transform trackerToParent = identity;
  tracker->RequestSetTransformAndParent( trackerToParent, view );
  tracker->RequestComputeTransformTo( ellipsoid1 );
  std::cout << "Testing tracker->RequestComputeTransformTo( ellipsoid1 ) : ";
  if (trackerObserver->GotPayload() == true )
    {
    igstk::Transform trackerToEllipsoid1Obs = trackerObserver->GetTransform();

    igstk::Transform 
               trackerToEllipsoid = igstk::Transform::TransformCompose( 
                                                 viewToParent, trackerToParent);
    if (trackerToEllipsoid1Obs.IsNumericallyEquivalent( 
                                     trackerToEllipsoid, tol ) == true)
      {
      std::cout << "passed." << std::endl;
      }
    else
      {
      testStatus = EXIT_FAILURE;
      std::cout << "FAILED! [Numeric equivalency test]" << std::endl;
      }
    }
  else
    {
    testStatus = EXIT_FAILURE;
    std::cout << "FAILED! [No event]" << std::endl;
    }

  std::cout << "Testing tracker->RequestGetTransformToParent() : ";
  tracker->RequestGetTransformToParent();
  if (trackerObserver->GotPayload() == true )
    {
    igstk::Transform trackerToParentObs = trackerObserver->GetTransform();

    if (trackerToParentObs.IsNumericallyEquivalent( 
                                trackerToParent, tol ) == true)
      {
      std::cout << "passed." << std::endl;
      }
    else
      {
      testStatus = EXIT_FAILURE;
      std::cout << "FAILED! [Numeric equivalency test]" << std::endl;
      }
    }
  else
    {
    testStatus = EXIT_FAILURE;
    std::cout << "FAILED! [No event]" << std::endl;
    }

  return testStatus;
}
