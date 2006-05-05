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

namespace igstk{

DOCR_Registration::DOCR_Registration( ImageSOType::Pointer imageSO, 
                                      USVolumeType::IndexType ROIstart,
                                      USVolumeType::SizeType ROIsize) 
{

  ITKImageObserver::Pointer itkImageObserver = ITKImageObserver::New();
  imageSO->AddObserver( ImageSOType::ITKImageModifiedEvent(), itkImageObserver);

  imageSO->RequestGetITKImage();

  if( itkImageObserver->GotITKImage() )
    {
    m_USEntireLoadedVolume = itkImageObserver->GetITKImage();

  //---------------------------------------------------------------------
    //typedef itk::ImageFileWriter< USVolumeType >  ROIWriterType;
    //ROIWriterType::Pointer ROIwriter = ROIWriterType::New();
    //ROIwriter->SetFileName( "dicoms.mha" );
    //ROIwriter->SetInput( itkImageObserver->GetITKImage() );
    //try 
    //  { 
    //  ROIwriter->Update(); 
    //  } 
    //catch( itk::ExceptionObject & err ) 
    //  { 
    //  std::cout << "ExceptionObject caught !" << std::endl; 
    //  std::cout << err << std::endl; 
    //  } 
//------------------------------------------------------------------------

    m_CT_Spacing = m_USEntireLoadedVolume->GetSpacing();
    m_ROIstart = ROIstart;
    m_ROIsize = ROIsize;

    for (int i = 0; i < NUM_FIDUCIALS; i++) m_fiducials[i][0][0] = 0;
    m_numFiducials = 0;

    m_verificationPoint[0] = 37.772; //4.05799;
    m_verificationPoint[1] = 48.8854; //7.0042;
    m_verificationPoint[2] = 9.52381; //19.7692;
    m_verificationPoint[2] *= -1; // reflection

    }
  else
    {
    std::cout<< "Error while accessing the ITK image..." << std::endl;
    }
}

DOCR_Registration::DOCR_Registration( USVolumeType::Pointer volume,
                                      USVolumeType::IndexType ROIstart,
                                      USVolumeType::SizeType ROIsize )
{
  m_USEntireLoadedVolume = volume;

  m_CT_Spacing = m_USEntireLoadedVolume->GetSpacing();
  m_ROIstart = ROIstart;
  m_ROIsize = ROIsize;

  for (int i = 0; i < NUM_FIDUCIALS; i++) m_fiducials[i][0][0] = 0;
  m_numFiducials = 0;

  m_verificationPoint[0] = 37.772; //4.05799;
  m_verificationPoint[1] = 48.8854; //7.0042;
  m_verificationPoint[2] = 9.52381; //19.7692;
  m_verificationPoint[2] *= -1; // reflection
}

DOCR_Registration::~DOCR_Registration()
{
}

void DOCR_Registration::compute() 
{
  m_ROIstart[0] = 44; m_ROIstart[1] = 127; m_ROIstart[2] = 42;
  m_ROIsize[0] = 75; m_ROIsize[1] = 75; m_ROIsize[2] = 25;
  // Initialization
  std::vector< vnl_vector<double> > segmentedFiducials(NUM_FIDUCIALS);
  for(int i = 0; i < NUM_FIDUCIALS; i++) segmentedFiducials[i].set_size(3);
  short found = -1;
  std::vector< vnl_vector<double> > testData(NUM_FIDUCIALS);
  for(int i = 0; i < NUM_FIDUCIALS; i++) testData[i].set_size(3);
  testData[0].copy_in(m_verificationPoint);
  vnl_matrix<double>                m_testPTProtation(3,3);


// ----------------------------------------------------------------------------

  std::cout << "--------- Extracting ROI from CT ---------\n\n";

  typedef itk::RegionOfInterestImageFilter< USVolumeType, 
    USVolumeType > ROIfilterType;
  ROIfilterType::Pointer ROIfilter = ROIfilterType::New();

  USVolumeType::RegionType desiredRegion;
  desiredRegion.SetSize(  m_ROIsize  );
  desiredRegion.SetIndex( m_ROIstart );

  ROIfilter->SetRegionOfInterest( desiredRegion );
  ROIfilter->SetInput( m_USEntireLoadedVolume );
  ROIfilter->Update();

  m_USLoadedVolume = ROIfilter->GetOutput();

  typedef itk::ImageFileWriter< USVolumeType >  ROIWriterType;
  ROIWriterType::Pointer ROIwriter = ROIWriterType::New();
  ROIwriter->SetFileName( "volume_2_CT_ROI_ushort.mha" );
  ROIwriter->SetInput( ROIfilter->GetOutput() );
  try 
  { 
    ROIwriter->Update(); 
  } 
  catch( itk::ExceptionObject & err ) 
  { 
    std::cout << "ExceptionObject caught !" << std::endl; 
    std::cout << err << std::endl; 
  } 

// ----------------------------------------------------------------------------

  {
    std::cout << "--------- Searching for segmentation threshold ---------\n\n";

    unsigned int bin, bigBin = 0;
    float /*__int64*/ avg = 0, max = 0;

    HistogramGeneratorType::Pointer histogramGenerator =
      HistogramGeneratorType::New();

    histogramGenerator->SetInput( m_USLoadedVolume );
    histogramGenerator->SetNumberOfBins( 4096 );
    histogramGenerator->SetMarginalScale( 10.0 );
    histogramGenerator->Compute();

    const HistogramType * histogram = histogramGenerator->GetOutput();
    const unsigned int histogramSize = histogram->Size();

    //std::cout << "Histogram size " << histogramSize << std::endl;

    for( bin=0; bin <= histogramSize; bin++ )
    {
      avg += histogram->GetFrequency( bin, 0 );
      if ((((bin+1)%BIGBINSIZE)==0)&&(bin < histogramSize)) {
        avg /= BIGBINSIZE;
        m_bins[bigBin] = avg;
        if (bigBin == NUMBERBIGBINS) std::cout << std::endl << "ERROR\n";
        if (avg > max) max = avg;
        avg = 0;
        bigBin++;
      } else if (bin == histogramSize) 
      {
        avg /= (bin+1)%BIGBINSIZE;
        m_bins[bigBin] = avg;
        if (bigBin == NUMBERBIGBINS) std::cout << std::endl << "ERROR\n";
        avg = 0;
        bigBin++;
      }
    }
  
    //std::cout << std::endl << max << std::endl << std::endl;
    avg=0;
    for( bin=0; bin <= histogramSize; bin++ ) // <= is correct !!
    {
      avg += histogram->GetFrequency( bin, 0 );
      if ((((bin+1)%BIGBINSIZE)==0)&&(bin < histogramSize)) {
        avg /= BIGBINSIZE;
        std::cout << bin << " " << avg << " ";
        for (int i=0; i <= (avg*120)/max; i++) std::cout << "#";
        std::cout << std::endl;
        avg = 0;
      } else if (bin == histogramSize) {
        avg /= (bin+1)%BIGBINSIZE;
        std::cout << bin << " " << avg << " ";
        for (int i=0; i <= (avg*120)/max; i++) std::cout << "#";
        std::cout << std::endl;
        avg = 0;
      }
    }
  
    bool thresholdfound = false;
    unsigned int subtracted = 0;
    do {
      for( bin=0; bin < NUMBERBIGBINS; bin++ ) m_bins[bin] -= 0.01;
      subtracted++;
      for( bin = NUMBERBIGBINS - 1; (bin >= 3)&&(m_bins[bin] >= 0); bin--) ;
      if((m_bins[bin] < 0)&&(m_bins[bin-1] < 0)&&
         (m_bins[bin-2] < 0)&&(m_bins[bin-3] < 0))
        thresholdfound = true;
    } while((!thresholdfound) && (bin < (NUMBERBIGBINS - 2)));

    if (thresholdfound)
      std::cout << "threshold bin: " << bin+1 << ". (of " <<
        NUMBERBIGBINS << ")" << std::endl;
    else std::cout << "no threshold found" << std::endl;
    m_threshold = bin * BIGBINSIZE;

    std::cout << "subtraction steps of 0.01: " << subtracted << std::endl;

    //for( bin=0; bin < NUMBERBIGBINS; bin++ )
    //  std::cout << bin << " " << m_bins[bin] << std::endl;

    if (!thresholdfound) throw "no threshold";

    std::cout << "threshold: " << m_threshold << std::endl;

    //  //HistogramType::ConstIterator itr = histogram->Begin();
    //  //HistogramType::ConstIterator end = histogram->End();
    //
    //  //unsigned int binNumber = 0;
    //  //while( itr != end )
    //  //{
    //   // std::cout << "bin = " << binNumber << " frequency = ";
    //   // std::cout << itr.GetFrequency() << std::endl;     
    //   // ++itr;
    //   // ++binNumber;
    //  //}
  }

// ----------------------------------------------------------------------------

  m_volumeSize = m_USLoadedVolume->GetLargestPossibleRegion().GetSize();

  itk::ImageRegionConstIterator< USVolumeType >
  //itk::ImageRegionIterator< USVolumeType >
    it1(m_USLoadedVolume, m_USLoadedVolume->GetLargestPossibleRegion() );
  it1.GoToBegin();

  std::cout << "----------------- Segmenting fiducials -------------------\n\n";

  for (int k = 0; k < m_volumeSize[2]; k++)              // z
  //for (int k = m_volumeSize[2] - 1; k >=0 ; k--)              // z
  {
    for (int j = 0; j < m_volumeSize[1]; j++)            // y
    {
      for (int i = 0; i < m_volumeSize[0]; i++, ++it1)   // x
      {
        if ((it1.Get() > /*m_threshold*/3000)&&!((i>10)&&(i<16)&&(j>22)&&(j<28)))
        {
          // C 9
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
                   (m_fiducials[l][m][2] >= (((m_volumeSize[2] - 1 - k - 1)<0) ? 0:(m_volumeSize[2] - 1 - k-1))) &&
                   (m_fiducials[l][m][2] <= (((m_volumeSize[2] - 1 - k + 1)>(m_volumeSize[2]-1)) ?
                   (m_volumeSize[2] - 1) : (m_volumeSize[2] - 1 - k + 1))))
                {
                  found = l;
                  m_fiducials[l][m_fiducials[l][0][0] + 1][0] = i;
                  m_fiducials[l][m_fiducials[l][0][0] + 1][1] = j;
                  m_fiducials[l][m_fiducials[l][0][0] + 1][2] = m_volumeSize[2] - 1 - k;
                }
              }
            }
          }
          if (found != -1)
          {
            m_fiducials[found][0][0]++;
            std::cout << "Pixel " << i << "/" << j << "/" << m_volumeSize[2] - 1 - k <<
              " add: Fiducial " << found << std::endl;
          }
          else
          {
            m_fiducials[m_numFiducials][0][0] = 1;
            m_fiducials[m_numFiducials][1][0] = i;
            m_fiducials[m_numFiducials][1][1] = j;
            m_fiducials[m_numFiducials][1][2] = m_volumeSize[2] - 1 - k;
            std::cout << "Pixel " << i << "/" << j << "/" << m_volumeSize[2] - 1 - k <<
                  " new: Fiducial " << m_numFiducials << std::endl;
            m_numFiducials++;
          }
        }
        found = -1;
      }
    }
  }

  // C 8

  std::cout << std::endl << "Modifying data according to spacing" <<
    std::endl << std::endl;

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
    
    USVolumeType::PointType origin = m_USEntireLoadedVolume->GetOrigin();
    std::cout<< "Origin of image: "<< origin << std::endl;
    for(int j=0; j<3; j++)
      {
      avg[j] = /*origin[j]*/ + (avg[j]+m_ROIstart[j]) * m_CT_Spacing[j];
      }
    avg[2] *= -1; // reflection

    segmentedFiducials[i].copy_in(avg);
    std::cout << "Fiducial " << i << ": " << segmentedFiducials[i] << 
      ", CT pixel space: " << avg[0]/m_CT_Spacing[0] << " " << avg[1]/m_CT_Spacing[1] << " " << -1*avg[2] << std::endl;
  }

  std::cout << std::endl <<
    "------------------- Doing PCA and sorting ----------------------" <<
    std::endl << std::endl;

  // C 1

  m_meanPoint.set_size(SPACE_DIMENSION);
  m_meanPoint = (double)(0.0);
  for(int i = 0; i < SPACE_DIMENSION; i++)
    for(int j = 0; j < m_numFiducials; j++)
      m_meanPoint[i] += segmentedFiducials[j][i];
  m_meanPoint /= m_numFiducials;

  std::cout << "mean point: " << m_meanPoint << std::endl;

  for(int i = 0; i < m_numFiducials; i++)
    m_transformedData.push_back(segmentedFiducials[i] - m_meanPoint);

  std::cout << "verification point: " << testData[0] << std::endl;

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
  m_inverseEigenVectors = vnl_matrix_inverse<double>(m_eigenVectors);
  for(int i = 0; i < SPACE_DIMENSION; i++)
    m_eigenValues.push_back(eigenSystem.D(i, i));

  for(int i = 0; i < m_numFiducials; i++) 
    m_transformedData[i].post_multiply(m_eigenVectors);

  // C 10

  //testData[0].post_multiply(m_eigenVectors);
  //std::cout << "verification point (after PCA): " << testData[0] << std::endl;
  //testData[0].post_multiply(m_inverseEigenVectors);
  //testData[0][0] += m_meanPoint[0]; testData[0][1] += m_meanPoint[1];
  //testData[0][2] += m_meanPoint[2]; 

  //std::cout << "m_eigenVectors: " << eigenSystem.V << std::endl;
  //std::cout << "m_covariance: " << m_covariance << std::endl;

  // C 2

  for (int i = 0; i < m_numFiducials; i++) 
    std::cout << "PCA Transformed Data " << i << ": " <<
        m_transformedData[i] << std::endl;

  for (int i = 0; i < m_numFiducials; i++)
        m_transformedDataZ[i] = m_transformedData[i][2];

  std::sort(m_transformedDataZ, m_transformedDataZ + m_numFiducials);

  for (int i = 0; i < m_numFiducials; i++)
  {
    for (int j = 0; j < m_numFiducials; j++)
    {
      if (m_transformedDataZ[i] == m_transformedData[j][2])
      {
        m_sortedFiducials[/*m_numFiducials - 1 -*/ i] = j;
        m_transformedData[j][2] += 999;
      }
    }
  }
  for (int i = 0; i < m_numFiducials; i++)
    if (m_transformedData[i][2] > 500)
      m_transformedData[i][2] -= 999;

  std::cout << "Sorted Fiducials starting with fiducial on top of" <<
    " needle holder: " << std::endl;
  for (int i = 0; i < m_numFiducials; i++)
     std::cout << m_sortedFiducials[i] << " ";
  std::cout << std::endl;

  //std::cout << "xyz-distance(divided by 4) points 0-4-8: " <<
  //  sqrt(pow(m_transformedData[m_sortedFiducials[0]][0]-
  //  m_transformedData[m_sortedFiducials[4]][0],2)+
  //  pow(m_transformedData[m_sortedFiducials[0]][1]-
  //  m_transformedData[m_sortedFiducials[4]][1],2)+
  //  pow(m_transformedData[m_sortedFiducials[0]][2]-
  //  m_transformedData[m_sortedFiducials[4]][2],2))/4 << " and " <<
  //  sqrt(pow(m_transformedData[m_sortedFiducials[8]][0]-
  //  m_transformedData[m_sortedFiducials[4]][0],2)+
  //  pow(m_transformedData[m_sortedFiducials[8]][1]-
  //  m_transformedData[m_sortedFiducials[4]][1],2)+
  //  pow(m_transformedData[m_sortedFiducials[8]][2]-
  //  m_transformedData[m_sortedFiducials[4]][2],2))/4 << std::endl;
  //std::cout << "xyz-distance(divided by 4) points 0-3-7: " <<
  //  sqrt(pow(m_transformedData[m_sortedFiducials[0]][0]-
  //  m_transformedData[m_sortedFiducials[3]][0],2)+
  //  pow(m_transformedData[m_sortedFiducials[0]][1]-
  //  m_transformedData[m_sortedFiducials[3]][1],2)+
  //  pow(m_transformedData[m_sortedFiducials[0]][2]-
  //  m_transformedData[m_sortedFiducials[3]][2],2))/4 << " and " <<
  //  sqrt(pow(m_transformedData[m_sortedFiducials[7]][0]-
  //  m_transformedData[m_sortedFiducials[3]][0],2)+
  //  pow(m_transformedData[m_sortedFiducials[7]][1]-
  //  m_transformedData[m_sortedFiducials[3]][1],2)+
  //  pow(m_transformedData[m_sortedFiducials[7]][2]-
  //  m_transformedData[m_sortedFiducials[3]][2],2))/4 << std::endl;
  //std::cout << "                           xy-distance : " <<
  //  sqrt(pow(m_transformedData[m_sortedFiducials[0]][0]-
  //  m_transformedData[m_sortedFiducials[3]][0],2)+
  //  pow(m_transformedData[m_sortedFiducials[0]][1]-
  //  m_transformedData[m_sortedFiducials[3]][1],2)) << " and " <<
  //  sqrt(pow(m_transformedData[m_sortedFiducials[7]][0]-
  //  m_transformedData[m_sortedFiducials[3]][0],2)+
  //  pow(m_transformedData[m_sortedFiducials[7]][1]-
  //  m_transformedData[m_sortedFiducials[3]][1],2)) << ", all: " <<
  //  sqrt(pow(m_transformedData[m_sortedFiducials[7]][0]-
  //  m_transformedData[m_sortedFiducials[0]][0],2)+
  //  pow(m_transformedData[m_sortedFiducials[7]][1]-
  //  m_transformedData[m_sortedFiducials[0]][1],2)) << std::endl;
  //std::cout << "xyz-distance(divided by 4) points 1-5: "   <<
  //  sqrt(pow(m_transformedData[m_sortedFiducials[1]][0]-
  //  m_transformedData[m_sortedFiducials[5]][0],2)+
  //  pow(m_transformedData[m_sortedFiducials[1]][1]-
  //  m_transformedData[m_sortedFiducials[5]][1],2)+
  //  pow(m_transformedData[m_sortedFiducials[1]][2]-
  //  m_transformedData[m_sortedFiducials[5]][2],2))/4 << " / xy-distance : " <<
  //  sqrt(pow(m_transformedData[m_sortedFiducials[1]][0]-
  //  m_transformedData[m_sortedFiducials[5]][0],2)+
  //  pow(m_transformedData[m_sortedFiducials[1]][1]-
  //  m_transformedData[m_sortedFiducials[5]][1],2)) << std::endl;
  //std::cout << "xyz-distance(divided by 4) points 2-6: "   <<
  //  sqrt(pow(m_transformedData[m_sortedFiducials[2]][0]-
  //  m_transformedData[m_sortedFiducials[6]][0],2)+
  //  pow(m_transformedData[m_sortedFiducials[2]][1]-
  //  m_transformedData[m_sortedFiducials[6]][1],2)+
  //  pow(m_transformedData[m_sortedFiducials[2]][2]-
  //  m_transformedData[m_sortedFiducials[6]][2],2))/4 << " / xy-distance : " <<
  //  sqrt(pow(m_transformedData[m_sortedFiducials[2]][0]-
  //  m_transformedData[m_sortedFiducials[6]][0],2)+
  //  pow(m_transformedData[m_sortedFiducials[2]][1]-
  //  m_transformedData[m_sortedFiducials[6]][1],2)) << std::endl;
  //std::cout << "xyz-distance(divided by 4) points 3-7: "   <<
  //  sqrt(pow(m_transformedData[m_sortedFiducials[3]][0]-
  //  m_transformedData[m_sortedFiducials[7]][0],2)+
  //  pow(m_transformedData[m_sortedFiducials[3]][1]-
  //  m_transformedData[m_sortedFiducials[7]][1],2)+
  //  pow(m_transformedData[m_sortedFiducials[3]][2]-
  //  m_transformedData[m_sortedFiducials[7]][2],2))/4 << " / xy-distance : " <<
  //  sqrt(pow(m_transformedData[m_sortedFiducials[3]][0]-
  //  m_transformedData[m_sortedFiducials[7]][0],2)+
  //  pow(m_transformedData[m_sortedFiducials[3]][1]-
  //  m_transformedData[m_sortedFiducials[7]][1],2)) << std::endl;

  std::cout << std::endl <<
    "------------ Doing closed solution landmark registration ----------------"
    << std::endl << std::endl;

  //-------------------------------------------------------------------------

  // C 7

  LandmarkImageType::Pointer fixedImage  = LandmarkImageType::New();
  LandmarkImageType::Pointer movingImage = LandmarkImageType::New();

  // Create fixed and moving images
  LandmarkImageType::RegionType fRegion;
  LandmarkImageType::SizeType   fSize;
  LandmarkImageType::IndexType  fIndex;
  fSize.Fill(30);
  fIndex.Fill(0);
  fRegion.SetSize( fSize );
  fRegion.SetIndex( fIndex );
  LandmarkImageType::RegionType mRegion;
  LandmarkImageType::SizeType   mSize;
  LandmarkImageType::IndexType  mIndex;
  mSize.Fill(30);
  mIndex.Fill(0);
  mRegion.SetSize( mSize );
  mRegion.SetIndex( mIndex );
  fixedImage->SetLargestPossibleRegion( fRegion );
  fixedImage->SetBufferedRegion( fRegion );
  fixedImage->SetRequestedRegion( fRegion );
  fixedImage->Allocate();
  movingImage->SetLargestPossibleRegion( mRegion );
  movingImage->SetBufferedRegion( mRegion );
  movingImage->SetRequestedRegion( mRegion );
  movingImage->Allocate();

  // Set the transform type
  m_transform = TransformType::New();
  TransformInitializerType::Pointer initializer =
    TransformInitializerType::New();

  // Set fixed and moving landmarks
  TransformInitializerType::LandmarkPointContainer landmarkSet1;
  TransformInitializerType::LandmarkPointContainer landmarkSet2;
  TransformInitializerType::LandmarkPointType point;
  TransformInitializerType::LandmarkPointType tmp;

  //point[0] = -3.5;
  //point[1] = 8.5;
  //point[2] = 40;
  //landmarkSet1.push_back(point);

  ////point[0] = 8.5;
  ////point[1] = 3.5;
  ////point[2] = 35.1;
  ////landmarkSet1.push_back(point);
  //
  //point[0] = 3.5;
  //point[1] = -8.5;
  //point[2] = 30;
  //landmarkSet1.push_back(point);
  //
  //point[0] = -8.5;
  //point[1] = -3.5;
  //point[2] = 25;
  //landmarkSet1.push_back(point);
  //
  //point[0] = -3.5;
  //point[1] = 8.5;
  //point[2] = 20;
  //landmarkSet1.push_back(point);

  //point[0] = 8.5;
  //point[1] = 3.5;
  //point[2] = 15;
  //landmarkSet1.push_back(point);
  //
  //point[0] = 3.5;
  //point[1] = -8.5;
  //point[2] = 10;
  //landmarkSet1.push_back(point);
  //
  //point[0] = -8.5;
  //point[1] = -3.5;
  //point[2] = 5;
  //landmarkSet1.push_back(point);
  //
  //point[0] = -3.5;
  //point[1] = 8.5;
  //point[2] = 0;
  //landmarkSet1.push_back(point);

  //0
  point[0] = 8.0;
  point[1] = -3.3;
  point[2] = 45 - 1.25;
  landmarkSet1.push_back(point);

  //1
  //point[0] = 3.3;
  //point[1] = -8.0;
  //point[2] = 42.5 - 1.25;
  //landmarkSet1.push_back(point);

  //2
  point[0] = -3.3;
  point[1] = -8.0;
  point[2] = 40 - 1.25;
  landmarkSet1.push_back(point);

  //3
  point[0] = -8.0;
  point[1] = -3.3;
  point[2] = 37.5 - 1.25;
  landmarkSet1.push_back(point);

  //4
  point[0] = -8.0;
  point[1] = 3.3;
  point[2] = 35 - 1.25;
  landmarkSet1.push_back(point);

  //5
  point[0] = -3.3;
  point[1] = 8.0;
  point[2] = 32.5 - 1.25;
  landmarkSet1.push_back(point);

  //6
  point[0] = 3.3;
  point[1] = 8.0;
  point[2] = 30 - 1.25;
  landmarkSet1.push_back(point);

  //7
  point[0] = 8.0;
  point[1] = 3.3;
  point[2] = 27.5 - 1.25;
  landmarkSet1.push_back(point);

  //8
  point[0] = 8.0;
  point[1] = -3.3;
  point[2] = 25 - 1.25;
  landmarkSet1.push_back(point);

  //9
  point[0] = 3.3;
  point[1] = -8.0;
  point[2] = 22.5 - 1.25;
  landmarkSet1.push_back(point);

  //10
  point[0] = -3.3;
  point[1] = -8.0;
  point[2] = 20 - 1.25;
  landmarkSet1.push_back(point);

  //11
  point[0] = -8.0;
  point[1] = -3.3;
  point[2] = 17.5 - 1.25;
  landmarkSet1.push_back(point);

  //12
  point[0] = -8.0;
  point[1] = 3.3;
  point[2] = 15 - 1.25;
  landmarkSet1.push_back(point);

  //13
  point[0] = -3.3;
  point[1] = 8.0;
  point[2] = 12.5 - 1.25;
  landmarkSet1.push_back(point);

  //14
  point[0] = 3.3;
  point[1] = 8.0;
  point[2] = 10 - 1.25;
  landmarkSet1.push_back(point);

  //15
  point[0] = 8.0;
  point[1] = 3.3;
  point[2] = 7.5 - 1.25;
  landmarkSet1.push_back(point);

  //16
  point[0] = 8.0;
  point[1] = -3.3;
  point[2] = 5 - 1.25;
  landmarkSet1.push_back(point);

  //17
  point[0] = 3.3;
  point[1] = -8.0;
  point[2] = 2.5 - 1.25;
  landmarkSet1.push_back(point);

  //18
  point[0] = -3.3;
  point[1] = -8.0;
  point[2] = 0 - 1.25;
  landmarkSet1.push_back(point);

  //point[0] = segmentedFiducials[m_sortedFiducials[0]][0]; 
  //point[1] = segmentedFiducials[m_sortedFiducials[0]][1]; 
  //point[2] = segmentedFiducials[m_sortedFiducials[0]][2];
  //landmarkSet2.push_back(point);

  //point[0] = segmentedFiducials[m_sortedFiducials[1]][0]; 
  //point[1] = segmentedFiducials[m_sortedFiducials[1]][1]; 
  //point[2] = segmentedFiducials[m_sortedFiducials[1]][2];
  //landmarkSet2.push_back(point);

  //point[0] = segmentedFiducials[m_sortedFiducials[2]][0]; 
  //point[1] = segmentedFiducials[m_sortedFiducials[2]][1]; 
  //point[2] = segmentedFiducials[m_sortedFiducials[2]][2];
  //landmarkSet2.push_back(point);

  //point[0] = segmentedFiducials[m_sortedFiducials[3]][0]; 
  //point[1] = segmentedFiducials[m_sortedFiducials[3]][1]; 
  //point[2] = segmentedFiducials[m_sortedFiducials[3]][2];
  //landmarkSet2.push_back(point);

  //point[0] = segmentedFiducials[m_sortedFiducials[4]][0]; 
  //point[1] = segmentedFiducials[m_sortedFiducials[4]][1]; 
  //point[2] = segmentedFiducials[m_sortedFiducials[4]][2];
  //landmarkSet2.push_back(point);

  //point[0] = segmentedFiducials[m_sortedFiducials[5]][0]; 
  //point[1] = segmentedFiducials[m_sortedFiducials[5]][1]; 
  //point[2] = segmentedFiducials[m_sortedFiducials[5]][2];
  //landmarkSet2.push_back(point);

  //point[0] = segmentedFiducials[m_sortedFiducials[6]][0]; 
  //point[1] = segmentedFiducials[m_sortedFiducials[6]][1]; 
  //point[2] = segmentedFiducials[m_sortedFiducials[6]][2];
  //landmarkSet2.push_back(point);

  //point[0] = segmentedFiducials[m_sortedFiducials[7]][0]; 
  //point[1] = segmentedFiducials[m_sortedFiducials[7]][1]; 
  //point[2] = segmentedFiducials[m_sortedFiducials[7]][2];
  //landmarkSet2.push_back(point);

  point[0] = segmentedFiducials[m_sortedFiducials[0]][0]; 
  point[1] = segmentedFiducials[m_sortedFiducials[0]][1]; 
  point[2] = segmentedFiducials[m_sortedFiducials[0]][2];
  landmarkSet2.push_back(point);

  point[0] = segmentedFiducials[m_sortedFiducials[1]][0]; 
  point[1] = segmentedFiducials[m_sortedFiducials[1]][1]; 
  point[2] = segmentedFiducials[m_sortedFiducials[1]][2];
  landmarkSet2.push_back(point);

  point[0] = segmentedFiducials[m_sortedFiducials[2]][0]; 
  point[1] = segmentedFiducials[m_sortedFiducials[2]][1]; 
  point[2] = segmentedFiducials[m_sortedFiducials[2]][2];
  landmarkSet2.push_back(point);

  point[0] = segmentedFiducials[m_sortedFiducials[3]][0]; 
  point[1] = segmentedFiducials[m_sortedFiducials[3]][1]; 
  point[2] = segmentedFiducials[m_sortedFiducials[3]][2];
  landmarkSet2.push_back(point);

  point[0] = segmentedFiducials[m_sortedFiducials[4]][0]; 
  point[1] = segmentedFiducials[m_sortedFiducials[4]][1]; 
  point[2] = segmentedFiducials[m_sortedFiducials[4]][2];
  landmarkSet2.push_back(point);

  point[0] = segmentedFiducials[m_sortedFiducials[5]][0]; 
  point[1] = segmentedFiducials[m_sortedFiducials[5]][1]; 
  point[2] = segmentedFiducials[m_sortedFiducials[5]][2];
  landmarkSet2.push_back(point);

  point[0] = segmentedFiducials[m_sortedFiducials[6]][0]; 
  point[1] = segmentedFiducials[m_sortedFiducials[6]][1]; 
  point[2] = segmentedFiducials[m_sortedFiducials[6]][2];
  landmarkSet2.push_back(point);

  point[0] = segmentedFiducials[m_sortedFiducials[7]][0]; 
  point[1] = segmentedFiducials[m_sortedFiducials[7]][1]; 
  point[2] = segmentedFiducials[m_sortedFiducials[7]][2];
  landmarkSet2.push_back(point);

  point[0] = segmentedFiducials[m_sortedFiducials[8]][0]; 
  point[1] = segmentedFiducials[m_sortedFiducials[8]][1]; 
  point[2] = segmentedFiducials[m_sortedFiducials[8]][2];
  landmarkSet2.push_back(point);

  point[0] = segmentedFiducials[m_sortedFiducials[9]][0]; 
  point[1] = segmentedFiducials[m_sortedFiducials[9]][1]; 
  point[2] = segmentedFiducials[m_sortedFiducials[9]][2];
  landmarkSet2.push_back(point);

  point[0] = segmentedFiducials[m_sortedFiducials[10]][0]; 
  point[1] = segmentedFiducials[m_sortedFiducials[10]][1]; 
  point[2] = segmentedFiducials[m_sortedFiducials[10]][2];
  landmarkSet2.push_back(point);

  point[0] = segmentedFiducials[m_sortedFiducials[11]][0]; 
  point[1] = segmentedFiducials[m_sortedFiducials[11]][1]; 
  point[2] = segmentedFiducials[m_sortedFiducials[11]][2];
  landmarkSet2.push_back(point);

  point[0] = segmentedFiducials[m_sortedFiducials[12]][0]; 
  point[1] = segmentedFiducials[m_sortedFiducials[12]][1]; 
  point[2] = segmentedFiducials[m_sortedFiducials[12]][2];
  landmarkSet2.push_back(point);

  point[0] = segmentedFiducials[m_sortedFiducials[13]][0]; 
  point[1] = segmentedFiducials[m_sortedFiducials[13]][1]; 
  point[2] = segmentedFiducials[m_sortedFiducials[13]][2];
  landmarkSet2.push_back(point);

  point[0] = segmentedFiducials[m_sortedFiducials[14]][0]; 
  point[1] = segmentedFiducials[m_sortedFiducials[14]][1]; 
  point[2] = segmentedFiducials[m_sortedFiducials[14]][2];
  landmarkSet2.push_back(point);

  point[0] = segmentedFiducials[m_sortedFiducials[15]][0]; 
  point[1] = segmentedFiducials[m_sortedFiducials[15]][1]; 
  point[2] = segmentedFiducials[m_sortedFiducials[15]][2];
  landmarkSet2.push_back(point);

  point[0] = segmentedFiducials[m_sortedFiducials[16]][0]; 
  point[1] = segmentedFiducials[m_sortedFiducials[16]][1]; 
  point[2] = segmentedFiducials[m_sortedFiducials[16]][2];
  landmarkSet2.push_back(point);

  point[0] = segmentedFiducials[m_sortedFiducials[17]][0]; 
  point[1] = segmentedFiducials[m_sortedFiducials[17]][1]; 
  point[2] = segmentedFiducials[m_sortedFiducials[17]][2];
  landmarkSet2.push_back(point);

//  point[0] = segmentedFiducials[m_sortedFiducials[18]][0]; 
//  point[1] = segmentedFiducials[m_sortedFiducials[18]][1]; 
//  point[2] = segmentedFiducials[m_sortedFiducials[18]][2];
//  landmarkSet2.push_back(point);

  initializer->SetFixedLandmarks(landmarkSet2);
  initializer->SetMovingLandmarks(landmarkSet1);

  initializer->SetFixedImage( fixedImage );
  initializer->SetMovingImage( movingImage );
  initializer->SetTransform( m_transform );
  initializer->InitializeTransform();

  TransformInitializerType::PointsContainerConstIterator 
    fitr = landmarkSet2.begin();
  TransformInitializerType::PointsContainerConstIterator 
    mitr = landmarkSet1.begin();

  while( mitr != landmarkSet1.end() )
  {
    error = *mitr - m_transform->TransformPoint( *fitr);

    std::cout << "Fixed Landmark: " << *fitr << " Moving landmark " << *mitr 
      << " Transformed fixed Landmark : " << 
      m_transform->TransformPoint( *fitr ) << std::endl <<
      "  Error: " << error.GetNorm() << std::endl;

    ++mitr;
    ++fitr;
  }

  point[0] = testData[0][0];
  point[1] = testData[0][1];
  point[2] = testData[0][2];
  std::cout << "verification point (after landmark registration): " <<
    m_transform->TransformPoint( point ) << std::endl;

  // C 6
  // C 5
}

}
