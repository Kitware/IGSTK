/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoFrameSpatialObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkVideoFrameSpatialObject_h
#define __igstkVideoFrameSpatialObject_h

#include "igstkMacros.h"
#include "igstkSpatialObject.h"
#include "igstkFrame.h"
#include "igstkTimeStamp.h"
#include "igstkVideoImagerTool.h"

#include "itkVTKImageExport.h"
#include "itkImage.h"
#include "itkSize.h"
#include "itkImageRegion.h"
#include "itkIndex.h"
#include "itkImportImageFilter.h"
#include "itkRGBPixel.h"

#include "vtkImageData.h"
#include "vtkImageImport.h"

#define DIMENSION 2

namespace igstk
{

template < class TPixelType, unsigned int TChannels >
class VideoFrameSpatialObject
: public SpatialObject
{

public:
  /** Macro with standard traits declarations. */
  igstkStandardTemplatedClassTraitsMacro( VideoFrameSpatialObject,
                                          SpatialObject )

  typedef itk::SpatialObject<3>          VideoFrameSpatialObjectType;

  typedef typename itk::RGBPixel< TPixelType >      RGBPixelType;
  typedef typename itk::ImportImageFilter< RGBPixelType, DIMENSION >
                                                            RGBImportFilterType;
  typedef typename itk::Image< RGBPixelType , DIMENSION >   RGBImageType;

  typedef TPixelType              PixelType;
  typedef typename itk::ImportImageFilter< PixelType, DIMENSION >
                                                               ImportFilterType;
  typedef typename itk::Image< PixelType , DIMENSION >         ImageType;

  typedef Frame                                FrameType;
  typedef typename TimeStamp::TimePeriodType   TimePeriodType;

  itkStaticConstMacro( m_NumberOfChannels, unsigned int, TChannels  );

  void Initialize();

  igstkLoadedTemplatedConstObjectEventMacro( ITKImageModifiedEvent,
                                                         IGSTKEvent, ImageType);

  igstkLoadedTemplatedConstObjectEventMacro( ITKRGBImageModifiedEvent,
                                                      IGSTKEvent, RGBImageType);

  /** Returns wheter the image has any pixels allocated or not */
  virtual bool IsEmpty() const;

  vtkImageData* GetImageData();
  TimeStamp::TimePeriodType GetFrameExpirationTime() const;
  TimeStamp::TimePeriodType GetFrameStartTime() const;

  igstkSetMacro(Width, unsigned int);
  igstkGetMacro(Width, unsigned int);

  igstkSetMacro(Height, unsigned int);
  igstkGetMacro(Height, unsigned int);

  igstkSetMacro(NumberOfScalarComponents, unsigned int);
  igstkGetMacro(NumberOfScalarComponents, unsigned int);

  //igstkGetMacro(NumberOfChannels, unsigned int);
  unsigned int GetNumberOfChannels() const;

  igstkSetMacro(PixelSizeX, double);
  igstkGetMacro(PixelSizeX, double);

  igstkSetMacro(PixelSizeY, double);
  igstkGetMacro(PixelSizeY, double);

  void RequestGetITKImage();
  void RequestGetVTKImage()const;
  void SetVideoImagerTool(igstk::VideoImagerTool::Pointer);


  void UpdateImages();
  TPixelType* GetImagePtr();

protected:
  VideoFrameSpatialObject( void );
  ~VideoFrameSpatialObject( void );

private:

  typename RGBImageType::Pointer        m_RGBImage;
  typename RGBImportFilterType::Pointer m_RGBImportFilter;

  typename ImageType::Pointer         m_Image;
  typename ImportFilterType::Pointer  m_ImportFilter;

  itk::Size<DIMENSION>              m_Size;
  itk::ImageRegion<DIMENSION>       m_Region;
  itk::Index<DIMENSION>             m_Start;

  vtkImageData*       m_VTKImage;
  TPixelType *        m_RawBuffer;

  VTKImageModifiedEvent  m_VtkImageLoadedEvent;

  igstk::VideoImagerTool::Pointer m_VideoImagerTool;
  /** raw frame for the spatial object */
  FrameType                       m_Frame;

  unsigned int              m_Width;
  unsigned int              m_Height;
  double                    m_PixelSizeX;
  double                    m_PixelSizeY;
  RGBPixelType*             m_RGBPixelContainer;

  unsigned int              m_NumberOfScalarComponents;

  /** Filters for exporting the ITK image as a vtkImageData class. */
  typedef itk::VTKImageExport< RGBImageType >      ITKRGBExportFilterType;
  typedef typename ITKRGBExportFilterType::Pointer ITKRGBExportFilterPointer;

  typedef itk::VTKImageExport< ImageType >      ITKExportFilterType;
  typedef typename ITKExportFilterType::Pointer ITKExportFilterPointer;

  typedef vtkImageImport             VTKImportFilterType;
  typedef VTKImportFilterType*       VTKImportFilterPointer;

  /** Classes to connect an ITK pipeline to a VTK pipeline */
  ITKRGBExportFilterPointer          m_ItkRGBExporter;
  ITKExportFilterPointer             m_ItkExporter;

  VTKImportFilterPointer             m_VtkRGBImporter;
  VTKImportFilterPointer             m_VtkImporter;
};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkVideoFrameSpatialObject.txx"
#endif

#endif // __igstkVideoFrameSpatialObject_h
