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
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

namespace SpatialObjectCoordinateSystemTest
{
class SpatialObjectObserver : public ::itk::Command
{
public:
  typedef igstk::CoordinateReferenceSystemTransformToEvent  EventType;
  typedef igstk::CoordinateReferenceSystemTransformToResult PayloadType;

  /** Standard class typedefs. */
  typedef SpatialObjectObserver         Self;
  typedef ::itk::Command                            Superclass;
  typedef ::itk::SmartPointer<Self>        Pointer;
  typedef ::itk::SmartPointer<const Self>  ConstPointer;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(SpatialObjectObserver, ::itk::Command);
  itkNewMacro(SpatialObjectObserver);

  SpatialObjectObserver()
    {
    m_GotPayload = false;
    m_Payload.m_Destination = NULL;
    m_Payload.m_Source = NULL;
    }

  ~SpatialObjectObserver()
    {
    m_GotPayload = false;
    m_Payload.m_Destination = NULL;
    m_Payload.m_Source = NULL;
    }

  void ClearPayload()
    {
    m_GotPayload = false;
    m_Payload.m_Destination = NULL;
    m_Payload.m_Source = NULL;
    }

  void Execute(const itk::Object *caller, const itk::EventObject & event)
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

  bool GotPayload()
    {
    return m_GotPayload;
    }

  PayloadType GetPayload()
    {
    return m_Payload;
    }

protected:

  PayloadType   m_Payload;
  bool          m_GotPayload;

};
}

int igstkSpatialObjectCoordinateSystemTest(int argc, char* argv[])
{
  typedef igstk::EllipsoidObject              EllipsoidObjectType;
  typedef igstk::EllipsoidObject::Pointer     EllipsoidObjectPointer;
  typedef SpatialObjectCoordinateSystemTest::SpatialObjectObserver 
                                                                ObserverType;
  typedef ObserverType::EventType             CoordinateSystemEventType;
  typedef itk::Logger                         LoggerType; 
  typedef itk::StdStreamLogOutput             LogOutputType;
  
  igstk::TimeStamp::TimePeriodType aReallyLongTime = 
                                igstk::TimeStamp::GetLongestPossibleTime();

  igstk::Transform::ErrorType errorVal = 1e-5;

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
                                           ellipsoid1.GetPointer());

  igstk::Transform getTE1ToE2;

#ifndef USE_SPATIAL_OBJECT_DEPRECATED
  ellipsoid1->RequestComputeTransformTo(ellipsoid2.GetPointer());
  
  if( ellipsoid1Observer->GotPayload() )
    {
    getTE1ToE2 = ellipsoid1Observer->GetPayload().m_Transform;

    if (getTE1ToE2.IsNumericallyEquivalent( transformE2ToE1.GetInverse() )
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

  igstk::Transform getTE2ToE1;
  ellipsoid2->RequestComputeTransformTo(ellipsoid1.GetPointer());

  if( ellipsoid2Observer->GotPayload() )
    {
    getTE2ToE1 = ellipsoid2Observer->GetPayload().m_Transform;

    if (getTE2ToE1.IsNumericallyEquivalent( transformE2ToE1 ) == false)
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
#endif

  return testStatus;
}
