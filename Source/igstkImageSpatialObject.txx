/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageSpatialObject.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

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
  this->RequestSetInternalSpatialObject( m_ImageSpatialObject );

  // initialize the logger 
  m_Logger = NULL;

  m_ItkExporter = ITKExportFilterType::New();
  m_VtkImporter = VTKImportFilterType::New();
  
  // Connect the given itk::VTKImageExport filter to
  // the given vtkImageImport filter.
 
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
  m_VtkImporter->SetUpdateDataCallback(m_ItkExporter->GetUpdateDataCallback());
  m_VtkImporter->SetDataExtentCallback(m_ItkExporter->GetDataExtentCallback());
  m_VtkImporter->SetBufferPointerCallback(
                                    m_ItkExporter->GetBufferPointerCallback());
  m_VtkImporter->SetCallbackUserData(m_ItkExporter->GetCallbackUserData());

  igstkAddInputMacro( ValidImage );
  igstkAddInputMacro( InvalidImage );
  igstkAddInputMacro( RequestITKImage );
  igstkAddInputMacro( RequestVTKImage );
  igstkAddInputMacro( RequestImageExtent );
  igstkAddInputMacro( RequestImageTransform );

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
  igstkAddTransitionMacro( Initial, RequestImageTransform, 
                           Initial, ReportImageNotAvailable );
  igstkAddTransitionMacro( Initial, RequestImageExtent, 
                           Initial, ReportImageNotAvailable );

  igstkAddTransitionMacro( ImageSet, ValidImage, 
                           ImageSet, SetImage );
  igstkAddTransitionMacro( ImageSet, InvalidImage, 
                           Initial,  ReportInvalidImage );
  igstkAddTransitionMacro( ImageSet, RequestITKImage, 
                           ImageSet, ReportITKImage );
  igstkAddTransitionMacro( ImageSet, RequestVTKImage, 
                           ImageSet, ReportVTKImage );
  igstkAddTransitionMacro( ImageSet, RequestImageTransform, 
                           ImageSet, ReportImageTransform );
  igstkAddTransitionMacro( ImageSet, RequestImageExtent, 
                           ImageSet, ReportImageExtent );

  igstkSetInitialStateMacro( Initial );

  this->m_StateMachine.SetReadyToRun();

  this->m_DICOMCoordinateSystem = CoordinateSystem::New();
}


/** Destructor */
template< class TPixelType, unsigned int VDimension >
ImageSpatialObject< TPixelType, VDimension >
::~ImageSpatialObject()  
{
  if( m_VtkImporter )
    {
    m_VtkImporter->Delete(); 
    m_VtkImporter = NULL;
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
::RequestGetImageTransform() const
{
  igstkLogMacro( DEBUG, "RequestGetImageTransform() called ....\n");
  Self * self = const_cast< Self * >( this );
  self->RequestGetImageTransform();
}


template< class TPixelType, unsigned int VDimension >
void
ImageSpatialObject< TPixelType, VDimension >
::RequestGetImageTransform() 
{
  igstkLogMacro( DEBUG, "RequestImageTransform() called ....\n");

  igstkPushInputMacro( RequestImageTransform );
  this->m_StateMachine.ProcessInputs();
}

template< class TPixelType, unsigned int VDimension >
void
ImageSpatialObject< TPixelType, VDimension >
::RequestGetImageExtent() const
{
  igstkLogMacro( DEBUG, "RequestGetImageExtent() called ....\n");
  Self * self = const_cast< Self * >( this );
  self->RequestGetImageExtent();
}


template< class TPixelType, unsigned int VDimension >
void
ImageSpatialObject< TPixelType, VDimension >
::RequestGetImageExtent() 
{
  igstkLogMacro( DEBUG, "RequestGetImageExtent() called ....\n");

  igstkPushInputMacro( RequestImageExtent );
  this->m_StateMachine.ProcessInputs();
}

template< class TPixelType, unsigned int VDimension >
void
ImageSpatialObject< TPixelType, VDimension >
::ReportImageTransformProcessing() 
{
  igstkLogMacro( DEBUG, "ReportImageTransformProcessing() called ....\n");
  CoordinateSystemTransformToResult transformCarrier;
  CoordinateSystemTransformToEvent transformEvent;

  transformCarrier.Initialize( this->m_ImageTransform,
    this->m_DICOMCoordinateSystem,
    this->GetCoordinateSystem() );

  transformEvent.Set( transformCarrier );
  this->InvokeEvent( transformEvent );
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
  event.Set( m_VtkImporter->GetOutput() );
  this->InvokeEvent( event );
}

template< class TPixelType, unsigned int VDimension >
void
ImageSpatialObject< TPixelType, VDimension >
::ReportImageExtentProcessing() 
{
  igstkLogMacro( DEBUG, "ReportImageExtentProcessing() called ....\n");

  int extent[6];
  m_VtkImporter->GetWholeExtent(extent);

  EventHelperType::ImageExtentType imageExtent;
  imageExtent.xmin = extent[0];
  imageExtent.xmax = extent[1];
  imageExtent.ymin = extent[2];
  imageExtent.ymax = extent[3];
  imageExtent.zmin = extent[4];
  imageExtent.zmax = extent[5];
 
  ImageExtentEvent event;
  event.Set( imageExtent );
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
 
  typename ImageType::DirectionType    directionCosines;
  directionCosines = m_Image->GetDirection();

  // Due to insufficient precision of the direction cosine representation
  // in the DICOM tag (0020,0037) with three decimal positions, the direction
  // cosine matrix may not represent a valid rotation matrix. Therefore, SVD
  // is used to get the nearest valid rotation matrix.

  // Compute singular value decomposition
  vnl_svd<double> svd (directionCosines.GetVnlMatrix());

  // Replace singular value matrix with identity
  svd.W(0,0) = 1;
  svd.W(1,1) = 1;
  svd.W(2,2) = 1;
  typename ImageType::DirectionType    directionCosineRotationMatrix(svd.recompose());

  typedef typename ImageType::PointType     OriginType;  
  OriginType origin =  m_Image->GetOrigin();

  Transform          transform;
  typename Transform::VersorType                    rotation;
  
  rotation.Set( directionCosineRotationMatrix );

  typename Transform::VectorType   tranlationToOrigin = 
                 origin - rotation.Transform( origin );
 
  typename Transform::ErrorType      errorValue = 1e-20;
  typename Transform::TimePeriodType validtyTime = 
    TimeStamp::GetLongestPossibleTime();
  
  m_ImageTransform.SetTranslationAndRotation( tranlationToOrigin, rotation, 
                                           errorValue, validtyTime );
 
  m_ItkExporter->SetInput( m_Image );
  m_VtkImporter->UpdateWholeExtent();
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
void
ImageSpatialObject< TPixelType, VDimension >
::TransformIndexToPhysicalPoint ( const IndexType & index, 
                                        PointType & point ) const 
{ 
  m_Image->TransformIndexToPhysicalPoint( index, point);
}


template< class TPixelType, unsigned int VDimension >
bool
ImageSpatialObject< TPixelType, VDimension >
::TransformPhysicalPointToIndex ( const PointType & point, 
                                        IndexType & index ) const 
{ 
  ContinuousIndexType cindex;
  bool isInside = 
             m_Image->TransformPhysicalPointToContinuousIndex( point, cindex);
  // Do the right rounding
  index[0] = int ( cindex[0] + 0.5 );
  index[1] = int ( cindex[1] + 0.5 );
  index[2] = int ( cindex[2] + 0.5 );
  return isInside;
}

template< class TPixelType, unsigned int VDimension >
bool
ImageSpatialObject< TPixelType, VDimension >
::TransformPhysicalPointToContinuousIndex ( const PointType & point, 
                                        ContinuousIndexType & index ) const 
{ 
  return m_Image->TransformPhysicalPointToContinuousIndex( point, index);  
}

/** Print Self function */
template< class TPixelType, unsigned int VDimension >
void 
ImageSpatialObject< TPixelType, VDimension >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << "Details of the image " << m_Image.GetPointer() << std::endl;
  os << "ITK Exporter filter " << m_ItkExporter.GetPointer() << std::endl;
  os << "VTK Importer filter " << m_VtkImporter << std::endl;
}

/** For coordinate systems, this method lets us indicate that 
 *  we need to provide an additional transform. 
 */
template< class TPixelType, unsigned int VDimension >
bool
ImageSpatialObject< TPixelType, VDimension >
::IsInternalTransformRequired()
{ 
  return true;
} 

/** For coordinate systems, allows us to hook in the image transform to
 *  calls to RequestSetTransformAndParent.
 */
template< class TPixelType, unsigned int VDimension >
Transform
ImageSpatialObject< TPixelType, VDimension >
::GetInternalTransform() const
{ 
  return m_ImageTransform;
}


} // end namespace igstk

#endif
