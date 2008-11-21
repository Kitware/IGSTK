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
  m_MapColors(NULL),
  m_LUT(NULL)
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
  m_MapColors  = vtkImageMapToColors::New();
  m_LUT        = vtkLookupTable::New();  

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

      // Set default values for window and level
      m_Window = 286;
      m_Level = 108;     
      
      m_MapColors->SetLookupTable( m_LUT );

      m_MapColors->SetInput( m_ImageData );

      m_VideoTexture->SetInput( m_MapColors->GetOutput() );
  
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

  m_LUT->SetTableRange ( (m_Level - m_Window/2.0), (m_Level + m_Window/2.0) );
}

/** Save current screenshot */
void 
VideoFrameRepresentation
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

  // Set default values for window and level
  m_Window = 286;
  m_Level = 108;  
  
  m_MapColors->SetLookupTable( m_LUT );
    
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
