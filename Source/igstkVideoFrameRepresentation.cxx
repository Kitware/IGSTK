/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoFrameRepresentation.cxx
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
#include <vtkTexture.h>
#include <vtkActor.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkLookupTable.h>
#include <vtkImageMapToColors.h>
#include <vtkMath.h>
#include <vtkPNGWriter.h>


namespace igstk
{

VideoFrameRepresentation::VideoFrameRepresentation(void) :
  m_StateMachine(this),
  m_ImageData(NULL),
  m_ImageActor(NULL),
  m_PlaneSource(NULL),
  m_VideoMapper(NULL),
  m_VideoTexture(NULL),
  m_ColorMap(NULL),
  m_LookupTable(NULL)
{
  igstkLogMacro( DEBUG, "VideoFrameRepresentation constructor called ...\n");
  
  // We create the image spatial object
  m_VideoFrameSpatialObject = NULL;

  this->RequestSetSpatialObject( m_VideoFrameSpatialObject );

  m_ImageActor = vtkActor::New();
  m_ImageActor->SetPosition(0,0,0);
  m_ImageActor->SetOrientation(0,0,0);
  m_ImageActor->GetProperty()->SetOpacity(1.0);
  this->AddActor( m_ImageActor );

  m_VideoMapper = vtkPolyDataMapper::New();
  m_VideoTexture = vtkTexture::New();
  m_PlaneSource = vtkPlaneSource::New();
  
  m_ColorMap  = vtkImageMapToColors::New();
  m_ColorMap->SetOutputFormatToRGBA();
  m_ColorMap->PassAlphaToOutputOn();

  m_LookupTable = vtkLookupTable::New();  
  m_LookupTable->SetNumberOfColors( 256);
  m_LookupTable->SetHueRange( 0, 0);
  m_LookupTable->SetSaturationRange( 0, 0);
  m_LookupTable->SetValueRange( 0 ,1);
  m_LookupTable->SetAlphaRange( 1, 1);
  m_LookupTable->Build();

  // Create the observer to VTK image events 
  m_VTKImageObserver = VTKImageObserver::New();  

  igstkAddInputMacro( ValidImageSpatialObject );
  igstkAddInputMacro( NullImageSpatialObject  );
  igstkAddInputMacro( EmptyImageSpatialObject  );
  igstkAddInputMacro( ConnectVTKPipeline );

  igstkAddStateMacro( NullImageSpatialObject );
  igstkAddStateMacro( ValidImageSpatialObject );
 
  igstkAddTransitionMacro( NullImageSpatialObject, NullImageSpatialObject,
                           NullImageSpatialObject,  No );
  igstkAddTransitionMacro( NullImageSpatialObject, EmptyImageSpatialObject,
                           NullImageSpatialObject,  No );
  igstkAddTransitionMacro( NullImageSpatialObject, ValidImageSpatialObject,
                           ValidImageSpatialObject,  SetVideoFrameSpatialObject );
  
  igstkAddTransitionMacro( ValidImageSpatialObject, NullImageSpatialObject,
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, EmptyImageSpatialObject,
                           NullImageSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidImageSpatialObject, ValidImageSpatialObject,
                           ValidImageSpatialObject,  No ); 
  
  igstkAddTransitionMacro( NullImageSpatialObject, ConnectVTKPipeline,
                           NullImageSpatialObject, No );
  igstkAddTransitionMacro( ValidImageSpatialObject, ConnectVTKPipeline,
                           ValidImageSpatialObject, ConnectVTKPipeline );

  igstkSetInitialStateMacro( NullImageSpatialObject );

  m_StateMachine.SetReadyToRun();
}

VideoFrameRepresentation::~VideoFrameRepresentation(void)
{
  igstkLogMacro( DEBUG, "VideoFrameRepresentation destructor called ...\n");

  if( m_ColorMap )
  {
    m_ColorMap->SetLookupTable( NULL );
    m_ColorMap->SetInput( NULL );
    m_ColorMap->Delete();
    m_ColorMap = NULL;
  }

  if( m_LookupTable )
  {
    m_LookupTable->Delete();
    m_LookupTable = NULL;
  }

  if ( m_VideoMapper )
  {
    m_VideoMapper->Delete();
  }

  if ( m_VideoTexture )
  {
    m_VideoTexture->Delete();
  }

  if ( m_PlaneSource )
  {
    m_PlaneSource->Delete();
  }
}

/** Overloaded DeleteActors function */
void 
VideoFrameRepresentation
::DeleteActors( )
{
  igstkLogMacro( DEBUG, "igstk::VideoFrameRepresentation\
                        ::DeleteActors called...\n");
  
  this->Superclass::DeleteActors();
  
  m_ImageActor = NULL;
}

/** Null Operation for a State Machine Transition */
void 
VideoFrameRepresentation
::NoProcessing()
{
}

/** Set the Video Spatial Object */
void 
VideoFrameRepresentation
::RequestSetVideoFrameSpatialObject( const VideoFrameSpatialObject * video )
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
void 
VideoFrameRepresentation
::SetVideoFrameSpatialObjectProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoFrameRepresentation\
                        ::SetVideoFrameSpatialObjectProcessing called...\n");

  // We create the video spatial object
  m_VideoFrameSpatialObject = m_VideoFrameSpatialObjectToAdd;

  m_VideoFrameSpatialObject->AddObserver( VTKImageModifiedEvent(), 
                                     m_VTKImageObserver );

  this->RequestSetSpatialObject( m_VideoFrameSpatialObject );
  
  // This method gets a VTK image data from the private method of the
  // ImageSpatialObject and stores it in the representation by invoking the
  // private SetImage method.
  
  m_VTKImageObserver->Reset();

  m_VideoFrameSpatialObject->RequestGetVTKImage();

  if( m_VTKImageObserver->GotVTKImage() ) 
  {
    m_ImageData = m_VTKImageObserver->GetVTKImage();

    if( m_ImageData )
    {
      m_ImageData->Update();      

      double range[2];
      m_ImageData->GetScalarRange(range);

      m_LookupTable->SetTableRange(range[0],range[1]);
      m_LookupTable->Build();

      m_Window = range[1] - range[0];
      m_Level = 0.5*(range[0] + range[1]);
      
      if( fabs( m_Window ) < 0.001 )
        {
        m_Window = 0.001 * ( m_Window < 0.0 ? -1 : 1 );
        }

      if( fabs( m_Level ) < 0.001 )
        {
        m_Level = 0.001 * ( m_Level < 0.0 ? -1 : 1 );
        }

      this->SetWindowLevel( m_Window, m_Level );

      m_ColorMap->SetLookupTable( m_LookupTable );

      m_ColorMap->SetInput( m_ImageData );

      m_VideoTexture->SetInput( m_ColorMap->GetOutput() );
  
      int dims[3];
      m_ImageData->GetDimensions(dims);

      double spacing[3];
      m_ImageData->GetSpacing(spacing);

      m_PlaneSource->SetOrigin(0,0,0);
      m_PlaneSource->SetPoint1(0, (double) dims[0]*spacing[0], 0);
      m_PlaneSource->SetPoint2((double) dims[1]*spacing[1], 0, 0);

      m_PlaneSource->SetCenter(0,0,0);

      m_VideoMapper->SetInput(m_PlaneSource->GetOutput());
    }   
  }
     
  m_ImageActor->SetTexture(m_VideoTexture);
  m_ImageActor->SetMapper(m_VideoMapper);

}


void 
VideoFrameRepresentation
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
void 
VideoFrameRepresentation
::SaveScreenShot( const std::string & filename )
{
  igstkLogMacro( DEBUG, "VideoFrameRepresentation::SaveScreenShot() called ...\n");

  vtkPNGWriter * writer = vtkPNGWriter::New();

  writer->SetInput( m_ColorMap->GetOutput() );
  
  writer->SetFileName( filename.c_str() );
  
  writer->Write();

  writer->SetInput( NULL );

  writer->Delete();
}

void 
VideoFrameRepresentation::
CreateActors()
{
  igstkLogMacro( DEBUG, "igstk::VideoFrameRepresentation"
                 "::CreateActors called...\n");

  // To avoid duplicates we clean the previous actors
  this->DeleteActors();
    
  m_ImageActor = vtkActor::New();
  m_ImageActor->SetPosition(0,0,0);
  m_ImageActor->SetOrientation(0,0,0);
  m_ImageActor->GetProperty()->SetOpacity(1.0);

  m_ImageActor->SetTexture(m_VideoTexture);
  m_ImageActor->SetMapper(m_VideoMapper);

  this->AddActor( m_ImageActor );  

    
  m_StateMachine.ProcessInputs(); 
}

void 
VideoFrameRepresentation::
UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoFrameRepresentation"
                 "::UpdateRepresentationProcessing called...\n");

  m_ImageData->Modified();
}

void
VideoFrameRepresentation
::ConnectVTKPipelineProcessing() 
{
  m_ImageActor->SetTexture(m_VideoTexture);
  m_ImageActor->SetMapper(m_VideoMapper);
  m_ImageActor->SetVisibility( 1 );
  m_ImageActor->SetPickable( 0 );  
}

/** Create a copy of the current object representation */
VideoFrameRepresentation::Pointer
VideoFrameRepresentation
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
