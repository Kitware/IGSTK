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
#ifndef __igstkVideoFrameRepresentation_txx
#define __igstkVideoFrameRepresentation_txx

#include "igstkVideoFrameRepresentation.h"

#include <vtkProperty.h>
#include <vtkImageActor.h>
#include <vtkMath.h>
#include <vtkPNGWriter.h>
#include <vtkLookupTable.h>
#include <vtkImageMapToColors.h>

namespace igstk
{

template < class TVideoFrameSpatialObject >
VideoFrameRepresentation< TVideoFrameSpatialObject >
::VideoFrameRepresentation(void):
  m_StateMachine(this),
  m_ImageData(NULL),
  m_ImageActor(NULL),
  m_LookupTable(NULL),
  m_MapColors(NULL)
{
  igstkLogMacro( DEBUG, "VideoFrameRepresentation constructor called ...\n");

  // We create the VideoFrame spatial object
  m_VideoFrameSpatialObject = NULL;

  this->RequestSetSpatialObject( m_VideoFrameSpatialObject );

  m_ImageActor = vtkImageActor::New();
  m_ImageActor->SetZSlice(0);
  m_ImageActor->SetPosition(0,0,0);
  m_ImageActor->SetOrientation(0,0,0);

  this->AddActor( m_ImageActor );

  //window level
  m_LookupTable = vtkLookupTable::New();
  m_MapColors   = vtkImageMapToColors::New();

  // Create the observer to VTK image events
  m_VTKImageObserver = VTKImageObserver::New();

  igstkAddInputMacro( ValidVideoFrameSpatialObject );
  igstkAddInputMacro( NullVideoFrameSpatialObject  );
  igstkAddInputMacro( EmptyVideoFrameSpatialObject  );
  igstkAddInputMacro( ConnectVTKPipeline );

  igstkAddStateMacro( NullVideoFrameSpatialObject );
  igstkAddStateMacro( ValidVideoFrameSpatialObject );

  igstkAddTransitionMacro( NullVideoFrameSpatialObject,
                           NullVideoFrameSpatialObject,
                           NullVideoFrameSpatialObject, No );
  igstkAddTransitionMacro( NullVideoFrameSpatialObject,
                           EmptyVideoFrameSpatialObject,
                           NullVideoFrameSpatialObject, No );
  igstkAddTransitionMacro( NullVideoFrameSpatialObject,
                           ValidVideoFrameSpatialObject,
                           ValidVideoFrameSpatialObject,
                           SetVideoFrameSpatialObject );
  igstkAddTransitionMacro( NullVideoFrameSpatialObject, ConnectVTKPipeline,
                           NullVideoFrameSpatialObject, No );

  igstkAddTransitionMacro( ValidVideoFrameSpatialObject,
                           NullVideoFrameSpatialObject,
                           NullVideoFrameSpatialObject,  No );
  igstkAddTransitionMacro( ValidVideoFrameSpatialObject,
                           EmptyVideoFrameSpatialObject,
                           NullVideoFrameSpatialObject,  No );
  igstkAddTransitionMacro( ValidVideoFrameSpatialObject,
                           ValidVideoFrameSpatialObject,
                           ValidVideoFrameSpatialObject, No );
  igstkAddTransitionMacro( ValidVideoFrameSpatialObject, ConnectVTKPipeline,
                           ValidVideoFrameSpatialObject, ConnectVTKPipeline );

  igstkSetInitialStateMacro( NullVideoFrameSpatialObject );

  m_StateMachine.SetReadyToRun();
}

template< class TVideoFrameSpatialObject >
VideoFrameRepresentation< TVideoFrameSpatialObject>
::~VideoFrameRepresentation(void)
{
  igstkLogMacro( DEBUG, "VideoFrameRepresentation destructor called ...\n");
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

  unsigned int jitter = 5000;

  if( this->GetRenderTimeStamp().GetExpirationTime() + jitter <
    this->m_VideoFrameSpatialObject->GetFrameStartTime() ||
    (this->GetRenderTimeStamp().GetStartTime() - jitter) >
    this->m_VideoFrameSpatialObject->GetFrameExpirationTime() )
    {
    return false;
    }
  else
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
    m_StateMachine.PushInput( m_NullVideoFrameSpatialObjectInput );
    }
  else
    {
    if( m_VideoFrameSpatialObjectToAdd->IsEmpty() )
      {
      m_StateMachine.PushInput( m_EmptyVideoFrameSpatialObjectInput );
      }
    else
      {
      m_StateMachine.PushInput( m_ValidVideoFrameSpatialObjectInput );
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
    this->m_ImageActor->SetInput( this->m_ImageData  );

    if( m_VideoFrameSpatialObject->GetNumberOfChannels() == 1 )
      {
      this->m_MapColors->SetInput( this->m_ImageData );
      this->m_ImageActor->SetInput( this->m_MapColors->GetOutput() );
      }
    }
  else
    {
    igstkLogMacro( DEBUG, "igstk::VideoFrameRepresentation"
              << "::SetVideoFrameSpatialObjectProcessing: No VTKImage Event\n");
    }
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
  m_LookupTable->SetTableRange( (m_Level - m_Window/2.0),
                                (m_Level + m_Window/2.0) );
  m_LookupTable->Build();
}

/** Save current screenshot */
template< class TVideoFrameSpatialObject >
void
VideoFrameRepresentation< TVideoFrameSpatialObject>
::SaveScreenShot( const std::string & filename )
{
  igstkLogMacro( DEBUG,
                     "VideoFrameRepresentation::SaveScreenShot() called ...\n");

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

  double hue = 0.0;
  double saturation = 0.0;
  double value = 1.0;

  vtkMath::RGBToHSV( this->GetRed(),
                     this->GetGreen(),
                     this->GetBlue(),
                     &hue,&saturation,&value );

  m_LookupTable->SetSaturationRange (saturation, saturation);
  m_LookupTable->SetAlphaRange (m_Opacity, m_Opacity);
  m_LookupTable->SetHueRange (hue, hue);
  m_LookupTable->SetValueRange (0, value);
  m_LookupTable->SetRampToLinear();

  if(m_VideoFrameSpatialObject->GetNumberOfChannels() == 1)
    {
    m_MapColors->SetLookupTable( m_LookupTable );
    }

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
      if(m_VideoFrameSpatialObject->GetNumberOfChannels() == 1)
        {
        m_MapColors->SetInput( m_ImageData );
        m_MapColors->Update();
        }
      else if(m_VideoFrameSpatialObject->GetNumberOfChannels() == 3)
        {
        m_ImageActor->SetInput( m_ImageData );
        }
      else
        {
        igstkLogMacro( DEBUG, "igstk::VideoFrameRepresentation"
                 << "::UpdateRepresentationProcessing: Number of channel is not"
                                           << "supported. Should be 1 or 3!\n");
        }
      }
    }
}

template< class TVideoFrameSpatialObject >
void
VideoFrameRepresentation< TVideoFrameSpatialObject>
::ConnectVTKPipelineProcessing()
{
  if(m_VideoFrameSpatialObject->GetNumberOfChannels() == 1)
    {
    m_MapColors->SetInput( m_ImageData );
    m_ImageActor->SetInput( m_MapColors->GetOutput() );
    }
  else if(m_VideoFrameSpatialObject->GetNumberOfChannels() == 3)
    {
    m_ImageActor->SetInput( m_ImageData );
    m_ImageActor->InterpolateOn();
    }
  else
    {
    igstkLogMacro( DEBUG, "igstk::VideoFrameRepresentation"
      << "::UpdateRepresentationProcessing: "
      << "Number of channel is not supported. Should be 1 or 3!\n");
    }
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

#endif
