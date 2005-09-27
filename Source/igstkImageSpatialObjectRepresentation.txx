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
#include "vtkActor.h"
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
  m_ImageActor = vtkImageActor::New();
  m_MapColors  = vtkImageMapToColors::New();
  m_LUT        = vtkLookupTable::New();

  // Set default values for window and level
  m_Level = 0;
  m_Window = 2000;
  
  m_StateMachine.AddInput( m_ValidImageSpatialObjectInput,  "ValidImageSpatialObjectInput" );
  m_StateMachine.AddInput( m_NullImageSpatialObjectInput,   "NullImageSpatialObjectInput"  );

  m_StateMachine.AddState( m_NullImageSpatialObjectState,  "NullImageSpatialObjectState"     );
  m_StateMachine.AddState( m_ValidImageSpatialObjectState, "ValidImageSpatialObjectState"     );

  const ActionType NoAction = 0;

  m_StateMachine.AddTransition( m_NullImageSpatialObjectState, m_NullImageSpatialObjectInput, m_NullImageSpatialObjectState,  NoAction );
  m_StateMachine.AddTransition( m_NullImageSpatialObjectState, m_ValidImageSpatialObjectInput, m_ValidImageSpatialObjectState,  & ImageSpatialObjectRepresentation::SetImageSpatialObject );
  m_StateMachine.AddTransition( m_ValidImageSpatialObjectState, m_NullImageSpatialObjectInput, m_NullImageSpatialObjectState,  NoAction ); 
  m_StateMachine.AddTransition( m_ValidImageSpatialObjectState, m_ValidImageSpatialObjectInput, m_ValidImageSpatialObjectState,  NoAction ); 

  m_StateMachine.SelectInitialState( m_NullImageSpatialObjectState );

  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
template < class TImageSpatialObject >
ImageSpatialObjectRepresentation< TImageSpatialObject >
::~ImageSpatialObjectRepresentation()  
{
  if( m_ImageActor )
    {
    m_ImageActor->Delete();
    m_LUT = NULL;
    }

  if( m_MapColors )
    {
    m_MapColors->Delete();
    m_MapColors = NULL;
    }

  if( m_LUT )
    {
    m_LUT->Delete();
    m_LUT = NULL;
    }
}

/** Set the Image Spatial Object */
template < class TImageSpatialObject >
void 
ImageSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetImageSpatialObject( const ImageSpatialObjectType * image )
{
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
::SetZSlice( int slice )
{
  int minz;
  int maxz;

  if( m_ImageActor )
    {
    minz = m_ImageActor->GetWholeZMin();
    maxz = m_ImageActor->GetWholeZMax();

    if( slice >= minz && slice <= maxz )
      {
      m_ImageActor->SetDisplayExtent( 0, 511, 0, 511, slice, slice );
      }
    }
}

template < class TImageSpatialObject >
void 
ImageSpatialObjectRepresentation< TImageSpatialObject >
::SetWindowLevel( double window, double level )
{
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
  // We create the image spatial object
  m_ImageSpatialObject = m_ImageSpatialObjectToAdd;

//  this->RequestSetSpatialObject( m_ImageSpatialObject );

  if( m_ImageActor && m_MapColors && m_LUT )
    {
//    m_LUT->SetTableRange (-1000, 1000);
    m_LUT->SetTableRange ( (m_Level - m_Window/2.0), (m_Level + m_Window/2.0) );
    m_LUT->SetSaturationRange (0, 0);
    m_LUT->SetHueRange (0, 0);
    m_LUT->SetValueRange (0, 1);
    m_LUT->SetRampToLinear();

    m_MapColors->SetLookupTable( m_LUT );

    // This method gets a VTK image data from the private method of the
    // ImageSpatialObject and stores it in the representation by invoking the
    // private SetImage method.
    this->ConnectImage();

    m_MapColors->SetInput( m_ImageData );
    
    m_ImageActor->SetInput( m_MapColors->GetOutput() );

    m_ImageActor->SetDisplayExtent( 0, 511, 0, 511, 0, 0 );
    m_ImageActor->InterpolateOn();
    }
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
//  m_PlaneSource->SetRadius(m_CylinderSpatialObject->GetRadius());
//  m_PlaneSource->SetHeight(m_CylinderSpatialObject->GetHeight());
}


/** Create the vtk Actors */
template < class TImageSpatialObject >
void
ImageSpatialObjectRepresentation< TImageSpatialObject >
::CreateActors()
{
  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  if( m_ImageActor )
  {
  this->AddActor( m_ImageActor );
  }
}

/** Create a copy of the current object representation */
template < class TImageSpatialObject >
typename ImageSpatialObjectRepresentation< TImageSpatialObject >::Pointer
ImageSpatialObjectRepresentation< TImageSpatialObject >
::Copy() const
{
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
  // This const_cast<> is needed here due to the lack of const-correctness in VTK 
  m_ImageData = const_cast< vtkImageData *>( image );
}



template < class TImageSpatialObject >
void
ImageSpatialObjectRepresentation< TImageSpatialObject >
::ConnectImage()
{
  typedef Friends::ImageSpatialObjectRepresentationToImageSpatialObject  HelperType;
  HelperType::ConnectImage( m_ImageSpatialObject.GetPointer(), this );
}



} // end namespace igstk

