/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkGenericImageSpatialObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkGenericImageSpatialObject.h"
#include "igstkImageSpatialObject.h"

//#include "itkOrientedImage.h"
#include "itkVTKImageExport.h"

#include "vtkImageImport.h"
#include "vtkImageData.h"


namespace igstk
{ 

/** \class PipelineCreator
 *  This helper class will take care of instantiating the appropriate
 *  ITK Export class corresponding to the actual pixel type of the 
 *  input image. */
template <class TPixel >
class PipelineCreator
{
public:
  
  typedef itk::ImageBase<3>                   ImageBaseType;
  typedef ImageBaseType::ConstPointer         ImageBasePointer;
  typedef itk::ProcessObject                  ExporterBaseType;
  typedef itk::ProcessObject::Pointer         ExporterBasePointer;
  typedef itk::Image< TPixel, 3 >             ImageType;

  typedef itk::SpatialObject< 3 >                   SpatialObjectBaseType;
  typedef typename SpatialObjectBaseType::Pointer   SpatialObjectBasePointer;
  typedef itk::ImageSpatialObject< 3, TPixel >      ImageSpatialObjectType;
  typedef typename ImageSpatialObjectType::Pointer  ImageSpatialObjectPointer;

  static void 
  CreateExporter( ImageBasePointer    & imageBase, 
                  SpatialObjectBasePointer & spatialObject,
                  ExporterBasePointer & exporter,
                  vtkImageImport      * importer  )
    {
    const ImageType * image = 
      dynamic_cast< const ImageType * >( imageBase.GetPointer() );

    if( image )
      {
      typedef itk::VTKImageExport< ImageType >   ExportFilterType;
      typedef typename ExportFilterType::Pointer ExportFilterPointer;

      ExportFilterPointer itkExporter = ExportFilterType::New();
      itkExporter->SetInput( image );

      ImageSpatialObjectPointer imageSpatialObject = 
        ImageSpatialObjectType::New();

      imageSpatialObject->SetImage( image );

      spatialObject = imageSpatialObject;

      exporter = itkExporter;

      importer->SetUpdateInformationCallback(
        itkExporter->GetUpdateInformationCallback());
      importer->SetPipelineModifiedCallback(
        itkExporter->GetPipelineModifiedCallback());
      importer->SetWholeExtentCallback(
        itkExporter->GetWholeExtentCallback());
      importer->SetSpacingCallback(
        itkExporter->GetSpacingCallback());
      importer->SetOriginCallback(
        itkExporter->GetOriginCallback());
      importer->SetScalarTypeCallback(
        itkExporter->GetScalarTypeCallback());
      importer->SetNumberOfComponentsCallback(
        itkExporter->GetNumberOfComponentsCallback());
      importer->SetPropagateUpdateExtentCallback(
        itkExporter->GetPropagateUpdateExtentCallback());
      importer->SetUpdateDataCallback(
        itkExporter->GetUpdateDataCallback());
      importer->SetDataExtentCallback(
        itkExporter->GetDataExtentCallback());
      importer->SetBufferPointerCallback(
        itkExporter->GetBufferPointerCallback());
      importer->SetCallbackUserData(
        itkExporter->GetCallbackUserData());
      }
    }
};


/** This helper macro will instantiate the pipeline creator for a particular
 * pixel type */
#define CreatePipelineMacro( PixelType ) \
  PipelineCreator< PixelType >::CreateExporter( \
      this->m_Image, this->m_GenericSpatialObject, \
      this->m_ItkExporter, this->m_VtkImporter );


/** Constructor */
GenericImageSpatialObject
::GenericImageSpatialObject():m_StateMachine(this)
{
  // initialize the logger 
  m_Logger = NULL;

  this->m_VtkImporter = vtkImageImport::New();

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
GenericImageSpatialObject
::~GenericImageSpatialObject()  
{
  if( this->m_VtkImporter )
    {
    this->m_VtkImporter->Delete(); 
    this->m_VtkImporter = NULL;
    }
}


void
GenericImageSpatialObject
::RequestSetImage( const ImageBaseType * image ) 
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


void
GenericImageSpatialObject
::RequestGetITKImage() const
{
  igstkLogMacro( DEBUG, "RequestGetITKImage() called ....\n");

  //
  // FIXME : Constness issue igstkPushInputMacro( RequestITKImage );
  //
  // This const_cast is allowed here only because 
  // all the transitions due to the RequestVTKImage
  // are idempotent, meaning that they do not change
  // the state of the state machine. Given that the 
  // state of the class is not changed, the method 
  // can still be considered to be const.
  Self * self = const_cast< Self * >( this );
  self->RequestGetITKImage();
}


void
GenericImageSpatialObject
::RequestGetITKImage() 
{
  igstkLogMacro( DEBUG, "RequestGetITKImage() called ....\n");

  igstkPushInputMacro( RequestITKImage );
  this->m_StateMachine.ProcessInputs();
}


void
GenericImageSpatialObject
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


void
GenericImageSpatialObject
::RequestGetVTKImage() 
{
  igstkLogMacro( DEBUG, "RequestGetVTKImage() called ....\n");

  igstkPushInputMacro( RequestVTKImage );
  this->m_StateMachine.ProcessInputs();
}


void
GenericImageSpatialObject
::ReportITKImageProcessing() 
{
  igstkLogMacro( DEBUG, "ReportITKImageProcessing() called ....\n");

  ITKImageModifiedEvent  event;
  event.Set( this->m_Image );
  this->InvokeEvent( event );
}


void
GenericImageSpatialObject
::ReportVTKImageProcessing() 
{
  igstkLogMacro( DEBUG, "ReportVTKImageProcessing() called ....\n");

  VTKImageModifiedEvent  event;
  event.Set( this->m_VtkImporter->GetOutput() );
  this->InvokeEvent( event );
}


void
GenericImageSpatialObject
::ReportImageNotAvailableProcessing() 
{
  ImageNotAvailableEvent  event;
  igstkLogMacro( WARNING, "ReportImageNotAvailableProcessing() called ...\n");
  this->InvokeEvent( event );
}

void
GenericImageSpatialObject
::SetImageProcessing() 
{
  igstkLogMacro( DEBUG, "SetImageProcessing() called ....\n");

  this->m_Image = this->m_ImageToBeSet;

  this->Modified();

  CreatePipelineMacro( unsigned char );
  CreatePipelineMacro( char );
  CreatePipelineMacro( unsigned short );
  CreatePipelineMacro( short );
  CreatePipelineMacro( unsigned int );
  CreatePipelineMacro( int );
  CreatePipelineMacro( unsigned long );
  CreatePipelineMacro( long );
  CreatePipelineMacro( float );
  CreatePipelineMacro( double );

  this->RequestSetInternalSpatialObject( this->m_GenericSpatialObject );

  this->m_VtkImporter->Update();

  // Get direction cosine information from the Oriented image
  // and use the information to setup transformation parameters 
 
  ImageBaseType::DirectionType    directionCosines;
  directionCosines = m_Image->GetDirection();

  PointType origin =  m_Image->GetOrigin();

  Transform          transform;
  Transform::VersorType                    rotation;
  
  rotation.Set( directionCosines );
 

  Transform::VectorType   tranlationToOrigin = 
                 origin - rotation.Transform( origin );
 
  Transform::ErrorType           errorValue = 1e-20;
  Transform::TimePeriodType      validtyTime = -1;
  
  transform.SetTranslationAndRotation( tranlationToOrigin, rotation, 
                                           errorValue, validtyTime );
 
  // FIXME once the API of the internal scene graph is defined
  // this->RequestSetTransformToSpatialObjectParent( transform ); 

  this->m_VtkImporter->UpdateWholeExtent();
}


void
GenericImageSpatialObject
::ReportInvalidImageProcessing() 
{
  igstkLogMacro( WARNING, "ReportInvalidImageProcessing() called ...\n");
}


bool
GenericImageSpatialObject
::IsInside( const PointType & itkNotUsed(point) ) const 
{ 
  //
  // FIXME: Update implementation
  //
  // return m_GenericImageSpatialObject->IsInside( point ); 
  //
  return false; // FIXME: to be removed when the new implementation is in place
}


bool
GenericImageSpatialObject
::IsEmpty() const 
{ 
  typedef ::itk::ImageRegion< 3 > RegionType;
  /* FIXME  update implementation
  RegionType region = 
    m_GenericImageSpatialObject->GetImage()->GetLargestPossibleRegion();
  const unsigned int numberOfPixels = region.GetNumberOfPixels();
  const bool isEmpty = ( numberOfPixels == 0 );
  return isEmpty;
  */
  return true;  // FIXME
}


bool
GenericImageSpatialObject
::TransformPhysicalPointToIndex ( const PointType & itkNotUsed(point), 
                                        IndexType & index ) const 
{ 
  ContinuousIndexType cindex;
  bool isInside = true;
  // FIXME: here there should be a switch statement...
  // bool isInside = 
  // m_Image->TransformPhysicalPointToContinuousIndex( point, cindex);
  // Do the right rounding
  index[0] = int ( cindex[0] + 0.5 );
  index[1] = int ( cindex[1] + 0.5 );
  index[2] = int ( cindex[2] + 0.5 );
  return isInside;
}

bool
GenericImageSpatialObject
::TransformPhysicalPointToContinuousIndex ( const PointType & itkNotUsed(point), 
                                        ContinuousIndexType & itkNotUsed(index) ) const 
{ 
  // FIXME: here there should be a switch statement...
  // return m_Image->TransformPhysicalPointToContinuousIndex( point, index);  
  //
  return false;
}

/** Print Self function */
void 
GenericImageSpatialObject
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << "Details of the image " << m_Image.GetPointer() << std::endl;
  os << "SpatialObjectBase " << m_GenericSpatialObject.GetPointer() << std::endl;
  os << "ITK Exporter filter " << m_ItkExporter.GetPointer() << std::endl;
  os << "VTK Importer filter " << this->m_VtkImporter << std::endl;
}


} // end namespace igstk
