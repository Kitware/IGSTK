/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSpatialObjectCoordinateSystemTest3.cxx
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
#include "igstkSpatialObject.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkTimeProbe.h"

namespace SpatialObjectCoordinateSystemTest3
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
                                    igstk::TimeStamp::GetLongestPossibleTime() );
  return result;
}
} // namespace

int igstkSpatialObjectCoordinateSystemTest3(int , char*[] )
{
  const int depth = 10;
  const int numIters = 100000;

  typedef igstk::Object::LoggerType               LoggerType;
  typedef itk::StdStreamLogOutput                 LogOutputType;
  typedef igstk::SpatialObject                    CoordinateSystem;
  typedef CoordinateSystem::Pointer               CoordinateSystemPointer;

  LoggerType::Pointer logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  // logger->SetPriorityLevel( LoggerType::CRITICAL );

  std::cout << "Running igstkSpatialObjectCoordinateSystemTest3 " << std::endl;

  CoordinateSystemPointer root = CoordinateSystem::New();
  // root->SetLogger( logger );

  igstk::Transform identity;
  identity.SetToIdentity(igstk::TimeStamp::GetLongestPossibleTime());

  std::vector<CoordinateSystemPointer> coordSysBranch1;
  std::vector<CoordinateSystemPointer> coordSysBranch2;

  // Scope temp and temp2
  {
  igstk::Transform randomTransform1 
                   = SpatialObjectCoordinateSystemTest3::GetRandomTransform();
  igstk::Transform randomTransform2 
                   = SpatialObjectCoordinateSystemTest3::GetRandomTransform();

  CoordinateSystemPointer temp = CoordinateSystem::New();
  // temp->SetLogger( logger );
  temp->RequestSetTransformAndParent(randomTransform1, root);
  coordSysBranch1.push_back(temp);

  CoordinateSystemPointer temp2 = CoordinateSystem::New();
  // temp2->SetLogger( logger );
  temp2->RequestSetTransformAndParent(randomTransform2, root);
  coordSysBranch2.push_back(temp2);
  }

  for (int i = 1; i < depth; i++)
    {
    igstk::Transform randomTransform1
                     = SpatialObjectCoordinateSystemTest3::GetRandomTransform();
    igstk::Transform randomTransform2
                     = SpatialObjectCoordinateSystemTest3::GetRandomTransform();

    CoordinateSystemPointer temp = CoordinateSystem::New();
    // temp->SetLogger( logger );
    temp->RequestSetTransformAndParent(randomTransform1, coordSysBranch1[i-1]);
    coordSysBranch1.push_back(temp);

    CoordinateSystemPointer temp2 = CoordinateSystem::New();
    // temp2->SetLogger( logger );
    temp2->RequestSetTransformAndParent(randomTransform2, coordSysBranch2[i-1]);
    coordSysBranch2.push_back(temp2);
    }

  CoordinateSystemPointer leaf1 = coordSysBranch1[depth-1];
  CoordinateSystemPointer leaf2 = coordSysBranch2[depth-1];

  // leaf1->SetReportTiming(true);
  // leaf2->SetReportTiming(true);

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

  return EXIT_SUCCESS;

}
