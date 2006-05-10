/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageSpatialObject.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkImageSpatialObject_txx
#define __igstkImageSpatialObject_txx


#include "igstkImageSpatialObject.h"


namespace igstk
{ 

/** Constructor */
template< class TPixelType, unsigned int VDimension >
ImageSpatialObject< TPixelType, VDimension >
::ImageSpatialObject():m_StateMachine(this)
{
  // Create the image spatial object
  m_ImageSpatialObject = ImageSpatialObjectType::New();
  this->RequestSetSpatialObject( m_ImageSpatialObject );

  // initialize the logger 
  m_Logger = NULL;

  m_itkExporter = ITKExportFilterType::New();
  m_vtkImporter = VTKImportFilterType::New();
  
  // Connect the given itk::VTKImageExport filter to
  // the given vtkImageImport filter.
 
  m_vtkImporter->SetUpdateInformationCallback(
                               m_itkExporter->GetUpdateInformationCallback());
  m_vtkImporter->SetPipelineModifiedCallback(
                                m_itkExporter->GetPipelineModifiedCallback());
  m_vtkImporter->SetWholeExtentCallback(
                                m_itkExporter->GetWholeExtentCallback());
  m_vtkImporter->SetSpacingCallback(m_itkExporter->GetSpacingCallback());
  m_vtkImporter->SetOriginCallback(m_itkExporter->GetOriginCallback());
  m_vtkImporter->SetScalarTypeCallback(
                                      m_itkExporter->GetScalarTypeCallback());
  m_vtkImporter->SetNumberOfComponentsCallback(
                              m_itkExporter->GetNumberOfComponentsCallback());
  m_vtkImporter->SetPropagateUpdateExtentCallback(
                           m_itkExporter->GetPropagateUpdateExtentCallback());
  m_vtkImporter->SetUpdateDataCallback(m_itkExporter->GetUpdateDataCallback());
  m_vtkImporter->SetDataExtentCallback(m_itkExporter->GetDataExtentCallback());
  m_vtkImporter->SetBufferPointerCallback(
                                    m_itkExporter->GetBufferPointerCallback());
  m_vtkImporter->SetCallbackUserData(m_itkExporter->GetCallbackUserData());

  igstkAddInputMacro( ValidImage );
  igstkAddInputMacro( InvalidImage );
  igstkAddInputMacro( RequestITKImage );
  igstkAddInputMacro( RequestVTKImage );

  igstkAddStateMacro( Initial );
  igstkAddStateMacro( ImageSet );

  igstkAddTransitionMacro( Initial, ValidImage, 
                           ImageSet,  SetImage );
  igstkAddTransitionMacro( Initial, InvalidImage, 
                           Initial, ReportInvalidImage );
  igstkAddTransitionMacro( Initial, RequestITKImage, 
                           Initial, ReportImageNotAvailable );
  igstkAddTransitionMacro( Initial, RequestVTKImage, 
                           Initial, ReportImageNotAvailable );

  igstkAddTransitionMacro( ImageSet, ValidImage, 
                           ImageSet, SetImage );
  igstkAddTransitionMacro( ImageSet, InvalidImage, 
                           Initial,  ReportInvalidImage );
  igstkAddTransitionMacro( ImageSet, RequestITKImage, 
                           ImageSet, ReportITKImage );
  igstkAddTransitionMacro( ImageSet, RequestVTKImage, 
                           ImageSet, ReportVTKImage );

  igstkSetInitialStateMacro( Initial );

  m_StateMachine.SetReadyToRun();

}


/** Destructor */
template< class TPixelType, unsigned int VDimension >
ImageSpatialObject< TPixelType, VDimension >
::~ImageSpatialObject()  
{
  if( m_vtkImporter )
    {
    m_vtkImporter->Delete(); 
    m_vtkImporter = NULL;
    }
}


template< class TPixelType, unsigned int VDimension >
void
ImageSpatialObject< TPixelType, VDimension >
::RequestSetImage( const ImageType * image ) 
{
  m_ImageToBeSet = image;

  if( m_ImageToBeSet )
    {
    igstkPushInputMacro( ValidImage );
    m_StateMachine.ProcessInputs();
    return;
    }
  else
    {
    igstkPushInputMacro( InvalidImage );
    m_StateMachine.ProcessInputs();
    return;
    }
}


template< class TPixelType, unsigned int VDimension >
void
ImageSpatialObject< TPixelType, VDimension >
::RequestGetITKImage() const
{
  igstkLogMacro( DEBUG, "RequestGetITKImage() called ....\n");

  igstkPushInputMacro( RequestITKImage );
  // This const_cast is allowed here only because 
  // all the transitions due to the RequestVTKImage
  // are idempotent, meaning that they do not change
  // the state of the state machine. Given that the 
  // state of the class is not changed, the method 
  // can still be considered to be const.
  Self * self = const_cast< Self * >( this );
  self->RequestGetITKImage();
}


template< class TPixelType, unsigned int VDimension >
void
ImageSpatialObject< TPixelType, VDimension >
::RequestGetITKImage() 
{
  igstkLogMacro( DEBUG, "RequestGetITKImage() called ....\n");

  igstkPushInputMacro( RequestITKImage );
  this->m_StateMachine.ProcessInputs();
}


template< class TPixelType, unsigned int VDimension >
void
ImageSpatialObject< TPixelType, VDimension >
::RequestGetVTKImage() const
{
  igstkLogMacro( DEBUG, "RequestGetVTKImage() called ....\n");

  // This const_cast is allowed here only because 
  // all the transitions due to the RequestVTKImage
  // are idempotent, meaning that they do not change
  // the state of the state machine. Given that the 
  // state of the class is not changed, the method 
  // can still be considered to be const.
  Self * self = const_cast< Self * >( this );
  self->RequestGetVTKImage();
}


template< class TPixelType, unsigned int VDimension >
void
ImageSpatialObject< TPixelType, VDimension >
::RequestGetVTKImage() 
{
  igstkLogMacro( DEBUG, "RequestGetVTKImage() called ....\n");

  igstkPushInputMacro( RequestVTKImage );
  this->m_StateMachine.ProcessInputs();
}


template< class TPixelType, unsigned int VDimension >
void
ImageSpatialObject< TPixelType, VDimension >
::ReportITKImageProcessing() 
{
  igstkLogMacro( DEBUG, "ReportITKImageProcessing() called ....\n");

  ITKImageModifiedEvent  event;
  event.Set( this->m_Image );
  this->InvokeEvent( event );
}


template< class TPixelType, unsigned int VDimension >
void
ImageSpatialObject< TPixelType, VDimension >
::ReportVTKImageProcessing() 
{
  igstkLogMacro( DEBUG, "ReportVTKImageProcessing() called ....\n");

  VTKImageModifiedEvent  event;
  event.Set( m_vtkImporter->GetOutput() );
  this->InvokeEvent( event );
}


template< class TPixelType, unsigned int VDimension >
void
ImageSpatialObject< TPixelType, VDimension >
::ReportImageNotAvailableProcessing() 
{
  ImageNotAvailableEvent  event;
  igstkLogMacro( WARNING, "ReportImageNotAvailableProcessing() called ...\n");
  this->InvokeEvent( event );
}

template< class TPixelType, unsigned int VDimension >
void
ImageSpatialObject< TPixelType, VDimension >
::SetImageProcessing() 
{
  igstkLogMacro( DEBUG, "SetImageProcessing() called ....\n");

  m_Image = m_ImageToBeSet;
  m_ImageSpatialObject->SetImage( m_Image );

  // Get direction cosine information from the Oriented image
  // and use the information to setup transformation parameters 
 
  /*  
  typename ImageType::DirectionType    directionCosines;
  directionCosines = m_Image->GetDirection();

  std::cout<< "Direction cosine" << directionCosines << std::endl; 
 
  Transform          transform;
  typename Transform::VersorType          rotation;

  rotation.Set( directionCosines );
  
  typename Transform::ErrorType           errorValue = 1e-20;

  typename Transform::TimePeriodType      validtyTime = -1;
  
  transform.SetRotation( rotation, errorValue, validtyTime );  
  
  this->RequestSetTransform( transform ); 
 */

  m_itkExporter->SetInput( m_Image );
  m_vtkImporter->UpdateWholeExtent();
}


template< class TPixelType, unsigned int VDimension >
void
ImageSpatialObject< TPixelType, VDimension >
::ReportInvalidImageProcessing() 
{
  igstkLogMacro( WARNING, "ReportInvalidImageProcessing() called ...\n");
}


template< class TPixelType, unsigned int VDimension >
bool
ImageSpatialObject< TPixelType, VDimension >
::IsInside( const PointType & point ) const 
{ 
  return m_ImageSpatialObject->IsInside( point ); 
}


template< class TPixelType, unsigned int VDimension >
bool
ImageSpatialObject< TPixelType, VDimension >
::IsEmpty() const 
{ 
  typedef ::itk::ImageRegion< VDimension > RegionType;
  RegionType region = 
    m_ImageSpatialObject->GetImage()->GetLargestPossibleRegion();
  const unsigned int numberOfPixels = region.GetNumberOfPixels();
  const bool isEmpty = ( numberOfPixels == 0 );
  return isEmpty;
}


template< class TPixelType, unsigned int VDimension >
bool
ImageSpatialObject< TPixelType, VDimension >
::TransformPhysicalPointToIndex ( const PointType & point, 
                                        IndexType & index ) const 
{ 
  return m_Image->TransformPhysicalPointToIndex( point, index);  
}


/** Print Self function */
template< class TPixelType, unsigned int VDimension >
void 
ImageSpatialObject< TPixelType, VDimension >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << "Details of the image " << m_Image.GetPointer() << std::endl;
  os << "ITK Exporter filter " << m_itkExporter.GetPointer() << std::endl;
  os << "VTK Importer filter " << m_vtkImporter << std::endl;
}


} // end namespace igstk

#endif
