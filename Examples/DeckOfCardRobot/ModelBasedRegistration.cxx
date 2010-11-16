/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    ModelBasedRegistration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

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
#include "itkVersorRigid3DTransformOptimizer.h"
#include "itkImageRegistrationMethod.h"
#include "itkTranslationTransform.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImage.h"

namespace igstk
{

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
  for ( unsigned int i=0; i<fiducialPoints.size(); i++)
    {
    for ( unsigned int j=0; j<3; j++)
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

  for (unsigned int i=0; i<3; i++)
    {
    fAxis[i] = fiducialAxis[i];
    mAxis[i] = modelAxis[i];
    }
  
  angle = acos( fAxis * mAxis );
  axis = itk::CrossProduct( mAxis, fAxis );
  axis.Normalize();

  // Calculate the initial translation
  for (unsigned int i=0; i<3; i++)
    {
    translation[i] = fiducialCenter[i] - modelCenter[i];
    }

  // Translate angle-axis into quaternion
  rotation.Set( axis, angle );
  rotation.Normalize();

  transform.SetTranslationAndRotation( translation, rotation, 0.1, 1e300 );

  m_Transform = transform;

  // define itk transform type 
  typedef itk::VersorRigid3DTransform< double > TransformType;
  TransformType::Pointer  itkTransform = TransformType::New();
  itkTransform->SetRotation( rotation );
  itkTransform->SetTranslation ( translation );

  // Define the registration optimizer type 
  typedef itk::VersorRigid3DTransformOptimizer           OptimizerType;
  OptimizerType::Pointer      optimizer         = OptimizerType::New();

  // Define the image types
  typedef itk::Image< int , 3 >              SegmentedImageType;
  typedef itk::Image< double, 3 >            FiducialModelImageType;

  // Define metric type
  typedef itk::MeanSquaresImageToImageMetric< 
                                    SegmentedImageType, 
                                    FiducialModelImageType >    MetricType;
  MetricType::Pointer         metric        = MetricType::New();

  // Define interpolator type
  typedef itk:: LinearInterpolateImageFunction< 
                                    FiducialModelImageType,
                                    double          >    InterpolatorType;
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();

  // Define image registration type
  typedef itk::ImageRegistrationMethod< 
                          SegmentedImageType, 
                          FiducialModelImageType >    RegistrationType;
  
  RegistrationType::Pointer   registration  = RegistrationType::New();

  typedef RegistrationType::ParametersType ParametersType;
 
  // initialize the registration components
  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetTransform(    itkTransform   );
  registration->SetInterpolator(  interpolator  );
  registration->SetFixedImage(    segmenter->GetSegmentedImage()    );
  registration->SetMovingImage(   model->GetModelImage()   );
   
  // set optmizer parameters
  typedef OptimizerType::ScalesType       OptimizerScalesType;
  OptimizerScalesType optimizerScales( itkTransform->GetNumberOfParameters() );
  const double translationScale = 1.0 / 1000.0;
  optimizerScales[0] = 1.0;
  optimizerScales[1] = 1.0;
  optimizerScales[2] = 1.0;
  optimizerScales[3] = translationScale;
  optimizerScales[4] = translationScale;
  optimizerScales[5] = translationScale;
  optimizer->SetScales( optimizerScales );
  optimizer->SetMaximumStepLength( 0.2000  ); 
  optimizer->SetMinimumStepLength( 0.0001 );
  optimizer->SetNumberOfIterations( 200 );

  // connect an observer
  typedef IterationCallback< OptimizerType >   IterationCallbackType;
  IterationCallbackType::Pointer callback = IterationCallbackType::New();
  callback->SetOptimizer( optimizer );

  // initialize the parameters
  registration->SetInitialTransformParameters( itkTransform->GetParameters() );
  // run the registration component  
  try 
    { 
    registration->StartRegistration(); 
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    return -1;
    } 

  ParametersType finalParameters = registration->GetLastTransformParameters();

  const double versorX              = finalParameters[0];
  const double versorY              = finalParameters[1];
  const double versorZ              = finalParameters[2];
  const double finalTranslationX    = finalParameters[3];
  const double finalTranslationY    = finalParameters[4];
  const double finalTranslationZ    = finalParameters[5];

  const unsigned int numberOfIterations = optimizer->GetCurrentIteration();
  const double bestValue = optimizer->GetValue();
  
  // Print out results
  std::cout << std::endl << std::endl;
  std::cout << "Result = " << std::endl;
  std::cout << " versor X      = " << versorX  << std::endl;
  std::cout << " versor Y      = " << versorY  << std::endl;
  std::cout << " versor Z      = " << versorZ  << std::endl;
  std::cout << " Translation X = " << finalTranslationX  << std::endl;
  std::cout << " Translation Y = " << finalTranslationY  << std::endl;
  std::cout << " Translation Z = " << finalTranslationZ  << std::endl;
  std::cout << " Iterations    = " << numberOfIterations << std::endl;
  std::cout << " Metric value  = " << bestValue          << std::endl;
  return true;
}

} // end namespace igstk
