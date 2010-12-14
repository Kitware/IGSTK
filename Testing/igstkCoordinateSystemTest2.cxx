/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateSystemTest2.cxx
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
#include "igstkCoordinateSystem.h"
#include "igstkCoordinateSystemTransformToResult.h"
#include "igstkCoordinateSystemTransformToErrorResult.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "igstkRealTimeClock.h"
#include "igstkTransformObserver.h"

namespace CoordinateSystemTest2
{

/** Using this could make debugging painful... */
igstk::Transform GetRandomTransform()
{
  igstk::Transform::VectorType translation;
  igstk::Transform::VersorType rotation;

  /** Rescale and shift the translation values to
   *  prevent large numbers from taking up too
   *  many bits to construct precise answers and
   *  to allow for negative numbers */
  const double rescale = 100.0 / static_cast<double>(RAND_MAX);
  const double shift = 50.0;

  translation[0] = static_cast<double>(rand())*rescale - shift;
  translation[1] = static_cast<double>(rand())*rescale - shift;
  translation[2] = static_cast<double>(rand())*rescale - shift;

  double x = static_cast<double>(rand())/static_cast<double>(RAND_MAX);
  double y = static_cast<double>(rand())/static_cast<double>(RAND_MAX);
  double z = static_cast<double>(rand())/static_cast<double>(RAND_MAX);
  double w = static_cast<double>(rand())/static_cast<double>(RAND_MAX);
  rotation.Set(x, y, z, w);

  igstk::Transform result;
  result.SetTranslationAndRotation( translation,
                                    rotation,
                                    1e-5, // error tol
                                    igstk::TimeStamp::GetLongestPossibleTime()
                                    );
  return result;
}
}

int igstkCoordinateSystemTest2(int , char* [] )
{

  typedef igstk::Object::LoggerType           LoggerType;
  typedef itk::StdStreamLogOutput             LogOutputType;
  typedef igstk::CoordinateSystem             CoordSysType;
  typedef igstk::Transform                    TransformType;
  typedef igstk::TimeStamp                    TimeStampType;
  typedef TimeStampType::TimePeriodType       TimePeriodType;
  typedef CoordSysType::Pointer               CoordinateSystemPointer;
  typedef CoordSysType::ConstPointer          ConstCoordinateSystemPointer;
  typedef igstk::TransformObserver            TransformObserverType;

  const TransformType::ErrorType              transformErrorValue = 1e-5;
  const TimePeriodType aReallyLongTime
                                   = TimeStampType::GetLongestPossibleTime();

  const double tol = 1.0e-12;

  igstk::RealTimeClock::Initialize();

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
  //                        --- A --    B
  //                       |    |   |   |
  //                       G    C   H   D
  //                            |
  //                            F
  CoordinateSystemPointer root = CoordSysType::New();
  root->SetName( "root" );
  root->GetName(); // coverage
  root->SetLogger( logger );

  TransformObserverType::Pointer rootObserver =
                                    TransformObserverType::New();
  rootObserver->ObserveTransformEventsFrom( root );

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

  CoordinateSystemPointer H = CoordSysType::New();
  H->SetName( "H" );
  H->SetLogger( logger );


  TransformType identity;
  identity.SetToIdentity(aReallyLongTime);
  root->RequestSetTransformAndParent(identity, NULL);
  root->RequestSetTransformAndParent(identity, root);
  root->Print(std::cout);

  std::cout << "                                            " << std::endl;
  std::cout << "                              root        E " << std::endl;
  std::cout << "                               | |          " << std::endl;
  std::cout << "                             --   --        " << std::endl;
  std::cout << "                            |       |       " << std::endl;
  std::cout << "                        --- A --    B       " << std::endl;
  std::cout << "                       |    |   |   |       " << std::endl;
  std::cout << "                       G    C   H   D       " << std::endl;
  std::cout << "                            |               " << std::endl;
  std::cout << "                            F               " << std::endl;
  std::cout << std::endl;


  TransformType TARoot = CoordinateSystemTest2::GetRandomTransform();
  A->RequestSetTransformAndParent(TARoot, root);

  TransformType TBRoot;
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

  TransformType TDB = CoordinateSystemTest2::GetRandomTransform();
  D->RequestSetTransformAndParent(TDB, B);

  TransformType TFC = CoordinateSystemTest2::GetRandomTransform();
  F->RequestSetTransformAndParent(TFC, C);

  TransformType TGA = CoordinateSystemTest2::GetRandomTransform();
  G->RequestSetTransformAndParent(TGA, A);

  TransformType THA = CoordinateSystemTest2::GetRandomTransform();
  H->RequestSetTransformAndParent(THA, A);

  std::cout << "Checking transform from root to A : ";

  rootObserver->Clear();
  root->RequestComputeTransformTo(A);

  TransformType TRootA;
  if( rootObserver->GotTransform() )
    {
    TRootA = rootObserver->GetTransform();

    if (TRootA.IsNumericallyEquivalent(TARoot.GetInverse(), tol) == false)
      {
      testPassed = EXIT_FAILURE;
      std::cout << "FAILED!" << std::endl;
      std::cout << "Requested transform: " << TRootA << std::endl;
      std::cout << "Expected transform: " << TARoot.GetInverse() << std::endl;
      }
    else
      {
      std::cout << "passed." << std::endl;
      }
    }
  else
    {
    testPassed = EXIT_FAILURE;
    std::cout << "FAILED! rootObserver did not get event." << std::endl;
    }

  // Reset the internal boolean flags
  rootObserver->Clear();

  std::cout << "Checking transform from root to B : ";

  root->RequestComputeTransformTo(B);
  TransformType TRootB;

  if( rootObserver->GotTransform() )
    {
    TRootB = rootObserver->GetTransform();

    if (TRootB.IsNumericallyEquivalent(TBRoot.GetInverse(), tol) == false)
      {
      testPassed = EXIT_FAILURE;
      std::cout << "FAILED!" << std::endl;
      std::cout << "Requested transform: " << TRootB << std::endl;
      std::cout << "Expected transform: " << TBRoot.GetInverse() << std::endl;
      }
    else
      {
      std::cout << "passed." << std::endl;
      }
    }
  else
    {
    testPassed = EXIT_FAILURE;
    std::cout << "FAILED! rootObserver did not get event." << std::endl;
    }

  // Reset the internal boolean flags
  rootObserver->Clear();

  std::cout << "Checking transform from root to C : ";
  root->RequestComputeTransformTo(C);
  TransformType TRootC;

  if( rootObserver->GotTransform() )
    {
    TRootC = rootObserver->GetTransform();

    TransformType TCRoot = TransformType::TransformCompose(TARoot, TCA);
    if (TRootC.IsNumericallyEquivalent(TCRoot.GetInverse(), tol) == false)
      {
      testPassed = EXIT_FAILURE;
      std::cout << "FAILED!" << std::endl;
      std::cout << "Requested transform: " << TRootC << std::endl;
      std::cout << "Expected transform: " << TCRoot.GetInverse() << std::endl;
      }
    else
      {
      std::cout << "passed." << std::endl;
      }
    }
  else
    {
    testPassed = EXIT_FAILURE;
    std::cout << "FAILED! rootObserver did not get event." << std::endl;
    }

  // Reset internal boolean flags.
  rootObserver->Clear();

  std::cout << "Checking transform from root to D : ";
  root->RequestComputeTransformTo(D);
  TransformType TRootD;
  if( rootObserver->GotTransform() )
    {
    TRootD = rootObserver->GetTransform();

    TransformType TDRoot = TransformType::TransformCompose(TBRoot, TDB);
    if (TRootD.IsNumericallyEquivalent(TDRoot.GetInverse(), tol) == false)
      {
      testPassed = EXIT_FAILURE;
      std::cout << "FAILED!" << std::endl;
      std::cout << "Requested transform: " << TRootD << std::endl;
      std::cout << "Expected transform: " << TDRoot.GetInverse() << std::endl;
      }
    else
      {
      std::cout << "passed." << std::endl;
      }
    }
  else
    {
    testPassed = EXIT_FAILURE;
    std::cout << "FAILED! rootObserver did not get event." << std::endl;
    }

  // Reset internal boolean flags.
  rootObserver->Clear();

  std::cout << "Checking transform from root to E : ";
  TimeStampType now;
  now.SetStartTimeNowAndExpireAfter(0);

  root->RequestComputeTransformTo(E);
  TransformType TRootE;
  if( rootObserver->GotTransform() )
    {
    // Disconnected, shouldn't get a transform
    testPassed = EXIT_FAILURE;
    std::cout << "FAILED! rootObserver got unexpected event." << std::endl;
    }
  else
    {
    std::cout << "passed." << std::endl;
    }

  // Reset internal boolean flags.
  rootObserver->Clear();

  root->RequestSetTransformAndParent( identity, A );
  root->RequestSetTransformAndParent( identity, B );

  root->RequestComputeTransformTo( NULL );
  root->RequestComputeTransformTo( root );

  // Reset internal boolean flags.
  rootObserver->Clear();

  TransformObserverType::Pointer FObserver =
                                    TransformObserverType::New();
  FObserver->ObserveTransformEventsFrom( F );
  F->RequestComputeTransformTo(G);

  std::cout << "Checking transform from F to G : ";
  if( FObserver->GotTransform() )
    {
    TransformType TFG = FObserver->GetTransform();

    TransformType TFGTrue =
              TransformType
                    ::TransformCompose( TGA.GetInverse(),
                                TransformType::TransformCompose(TCA, TFC));

    if (TFGTrue.IsNumericallyEquivalent( TFG, tol ) == false)
      {
      std::cout << "FAILED!" << std::endl;
      std::cout << "Requested transform: " << TFG << std::endl;
      std::cout << "Expected transform: " << TFGTrue << std::endl;
      testPassed = EXIT_FAILURE;
      }
    else
      {
      std::cout << "passed." << std::endl;
      }
    }
  else
    {
    std::cout << "FAILED! FObserver did not get event." << std::endl;
    testPassed = EXIT_FAILURE;
    }

  // Reset internal boolean flags.
  FObserver->Clear();

  std::cout << "Checking transform from D to F : ";

  TransformObserverType::Pointer DObserver =
                                    TransformObserverType::New();
  DObserver->ObserveTransformEventsFrom( D );

  D->RequestComputeTransformTo(F);

  if( DObserver->GotTransform() )
    {
    TransformType TDF = DObserver->GetTransform();

    TransformType TDRoot = TransformType::TransformCompose(TBRoot, TDB);
    TransformType TFRoot = TransformType::TransformCompose(TARoot,
                                  TransformType::TransformCompose(TCA, TFC));
    TransformType TDFTrue = TransformType
                             ::TransformCompose(TFRoot.GetInverse(), TDRoot);

    if (TDFTrue.IsNumericallyEquivalent( TDF, tol ) == false)
      {
      std::cout << "FAILED!" << std::endl;
      std::cout << "Requested transform: " << TDF << std::endl;
      std::cout << "Expected transform: " << TDFTrue << std::endl;
      testPassed = EXIT_FAILURE;
      }
    else
      {
      std::cout << "passed." << std::endl;
      }
    }
  else
    {
    std::cout << "FAILED! - DObserver did not get event." << std::endl;
    testPassed = EXIT_FAILURE;
    }

  // Reset internal boolean flags.
  DObserver->Clear();

  E->RequestDetachFromParent(); // coverage
  F->RequestDetachFromParent(); // coverage

  std::cout << F << std::endl;

  return testPassed;
}
