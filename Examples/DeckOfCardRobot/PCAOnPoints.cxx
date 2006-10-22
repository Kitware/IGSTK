/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    PCAOnPoints.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "PCAOnPoints.h"

#include "vnl/vnl_matrix.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "vnl/vnl_cross.h"

namespace igstk 
{

PCAOnPoints::PCAOnPoints()
{
  m_SamplePoints.clear();

}

void PCAOnPoints::PrintSelf(std::ostream& os, itk::Indent indent)
{
  Superclass::PrintSelf(os, indent);
}

bool PCAOnPoints::Execute()
{ 
  const int N = m_SamplePoints.size();

  // Calculate mean position of the fiducial points
  m_Center.Fill( 0 );
  for ( int i=0; i<N; i++)
    {
    m_Center[0] = m_Center[0] + m_SamplePoints[i][0];
    m_Center[1] = m_Center[1] + m_SamplePoints[i][1];
    m_Center[2] = m_Center[2] + m_SamplePoints[i][2];
    }  
  m_Center[0] /= N;
  m_Center[1] /= N;
  m_Center[2] /= N;

  // Calculate covariance matrix of the fiducial points
  vnl_matrix< double > covar( 3, 3 );
  covar.fill( 0.0 );

  vnl_vector< double > diff( 3 );
  for ( unsigned int i=0; i< m_SamplePoints.size(); i++)
    {
    diff[0] = m_SamplePoints[i][0] - m_Center[0];
    diff[1] = m_SamplePoints[i][1] - m_Center[1];
    diff[2] = m_SamplePoints[i][2] - m_Center[2];

    for ( unsigned int j=0; j<3; j++)
      {
      for ( unsigned int k=0; k<3; k++)
        {
        covar( j, k ) += diff[j] * diff[k];
        }
      }
    }

  covar /= m_SamplePoints.size() - 1;

  // Get the principle eigenvector
  vnl_symmetric_eigensystem< double > eigenSystem( covar );
  m_PrincipleAxis = eigenSystem.get_eigenvector( 2 );
  std::cout<< "Eigenvector: \n" << m_PrincipleAxis << std::endl;

  // Project points on to the eigenvector axis
  std::vector< double > projection;
  for ( unsigned int i=0; i< m_SamplePoints.size(); i++)
    {
    vnl_vector< double > p( 3 );
    p[0] = m_SamplePoints[i][0];
    p[1] = m_SamplePoints[i][1];
    p[2] = m_SamplePoints[i][2];

    //std::cout<< "Point: " << p << std::endl;
    projection.push_back( dot_product( p, m_PrincipleAxis ) );
    //std::cout<< "Position: " << dot_product( p, eigenVector ) << std::endl;
    }

  // Sort the points according to the projection
  for ( unsigned int i=0; i< m_SamplePoints.size()-1; i++ )
    {
    for ( unsigned int j=0; j< m_SamplePoints.size()-1; j++ )
      {
      if ( projection[j] > projection[j+1] )
        {
        double t = projection[j+1];
        projection[j+1] = projection[j];
        projection[j]   = t;

        PointType tp;
        tp = m_SamplePoints[j+1];
        m_SamplePoints[j+1] = m_SamplePoints[j];
        m_SamplePoints[j] = tp;
        }
      }
    }

  m_SortedPoints.clear();
  // Put the largest Y index first, which corresponds to the smallest
  // Z index in model
  // Assumption: The needle holder will always pointing downwards
  if ( m_SamplePoints.front()[1] < m_SamplePoints.back()[1])
    {
    while ( ! m_SamplePoints.empty())
      {
      m_SortedPoints.push_back( m_SamplePoints.back() );
      m_SamplePoints.pop_back();
      }
    }
  else
    {
    m_SortedPoints = m_SamplePoints;
    }
  
  return true;
}

}
