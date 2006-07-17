/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    LandmarkBasedRegistration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "LandmarkBasedRegistration.h"
#include "FiducialSegmentation.h"
#include "FiducialModel.h"
#include "ModelBasedClustering.h"
#include "itkRegionOfInterestImageFilter.h"
#include "PCAOnPoints.h"
#include "igstkLandmark3DRegistration.h"

namespace igstk 
{

LandmarkBasedRegistration::LandmarkBasedRegistration()
{
}

void LandmarkBasedRegistration::PrintSelf(std::ostream& os, itk::Indent indent)
{
  Superclass::PrintSelf(os, indent);
}

bool LandmarkBasedRegistration::Execute()
{
  FiducialSegmentation::PointsListType     fiducialPoints;
  FiducialSegmentation::PointsListType     modelPoints;

  // Segment the fiducial points
  FiducialSegmentation::Pointer segmenter = FiducialSegmentation::New();
  segmenter->SetITKImage( m_ITKImage );
  segmenter->SetThreshold( 3000 );
  segmenter->SetMaxSize( 20 );
  segmenter->SetMinSize( 0 );
  segmenter->SetMergeDistance( 1 );
  segmenter->Execute();
  fiducialPoints = segmenter->GetFiducialPoints();

  // Generate the model points
  FiducialModel::Pointer model = FiducialModel::New();
  modelPoints = model->GetFiducialPoints();

  // Cluster the points, eliminate outliers
  ModelBasedClustering::Pointer cluster = ModelBasedClustering::New();
  cluster->SetSamplePoints( fiducialPoints );
  cluster->SetModelPoints( modelPoints );
  cluster->Execute();
  fiducialPoints = cluster->GetClusteredPoints();

  // Calculate the bounding box of the current fiducial points
  const double Margin = 5;
  vnl_vector< double > minXYZ(3), maxXYZ(3);
  minXYZ.fill( itk::NumericTraits<double>::max() );
  maxXYZ.fill( itk::NumericTraits<double>::NonpositiveMin() );
  for ( int i=0; i<fiducialPoints.size(); i++)
    {
    for ( int j=0; j<3; j++)
      {
      minXYZ(j) = vnl_math_min( minXYZ(j), fiducialPoints[i][j] );
      maxXYZ(j) = vnl_math_max( maxXYZ(j), fiducialPoints[i][j] );
      }
    }
  minXYZ -= Margin;
  maxXYZ += Margin;
  std::cout<< "Physical bounds: " << std::endl;
  std::cout<< minXYZ << std::endl;
  std::cout<< maxXYZ << std::endl;

  ImageType::RegionType roi;
  ImageType::IndexType  startIndex, endIndex;
  ImageType::SizeType   size;
  FiducialSegmentation::PointType p;
  p[0] = minXYZ[0];  p[1] = minXYZ[1];  p[2] = minXYZ[2];
  m_ITKImage->TransformPhysicalPointToIndex( p, startIndex );
  p[0] = maxXYZ[0];  p[1] = maxXYZ[1];  p[2] = maxXYZ[2];
  m_ITKImage->TransformPhysicalPointToIndex( p, endIndex );
  size[0] = endIndex[0] - startIndex[0];
  size[1] = endIndex[1] - startIndex[1];
  size[2] = endIndex[2] - startIndex[2];

  roi.SetIndex( startIndex );
  roi.SetSize( size );

  typedef itk::RegionOfInterestImageFilter< ImageType, ImageType > 
                                                                 ROIFilterType;
  ROIFilterType::Pointer roiFilter = ROIFilterType::New();
  roiFilter->SetInput( m_ITKImage );
  roiFilter->SetRegionOfInterest( roi );
  roiFilter->Update();

  // Segment the fiducial points
  segmenter->SetITKImage( roiFilter->GetOutput() );
  segmenter->SetThreshold( 2000 );
  segmenter->SetMaxSize( 20 );
  segmenter->SetMinSize( 0 );
  segmenter->SetMergeDistance( 1 );
  segmenter->Execute();

  // Cluster the points, eliminate outliers
  cluster->SetSamplePoints( segmenter->GetFiducialPoints() );
  cluster->SetModelPoints( model->GetFiducialPoints() );
  cluster->Execute();
  
  // PCA on the points, sort the points
  PCAOnPoints::Pointer pca = PCAOnPoints::New();
  pca->SetSamplePoints( cluster->GetClusteredPoints() );
  pca->Execute();
  fiducialPoints = pca->GetSortedPoints();

  if ( fiducialPoints.size() != modelPoints.size())
    {
    std::cout << "Error segmenting the image:" << std::endl;
    std::cout << "Number of segmented fiducials: " << fiducialPoints.size()
      << " doesn't match the number of points in model: " << modelPoints.size()
      << std::endl;
    return false;
    }

  typedef igstk::Landmark3DRegistration                RegistrationType;
  typedef RegistrationType::LandmarkImagePointType     LandmarkPointType;

  igstk::Landmark3DRegistration::Pointer landmarkRegistration = 
                                          igstk::Landmark3DRegistration::New();
  for ( int i=0; i<fiducialPoints.size(); i++)
    {
    LandmarkPointType p;
    p = fiducialPoints[i];
    landmarkRegistration->RequestAddImageLandmarkPoint( p );
    p = modelPoints[i];
    landmarkRegistration->RequestAddTrackerLandmarkPoint( p );
    }
  landmarkRegistration->RequestComputeTransform();

  TransformObserver::Pointer transformObserver = TransformObserver::New();
  landmarkRegistration->AddObserver( igstk::TransformModifiedEvent(),
                                     transformObserver );
  landmarkRegistration->RequestGetTransform();
  if ( transformObserver->GotTransform())
    {
    m_Transform = transformObserver->GetTransform();
    std::cout << "RMS Error: " << landmarkRegistration->ComputeRMSError() 
      << std::endl;
    }

  return true;
}

} // end namespace igstk
