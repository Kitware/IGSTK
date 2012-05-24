/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoFrameSpatialObject.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkVideoFrameSpatialObject_txx
#define __igstkVideoFrameSpatialObject_txx

#include "igstkVideoFrameSpatialObject.h"

namespace igstk
{

template< class TPixelType, unsigned int TChannels >
VideoFrameSpatialObject< TPixelType, TChannels >
::VideoFrameSpatialObject() :
  m_StateMachine(this)
{
  igstkLogMacro( DEBUG, "VideoFrameSpatialObject Constructor called ....\n" );

  VideoFrameSpatialObjectType::Pointer dummy
                                           = VideoFrameSpatialObjectType::New();
  this->RequestSetInternalSpatialObject(dummy);

  m_Width=0;
  m_Height=0;

  this->m_VideoImagerTool=NULL;
  m_PixelSizeX = 0;
  m_PixelSizeY = 0;
  m_NumberOfScalarComponents = 0;

  if( m_NumberOfChannels == 3 )
  {
    m_ItkRGBExporter = ITKRGBExportFilterType::New();
    m_VtkRGBImporter = VTKImportFilterType::New();

    /** Connect the given itk::VTKImageExport filter to the given
      * vtkImageImport filter */
    m_VtkRGBImporter->SetUpdateInformationCallback(
                              m_ItkRGBExporter->GetUpdateInformationCallback());
    m_VtkRGBImporter->SetPipelineModifiedCallback(
                               m_ItkRGBExporter->GetPipelineModifiedCallback());
    m_VtkRGBImporter->SetWholeExtentCallback(
                                    m_ItkRGBExporter->GetWholeExtentCallback());
    m_VtkRGBImporter->SetSpacingCallback(
                                        m_ItkRGBExporter->GetSpacingCallback());
    m_VtkRGBImporter->SetOriginCallback(m_ItkRGBExporter->GetOriginCallback());
    m_VtkRGBImporter->SetScalarTypeCallback(
                                     m_ItkRGBExporter->GetScalarTypeCallback());
    m_VtkRGBImporter->SetNumberOfComponentsCallback(
                             m_ItkRGBExporter->GetNumberOfComponentsCallback());
    m_VtkRGBImporter->SetPropagateUpdateExtentCallback(
                          m_ItkRGBExporter->GetPropagateUpdateExtentCallback());
    m_VtkRGBImporter->SetUpdateDataCallback(
                                     m_ItkRGBExporter->GetUpdateDataCallback());
    m_VtkRGBImporter->SetDataExtentCallback(
                                     m_ItkRGBExporter->GetDataExtentCallback());
    m_VtkRGBImporter->SetBufferPointerCallback(
                                  m_ItkRGBExporter->GetBufferPointerCallback());
    m_VtkRGBImporter->SetCallbackUserData(
                                       m_ItkRGBExporter->GetCallbackUserData());
    }
  else if(m_NumberOfChannels == 1)
    {
    m_ItkExporter = ITKExportFilterType::New();
    m_VtkImporter = VTKImportFilterType::New();

    m_VtkImporter->SetUpdateInformationCallback(
                                 m_ItkExporter->GetUpdateInformationCallback());
    m_VtkImporter->SetPipelineModifiedCallback(
                                  m_ItkExporter->GetPipelineModifiedCallback());
    m_VtkImporter->SetWholeExtentCallback(
                                       m_ItkExporter->GetWholeExtentCallback());
    m_VtkImporter->SetSpacingCallback(m_ItkExporter->GetSpacingCallback());
    m_VtkImporter->SetOriginCallback(m_ItkExporter->GetOriginCallback());
    m_VtkImporter->SetScalarTypeCallback(
                                        m_ItkExporter->GetScalarTypeCallback());
    m_VtkImporter->SetNumberOfComponentsCallback(
                                m_ItkExporter->GetNumberOfComponentsCallback());
    m_VtkImporter->SetPropagateUpdateExtentCallback(
                             m_ItkExporter->GetPropagateUpdateExtentCallback());
    m_VtkImporter->SetUpdateDataCallback(
                                        m_ItkExporter->GetUpdateDataCallback());
    m_VtkImporter->SetDataExtentCallback(
                                        m_ItkExporter->GetDataExtentCallback());
    m_VtkImporter->SetBufferPointerCallback(
                                     m_ItkExporter->GetBufferPointerCallback());
    m_VtkImporter->SetCallbackUserData(m_ItkExporter->GetCallbackUserData());
    }
  else
    {
    igstkLogMacro( DEBUG, "VideoFrameSpatialObject::Constructor called "
            "with wrong channel number. Only 1 (grayscale) and 3 (RGB)"
            "are allowed! \n" );
    }

  m_StateMachine.SetReadyToRun();
}

template< class TPixelType, unsigned int TChannels >
VideoFrameSpatialObject< TPixelType, TChannels>
::~VideoFrameSpatialObject()
{
  igstkLogMacro( DEBUG, "VideoFrameSpatialObject Destructor called ....\n" );

  if( m_NumberOfChannels == 3 )
    {
    if( m_VtkRGBImporter )
      {
      m_VtkRGBImporter->Delete();
      m_VtkRGBImporter = NULL;
      }

    if( m_RGBPixelContainer )
      {
      delete [] m_RGBPixelContainer;
      m_RGBPixelContainer = NULL;
      }
    }
  else if( m_NumberOfChannels == 1 )
    {
    if( m_VtkImporter )
      {
      m_VtkImporter->Delete();
      }
    }
}

template< class TPixelType, unsigned int TChannels >
void
VideoFrameSpatialObject< TPixelType, TChannels>
::Initialize()
{
  igstkLogMacro( DEBUG, "VideoFrameSpatialObject::Initialize called ....\n" );

  m_Size[0]  = m_Width;  // size along X
  m_Size[1]  = m_Height;  // size along Y

  m_Start.Fill( 0 );

  m_Region.SetIndex( m_Start );
  m_Region.SetSize(  m_Size  );

  double origin[ DIMENSION ];
  origin[0] = 0.0;    // X coordinate
  origin[1] = 0.0;    // Y coordinate

  double spacing[ DIMENSION ];
  spacing[0] = 1.0;    // along X direction
  spacing[1] = 1.0;    // along Y direction

  if( m_NumberOfChannels == 3 )
  {
    m_RGBPixelContainer = new RGBPixelType[m_Width * m_Height];//

    m_RawBuffer = (TPixelType*)malloc(m_Width * m_Height * 3);
    for(unsigned int i = 0; i< m_Width * m_Height * 3; i++)
    {
    if(i%2==0)
      m_RawBuffer[i]='h';
    else
      m_RawBuffer[i]='a';
    }

    m_RGBImportFilter = RGBImportFilterType::New();

    m_RGBImportFilter->SetRegion( m_Region );

    m_RGBImportFilter->SetOrigin( origin );

    m_RGBImportFilter->SetSpacing( spacing );

    int j=0;
    for( unsigned int i=0; i < m_Width * m_Height * 3; i+=3 )
      {
      RGBPixelType temp;
      temp[0]=m_RawBuffer[i];
      temp[1]=m_RawBuffer[i+1];
      temp[2]=m_RawBuffer[i+2];
      m_RGBPixelContainer[j]=temp;
      j++;
      }
    m_RGBImportFilter->SetImportPointer(m_RGBPixelContainer,
                                        m_Width * m_Height,
                                        false );
    m_RGBImportFilter->Update();

    this->m_RGBImage = m_RGBImportFilter->GetOutput();

    m_ItkRGBExporter->SetInput( m_RGBImportFilter->GetOutput() );
    m_VtkRGBImporter->UpdateWholeExtent();

    m_VTKImage = m_VtkRGBImporter->GetOutput();
    }
  else if (m_NumberOfChannels == 1)
    {
    m_ImportFilter = ImportFilterType::New();

    m_ImportFilter->SetRegion( m_Region );

    m_ImportFilter->SetOrigin( origin );

    m_ImportFilter->SetSpacing( spacing );

    m_ImportFilter->SetImportPointer(m_RawBuffer,
                      m_Width * m_Height,
                                        false);
    m_ImportFilter->Update();

    this->m_Image = m_ImportFilter->GetOutput();

    m_ItkExporter->SetInput( m_ImportFilter->GetOutput() );
    m_VtkImporter->UpdateWholeExtent();

    m_VTKImage = m_VtkImporter->GetOutput();
    }
  else
    {
    igstkLogMacro( DEBUG, "VideoFrameSpatialObject::Initialize called "
      "with wrong channel number. Only 1 (grayscale) and 3 (RGB)"
      "are allowed! \n" );
    }
}

template< class TPixelType, unsigned int TChannels >
unsigned int
VideoFrameSpatialObject< TPixelType, TChannels>
::GetNumberOfChannels() const
{
  return m_NumberOfChannels;
}

template< class TPixelType, unsigned int TChannels >
bool
VideoFrameSpatialObject< TPixelType, TChannels>
::IsEmpty() const
{
  igstkLogMacro( DEBUG, "VideoFrameSpatialObject::IsEmpty called ....\n" );

  if( m_NumberOfChannels == 3 )
    {
    if(m_RGBImage.IsNull())
      {
      return true;
      }
    else
      {
      typename RGBImportFilterType::RegionType    tmpregion;
      tmpregion = m_RGBImage->GetLargestPossibleRegion();
      const unsigned int numberOfPixels = tmpregion.GetNumberOfPixels();
      const bool isEmpty = ( numberOfPixels == 0 );
      return isEmpty;
      }
    }
  else if(m_NumberOfChannels == 1)
    {
    if(m_Image.IsNull())
      {
      return true;
      }
    else
      {
      typename ImportFilterType::RegionType     tmpregion;
      tmpregion = m_Image->GetLargestPossibleRegion();
      const unsigned int numberOfPixels = tmpregion.GetNumberOfPixels();
      const bool isEmpty = ( numberOfPixels == 0 );
      return isEmpty;
      }
    }
  else
    {
    igstkLogMacro( DEBUG, "VideoFrameSpatialObject::UpdateImages called "
            "with wrong channel number. Only 1 (grayscale) and 3 (RGB)"
            "are allowed! \n" );
    }

}

template< class TPixelType, unsigned int TChannels >
TimeStamp::TimePeriodType
VideoFrameSpatialObject< TPixelType, TChannels >
::GetFrameExpirationTime() const
{
  if(this->m_VideoImagerTool.IsNotNull())
    {
    return (m_VideoImagerTool->GetInternalFrame())->GetExpirationTime();
    }
  else
  return igstk::TimeStamp::GetZeroValue();
}

template< class TPixelType, unsigned int TChannels >
TimeStamp::TimePeriodType
VideoFrameSpatialObject< TPixelType, TChannels >
::GetFrameStartTime() const
{
  if(this->m_VideoImagerTool.IsNotNull())
    {
    return (m_VideoImagerTool->GetInternalFrame())->GetStartTime();
    }
  else
  return igstk::TimeStamp::GetLongestPossibleTime();
}

template< class TPixelType, unsigned int TChannels >
void
VideoFrameSpatialObject< TPixelType, TChannels >
::SetVideoImagerTool(igstk::VideoImagerTool::Pointer VideoImagerTool)
{
  this->m_VideoImagerTool = VideoImagerTool;
}

template< class TPixelType, unsigned int TChannels >
void
VideoFrameSpatialObject< TPixelType, TChannels >
::RequestGetITKImage()
{
  igstkLogMacro( DEBUG,
                 "VideoFrameSpatialObject::RequestGetITKImage() called ....\n");

  this->UpdateImages();

  if( m_NumberOfChannels == 3 )
    {
    ITKRGBImageModifiedEvent  event;
    event.Set( this->m_RGBImage );
    this->InvokeEvent( event );
    }
  else if(m_NumberOfChannels == 1)
    {
    ITKImageModifiedEvent  event;
    event.Set( this->m_Image );
    this->InvokeEvent( event );
    }
  else
    {
    igstkLogMacro( DEBUG, "VideoFrameSpatialObject::UpdateImages called "
            "with wrong channel number. Only 1 (grayscale) and 3 (RGB)"
            "are allowed! \n" );
    }
}

template< class TPixelType, unsigned int TChannels >
void
VideoFrameSpatialObject< TPixelType, TChannels >
::RequestGetVTKImage()  const
{
  igstkLogMacro( DEBUG,
                 "VideoFrameSpatialObject::RequestGetVTKImage() called ....\n");

  Self * self = const_cast< Self * >( this );
  self->UpdateImages();

  VTKImageModifiedEvent  vtkImageLoadedEvent;
  vtkImageLoadedEvent.Set( m_VTKImage );
  this->InvokeEvent( vtkImageLoadedEvent );
}

template< class TPixelType, unsigned int TChannels >
void
VideoFrameSpatialObject< TPixelType, TChannels>
::UpdateImages()
{
  if(this->m_VideoImagerTool.IsNotNull())
    {
    m_RawBuffer=(unsigned char*)
               (m_VideoImagerTool->GetTemporalCalibratedFrame())->GetImagePtr();
    }
  else
    {
    igstkLogMacro( DEBUG, "VideoFrameSpatialObject::UpdateImages():"
                                    << "VideoImagerTool is not set properly\n");
    }

  if( m_NumberOfChannels == 3 )
    {
    int j=0;
    for( unsigned int i=0; i < m_Width * m_Height * 3; i+=3 )
      {
      RGBPixelType temp;
      temp[0]=m_RawBuffer[i];
      temp[1]=m_RawBuffer[i+1];
      temp[2]=m_RawBuffer[i+2];
      m_RGBPixelContainer[j]=temp;
      j++;
      }
    m_RGBImportFilter->SetImportPointer(m_RGBPixelContainer,
                                        m_Width * m_Height,
                                        false );
    m_RGBImportFilter->Update();
    this->m_RGBImage = m_RGBImportFilter->GetOutput();

    m_ItkRGBExporter->SetInput( m_RGBImportFilter->GetOutput() );
    m_VtkRGBImporter->UpdateWholeExtent();
    m_VtkRGBImporter->Update();

    m_VTKImage = m_VtkRGBImporter->GetOutput();

    }
  else if(m_NumberOfChannels == 1)
    {
    m_ImportFilter->SetImportPointer(m_RawBuffer,
                    m_Width * m_Height,
                                      false);
    m_ImportFilter->Update();

    this->m_Image = m_ImportFilter->GetOutput();

    m_ItkExporter->SetInput( m_ImportFilter->GetOutput() );
    m_VtkImporter->UpdateWholeExtent();

    m_VTKImage = m_VtkImporter->GetOutput();
    }
  else
    {
    igstkLogMacro( DEBUG, "VideoFrameSpatialObject::UpdateImages called "
          "with wrong channel number. Only 1 (grayscale) and 3 (RGB)"
          "are allowed! \n" );
    }
}

template< class TPixelType, unsigned int TChannels >
TPixelType*
VideoFrameSpatialObject< TPixelType, TChannels>
::GetImagePtr()
{
  igstkLogMacro( DEBUG, "VideoFrameSpatialObject::GetImagePtr called ....\n");

  return m_RawBuffer;
}

} // end namespace igstk

#endif
