/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    PointsClusteringTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
//Warning about: identifier was truncated to '255' characters 
//in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "FiducialModel.h"
#include "PointsClustering.h"

int main(int argc , char * argv [] )
{
  if ( argc < 3 ) 
    {
    std::cerr << "Usage" << argv[0] 
              << "DistanceType  " 
              << "MaxMergeDistance  "<< std::endl;
    return EXIT_FAILURE;
    } 

  typedef igstk::FiducialModel  FiducialModelType;
  FiducialModelType::Pointer  fiducialModel = FiducialModelType::New();
  FiducialModelType::PointsListType points = fiducialModel->GetFiducialPoints();

  
  FiducialModelType::PointType p;
  p[0] = 0.0; p[1] = 0.0; p[2] = 0.0;
  points.push_back( p );
  p[0] = 30.0; p[1] = 0.0; p[2] = 20.0;
  points.push_back( p );
  p[0] = -30.0; p[1] = 0.0; p[2] = 20.0;
  points.push_back( p );
  p[0] = 0.0; p[1] = 30.0; p[2] = 20.0;
  points.push_back( p );
  p[0] = 0.0; p[1] = -30.0; p[2] = 20.0;
  points.push_back( p );
  p[0] = 0.0; p[1] = 0.0; p[2] = 100.0;
  points.push_back( p );


  igstk::PointsClustering::Pointer cluster = igstk::PointsClustering::New();
  cluster->SetDistanceType( atoi( argv[1] ) );
  cluster->SetMaxMergeDistance( atof( argv[2] ) );
  cluster->SetSamplePoints( points );

  cluster->Execute();

  igstk::PointsClustering::PointClusterType clusteredPoints = 
                                                          cluster->GetCluster();

  return EXIT_SUCCESS;
}
