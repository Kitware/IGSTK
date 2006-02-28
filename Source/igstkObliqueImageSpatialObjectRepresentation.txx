/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkObliqueImageSpatialObjectRepresentation.txx
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

#include "igstkObliqueImageSpatialObjectRepresentation.h"

#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"


#include "igstkEvents.h"

#include "itkCastImageFilter.h"
#include "itkImageFileReader.h"

namespace igstk
{

/** Constructor */

template < class TImageSpatialObject >
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::ObliqueImageSpatialObjectRepresentation():m_StateMachine(this)
{
  // We create the image spatial object
  m_ImageSpatialObject = NULL;

  this->RequestSetSpatialObject( m_ImageSpatialObject );

  // Create classes for displaying images
  m_ImageActor = vtkImageActor::New();
  this->AddActor( m_ImageActor );
  m_MapColors  = vtkImageMapToColors::New();
  m_LUT        = vtkLookupTable::New();
  m_ImageData  = NULL;

  // Image reslice
  m_ImageReslice = vtkImageReslice::New();      


  // Set default values for window and level
  m_Level = 0;
  m_Window = 2000;
  
  igstkAddInputMacro( ValidImageSpatialObject );
  igstkAddInputMacro( NullImageSpatialObject  );
  igstkAddInputMacro( EmptyImageSpatialObject  );
  igstkAddInputMacro( ConnectVTKPipeline );

  igstkAddStateMacro( NullImageSpatialObject );
  igstkAddStateMacro( ValidImageSpatialObject );

  igstkAddTransitionMacro( NullImageSpatialObject, NullImageSpatialObject, NullImageSpatialObject,  No );
  igstkAddTransitionMacro( NullImageSpatialObject, EmptyImageSpatialObject, NullImageSpatialObject,  No );
  igstkAddTransitionMacro( NullImageSpatialObject, ValidImageSpatialObject, ValidImageSpatialObject,  SetImageSpatialObject );

  igstkAddTransitionMacro( ValidImageSpatialObject, NullImageSpatialObject, NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, EmptyImageSpatialObject, NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, ValidImageSpatialObject, ValidImageSpatialObject,  No ); 

  igstkAddTransitionMacro( NullImageSpatialObject, ConnectVTKPipeline, NullImageSpatialObject, No );
  igstkAddTransitionMacro( ValidImageSpatialObject, ConnectVTKPipeline, ValidImageSpatialObject, ConnectVTKPipeline );
  igstkSetInitialStateMacro( NullImageSpatialObject );

  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
template < class TImageSpatialObject >
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::~ObliqueImageSpatialObjectRepresentation()  
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
 
  if( m_ImageReslice )
    {
    m_ImageReslice->Delete();
    m_ImageReslice = NULL;
    }
}

/** Overloaded DeleteActor function*/
template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::DeleteActors( )
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::DeleteActors called...\n");
  
  this->Superclass::DeleteActors();
  
  m_ImageActor = NULL;

}
 
/** Set the Image Spatial Object */
template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetImageSpatialObject( const ImageSpatialObjectType * image )
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::RequestSetImageSpatialObject called...\n");
  
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
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::SetWindowLevel( double window, double level )
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::SetWindowLevel called...\n");

  m_Window = window;
  m_Level = level;

  m_LUT->SetTableRange ( (m_Level - m_Window/2.0), (m_Level + m_Window/2.0) );
}

/** Null Operation for a State Machine Transition */
template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::NoProcessing()
{
}

/** Set the Image Spatial Object */
template < class TImageSpatialObject >
void 
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::SetImageSpatialObjectProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::SetImageSpatialObjectProcessing called...\n");

  // We create the image spatial object
  m_ImageSpatialObject = m_ImageSpatialObjectToAdd;

  this->RequestSetSpatialObject( m_ImageSpatialObject );
  
  // This method gets a VTK image data from the private method of the
  // ImageSpatialObject and stores it in the representation by invoking the
  // private SetImage method.
  this->ConnectImage();

  m_MapColors->SetInput( m_ImageData );

  m_ImageActor->SetInput( m_MapColors->GetOutput() );
}


/** Print Self function */
template < class TImageSpatialObject >
void
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


/** Update the visual representation in response to changes in the geometric
 * object */
template < class TImageSpatialObject >
void
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::UpdateRepresentationProcessing()
{    
   igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::UpdateRepresentationProcessing called...\n");
   if( m_ImageData )
     {
     m_MapColors->SetInput( m_ImageData );
     }
}


/** Create the vtk Actors */
template < class TImageSpatialObject >
void
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::CreateActors()
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::CreateActors called...\n");

  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  m_ImageActor = vtkImageActor::New();
  this->AddActor( m_ImageActor );
    
  m_LUT->SetTableRange ( (m_Level - m_Window/2.0), (m_Level + m_Window/2.0) );
  m_LUT->SetSaturationRange (0, 0);
  m_LUT->SetHueRange (0, 0);
  m_LUT->SetValueRange (0, 1);
  m_LUT->SetRampToLinear();

  m_MapColors->SetLookupTable( m_LUT );

  igstkPushInputMacro( ConnectVTKPipeline );
  m_StateMachine.ProcessInputs(); 

}

/** Create a copy of the current object representation */
template < class TImageSpatialObject >
typename ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >::Pointer
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::Copy() const
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::Copy called...\n");

  Pointer newOR = ObliqueImageSpatialObjectRepresentation::New();
  newOR->SetColor(this->GetRed(),this->GetGreen(),this->GetBlue());
  newOR->SetOpacity(this->GetOpacity());
  newOR->RequestSetImageSpatialObject(m_ImageSpatialObject);

  return newOR;
}

  
template < class TImageSpatialObject >
void
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::SetImage( const vtkImageData * image )
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::SetImage called...\n");

  // This const_cast<> is needed here due to the lack of const-correctness in VTK 
  m_ImageData = const_cast< vtkImageData *>( image );
}



template < class TImageSpatialObject >
void
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::ConnectImage()
{
  igstkLogMacro( DEBUG, "igstk::ObliqueImageSpatialObjectRepresentation::ConnectImage called...\n");

  typedef Friends::ObliqueImageSpatialObjectRepresentationToImageSpatialObject  HelperType;
  HelperType::ConnectImage( m_ImageSpatialObject.GetPointer(), this );
  if( m_ImageData )
    {
    m_ImageData->Update();
    }
}


template < class TImageSpatialObject >
void
ObliqueImageSpatialObjectRepresentation< TImageSpatialObject >
::ConnectVTKPipelineProcessing() 
{
  m_MapColors->SetInput( m_ImageData );
  m_ImageActor->SetInput( m_MapColors->GetOutput() );
  m_ImageActor->InterpolateOn();
}


} // end namespace igstk

