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
// Attention for using mha files: There is a reflection in z dimension!
// You can decide whether you want to reflect m_transform OR points
// from CT space. (Search for "// reflection".)
// 
// -----------------------------------------------------------------------------
//
// Verification:
//
// There is a build-in verification: Set m_verificationPoint (physical space)
// in the constructor. The corresponding point in needle holder space is
// displayed via std::cout at the end of compute().
// Measurement of m_verificationPoint is 
//
// origin[j] + (avg[j]+m_ROIstart[j]) * m_CT_Spacing[j]
//
// -----------------------------------------------------------------------------
//
//Comments:
//
// I changes type name USVolumeType to SSVolumeType.
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

#include "igstkObject.h"
#include "igstkCTImageSpatialObject.h"


namespace igstk {

  class DOCR_Registration : public Object
{

#define NUM_FIDUCIALS 18 //8 //9
#define NUM_SEGMENTATION_LABELS NUM_FIDUCIALS*3
#define SPACE_DIMENSION 3
#define BIGBINSIZE 30
#define DUMMY 4096/BIGBINSIZE
#define HIGHESTGRAYVALUE 3071 // depends on if data is signed or unsigned
#define NUMBERBIGBINS (((4096 - (DUMMY)*BIGBINSIZE) == 0) ? DUMMY : (DUMMY + 1))
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

public:
  typedef igstk::CTImageSpatialObject                 ImageSOType;

  typedef igstk::CTImageSpatialObject::ImageType      SSVolumeType;
  //typedef itk::Image< unsigned short, 3 >           SSVolumeType;
  typedef igstk::CTImageSpatialObject::ImageType      LandmarkImageType;
  //typedef itk::Image< unsigned short, 3 >           LandmarkImageType;

  typedef itk::VersorRigid3DTransform< double >       TransformType;
  
  typedef itk::LandmarkBasedTransformInitializer< TransformType,
               LandmarkImageType, LandmarkImageType > TransformInitializerType;

  typedef itk::Statistics::ScalarImageToHistogramGenerator< 
                                     SSVolumeType >   HistogramGeneratorType;

  typedef HistogramGeneratorType::HistogramType       HistogramType;

  igstkObserverConstObjectMacro( ITKImage,
                              ImageSOType::ITKImageModifiedEvent, SSVolumeType)

  DOCR_Registration( ImageSOType::Pointer imageSO,
                     SSVolumeType::IndexType ROIstart,
                     SSVolumeType::SizeType ROIsize );
  
  DOCR_Registration( SSVolumeType::Pointer volume,
                     SSVolumeType::IndexType ROIstart,
                     SSVolumeType::SizeType ROIsize );

  ~DOCR_Registration();
  bool compute();

  SSVolumeType::ConstPointer         m_SSEntireLoadedVolume;
  SSVolumeType::Pointer              m_SSLoadedVolume;

  SSVolumeType::SpacingType          m_CT_Spacing;
  SSVolumeType::IndexType            m_ROIstart;
  SSVolumeType::SizeType             m_ROIsize;
  vnl_matrix<double>                 m_eigenVectors;
  vnl_matrix<double>                 m_covariance;
  vnl_matrix<double>                 m_inverseEigenVectors;
  std::vector<double>                m_eigenValues;
  vnl_vector<double>                 m_meanPoint;
  std::vector< vnl_vector<double> >  m_transformedData;
  SSVolumeType::SizeType             m_volumeSize;
  unsigned short     m_fiducials[NUM_SEGMENTATION_LABELS][100][SPACE_DIMENSION],
                                     m_numFiducials,
                                     m_sortedFiducials[NUM_FIDUCIALS];
  double                             m_transformedDataZ[NUM_FIDUCIALS],
                                     m_verificationPoint[SPACE_DIMENSION];
  TransformInitializerType::OutputVectorType  error;
  float                              m_bins[NUMBERBIGBINS],
                                     m_meanRegistrationError,
                                     m_maxRegistrationError;
  int                                m_threshold;
  TransformType::Pointer             m_transform;
  bool                               m_usingMHA;
};

}
