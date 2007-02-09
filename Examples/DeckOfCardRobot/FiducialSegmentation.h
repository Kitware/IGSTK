/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    FiducialSegmentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __FiducialSegmentation_h
#define __FiducialSegmentation_h

#include "itkImage.h"
#include "igstkCTImageSpatialObject.h"
#include "vnl/vnl_matrix.h"

namespace igstk
{

/** \class FiducialSegmentation
*  \brief Segment the fiducial points in the image.
*
* This class takes CT images and segment out the high intensity fiducials 
* appears in the image and return the centroids of the objects.
*
* A high threshold (3000~4000) can be used to roughly identify the metal
* objects in the CT images. 
*
* A MaxSize and MinSize can be used to filter out unwanted objects, such as 
* other big metal objects (needle) in the image, or noise 
*
* A MergeDistance can be specified to merge the points close to each other
*/

class FiducialSegmentation: public itk::Object
{

public:

  typedef FiducialSegmentation              Self;
  typedef itk::Object                       Superclass;
  typedef itk::SmartPointer< Self >         Pointer;
  typedef itk::SmartPointer< const Self >   ConstPointer;

  itkNewMacro( Self );

  // Interface to IGSTK image data, it's an itkOrientedImage
  typedef igstk::CTImageSpatialObject::ImageType   ImageType;
  
  typedef itk::Image< signed short, 3>             CTImageType;
  typedef itk::Image< int, 3 >                     BinaryImageType;
  typedef itk::Point< double, 3 >                  PointType;
  typedef std::vector< PointType >                 PointsListType;
  typedef vnl_matrix< double >                     DistanceMapType;

  itkGetMacro( ITKImage, ImageType::ConstPointer );
  itkSetMacro( ITKImage, ImageType::ConstPointer );
 
  itkGetMacro( SegmentedImage, BinaryImageType::ConstPointer );
   
  itkGetMacro( Threshold, short );
  itkSetMacro( Threshold, short );

  itkGetMacro( MinSize, int );
  itkSetMacro( MinSize, int );

  itkGetMacro( MaxSize, int );
  itkSetMacro( MaxSize, int );

  itkGetMacro( MergeDistance, double );
  itkSetMacro( MergeDistance, double );

  bool Execute();

  PointsListType GetFiducialPoints()
    {
    return m_FiducialPoints;
    };


protected:

  FiducialSegmentation();
  virtual ~FiducialSegmentation(){};
  void PrintSelf(std::ostream& os, itk::Indent indent) const;

  DistanceMapType DistanceMap( PointsListType pointsList);

private:

  ImageType::ConstPointer                                 m_ITKImage;
  BinaryImageType::ConstPointer                           m_SegmentedImage;
  
  short                                                   m_Threshold;

  int                                                     m_MinSize;
  int                                                     m_MaxSize;
  double                                                  m_MergeDistance;

  PointsListType                                          m_FiducialPoints;

};

}

#endif
