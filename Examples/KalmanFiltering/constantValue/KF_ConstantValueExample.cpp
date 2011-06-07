/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    KF_ConstantValueExample.cpp
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include "cv.h"
#include "cvaux.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <fstream>

using namespace std;

int main( int argc, char *argv[] )
{
  if( argc!=9 )
  {
    std::cerr<<"Usage: "<<argv[0];
    std::cerr<<" result_filename\tmeasurement_filename\tvalueToBeEstimated\tinitial_value\tinitial_uncertainty\tprocess_model_noise_cov\tmeasurement_model_noise_cov\tmeasurement_generation_noise_cov\n";
    return EXIT_FAILURE;
  }//end check arguments


  //constant value to be estimated
  double actual_value = atof(argv[3]);

  //initial value
  double x_init = atof(argv[4]);

  //initial uncertainty
  double sigma_init = atof(argv[5]);

  //process model noise covariance
  double process_sigma = atof(argv[6]);

  //measurement model noise covariance
  double measurement_sigma = atof(argv[7]);

  //measurement generation noise covariance
  double measurement_gen_sigma = atof(argv[8]);

  //cv number generator
  CvRandState rng;
    cvRandInit( &rng, 0, measurement_gen_sigma, 0, CV_RAND_NORMAL);//normal distribution, std=user_defined, mean=0, firstvalue=0

  //Kalman Filter structure
  CvKalman* kalman =cvCreateKalman(1,1,0);//dimensionality of: state, measurement, control

  //precompute noisy measurements
  CvMat* z = cvCreateMat( 100, 1, CV_32FC1 );//100x1 column vector of floating point numebers
  cvRand(&rng, z);
  cvAddS(z,cvRealScalar(actual_value),z,NULL);

  //write measurements to file
  ofstream f(argv[2]);
  for(int i=0; i<100; ++i)
    f<<cvmGet(z,i,0)<<std::endl;
  f.close();


  //state is monodimensional
  CvMat* x_k = cvCreateMat( 1, 1, CV_32FC1 );

  //measurement vector is monodimensional
  CvMat* z_k = cvCreateMat( 1, 1, CV_32FC1 );

  //process noise
  CvMat* w_k = cvCreateMat( 1, 1, CV_32FC1 );

  // Transition matrix F describes model parameters at and k and k+1
  const float F[] = { 1 };
  memcpy( kalman->transition_matrix->data.fl, F, sizeof(F));

  // Initialize other Kalman parameters
  cvSetIdentity( kalman->measurement_matrix, cvRealScalar(1) );
  cvSetIdentity( kalman->process_noise_cov, cvRealScalar(process_sigma) );
  cvSetIdentity( kalman->measurement_noise_cov, cvRealScalar(measurement_sigma) );
  cvSetIdentity( kalman->error_cov_post, cvRealScalar(sigma_init) );

  //set initial state
  cvmSet(kalman->state_post,0,0, x_init);

  //matrix for saving results
  CvMat* result = cvCreateMat( 100, 1, CV_32FC1 );

  for(int k =0; k<100; ++k)
  {
    //prediction
    const CvMat* y_k = cvKalmanPredict( kalman, NULL );

    //read measurement
    cvmSet(z_k,0,0, cvmGet(z,k,0));

    //correction
    cvKalmanCorrect(kalman,z_k);
    cvmSet(result,k,0,cvmGet(y_k,0,0));
  }//end filter cycle

  //write results to file
  ofstream resultFile(argv[1]);
  for(int i=0; i<100; ++i)
    resultFile<<cvmGet(result,i,0)<<std::endl;

  resultFile.close();

  //release memory
  cvReleaseMat(&z);
  cvReleaseMat(&x_k);
  cvReleaseMat(&z_k);
  cvReleaseMat(&result);

}//end main


