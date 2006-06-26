/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    ModelBasedClustering.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "ModelBasedClustering.h"

#include "vnl/vnl_matrix.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "vnl/vnl_cross.h"


ModelBasedClustering::ModelBasedClustering()
{
  m_SamplePoints.clear();
  m_ModelPoints.clear();
  m_ClusteredPoints.clear();

}

void ModelBasedClustering::PrintSelf(std::ostream& os, itk::Indent indent)
{
  Superclass::PrintSelf(os, indent);
}

bool ModelBasedClustering::Execute()
{ 
  const int N = m_SamplePoints.size();
  // Calculate Distance Map and clustering the points
  // Get rid of far way high intensity points
  vnl_matrix< double > distanceMap( N, N );
  distanceMap.fill( 0.0 );
  vnl_vector< double > distAvr( N );
  distAvr.fill( 0.0 );
  for ( int i=0; i<N; i++)
    {
    for ( int j=0; j<N; j++)
      {
      distanceMap( i, j ) = this->Distance( m_SamplePoints[i], 
                                            m_SamplePoints[j] );
      distAvr[i] += distanceMap( i, j );
      }
    }
  distAvr /= N-1;

  // Clustering according to the average distance and distance threshold
  bool done = false;
  const double distT = 48; //46.6345
  const int    stopCounts = 2;
  int index;
  std::vector< int > deleteList;
  deleteList.clear();

  do {
    // Find the largest average distance
    double max = 0;
    for ( int i=0; i<N; i++)
      {
      if ( distAvr[i] > max )
        {
        max = distAvr[i];
        index = i;        
        }
      }
    // Find this point has how many distance pairs exceed the threshold
    int count = 0;
    for ( int i=0; i<N; i++)
      {
      if ( distanceMap( index,i ) > distT )
        {
        count++;
        }
      }
    // Depending on the count, see if we should to filter out points or stop
    if ( count >= stopCounts)
      {
      deleteList.push_back( index );
      // Update the distanceMap and disAvr
      for ( int i=0; i<N; i++)
        {
        distanceMap( index, i ) = 0;
        distanceMap( i, index ) = 0;
        }
      distAvr.fill( 0.0 );
      for ( int i=0; i<N; i++)
        {
        for ( int j=0; j<N; j++)
          {
          distAvr[i] += distanceMap( i, j );
          }
        }
      distAvr /= N-1-deleteList.size();
      }
    else
      {
      done = true;
      }
  
  } while( !done );

  // Delete points from the m_SamplePoints according to the deleteList
  PointsListType pTemp;
  m_ClusteredPoints.clear();
    for ( int i=0; i<N; i++)
      {
      bool ToBeDelete = false;
      for ( int j=0; j<deleteList.size(); j++)
        {
        if ( i == deleteList[j] )
          {
          ToBeDelete = true;
          }
        }
      if ( !ToBeDelete )
        {
        m_ClusteredPoints.push_back( m_SamplePoints[i] );
        }
      else
        {
        std::cout<< "Delete point: " << m_SamplePoints[i] << std::endl;
        }
      }
  
  return true;
}

double ModelBasedClustering::Distance( PointType p1, PointType p2)
{
  return sqrt( pow(p1[0]-p2[0],2) + pow(p1[1]-p2[1],2) + pow(p1[2]-p2[2],2) );
}

double ModelBasedClustering::DistanceToSimilarity( double dis )
{
  const double delta = 7.076;
  
  dis = dis - delta;
  /*
  return vnl_math_abs( cos( vnl_math::pi * dis / delta ) ) * 
                       pow( vnl_math::e, (-0.5*dis*dis) /(delta*delta) ) /
                       ( delta * vnl_math::sqrt2 * sqrt( vnl_math::pi) );
  */
  dis = dis - delta;
  return pow( vnl_math::e, (-0.5*dis*dis) /(delta*delta) ) /
    ( delta * vnl_math::sqrt2 * sqrt( vnl_math::pi) );
}
