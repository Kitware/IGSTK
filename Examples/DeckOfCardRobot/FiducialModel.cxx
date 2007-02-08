/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    FiducialModel.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
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

namespace igstk 
{

FiducialModel::FiducialModel()
{
  m_ModelImage = NULL;
  m_Size = 4;
  m_BlurSize = 1;
  m_FiducialPoints.clear();

  std::cout << "Generating fiducial points...." << std::endl;
  this->GenerateFiducialPointList();
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

  GroupSpatialObjectType::Pointer groupSpatialObject = 
                                          GroupSpatialObjectType::New(); 
 
  typedef itk::SpatialObjectToImageFilter< GroupSpatialObjectType, ImageType>
                                          SpatialObjectToImageFilterType;

  SpatialObjectToImageFilterType::Pointer spatialObjectToImageFilter = 
                                       SpatialObjectToImageFilterType::New();
    
  const double FiducialSize = this->m_Size;

  PointsListType::const_iterator it; 

  double minx =  1000.0;
  double maxx = -1000.0;
  double miny =  1000.0;
  double maxy = -1000.0;
  double minz =  1000.0;
  double maxz = -1000.0;

  for ( it = m_FiducialPoints.begin(); it != m_FiducialPoints.end(); it++ )
    {
    EllipseSpatialObjectType::Pointer ellipse = 
                     EllipseSpatialObjectType::New();
    ellipse->SetRadius( FiducialSize );
   
    std::cout << "Working on fiducial: " << *it << std::endl; 
    OffsetType offset;
    offset[0]  = (*it)[0]; 
    offset[1]  = (*it)[1]; 
    offset[2]  = (*it)[2]; 
    ellipse->GetObjectToParentTransform()->SetOffset( offset );
    ellipse->ComputeObjectToWorldTransform();
    groupSpatialObject->AddSpatialObject( ellipse );

    minx = ( offset[0] < minx ) ? offset[0] : minx;
    maxx = ( offset[0] > maxx ) ? offset[0] : maxx;
    miny = ( offset[1] < miny ) ? offset[1] : miny;
    maxy = ( offset[1] > maxy ) ? offset[1] : maxy;
    minz = ( offset[2] < minz ) ? offset[2] : minz;
    maxz = ( offset[2] > maxz ) ? offset[2] : maxz;
    } 

  std::cout << " Bounding box for the fiducial coordinates: " 
            << "minx=" << minx << "maxx=" << maxx 
            << "miny=" << miny << "maxy=" << maxy
            << "minz=" << minz << "maxz=" << maxz << std::endl;

  spatialObjectToImageFilter->SetInput( groupSpatialObject );
  
  ImageType::PointType origin;
  origin[ 0 ] = minx - 5;
  origin[ 1 ] = miny - 5;
  origin[ 2 ] = minz - 5;

  std::cout << "Image origin: " << origin << std::endl;
  spatialObjectToImageFilter->SetOrigin( origin );

  ImageType::SpacingType spacing;
  spacing[ 0 ] = 1.0;
  spacing[ 1 ] = 1.0;
  spacing[ 2 ] = 1.0;
  std::cout << "Image spacing: " << spacing << std::endl;
  spatialObjectToImageFilter->SetSpacing( spacing );

  ImageType::SizeType  size;
  size[ 0 ] = (unsigned long) ((maxx - minx)/spacing[0]) + 5;
  size[ 1 ] = (unsigned long) ((maxy - miny)/spacing[1]) + 5;
  size[ 2 ] = (unsigned long) ((maxz - minz)/spacing[2]) + 5;
  std::cout << "Image size: " << size << std::endl;

  spatialObjectToImageFilter->SetSize( size );
  spatialObjectToImageFilter->Update();

  // Smooth the image
  typedef itk::Image< double, 3 >       DoubleImageType;
  
  typedef itk::DiscreteGaussianImageFilter < ImageType, DoubleImageType > 
                                         GaussianImageFilterType;
  
  GaussianImageFilterType::Pointer smoothingFilter = 
                                       GaussianImageFilterType::New();

  smoothingFilter->SetInput ( spatialObjectToImageFilter->GetOutput() );
  const double smoothingVariance = 0.2; 
  smoothingFilter->SetVariance( smoothingVariance );
  smoothingFilter->Update();
  this->m_ModelImage = smoothingFilter->GetOutput(); 
}

void FiducialModel::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Size: " << m_Size << std::endl;
  os << indent << "Blur Size: " << m_BlurSize << std::endl;
}

FiducialModel::DoubleImageType::Pointer
FiducialModel::GetModelImage()
{
  return m_ModelImage;
}

} // end namespace igstk
