/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkEllipseEstimatorTest.cxx
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
#include <vector>

int igstkEllipseEstimatorTest(int argc, char * argv [])
{
	
  typedef std::vector<double>			PointType;
	typedef std::vector<double>			ParametersType;

  typedef std::vector<PointType> VectorOfPointsType;

  igstk::EllipseEstimator estimator;
  
  PointType point0(3);
  point0[0] = 9.2;
  point0[1] = 11.8;

  PointType point1(3);
  point1[0] = 6.0;
  point1[1] = 9.52;

  PointType point2(3);
  point2[0] = 4.0;
  point2[1] = 6.0;

  PointType point3(3);
  point3[0] = 8.0;
  point3[1] = 2.8;
 
  PointType point4(3);
  point4[0] = 13.0;
  point4[1] = 7.85;

  VectorOfPointsType ellipseInputPoints(5);

	ellipseInputPoints.clear();
  ellipseInputPoints.push_back(point0);
  ellipseInputPoints.push_back(point1);
  ellipseInputPoints.push_back(point2);
  ellipseInputPoints.push_back(point3);
  ellipseInputPoints.push_back(point4);


  estimator.SetInput(ellipseInputPoints);
	
	//print the input points
	/*
  for(int i=0; i < ellipseInputPoints.size(); i++){
	  PointType p = estimator.GetInputPoint(i);
	  std::cout<<"\n Point "<<i<<": x= "<< p[0] << "   y= " << p[1];
  }
	*/
	ParametersType ellipseParameters  = estimator.EstimateParameters();

	//check if ellipse parameters are the expected ones
	
	if ( estimator.GetEllipseParameter(0) != 1.0					 ) { std::cout <<  "\n param 0 not corect";return EXIT_FAILURE; };
	if ( estimator.GetEllipseParameter(1) != -9.6581180183360982e-001) { std::cout <<  "\n param 1 not corect";return EXIT_FAILURE; };
	if ( estimator.GetEllipseParameter(2) != 9.58413007190726750e-001) { std::cout <<  "\n param 2 not corect";return EXIT_FAILURE; };
	if ( estimator.GetEllipseParameter(3) != -1.0184816645592235e+001) { std::cout <<  "\n param 3 not corect";return EXIT_FAILURE; };
	if ( estimator.GetEllipseParameter(4) != -5.6821493693518850e+000) { std::cout <<  "\n param 4 not corect";return EXIT_FAILURE; };
	if ( estimator.GetEllipseParameter(5) != 4.75087777836207150e+001) { std::cout <<  "\n param 5 not corect";return EXIT_FAILURE; };
	
	VectorOfPointsType solutions = estimator.GetPoints(6.4);

	PointType sol1 = solutions[0];
	PointType sol2 = solutions[1];


	if ( sol1[1] != 2.4463225515042906 ){ std::cout <<  "\n sol 1 not corect";return EXIT_FAILURE; };
	if ( sol2[1] != 9.931790863150308 ){ std::cout <<  "\n sol 2 not corect";return EXIT_FAILURE; };
	

  //print the ellipse parameters
	/*
	for ( int j=0; j < ellipseParameters.size(); j++)
	{
			printf("\n Param %d : %e", j,estimator.GetEllipseParameter(j));
	}
	*/
	std::cout << "\n Success Test" ;
	return EXIT_SUCCESS;

}

