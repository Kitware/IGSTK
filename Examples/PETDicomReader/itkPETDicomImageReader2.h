/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    itkPETDicomImageReader2.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkPETDicomImageReader2_h
#define __itkPETDicomImageReader2_h

#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"


namespace itk
{

/** \class PETDicomImageReader2
 * 
 * \brief This class reads PET DICOM files. 
 *
 */
template< class TImageType >
class PETDicomImageReader2 : public  Object
{

public: 
  /** Standard class typedef. */
  typedef PETDicomImageReader2       Self;
  typedef Object                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-type information ( and related methods.) */
  itkTypeMacro( PETDicomImageReader2, Object );

  /** Some convenient typedefs for input image */
  typedef TImageType  ImageType;
  typedef itk::ImageSeriesReader< ImageType >    ImageSeriesReaderType;
  typedef itk::ImageFileWriter< ImageType >      ImageWriterType;

 

public:

  itkGetStringMacro ( WeightUnits );
  itkSetStringMacro ( WeightUnits );

  itkGetStringMacro ( DoseRadioactivityUnits );
  itkSetStringMacro ( DoseRadioactivityUnits );

  itkGetStringMacro ( TissueRadioactivityUnits );
  itkSetStringMacro ( TissueRadioactivityUnits );
  
  itkGetStringMacro ( PatientName );
  itkSetStringMacro ( PatientName );
  
  itkGetStringMacro ( StudyDate );
  itkSetStringMacro ( StudyDate );
  
  itkGetStringMacro ( RadiopharmaceuticalStartTime );
  itkSetStringMacro ( RadiopharmaceuticalStartTime );

  itkGetStringMacro (DecayCorrection);
  itkSetStringMacro (DecayCorrection);
  
  itkGetStringMacro (DecayFactor );
  itkSetStringMacro (DecayFactor );
    
  itkGetStringMacro (FrameReferenceTime );
  itkSetStringMacro (FrameReferenceTime );

  itkGetStringMacro (RadionuclideHalfLife );
  itkSetStringMacro (RadionuclideHalfLife );

  itkGetStringMacro (SeriesTime );
  itkSetStringMacro (SeriesTime );
  
  itkGetStringMacro (PhilipsSUVFactor );
  itkSetStringMacro (PhilipsSUVFactor );
  
  itkGetStringMacro (CalibrationFactor );
  itkSetStringMacro (CalibrationFactor );

  itkGetStringMacro (VolumeUnits );
  itkSetStringMacro (VolumeUnits );

  itkGetMacro ( PETMin, double );
  itkSetMacro ( PETMin, double );

  itkGetMacro ( PETMax, double );
  itkSetMacro ( PETMax, double );

  itkGetMacro ( PETSUVmax, double );
  itkSetMacro ( PETSUVmax, double );

  itkGetMacro ( InjectedDose, double );
  itkSetMacro ( InjectedDose, double );

  itkGetMacro ( PatientWeight, double );
  itkSetMacro ( PatientWeight, double );

  itkGetMacro( StudyDateYear, int );
  itkSetMacro( StudyDateYear, int );

  itkGetMacro( StudyDateMonth, int );
  itkSetMacro( StudyDateMonth, int );

  itkGetMacro( StudyDateDay, int );
  itkSetMacro( StudyDateDay, int );

  void ClearStudyDate();
  
  itkGetStringMacro (MetaImageFilename );
  itkSetStringMacro (MetaImageFilename );


  /** Type for representing the string of the directory 
   *  from which the DICOM files will be read. */
  typedef std::string    DirectoryNameType;
  
  /** Method to pass the directory name containing the DICOM image data */
  void SetDirectory( const DirectoryNameType & directory );

  /** This method request image read */
  void ReadImage();

  /** Retrieve parameters from the PET DICOM header */
  int GetParametersFromDicomHeader();

  /** Write out the PET dicom image in a meta image format */
  void ConvertDICOMDataToMetaImage();

protected:

  PETDicomImageReader2( void );
  ~PETDicomImageReader2( void );

    /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:
  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  PETDicomImageReader2(const Self&);    //purposely not implemented
  void operator=(const Self&);      //purposely not implemented

  /** Helper classes for the image series reader */
  itk::GDCMSeriesFileNames::Pointer     m_FileNames;
  itk::GDCMImageIO::Pointer             m_ImageIO;

  typename ImageSeriesReaderType::Pointer        m_ImageSeriesReader;
  typename ImageWriterType::Pointer              m_ImageFileWriter;


  DirectoryNameType            m_ImageDirectoryName;

  std::string m_WeightUnits;
  std::string m_DoseRadioactivityUnits;
  std::string m_TissueRadioactivityUnits;
  std::string m_PatientName;
  std::string m_StudyDate;
  std::string m_RadiopharmaceuticalStartTime;
  std::string m_DecayCorrection;
  std::string m_DecayFactor;
  std::string m_FrameReferenceTime;
  std::string m_RadionuclideHalfLife;
  std::string m_SeriesTime;
  std::string m_PhilipsSUVFactor;
  std::string m_CalibrationFactor;
  std::string m_VolumeUnits;
  
  double m_PETMin;
  double m_PETMax;
  double m_PETSUVmax;
  double m_PETSUVmin;
  double m_InjectedDose;
  double m_PatientWeight;

  int m_StudyDateDay;
  int m_StudyDateMonth;
  int m_StudyDateYear;
    

  bool m_ValidDicomDirectory;
  bool m_DicomDataRead;

  std::string  m_MetaImageFilename;
};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "itkPETDicomImageReader2.txx"
#endif

#endif // __itkPETDicomImageReader2_h
