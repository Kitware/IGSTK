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
  m_StateMachine.SetOwnerClass( this );

	// Set the state descriptors
	m_StateMachine.AddState( "ImageNotLoadedState" );
	m_StateMachine.AddState( "RenderNeedsRefreshState" );
	m_StateMachine.AddState( "RenderUpdatedState" );

	// Set the input descriptors
	m_StateMachine.AddInput( "NewImageDataInput");
	m_StateMachine.AddInput( "NullImageData");
	m_StateMachine.AddInput( "SetUpCamera");
  m_StateMachine.AddInput( "SelectSlice");
  m_StateMachine.AddInput( "SelectSliceUnderFlow");
  m_StateMachine.AddInput( "SelectSliceOverFlow");
  m_StateMachine.AddInput( "SelectPoint");
  m_StateMachine.AddInput( "RefreshRenderRequest");

	const ActionType NoAction = 0;

//  m_StateMachine.SetValidator( "SelectSliceInput", & ImageSliceViewer::ValidateSliceNumber );

//  m_StateMachine.SetEntryAction( "RenderNeedsRefreshState", & ImageSliceViewer::EntryActionA );

	// Programming the machine
  m_StateMachine.AddTransition( "ImageNotLoadedState",     "RefreshRenderRequest", "ImageNotLoadedState",         NoAction );
  m_StateMachine.AddTransition( "ImageNotLoadedState",     "SetUpCamera",          "ImageNotLoadedState",         NoAction );
  m_StateMachine.AddTransition( "ImageNotLoadedState",     "SelectSlice",          "ImageNotLoadedState",         NoAction );
  m_StateMachine.AddTransition( "ImageNotLoadedState",     "SelectPoint",          "ImageNotLoadedState",         NoAction );
  m_StateMachine.AddTransition( "ImageNotLoadedState",     "NewImageData",         "RenderNeedsRefreshState",     NoAction );
  m_StateMachine.AddTransition( "ImageNotLoadedState",     "NullImageData",        "ImageNotLoadedState",         NoAction );

  m_StateMachine.AddTransition( "RenderNeedsRefreshState", "SelectSlice",          "RenderNeedsRefreshState",   & ImageSliceViewer::SetSlice );
  m_StateMachine.AddTransition( "RenderNeedsRefreshState", "SelectPoint",          "RenderNeedsRefreshState",   & ImageSliceViewer::SetPoint );
  m_StateMachine.AddTransition( "RenderNeedsRefreshState", "NullImageData",        "ImageNotLoadedState",         NoAction );
  m_StateMachine.AddTransition( "RenderNeedsRefreshState", "RefreshRenderRequest", "RenderUpdatedState",             & ImageSliceViewer::Render ); 
  m_StateMachine.AddTransition( "RenderNeedsRefreshState", "NewImageData",         "RenderNeedsRefreshState",     NoAction );
  m_StateMachine.AddTransition( "RenderNeedsRefreshState", "SetUpCamera",          "RenderNeedsRefreshState",   & ImageSliceViewer::SetupCamera );    

  m_StateMachine.AddTransition( "RenderUpdatedState",      "SelectSlice",          "RenderNeedsRefreshState",   & ImageSliceViewer::SetSlice );
  m_StateMachine.AddTransition( "RenderUpdatedState",      "SelectPoint",          "RenderNeedsRefreshState",   & ImageSliceViewer::SetPoint );
  m_StateMachine.AddTransition( "RenderUpdatedState",      "NullImageData",        "ImageNotLoadedState",         NoAction );
  m_StateMachine.AddTransition( "RenderUpdatedState",      "RefreshRenderRequest", "RenderUpdatedState",               NoAction );
  m_StateMachine.AddTransition( "RenderUpdatedState",      "NewImageData",         "RenderNeedsRefreshState",     NoAction );
  m_StateMachine.AddTransition( "RenderUpdatedState",      "SetUpCamera",          "RenderNeedsRefreshState",   & ImageSliceViewer::SetupCamera );    



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
::RefreshRender()
{
  m_StateMachine.ProcessInput( "RenderRequest" );
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
::SetImage( const vtkImageData * image )
{

  //  
  vtkImageData * nonconstImage = const_cast< vtkImageData * >( image );

  if( image == 0 )
    {
    m_StateMachine.ProcessInput( "NullImageData" );
    return;
    }
  m_Actor->SetInput( nonconstImage );
  m_StateMachine.ProcessInput( "NewImageData" );
  m_StateMachine.ProcessInput( "SetUpCamera" );
}


void
ImageSliceViewer
::SetZoomFactor( double factor )
{
  m_ZoomFactor = factor;
  m_StateMachine.ProcessInput( "SetUpCamera" );
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
  m_StateMachine.ProcessInput( "SetUpCamera" );
}



void
ImageSliceViewer
::SelectSlice( int slice )
{
  m_SliceNum = slice;
  // check range
  if( slice < 100 ) 
    {
    m_StateMachine.ProcessInput( "SelectSlice" );
    }
  else
    {
    m_StateMachine.ProcessInput("InvalidSelectSlice");
    }
}



void
ImageSliceViewer
::SelectScreenPixel( int x, int y )
{
  m_InputPoint[0] = x ; 
  m_InputPoint[1] = y ;
  m_StateMachine.ProcessInput( "SelectPoint" );
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
::SetupCamera( void )
{

   vtkImageData * image = m_Actor->GetInput();

  if ( !image )
    {
    return;
    }

  vtkFloatingPointType spacing[3];
  vtkFloatingPointType origin[3];
  int   dimensions[3];


  image->GetSpacing(spacing);
  image->GetOrigin(origin);
  image->GetDimensions(dimensions);

  double focalPoint[3];
  double position[3];

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

  const double distanceToFocalPoint = 1000;
  position[idx] += distanceToFocalPoint;

  m_Camera->SetPosition (   position );
  m_Camera->SetFocalPoint ( focalPoint );

#define myMAX(x,y) (((x)>(y))?(x):(y))  

   int d1 = (idx + 1) % 3;
   int d2 = (idx + 2) % 3;
 
  double max = myMAX( 
    spacing[d1] * dimensions[d1],
    spacing[d2] * dimensions[d2]);

#undef myMAX

  m_Camera->SetParallelScale( max / 2  * m_ZoomFactor );

}




void  
ImageSliceViewer
::SetPoint( double x, double y, double z )
{
  m_InputPoint[0] = x;
  m_InputPoint[1] = y;
  m_InputPoint[2] = z;
}





void  
ImageSliceViewer
::SetPoint()
{
  m_SelectedPoint[0] = m_InputPoint[0];
  m_SelectedPoint[1] = m_InputPoint[1];
  m_SelectedPoint[2] = m_InputPoint[2];
}




void 
ImageSliceViewer
::GetSelectPoint(double data[3])
{
  for(int i=0; i<3; i++)
  {
    data[i] = m_SelectedPoint[i];
  }
}




unsigned long 
ImageSliceViewer
::AddObserver( const itk::EventObject & event, itk::Command * command)
{
  return m_Notifier->AddObserver( event, command );
}


}

