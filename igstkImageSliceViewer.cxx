/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkImageSliceViewer.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm 
for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkImageSliceViewer.h"

#include "vtkImageData.h"
#include "vtkInteractorStyleImage.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"

#include "igstkInteractorObserver.h"
#include "itkPoint.h"
#include "igstkClickedPointEvent.h"

namespace igstk
{


ImageSliceViewer
::ImageSliceViewer()
{
	// Connect the state machine to 'this' ImageSliceViewer class.
  m_StateMachine.SetClass( this );

	// Set the state descriptors
	m_StateMachine.SetStateDescriptor( 0, "ImageNotLoadedState" );
	m_StateMachine.SetStateDescriptor( 1, "ImageLoadedState" );

	// Set the input descriptors
	m_StateMachine.SetInputDescriptor( 0, "ImageData");
	m_StateMachine.SetInputDescriptor( 1, "SetUpCamera");
  m_StateMachine.SetInputDescriptor( 2, "SetSlice");
  m_StateMachine.SetInputDescriptor( 3, "SetPoint");

	const ActionType NoAction = 0;

	// Programming the machine
	m_StateMachine.SetTransition( "ImageNotLoadedState", "ImageData", 
		"ImageLoadedState", & ImageSliceViewer::UpdateImageInformation );

	m_StateMachine.SetTransition( "ImageLoadedState", "ImageData", 
		"ImageLoadedState", & ImageSliceViewer::UpdateImageInformation );

  m_StateMachine.SetTransition( "ImageNotLoadedState", "SetUpCamera", 
    "ImageLoadedState", NoAction );

  m_StateMachine.SetTransition( "ImageLoadedState", "SetUpCamera", 
    "ImageLoadedState", & ImageSliceViewer::SetupCamera );

  m_StateMachine.SetTransition( "ImageNotLoadedState", "SetSlice", 
    "ImageLoadedState", NoAction );

  m_StateMachine.SetTransition( "ImageLoadedState", "SetSlice", 
    "ImageLoadedState", & ImageSliceViewer::SetSlice );

  m_StateMachine.SetTransition( "ImageNotLoadedState", "SetPoint", 
    "ImageLoadedState", NoAction );

  m_StateMachine.SetTransition( "ImageLoadedState", "SetPoint", 
    "ImageLoadedState", & ImageSliceViewer::SetPoint );

	// Finish the state machine programming and get ready to run
	m_StateMachine.SetReadyToRun();

  // Make the vtk Visualization pipeline
  m_Actor         = vtkImageActor::New();
  m_Renderer      = vtkRenderer::New();
  m_RenderWindow  = vtkRenderWindow::New();
  m_Renderer->AddActor( m_Actor );
  m_Actor->Delete();

  m_Camera = m_Renderer->GetActiveCamera();
  m_Camera->ParallelProjectionOn();

  m_RenderWindow->AddRenderer( m_Renderer );
  m_Renderer->Delete();

  this->SetOrientation( Axial );

  // Define an interactor/observer for mouse events in the viewer
  InteractorObserver * observer = InteractorObserver::New();
  observer->SetImageSliceViewer( this );
  m_InteractorObserver = observer;
  m_Notifier      = itk::Object::New();

  m_NearPlane = 0.1;
  m_FarPlane  = 100000.0;
  m_ZoomFactor = 1.0;
  m_Renderer->SetBackground( 0, 0, 0 );
}



ImageSliceViewer
::~ImageSliceViewer()
{
  if( m_RenderWindow )
    {
    m_RenderWindow->Delete();
    }   

  if( m_InteractorObserver )
    {
    m_InteractorObserver->Delete();
    }
}



void
ImageSliceViewer
::Render()
{
  m_Camera->SetClippingRange( m_NearPlane, m_FarPlane );
  m_RenderWindow->Render();
}



vtkRenderer *
ImageSliceViewer
::GetRenderer()
{
  return m_Renderer;
}


void
ImageSliceViewer
::SetInput( vtkImageData * image )
{
  m_Actor->SetInput( image );
  m_StateMachine->SetInput( "ImageData" );
  m_StateMachine->StateTransition();
  m_StateMachine->SetInput( "SetUpCamera" );
  m_StateMachine->StateTransition();
}


void
ImageSliceViewer
::SetZoomFactor( double factor )
{
  m_ZoomFactor = factor;
  m_StateMachine->SetInput( "SetUpCamera" );
  m_StateMachine->StateTransition();
}



void
ImageSliceViewer
::SetInteractor( vtkRenderWindowInteractor * interactor )
{
  m_RenderWindow->SetInteractor( interactor );

  vtkInteractorStyleImage * interactorStyle = vtkInteractorStyleImage::New();
  interactor->SetInteractorStyle( interactorStyle );
  interactorStyle->Delete();
  interactor->AddObserver( ::vtkCommand::LeftButtonPressEvent, m_InteractorObserver );
  interactor->AddObserver( ::vtkCommand::LeftButtonReleaseEvent, m_InteractorObserver );
  interactor->AddObserver( ::vtkCommand::MouseMoveEvent, m_InteractorObserver );
}


void
ImageSliceViewer
::SetOrientation( OrientationType orientation )
{
  m_Orientation = orientation;
  m_StateMachine->SetInput( "SetUpCamera" );
  m_StateMachine->StateTransition();
}



void
ImageSliceViewer
::SelectSlice( int slice )
{
  m_SliceNum = slice;
  m_StateMachine->SetInput( "SetSlice" );
  m_StateMachine->StateTransition();
}



void
ImageSliceViewer
::SelectPoint( int x, int y )
{
  m_SelectPoint[0] = x ; 
  m_SelectPoint[1] = y ;
  m_StateMachine->SetInput( "SetPoint" );
  m_StateMachine->StateTransition();
}



void
ImageSliceViewer
::SetSlice( void )
{
  int ext[6];
  m_Actor->GetInput()->GetExtent( ext );

  switch( m_Orientation )
    {
  case Saggital:
    {
      if(m_SliceNum<ext[0]) m_SliceNum = ext[0];
      if(m_SliceNum>ext[1]) m_SliceNum = ext[1];
      ext[0] = m_SliceNum;
      ext[1] = m_SliceNum;
      break;
    }
  case Coronal:
    {
      if(m_SliceNum<ext[2]) m_SliceNum = ext[2];
      if(m_SliceNum>ext[3]) m_SliceNum = ext[3];
      ext[2] = m_SliceNum;
      ext[3] = m_SliceNum;
      break;
    }
  case Axial:
    {
      if(m_SliceNum<ext[4]) m_SliceNum = ext[4];
      if(m_SliceNum>ext[5]) m_SliceNum = ext[5];
      ext[4] = m_SliceNum;
      ext[5] = m_SliceNum;
      break;
    }
    }

  m_Actor->SetDisplayExtent( ext );
}




void
ImageSliceViewer
::UpdateImageInformation( void )
{
  vtkImageData * image = m_Actor->GetInput();
  image->GetSpacing(m_ImagePixelSpacing);
  image->GetOrigin(m_ImageOrigin);
  image->GetDimensions(m_ImageDimensions);
  image->GetExtent( m_ImageExtents );
  m_SliceNum = m_Dimensions[m_Orientation]/2;
}




void
ImageSliceViewer
::SetupCamera( void )
{
  float focalPoint[3];
  float position[3];

  for ( unsigned int cc = 0; cc < 3; cc++)
    {
    focalPoint[cc] = m_ImageOrigin[cc] + 
      ( m_ImagePixelSpacing[cc] * m_ImageDimensions[cc] ) / 2.0;
    position[cc]   = focalPoint[cc];
    }

  int idx = 0;
  switch( m_Orientation )
    {
  case Saggital:
      {
      idx = 0;
      m_Camera->SetViewUp (     0,  0,  -1 );
      break;
      }
  case Coronal:
      {
      idx = 1;
      m_Camera->SetViewUp (     0,  0,  -1 );
      break;
      }
  case Axial:
      {
      idx = 2;
      m_Camera->SetViewUp (     0,  -1,  0 );
      break;
      }
    }

  const float distanceToFocalPoint = 1000;
  position[idx] += distanceToFocalPoint;

  m_Camera->SetPosition (   position );
  m_Camera->SetFocalPoint ( focalPoint );

#define myMAX(x,y) (((x)>(y))?(x):(y))  

   int d1 = (idx + 1) % 3;
   int d2 = (idx + 2) % 3;
 
  float max = myMAX( 
    spacing[d1] * dimensions[d1],
    spacing[d2] * dimensions[d2]);

#undef myMAX

  m_Camera->SetParallelScale( max / 2  * m_ZoomFactor );

}


void
ImageSliceViewer
::SetPoint( void )
{

  int x = m_SelectPoint[0]; 
  int y = m_SelectPoint[1];
  
  // Invert the y coordinate (vtk uses opposite y as FLTK)
  int* winsize = m_RenderWindow->GetSize();
  y = winsize[1] - y;

  // Convert display point to world point
  float wpoint[4];
  const double z = m_SliceNum / ( m_FarPlane - m_NearPlane );
  m_Renderer->SetDisplayPoint( x, y, 0 );
  m_Renderer->DisplayToWorld();
  m_Renderer->GetWorldPoint( wpoint );

  // Fix camera Z coorinate to match the current slice
  float spacing[3]={1,1,1};
  float origin[3] ={0,0,0};
  int dimensions[3] = { 100, 100, 100 };
  if ( m_Actor->GetInput() )
    {
    m_Actor->GetInput()->GetSpacing(spacing);
    m_Actor->GetInput()->GetOrigin(origin);
    m_Actor->GetInput()->GetDimensions(dimensions);
    }

  int idx = 0;
  switch( m_Orientation )
    {
  case Saggital:
      {
      idx = 0;
      break;
      }
  case Coronal:
      {
      idx = 1;
      break;
      }
  case Axial:
      {
      idx = 2;
      break;
      }
    }
  float realz = m_SliceNum * spacing[idx] + origin[idx];
  wpoint[idx] = realz;

  // At this point we have 3D position in the variable wpoint
  this->SetClickedPoint(wpoint[0], wpoint[1], wpoint[2]);

  m_Notifier->InvokeEvent( ClickedPointEvent() );
}




void  
ImageSliceViewer
::SelectPoint( float x, float y, float z )
{
  m_SelectPoint[0] = x;
  m_SelectPoint[1] = y;
  m_SelectPoint[2] = z;
}




void 
ImageSliceViewer
::GetSelectPoint(float data[3])
{
  for(int i=0; i<3; i++)
  {
    data[i] = m_SelectPoint[i];
  }
}




unsigned long 
ImageSliceViewer
::AddObserver( const itk::EventObject & event, itk::Command * command)
{
  return m_Notifier->AddObserver( event, command );
}


}

