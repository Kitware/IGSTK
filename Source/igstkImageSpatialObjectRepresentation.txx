/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageSpatialObjectRepresentation.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkCommand.h"
#include "itkImage.h"

#include "igstkImageSpatialObjectRepresentation.h"

#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkImageData.h"

#include "igstkEvents.h"

#include "itkCastImageFilter.h"
#include "itkImageFileReader.h"

namespace igstk
{

/** Constructor */

template < class TImageSpatialObject >
ImageSpatialObjectRepresentation< TImageSpatialObject >
::ImageSpatialObjectRepresentation():m_StateMachine(this)
{
  // We create the image spatial object
  m_ImageSpatialObject = NULL;
  this->RequestSetSpatialObject( m_ImageSpatialObject );

  // Create classes for displaying images
  m_ImageActor = NULL;
  m_MapColors  = vtkImageMapToColors::New();
  m_LUT        = vtkLookupTable::New();
  m_ImageData  = NULL;

  // Set default values for window and level
  m_Level = 0;
  m_Window = 2000;
  
  igstkAddInputMacro( ValidImageSpatialObjectInput );
  igstkAddInputMacro( NullImageSpatialObjectInput  );
  igstkAddInputMacro( EmptyImageSpatialObjectInput  );
  igstkAddInputMacro( SetSliceNumberInput  );
  igstkAddInputMacro( ValidSliceNumberInput  );
  igstkAddInputMacro( InvalidSliceNumberInput  );
  igstkAddInputMacro( ValidOrientationInput  );
  igstkAddInputMacro( RequestSliceNumberBoundsInput);

  igstkAddStateMacro( NullImageSpatialObjectState );
  igstkAddStateMacro( ValidImageSpatialObjectState );
  igstkAddStateMacro( ValidImageOrientationState );
  igstkAddStateMacro( ValidSliceNumberState );
  igstkAddStateMacro( AttemptingToSetSliceNumberState );

  igstkAddTransitionMacro( NullImageSpatialObjectState, NullImageSpatialObjectInput, NullImageSpatialObjectState,  NoAction );
  igstkAddTransitionMacro( NullImageSpatialObjectState, EmptyImageSpatialObjectInput, NullImageSpatialObjectState,  NoAction );
  igstkAddTransitionMacro( NullImageSpatialObjectState, ValidImageSpatialObjectInput, ValidImageSpatialObjectState,  SetImageSpatialObject );
  igstkAddTransitionMacro( NullImageSpatialObjectState, SetSliceNumberInput, NullImageSpatialObjectState,  NoAction );
  igstkAddTransitionMacro( NullImageSpatialObjectState, ValidSliceNumberInput, NullImageSpatialObjectState,  NoAction );
  igstkAddTransitionMacro( NullImageSpatialObjectState, InvalidSliceNumberInput, NullImageSpatialObjectState,  NoAction );
  igstkAddTransitionMacro( NullImageSpatialObjectState, ValidOrientationInput, NullImageSpatialObjectState,  NoAction );

  igstkAddTransitionMacro( ValidImageSpatialObjectState, NullImageSpatialObjectInput, NullImageSpatialObjectState,  NoAction ); 
  igstkAddTransitionMacro( ValidImageSpatialObjectState, EmptyImageSpatialObjectInput, NullImageSpatialObjectState,  NoAction ); 
  igstkAddTransitionMacro( ValidImageSpatialObjectState, ValidImageSpatialObjectInput, ValidImageSpatialObjectState,  NoAction ); 
  igstkAddTransitionMacro( ValidImageSpatialObjectState, SetSliceNumberInput, ValidImageSpatialObjectState,  NoAction ); 
  igstkAddTransitionMacro( ValidImageSpatialObjectState, ValidSliceNumberInput, ValidImageSpatialObjectState,  NoAction ); 
  igstkAddTransitionMacro( ValidImageSpatialObjectState, InvalidSliceNumberInput, ValidImageSpatialObjectState,  NoAction ); 
  igstkAddTransitionMacro( ValidImageSpatialObjectState, ValidOrientationInput, ValidImageOrientationState,  SetOrientation ); 

  igstkAddTransitionMacro( ValidImageOrientationState, NullImageSpatialObjectInput, NullImageSpatialObjectState,  NoAction ); 
  igstkAddTransitionMacro( ValidImageOrientationState, EmptyImageSpatialObjectInput, NullImageSpatialObjectState,  NoAction ); 
  igstkAddTransitionMacro( ValidImageOrientationState, ValidImageSpatialObjectInput, ValidImageSpatialObjectState,  SetImageSpatialObject );
  igstkAddTransitionMacro( ValidImageOrientationState, SetSliceNumberInput, AttemptingToSetSliceNumberState,  AttemptSetSliceNumber ); 
  igstkAddTransitionMacro( ValidImageOrientationState, ValidSliceNumberInput, ValidSliceNumberState,  SetSliceNumber ); 
  igstkAddTransitionMacro( ValidImageOrientationState, InvalidSliceNumberInput, ValidImageOrientationState,  NoAction ); 
  igstkAddTransitionMacro( ValidImageOrientationState, ValidOrientationInput, ValidImageOrientationState,  SetOrientation ); 

  igstkAddTransitionMacro( ValidSliceNumberState, NullImageSpatialObjectInput, NullImageSpatialObjectState,  NoAction ); 
  igstkAddTransitionMacro( ValidSliceNumberState, EmptyImageSpatialObjectInput, NullImageSpatialObjectState,  NoAction ); 
  igstkAddTransitionMacro( ValidSliceNumberState, ValidImageSpatialObjectInput, ValidImageSpatialObjectState,  SetImageSpatialObject );
  igstkAddTransitionMacro( ValidSliceNumberState, SetSliceNumberInput, AttemptingToSetSliceNumberState,  AttemptSetSliceNumber ); 
  igstkAddTransitionMacro( ValidSliceNumberState, ValidSliceNumberInput, ValidSliceNumberState,  SetSliceNumber ); 
  igstkAddTransitionMacro( ValidSliceNumberState, InvalidSliceNumberInput, ValidImageOrientationState,  NoAction ); 
  igstkAddTransitionMacro( ValidSliceNumberState, ValidOrientationInput, ValidImageOrientationState,  SetOrientation ); 

  igstkAddTransitionMacro( AttemptingToSetSliceNumberState, NullImageSpatialObjectInput, NullImageSpatialObjectState,  NoAction ); 
  igstkAddTransitionMacro( AttemptingToSetSliceNumberState, EmptyImageSpatialObjectInput, NullImageSpatialObjectState,  NoAction ); 
  igstkAddTransitionMacro( AttemptingToSetSliceNumberState, ValidImageSpatialObjectInput, ValidImageSpatialObjectState,  SetImageSpatialObject );
  igstkAddTransitionMacro( AttemptingToSetSliceNumberState, SetSliceNumberInput, AttemptingToSetSliceNumberState,  AttemptSetSliceNumber ); 
  igstkAddTransitionMacro( AttemptingToSetSliceNumberState, ValidSliceNumberInput, ValidSliceNumberState,  SetSliceNumber ); 
  igstkAddTransitionMacro( AttemptingToSetSliceNumberState, InvalidSliceNumberInput, ValidImageOrientationState,  NoAction ); 
  igstkAddTransitionMacro( AttemptingToSetSliceNumberState, ValidOrientationInput, ValidImageOrientationState,  SetOrientation ); 

  igstkAddTransitionMacro( ValidImageSpatialObjectState, RequestSliceNumberBoundsInput, ValidImageSpatialObjectState, ReportSliceNumberBounds );
  igstkAddTransitionMacro( ValidSliceNumberState, RequestSliceNumberBoundsInput, ValidSliceNumberState, ReportSliceNumberBounds );
  igstkAddTransitionMacro( ValidImageOrientationState, RequestSliceNumberBoundsInput, ValidImageOrientationState, ReportSliceNumberBounds );

  m_StateMachine.SelectInitialState( m_NullImageSpatialObjectState );

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

/** Overloaded DeleteActor function*/
template < class TImageSpatialObject >
void 
ImageSpatialObjectRepresentation< TImageSpatialObject >
::DeleteActors( )
{
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation::DeleteActors called...\n");
  
  this->Superclass::DeleteActors();
  
  m_ImageActor = NULL;

}
 
/** Set the Image Spatial Object */
template < class TImageSpatialObject >
void 
ImageSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetImageSpatialObject( const ImageSpatialObjectType * image )
{
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation::RequestSetImageSpatialObject called...\n");
  
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
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation::RequestSetOrientation called...\n");
  
  m_OrientationToBeSet = orientation;

  m_StateMachine.PushInput( m_ValidOrientationInput );
  m_StateMachine.ProcessInputs();
}


template < class TImageSpatialObject >
void 
ImageSpatialObjectRepresentation< TImageSpatialObject >
::SetOrientation()
{
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation::SetOrientation called...\n");
  m_Orientation = m_OrientationToBeSet;
}
  

template < class TImageSpatialObject >
void 
ImageSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetSliceNumber( SliceNumberType slice )
{

  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation::RequestSetSliceNumber called...\n");

  m_SliceNumberToBeSet = slice;

  m_StateMachine.PushInput( m_SetSliceNumberInput );
  m_StateMachine.ProcessInputs();

}


template < class TImageSpatialObject >
void 
ImageSpatialObjectRepresentation< TImageSpatialObject >
::AttemptSetSliceNumber()
{

  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation::AttemptSetSliceNumber called...\n");
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
::SetSliceNumber()
{
  
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation::SetSliceNumber called...\n");

  m_SliceNumber = m_SliceNumberToBeSet;

  int ext[6];
  m_ImageData->GetExtent( ext );

  switch( m_Orientation )
    {
    case Axial:
      m_ImageActor->SetDisplayExtent( ext[0], ext[1], ext[2], ext[3], m_SliceNumber, m_SliceNumber );
      break;
    case Sagittal:
      m_ImageActor->SetDisplayExtent( m_SliceNumber, m_SliceNumber, ext[2], ext[3], ext[4], ext[5] );
      break;
    case Coronal:
      m_ImageActor->SetDisplayExtent( ext[0], ext[1], m_SliceNumber, m_SliceNumber, ext[4], ext[5] );
      break;
    }

}


template < class TImageSpatialObject >
void 
ImageSpatialObjectRepresentation< TImageSpatialObject >
::SetWindowLevel( double window, double level )
{
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation::SetWindowLevel called...\n");

  m_Window = window;
  m_Level = level;

  m_LUT->SetTableRange ( (m_Level - m_Window/2.0), (m_Level + m_Window/2.0) );
}

/** Null Operation for a State Machine Transition */
template < class TImageSpatialObject >
void 
ImageSpatialObjectRepresentation< TImageSpatialObject >
::NoAction()
{
}

/** Set the Image Spatial Object */
template < class TImageSpatialObject >
void 
ImageSpatialObjectRepresentation< TImageSpatialObject >
::SetImageSpatialObject()
{
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation::SetImageSpatialObject called...\n");

  // We create the image spatial object
  m_ImageSpatialObject = m_ImageSpatialObjectToAdd;

  this->RequestSetSpatialObject( m_ImageSpatialObject );
  
  // This method gets a VTK image data from the private method of the
  // ImageSpatialObject and stores it in the representation by invoking the
  // private SetImage method.
  this->ConnectImage();

  m_MapColors->SetInput( m_ImageData );
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
::UpdateRepresentation()
{    
   igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation::UpdateRepresentation called...\n");
   if( m_ImageData )
     {
     m_MapColors->SetInput( m_ImageData );
     }
}


/** Create the vtk Actors */
template < class TImageSpatialObject >
void
ImageSpatialObjectRepresentation< TImageSpatialObject >
::CreateActors()
{
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation::CreateActors called...\n");

  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  m_ImageActor = vtkImageActor::New();

  if( m_ImageActor && m_MapColors && m_LUT  )
    {

    this->AddActor( m_ImageActor );
    
    m_LUT->SetTableRange ( (m_Level - m_Window/2.0), (m_Level + m_Window/2.0) );
    m_LUT->SetSaturationRange (0, 0);
    m_LUT->SetHueRange (0, 0);
    m_LUT->SetValueRange (0, 1);
    m_LUT->SetRampToLinear();

    m_MapColors->SetLookupTable( m_LUT );
    m_MapColors->SetInput( m_ImageData );
    
    m_ImageActor->SetInput( m_MapColors->GetOutput() );

    m_ImageActor->InterpolateOn();
    
    }
  else
    {
    igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation::CreateActors called with missing components\n");
    }

}

/** Create a copy of the current object representation */
template < class TImageSpatialObject >
typename ImageSpatialObjectRepresentation< TImageSpatialObject >::Pointer
ImageSpatialObjectRepresentation< TImageSpatialObject >
::Copy() const
{
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation::Copy called...\n");

  Pointer newOR = ImageSpatialObjectRepresentation::New();
  newOR->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());
  newOR->SetOpacity(this->GetOpacity());
  newOR->RequestSetImageSpatialObject(m_ImageSpatialObject);

  return newOR;
}

  
template < class TImageSpatialObject >
void
ImageSpatialObjectRepresentation< TImageSpatialObject >
::SetImage( const vtkImageData * image )
{
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation::SetImage called...\n");

  // This const_cast<> is needed here due to the lack of const-correctness in VTK 
  m_ImageData = const_cast< vtkImageData *>( image );
}



template < class TImageSpatialObject >
void
ImageSpatialObjectRepresentation< TImageSpatialObject >
::ConnectImage()
{
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation::ConnectImage called...\n");

  typedef Friends::ImageSpatialObjectRepresentationToImageSpatialObject  HelperType;
  HelperType::ConnectImage( m_ImageSpatialObject.GetPointer(), this );
  if( m_ImageData )
    {
    m_ImageData->Update();
    }
}




template < class TImageSpatialObject >
void
ImageSpatialObjectRepresentation< TImageSpatialObject >
::ReportSliceNumberBounds() 
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




} // end namespace igstk

