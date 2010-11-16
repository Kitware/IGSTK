/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    FiducialSegmentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "FiducialSegmentation.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkImageRegionConstIteratorWithIndex.h"

namespace igstk
{

FiducialSegmentation::FiducialSegmentation()
{
  m_ITKImage      = NULL;
  m_Threshold     = 3000; // High intensity value for metal object
  m_MaxSize       = itk::NumericTraits< int >::max(); 
  m_MinSize       = 0;
  m_MergeDistance = 0;
  m_FiducialPoints.clear();
}

void FiducialSegmentation::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Threshold: "
  << m_Threshold << std::endl;
  os << indent << "Maximum Fiducial Size: "
  << m_MaxSize << std::endl;
  os << indent << "Minimum Fiducial Size: "
  << m_MinSize << std::endl;
  os << indent << "Distance for merging two fiducial points: "
  << m_MergeDistance << std::endl;
}

bool FiducialSegmentation::Execute()
{  

  if ( !m_ITKImage )
    {
    itkExceptionMacro( "No input image for segmentation!!" );
    return false;
    }
  
  /*-------------------------------------------------------------------------*/
  std::cout<< "Executing fiducial segmentation.... " << std::endl;

  // Binary threshold filtering
  typedef itk::BinaryThresholdImageFilter< ImageType, BinaryImageType > 
                                                            ThresholdFilterType;
  ThresholdFilterType::Pointer threshold = ThresholdFilterType::New();

  threshold->SetInput( m_ITKImage );
  threshold->SetInsideValue( 1 );
  threshold->SetOutsideValue( 0 );
  threshold->SetLowerThreshold( m_Threshold );
  threshold->SetUpperThreshold( 10000 );  // Maximum value in CT images

  // Labeling and relabeling the result binary image
  typedef itk::ConnectedComponentImageFilter< BinaryImageType, BinaryImageType >
                                                               LabelFilterType;
  LabelFilterType::Pointer  label = LabelFilterType::New();

  typedef itk::RelabelComponentImageFilter< BinaryImageType, BinaryImageType >
                                                             RelabelFilterType;
  RelabelFilterType::Pointer relable = RelabelFilterType::New();

  label->SetInput( threshold->GetOutput() );
  relable->SetInput( label->GetOutput() );
  relable->Update();

  this->m_SegmentedImage = relable->GetOutput();
      
  int N = relable->GetOriginalNumberOfObjects();
  std::cout<< "Number of objects found: "<< N << std::endl;

  /*-------------------------------------------------------------------------*/
  std::cout<< "filtering detected fiducial points according to their size and"; 
  std::cout<< " distances.... " << std::endl;
  std::cout<< " Max size.... " << m_MaxSize << std::endl;
  std::cout<< " Min size.... " << m_MinSize << std::endl;


  // Collecting indices for each label  
  std::vector< itk::ContinuousIndex< double, 3 > > indices;
  for (unsigned int i=0; i<=N; i++)
    {
    itk::ContinuousIndex< double, 3 > index;
    index.Fill( 0.0 );
    indices.push_back( index );
    }

  itk::ImageRegionConstIteratorWithIndex< BinaryImageType > it;
  it = itk::ImageRegionConstIteratorWithIndex< BinaryImageType >
    ( relable->GetOutput(), relable->GetOutput()->GetLargestPossibleRegion());

  it.GoToBegin();
  while ( !it.IsAtEnd() )
    {
    if (it.Get() == 0 )
      {
      ++it;
      continue;
      }
    // Adding indices together
    indices[ it.Get() ][0] += it.GetIndex()[0];
    indices[ it.Get() ][1] += it.GetIndex()[1];
    indices[ it.Get() ][2] += it.GetIndex()[2];
    ++it;
    }

  // Size for each label
  std::vector< float > size = relable->GetSizeOfObjectsInPhysicalUnits();  

  PointType p; 
  m_FiducialPoints.clear();
  for (unsigned int i=0; i<N; i++)
    {
    // Filter out abnormal size labels
    if ( (size[i]<=m_MaxSize) & (size[i]>=m_MinSize) )
      {
      // Calculate the centroid for each label
      indices[i+1][0] /= relable->GetSizeOfObjectsInPixels()[i];
      indices[i+1][1] /= relable->GetSizeOfObjectsInPixels()[i];
      indices[i+1][2] /= relable->GetSizeOfObjectsInPixels()[i];
      m_ITKImage->TransformContinuousIndexToPhysicalPoint( indices[i+1], p );
      m_FiducialPoints.push_back( p );
      }
    else
      {
      std::cout<< "Discard object with size: "<< size[i] << std::endl;
      }
    }
  
  // Calculate Distance Map and recursively merge two closest points
  DistanceMapType distanceMap = this->DistanceMap( m_FiducialPoints );
  int pa, pb;
  double minDistance = distanceMap.max_value();
  N = m_FiducialPoints.size();
  for ( int i=0; i<N; i++)
    {
    for ( int j=0; j<N; j++)
      {
      if ( ( i!= j) && ( distanceMap(i,j) < minDistance) )
        {
        minDistance = distanceMap(i,j);
        pa = i; pb = j; // Find the minimum distance points pair
        }
      }
    }

  while ( minDistance < m_MergeDistance )
    {
    // Merge m_FiducialPoints[pa] and m_FiducialPoints[pb]
    std::cout<< "Merging fiducial point: " << std::endl;
    std::cout<< m_FiducialPoints[pa]       << std::endl;
    std::cout<< "and fiducial point: "     << std::endl;
    std::cout<< m_FiducialPoints[pb]       << std::endl;
    std::cout<< "with ditance of: "        << minDistance << std::endl;

    m_FiducialPoints[pa][0] = 
                        ( m_FiducialPoints[pa][0] + m_FiducialPoints[pb][0])/2;
    m_FiducialPoints[pa][1] = 
                        ( m_FiducialPoints[pa][1] + m_FiducialPoints[pb][1])/2;
    m_FiducialPoints[pa][2] = 
                        ( m_FiducialPoints[pa][2] + m_FiducialPoints[pb][2])/2;
    m_FiducialPoints.erase( m_FiducialPoints.begin() + pb );


    distanceMap = this->DistanceMap( m_FiducialPoints );
    minDistance = distanceMap.max_value();
    N = m_FiducialPoints.size();
    for ( int i=0; i<N; i++)
      {
      for ( int j=0; j<N; j++)
        {
        if ( ( i!= j) && ( distanceMap(i,j) < minDistance) )
          {
          minDistance = distanceMap(i,j);
          pa = i; pb = j; // Find the minimum distance points pair
          }
        }
      }
    }
    
  std::cout<< "Fiducial segmentation results: " << std::endl;
  std::cout<< "Number of fiducial points detected: " << 
                                          m_FiducialPoints.size() << std::endl;
  for (unsigned int i=0; i<m_FiducialPoints.size(); i++)
    {
     std::cout<< m_FiducialPoints[i] << std::endl;
    }

  return true;
}

FiducialSegmentation::DistanceMapType 
FiducialSegmentation::DistanceMap( PointsListType pointList )
{
  const int N = pointList.size();
  
  vnl_matrix< double > distanceMap( N, N );
  distanceMap.fill( 0.0 );

  for ( int i=0; i<N; i++)
    {
    for ( int j=0; j<N; j++)
      {
      distanceMap( i, j ) = sqrt( pow( pointList[i][0]-pointList[j][0],2) + 
                                  pow( pointList[i][1]-pointList[j][2],2) +
                                  pow( pointList[i][1]-pointList[j][2],2) );
      }
    }

  return distanceMap;

}

} // end namespace igstk
