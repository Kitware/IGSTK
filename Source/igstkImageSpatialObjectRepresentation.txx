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

  // Set default values for window and level
  m_Level = 0;
  m_Window = 2000;
  
  m_StateMachine.AddInput( m_ValidImageSpatialObjectInput,  "ValidImageSpatialObjectInput" );
  m_StateMachine.AddInput( m_NullImageSpatialObjectInput,   "NullImageSpatialObjectInput"  );
  m_StateMachine.AddInput( m_ValidSliceNumberInput,   "ValidSliceNumberInput"  );
  m_StateMachine.AddInput( m_InvalidSliceNumberInput,   "InvalidSliceNumberInput"  );
  m_StateMachine.AddInput( m_ValidOrientationInput,   "ValidOrientationInput"  );

  m_StateMachine.AddState( m_NullImageSpatialObjectState,  "NullImageSpatialObjectState"     );
  m_StateMachine.AddState( m_ValidImageSpatialObjectState, "ValidImageSpatialObjectState"     );
  m_StateMachine.AddState( m_ValidImageOrientationState, "ValidImageOrientationState"     );
  m_StateMachine.AddState( m_ValidSliceNumberState, "ValidSliceNumberState"     );

  const ActionType NoAction = 0;

  m_StateMachine.AddTransition( m_NullImageSpatialObjectState, m_NullImageSpatialObjectInput, m_NullImageSpatialObjectState,  NoAction );
  m_StateMachine.AddTransition( m_NullImageSpatialObjectState, m_ValidImageSpatialObjectInput, m_ValidImageSpatialObjectState,  & ImageSpatialObjectRepresentation::SetImageSpatialObject );
  m_StateMachine.AddTransition( m_NullImageSpatialObjectState, m_ValidSliceNumberInput, m_NullImageSpatialObjectState,  NoAction );
  m_StateMachine.AddTransition( m_NullImageSpatialObjectState, m_InvalidSliceNumberInput, m_NullImageSpatialObjectState,  NoAction );
  m_StateMachine.AddTransition( m_NullImageSpatialObjectState, m_ValidOrientationInput, m_NullImageSpatialObjectState,  NoAction );

  m_StateMachine.AddTransition( m_ValidImageSpatialObjectState, m_NullImageSpatialObjectInput, m_NullImageSpatialObjectState,  NoAction ); 
  m_StateMachine.AddTransition( m_ValidImageSpatialObjectState, m_ValidImageSpatialObjectInput, m_ValidImageSpatialObjectState,  NoAction ); 
  m_StateMachine.AddTransition( m_ValidImageSpatialObjectState, m_ValidSliceNumberInput, m_ValidImageSpatialObjectState,  NoAction ); 
  m_StateMachine.AddTransition( m_ValidImageSpatialObjectState, m_InvalidSliceNumberInput, m_ValidImageSpatialObjectState,  NoAction ); 
  m_StateMachine.AddTransition( m_ValidImageSpatialObjectState, m_ValidOrientationInput, m_ValidImageOrientationState,  & ImageSpatialObjectRepresentation::SetOrientation ); 

  m_StateMachine.AddTransition( m_ValidImageOrientationState, m_NullImageSpatialObjectInput, m_NullImageSpatialObjectState,  NoAction ); 
  m_StateMachine.AddTransition( m_ValidImageOrientationState, m_ValidImageSpatialObjectInput, m_ValidImageSpatialObjectState,  & ImageSpatialObjectRepresentation::SetImageSpatialObject );
  m_StateMachine.AddTransition( m_ValidImageOrientationState, m_ValidSliceNumberInput, m_ValidSliceNumberState,  & ImageSpatialObjectRepresentation::SetSliceNumber ); 
  m_StateMachine.AddTransition( m_ValidImageOrientationState, m_InvalidSliceNumberInput, m_ValidImageOrientationState,  NoAction ); 
  m_StateMachine.AddTransition( m_ValidImageOrientationState, m_ValidOrientationInput, m_ValidImageOrientationState,  & ImageSpatialObjectRepresentation::SetOrientation ); 

  m_StateMachine.AddTransition( m_ValidSliceNumberState, m_NullImageSpatialObjectInput, m_NullImageSpatialObjectState,  NoAction ); 
  m_StateMachine.AddTransition( m_ValidSliceNumberState, m_ValidImageSpatialObjectInput, m_ValidImageSpatialObjectState,  & ImageSpatialObjectRepresentation::SetImageSpatialObject );
  m_StateMachine.AddTransition( m_ValidSliceNumberState, m_ValidSliceNumberInput, m_ValidSliceNumberState,  & ImageSpatialObjectRepresentation::SetSliceNumber ); 
  m_StateMachine.AddTransition( m_ValidSliceNumberState, m_InvalidSliceNumberInput, m_ValidImageOrientationState,  NoAction ); 
  m_StateMachine.AddTransition( m_ValidSliceNumberState, m_ValidOrientationInput, m_ValidImageOrientationState,  & ImageSpatialObjectRepresentation::SetOrientation ); 

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
    m_StateMachine.ProcessInputs();
    }
  else
    {
    m_StateMachine.PushInput( m_ValidImageSpatialObjectInput );
    m_StateMachine.ProcessInputs();
    }
}


template < class TImageSpatialObject >
void 
ImageSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetOrientation( OrientationType orientation )
{
  igstkLogMacro( DEBUG, "igstk::ImageSpatialObjectRepresentation::RequestSetOrientation called...\n");
  
  m_OrientationToBeSet = orientation;

  m_StateMachine.PushInput( m_ValidOrientationInput );
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

  if( m_ImageActor )
    {

    SliceNumberType minSlice = 0;
    SliceNumberType maxSlice = 0;
    
    int ext[6];
    m_ImageData->GetExtent( ext );

    switch( m_Orientation )
      {
      case Axial:
        minSlice = ext[0];
        maxSlice = ext[1];
        break;
      case Sagittal:
        minSlice = ext[4];
        maxSlice = ext[5];
        break;
      case Coronal:
        minSlice = ext[2];
        maxSlice = ext[3];
        break;
      }

    if( slice >= minSlice && slice <= maxSlice )
      {
      m_StateMachine.PushInput( m_ValidSliceNumberInput );
      }
    else
      {
      m_StateMachine.PushInput( m_InvalidSliceNumberInput );
      }

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
   m_MapColors->SetInput( m_ImageData );
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

  if( m_ImageActor )
    {
    this->AddActor( m_ImageActor );
    }

  if( m_ImageActor && m_MapColors && m_LUT )
    {
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
}




template < class TImageSpatialObject >
typename ImageSpatialObjectRepresentation< TImageSpatialObject >::SliceNumberType
ImageSpatialObjectRepresentation< TImageSpatialObject >
::GetMinimumSliceNumber() const
{
    int ext[6];
    m_ImageData->GetExtent( ext );

    SliceNumberType minimum = 0;
    
    switch( m_Orientation )
      {
      case Axial:
        minimum = ext[0];
        break;
      case Sagittal:
        minimum = ext[4];
        break;
      case Coronal:
        minimum = ext[2];
        break;
      }
   return minimum;
}



template < class TImageSpatialObject >
typename ImageSpatialObjectRepresentation< TImageSpatialObject >::SliceNumberType
ImageSpatialObjectRepresentation< TImageSpatialObject >
::GetMaximumSliceNumber() const
{

    int ext[6];
    m_ImageData->GetExtent( ext );

    SliceNumberType maximum = 0;
    
    switch( m_Orientation )
      {
      case Axial:
        maximum = ext[1];
        break;
      case Sagittal:
        maximum = ext[5];
        break;
      case Coronal:
        maximum = ext[3];
        break;
      }
   return maximum;   
}




} // end namespace igstk

