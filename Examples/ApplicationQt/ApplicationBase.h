

#ifndef _APPLICATIONBASE_H_ 
#define _APPLICATIONBASE_H_

#include "itkImage.h"
#include "itkImageToVTKImageFilter.h"

#include "DicomImageReader.h"

#include <vector>

class ApplicationBase  
{
public:

	typedef signed short PixelType;

	typedef itk::Image< PixelType, 3 > ImageType;

	typedef ISIS::DicomImageReader< ImageType >	DicomReaderType;
  
	typedef itk::ImageToVTKImageFilter< ImageType >	ITK2VTKAdaptorType;

  typedef itk::DICOMSeriesFileNames SeriesFileNamesType;

public:

	ImageType::Pointer	m_ImageData;

  DicomReaderType m_DicomReader;

	ITK2VTKAdaptorType::Pointer	m_ITK2VTKAdaptor;

  SeriesFileNamesType::Pointer  m_SeriesFileNames;

public:
	
	ApplicationBase();

	virtual ~ApplicationBase();

	void LoadImageSeries( std::string seriesUID );

  std::string GetFirstSeriesUID( std::string dir );

  vtkImageData* GetVTKInput();

};

#endif 
