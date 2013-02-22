/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateSystemTest3.cxx
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
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkTimeProbe.h"


int igstkCoordinateSystemTest3(int , char* [])
{
  const int depth = 10;
  const int numIters = 100000;

  typedef igstk::Object::LoggerType               LoggerType;
  typedef itk::StdStreamLogOutput                 LogOutputType;
  typedef igstk::CoordinateSystem                 CoordinateSystem;
  typedef CoordinateSystem::Pointer               CoordinateSystemPointer;

  LoggerType::Pointer logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( LoggerType::CRITICAL );

  std::cout << "Running igstkCoordinateSystemTest3" << std::endl;

  CoordinateSystemPointer root = CoordinateSystem::New();
  root->SetLogger( logger );

  igstk::Transform identity;
  identity.SetToIdentity(igstk::TimeStamp::GetLongestPossibleTime());

  std::vector<CoordinateSystemPointer> coordSysBranch1;
  std::vector<CoordinateSystemPointer> coordSysBranch2;

  // Scope temp and temp2
  {
  CoordinateSystemPointer temp = CoordinateSystem::New();
  temp->SetLogger( logger );
  temp->RequestSetTransformAndParent(identity, root);
  coordSysBranch1.push_back(temp);

  CoordinateSystemPointer temp2 = CoordinateSystem::New();
  temp2->SetLogger( logger );
  temp2->RequestSetTransformAndParent(identity, root);
  coordSysBranch2.push_back(temp2);
  }

  for (int i = 1; i < depth; i++)
    {
    CoordinateSystemPointer temp = CoordinateSystem::New();
    temp->SetLogger( logger );
    temp->RequestSetTransformAndParent(identity, coordSysBranch1[i-1]);
    coordSysBranch1.push_back(temp);

    CoordinateSystemPointer temp2 = CoordinateSystem::New();
    temp2->SetLogger( logger );
    temp2->RequestSetTransformAndParent(identity,coordSysBranch2[i-1]);
    coordSysBranch2.push_back(temp2);
    }

  CoordinateSystemPointer leaf1 = coordSysBranch1[depth-1];
  CoordinateSystemPointer leaf2 = coordSysBranch2[depth-1];

  itk::TimeProbe probe1;
  itk::TimeProbe probe2;

  for (int j = 0; j < numIters; j++)
    {
    probe1.Start();
    leaf1->RequestComputeTransformTo(leaf2);
    probe1.Stop();

    probe2.Start();
    leaf2->RequestComputeTransformTo(leaf1);
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

  return EXIT_SUCCESS;

}
