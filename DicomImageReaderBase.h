
#ifndef __ISIS_DicomImageReaderBase_h__
#define __ISIS_DicomImageReaderBase_h__

#include "itkImageSeriesReader.h"
#include "itkDICOMSeriesFileNames.h"


namespace ISIS
{

class DicomImageReaderBase
{

  typedef itk::DICOMSeriesFileNames       SeriesFileNamesType;
  typedef SeriesFileNamesType::Pointer    SeriesFileNamesPointer;


public:
  DicomImageReaderBase( void );

  virtual ~DicomImageReaderBase( void );

  void SetDirectory( const char * directory );

  const std::vector<std::string> &GetSeriesUIDs();


protected:

  SeriesFileNamesPointer    m_FilenamesGenerator;

};


}  // end namespace ISIS

  

#endif



