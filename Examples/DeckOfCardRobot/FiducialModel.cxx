/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    FiducialModel.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "FiducialModel.h"

#include "itkGroupSpatialObject.h"
#include "itkEllipseSpatialObject.h"
#include "itkSpatialObjectToImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"

FiducialModel::FiducialModel()
{
  m_ModelImage = NULL;
  m_Size = 4;
  m_BlurSize = 1;
  m_FiducialPoints.clear();

  this->GenerateFiducialPointList();
  this->GenerateModelImage();
}

void FiducialModel::GenerateFiducialPointList()
{
  double const R = sqrt( 3.3 * 3.3 + 8.0 * 8.0 );
  double const W = vnl_math::pi / 10;
  double const Theta = atan( 8 / 3.3 );

  PointType p;
  for ( double z = 0; z <= 45; z += 2.5)
    {
    double x = R * cos( W * z + Theta );
    double y = R * sin( W * z + Theta );
    p[0] = x;
    p[1] = y;
    p[2] = z - 1.25;
    m_FiducialPoints.push_back( p );
    }
}

void FiducialModel::GenerateModelImage()
{
  typedef itk::EllipseSpatialObject< 3 >  EllipseSpatialObjectType;
  typedef EllipseSpatialObjectType::TransformType::OffsetType OffsetType; 
  
  typedef itk::GroupSpatialObject< 3 >    GroupSpatialObjectType;
  GroupSpatialObjectType::Pointer groupSpatialObject = GroupSpatialObjectType::New(); 
 
  typedef itk::SpatialObjectToImageFilter< GroupSpatialObjectType, ImageType>
                                          SpatialObjectToImageFilterType;
  SpatialObjectToImageFilterType::Pointer 
               spatialObjectToImageFilter = SpatialObjectToImageFilterType::New();
    
  const double FiducialSize = this->m_Size;

  PointsListType::const_iterator it; 

  for ( it = m_FiducialPoints.begin(); it < m_FiducialPoints.end(); it++ )
    {
      EllipseSpatialObjectType::Pointer ellipse = EllipseSpatialObjectType::New();
      ellipse->SetRadius( FiducialSize );
      
      OffsetType offset;
      offset[0]  = (*it)[0]; 
      offset[1]  = (*it)[1]; 
      offset[2]  = (*it)[2]; 
      ellipse->GetObjectToParentTransform()->SetOffset( offset );
      ellipse->ComputeObjectToWorldTransform();
      groupSpatialObject->AddSpatialObject( ellipse );     
    } 

  spatialObjectToImageFilter->SetInput( groupSpatialObject );
  ImageType::SizeType  size;
  size[ 0 ] = 512;
  size[ 1 ] = 512;
  size[ 2 ] = 10;
  spatialObjectToImageFilter->SetSize( size );
  spatialObjectToImageFilter->Update();

  // Smooth the image
  typedef itk::DiscreteGaussianImageFilter < ImageType, ImageType > 
                                         GaussianImageFilterType;
  GaussianImageFilterType::Pointer smoothingFilter = GaussianImageFilterType::New();  

  smoothingFilter->SetVariance( this->m_BlurSize );
  smoothingFilter->Update();
  this->m_ModelImage = smoothingFilter->GetOutput();  
}

void FiducialModel::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Size: "
    << m_Size << std::endl;
  os << indent << "Blur Size: "
    << m_BlurSize << std::endl;

}

FiducialModel::ImageType::Pointer
FiducialModel::GetModelImage()
{
  return m_ModelImage;
}
