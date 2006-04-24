/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    DOCR_Registration.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// -----------------------------------------------------------------------------
// Deck of cards robot registration
// 
// v0.1
//
// Roland Stenzel, ISIS Center
// -----------------------------------------------------------------------------
// Output data:
// 
//   - PCA: CT coordinate system -> PCA system:
// 
//           translation: m_meanPoint, i.e. new point = old point - m_meanPoint
//           rotation:    m_eigenVectors (m_inverseEigenVectors available)
// 
//   - PtP Registration: PCA system -> needle holder/end effector/Guide system:
// 
//           translation: 3-5 (x,y,z) of m_parameters
//           rotation:    0-2 (x,y,z) of m_parameters
// 
// In case the relations between all segmented fiducials are exactly the same
// everytime, we would NOT need the PtP registration. But in reality it might be
// possible that the segmentation result is slightly different, i.e. the
// distance between the centers of mass of two segmented fiducials is slightly
// different. This means, in reality it might be usefull due to segmentation
// errors.
// If we decide not to use PtP, the transform from PCA system to needle holder
// system could be hard coded into the software because it is ALWAYS the same.
// Believe me.
// See http://en.wikipedia.org/wiki/Principal_components_analysis
// The PCA -> Needle holder transform is always the same because the PCA
// algorithm is transforming the coordinate system in such a way that the 3.
// dimension has the biggest variance, i.e. it is "aligning" the data to the z
// axis (main axis). The relation between the fiducials is always the same, i.e.
// the main axis is always the same. Only the transformation changes. This
// means, the PCA is enough for our purpose, if the relations between the
// fiducials after segmentation stays the same.
// -Roland
// -----------------------------------------------------------------------------

// ----- PCA and sorting ----------------
#include <vector>
#include <algorithm>
#include <vnl/vnl_vector.h> 
#include <vnl/vnl_matrix.h> 
#include <vnl/algo/vnl_symmetric_eigensystem.h> 

// ----- Pointset to pointset registration --------------
#include "itkEuler3DTransform.h"
#include "itkEuclideanDistancePointMetric.h"
#include "itkLevenbergMarquardtOptimizer.h"
#include "itkPointSetToPointSetRegistrationMethod.h"

#include "itkImageRegionConstIterator.h"

class DOCR_Registration {

#define NUM_FIDUCIALS 9
#define SPACE_DIMENSION 3

  typedef itk::Image< unsigned short, 3 >              USVolumeType;

  // Point to point registration
  typedef itk::PointSet< float, 3 >                    PointSetType;
  typedef PointSetType::PointType                      PointType;
  typedef PointSetType::PointsContainer                PointsContainer;
  // Metric
  typedef itk::EuclideanDistancePointMetric< PointSetType, 
                                             PointSetType >      MetricType;
  typedef MetricType::TransformType                    TransformBaseType;
  typedef TransformBaseType::ParametersType            ParametersType;
  typedef TransformBaseType::JacobianType              JacobianType;
  // Transform
  typedef itk::Euler3DTransform< double >              TransformType;
  // Optimizer
  typedef itk::LevenbergMarquardtOptimizer             OptimizerType;
  // Registration
  typedef itk::PointSetToPointSetRegistrationMethod< 
                         PointSetType, PointSetType >  RegistrationType;

public:
  DOCR_Registration(USVolumeType *volume, USVolumeType::SpacingType spacing);
  ~DOCR_Registration();
  void compute();

  USVolumeType                       *m_USLoadedVolume;
  USVolumeType::SpacingType          m_CT_Spacing;
  vnl_matrix<double>                 m_eigenVectors;
  vnl_matrix<double>                 m_covariance;
  vnl_matrix<double>                 m_inverseEigenVectors;
  std::vector<double>                m_eigenValues;
  vnl_vector<double>                 m_meanPoint;
  std::vector< vnl_vector<double> >  m_transformedData;
  USVolumeType::SizeType             m_volumeSize;
  unsigned short            m_fiducials[NUM_FIDUCIALS][100][SPACE_DIMENSION];
  unsigned short                     m_numFiducials;
  unsigned short                     m_sortedFiducials[NUM_FIDUCIALS];
  double                             m_transformedDataZ[NUM_FIDUCIALS];
  TransformType::OutputVectorType    m_translation;
  TransformType::MatrixType          m_TransformMatrix;
  TransformType::ParametersType      m_parameters;
  OptimizerType::MeasureType         m_optimizerValues;
};
