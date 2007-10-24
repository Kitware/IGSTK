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

  const TransformType::ErrorType              transformErrorValue = 1e-5;
  const TimePeriodType aReallyLongTime 
                                   = TimeStampType::GetLongestPossibleTime();

  int testPassed = EXIT_SUCCESS;

  LoggerType::Pointer logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );

  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( LoggerType::DEBUG );

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
  std::cout << "Root identifier : " << root->GetIdentifier() << std::endl;

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
  root->SetTransformAndParent(identity, NULL);
  root->SetTransformAndParent(identity, root);
  root->Print(std::cout);

  TransformType TARoot;
  TARoot.SetToIdentity(aReallyLongTime);
  A->SetTransformAndParent(TARoot, root);

  TransformType TBRoot;
  TBRoot.SetToIdentity(aReallyLongTime);
  TransformType::VersorType tbrootRotation;
  tbrootRotation.SetRotationAroundX(vnl_math::pi/2.0);
  TBRoot.SetRotation(tbrootRotation, transformErrorValue, aReallyLongTime);
  B->SetTransformAndParent(TBRoot, root);

  TransformType TCA;
  TransformType::VectorType trans;
  trans[0] = 10;
  trans[1] = 0;
  trans[2] = 0;
  TCA.SetTranslation(trans, transformErrorValue, aReallyLongTime);
  C->SetTransformAndParent(TCA, A);

  TransformType TDB;
  TDB.SetToIdentity(aReallyLongTime);
  D->SetTransformAndParent(TDB, B);

  TransformType TFC;
  TFC.SetToIdentity(aReallyLongTime);
  F->SetTransformAndParent(TFC, C);

  TransformType TGA;
  TGA.SetToIdentity(aReallyLongTime);
  G->SetTransformAndParent(TGA, A);

  CoordSysType::ConstPointer ancestor;
  ancestor = CoordSysType::GetLowestCommonAncestor(root, A);
  std::cout << "root and A has ancestor: " 
            << ancestor->GetName() << std::endl;

  ancestor = CoordSysType::GetLowestCommonAncestor(root, B);
  std::cout << "root and B has ancestor: " 
            << ancestor->GetName() << std::endl;

  ancestor = CoordSysType::GetLowestCommonAncestor(root, C);
  std::cout << "root and C has ancestor: " 
            << ancestor->GetName() << std::endl;

  ancestor = CoordSysType::GetLowestCommonAncestor(root, D);
  std::cout << "root and D has ancestor: " 
            << ancestor->GetName() << std::endl;

  ancestor = CoordSysType::GetLowestCommonAncestor(A, B);
  std::cout << "A and B has ancestor: " 
            <<  ancestor->GetName() << std::endl;

  ancestor = CoordSysType::GetLowestCommonAncestor(C, D);
  std::cout << "C and D has ancestor: " 
            <<  ancestor->GetName() << std::endl;

  ancestor = CoordSysType::GetLowestCommonAncestor(A, C);
  std::cout << "A and C has ancestor: " 
            <<  ancestor->GetName() << std::endl;
  
  // NULL pointer return
  ancestor = CoordSysType::GetLowestCommonAncestor(E, C); 
  const char* ancestorName;
  if (ancestor.IsNull())
    {
    ancestorName = "NULL";
    }
  else
    {
    ancestorName = ancestor->GetName();
    }
  std::cout << "E and C has ancestor: " <<  ancestorName << std::endl;

  ancestor = CoordSysType::GetLowestCommonAncestor(NULL, NULL);
  if (ancestor.IsNull())
    {
    ancestorName = "NULL";
    }
  else
    {
    ancestorName = ancestor->GetName();
    }
  std::cout << "NULL and NULL has ancestor: " <<  ancestorName << std::endl;

  ancestor = CoordSysType::GetLowestCommonAncestor(NULL, E);
  if (ancestor.IsNull())
    {
    ancestorName = "NULL";
    }
  else
    {
    ancestorName = ancestor->GetName();
    }
  std::cout << "NULL and E has ancestor : " <<  ancestorName << std::endl; 

  ancestor = CoordSysType::GetLowestCommonAncestor(E, NULL);
  if (ancestor.IsNull())
    {
    ancestorName = "NULL";
    }
  else
    {
    ancestorName = ancestor->GetName();
    }
  std::cout << "E and NULL has ancestor : " <<  ancestorName << std::endl; 

  ancestor = CoordSysType::GetLowestCommonAncestor(F, G);
  if (ancestor.IsNull())
    {
    ancestorName = "NULL";
    }
  else
    {
    ancestorName = ancestor->GetName();
    }
  std::cout << "F and G has ancestor : " << ancestorName << std::endl; 
  
  ancestor = CoordSysType::GetLowestCommonAncestor(G, G);
  if (ancestor.IsNull())
    {
    ancestorName = "NULL";
    }
  else
    {
    ancestorName = ancestor->GetName();
    }
  std::cout << "G and G has ancestor : " << ancestorName << std::endl; 

  ancestor = CoordSysType::GetLowestCommonAncestor(root, root);
  if (ancestor.IsNull())
    {
    ancestorName = "NULL";
    }
  else
    {
    ancestorName = ancestor->GetName();
    }
  std::cout << "root and root has ancestor : " << ancestorName << std::endl; 


  std::cout << "Checking transform from root to A : ";
  TransformType TRootA = CoordSysType::GetTransformBetween(root,A);
  if (TRootA.IsNumericallyEquivalent(TARoot.GetInverse()) == false)
    {
    testPassed = EXIT_FAILURE;
    std::cout << "FAILED!" << std::endl;
    }
  else
    {
    std::cout << "passed." << std::endl;
    }

  std::cout << "Checking transform from root to B : ";
  TransformType TRootB = CoordSysType::GetTransformBetween(root,B);
  if (TRootB.IsNumericallyEquivalent(TBRoot.GetInverse()) == false)
    {
    testPassed = EXIT_FAILURE;
    std::cout << "FAILED!" << std::endl;
    }
  else
    {
    std::cout << "passed." << std::endl;
    }

  std::cout << "Checking transform from root to C : "; 
  TransformType TRootC = CoordSysType::GetTransformBetween(root,C);
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

  std::cout << "Checking transform from root to D : "; 
  TransformType TRootD = CoordSysType::GetTransformBetween(root,D);
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

  std::cout << "Checking transform from root to E : " ;
  TimeStampType now;
  now.SetStartTimeNowAndExpireAfter(0);

  TransformType TRootE = CoordSysType::GetTransformBetween(root,E);
  // Should never be valid since root and E are disconnected.
  if (TRootE.GetExpirationTime() >= now.GetExpirationTime())
    {
    testPassed = EXIT_FAILURE;
    std::cout << "FAILED!" << std::endl;
    std::cout << TRootE.GetExpirationTime() << std::endl;
    std::cout << now.GetExpirationTime() << std::endl;
    }
  else
    {
    std::cout << "passed." << std::endl;
    }
 
  return testPassed;

}

