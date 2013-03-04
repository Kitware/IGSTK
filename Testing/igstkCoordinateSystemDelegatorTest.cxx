/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateSystemDelegatorTest.cxx
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
#include "igstkCoordinateSystemDelegator.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkTimeProbe.h"

#include "igstkView2D.h"

int igstkCoordinateSystemDelegatorTest(int , char* [])
{
  const int depth = 10;
  const int numIters = 100000;

  typedef igstk::Object::LoggerType         LoggerType;
  typedef itk::StdStreamLogOutput           LogOutputType;
  typedef igstk::CoordinateSystemDelegator
                                            CoordinateSystemDelegator;
  typedef CoordinateSystemDelegator::Pointer
                                            CoordinateSystemDelegatorPointer;

  LoggerType::Pointer logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( LoggerType::CRITICAL );

  std::cout << "Running igstkCoordinateSystemDelegatorTest" 
            << std::endl;

  CoordinateSystemDelegatorPointer root = CoordinateSystemDelegator::New();
  root->SetLogger( logger );

  std::string typeName = "Tracker";
  root->SetType( typeName );
  typeName = root->GetType();

  igstk::Transform identity;
  identity.SetToIdentity(igstk::TimeStamp::GetLongestPossibleTime());

  std::vector<CoordinateSystemDelegatorPointer> coordSysBranch1;
  std::vector<CoordinateSystemDelegatorPointer> coordSysBranch2;

  // Scope temp and temp2
  {
  CoordinateSystemDelegatorPointer temp = CoordinateSystemDelegator::New();
  temp->SetLogger( logger );
  temp->RequestSetTransformAndParent(identity, root);
  coordSysBranch1.push_back(temp);

  CoordinateSystemDelegatorPointer temp2 = CoordinateSystemDelegator::New();
  temp2->SetLogger( logger );
  temp2->RequestSetTransformAndParent(identity, root);
  coordSysBranch2.push_back(temp2);
  }

  for (int i = 1; i < depth; i++)
    {
    CoordinateSystemDelegatorPointer temp = CoordinateSystemDelegator::New();
    temp->SetLogger( logger );
    temp->RequestSetTransformAndParent(identity, coordSysBranch1[i-1]);
    coordSysBranch1.push_back(temp);

    CoordinateSystemDelegatorPointer temp2 = CoordinateSystemDelegator::New();
    temp2->SetLogger( logger );
    temp2->RequestSetTransformAndParent(identity,coordSysBranch2[i-1]);
    coordSysBranch2.push_back(temp2);
    }

  CoordinateSystemDelegatorPointer leaf1 = coordSysBranch1[depth-1];
  CoordinateSystemDelegatorPointer leaf2 = coordSysBranch2[depth-1];

  itk::TimeProbe probe1;
  itk::TimeProbe probe2;

  for (int j = 0; j < numIters; j++)
    {
    probe1.Start();
    leaf1->RequestComputeTransformTo( leaf2 );
    probe1.Stop();

    probe2.Start();
    leaf2->RequestComputeTransformTo( leaf1 );
    probe2.Stop();
    }

  std::cout << "Probe1 [leaf1->RequestComputeTransformTo(leaf2)] : Starts = "
            << probe1.GetNumberOfStarts() 
            << " Stops = "
            << probe1.GetNumberOfStops() 
            << " Mean time = "
            << probe1.GetMean() << std::endl;

  std::cout << "Probe2 [leaf2->RequestComputeTransformTo(leaf1)] : Starts = "
            << probe2.GetNumberOfStarts() 
            << " Stops = "
            << probe2.GetNumberOfStops() 
            << " Mean time = "
            << probe2.GetMean() << std::endl;

  CoordinateSystemDelegator* nullCS = NULL;
  leaf1->RequestSetTransformAndParent( identity, nullCS ); // coverage
  leaf1->RequestComputeTransformTo( nullCS ); // coverage
  leaf1->Print( std::cout ); // coverage
  std::string leaf1Name = "leaf1";
  leaf1->SetName( "leaf1" ); // coverage
  leaf1->SetName( leaf1Name ); // coverage
  if (leaf1->GetName() != leaf1Name) // coverage
    {
    std::cerr << "leaf1->GetName() returned the wrong name." << std::endl;
    std::cerr << "Got : " << leaf1->GetName() << std::endl;
    std::cerr << "Expected : " << leaf1Name << std::endl;
    return EXIT_FAILURE;
    }

  igstk::CoordinateSystem::Pointer fooCS = 
                                      igstk::CoordinateSystem::New();

  if ( leaf1->IsCoordinateSystem( fooCS ) == true ) // coverage
    {
    std::cerr << "leaf1->IsCoordinateSystem( fooCS ) ";
    std::cerr << "should not return true!" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;

}
