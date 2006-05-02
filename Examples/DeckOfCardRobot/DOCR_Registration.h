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
// Roland Stenzel, ISIS Center, April 2006
// -----------------------------------------------------------------------------
//
// Output data (CT space --> needle holder space):
// 
// What ever you want from...
//
// VersorRigid3DTransform< double >::Pointer m_transform
//
// Attention: There is a reflection in z dimension!
// You can decide whether you want to reflect m_transform OR points
// from CT space. (Search for "// reflection".)
// 
// -----------------------------------------------------------------------------
//
// Verification:
//
// There is a build-in verification: Set m_verificationPoint (CT space) in the
// constructor. The corresponding point in needle holder space is displayed
// via std::cout at the end of compute().
// Measurement of m_verificationPoint is mm, i.e. you have to include spacing.
//
// -----------------------------------------------------------------------------

// ----- PCA and sorting ----------------
#include <vector>
#include <algorithm>
#include <vnl/vnl_vector.h> 
#include <vnl/vnl_matrix.h> 
#include <vnl/algo/vnl_symmetric_eigensystem.h> 

// ----- registration --------------
// C 13
#include "itkLandmarkBasedTransformInitializer.h"

#include "itkImageRegionConstIterator.h"

// Histogram
#include "itkScalarImageToHistogramGenerator.h"

// ----- Region of interest --------
#include "itkRegionOfInterestImageFilter.h"

// ----- test --------
#include "itkImageFileWriter.h"

class DOCR_Registration {

#define NUM_FIDUCIALS 19 //8 //9
#define SPACE_DIMENSION 3
#define BIGBINSIZE 30
#define DUMMY 4096/BIGBINSIZE
#define NUMBERBIGBINS (((4096 - (DUMMY)*BIGBINSIZE) == 0) ? DUMMY : (DUMMY + 1))
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

  typedef itk::Image< unsigned short, 3 >              USVolumeType;

  typedef itk::Image< unsigned short, 3 >  LandmarkImageType;
  typedef itk::VersorRigid3DTransform< double > TransformType;
  typedef itk::LandmarkBasedTransformInitializer< TransformType,
    LandmarkImageType, LandmarkImageType > TransformInitializerType;
  typedef itk::Statistics::ScalarImageToHistogramGenerator< 
    USVolumeType >   HistogramGeneratorType;
  typedef HistogramGeneratorType::HistogramType  HistogramType;


  // C 11
public:
  DOCR_Registration(USVolumeType *volume,
                    USVolumeType::SpacingType spacing,
                    USVolumeType::IndexType ROIstart,
                    USVolumeType::SizeType ROIsize);
  ~DOCR_Registration();
  void compute();

  USVolumeType                       *m_USEntireLoadedVolume,
                                     *m_USLoadedVolume;
  USVolumeType::SpacingType          m_CT_Spacing;
  USVolumeType::IndexType            m_ROIstart;
  USVolumeType::SizeType             m_ROIsize;
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
  TransformInitializerType::OutputVectorType  error;
  float                              m_bins[NUMBERBIGBINS];
  unsigned int                       m_threshold;
  TransformType::Pointer             m_transform;
  double                             m_verificationPoint[3];
};
