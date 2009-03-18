/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoFrameRepresentation.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

  Made by SINTEF Health Research - Medical technology:
  http://www.sintef.no/medtech

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkVideoFrameRepresentation.h"

#include <vtkProperty.h>
#include <vtkImageActor.h>
#include <vtkMath.h>
#include <vtkPNGWriter.h>
#include <vtkLookupTable.h>
#include <vtkImageMapToColors.h>

//TODO
#include "itkImageFileWriter.h"


namespace igstk
{

template < class TVideoFrameSpatialObject >
VideoFrameRepresentation< TVideoFrameSpatialObject >
::VideoFrameRepresentation(void):
  m_StateMachine(this)//,
//  m_ImageData(NULL),
//  m_ImageActor(NULL),
//  m_MapColors(NULL),
//  m_LookupTable(NULL)
{
  igstkLogMacro( DEBUG, "VideoFrameRepresentation constructor called ...\n");

  // We create the image spatial object
  m_VideoFrameSpatialObject = NULL;

  this->RequestSetSpatialObject( m_VideoFrameSpatialObject );

  m_ImageActor = vtkImageActor::New();
  m_ImageActor->SetZSlice(0);
  m_ImageActor->SetPosition(0,0,0);
  m_ImageActor->SetOrientation(0,0,0);
  //TODO beyond a specific size the OS crashes maybe justify this
  //m_ImageActor->SetDisplayExtent(0,640-1,0,480-1,0,1-1);

  this->AddActor( m_ImageActor );

  //window level
  m_LookupTable = vtkLookupTable::New();
  m_MapColors  = vtkImageMapToColors::New();

  // Create the observer to VTK image events
  m_VTKImageObserver = VTKImageObserver::New();

  igstkAddInputMacro( ValidImageSpatialObject );
  igstkAddInputMacro( NullImageSpatialObject  );
  igstkAddInputMacro( EmptyImageSpatialObject  );
  igstkAddInputMacro( ConnectVTKPipeline );

  igstkAddStateMacro( NullImageSpatialObject );
  igstkAddStateMacro( ValidImageSpatialObject );

  igstkAddTransitionMacro( NullImageSpatialObject, NullImageSpatialObject,
                           NullImageSpatialObject, No );
  igstkAddTransitionMacro( NullImageSpatialObject, EmptyImageSpatialObject,
                           NullImageSpatialObject, No );
  igstkAddTransitionMacro( NullImageSpatialObject, ValidImageSpatialObject,
                           ValidImageSpatialObject,SetVideoFrameSpatialObject );

  igstkAddTransitionMacro( ValidImageSpatialObject, NullImageSpatialObject,
                           NullImageSpatialObject,  No );
  igstkAddTransitionMacro( ValidImageSpatialObject, EmptyImageSpatialObject,
                           NullImageSpatialObject,  No );
  igstkAddTransitionMacro( ValidImageSpatialObject, ValidImageSpatialObject,
                           ValidImageSpatialObject, No );

  igstkAddTransitionMacro( NullImageSpatialObject, ConnectVTKPipeline,
                           NullImageSpatialObject, No );
  igstkAddTransitionMacro( ValidImageSpatialObject, ConnectVTKPipeline,
                           ValidImageSpatialObject, ConnectVTKPipeline );

  igstkSetInitialStateMacro( NullImageSpatialObject );

  m_StateMachine.SetReadyToRun();
}

template< class TVideoFrameSpatialObject >
VideoFrameRepresentation< TVideoFrameSpatialObject>
::~VideoFrameRepresentation(void)
{
  igstkLogMacro( DEBUG, "VideoFrameRepresentation destructor called ...\n");

  m_VTKImageObserver->Delete();
  m_VTKImageObserver = NULL;
  if( m_LookupTable )
  {
    m_LookupTable->Delete();
    m_LookupTable = NULL;
  }
  if( m_MapColors )
  {
    m_MapColors->SetLookupTable( NULL );
    m_MapColors->SetInput( NULL );
    m_MapColors->Delete();
    m_MapColors = NULL;
  }
}

/** Overloaded DeleteActors function */
template< class TVideoFrameSpatialObject >
void
VideoFrameRepresentation< TVideoFrameSpatialObject>
::DeleteActors( )
{
  igstkLogMacro( DEBUG, "igstk::VideoFrameRepresentation\
                        ::DeleteActors called...\n");
  this->Superclass::DeleteActors();
  m_ImageActor = NULL;
}

template< class TVideoFrameSpatialObject >
bool
VideoFrameRepresentation< TVideoFrameSpatialObject>
::VerifyTimeStamp( ) const
{
  igstkLogMacro( DEBUG,
    "igstk::VideoFrameRepresentation::VerifyTimeStamp called...\n");
//TODO
//  if( this->GetRenderTimeStamp().GetExpirationTime() <
//    this->m_VideoFrameSpatialObject->GetFrameStartTime() ||
//      this->GetRenderTimeStamp().GetStartTime() >
//      this->m_VideoFrameSpatialObject->GetFrameExpirationTime() )
//  {
//    return false;
//  }
//  else
  {
    return true;
  }
}

/** Null Operation for a State Machine Transition */
template< class TVideoFrameSpatialObject >
void
VideoFrameRepresentation< TVideoFrameSpatialObject>
::NoProcessing()
{
}

/** Set the Video Spatial Object */
template< class TVideoFrameSpatialObject >
void
VideoFrameRepresentation< TVideoFrameSpatialObject >
::RequestSetVideoFrameSpatialObject( const VideoFrameSpatialObjectType * video )
{
   igstkLogMacro( DEBUG, "igstk::VideoFrameRepresentation\
                        ::RequestSetVideoFrameSpatialObject called...\n");

  m_VideoFrameSpatialObjectToAdd = video;

  if( !m_VideoFrameSpatialObjectToAdd )
  {
    m_StateMachine.PushInput( m_NullImageSpatialObjectInput );
  }
  else
  {
    if( m_VideoFrameSpatialObjectToAdd->IsEmpty() )
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


/** Set the Ultrasound Spatial Object */
template< class TVideoFrameSpatialObject >
void
VideoFrameRepresentation< TVideoFrameSpatialObject>
::SetVideoFrameSpatialObjectProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoFrameRepresentation\
                        ::SetVideoFrameSpatialObjectProcessing called...\n");

  // We create the video spatial object
  m_VideoFrameSpatialObject = m_VideoFrameSpatialObjectToAdd;

  m_VideoFrameSpatialObject->AddObserver( VTKImageModifiedEvent(),
                                     m_VTKImageObserver );

  this->RequestSetSpatialObject( m_VideoFrameSpatialObject );

  m_VTKImageObserver->Reset();

  m_VideoFrameSpatialObject->RequestGetVTKImage();

  if( m_VTKImageObserver->GotVTKImage() )
  {
    m_ImageData = m_VTKImageObserver->GetVTKImage();

//    if( m_ImageData )
//    {
//      m_ImageData->Update();
//      cout << m_ImageData->GetNumberOfPoints() << "getnumberofpoints";
//    }
    this->m_MapColors->SetInput( this->m_ImageData );
  }
  this->m_ImageActor->SetInput( this->m_MapColors->GetOutput() );
}

template< class TVideoFrameSpatialObject >
void
VideoFrameRepresentation< TVideoFrameSpatialObject>
::SetWindowLevel( double window, double level )
{
  igstkLogMacro( DEBUG, "igstk::VideoFrameRepresentation\
                        ::SetWindowLevel called...\n");

  m_Window = window;
  m_Level = level;

  m_LookupTable->SetTableRange ( (m_Level - m_Window/2.0), (m_Level + m_Window/2.0) );
  m_LookupTable->Build();
}

/** Save current screenshot */
template< class TVideoFrameSpatialObject >
void
VideoFrameRepresentation< TVideoFrameSpatialObject>
::SaveScreenShot( const std::string & filename )
{
  igstkLogMacro( DEBUG, "VideoFrameRepresentation::SaveScreenShot() called ...\n");

  vtkPNGWriter * writer = vtkPNGWriter::New();
  writer->SetInput( m_MapColors->GetOutput() );
  writer->SetFileName( filename.c_str() );
  writer->Write();
  writer->SetInput( NULL );
  writer->Delete();
}

template< class TVideoFrameSpatialObject >
void
VideoFrameRepresentation< TVideoFrameSpatialObject>
::CreateActors()
{
  igstkLogMacro( DEBUG, "igstk::VideoFrameRepresentation"
                 "::CreateActors called...\n");

//  //  to avoid duplicates we clean the previous actors
//  this->DeleteActors();
//
//  m_ImageActor = vtkImageActor::New();
//  m_ImageActor->SetPosition(0,0,0);
//  m_ImageActor->SetOrientation(0,0,0);
//  m_ImageActor->SetZSlice(0);
//
//  //TODO beyond a specific size the OS crashes maybe justify this
//  //m_ImageActor->SetDisplayExtent(0,640-1,0,480-1,0,1-1);
//
//   this->AddActor( m_ImageActor );

    double hue = 0.0;
    double saturation = 0.0;
    double value = 1.0;

    vtkMath::RGBToHSV( this->GetRed(),
                       this->GetGreen(),
                       this->GetBlue(),
                       &hue,&saturation,&value );

    m_LookupTable->SetTableRange ( (m_Level - m_Window/2.0), (m_Level + m_Window/2.0) );
    m_LookupTable->SetSaturationRange (saturation, saturation);
    m_LookupTable->SetAlphaRange (m_Opacity, m_Opacity);
    m_LookupTable->SetHueRange (hue, hue);
    m_LookupTable->SetValueRange (0, value);
    m_LookupTable->SetRampToLinear();


    //TODO according to channel amount connect colormapper with lookuptable
    //m_MapColors->SetLookupTable( m_LookupTable );

    igstkPushInputMacro( ConnectVTKPipeline );
    m_StateMachine.ProcessInputs();
}

template< class TVideoFrameSpatialObject >
void
VideoFrameRepresentation< TVideoFrameSpatialObject>
::UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoFrameRepresentation"
                 "::UpdateRepresentationProcessing called...\n");

  m_VTKImageObserver->Reset();

  m_VideoFrameSpatialObject->RequestGetVTKImage();

  if( m_VTKImageObserver->GotVTKImage() )
  {
    m_ImageData = m_VTKImageObserver->GetVTKImage();

    if( m_ImageData )
    {
      //m_ImageData->Update();
      //m_ImageActor->SetInput(m_ImageData);
      m_MapColors->SetInput( m_ImageData );
      m_MapColors->Update();
      //cout << "o" << endl;
    }
  }
}

template< class TVideoFrameSpatialObject >
void
VideoFrameRepresentation< TVideoFrameSpatialObject>
::ConnectVTKPipelineProcessing()
{
  m_MapColors->SetInput( m_ImageData );
  m_ImageActor->SetInput( m_MapColors->GetOutput() );
  //TODO faster?
  //m_ImageActor->InterpolateOn();
}

/** Create a copy of the current object representation */
template< class TVideoFrameSpatialObject >
typename VideoFrameRepresentation< TVideoFrameSpatialObject>::Pointer
VideoFrameRepresentation< TVideoFrameSpatialObject>
::Copy() const
{
  igstkLogMacro( DEBUG, "igstk::VideoFrameRepresentation\
                        ::Copy called...\n");

  Pointer newOR = VideoFrameRepresentation::New();
  newOR->SetColor( this->GetRed(),this->GetGreen(),this->GetBlue() );
  newOR->SetOpacity( this->GetOpacity() );
  newOR->RequestSetVideoFrameSpatialObject( m_VideoFrameSpatialObject );

  return newOR;
}

} // namespace
