/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageResliceSpatialObjectRepresentation.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkImageResliceSpatialObjectRepresentation_txx
#define __igstkImageResliceSpatialObjectRepresentation_txx


#include "igstkImageResliceSpatialObjectRepresentation.h"
#include "igstkEvents.h"

#include "vtkImageActor.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkMath.h"
#include "vtkImageReslice.h"
#include "vtkMatrix4x4.h"
#include "vtkImageData.h"

namespace igstk
{

/** Constructor */
template < class TImageSpatialObject >
ImageResliceSpatialObjectRepresentation < TImageSpatialObject >
::ImageResliceSpatialObjectRepresentation():m_StateMachine(this)
{
  m_ImageSpatialObject = NULL;
  m_ReslicePlaneSpatialObject= NULL;

  m_ImageActor = vtkImageActor::New();
  this->AddActor( m_ImageActor );

  m_MapColors  = vtkImageMapToColors::New();
  m_LUT        = vtkLookupTable::New();
  m_ImageData  = NULL;

  m_ImageReslice  = vtkImageReslice::New();

  m_ResliceAxes   = vtkMatrix4x4::New(); 
  m_ResliceAxes->Identity();

  m_ImageReslice->SetResliceAxes( m_ResliceAxes );

  // Set default values for window and level
  m_Level = 0;
  m_Window = 2000;
  
  // Create the observer to VTK image events 
  m_VTKImageObserver = VTKImageObserver::New();
  m_ImageTransformObserver = ImageTransformObserver::New();

  igstkAddInputMacro( ValidImageSpatialObject );
  igstkAddInputMacro( InValidImageSpatialObject );
  igstkAddInputMacro( ValidReslicePlaneSpatialObject);
  igstkAddInputMacro( InValidReslicePlaneSpatialObject  );
  igstkAddInputMacro( ConnectVTKPipeline  );

  igstkAddStateMacro( Initial  );
  igstkAddStateMacro( ImageSpatialObjectSet );
  igstkAddStateMacro( ReslicePlaneSpatialObjectSet );

  //From Initial state
  igstkAddTransitionMacro( Initial, ValidImageSpatialObject, 
                           ImageSpatialObjectSet,  SetImageSpatialObject );
  igstkAddTransitionMacro( Initial, InValidImageSpatialObject, 
                           Initial,  ReportInvalidImageSpatialObject );

  //From ImageSpatialObjectSet
  igstkAddTransitionMacro( ImageSpatialObjectSet, ValidReslicePlaneSpatialObject, 
                           ReslicePlaneSpatialObjectSet, SetReslicePlaneSpatialObject);

  igstkAddTransitionMacro( ImageSpatialObjectSet, InValidReslicePlaneSpatialObject, 
                           ImageSpatialObjectSet, ReportInvalidReslicePlaneSpatialObject );

  igstkAddTransitionMacro( ImageSpatialObjectSet, ConnectVTKPipeline,
                           ImageSpatialObjectSet, ConnectVTKPipeline );
 
  //From ReslicePlaneSpatialObjectSet
  igstkAddTransitionMacro( ReslicePlaneSpatialObjectSet, ConnectVTKPipeline,
                           ReslicePlaneSpatialObjectSet, ConnectVTKPipeline );
 
  //From ReslicePlaneSpatialObjectSet
  igstkAddTransitionMacro( ReslicePlaneSpatialObjectSet, ConnectVTKPipeline,
                           ReslicePlaneSpatialObjectSet, ConnectVTKPipeline );
 
  igstkSetInitialStateMacro( Initial );

  m_StateMachine.SetReadyToRun();
}

/** Destructor */

template < class TImageSpatialObject >
ImageResliceSpatialObjectRepresentation < TImageSpatialObject >
::~ImageResliceSpatialObjectRepresentation()
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

  if( m_ResliceAxes )
    {
    m_ResliceAxes->Delete();
    m_ResliceAxes = NULL;
    }
}

/** Create the vtk Actors */
template < class TImageSpatialObject >
void
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::CreateActors()
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceSpatialObjectRepresentation\
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

/** Overloaded DeleteActor function */
template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::DeleteActors( )
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceSpatialObjectRepresentation\
                        ::DeleteActors called...\n");
  this->Superclass::DeleteActors();
  m_ImageActor = NULL;

}

template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::SetWindowLevel( double window, double level )
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceSpatialObjectRepresentation\
                        ::SetWindowLevel called...\n");

  m_Window = window;
  m_Level = level;

  m_LUT->SetTableRange ( (m_Level - m_Window/2.0), (m_Level + m_Window/2.0) );
}

/** Set the opacity */
template < class TImageSpatialObject >
void
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
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


template < class TImageSpatialObject >
void
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::ConnectVTKPipelineProcessing() 
{
  m_MapColors->SetInput( m_ImageData );
  m_ImageReslice->SetInput ( m_MapColors->GetOutput() );
  m_ImageActor->SetInput( m_ImageReslice->GetOutput() );
  m_ImageActor->InterpolateOn();

}

template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetImageSpatialObject( const ImageSpatialObjectType * imageSpatialObject )
{  
  igstkLogMacro( DEBUG,"igstk::ImageResliceSpatialObjectRepresentation\
                       ::RequestSetImageSpatialObject called...\n");

  m_ImageSpatialObjectToBeSet = const_cast< ImageSpatialObjectType *>(
imageSpatialObject );

  if( !m_ImageSpatialObjectToBeSet )
    {
    m_StateMachine.PushInput( m_InValidImageSpatialObjectInput );
    }
  else
    {
    m_StateMachine.PushInput( m_ValidImageSpatialObjectInput );
    }

  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::SetImageSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageResliceSpatialObjectRepresentation\
                       ::SetImageSpatialObjectProcessing called...\n");

  m_ImageSpatialObject = m_ImageSpatialObjectToBeSet;
  this->RequestSetSpatialObject( m_ImageSpatialObject );

  m_ImageSpatialObject->AddObserver( VTKImageModifiedEvent(), 
                                     m_VTKImageObserver );

  m_ImageSpatialObject->AddObserver( CoordinateSystemTransformToEvent(), 
                                     m_ImageTransformObserver );

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
    this->m_ImageReslice->SetInput( m_MapColors->GetOutput() );
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

  this->m_ImageActor->SetInput( this->m_ImageReslice->GetOutput() );
}

template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::ReportInvalidImageSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageResliceSpatialObjectRepresentation\
                       ::ReportInvalidImageSpatialObjectProcessing called...\n");
}

template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetReslicePlaneSpatialObject( const ReslicePlaneSpatialObjectType *
reslicePlaneSpatialObject )
{  
  igstkLogMacro( DEBUG,"igstk::ImageResliceSpatialObjectRepresentation\
                       ::RequestSetReslicePlaneSpatialObject called...\n");

  m_ReslicePlaneSpatialObjectToBeSet = const_cast< ReslicePlaneSpatialObjectType
*>(reslicePlaneSpatialObject);

  if( !m_ReslicePlaneSpatialObjectToBeSet )
    {
    m_StateMachine.PushInput( m_InValidReslicePlaneSpatialObjectInput );
    }
  else
    {
    m_StateMachine.PushInput( m_ValidReslicePlaneSpatialObjectInput );
    }

  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::SetReslicePlaneSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageResliceSpatialObjectRepresentation\
                       ::SetReslicePlaneSpatialObjectProcessing called...\n");

  m_ReslicePlaneSpatialObject = m_ReslicePlaneSpatialObjectToBeSet;

  /** Get reslicing plane */
  m_ReslicePlaneSpatialObject->RequestComputeReslicingPlane();
  m_ResliceAxes = m_ReslicePlaneSpatialObject->RequestGetReslicingMatrix();

  m_ImageReslice->SetResliceAxes( m_ResliceAxes );
  m_ImageReslice->SetInput( this->m_MapColors->GetOutput() ); 
  m_ImageReslice->SetBackgroundColor( 128.0, 128.0, 128.0, 0 );
  m_ImageReslice->AutoCropOutputOn();
  m_ImageReslice->SetOptimization( 1 );

  m_ImageReslice->SetOutputSpacing( 1, 1, 1 );
  m_ImageReslice->SetOutputDimensionality( 2 );
  m_ImageReslice->SetOutputOrigin( 0, 0, 0 );
  m_ImageActor->SetInput( m_ImageReslice->GetOutput() );  
}

/** Update the visual representation in response to changes in the geometric
 * object */
template < class TImageSpatialObject >
void
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceSpatialObjectRepresentation\
                       ::UpdateRepresentationProcessing called...\n");
  if( m_ImageData )
    {
    m_MapColors->SetInput( m_ImageData );
    }

  /* Updated reslicing: */ 
  if( m_ReslicePlaneSpatialObject )
    {
    m_ReslicePlaneSpatialObject->RequestComputeReslicingPlane();
    m_ResliceAxes = m_ReslicePlaneSpatialObject->RequestGetReslicingMatrix();
    m_ImageReslice->SetResliceAxes( m_ResliceAxes );
    }
}

template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::ReportInvalidReslicePlaneSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageResliceSpatialObjectRepresentation\
                       ::ReportInvalidReslicePlaneSpatialObjectProcessing called...\n");
}


/** Report invalid request */
template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::ReportInvalidRequestProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::ImageResliceSpatialObjectRepresentation::ReportInvalidRequestProcessing called...\n");

  this->InvokeEvent( InvalidRequestErrorEvent() );
}

/** Print Self function */
template < class TImageSpatialObject >
bool
ImageResliceSpatialObjectRepresentation < TImageSpatialObject >
::VerifyTimeStamp( ) const
{
  std::cout << "ImageResliceSpatialObjectRepresentation::VerifyTimeStamp" << std::endl;

  if( this->m_ReslicePlaneSpatialObject.IsNull() )
    {
    return false;
    }

  if( this->GetRenderTimeStamp().GetExpirationTime() <
    this->m_ReslicePlaneSpatialObject->GetToolTransform().GetStartTime() ||
    this->GetRenderTimeStamp().GetStartTime() >
    this->m_ReslicePlaneSpatialObject->GetToolTransform().GetExpirationTime() )
    {
    return false;
    }
  else
    {
    return true;
    }
}

/** Print Self function */
template < class TImageSpatialObject >
void
ImageResliceSpatialObjectRepresentation < TImageSpatialObject >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace igstk

#endif
