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

  ConstCoordinateSystemPointer rootConst = root;
  ConstCoordinateSystemPointer AConst = A;
  ConstCoordinateSystemPointer BConst = B;
  ConstCoordinateSystemPointer CConst = C;
  ConstCoordinateSystemPointer DConst = D;
  ConstCoordinateSystemPointer EConst = E;
  ConstCoordinateSystemPointer FConst = F;
  ConstCoordinateSystemPointer GConst = G;

  CoordSysType::ConstPointer ancestor;
  ancestor = CoordSysType::GetLowestCommonAncestor(rootConst, AConst);
  std::cout << "root and A has ancestor: " 
            << ancestor->GetName() << std::endl;

  ancestor = CoordSysType::GetLowestCommonAncestor(rootConst, BConst);
  std::cout << "root and B has ancestor: " 
            << ancestor->GetName() << std::endl;

  ancestor = CoordSysType::GetLowestCommonAncestor(rootConst, CConst);
  std::cout << "root and C has ancestor: " 
            << ancestor->GetName() << std::endl;

  ancestor = CoordSysType::GetLowestCommonAncestor(rootConst, DConst);
  std::cout << "root and D has ancestor: " 
            << ancestor->GetName() << std::endl;

  ancestor = CoordSysType::GetLowestCommonAncestor(AConst, BConst);
  std::cout << "A and B has ancestor: " 
            <<  ancestor->GetName() << std::endl;

  ancestor = CoordSysType::GetLowestCommonAncestor(CConst, DConst);
  std::cout << "C and D has ancestor: " 
            <<  ancestor->GetName() << std::endl;

  ancestor = CoordSysType::GetLowestCommonAncestor(AConst, CConst);
  std::cout << "A and C has ancestor: " 
            <<  ancestor->GetName() << std::endl;
  
  // NULL pointer return
  ancestor = CoordSysType::GetLowestCommonAncestor(EConst, CConst); 
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

  ancestor = CoordSysType::GetLowestCommonAncestor(NULL, EConst);
  if (ancestor.IsNull())
    {
    ancestorName = "NULL";
    }
  else
    {
    ancestorName = ancestor->GetName();
    }
  std::cout << "NULL and E has ancestor : " <<  ancestorName << std::endl; 

  ancestor = CoordSysType::GetLowestCommonAncestor(EConst, NULL);
  if (ancestor.IsNull())
    {
    ancestorName = "NULL";
    }
  else
    {
    ancestorName = ancestor->GetName();
    }
  std::cout << "E and NULL has ancestor : " <<  ancestorName << std::endl; 

  ancestor = CoordSysType::GetLowestCommonAncestor(FConst, GConst);
  if (ancestor.IsNull())
    {
    ancestorName = "NULL";
    }
  else
    {
    ancestorName = ancestor->GetName();
    }
  std::cout << "F and G has ancestor : " << ancestorName << std::endl; 
  
  ancestor = CoordSysType::GetLowestCommonAncestor(GConst, GConst);
  if (ancestor.IsNull())
    {
    ancestorName = "NULL";
    }
  else
    {
    ancestorName = ancestor->GetName();
    }
  std::cout << "G and G has ancestor : " << ancestorName << std::endl; 

  ancestor = CoordSysType::GetLowestCommonAncestor(rootConst, rootConst);
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
  TransformType TRootA = CoordSysType::GetTransformBetween(rootConst,AConst);
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
  TransformType TRootB = CoordSysType::GetTransformBetween(rootConst,BConst);
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
  TransformType TRootC = CoordSysType::GetTransformBetween(rootConst,CConst);
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
  TransformType TRootD = CoordSysType::GetTransformBetween(rootConst,DConst);
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

  TransformType TRootE = CoordSysType::GetTransformBetween(rootConst,EConst);
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

