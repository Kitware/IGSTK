/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmark3DRegistrationErrorEstimator.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more DEBUGrmation.
=========================================================================*/
#ifndef __igstkLandmark3DRegistrationErrorEstimator_cxx
#define __igstkLandmark3DRegistrationErrorEstimator_cxx

#if defined(_MSC_VER)
#pragma warning( disable : 4786 )
#endif

#include "igstkLandmark3DRegistrationErrorEstimator.h"

#include "itkMatrix.h"
#include "itkSymmetricEigenAnalysis.h"
#include "vnl/vnl_math.h"

namespace igstk
{ 
/** Constructor */
Landmark3DRegistrationErrorEstimator::
  Landmark3DRegistrationErrorEstimator() :  m_StateMachine( this )
{

} 

/** Destructor */
Landmark3DRegistrationErrorEstimator::~Landmark3DRegistrationErrorEstimator()  
{

}

/** Compute and set landmark principal axes */
void Landmark3DRegistrationErrorEstimator::ComputeLandmarkPrincipalAxes()
{
   igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator::"
                 "ComputeLandmarkPrincipalAxes called...\n");

   typedef itk::Matrix<double,3,3>                 MatrixType;
   typedef itk::Matrix<double,4,4>                 AugmentedMatrixType;
   typedef itk::Vector<double,3>                   VectorType;
   typedef itk::Vector<double,4>                   AugmentedVectorType;

   MatrixType                                      covarianceMatrix;
   
   PointsContainerConstIterator                    pointItr;
   
   VectorType                                      landmarkVector;
   VectorType                                      landmarkCentered;
   
   landmarkVector.Fill(0.0);
   
   pointItr  = m_ImageLandmarks.begin();
   while( pointItr != m_ImageLandmarks.end() )
     {
     landmarkVector[0] += (*pointItr)[0] ;
     landmarkVector[1] += (*pointItr)[1] ;
     landmarkVector[2] += (*pointItr)[2] ;
     ++pointItr;
     }

   pointItr  = m_ImageLandmarks.begin();
   while( pointItr != m_ImageLandmarks.end() )
     {
     for(unsigned int i=0; i<3; i++)
       {     
       landmarkCentered[i]  = (*pointItr)[i]  - this->m_LandmarkCentroid[i];
       }

     for(unsigned int i=0; i<3; i++)
       {
       for(unsigned int j=0; j<3; j++)
         {
         covarianceMatrix[i][j] += landmarkCentered[i] * landmarkCentered[j];
         }
       }
     ++pointItr;
     }

   // Expand the matrix 
   AugmentedMatrixType                                      augmentedCovarianceMatrix;
        
   augmentedCovarianceMatrix[0][0] =  covarianceMatrix[0][0] +covarianceMatrix[1][1] +
                                                             covarianceMatrix[2][2];
   augmentedCovarianceMatrix[1][1] =  covarianceMatrix[0][0] -covarianceMatrix[1][1] -
                                                              covarianceMatrix[2][2];
   augmentedCovarianceMatrix[2][2] = -covarianceMatrix[0][0] +covarianceMatrix[1][1] -
                                                               covarianceMatrix[2][2];
   augmentedCovarianceMatrix[3][3] = -covarianceMatrix[0][0] -covarianceMatrix[1][1] +
                                                                covarianceMatrix[2][2];
        
   // off-diagonal elements
   augmentedCovarianceMatrix[0][1] = augmentedCovarianceMatrix[1][0] = 
                                     covarianceMatrix[1][2] -covarianceMatrix[2][1];
   augmentedCovarianceMatrix[0][2] = augmentedCovarianceMatrix[2][0] = 
                                     covarianceMatrix[2][0] -covarianceMatrix[0][2];
   augmentedCovarianceMatrix[0][3] = augmentedCovarianceMatrix[3][0] = 
                                     covarianceMatrix[0][1] -covarianceMatrix[1][0];

   augmentedCovarianceMatrix[1][2] = augmentedCovarianceMatrix[2][1] = 
                                     covarianceMatrix[0][1] +covarianceMatrix[1][0];
   augmentedCovarianceMatrix[1][3] = augmentedCovarianceMatrix[3][1] = 
                                     covarianceMatrix[2][0] +covarianceMatrix[0][2];
   augmentedCovarianceMatrix[2][3] = augmentedCovarianceMatrix[3][2] = 
                                     covarianceMatrix[1][2] +covarianceMatrix[2][1];
   
   AugmentedMatrixType                    eigenVectors;
   AugmentedVectorType                    eigenValues;

   typedef itk::SymmetricEigenAnalysis< 
           AugmentedMatrixType,  
           AugmentedVectorType,
           AugmentedMatrixType > SymmetricEigenAnalysisType;

   SymmetricEigenAnalysisType symmetricEigenSystem(4);
        
   symmetricEigenSystem.ComputeEigenValuesAndVectors
               ( augmentedCovarianceMatrix, eigenValues, eigenVectors );

   this->m_LandmarkPrincipalAxes.Set( eigenVectors[3][1],
                    eigenVectors[3][2],
                    eigenVectors[3][3],
                    eigenVectors[3][0]  );
}

/** Compute landmark centroid */
void Landmark3DRegistrationErrorEstimator::ComputeLandmarksCentroid()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator::"
                 "ComputeLandmarksCentroid called...\n");

  typedef itk::Vector< double, 3 >                      VectorType;

  PointsContainerConstIterator                          pointItr;
  VectorType                                            landmarkVector;

  // initialize to zero
  landmarkVector[0] = 0.0;
  landmarkVector[1] = 0.0;
  landmarkVector[2] = 0.0;

  pointItr  = m_ImageLandmarks.begin();
  while( pointItr != m_ImageLandmarks.end() )
    {
    landmarkVector[0] += (*pointItr)[0] ;
    landmarkVector[1] += (*pointItr)[1] ;
    landmarkVector[2] += (*pointItr)[2] ;
    ++pointItr;
    }

  for(unsigned int ic=0; ic<3; ic++)
    {
    this->m_LandmarkCentroid[ic]  = landmarkVector[ic]  / this->m_ImageLandmarks.size();
    } 
}

/** Compute RMS distance of landmarks from the principal axes */
void Landmark3DRegistrationErrorEstimator::
     ComputeRMSDistanceLandmarksFromPrincipalAxes()
{
  typedef itk::Vector< double, 3 >                VectorType;
  typedef itk::Matrix< double, 3, 3 >             MatrixType; 
  typedef double                                  DistanceType; 
  
  PointsContainerConstIterator                    pointItr;
  VectorType                                      landmarkVector;
  MatrixType                                      principalAxesMatrix;
  VectorType                                      principalAxes1;
  VectorType                                      principalAxes2;
  VectorType                                      principalAxes3;
    
  // Convert versor to a matrix
  principalAxesMatrix = this->m_LandmarkPrincipalAxes.GetMatrix();
 
  for ( unsigned int i=0 ; i < 3 ; i ++ ) 
    {
    principalAxes1[i] = principalAxesMatrix[0][i]; 
    principalAxes2[i] = principalAxesMatrix[1][i]; 
    principalAxes3[i] = principalAxesMatrix[2][i]; 
    }
  
  // Normalize the principal axes 
  VectorType        normalizedPrincipalAxes1;
  VectorType        normalizedPrincipalAxes2;
  VectorType        normalizedPrincipalAxes3;

  normalizedPrincipalAxes1 = principalAxes1;
  normalizedPrincipalAxes2 = principalAxes1;
  normalizedPrincipalAxes3 = principalAxes1;

  normalizedPrincipalAxes1.Normalize();
  normalizedPrincipalAxes2.Normalize();
  normalizedPrincipalAxes3.Normalize();

  landmarkVector.Fill(0.0);
 
  // Vector from landmark point to landmark centroid  
  VectorType                                       differenceVector;
 
  //  Distance from the landmark point to the principal axes
  DistanceType    distanceFromLandmarkPointToPrinciaplAxes1 = 0.0;
  DistanceType    distanceFromLandmarkPointToPrinciaplAxes2 = 0.0;
  DistanceType    distanceFromLandmarkPointToPrinciaplAxes3 = 0.0;
  
  pointItr  = m_ImageLandmarks.begin();
  while( pointItr != m_ImageLandmarks.end() )
   {
     
     differenceVector[0] = (*pointItr)[0] - this->m_LandmarkCentroid[0];
     differenceVector[1] = (*pointItr)[1] - this->m_LandmarkCentroid[1];
     differenceVector[2] = (*pointItr)[2] - this->m_LandmarkCentroid[2];
   
     distanceFromLandmarkPointToPrinciaplAxes1 += 
        vnl_math_sqr( CrossProduct ( normalizedPrincipalAxes1, differenceVector ).GetNorm() ) ; 
     distanceFromLandmarkPointToPrinciaplAxes2 += 
        vnl_math_sqr( CrossProduct ( normalizedPrincipalAxes2, differenceVector ).GetNorm() ); 
     distanceFromLandmarkPointToPrinciaplAxes3 += 
        vnl_math_sqr( CrossProduct ( normalizedPrincipalAxes3, differenceVector ).GetNorm() ); 
     ++pointItr;
   }

  this->m_RMSDistanceFromLandmarkToPrincipalAxes[0] = 
        sqrt( distanceFromLandmarkPointToPrinciaplAxes1 / m_ImageLandmarks.size() );
  this->m_RMSDistanceFromLandmarkToPrincipalAxes[1] = 
        sqrt( distanceFromLandmarkPointToPrinciaplAxes2 / m_ImageLandmarks.size() );
  this->m_RMSDistanceFromLandmarkToPrincipalAxes[2] = 
        sqrt( distanceFromLandmarkPointToPrinciaplAxes3 / m_ImageLandmarks.size() );
  
}

/* The "ComputeErrorParameters" method calculates all the rigid body
  registration error parameters */
void 
Landmark3DRegistrationErrorEstimator:: ComputeErrorParameters()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator::"
                 "ComputeErrorParameters called...\n");
  this->ComputeLandmarksCentroid();
  this->ComputeLandmarkPrincipalAxes();
  this->ComputeRMSDistanceLandmarksFromPrincipalAxes();
}

/* The "EstimateTargetRegistrationError" method estimates target signal ( sample point )
 * registration error */
Landmark3DRegistrationErrorEstimator::ErrorType 
Landmark3DRegistrationErrorEstimator
:: EstimateTargetRegistrationError( const TargetPointType & targetPoint )
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator::"
                 "EstimateTargetRegistrationError called...\n");
  
  typedef itk::Vector< double, 3 >                VectorType;
  typedef itk::Matrix< double, 3, 3 >             MatrixType; 
  typedef double                                  DistanceType; 
  
  MatrixType                                      principalAxesMatrix;
  VectorType                                      principalAxes1;
  VectorType                                      principalAxes2;
  VectorType                                      principalAxes3;
  
  //  Distance vector from target point to the principal axes
  VectorType                               distanceFromTargetPointToPrincipalAxes;

  // Vector from the target point to the landmark centroid
  VectorType                               differenceVector;

  differenceVector = targetPoint - this->m_LandmarkCentroid;

  // Convert versor to a matrix
  principalAxesMatrix = this->m_LandmarkPrincipalAxes.GetMatrix();
 
  for ( unsigned int i=0 ; i < 3 ; i ++ ) 
    {
    principalAxes1[i] = principalAxesMatrix[0][i]; 
    principalAxes2[i] = principalAxesMatrix[1][i]; 
    principalAxes3[i] = principalAxesMatrix[2][i]; 
    }
  
  // Normalize the principal axes 
  VectorType   normalizedPrincipalAxes1;
  VectorType   normalizedPrincipalAxes2;
  VectorType   normalizedPrincipalAxes3;

  normalizedPrincipalAxes1 = principalAxes1;
  normalizedPrincipalAxes2 = principalAxes1;
  normalizedPrincipalAxes3 = principalAxes1;

  normalizedPrincipalAxes1.Normalize();
  normalizedPrincipalAxes2.Normalize();
  normalizedPrincipalAxes3.Normalize();


  // calculate the distance from the target point to the principal axes
  distanceFromTargetPointToPrincipalAxes[0] = 
               CrossProduct ( normalizedPrincipalAxes1, differenceVector ).GetNorm() ; 
  distanceFromTargetPointToPrincipalAxes[1] = 
               CrossProduct ( normalizedPrincipalAxes2, differenceVector ).GetNorm() ; 
  distanceFromTargetPointToPrincipalAxes[2] = 
               CrossProduct ( normalizedPrincipalAxes3, differenceVector ).GetNorm() ; 

  // target error parameter 
  ErrorType                      targetRegistrationError = 0.0;
 
  for( unsigned int i=0; i < 3 ; i++ ) 
    {
    targetRegistrationError += 
           vnl_math_sqr( distanceFromTargetPointToPrincipalAxes[i] ) / 
           vnl_math_sqr( this->m_RMSDistanceFromLandmarkToPrincipalAxes[i] ); 
    }

  targetRegistrationError *= (1.0/3.0);
  targetRegistrationError += 1.0;
  targetRegistrationError *= ( vnl_math_sqr( this->m_LandmarkRegistrationError ) ) / 
                              ( this->m_ImageLandmarks.size() - 2 );

  return targetRegistrationError;
}


/** Print Self function */
void 
Landmark3DRegistrationErrorEstimator::PrintSelf( std::ostream& os,
                                             itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Image Landmarks: " << std::endl;
  PointsContainerConstIterator mitr = m_ImageLandmarks.begin();
  while( mitr != m_ImageLandmarks.end() )
    {
    os << indent << *mitr << std::endl;
    ++mitr;
    }

  os << indent << "Landmark Principal Axes: " << std::endl;
  os << indent << this->m_LandmarkPrincipalAxes << std::endl;
}

} // end namespace igstk
#endif


