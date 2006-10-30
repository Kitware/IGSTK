/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    PointsClustering.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __PointsClustering_h
#define __PointsClustering_h

#include "itkImage.h"
#include "vnl/vnl_matrix.h"

namespace igstk
{

/** \class PointsClustering
*  \brief Perform agglomerative hierarchical clustering on points set.
*
* This class takes a set of points and clustering them based on their distances.
*
* Distance can be defined as MAX_MIN_DISTANCE or AVE_MIN_DISTANCE
*
* A MaxMergeDistance can be control to stop the clustering process, if no 
* specific value is being set, the final clustering result will be an single 
* cluster
*
*/

class PointsClustering: public itk::Object
{

public:

  typedef PointsClustering                  Self;
  typedef itk::Object                       Superclass;
  typedef itk::SmartPointer< Self >         Pointer;
  typedef itk::SmartPointer< const Self >   ConstPointer;

  itkNewMacro( Self );

  typedef itk::Point< double, 3 >           PointType;
  typedef std::vector< PointType >          PointsListType;
  typedef std::vector< int >                PointIDsListType;
  typedef std::vector< PointsListType >     PointClusterType;
  typedef std::vector< PointIDsListType >   PointIDClusterType;

  typedef enum
    {
    MAX_MIN_DISTANCE = 1,
    AVE_MIN_DISTANCE = 2,
    CENTER_TO_CENTER = 3,
    } DistanceType;

  bool Execute();

  void SetSamplePoints( PointsListType pList )
    {
    m_SamplePoints = pList;
    }

  itkSetMacro( MaxMergeDistance,  double );
  itkGetMacro( LastMergeDistance, double );
  itkSetMacro( DistanceType,      int );

  PointClusterType GetCluster()
    {
    return m_Cluster;
    }

protected:
  PointsClustering();
  virtual ~PointsClustering(){};
  void PrintSelf( std::ostream & os, itk::Indent indet );

private:

  double EuclideanDistance( PointType p1, PointType p2 );

  double MaxMinDistance( PointsListType pts1, PointsListType pts2 );
  double AveMinDistance( PointsListType pts1, PointsListType pts2 );
  double CenterToCenter( PointsListType pts1, PointsListType pts2 );

  double MaxMinDistance( PointIDsListType ids1, PointIDsListType ids2 );
  double AveMinDistance( PointIDsListType ids1, PointIDsListType ids2 );
  double CenterToCenter( PointIDsListType ids1, PointIDsListType ids2 );

  PointsListType                       m_SamplePoints;
  PointClusterType                     m_Cluster;
  PointIDClusterType                   m_IDCluster;
  double                               m_MaxMergeDistance;
  double                               m_LastMergeDistance;
  int                                  m_DistanceType;
  vnl_matrix<double>                   m_DistanceMap;

};

} // end namespace igstk

#endif
