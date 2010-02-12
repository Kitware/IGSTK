/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageSpatialObjectRepresentation.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkImageSpatialObjectRepresentation_txx
#define __igstkImageSpatialObjectRepresentation_txx

#include "itkCommand.h"
#include "itkImage.h"
#include "itkCastImageFilter.h"
#include "itkImageFileReader.h"

#include "igstkImageSpatialObjectRepresentation.h"
#include "igstkEvents.h"

#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkImageData.h"
#include "vtkMath.h"

namespace igstk
{

/** Constructor */
template < class TImageSpatialObject >
ImageSpatialObjectRepresentation< TImageSpatialObject >
::ImageSpatialObjectRepresentation():m_StateMachine(this)
{
  // We create the image spatial object
  m_ImageSpatialObject = NULL;

  m_Orientation = Axial;

  this->RequestSetSpatialObject( m_ImageSpatialObject );

  // Create classes for displaying images
  m_ImageActor = vtkImageActor::New();
  this->AddActor( m_ImageActor );

  m_MapColors  = vtkImageMapToColors::New();
  m_LUT        = vtkLookupTable::New();
  m_ImageData  = NULL;

  // Set default values for window and level
  m_Level = 0;
  m_Window = 2000;
  
  // Create the observer to VTK image events 
  m_VTKImageObserver = VTKImageObserver::New();
  m_ImageTransformObserver = ImageTransformObserver::New();


  igstkAddInputMacro( ValidImageSpatialObject );
  igstkAddInputMacro( NullImageSpatialObject  );
  igstkAddInputMacro( EmptyImageSpatialObject  );
  igstkAddInputMacro( SetSliceNumber  );
  igstkAddInputMacro( ValidSliceNumber  );
  igstkAddInputMacro( InvalidSliceNumber  );
  igstkAddInputMacro( ValidOrientation  );
  igstkAddInputMacro( RequestSliceNumberBounds);
  igstkAddInputMacro( ConnectVTKPipeline );

  igstkAddStateMacro( NullImageSpatialObject );
  igstkAddStateMacro( ValidImageSpatialObject );
  igstkAddStateMacro( ValidImageOrientation );
  igstkAddStateMacro( ValidSliceNumber );
  igstkAddStateMacro( AttemptingToSetSliceNumber );

  igstkAddTransitionMacro( NullImageSpatialObject, NullImageSpatialObject,
                           NullImageSpatialObject,  No );
  igstkAddTransitionMacro( NullImageSpatialObject, EmptyImageSpatialObject,
                           NullImageSpatialObject,  No );
  igstkAddTransitionMacro( NullImageSpatialObject, ValidImageSpatialObject,
                           ValidImageSpatialObject,  SetImageSpatialObject );
  igstkAddTransitionMacro( NullImageSpatialObject, SetSliceNumber,
                           NullImageSpatialObject,  No );
  igstkAddTransitionMacro( NullImageSpatialObject, ValidSliceNumber,
                           NullImageSpatialObject,  No );
  igstkAddTransitionMacro( NullImageSpatialObject, InvalidSliceNumber,
                           NullImageSpatialObject,  No );
  igstkAddTransitionMacro( NullImageSpatialObject, ValidOrientation,
                           NullImageSpatialObject,  No );

  igstkAddTransitionMacro( ValidImageSpatialObject, NullImageSpatialObject,
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, EmptyImageSpatialObject,
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, ValidImageSpatialObject,
                           ValidImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, SetSliceNumber,
                           ValidImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, ValidSliceNumber,
                           ValidImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, InvalidSliceNumber,
                           ValidImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, ValidOrientation,
                           ValidImageOrientation,  SetOrientation ); 

  igstkAddTransitionMacro( ValidImageOrientation, NullImageSpatialObject,
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageOrientation, EmptyImageSpatialObject,
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageOrientation, ValidImageSpatialObject,
                           ValidImageSpatialObject,  SetImageSpatialObject );
  igstkAddTransitionMacro( ValidImageOrientation, SetSliceNumber,
                       AttemptingToSetSliceNumber,  AttemptSetSliceNumber ); 
  igstkAddTransitionMacro( ValidImageOrientation, ValidSliceNumber,
                           ValidSliceNumber,  SetSliceNumber ); 
  igstkAddTransitionMacro( ValidImageOrientation, InvalidSliceNumber,
                           ValidImageOrientation,  No ); 
  igstkAddTransitionMacro( ValidImageOrientation, ValidOrientation,
                           ValidImageOrientation,  SetOrientation ); 

  igstkAddTransitionMacro( ValidSliceNumber, NullImageSpatialObject,
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidSliceNumber, EmptyImageSpatialObject,
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidSliceNumber, ValidImageSpatialObject,
                           ValidImageSpatialObject,  SetImageSpatialObject );
  igstkAddTransitionMacro( ValidSliceNumber, SetSliceNumber,
                        AttemptingToSetSliceNumber,  AttemptSetSliceNumber );
  igstkAddTransitionMacro( ValidSliceNumber, ValidSliceNumber,
                           ValidSliceNumber,  SetSliceNumber ); 
  igstkAddTransitionMacro( ValidSliceNumber, InvalidSliceNumber,
                           ValidImageOrientation,  No ); 
  igstkAddTransitionMacro( ValidSliceNumber, ValidOrientation,
                           ValidImageOrientation,  SetOrientation ); 

  igstkAddTransitionMacro( AttemptingToSetSliceNumber, NullImageSpatialObject,
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( AttemptingToSetSliceNumber, EmptyImageSpatialObject,
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( AttemptingToSetSliceNumber, ValidImageSpatialObject,
                           ValidImageSpatialObject,  SetImageSpatialObject );
  igstkAddTransitionMacro( AttemptingToSetSliceNumber, SetSliceNumber,
                        AttemptingToSetSliceNumber,  AttemptSetSliceNumber );
  igstkAddTransitionMacro( AttemptingToSetSliceNumber, ValidSliceNumber,
                           ValidSliceNumber,  SetSliceNumber ); 
  igstkAddTransitionMacro( AttemptingToSetSliceNumber, InvalidSliceNumber,
                           ValidImageOrientation,  No ); 
  igstkAddTransitionMacro( AttemptingToSetSliceNumber, ValidOrientation,
                           ValidImageOrientation,  SetOrientation ); 

  igstkAddTransitionMacro( ValidImageSpatialObject, RequestSliceNumberBounds,
                           ValidImageSpatialObject, ReportSliceNumberBounds );
  igstkAddTransitionMacro( ValidSliceNumber, RequestSliceNumberBounds,
                           ValidSliceNumber, ReportSliceNumberBounds );
  igstkAddTransitionMacro( ValidImageOrientation, RequestSliceNumberBounds,
                           ValidImageOrientation, ReportSliceNumberBounds );

  igstkAddTransitionMacro( NullImageSpatialObject, ConnectVTKPipeline,
                           NullImageSpatialObject, No );
  igstkAddTransitionMacro( ValidImageSpatialObject, ConnectVTKPipeline,
                           ValidImageSpatialObject, ConnectVTKPipeline );
  igstkAddTransitionMacro( ValidImageOrientation, ConnectVTKPipeline,
                           ValidImageOrientation, ConnectVTKPipeline );
  igstkAddTransitionMacro( ValidSliceNumber, ConnectVTKPipeline, 
                           ValidSliceNumber, ConnectVTKPipeline );

  igstkSetInitialStateMacro( NullImageSpatialObject );

  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
template < class TImageSpatialObject >
ImageSpatialObjectRepresentation< TImageSpatialObject >
::~ImageSpatialObjectRepresentation()  
{

  // This deletes also the m_ImageActor
  this->DeleteActors();


  if( m_MapColors )
    {
    m_MapColors->SetLookupTable( NULL );
    m_MapColors->SetInput( NULL );
    m_MapColors->Delete();
    m_MapColors = NULL;
    }

    
  if( m_LUT )
    {
    m_LUT->Delete();
    m_LUT = NULL;
    }
}

/** Overloaded DeleteActor function */
template < class TImageSpatialObject >
void 
ImageSpatialObjectRepresentation< TImageSpatialObject >
::DeleteActors( )
{
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation\
                        ::DeleteActors called...\n");
  
  this->Superclass::DeleteActors();
  
  m_ImageActor = NULL;

}
 
/** Set the Image Spatial Object */
template < class TImageSpatialObject >
void 
ImageSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetImageSpatialObject( const ImageSpatialObjectType * image )
{
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation\
                        ::RequestSetImageSpatialObject called...\n");
  
  m_ImageSpatialObjectToAdd = image;

  if( !m_ImageSpatialObjectToAdd )
    {
    m_StateMachine.PushInput( m_NullImageSpatialObjectInput );
    }
  else 
    {
    if( m_ImageSpatialObjectToAdd->IsEmpty() )
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


template < class TImageSpatialObject >
void 
ImageSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetOrientation( OrientationType orientation )
{
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation\
                        ::RequestSetOrientation called...\n");
  
  m_OrientationToBeSet = orientation;

  m_StateMachine.PushInput( m_ValidOrientationInput );
  m_StateMachine.ProcessInputs();
}


template < class TImageSpatialObject >
void 
ImageSpatialObjectRepresentation< TImageSpatialObject >
::SetOrientationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation\
                        ::SetOrientationProcessing called...\n");
  m_Orientation = m_OrientationToBeSet;
 
}
  

template < class TImageSpatialObject >
void 
ImageSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetSliceNumber( SliceNumberType slice )
{

  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation\
                        ::RequestSetSliceNumber called...\n");

  m_SliceNumberToBeSet = slice;

  m_StateMachine.PushInput( m_SetSliceNumberInput );
  m_StateMachine.ProcessInputs();

}


template < class TImageSpatialObject >
void 
ImageSpatialObjectRepresentation< TImageSpatialObject >
::AttemptSetSliceNumberProcessing()
{

  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation\
                        ::AttemptSetSliceNumberProcessing called...\n");
  if( m_ImageActor )
    {

    SliceNumberType minSlice = 0;
    SliceNumberType maxSlice = 0;
    
    int ext[6];

    m_ImageData->Update();
    m_ImageData->GetExtent( ext );

    switch( m_Orientation )
      {
      case Axial:
        minSlice = ext[4];
        maxSlice = ext[5];
        break;
      case Sagittal:
        minSlice = ext[0];
        maxSlice = ext[1];
        break;
      case Coronal:
        minSlice = ext[2];
        maxSlice = ext[3];
        break;
      }

    if( m_SliceNumberToBeSet >= minSlice && m_SliceNumberToBeSet <= maxSlice )
      {
      m_StateMachine.PushInput( m_ValidSliceNumberInput );
      }
    else
      {
      m_StateMachine.PushInput( m_InvalidSliceNumberInput );
      }

    m_StateMachine.ProcessInputs();
    }
}

template < class TImageSpatialObject >
void 
ImageSpatialObjectRepresentation< TImageSpatialObject >
::SetSliceNumberProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation\
                        ::SetSliceNumber called...\n");

  m_SliceNumber = m_SliceNumberToBeSet;

  int ext[6];
  m_ImageData->GetExtent( ext );

  switch( m_Orientation )
    {
    case Axial:
      m_ImageActor->SetDisplayExtent( ext[0], ext[1], ext[2], 
                                      ext[3], m_SliceNumber, m_SliceNumber );
      break;
    case Sagittal:
      m_ImageActor->SetDisplayExtent( m_SliceNumber, m_SliceNumber, 
                                      ext[2], ext[3], ext[4], ext[5] );
      break;
    case Coronal:
      m_ImageActor->SetDisplayExtent( ext[0], ext[1], m_SliceNumber, 
                                      m_SliceNumber, ext[4], ext[5] );
      break;
    }

}


template < class TImageSpatialObject >
void 
ImageSpatialObjectRepresentation< TImageSpatialObject >
::SetWindowLevel( double window, double level )
{
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation\
                        ::SetWindowLevel called...\n");

  m_Window = window;
  m_Level = level;

  m_LUT->SetTableRange ( (m_Level - m_Window/2.0), (m_Level + m_Window/2.0) );
}

/** Null Operation for a State Machine Transition */
template < class TImageSpatialObject >
void 
ImageSpatialObjectRepresentation< TImageSpatialObject >
::NoProcessing()
{
}

/** Set the Image Spatial Object */
template < class TImageSpatialObject >
void 
ImageSpatialObjectRepresentation< TImageSpatialObject >
::SetImageSpatialObjectProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation\
                        ::SetImageSpatialObjectProcessing called...\n");

  // We create the image spatial object
  m_ImageSpatialObject = m_ImageSpatialObjectToAdd;

  m_ImageSpatialObject->AddObserver( VTKImageModifiedEvent(), 
                                     m_VTKImageObserver );

  m_ImageSpatialObject->AddObserver( CoordinateSystemTransformToEvent(), 
                                     m_ImageTransformObserver );

  this->RequestSetSpatialObject( m_ImageSpatialObject );
  
  // This method gets a VTK image data from the private method of the
  // ImageSpatialObject and stores it in the representation by invoking the
  // private SetImage method.
  //
  // 
  this->m_VTKImageObserver->Reset();

  this->m_ImageSpatialObject->RequestGetVTKImage();

  if( this->m_VTKImageObserver->GotVTKImage() ) 
    {
    this->m_ImageData = this->m_VTKImageObserver->GetVTKImage();
    if( this->m_ImageData )
      {
      this->m_ImageData->Update();
      }
    this->m_MapColors->SetInput( this->m_ImageData );
    }

  this->m_ImageTransformObserver->Reset();

  this->m_ImageSpatialObject->RequestGetImageTransform();

  if( this->m_ImageTransformObserver->GotImageTransform() ) 
    {
    const CoordinateSystemTransformToResult transformCarrier =
      this->m_ImageTransformObserver->GetImageTransform();
    this->m_ImageTransform = transformCarrier.GetTransform();

    // Image Actor takes care of the image origin position internally.
    this->m_ImageActor->SetPosition(0,0,0); 

    vtkMatrix4x4 * imageTransformMatrix = vtkMatrix4x4::New();

    this->m_ImageTransform.ExportTransform( *imageTransformMatrix );

    this->m_ImageActor->SetUserMatrix( imageTransformMatrix );
    imageTransformMatrix->Delete();
    }

  this->m_ImageActor->SetInput( this->m_MapColors->GetOutput() );
}


/** Print Self function */
template < class TImageSpatialObject >
void
ImageSpatialObjectRepresentation< TImageSpatialObject >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


/** Update the visual representation in response to changes in the geometric
 * object */
template < class TImageSpatialObject >
void
ImageSpatialObjectRepresentation< TImageSpatialObject >
::UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation\
                       ::UpdateRepresentationProcessing called...\n");
  //if( m_ImageData )
  //  {
  //  m_MapColors->SetInput( m_ImageData );
  //  }
}


/** Create the vtk Actors */
template < class TImageSpatialObject >
void
ImageSpatialObjectRepresentation< TImageSpatialObject >
::CreateActors()
{
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation\
                        ::CreateActors called...\n");

  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  m_ImageActor = vtkImageActor::New();
  m_ImageActor->SetPosition(0,0,0);
  m_ImageActor->SetOrientation(0,0,0);
    
  this->AddActor( m_ImageActor );

  //convert RGB to HSV
  double hue = 0.0;
  double saturation = 0.0;
  double value = 1.0;

  vtkMath::RGBToHSV( this->GetRed(),
                     this->GetGreen(),
                     this->GetBlue(),
                     &hue,&saturation,&value );

  m_LUT->SetTableRange ( (m_Level - m_Window/2.0), (m_Level + m_Window/2.0) );
  m_LUT->SetSaturationRange (saturation, saturation);
  m_LUT->SetAlphaRange (m_Opacity, m_Opacity);
  m_LUT->SetHueRange (hue, hue);
  m_LUT->SetValueRange (0, value);
  m_LUT->SetRampToLinear();

  m_MapColors->SetLookupTable( m_LUT );

  igstkPushInputMacro( ConnectVTKPipeline );
  m_StateMachine.ProcessInputs(); 

}

/** Create a copy of the current object representation */
template < class TImageSpatialObject >
typename ImageSpatialObjectRepresentation< TImageSpatialObject >::Pointer
ImageSpatialObjectRepresentation< TImageSpatialObject >
::Copy() const
{
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation\
                        ::Copy called...\n");

  Pointer newOR = ImageSpatialObjectRepresentation::New();
  newOR->SetColor( this->GetRed(), this->GetGreen(), this->GetBlue() );
  newOR->SetOpacity( this->GetOpacity() );
  newOR->RequestSetImageSpatialObject( m_ImageSpatialObject );

  return newOR;
}

 
template < class TImageSpatialObject >
void
ImageSpatialObjectRepresentation< TImageSpatialObject >
::SetImage( const vtkImageData * image )
{
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation\
                        ::SetImage called...\n");

  // This const_cast<> is needed here due to the lack of 
  // const-correctness in VTK 
  m_ImageData = const_cast< vtkImageData *>( image );
}


template < class TImageSpatialObject >
void
ImageSpatialObjectRepresentation< TImageSpatialObject >
::ReportSliceNumberBoundsProcessing() 
{
  int ext[6];

  m_ImageData->Update();
  m_ImageData->GetExtent( ext );

  EventHelperType::IntegerBoundsType bounds;

  switch( m_Orientation )
    {
    case Axial:
      {
      bounds.minimum = ext[4];
      bounds.maximum = ext[5];
      AxialSliceBoundsEvent event;
      event.Set( bounds );
      this->InvokeEvent( event );
      break;
      }
    case Sagittal:
      {
      bounds.minimum = ext[0];
      bounds.maximum = ext[1];
      SagittalSliceBoundsEvent event;
      event.Set( bounds );
      this->InvokeEvent( event );
      break;
      }
    case Coronal:
      {
      bounds.minimum = ext[2];
      bounds.maximum = ext[3];
      CoronalSliceBoundsEvent event;
      event.Set( bounds );
      this->InvokeEvent( event );
      break;
      }
    }
}

template < class TImageSpatialObject >
void
ImageSpatialObjectRepresentation< TImageSpatialObject >
::RequestGetSliceNumberBounds() 
{
  m_StateMachine.PushInput( m_RequestSliceNumberBoundsInput );
  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void
ImageSpatialObjectRepresentation< TImageSpatialObject >
::ConnectVTKPipelineProcessing() 
{
  m_MapColors->SetInput( m_ImageData );
  m_ImageActor->SetInput( m_MapColors->GetOutput() );
  m_ImageActor->InterpolateOn();
}

/** Set the opacity */
template < class TImageSpatialObject >
void
ImageSpatialObjectRepresentation< TImageSpatialObject >
::SetOpacity(float alpha)
{
  if(m_Opacity == alpha)
    {
    return;
    }
  m_Opacity = alpha;

  // Update all the actors
  ActorsListType::iterator it = m_Actors.begin();
  while(it != m_Actors.end())
    {
    vtkImageActor * va = static_cast<vtkImageActor*>(*it);
    va->SetOpacity(m_Opacity); 
    it++;
    }
}

} // end namespace igstk

#endif
