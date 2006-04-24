/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    DOCR_Registration.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "DOCR_Registration.h"

DOCR_Registration::DOCR_Registration( USVolumeType *volume, 
                                      USVolumeType::SpacingType spacing ) 
{
  m_USLoadedVolume = volume;
  m_CT_Spacing = spacing;
  for (int i = 0; i < NUM_FIDUCIALS; i++) m_fiducials[i][0][0] = 0;
  m_numFiducials = 0;
}

DOCR_Registration::~DOCR_Registration()
{
}

void DOCR_Registration::compute() 
{
  std::vector< vnl_vector<double> > segmentedFiducials(NUM_FIDUCIALS);
  for(int i = 0; i < NUM_FIDUCIALS; i++) segmentedFiducials[i].set_size(3);
  short found = -1;

  m_volumeSize = m_USLoadedVolume->GetLargestPossibleRegion().GetSize();

  itk::ImageRegionConstIterator< USVolumeType >
    it1(m_USLoadedVolume, m_USLoadedVolume->GetLargestPossibleRegion() );
  it1.GoToBegin();

  std::cout << "----------------- Segmenting fiducials -------------------\n\n";

  for (int k = 0; k < m_volumeSize[2]; k++)              // z
  {
    for (int j = 0; j < m_volumeSize[1]; j++)            // y
    {
      for (int i = 0; i < m_volumeSize[0]; i++, ++it1)   // x
      {
        if (it1.Get() > 3500)
        {
          for (int l = 0; l < NUM_FIDUCIALS; l++)        // fiducial
          {
            if ((m_fiducials[l][0][0] != 0) && (found == -1))
            {
              for (int m = 1; m <= m_fiducials[l][0][0]; m++)    // voxel
              {
                if ((found == -1)&&
                    (m_fiducials[l][m][0] >= (((i - 1)<0) ? 0:(i-1))) &&
                    (m_fiducials[l][m][0] <= (((i + 1)>(m_volumeSize[0]-1)) ? 
                    (m_volumeSize[0] - 1) : (i + 1))) &&
                    (m_fiducials[l][m][1] >= (((j - 1)<0) ? 0:(j-1))) &&
                    (m_fiducials[l][m][1] <= (((j + 1)>(m_volumeSize[1]-1)) ?
                    (m_volumeSize[1] - 1) : (j + 1))) &&
                    (m_fiducials[l][m][2] >= (((k - 1)<0) ? 0:(k-1))) &&
                    (m_fiducials[l][m][2] <= (((k + 1)>(m_volumeSize[2]-1)) ?
                    (m_volumeSize[2] - 1) : (k + 1))))
                {
                  found = l;
                  m_fiducials[l][m_fiducials[l][0][0] + 1][0] = i;
                  m_fiducials[l][m_fiducials[l][0][0] + 1][1] = j;
                  m_fiducials[l][m_fiducials[l][0][0] + 1][2] = k;
                }
              }
            }
          }
          if (found != -1)
          {
            m_fiducials[found][0][0]++;
            std::cout << "Pixel " << i << "/" << j << "/" << k <<
                         " add: Fiducial " << found << std::endl;
          }
          else
          {
            m_fiducials[m_numFiducials][0][0] = 1;
            m_fiducials[m_numFiducials][1][0] = i;
            m_fiducials[m_numFiducials][1][1] = j;
            m_fiducials[m_numFiducials][1][2] = k;
            std::cout << "Pixel " << i << "/" << j << "/" << k <<
                         " new: Fiducial " << m_numFiducials << std::endl;
            m_numFiducials++;
          }
        }
        found = -1;
      }
    }
  }

  std::cout << std::endl;

  for (int i = 0; i < m_numFiducials; i++)
  {
    double avg[3] = {0, 0, 0};
    for (int j = 1; j <= m_fiducials[i][0][0]; j++)
    {
      avg[0] += m_fiducials[i][j][0];
      avg[1] += m_fiducials[i][j][1];
      avg[2] += m_fiducials[i][j][2];
    }
    avg[0] /= m_fiducials[i][0][0];
    avg[1] /= m_fiducials[i][0][0];
    avg[2] /= m_fiducials[i][0][0];
    avg[0] *= m_CT_Spacing[0];
    avg[1] *= m_CT_Spacing[1];
    segmentedFiducials[i].copy_in(avg);
    std::cout << "Fiducial " << i << ": " << segmentedFiducials[i] << std::endl;
  }

  std::cout << std::endl <<
    "------------------- Doing PCA and sorting ----------------------" <<
    std::endl << std::endl;

  // unnecessary: calculates max distance between all fiducials
  //{
  //  float maxdist = 0;
  //  int maxp1 = -1, maxp2 = -1;
  //  for (int i = 0; i < m_numFiducials; i++)
  //  {
  //    for (int j = 0; (j < m_numFiducials) && (i != j); j++)
  //    {
  //      float dist = sqrtf(pow(segmentedFiducials[i][0]-
  //           segmentedFiducials[j][0], 2)+
  //           pow(segmentedFiducials[i][1]-
  //           segmentedFiducials[j][1], 2)+
  //           pow(segmentedFiducials[i][2]-
  //           segmentedFiducials[j][2], 2));
  //      if (dist > maxdist)
  //      {
  //        maxdist = dist;
  //        maxp1 = i;
  //        maxp2 = j;
  //      }
  //    }
  //  }
  //  std::cout << "Maximum distance between points " << maxp1 << " and " <<
  //      maxp2 << " : " << maxdist << std::endl;
  //}

  // unnecessary: old class version of PCA
  //Nutzung von PCA.h
  //PCA<double> pca(segmentedFiducials);
  //std::vector< vnl_vector<double> > m_transformedData(NUM_FIDUCIALS);
  //for(i = 0; i < NUM_FIDUCIALS; i++) m_transformedData[i].set_size(3);
  //m_transformedData = pca.getTransformedData();
  //std::cout << "V: " << pca.getEigenVectors() << std::endl;
  //std::cout << "cov: " << pca.getCovarianceMatrix() << std::endl;
  //std::cout << "mean: " << pca.getMeanPoint() << std::endl;
  //for (int h = 0; h < m_numFiducials; h++)
  //  std::cout << segmentedFiducials[h] << std::endl;
  // end Nutzung von PCA.h

  m_meanPoint.set_size(SPACE_DIMENSION);
  m_meanPoint = (double)(0.0);
  for(int i = 0; i < SPACE_DIMENSION; i++)
    for(int j = 0; j < m_numFiducials; j++)
      m_meanPoint[i] += segmentedFiducials[j][i];
  m_meanPoint /= m_numFiducials;

  for(int i = 0; i < m_numFiducials; i++)
    m_transformedData.push_back(segmentedFiducials[i] - m_meanPoint);

  m_covariance.set_size(SPACE_DIMENSION, SPACE_DIMENSION);
  m_covariance = (double)0.0;            
  for(int i = 0; i < m_numFiducials; i++)
  {     
    for(int j = 0; j < SPACE_DIMENSION; j++)
      for(int k = j; k < SPACE_DIMENSION; k++)
        m_covariance(j, k) += m_transformedData[i][j] * m_transformedData[i][k];
  }
  for(int j = 0; j < SPACE_DIMENSION; j++)
    for(int k = j + 1; k < SPACE_DIMENSION; k++)
      m_covariance(k, j) = m_covariance(j, k);

  vnl_symmetric_eigensystem< double > eigenSystem(m_covariance);

  m_eigenVectors = eigenSystem.V;
  for(int i = 0; i < SPACE_DIMENSION; i++)
    m_eigenValues.push_back(eigenSystem.D(i, i));

  for(int i = 0; i < m_numFiducials; i++) 
    m_transformedData[i].post_multiply(m_eigenVectors);

  //std::cout << "m_eigenVectors: " << eigenSystem.V << std::endl;
  //std::cout << "m_covariance: " << m_covariance << std::endl;

  m_inverseEigenVectors = vnl_matrix_inverse<double>(m_eigenVectors);

  // unnecessary: code to test inverse matrix
  //std::cout << "Vector: (1 2 3)" << std::endl;
  //vnl_vector<double> testPoint;
  //testPoint.set_size(3);
  //testPoint[0] = 1; testPoint[1] = 2; testPoint[2] = 3;
  //testPoint.post_multiply(m_eigenVectors);
  //std::cout << "Ergebnis: " << testPoint << std::endl;
  //testPoint.post_multiply(m_inverseEigenVectors);
  //std::cout << "Inverse-Ergebnis: " << testPoint << std::endl;
  //std::cout << "Inverse Matrix: " << m_inverseEigenVectors << std::endl;

  for (int i = 0; i < m_numFiducials; i++) 
    std::cout << "PCA Transformed Data " << i << ": " <<
        m_transformedData[i] << std::endl;

  for (int i = 0; i < m_numFiducials; i++)
        m_transformedDataZ[i] = m_transformedData[i][2];

  std::sort(m_transformedDataZ, m_transformedDataZ + m_numFiducials);
  //for (int i = 0; i < m_numFiducials; i++)
  //  std::cout << m_transformedDataZ[i] << " ";
  //std::cout << std::endl;

  for (int i = 0; i < m_numFiducials; i++)
  {
    for (int j = 0; j < m_numFiducials; j++)
    {
      if (m_transformedDataZ[i] == m_transformedData[j][2])
      {
        m_sortedFiducials[i] = j;
        m_transformedData[j][2] += 999;
      }
    }
  }
  for (int i = 0; i < m_numFiducials; i++)
    if (m_transformedData[i][2] > 500)
      m_transformedData[i][2] -= 999;

  std::cout << "Sorted Fiducials starting with fiducial on top: " << std::endl;
  for (int i = 0; i < m_numFiducials; i++)
     std::cout << m_sortedFiducials[i] << " ";
  std::cout << std::endl;

  std::cout << "xyz-distance(divided by 4) points 0-4-8: " << sqrt(pow(m_transformedData[m_sortedFiducials[0]][0]-m_transformedData[m_sortedFiducials[4]][0],2)+pow(m_transformedData[m_sortedFiducials[0]][1]-m_transformedData[m_sortedFiducials[4]][1],2)+pow(m_transformedData[m_sortedFiducials[0]][2]-m_transformedData[m_sortedFiducials[4]][2],2))/4 << " and " << sqrt(pow(m_transformedData[m_sortedFiducials[8]][0]-m_transformedData[m_sortedFiducials[4]][0],2)+pow(m_transformedData[m_sortedFiducials[8]][1]-m_transformedData[m_sortedFiducials[4]][1],2)+pow(m_transformedData[m_sortedFiducials[8]][2]-m_transformedData[m_sortedFiducials[4]][2],2))/4 << std::endl;
  std::cout << "                           xy-distance : " << sqrt(pow(m_transformedData[m_sortedFiducials[0]][0]-m_transformedData[m_sortedFiducials[4]][0],2)+pow(m_transformedData[m_sortedFiducials[0]][1]-m_transformedData[m_sortedFiducials[4]][1],2)) << " and " << sqrt(pow(m_transformedData[m_sortedFiducials[8]][0]-m_transformedData[m_sortedFiducials[4]][0],2)+pow(m_transformedData[m_sortedFiducials[8]][1]-m_transformedData[m_sortedFiducials[4]][1],2)) << ", all: " << sqrt(pow(m_transformedData[m_sortedFiducials[8]][0]-m_transformedData[m_sortedFiducials[0]][0],2)+pow(m_transformedData[m_sortedFiducials[8]][1]-m_transformedData[m_sortedFiducials[0]][1],2)) << std::endl;
  std::cout << "xyz-distance(divided by 4) points 1-5: "   << sqrt(pow(m_transformedData[m_sortedFiducials[1]][0]-m_transformedData[m_sortedFiducials[5]][0],2)+pow(m_transformedData[m_sortedFiducials[1]][1]-m_transformedData[m_sortedFiducials[5]][1],2)+pow(m_transformedData[m_sortedFiducials[1]][2]-m_transformedData[m_sortedFiducials[5]][2],2))/4 << " / xy-distance : " << sqrt(pow(m_transformedData[m_sortedFiducials[1]][0]-m_transformedData[m_sortedFiducials[5]][0],2)+pow(m_transformedData[m_sortedFiducials[1]][1]-m_transformedData[m_sortedFiducials[5]][1],2)) << std::endl;
  std::cout << "xyz-distance(divided by 4) points 2-6: "   << sqrt(pow(m_transformedData[m_sortedFiducials[2]][0]-m_transformedData[m_sortedFiducials[6]][0],2)+pow(m_transformedData[m_sortedFiducials[2]][1]-m_transformedData[m_sortedFiducials[6]][1],2)+pow(m_transformedData[m_sortedFiducials[2]][2]-m_transformedData[m_sortedFiducials[6]][2],2))/4 << " / xy-distance : " << sqrt(pow(m_transformedData[m_sortedFiducials[2]][0]-m_transformedData[m_sortedFiducials[6]][0],2)+pow(m_transformedData[m_sortedFiducials[2]][1]-m_transformedData[m_sortedFiducials[6]][1],2)) << std::endl;
  std::cout << "xyz-distance(divided by 4) points 3-7: "   << sqrt(pow(m_transformedData[m_sortedFiducials[3]][0]-m_transformedData[m_sortedFiducials[7]][0],2)+pow(m_transformedData[m_sortedFiducials[3]][1]-m_transformedData[m_sortedFiducials[7]][1],2)+pow(m_transformedData[m_sortedFiducials[3]][2]-m_transformedData[m_sortedFiducials[7]][2],2))/4 << " / xy-distance : " << sqrt(pow(m_transformedData[m_sortedFiducials[3]][0]-m_transformedData[m_sortedFiducials[7]][0],2)+pow(m_transformedData[m_sortedFiducials[3]][1]-m_transformedData[m_sortedFiducials[7]][1],2)) << std::endl;
  std::cout << std::endl <<
    "--------------------- Doing PtP registration --------------------------"
    << std::endl << std::endl;


  //-------------------------------------------------------------------------


  PointSetType::Pointer fixedPointSet  = PointSetType::New();
  PointSetType::Pointer movingPointSet = PointSetType::New();

  PointsContainer::Pointer fixedPointContainer  = PointsContainer::New();
  PointsContainer::Pointer movingPointContainer = PointsContainer::New();

  PointType fixedPoint;
  PointType movingPoint;

  // unnecessary: old test data
  //fixedPoint[0] = 13.5; fixedPoint[1] = 42.5; fixedPoint[2] = 4.5;
  //fixedPointContainer->InsertElement( 0, fixedPoint );
  //fixedPoint[0] = 12.2727; fixedPoint[1] = 14.7273; fixedPoint[2] = 6;
  //fixedPointContainer->InsertElement( 1, fixedPoint );
  //fixedPoint[0] = 30.7273; fixedPoint[1] = 49.2727; fixedPoint[2] = 9;
  //fixedPointContainer->InsertElement( 2, fixedPoint );
  //fixedPoint[0] = 29.3846; fixedPoint[1] = 21.6154; fixedPoint[2] = 10.4615;
  //fixedPointContainer->InsertElement( 3, fixedPoint );
  //fixedPoint[0] = 7.5; fixedPoint[1] = 65; fixedPoint[2] = 16.4;
  //fixedPointContainer->InsertElement( 4, fixedPoint );
  //fixedPoint[0] = 6.5; fixedPoint[1] = 37.5; fixedPoint[2] = 17.5;
  //fixedPointContainer->InsertElement( 5, fixedPoint );
  //fixedPoint[0] = 5.61538; fixedPoint[1] = 9.69231; fixedPoint[2] = 19.7692;
  //fixedPointContainer->InsertElement( 6, fixedPoint );
  //fixedPoint[0] = 24.3333; fixedPoint[1] = 57.6667; fixedPoint[2] = 21.5;
  //fixedPointContainer->InsertElement( 7, fixedPoint );
  //fixedPoint[0] = 23.4286; fixedPoint[1] = 30.4286; fixedPoint[2] = 22.5;
  //fixedPointContainer->InsertElement( 8, fixedPoint );

  fixedPoint[0] = -3.5;
  fixedPoint[1] = 8.5;
  fixedPoint[2] = 40;
  fixedPointContainer->InsertElement( 0, fixedPoint );

  fixedPoint[0] = 8.5;
  fixedPoint[1] = 3.5;
  fixedPoint[2] = 35.1;
  fixedPointContainer->InsertElement( 1, fixedPoint );
  
  fixedPoint[0] = 3.5;
  fixedPoint[1] = -8.5;
  fixedPoint[2] = 30;
  fixedPointContainer->InsertElement( 2, fixedPoint );
  
  fixedPoint[0] = -8.5;
  fixedPoint[1] = -3.5;
  fixedPoint[2] = 25;
  fixedPointContainer->InsertElement( 3, fixedPoint );
  
  fixedPoint[0] = -3.5;
  fixedPoint[1] = 8.5;
  fixedPoint[2] = 20;
  fixedPointContainer->InsertElement( 4, fixedPoint );

  fixedPoint[0] = 8.5;
  fixedPoint[1] = 3.5;
  fixedPoint[2] = 15;
  fixedPointContainer->InsertElement( 5, fixedPoint );
  
  fixedPoint[0] = 3.5;
  fixedPoint[1] = -8.5;
  fixedPoint[2] = 10;
  fixedPointContainer->InsertElement( 6, fixedPoint );
  
  fixedPoint[0] = -8.5;
  fixedPoint[1] = -3.5;
  fixedPoint[2] = 5;
  fixedPointContainer->InsertElement( 7, fixedPoint );
  
  fixedPoint[0] = -3.5;
  fixedPoint[1] = 8.5;
  fixedPoint[2] = 0;
  fixedPointContainer->InsertElement( 8, fixedPoint );

  fixedPointSet->SetPoints( fixedPointContainer );

  // unnecessary: old test data
  //movingPoint[0] = 114; movingPoint[1] = 143; movingPoint[2] = 105;
  //movingPointContainer->InsertElement( 0, movingPoint );
  //movingPoint[0] = 112.7727; movingPoint[1] = 115.2273;
  //    movingPoint[2] = 106.5;
  //movingPointContainer->InsertElement( 1, movingPoint );
  //movingPoint[0] = 131.2273; movingPoint[1] = 149.7727;
  //    movingPoint[2] = 109.5;
  //movingPointContainer->InsertElement( 2, movingPoint );
  //movingPoint[0] = 129.8846; movingPoint[1] = 122.1154;
  //    movingPoint[2] = 110.9615;
  //movingPointContainer->InsertElement( 3, movingPoint );
  //movingPoint[0] = 108; movingPoint[1] = 165.5; movingPoint[2] = 116.9;
  //movingPointContainer->InsertElement( 4, movingPoint );
  //movingPoint[0] = 107; movingPoint[1] = 138; movingPoint[2] = 118;
  //movingPointContainer->InsertElement( 5, movingPoint );
  //movingPoint[0] = 106.11538; movingPoint[1] = 110.19231;
  //    movingPoint[2] = 120.2692;
  //movingPointContainer->InsertElement( 6, movingPoint );
  //movingPoint[0] = 124.8333; movingPoint[1] = 158.1667; movingPoint[2] = 122;
  //movingPointContainer->InsertElement( 7, movingPoint );
  //movingPoint[0] = 123.9286; movingPoint[1] = 130.9286; movingPoint[2] = 123;
  //movingPointContainer->InsertElement( 8, movingPoint );

  movingPoint[0] = m_transformedData[m_sortedFiducials[8]][0]; 
  movingPoint[1] = m_transformedData[m_sortedFiducials[8]][1]; 
  movingPoint[2] = m_transformedData[m_sortedFiducials[8]][2];
  movingPointContainer->InsertElement( 0, movingPoint );

  movingPoint[0] = m_transformedData[m_sortedFiducials[7]][0]; 
  movingPoint[1] = m_transformedData[m_sortedFiducials[7]][1]; 
  movingPoint[2] = m_transformedData[m_sortedFiducials[7]][2];
  movingPointContainer->InsertElement( 1, movingPoint );

  movingPoint[0] = m_transformedData[m_sortedFiducials[6]][0]; 
  movingPoint[1] = m_transformedData[m_sortedFiducials[6]][1]; 
  movingPoint[2] = m_transformedData[m_sortedFiducials[6]][2];
  movingPointContainer->InsertElement( 2, movingPoint );

  movingPoint[0] = m_transformedData[m_sortedFiducials[5]][0]; 
  movingPoint[1] = m_transformedData[m_sortedFiducials[5]][1]; 
  movingPoint[2] = m_transformedData[m_sortedFiducials[5]][2];
  movingPointContainer->InsertElement( 3, movingPoint );

  movingPoint[0] = m_transformedData[m_sortedFiducials[4]][0]; 
  movingPoint[1] = m_transformedData[m_sortedFiducials[4]][1]; 
  movingPoint[2] = m_transformedData[m_sortedFiducials[4]][2];
  movingPointContainer->InsertElement( 4, movingPoint );

  movingPoint[0] = m_transformedData[m_sortedFiducials[3]][0]; 
  movingPoint[1] = m_transformedData[m_sortedFiducials[3]][1]; 
  movingPoint[2] = m_transformedData[m_sortedFiducials[3]][2];
  movingPointContainer->InsertElement( 5, movingPoint );

  movingPoint[0] = m_transformedData[m_sortedFiducials[2]][0]; 
  movingPoint[1] = m_transformedData[m_sortedFiducials[2]][1]; 
  movingPoint[2] = m_transformedData[m_sortedFiducials[2]][2];
  movingPointContainer->InsertElement( 6, movingPoint );

  movingPoint[0] = m_transformedData[m_sortedFiducials[1]][0]; 
  movingPoint[1] = m_transformedData[m_sortedFiducials[1]][1]; 
  movingPoint[2] = m_transformedData[m_sortedFiducials[1]][2];
  movingPointContainer->InsertElement( 7, movingPoint );

  movingPoint[0] = m_transformedData[m_sortedFiducials[0]][0]; 
  movingPoint[1] = m_transformedData[m_sortedFiducials[0]][1]; 
  movingPoint[2] = m_transformedData[m_sortedFiducials[0]][2];
  movingPointContainer->InsertElement( 8, movingPoint );

  movingPointSet->SetPoints( movingPointContainer );

  // Metric
  MetricType::Pointer        metric = MetricType::New();
  // Transform
  TransformType::Pointer     transform = TransformType::New();
  // Optimizer
  OptimizerType::Pointer     optimizer     = OptimizerType::New();
  optimizer->SetUseCostFunctionGradient(false);
  // Registration
  RegistrationType::Pointer  registration  = RegistrationType::New();

  // Scale the translation components of the Transform in the Optimizer
  OptimizerType::ScalesType scales( transform->GetNumberOfParameters() );

  // dynamic range of translations
  const double translationScale = 10000.0;
  // dynamic range of rotations
  const double rotationScale    =     1.0;   

  scales[0] = 1.0 / rotationScale;
  scales[1] = 1.0 / rotationScale;
  scales[2] = 1.0 / rotationScale;
  scales[3] = 1.0 / translationScale; 
  scales[4] = 1.0 / translationScale; 
  scales[5] = 1.0 / translationScale;

  unsigned long   numberOfIterations =  2000;
  // convergence criterion
  double          gradientTolerance  =  1e-5;   //1e-4;   
  double          valueTolerance     =  1e-5;   //1e-4;
  double          epsilonFunction    =  1e-6;   //1e-5;

  optimizer->SetScales( scales );
  optimizer->SetNumberOfIterations( numberOfIterations );
  optimizer->SetValueTolerance( valueTolerance );
  optimizer->SetGradientTolerance( gradientTolerance );
  optimizer->SetEpsilonFunction( epsilonFunction );

  // Start from an Identity transform (in a normal case, 
  // the user can probably provide a better guess than the identity...
  transform->SetIdentity();

  registration->SetInitialTransformParameters( transform->GetParameters() );

  // Connect all the components required for Registration
  registration->SetMetric(          metric          );
  registration->SetOptimizer(       optimizer       );
  registration->SetTransform(       transform       );
  registration->SetFixedPointSet(   fixedPointSet   );
  registration->SetMovingPointSet(  movingPointSet  );

  try 
  {
    registration->StartRegistration();
  }
  catch( itk::ExceptionObject & e )
  {
    std::cout << e << std::endl;
  }

  m_parameters = transform->GetParameters();
  std::cout << "Solution = " << m_parameters << std::endl;

  m_optimizerValues = optimizer->GetValue();
  std::cout << "Optimizer values: " << std::endl << m_optimizerValues;
  std::cout << std::endl;

  m_TransformMatrix = transform->GetMatrix();
  std::cout << "Matrix: " << std::endl << m_TransformMatrix << std::endl;

  m_translation = transform->GetTranslation();

  //unnecessary: test code
  /*
  double angleAroundX, angleAroundY, angleAroundZ;
  angleAroundX = ( ( acos( m_TransformMatrix[2][2] ) + 
                     acos( m_TransformMatrix[1][1] ) +
                     asin( -m_TransformMatrix[2][1] ) + 
                     asin( m_TransformMatrix[1][2] ) 
                   ) / 4 
                 ) * ( 180 / 3.141593 );

  angleAroundY = ( ( acos( m_TransformMatrix[2][2] ) + 
                     acos( m_TransformMatrix[0][0] ) + 
                     asin( -m_TransformMatrix[0][2] ) +
                     asin( m_TransformMatrix[2][0] )
                   ) / 4 
                  ) * ( 180 / 3.141593 );

  angleAroundZ = ( ( acos( m_TransformMatrix[0][0] ) +
                     acos( m_TransformMatrix[1][1] ) +
                     asin( -m_TransformMatrix[1][0] ) + 
                     asin( m_TransformMatrix[0][1] ) 
                   ) / 4 
                 ) * ( 180 / 3.141593 );

  std::cout << std::endl << "Rotation around X: " << angleAroundX << ", Y: ";
  std::cout << angleAroundY << ", Z: " << angleAroundZ << std::endl;
  */

  //TransformType::Pointer    transform2 = TransformType::New();

}
