/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkEllipseEstimator.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm 
for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkEllipseEstimator.h"

#include <iostream>
#include <math.h>

#define cross(a, b, ab) ab[0] = a[1]*b[2] - a[2]*b[1]; \
			ab[1] = a[2]*b[0] - a[0]*b[2]; \
			ab[2] = a[0]*b[1] - a[1]*b[0];

#define abs(x) ((x > 0.0) ? x : (-x))

namespace igstk
{


EllipseEstimator
::EllipseEstimator()
{
	
  
}



EllipseEstimator
::~EllipseEstimator()
{
  
}

EllipseEstimator::PointType
EllipseEstimator::GetInputPoint(int i)
{
	EllipseEstimator::PointType point = EllipseEstimator::inputPoints[i];
  return point;
}

/**
*/
void
EllipseEstimator::SetInput( EllipseEstimator::VectorOfPointsType ellipseInputPoints)
{
  EllipseEstimator::inputPoints = ellipseInputPoints;
}


/**
*/

EllipseEstimator::ParametersType 
EllipseEstimator::EstimateParameters( )
{
	
  double a, b, c, d, e, f;
  
  EllipseEstimator::ParametersType ellipseParameters;

  if ( ToConic(inputPoints, &a, &b, &c, &d, &e, &f) )
  {
	ellipseParameters.clear();
    ellipseParameters.push_back(a);
    ellipseParameters.push_back(b);
    ellipseParameters.push_back(c);
    ellipseParameters.push_back(d);
    ellipseParameters.push_back(e);
    ellipseParameters.push_back(f);
		
    
	this->m_EllipseParameters = ellipseParameters;
    return ellipseParameters;

  } else {
			this->m_EllipseParameters.clear();
			return this->m_EllipseParameters;
	}

}

/** returns one parameter of the ellipse
* 
*/
const double 
EllipseEstimator::GetEllipseParameter(int i)
{
	return m_EllipseParameters[i];
}


int 
EllipseEstimator::ToConic(EllipseEstimator::VectorOfPointsType inputPoints, 
													double *a, double *b, double *c, double *d, double *e, double *f)
{
	EllipseEstimator::PointType point0 = inputPoints[0];
	EllipseEstimator::PointType point1 = inputPoints[1];
	EllipseEstimator::PointType point2 = inputPoints[2];
	EllipseEstimator::PointType point3 = inputPoints[3];
	EllipseEstimator::PointType point4 = inputPoints[4];


  
	PointType p0 = point0; p0[2] = 1;
	PointType p1 = point1; p1[2] = 1;
	PointType p2 = point2; p2[2] = 1;
	PointType p3 = point3; p3[2] = 1;
	PointType p4 = point4; p4[2] = 1;
	
	double L0[3], L1[3], L2[3], L3[3];
  double A, B, C, Q[3];
  double a1, a2, b1, b2, c1, c2;
  double x0, x4, y0, y4, w0, w4;
  double aa, bb, cc, dd, ee, ff;
  double y4w0, w4y0, w4w0, y4y0, x4w0, w4x0, x4x0, y4x0, x4y0;
  double a1a2, a1b2, a1c2, b1a2, b1b2, b1c2, c1a2, c1b2, c1c2;
    
  cross(p0, p1, L0)
  cross(p1, p2, L1)
  cross(p2, p3, L2)
  cross(p3, p4, L3)
  cross(L0, L3, Q)
    A = Q[0]; B = Q[1]; C = Q[2];
    a1 = L1[0]; b1 = L1[1]; c1 = L1[2];
    a2 = L2[0]; b2 = L2[1]; c2 = L2[2];
    x0 = p0[0]; y0 = p0[1]; w0 = p0[2];
    x4 = p4[0]; y4 = p4[1]; w4 = p4[2];
    
    y4w0 = y4*w0;
    w4y0 = w4*y0;
    w4w0 = w4*w0;
    y4y0 = y4*y0;
    x4w0 = x4*w0;
    w4x0 = w4*x0;
    x4x0 = x4*x0;
    y4x0 = y4*x0;
    x4y0 = x4*y0;
    a1a2 = a1*a2;
    a1b2 = a1*b2;
    a1c2 = a1*c2;
    b1a2 = b1*a2;
    b1b2 = b1*b2;
    b1c2 = b1*c2;
    c1a2 = c1*a2;
    c1b2 = c1*b2;
    c1c2 = c1*c2;

    aa = -A*a1a2*y4w0
	 +A*a1a2*w4y0 
	 -B*b1a2*y4w0
	 -B*c1a2*w4w0
	 +B*a1b2*w4y0
	 +B*a1c2*w4w0
	 +C*b1a2*y4y0
	 +C*c1a2*w4y0
	 -C*a1b2*y4y0
	 -C*a1c2*y4w0;

    cc =  A*c1b2*w4w0
         +A*a1b2*x4w0
	 -A*b1c2*w4w0
	 -A*b1a2*w4x0
	 +B*b1b2*x4w0
	 -B*b1b2*w4x0
	 +C*b1c2*x4w0
	 +C*b1a2*x4x0
	 -C*c1b2*w4x0
	 -C*a1b2*x4x0;

    ff =  A*c1a2*y4x0
	 +A*c1b2*y4y0
	 -A*a1c2*x4y0
	 -A*b1c2*y4y0
	 -B*c1a2*x4x0
	 -B*c1b2*x4y0
	 +B*a1c2*x4x0
	 +B*b1c2*y4x0
	 -C*c1c2*x4y0
	 +C*c1c2*y4x0;

    bb =  A*c1a2*w4w0
	 +A*a1a2*x4w0
	 -A*a1b2*y4w0
	 -A*a1c2*w4w0
	 -A*a1a2*w4x0
	 +A*b1a2*w4y0
	 +B*b1a2*x4w0
	 -B*b1b2*y4w0
	 -B*c1b2*w4w0
	 -B*a1b2*w4x0
	 +B*b1b2*w4y0
	 +B*b1c2*w4w0
	 -C*b1c2*y4w0
	 -C*b1a2*x4y0
	 -C*b1a2*y4x0
	 -C*c1a2*w4x0
	 +C*c1b2*w4y0
	 +C*a1b2*x4y0
	 +C*a1b2*y4x0
	 +C*a1c2*x4w0;

    dd = -A*c1a2*y4w0
	 +A*a1a2*y4x0
	 +A*a1b2*y4y0
	 +A*a1c2*w4y0
	 -A*a1a2*x4y0
	 -A*b1a2*y4y0
	 +B*b1a2*y4x0
	 +B*c1a2*w4x0
	 +B*c1a2*x4w0
	 +B*c1b2*w4y0
	 -B*a1b2*x4y0
	 -B*a1c2*w4x0
	 -B*a1c2*x4w0
	 -B*b1c2*y4w0
	 +C*b1c2*y4y0
	 +C*c1c2*w4y0
	 -C*c1a2*x4y0
	 -C*c1b2*y4y0
	 -C*c1c2*y4w0
	 +C*a1c2*y4x0;

    ee = -A*c1a2*w4x0
	 -A*c1b2*y4w0
	 -A*c1b2*w4y0
	 -A*a1b2*x4y0
	 +A*a1c2*x4w0
	 +A*b1c2*y4w0
	 +A*b1c2*w4y0
	 +A*b1a2*y4x0
	 -B*b1a2*x4x0
	 -B*b1b2*x4y0
	 +B*c1b2*x4w0
	 +B*a1b2*x4x0
	 +B*b1b2*y4x0
	 -B*b1c2*w4x0
	 -C*b1c2*x4y0
	 +C*c1c2*x4w0
	 +C*c1a2*x4x0
	 +C*c1b2*y4x0
	 -C*c1c2*w4x0
	 -C*a1c2*x4x0;

    if (aa != 0.0) {
	bb /= aa; cc /= aa; dd /= aa; ee /= aa; ff /= aa; aa = 1.0;
    } else if (bb != 0.0) {
	cc /= bb; dd /= bb; ee /= bb; ff /= bb; bb = 1.0;
    } else if (cc != 0.0) {
	dd /= cc; ee /= cc; ff /= cc; cc = 1.0;
    } else if (dd != 0.0) {
	ee /= dd; ff /= dd; dd = 1.0;
    } else if (ee != 0.0) {
	ff /= ee; ee = 1.0;
    } else {
	return 0;
    }
    *a = aa;
    *b = bb;
    *c = cc;
    *d = dd;
    *e = ee;
    *f = ff;
    return 1;
}



  /**
  */
bool
EllipseEstimator::IsEllipse( )
{
  double delta = ( GetEllipseParameter(1) * GetEllipseParameter(1) ) - 4 * GetEllipseParameter(0) * GetEllipseParameter(2);
	
  if ( delta < 0 ) return true;
  else return false;
}

  /**given x coordinate computes the coresponding two points
  *  if no points exist, the returned points have 0 sa coordinate values
  */
EllipseEstimator::VectorOfPointsType 
EllipseEstimator::GetPoints ( double x )
{
  EllipseEstimator::VectorOfPointsType	points(2);

  if ( IsEllipse() )
  {
    double A = GetEllipseParameter(2);
	double B = GetEllipseParameter(1) * x + GetEllipseParameter(4);
	double C = GetEllipseParameter(0) * x * x + GetEllipseParameter(3) * x + GetEllipseParameter(5);

	double delta = B *B - 4 *A * C;

	if ( ( delta >= 0) && ( A != 0 ) )
	{
	  PointType p1(2);
	  p1[0] = x;
	  p1[1] = ( -B - sqrt(delta) ) / (2 * A);

	  PointType p2(2);
	  p2[0] = x;
	  p2[1] = ( -B + sqrt(delta) ) / (2 * A);

	  points[0] = p1;
	  points[1] = p2;
	}
	else {
	  PointType p1;
	  p1[0] =  p1[1] = 0;

	  PointType p2;
	  p2[0] = p2[1] = 0;

	  points[0] = p1;
	  points[1] = p2; 
	}
  }
  return points;
}
/**
*/
void
EllipseEstimator::PrintParameters()
{
	
	for ( unsigned int i=0; i<m_EllipseParameters.size(); i++)
		{
		std::cout<<"\n"<< m_EllipseParameters[i];
		}

}




}


