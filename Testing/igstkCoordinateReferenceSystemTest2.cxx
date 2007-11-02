/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateReferenceSystemTest2.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTransform.h"
#include "igstkCoordinateReferenceSystem.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"


class CoordinateReferenceSystemObserver : public ::itk::Command
{
public:
  typedef igstk::CoordinateReferenceSystemTransformToEvent  EventType;
  typedef igstk::CoordinateReferenceSystemTransformToResult PayloadType;

  /** Standard class typedefs. */
  typedef CoordinateReferenceSystemObserver         Self;
  typedef ::itk::Command                            Superclass;
  typedef ::itk::SmartPointer<Self>        Pointer;
  typedef ::itk::SmartPointer<const Self>  ConstPointer;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(CoordinateReferenceSystemObserver, ::itk::Command);
  itkNewMacro(CoordinateReferenceSystemObserver);

  CoordinateReferenceSystemObserver()
    {
    m_GotPayload = false;
    m_Payload.m_Destination = NULL;
    m_Payload.m_Source = NULL;
    }

  ~CoordinateReferenceSystemObserver()
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

int igstkCoordinateReferenceSystemTest2(int argc, char* argv[])
{

  typedef itk::Logger                         LoggerType; 
  typedef itk::StdStreamLogOutput             LogOutputType;
  typedef igstk::CoordinateReferenceSystem    CoordSysType;
  typedef igstk::Transform                    TransformType;
  typedef igstk::TimeStamp                    TimeStampType;
  typedef TimeStampType::TimePeriodType       TimePeriodType;
  typedef CoordSysType::Pointer       CoordinateSystemPointer;
  typedef CoordSysType::ConstPointer  ConstCoordinateSystemPointer;
  typedef CoordinateReferenceSystemObserver::EventType CoordinateSystemEventType;

  const TransformType::ErrorType              transformErrorValue = 1e-5;
  const TimePeriodType aReallyLongTime 
                                   = TimeStampType::GetLongestPossibleTime();

  int testPassed = EXIT_SUCCESS;

  LoggerType::Pointer logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );

  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( LoggerType::CRITICAL );

  //
  //
  //                              root        E
  //                               | | 
  //                             --   --
  //                            |       |
  //                        --- A       B
  //                       |    |       |
  //                       G    C       D
  //                            |
  //                            F
  CoordinateSystemPointer root = CoordSysType::New();
  root->SetName( "root" );
  root->SetLogger( logger );


  CoordinateReferenceSystemObserver::Pointer rootObserver = CoordinateReferenceSystemObserver::New();
  root->AddObserver( CoordinateSystemEventType(), rootObserver );

  CoordinateSystemPointer A = CoordSysType::New();
  A->SetName( "A" );
  A->SetLogger( logger );

  CoordinateSystemPointer B = CoordSysType::New();
  B->SetName( "B" );
  B->SetLogger( logger );

  CoordinateSystemPointer C = CoordSysType::New();
  C->SetName( "C" );
  C->SetLogger( logger );

  CoordinateSystemPointer D = CoordSysType::New();
  D->SetName( "D" );
  D->SetLogger( logger );

  CoordinateSystemPointer E = CoordSysType::New();
  E->SetName( "E" );
  E->SetLogger( logger );

  CoordinateSystemPointer F = CoordSysType::New();
  F->SetName( "F" );
  F->SetLogger( logger );

  CoordinateSystemPointer G = CoordSysType::New();
  G->SetName( "G" );
  G->SetLogger( logger );

  TransformType identity;
  identity.SetToIdentity(aReallyLongTime);
  root->RequestSetTransformAndParent(identity, NULL);
  root->RequestSetTransformAndParent(identity, root);
  root->Print(std::cout);

  TransformType TARoot;
  TARoot.SetToIdentity(aReallyLongTime);
  A->RequestSetTransformAndParent(TARoot, root);

  TransformType TBRoot;
  TBRoot.SetToIdentity(aReallyLongTime);
  TransformType::VersorType tbrootRotation;
  tbrootRotation.SetRotationAroundX(vnl_math::pi/2.0);
  TBRoot.SetRotation(tbrootRotation, transformErrorValue, aReallyLongTime);
  B->RequestSetTransformAndParent(TBRoot, root);

  TransformType TCA;
  TransformType::VectorType trans;
  trans[0] = 10;
  trans[1] = 0;
  trans[2] = 0;
  TCA.SetTranslation(trans, transformErrorValue, aReallyLongTime);
  C->RequestSetTransformAndParent(TCA, A);

  TransformType TDB;
  TDB.SetToIdentity(aReallyLongTime);
  D->RequestSetTransformAndParent(TDB, B);

  TransformType TFC;
  TFC.SetToIdentity(aReallyLongTime);
  F->RequestSetTransformAndParent(TFC, C);

  TransformType TGA;
  TGA.SetToIdentity(aReallyLongTime);
  G->RequestSetTransformAndParent(TGA, A);

  std::cout << "Checking transform from root to A : ";
  
  root->RequestComputeTransformTo(A);

  TransformType TRootA;
  if (rootObserver->GotPayload())
    {
    TRootA = rootObserver->GetPayload().m_Transform;

    if (TRootA.IsNumericallyEquivalent(TARoot.GetInverse()) == false)
      {
      testPassed = EXIT_FAILURE;
      std::cout << "FAILED!" << std::endl;
      }
    else
      {
      std::cout << "passed." << std::endl;
      }
    }
  else
    {
    testPassed = EXIT_FAILURE;
    std::cout << "FAILED!" << std::endl;
    }

  std::cout << "Checking transform from root to B : ";

  root->RequestComputeTransformTo(B);
  TransformType TRootB;

  if (rootObserver->GotPayload())
    {
    TRootB = rootObserver->GetPayload().m_Transform;

    if (TRootB.IsNumericallyEquivalent(TBRoot.GetInverse()) == false)
      {
      testPassed = EXIT_FAILURE;
      std::cout << "FAILED!" << std::endl;
      }
    else
      {
      std::cout << "passed." << std::endl;
      }
    }
  else
    {
    testPassed = EXIT_FAILURE;
    std::cout << "FAILED!" << std::endl;
    }

  std::cout << "Checking transform from root to C : "; 
  root->RequestComputeTransformTo(C);
  TransformType TRootC;

  if (rootObserver->GotPayload())
    {
    TRootC = rootObserver->GetPayload().m_Transform;

    TransformType TCRoot = TransformType::TransformCompose(TARoot, TCA);
    if (TRootC.IsNumericallyEquivalent(TCRoot.GetInverse()) == false)
      {
      testPassed = EXIT_FAILURE;
      std::cout << "FAILED!" << std::endl;
      }
    else
      {
      std::cout << "passed." << std::endl;
      }
    }
  else
    {
    testPassed = EXIT_FAILURE;
    std::cout << "FAILED!" << std::endl;
    }

  std::cout << "Checking transform from root to D : "; 
  root->RequestComputeTransformTo(D);
  TransformType TRootD;
  if (rootObserver->GotPayload())
    {
    TRootD = rootObserver->GetPayload().m_Transform;

    TransformType TDRoot = TransformType::TransformCompose(TBRoot, TDB);
    if (TRootD.IsNumericallyEquivalent(TDRoot.GetInverse()) == false)
      {
      testPassed = EXIT_FAILURE;
      std::cout << "FAILED!" << std::endl;
      }
    else
      {
      std::cout << "passed." << std::endl;
      }
    }
  else
    {
    testPassed = EXIT_FAILURE;
    std::cout << "FAILED!" << std::endl;
    }


  std::cout << "Checking transform from root to E : " ;
  TimeStampType now;
  now.SetStartTimeNowAndExpireAfter(0);

  root->RequestComputeTransformTo(E);
  TransformType TRootE;
  if (rootObserver->GotPayload())
    {
    // Disconnected, shouldn't get a transform
    testPassed = EXIT_FAILURE;
    std::cout << "FAILED!" << std::endl;
    }
  else
    {
    std::cout << "passed." << std::endl;
    }

  root->RequestSetTransformAndParent( identity, A );
  root->RequestSetTransformAndParent( identity, B );

  root->RequestComputeTransformTo( NULL );
  root->RequestComputeTransformTo( root );

  root->SetReportTiming( true );

  CoordinateSystemPointer Leak = CoordSysType::New();
  Leak->SetName( "Leak" );

  rootObserver->ClearPayload();

  return testPassed;
}

