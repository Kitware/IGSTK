/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkEllipseEstimator.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstk_EllipseEstimator_h__
#define __igstk_EllipseEstimator_h__

#include <vector>

namespace igstk
{
	/** \class EllipseEstimator
	    \brief This class estimates an ellipse
      \warning
	    \sa everyone needs to look at doxygen tags
	*/

class EllipseEstimator
{


public:

  /** point of the ellipse  */
  typedef std::vector<double> PointType;

  /** vector of points from the ellipse  */
  typedef std::vector<PointType> VectorOfPointsType;

  /** vector of parametres of the ellipse  */
  typedef std::vector<double> ParametersType;

  
  /** default constructor  */
  EllipseEstimator( void );

  /** default destructor  */
  virtual ~EllipseEstimator( void );

  /** obtainns a point  */
  EllipseEstimator::PointType GetInputPoint(int i);

  /** SetInput: This method inputs the points that are necesary 
  *   to estimate the ellipse
  */
  void SetInput( EllipseEstimator::VectorOfPointsType  inputPoints);

  /** Get the parameters that uniquely define the ellipse
   * There are 6 parameters: A, B, C, D, E, F
   * that corespond to the quadratic equation
   * AX^2 + BXY + CY^2 + DX + EY + F = 0
  */
  EllipseEstimator::ParametersType  EstimateParameters( );

	/** obtainns one parameter of the ellipse  
	*	0 is for A, 1 for B, ...., 5 for F
	*/
  const double GetEllipseParameter(int i);


  void PrintParameters();
  
  
private:
  /** points that are used to estimate the ellipse  */
  VectorOfPointsType inputPoints;

  /** parameters of the ellipse  */
  ParametersType m_EllipseParameters;

  /** internaly computes the parameters of the ellipse */
	int ToConic(EllipseEstimator::VectorOfPointsType inputPoints, 
		double *a, double *b, double *c, double *d, double *e, double *f);

};


}  // end namespace ISIS

#endif



