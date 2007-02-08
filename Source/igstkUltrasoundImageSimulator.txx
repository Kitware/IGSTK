/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkUltrasoundImageSimulator.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkUltrasoundImageSimulator_txx
#define __igstkUltrasoundImageSimulator_txx

#include "igstkUltrasoundImageSimulator.h"

#include "vtkImageData.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkCastImageFilter.h"

namespace igstk
{

/** Constructor */

template < class TImageGeometricModel >
UltrasoundImageSimulator< TImageGeometricModel >
::UltrasoundImageSimulator():m_StateMachine(this)
{
  // We create the image spatial object
  m_ImageGeometricModel = NULL;
  m_RescaledUSImage = NULL;

  // Create classes for displaying images
  m_ImageData  = NULL;
  m_ReslicedImageData  = NULL;
  m_VTKImageImporter = VTKImageImporterType::New();

  // Image reslice
  m_ImageReslice = vtkImageReslice::New();
  
  m_VTKImageObserver = VTKImageObserver::New();

  m_USImage = USImageObject::New();

  m_VTKExporter = vtkImageExport::New();


  igstkAddInputMacro( ValidImageSpatialObject );
  igstkAddInputMacro( NullImageSpatialObject  );

  igstkAddInputMacro( ValidTransform );
  igstkAddInputMacro( InvalidTransform );

  igstkAddInputMacro( EmptyImageSpatialObject  );
  igstkAddInputMacro( ConnectVTKPipeline );

  igstkAddInputMacro( Reslice );
  igstkAddInputMacro( GetImage );

  igstkAddStateMacro( NullImageSpatialObject );
  igstkAddStateMacro( ValidImageSpatialObject );

  igstkAddStateMacro( ValidTransform );
  igstkAddStateMacro( NullTransform );

  igstkAddTransitionMacro( NullImageSpatialObject, NullImageSpatialObject, 
                           NullImageSpatialObject,  No );
  igstkAddTransitionMacro( NullImageSpatialObject, EmptyImageSpatialObject,
                           NullImageSpatialObject,  No );
  igstkAddTransitionMacro( NullImageSpatialObject, ValidImageSpatialObject,
                           ValidImageSpatialObject, SetImageGeometricModel );

  igstkAddTransitionMacro( ValidImageSpatialObject,ValidTransform,
                           ValidImageSpatialObject, 
                           SetTransform );
  
  igstkAddTransitionMacro( ValidImageSpatialObject, Reslice, 
                           ValidImageSpatialObject , Reslice);
  igstkAddTransitionMacro( ValidImageSpatialObject, NullImageSpatialObject, 
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, EmptyImageSpatialObject, 
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, ValidImageSpatialObject, 
                           ValidImageSpatialObject,  No ); 

  igstkAddTransitionMacro( ValidImageSpatialObject,
                           GetImage,
                           ValidImageSpatialObject,
                           ReportImage );

  igstkSetInitialStateMacro( NullImageSpatialObject );

  m_StateMachine.SetReadyToRun();

  this->RequestSetImageGeometricModel( m_ImageGeometricModel );

} 

/** Destructor */
template < class TImageGeometricModel >
UltrasoundImageSimulator< TImageGeometricModel >
::~UltrasoundImageSimulator()  
{
  if( m_ImageReslice )
    {
    m_ImageReslice->Delete();
    m_ImageReslice = NULL;
    }

  m_VTKExporter->Delete();
}
 
/** Set the Image Spatial Object */
template < class TImageGeometricModel >
void 
UltrasoundImageSimulator< TImageGeometricModel >
::RequestSetImageGeometricModel( const ImageGeometricModelType * image )
{
  igstkLogMacro( DEBUG, "igstk::UltrasoundImageSimulator\
                        ::RequestSetImageSpatialObject called...\n");
  
  m_ImageGeometricModelToAdd = image;

  if( !m_ImageGeometricModelToAdd )
    {
    m_StateMachine.PushInput( m_NullImageSpatialObjectInput );
    }
  else 
    {
    if( m_ImageGeometricModelToAdd->IsEmpty() )
      {
      m_StateMachine.PushInput( m_EmptyImageSpatialObjectInput );
      }
    else
      {
      m_StateMachine.PushInput( m_ValidImageSpatialObjectInput );
      }
    }
  
  m_StateMachine.ProcessInputs();
}

template <class TImageGeometricModel>
void UltrasoundImageSimulator<TImageGeometricModel>
::RequestGetImage()
{
  igstkLogMacro( DEBUG, 
              "igstk::UltrasoundImageSimulator::RequestGetImage called...\n");
  m_StateMachine.PushInput(m_GetImageInput);
  m_StateMachine.ProcessInputs();
}

/** This function reports the image */
template <class TImageGeometricModel>
const typename UltrasoundImageSimulator<TImageGeometricModel>::USImageType * 
UltrasoundImageSimulator<TImageGeometricModel>
::GetITKImage() const
{
  return m_RescaledUSImage;
}


/** This function reports the image */
template <class TImageGeometricModel>
void 
UltrasoundImageSimulator<TImageGeometricModel>
::ReportImageProcessing()
{
  ImageModifiedEvent  event;
  event.Set( this->m_USImage );
  this->InvokeEvent( event );
}


template < class TImageGeometricModel >
void 
UltrasoundImageSimulator< TImageGeometricModel >
::RequestSetTransform( const TransformType & transform )
{
  igstkLogMacro( DEBUG, "igstk::UltrasoundImageSimulator::\
                                 RequestSetTransform called...\n");
  
  m_TransformToBeSet = transform;

  // check if it is a valid transform
  m_StateMachine.PushInput( m_ValidTransformInput );
  m_StateMachine.ProcessInputs();
}

template < class TImageGeometricModel >
void 
UltrasoundImageSimulator< TImageGeometricModel >
::SetTransformProcessing( )
{
  igstkLogMacro( DEBUG, 
       "igstk::UltrasoundImageSimulator\
       ::SetTransformProcessing called...\n");
  
  m_Transform = m_TransformToBeSet;
}


/** Request reslicing */
template < class TImageGeometricModel >
void 
UltrasoundImageSimulator< TImageGeometricModel >
::RequestReslice(  )
{
  igstkLogMacro( DEBUG, "igstk::UltrasoundImageSimulator\
                         ::RequestReslice called...\n");
  
  m_StateMachine.PushInput( m_ResliceInput );
  m_StateMachine.ProcessInputs();
}

template < class TImageGeometricModel >
void 
UltrasoundImageSimulator< TImageGeometricModel >
::ResliceProcessing( )
{
  igstkLogMacro( DEBUG, 
           "igstk::UltrasoundImageSimulator\
           ::ResliceProcessing called...\n");

  typedef TransformType::VectorType   VectorType;
  VectorType                position;
  position = m_Transform.GetTranslation();
  VectorType v1;
  v1[0] = 1;
  v1[1] = 0;
  v1[2] = 0;
  v1 = m_Transform.GetRotation().Transform(v1);
  
  VectorType v2;
  v2[0] = 0;
  v2[1] = 1;
  v2[2] = 0;
  v2 = m_Transform.GetRotation().Transform(v2);
 
  m_ImageReslice->SetInput(m_ImageData);

  double origin[3];
  m_ImageData->GetOrigin( origin );

  // Set the reslice axes origin
  m_ImageReslice->SetResliceAxesOrigin(position[0],
                                       position[1],
                                       position[2]);
  VectorType v3;  
   
  // Check if vector v1 and v2 are orthogonal
  double dot_product = v1*v2;

  if ( dot_product > 1e-9 )
    {
    // FIXME: need to handle this situation too
    std::cerr<< "The two vectors are not perpendicular" << std::endl;
    }

  // Generate vector v3 
  v3 = itk::CrossProduct( v1, v2 ); 
  
  VectorType v1Normalized;
  VectorType v2Normalized;
  VectorType v3Normalized;
  
  v1Normalized = v1/v1.GetNorm();
  v2Normalized = v2/v2.GetNorm();
  v3Normalized = v3/v3.GetNorm();

  // set the reslice axes 
  m_ImageReslice->SetResliceAxesDirectionCosines( 
                 v1Normalized[0], v1Normalized[1], v1Normalized[2],
                 v2Normalized[0], v2Normalized[1], v2Normalized[2],
                 v3Normalized[0], v3Normalized[1], v3Normalized[2]);

  // set the output dimensionality to 3
  m_ImageReslice->SetOutputDimensionality(3);

  // Set the output spacing 
  double spacing[3];
  m_ImageData->GetSpacing( spacing );
  m_ImageReslice->SetOutputSpacing( spacing[0],spacing[1], 1 );

  // Set the output extent
  int ext[6];
  m_ImageData->GetExtent( ext );
  m_ImageReslice->SetOutputExtent( ext[0], ext[1], ext[2], 
                                   ext[3], 0, 0);

  m_ImageReslice->SetOutputOrigin(origin);
  m_ImageReslice->SetInterpolationModeToLinear();
  m_ImageReslice->Update();
  
  m_VTKExporter->SetInput(m_ReslicedImageData);

  m_VTKImageImporter->SetUpdateInformationCallback( 
                               m_VTKExporter->GetUpdateInformationCallback());
  m_VTKImageImporter->SetPipelineModifiedCallback(
                                m_VTKExporter->GetPipelineModifiedCallback());
  m_VTKImageImporter->SetWholeExtentCallback( 
                                     m_VTKExporter->GetWholeExtentCallback());
  m_VTKImageImporter->SetSpacingCallback(m_VTKExporter->GetSpacingCallback());
  m_VTKImageImporter->SetOriginCallback( m_VTKExporter->GetOriginCallback() );
  m_VTKImageImporter->SetScalarTypeCallback( 
                                      m_VTKExporter->GetScalarTypeCallback());
  m_VTKImageImporter->SetNumberOfComponentsCallback( 
                              m_VTKExporter->GetNumberOfComponentsCallback());
  m_VTKImageImporter->SetPropagateUpdateExtentCallback( 
                           m_VTKExporter->GetPropagateUpdateExtentCallback());
  m_VTKImageImporter->SetUpdateDataCallback( 
                                      m_VTKExporter->GetUpdateDataCallback());
  m_VTKImageImporter->SetDataExtentCallback( 
                                      m_VTKExporter->GetDataExtentCallback());
  m_VTKImageImporter->SetBufferPointerCallback( 
                                   m_VTKExporter->GetBufferPointerCallback());
  m_VTKImageImporter->SetCallbackUserData( 
                                        m_VTKExporter->GetCallbackUserData());

  // Do the actual rescaling and casting of the US image
  typedef itk::RescaleIntensityImageFilter<MRImageType> RescaleFilterType;
  typename RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
  rescaler->SetInput(m_VTKImageImporter->GetOutput());
  rescaler->SetOutputMinimum(0);
  rescaler->SetOutputMaximum(255);
  rescaler->Update();

  typedef itk::CastImageFilter<MRImageType,USImageType> CastFilterType;
  typename CastFilterType::Pointer caster = CastFilterType::New();
  caster->SetInput(rescaler->GetOutput());
  caster->Update();
  
  m_RescaledUSImage = caster->GetOutput();


  typedef Friends::UltrasoundImageSimulatorToImageSpatialObject  HelperType;
  HelperType::SetITKImage( this, m_USImage.GetPointer() );

  // Update the transform of the image 
  m_USImage->RequestSetTransform(m_Transform);

}


/** Null Operation for a State Machine Transition */
template < class TImageGeometricModel >
void 
UltrasoundImageSimulator< TImageGeometricModel >
::NoProcessing()
{
}

/** Set nhe Image Spatial Object */
template < class TImageGeometricModel >
void 
UltrasoundImageSimulator< TImageGeometricModel >
::SetImageGeometricModelProcessing()
{
  igstkLogMacro( DEBUG, "igstk::UltrasoundImageSimulator\
                         ::SetImageSpatialObjectProcessing called...\n");

  // We create the image spatial object
  m_ImageGeometricModel = m_ImageGeometricModelToAdd;

  this->RequestSetImageGeometricModel( m_ImageGeometricModel );
  
  m_ImageGeometricModel->AddObserver( VTKImageModifiedEvent(), 
                                      m_VTKImageObserver );

  //
  // Here we get a VTK image data from the private method of the
  // ImageSpatialObject and stores it in the representation .
  //
  m_VTKImageObserver->Reset();
  m_ImageGeometricModel->RequestGetVTKImage();
  
  if( m_VTKImageObserver->GotVTKImage() )
    {

    this->SetImage( m_VTKImageObserver->GetVTKImage() );
    
    if( m_ImageData )
      {
      m_ImageData->Update();
      }
    }


  m_ImageReslice->SetInput ( m_ImageData );
  m_ReslicedImageData = m_ImageReslice->GetOutput();
}


/** Print Self function */
template < class TImageGeometricModel >
void
UltrasoundImageSimulator< TImageGeometricModel >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Transform";
  os << indent << this->m_Transform << std::endl;
  os << indent << "USImage";
  os << indent << this->m_USImage.GetPointer() << std::endl;
  os << indent << "RescaledUSImage";
  os << indent << this->m_RescaledUSImage.GetPointer() << std::endl;
}
  
template < class TImageGeometricModel >
void
UltrasoundImageSimulator< TImageGeometricModel >
::SetImage( const vtkImageData * image )
{
  igstkLogMacro( DEBUG, "igstk::UltrasoundImageSimulator\
                        ::SetImage called...\n");

  // This const_cast<> is needed here due to 
  // the lack of const-correctness in VTK 
  m_ImageData = const_cast< vtkImageData *>( image );
}


} // end namespace igstk

#endif
