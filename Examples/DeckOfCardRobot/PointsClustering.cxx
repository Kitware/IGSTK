/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    PointsClustering.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "PointsClustering.h"

namespace igstk
{

PointsClustering::PointsClustering()
{
  m_SamplePoints.clear();
  m_Cluster.clear();
  m_MaxMergeDistance = 5;
  m_DistanceType = MAX_MIN_DISTANCE;
}

void PointsClustering::PrintSelf(std::ostream& os, itk::Indent indent)
{
  Superclass::PrintSelf(os, indent);
}

bool PointsClustering::Execute()
{ 
  // Check if the sample points list is empty
  const int N = m_SamplePoints.size();
  if ( N == 0 )
    {
    std::cout<< "Nothing to cluster!!" << std::endl;
    return false;
    }
  
  // Initialise the cluster
  m_Cluster.clear();
  m_IDCluster.clear();  
  for ( int i=0; i<N; i++)
    {
    PointsListType pList;
    pList.clear(); pList.push_back( m_SamplePoints[i] );
    m_Cluster.push_back( pList );

    PointIDsListType idList;
    idList.clear(); idList.push_back( i );
    m_IDCluster.push_back( idList );
    }
  
  // Calculate the distance map
  m_DistanceMap = vnl_matrix<double>( N,N );
  m_DistanceMap.fill( 0.0 );
  for ( int i=0; i<N; i++)
    {
    for ( int j=i+1; j<N; j++)
      {
      m_DistanceMap( i, j ) = 
                     EuclideanDistance( m_SamplePoints[i], m_SamplePoints[j] );
      m_DistanceMap( j, i ) = m_DistanceMap( i, j ); // Copy the matrix
      }
    }

  // Build the cluster distance map -- re-scalable
  typedef std::vector< double >              DistanceListType;
  typedef std::vector< DistanceListType >    ClusterDistanceMapType;
  ClusterDistanceMapType   clusterDistanceMap;
  clusterDistanceMap.clear();
  for ( int i=0; i<N; i++)
    {
    DistanceListType dList;
    dList.clear();
    for ( int j=0; j<N; j++)
      {
      dList.push_back ( m_DistanceMap( i, j ) );
      }
    clusterDistanceMap.push_back( dList );
    }

  // Start the iteration of merging and re-calculate distance map
  for ( int n=0; n<N-1; n++)
    {
    unsigned int iID;
    unsigned int jID;
    double minDistance = itk::NumericTraits<double>::max();

    //Find the minimum distance pair (iID is always smaller than jID)
    for ( unsigned int i=0; i< clusterDistanceMap.size(); i++)
      {
      for ( unsigned int j=i+1; j< clusterDistanceMap.size(); j++)
        {
        if ( clusterDistanceMap[i][j] < minDistance )
          {
          minDistance = clusterDistanceMap[i][j];
          iID = i;
          jID = j;
          }
        }
      }
    
    // Print the distance map
    /*
    std::cout <<"--------------------------------------------------------\n";
    std::cout<< "Pair:" << iID << "," << jID << std::endl;
    std::cout<< "Distance:" << minDistance << std::endl;
    for ( int i=0; i<clusterDistanceMap.size(); i++)
      {
      std::cout<< "[";
      for (int j=0; j<clusterDistanceMap[i].size(); j++)
        {
        std::cout<< clusterDistanceMap[i][j] << ",\t";
        }
      std::cout<< "]\n";
      }
    std::cout <<"------------------------------------------------------\n\n";
    */

    // Stop clustering if the minimum distance between clusters are larger than
    // the pre-set threshold
    if ( minDistance > m_MaxMergeDistance)
      {
      break;
      }
    
    m_LastMergeDistance = minDistance;
    // Merge jID cluster into the iID cluster, and delete the jID cluster
    while( !m_Cluster[jID].empty() )
      {
      m_Cluster[iID].push_back( m_Cluster[jID].back() );
      m_Cluster[jID].pop_back();
      }
    m_Cluster.erase( m_Cluster.begin() + jID );
    while( !m_IDCluster[jID].empty() )
      {
      m_IDCluster[iID].push_back( m_IDCluster[jID].back() );
      m_IDCluster[jID].pop_back();
      }
    m_IDCluster.erase( m_IDCluster.begin() + jID );

    // Update iID column and iID row distances in the clusterDistanceMap
    for ( unsigned int i=0; i<clusterDistanceMap.size(); i++)
      {
      clusterDistanceMap[i].erase( clusterDistanceMap[i].begin() + jID );
      }
    clusterDistanceMap.erase( clusterDistanceMap.begin() + jID );
    
    for ( unsigned int j=0; j<clusterDistanceMap.size(); j++)
      {
      if ( j == iID )
        {
        clusterDistanceMap[iID][j] = 0;
        }
      else if( m_DistanceType == MAX_MIN_DISTANCE ) 
        {
        clusterDistanceMap[iID][j] = MaxMinDistance( m_IDCluster[iID], 
                                                     m_IDCluster[j] );
        }
      else if ( m_DistanceType == AVE_MIN_DISTANCE )
        {
        clusterDistanceMap[iID][j] = AveMinDistance( m_IDCluster[iID], 
                                                     m_IDCluster[j] );
        }
      else if ( m_DistanceType == CENTER_TO_CENTER )
        {
        clusterDistanceMap[iID][j] = CenterToCenter( m_Cluster[iID], 
                                                     m_Cluster[j] );
        }
      clusterDistanceMap[j][iID] = clusterDistanceMap[iID][j]; // update column
      }

    }

  // Print the clustering result
  for( unsigned int i=0; i<m_Cluster.size(); i++ )
    {
    std::cout<< "Cluster " << i << " :" << std::endl;
    for (unsigned int j=0; j< m_Cluster[i].size(); j++)
      {
      std::cout << "Point ID: " << m_IDCluster[i][j] << "\t";
      std::cout << m_Cluster[i][j] << std::endl;
      }
    std::cout<< "\n\n";
    }
  std::cout << "Last merge distance: " << m_LastMergeDistance << std::endl;

  return true;
}

double PointsClustering::EuclideanDistance( PointType p1, PointType p2)
{
  return sqrt( pow(p1[0]-p2[0],2) + pow(p1[1]-p2[1],2) + pow(p1[2]-p2[2],2) );
}

double PointsClustering::MaxMinDistance( PointIDsListType ids1, 
                                         PointIDsListType ids2)
{
  double maxD = -1;
  for (unsigned int i=0; i< ids1.size(); i++)
    {
    double minD = itk::NumericTraits<double>::max();
    for (unsigned int j=0; j< ids2.size(); j++)
      {
      minD = ( m_DistanceMap( ids1[i] , ids2[j] ) < minD ) ? 
               m_DistanceMap( ids1[i] , ids2[j] ) : minD;
      }
    maxD = ( maxD > minD ) ? maxD : minD;
    }
  for (unsigned int i=0; i< ids2.size(); i++)
    {
    double minD = itk::NumericTraits<double>::max();
    for (unsigned int j=0; j< ids1.size(); j++)
      {
      minD = ( m_DistanceMap( ids2[i] , ids1[j] ) < minD ) ? 
               m_DistanceMap( ids2[i] , ids1[j] ) : minD;
      }
    maxD = ( maxD > minD ) ? maxD : minD;
    }
  return maxD;
}

double PointsClustering::AveMinDistance( PointIDsListType ids1, 
                                         PointIDsListType ids2)
{
  double aveD = 0;
  for (unsigned int i=0; i< ids1.size(); i++)
    {
    double minD = itk::NumericTraits<double>::max();
    for (unsigned int j=0; j< ids2.size(); j++)
      {
      minD = ( m_DistanceMap( ids1[i] , ids2[j] ) < minD ) ? 
               m_DistanceMap( ids1[i] , ids2[j] ) : minD;
      }
    aveD += minD;
    }
  for (unsigned int i=0; i< ids2.size(); i++)
    {
    double minD = itk::NumericTraits<double>::max();
    for (unsigned int j=0; j< ids1.size(); j++)
      {
      minD = ( m_DistanceMap( ids2[i] , ids1[j] ) < minD ) ? 
               m_DistanceMap( ids2[i] , ids1[j] ) : minD;
      }
    aveD += minD;
    }
  return aveD / ( ids1.size() + ids2.size() );
}

double PointsClustering::CenterToCenter( PointIDsListType ids1, 
                                        PointIDsListType ids2)
{
  return 0.0;
}

double PointsClustering::MaxMinDistance( PointsListType pts1, 
                                         PointsListType pts2)
{
  return 0.0;
}

double PointsClustering::AveMinDistance( PointsListType pts1, 
                                         PointsListType pts2)
{
  return 0.0;
}

double PointsClustering::CenterToCenter( PointsListType pts1, 
                                         PointsListType pts2)
{
  PointType pc1, pc2;
  pc1.Fill( 0.0 ); pc2.Fill( 0.0 );

  for ( unsigned int i=0; i< pts1.size(); i++)
    {
    pc1[0] += pts1[i][0] / pts1.size();
    pc1[1] += pts1[i][1] / pts1.size();
    pc1[2] += pts1[i][2] / pts1.size();
    }

  for ( unsigned int i=0; i< pts2.size(); i++)
    {
    pc2[0] += pts2[i][0] / pts2.size();
    pc2[1] += pts2[i][1] / pts2.size();
    pc2[2] += pts2[i][2] / pts2.size();
    }

  return this->EuclideanDistance( pc1, pc2 );
}

} // end namespace igstk
