/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    ModelBasedRegistration.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "ModelBasedRegistration.h"
#include "FiducialSegmentation.h"
#include "FiducialModel.h"
#include "ModelBasedClustering.h"
#include "itkRegionOfInterestImageFilter.h"
#include "PCAOnPoints.h"
#include "igstkLandmark3DRegistration.h"
#include "itkVersorRigid3DTransform.h"

ModelBasedRegistration::ModelBasedRegistration()
{
}


void ModelBasedRegistration::PrintSelf(std::ostream& os, itk::Indent indent)
{
  Superclass::PrintSelf(os, indent);
}

bool ModelBasedRegistration::Execute()
{
  FiducialSegmentation::PointsListType     fiducialPoints;
  FiducialSegmentation::PointsListType   modelPoints;

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
  fiducialPoints = cluster->GetClusteredPoints();

  // PCA on the points, get the initial transform
  PCAOnPoints::Pointer pca = PCAOnPoints::New();
  pca->SetSamplePoints( fiducialPoints );
  pca->Execute();
  PCAOnPoints::PointType  fiducialCenter = pca->GetCenter();
  PCAOnPoints::VectorType fiducialAxis   = pca->GetPrincipleAxis();
  PCAOnPoints::VectorType yAxis(3);
  yAxis.fill(0);  yAxis[1] = -1; // Negative Y axis direction
  if ( dot_product( yAxis, fiducialAxis ) < 0)
    {
    fiducialAxis *= -1;
    }
  std::cout<< "Centroid of the points: " << fiducialCenter << std::endl;
  std::cout<< "Principle axis: " << fiducialAxis << std::endl;

  pca->SetSamplePoints( modelPoints );
  pca->Execute();
  PCAOnPoints::PointType  modelCenter = pca->GetCenter();
  PCAOnPoints::VectorType modelAxis   = pca->GetPrincipleAxis();
  PCAOnPoints::VectorType zAxis(3);
  zAxis.fill(0);  zAxis[2] = 1; // Positive Z axis direction
  if ( dot_product( zAxis, modelAxis ) < 0)
    {
    modelAxis *= -1;
    }
  std::cout<< "Centroid of the points: " << modelCenter << std::endl;
  std::cout<< "Principle axis: " << modelAxis << std::endl;

  // Construct the initial transform
  igstk::Transform               transform;
  igstk::Transform::VersorType   rotation;
  igstk::Transform::VectorType   translation, fAxis, mAxis, axis;
  double                         angle;

  for (int i=0; i<3; i++)
    {
    fAxis[i] = fiducialAxis[i];
    mAxis[i] = modelAxis[i];
    }
  
  angle = acos( fAxis * mAxis );
  axis = itk::CrossProduct( mAxis, fAxis );
  axis.Normalize();

  // Calculate the initial translation
  for (int i=0; i<3; i++)
    {
    translation[i] = fiducialCenter[i] - modelCenter[i];
    }

  // Translate angle-axis into quaternion
  rotation.Set( axis, angle );
  rotation.Normalize();

  transform.SetTranslationAndRotation( translation, rotation, 0.1, 1e300 );

  m_Transform = transform;
  return true;
}
