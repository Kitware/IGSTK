/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkView.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// Disabling warning C4355: 'this' : used in base member initializer list
#if defined(_MSC_VER)
#pragma warning ( disable : 4355 )
#endif

// vtk include files
#include "vtkVersion.h"
#include "vtkCommand.h"
#include "vtkWindowToImageFilter.h"
#include "vtkPNGWriter.h"
#include "vtkViewport.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"
#include "vtkInteractorStyle.h"
#include "vtkRenderer.h"
#include "vtkWorldPointPicker.h"

#if defined(__APPLE__) && defined(VTK_USE_CARBON)
#include "vtkCarbonRenderWindow.h"
#endif

//igstk include files
#include "igstkView.h"

#include "itksys/SystemTools.hxx"

namespace igstk
{

/** Constructor */
View::View() : 
m_StateMachine(this)
{ 
  /** Coordinate system interface */
  igstkCoordinateSystemClassInterfaceConstructorMacro();

  igstkLogMacro( DEBUG, "igstkView::Constructor() called ...\n");
  
  this->m_Logger = NULL;
  
  // Create a default render window
  this->m_RenderWindow = vtkRenderWindow::New();
  this->m_Renderer = vtkRenderer::New();
  this->m_RenderWindowInteractor = RenderWindowInteractor::New();

  this->m_RenderWindow->AddRenderer( this->m_Renderer );
  this->m_Camera = this->m_Renderer->GetActiveCamera();
  this->m_RenderWindow->BordersOff();
  this->m_Renderer->SetBackground(0.5,0.5,0.5);

  // Initialize the render window size 
  this->m_RenderWindow->SetSize( 512, 512 );

  int * size = this->m_RenderWindow->GetSize();
    
  this->m_RenderWindowInteractor->SetSize( size );

  this->m_RenderWindowInteractor->SetRenderWindow( this->m_RenderWindow );

  // Set the default view to be axial
  this->m_Camera->SetPosition ( 0, 0, -1 );
  this->m_Camera->SetViewUp( 0, -1, 0 );
  this->m_Renderer->ResetCamera();

  // create a picker
  this->m_PointPicker = vtkWorldPointPicker::New(); 
  this->m_RenderWindowInteractor->SetPicker( this->m_PointPicker );
  
  igstkAddInputMacro( ValidAddObject );
  igstkAddInputMacro( NullAddObject  );
  igstkAddInputMacro( ValidAddAnnotation2D );
  igstkAddInputMacro( NullAddAnnotation2D  );
  igstkAddInputMacro( ExistingAddObject );
  igstkAddInputMacro( ValidRemoveObject );
  igstkAddInputMacro( InexistingRemoveObject );
  igstkAddInputMacro( NullRemoveObject  );
  igstkAddInputMacro( ValidAddActor );
  igstkAddInputMacro( NullAddActor  );
  igstkAddInputMacro( ValidRemoveActor );
  igstkAddInputMacro( NullRemoveActor  );
  igstkAddInputMacro( ResetCamera  );
  igstkAddInputMacro( StartRefreshing  );
  igstkAddInputMacro( StopRefreshing  );
  igstkAddInputMacro( ValidScreenShotFileName  );
  igstkAddInputMacro( InvalidScreenShotFileName  );
  igstkAddInputMacro( ValidRenderWindowSize  );
  igstkAddInputMacro( InValidRenderWindowSize  );
  igstkAddInputMacro( InitializeInteractor  );

  igstkAddStateMacro( Idle       );
  igstkAddStateMacro( InteractorInitialized );
  igstkAddStateMacro( Refreshing );


  igstkAddTransitionMacro( Idle, ValidAddObject, 
                           Idle,  AddObject );
  igstkAddTransitionMacro( Idle, NullAddObject,
                           Idle,  ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ValidAddAnnotation2D,
                           Idle,  AddAnnotation2D );
  igstkAddTransitionMacro( Idle, NullAddAnnotation2D,
                           Idle,  ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ExistingAddObject,
                           Idle,  ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ValidRemoveObject,
                           Idle,  RemoveObject );
  igstkAddTransitionMacro( Idle, NullRemoveObject,
                           Idle,  ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, InexistingRemoveObject,
                           Idle,  ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ValidAddActor,
                           Idle,  AddActor );
  igstkAddTransitionMacro( Idle, NullAddActor,
                           Idle,  ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ValidRemoveActor,
                           Idle,  RemoveActor );
  igstkAddTransitionMacro( Idle, NullRemoveActor,
                           Idle,  ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ResetCamera,
                           Idle,  ResetCamera );
  igstkAddTransitionMacro( Idle, StartRefreshing,
                           Idle,  ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, StopRefreshing,
                           Idle,  ReportInvalidRequest );

  igstkAddTransitionMacro( Idle, InitializeInteractor,
                           InteractorInitialized,  
                           InitializeRenderWindowInteractor);

  igstkAddTransitionMacro( InteractorInitialized, StartRefreshing,
                           Refreshing,  Start );
  igstkAddTransitionMacro( InteractorInitialized, StopRefreshing,
                           InteractorInitialized,  ReportInvalidRequest );

  igstkAddTransitionMacro( InteractorInitialized, ValidAddObject, 
                           InteractorInitialized,  AddObject );
  igstkAddTransitionMacro( InteractorInitialized, NullAddObject,
                           InteractorInitialized,  ReportInvalidRequest );
  igstkAddTransitionMacro( InteractorInitialized, ValidAddAnnotation2D,
                           InteractorInitialized,  AddAnnotation2D );
  igstkAddTransitionMacro( InteractorInitialized, NullAddAnnotation2D,
                           InteractorInitialized,  ReportInvalidRequest );
  igstkAddTransitionMacro( InteractorInitialized, ExistingAddObject,
                           InteractorInitialized,  ReportInvalidRequest );
  igstkAddTransitionMacro( InteractorInitialized, ValidRemoveObject,
                           InteractorInitialized,  RemoveObject );
  igstkAddTransitionMacro( InteractorInitialized, NullRemoveObject,
                           InteractorInitialized,  ReportInvalidRequest );
  igstkAddTransitionMacro( InteractorInitialized, InexistingRemoveObject,
                           InteractorInitialized,  ReportInvalidRequest );
  igstkAddTransitionMacro( InteractorInitialized, ValidAddActor,
                           InteractorInitialized,  AddActor );
  igstkAddTransitionMacro( InteractorInitialized, NullAddActor,
                           InteractorInitialized,  ReportInvalidRequest );
  igstkAddTransitionMacro( InteractorInitialized, ValidRemoveActor,
                           InteractorInitialized,  RemoveActor );
  igstkAddTransitionMacro( InteractorInitialized, NullRemoveActor,
                           InteractorInitialized,  ReportInvalidRequest );
  igstkAddTransitionMacro( InteractorInitialized, ResetCamera,
                           InteractorInitialized,  ResetCamera );
  igstkAddTransitionMacro( InteractorInitialized, ValidScreenShotFileName,
                           InteractorInitialized,  
                            SaveScreenShotWhileInteractorInitialized );
  igstkAddTransitionMacro( InteractorInitialized, InvalidScreenShotFileName,
                           InteractorInitialized,  
                           ReportInvalidScreenShotFileName );

  igstkAddTransitionMacro( Idle, ValidScreenShotFileName,
                           Idle, SaveScreenShotWhileIdle )

  igstkAddTransitionMacro( Idle, InvalidScreenShotFileName,
                           Idle, ReportInvalidScreenShotFileName );

  igstkAddTransitionMacro( Idle, ValidRenderWindowSize,
                           Idle, SetRenderWindowSize );

  igstkAddTransitionMacro( Idle, InValidRenderWindowSize,
                           Idle, ReportInvalidRenderWindowSize);
  
  igstkAddTransitionMacro( Refreshing, ValidAddObject,
                           Refreshing,  AddObject );
  igstkAddTransitionMacro( Refreshing, NullAddObject,
                           Refreshing,  ReportInvalidRequest );
  igstkAddTransitionMacro( Refreshing, ValidAddAnnotation2D,
                           Refreshing,  AddAnnotation2D );
  igstkAddTransitionMacro( Refreshing, NullAddAnnotation2D,
                           Refreshing,  ReportInvalidRequest );
  igstkAddTransitionMacro( Refreshing, ExistingAddObject,
                           Refreshing,  ReportInvalidRequest );
  igstkAddTransitionMacro( Refreshing, ValidRemoveObject,
                           Refreshing,  RemoveObject );
  igstkAddTransitionMacro( Refreshing, NullRemoveObject,
                           Refreshing,  ReportInvalidRequest );
  igstkAddTransitionMacro( Refreshing, InexistingRemoveObject,
                           Refreshing,  ReportInvalidRequest );
  igstkAddTransitionMacro( Refreshing, ValidAddActor,
                           Refreshing,  AddActor );
  igstkAddTransitionMacro( Refreshing, NullAddActor,
                           Refreshing,  ReportInvalidRequest );
  igstkAddTransitionMacro( Refreshing, ValidRemoveActor,
                           Refreshing,  RemoveActor );
  igstkAddTransitionMacro( Refreshing, NullRemoveActor,
                           Refreshing,  ReportInvalidRequest );
  igstkAddTransitionMacro( Refreshing, ResetCamera,
                           Refreshing,  ResetCamera );
  igstkAddTransitionMacro( Refreshing, StartRefreshing,
                           Refreshing,  ReportInvalidRequest );
  igstkAddTransitionMacro( Refreshing, StopRefreshing,
                           InteractorInitialized,  Stop );
  igstkAddTransitionMacro( Refreshing, ValidScreenShotFileName,
                           Refreshing, SaveScreenShotWhileRefreshing )
  igstkAddTransitionMacro( Refreshing, InvalidScreenShotFileName,
                           Refreshing, ReportInvalidScreenShotFileName );
  igstkAddTransitionMacro( Refreshing, ValidRenderWindowSize,
                           Refreshing, SetRenderWindowSize );
  igstkAddTransitionMacro( Refreshing, InValidRenderWindowSize,
                           Refreshing, ReportInvalidRenderWindowSize);
  igstkAddTransitionMacro( Refreshing, InitializeInteractor,
                           Refreshing, ReportInvalidRequest);
 
 

  igstkSetInitialStateMacro( Idle );

  this->m_StateMachine.SetReadyToRun();

  this->m_ActorToBeAdded = 0;
  this->m_ActorToBeRemoved = 0;

  this->m_PulseGenerator = PulseGenerator::New();
  
  this->m_PulseObserver = ObserverType::New();
  this->m_PulseObserver->SetCallbackFunction( this, & View::RefreshRender );

  this->m_PulseGenerator->AddObserver( PulseEvent(), this->m_PulseObserver );

  this->SetRefreshRate( 30 ); // 30 Hz is rather low frequency for video.

  this->m_PickerCoordinateSystem = CoordinateSystem::New();
}

/** Destructor */
View::~View()
{
  igstkLogMacro( DEBUG, "igstkView::Destructor() called ...\n");
  
  m_PulseGenerator->RequestStop();

  this->m_RenderWindowInteractor->SetRenderWindow( NULL ); 
  this->m_RenderWindowInteractor->SetPicker( NULL );

  this->m_RenderWindow->RemoveRenderer( this->m_Renderer );

  if( this->m_RenderWindowInteractor != NULL )
    {
    this->m_RenderWindowInteractor->Delete();
    }

  if( this->m_Renderer != NULL )
    {
    this->m_Renderer->Delete();
    }

  if( this->m_RenderWindow != NULL )
    {
    this->m_RenderWindow->Delete();
    }

  if( this->m_PointPicker != NULL )
    {
    this->m_PointPicker->Delete();
    }
}

/** Get renderer */ 
vtkRenderer *  View::GetRenderer() const
{
  return this->m_Renderer; 
}
 
/** Get render window */
vtkRenderWindow * View::GetRenderWindow() const
{
  return this->m_RenderWindow; 
} 

/** Get render window interactor */
RenderWindowInteractor *  View::GetRenderWindowInteractor() const
{
  return this->m_RenderWindowInteractor; 
}

/** Request initialize render window interactor */
void View::RequestInitializeRenderWindowInteractor()
{
  igstkLogMacro(DEBUG,
                "RequestInitializeRenderWindowInteractor() called.\n");

  igstkPushInputMacro( InitializeInteractor );
  m_StateMachine.ProcessInputs();
} 

/** Initialize render window interactor */
void View::InitializeRenderWindowInteractorProcessing()
{

  igstkLogMacro(DEBUG,
                "InitializeRenderWindowInteractorProcessing() called...\n");

  if( !this->m_RenderWindowInteractor->GetInitialized() )
    {
    this->m_RenderWindowInteractor->Initialize();
    }
}

/** */
void View::RequestAddActor( vtkProp * actor )
{
  igstkLogMacro( DEBUG, "igstkView::RequestAddActor() called ...\n");
  this->m_ActorToBeAdded = actor;
  if( !actor )
    {
    igstkPushInputMacro( NullAddActor );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidAddActor );
    m_StateMachine.ProcessInputs();
    }
}

/** */
void View::AddActorProcessing()
{
  igstkLogMacro( DEBUG, "igstkView::AddActorProcessing() called ...\n");
  this->m_Renderer->AddActor( this->m_ActorToBeAdded );

  //add the actor to the picker's list
  this->m_PointPicker->AddPickList( this->m_ActorToBeAdded );
}


/** */
void View::RequestRemoveActor( vtkProp * actor )
{
  igstkLogMacro( DEBUG, "igstkView::RequestRemoveActor() called ...\n");
  this->m_ActorToBeRemoved = actor;
  if( !actor )
    {
    igstkPushInputMacro( NullRemoveActor );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidRemoveActor );
    m_StateMachine.ProcessInputs();
    }
}


/** */
void View::RemoveActorProcessing()
{
  igstkLogMacro( DEBUG, "igstkView::RemoveActorProcessing() called ...\n");
  this->m_Renderer->RemoveActor( this->m_ActorToBeRemoved );

  //remove the actor from the picker's list
  this->m_PointPicker->DeletePickList( this->m_ActorToBeRemoved );
}

void View::SetPickedPointCoordinates( double x, double y )
{
  igstkLogMacro( DEBUG, "igstkView::SetPickedPointCoordinates() called ...\n");
  this->m_PointPicker->Pick( x, y, 0, this->m_Renderer );

  double data[3];
  this->m_PointPicker->GetPickPosition( data );

  Transform::VectorType pickedPoint;
  pickedPoint[0] = data[0];
  pickedPoint[1] = data[1];
  pickedPoint[2] = data[2];
  
  double validityTime = itk::NumericTraits<double>::max();
  double errorValue = 1.0; // this should be obtained from 
                           // the picked object.

  Transform transform;
  transform.SetTranslation( pickedPoint, errorValue, validityTime );

  CoordinateSystemTransformToResult transformCarrier;

  transformCarrier.Initialize( transform,
    this->m_PickerCoordinateSystem,
    this->GetCoordinateSystem() );

  CoordinateSystemTransformToEvent  transformEvent;
  transformEvent.Set( transformCarrier );

  this->InvokeEvent( transformEvent );
}
 
/** */
void View::RequestResetCamera()
{
  igstkLogMacro( DEBUG, "igstkView::RequestResetCamera() called ...\n");
  igstkPushInputMacro( ResetCamera );
  m_StateMachine.ProcessInputs();
}

/** */
void View::ResetCameraProcessing()
{
  igstkLogMacro( DEBUG, "igstkView::ResetCameraProcessing() called ...\n");

  /** Before resetting the camera, make sure that that object
   *  representations are up to date. Representations need to be
   *  updated to make sure they are in the right position,
   *  and sized correctly so the reset camera parameters make
   *  sense. This will also make sure that the object
   *  representations have a valid transform, which should
   *  make them visible.
   */

  /** First, compute the time at which we
   *  estimate that the scene will be rendered. */
  TimeStamp renderTime;
  double frequency = this->m_PulseGenerator->GetFrequency();

  /** Frequency is in hertz but period is expected to be in milliseconds
   *  Transform is valid for one pulse. */
  renderTime.SetStartTimeNowAndExpireAfter( 1000.0 / frequency );

  /** Second, notify all the representation objects of the time at which this
   *  scene will be rendered. */
  ObjectListType::iterator itr    = this->m_Objects.begin();
  ObjectListType::iterator endItr = this->m_Objects.end();

  typedef igstk::Friends::CoordinateSystemHelper     CoordinateSystemHelperType;
                          
  const CoordinateSystem* thisCS = 
    CoordinateSystemHelperType::GetCoordinateSystem( this );
  while( itr != endItr )
    {
    (*itr)->RequestUpdateRepresentation( renderTime, thisCS );
    ++itr;
    }

  this->m_Renderer->ResetCamera();
  this->m_Camera->SetClippingRange( 0.1, 10000);
}

/** */
void View::StartProcessing()
{
  igstkLogMacro( DEBUG, "igstkView::StartProcessing() called ...\n");
  // the internal pulse generator will control the redraws
  this->m_PulseGenerator->RequestStart();
}

/** */
void View::StopProcessing()
{
  igstkLogMacro( DEBUG, "igstkView::StopProcessing() called ...\n");
  // the internal pulse generator will control the redraws
  this->m_PulseGenerator->RequestStop();
}

/** Set RenderWindow size */
void View::SetRenderWindowSizeProcessing()
{
  igstkLogMacro( DEBUG, 
                "igstkView::SetRenderWindowSizeProcessing(...) called ...\n");

  this->m_RenderWindowInteractor->UpdateSize( this->m_RenderWindowWidthToBeSet,
                                        this->m_RenderWindowHeightToBeSet);
  this->m_RenderWindow->SetSize(this->m_RenderWindowWidthToBeSet,
                          this->m_RenderWindowHeightToBeSet);

 
  // update the viewport size of the annotations if annotations have been
  // added
  if( this->m_Annotation2DToBeAdded ) 
    {
    this->m_Annotation2DToBeAdded->RequestSetAnnotationsViewPort( 
      this->m_RenderWindowWidthToBeSet,
      this->m_RenderWindowHeightToBeSet );
    }

  this->m_RenderWindowInteractor->Modified();
  this->m_RenderWindow->Modified();
}

/** Set camera position */
void View::RequestSetRenderWindowSize( int width , int height )
{
  igstkLogMacro( DEBUG, 
                 "igstkView::RequestSetRenderWindowSize(...) called ...\n");

  const int * size = this->m_RenderWindowInteractor->GetSize();
    
  // If the size is the same then ignore the request
  if ( (width == size[0]) && (height == size[1]) )
    {
    return;
    }

  if ( width > 0 && height > 0 )
    {
    this->m_RenderWindowWidthToBeSet = width;
    this->m_RenderWindowHeightToBeSet = height;
    igstkPushInputMacro( ValidRenderWindowSize );
    }
  else
    {
    igstkPushInputMacro( InValidRenderWindowSize );
    }

  m_StateMachine.ProcessInputs();
}

/** Set camera position */
void View::SetCameraPosition( double x, double y, double z )
{
  igstkLogMacro( DEBUG, "igstkView::SetCameraPosition(...) called ...\n");
  this->m_Camera->SetPosition( x,y,z );
}

/** Set camera focal point */
void View::SetCameraFocalPoint( double x, double y, double z )
{
  igstkLogMacro( DEBUG, "igstkView::SetCameraFocalPoint(...) called ...\n");
  this->m_Camera->SetFocalPoint( x,y,z );
}

/** Set camera view up vector */
void View::SetCameraViewUp( double vx, double vy, double vz )
{
  igstkLogMacro( DEBUG, "igstkView::RequestSetViewUp(...) called ...\n");
  this->m_Camera->SetViewUp( vx,vy,vz );
}

/** Set camera clipping range */
void View::SetCameraClippingRange( double dNear, double dFar)
{
  igstkLogMacro( DEBUG, "igstkView::SetCameraClippingRange(...) called ...\n");
  this->m_Camera->SetClippingRange( dNear, dFar );
}

/** Turn on/off parallel projection */
void View::SetCameraParallelProjection( bool flag )
{
  igstkLogMacro( DEBUG,
                 "igstkView::SetCameraParallelProjection(...) called ...\n");
  this->m_Camera->SetParallelProjection( flag );
}

/** Set background color */
void View::
SetRendererBackgroundColor( double red, double green, double blue)
{
  igstkLogMacro( DEBUG,
                 "igstkView::SetRendererBackgroundColor(...) called ...\n");
  this->m_Renderer->SetBackground( red, green, blue );
}

/** Set camera zoom factor */
void View::
SetCameraZoomFactor( double factor )
{
  igstkLogMacro( DEBUG, "igstkView::SetCameraZoomFactor(...) called ...\n");
  this->m_Camera->Zoom( factor );
}

/** Define the refresh rate by programming the internal pulse generator */
void View::SetRefreshRate( double frequencyHz )
{
  igstkLogMacro( DEBUG, "igstkView::SetRefreshRate() called ...\n");
  // Let the state machine of the pulse generator manage this request
  this->m_PulseGenerator->RequestSetFrequency( frequencyHz );
}

/** Refresh the rendering. This function is called in response to pulses from
 * the pulse generator. */
void View::RefreshRender()
{
  igstkLogMacro( DEBUG, "igstkView::RefreshRender() called ...\n");

  // First, compute the time at which we
  // estimate that the scene will be rendered
  TimeStamp renderTime;
  double frequency = this->m_PulseGenerator->GetFrequency();
  // Frequency is in hertz but period is expected to be in milliseconds
  renderTime.SetStartTimeNowAndExpireAfter( 1000.0 / frequency );

  // Second, notify all the representation object of the time at which this
  // scene will be rendered.
  ObjectListType::iterator itr    = this->m_Objects.begin();
  ObjectListType::iterator endItr = this->m_Objects.end();

  typedef igstk::Friends::CoordinateSystemHelper     CoordinateSystemHelperType;
  
  const CoordinateSystem* thisCS = 
     CoordinateSystemHelperType::GetCoordinateSystem( this );  

  while( itr != endItr )
    {
    (*itr)->RequestUpdateRepresentation( renderTime, thisCS );
    ++itr;
    }

  //Third, trigger VTK rendering
  this->m_RenderWindowInteractor->Render();

  // Last, report to observers that a refresh event took place.
  this->InvokeEvent( RefreshEvent() );
}

/** Request for Adding an object to the View */
void View::RequestAddObject( ObjectRepresentation* pointer )
{
  igstkLogMacro( DEBUG, "igstkView::RequestAddObject() called ...\n");

  this->m_ObjectToBeAdded = pointer;

  if( !pointer )
    {
    igstkPushInputMacro( NullAddObject );
    m_StateMachine.ProcessInputs();
    return;
    }

  ObjectListType::iterator it =
    std::find(this->m_Objects.begin(),this->m_Objects.end(),pointer);
  if( it != this->m_Objects.end() )
    {
    igstkPushInputMacro( ExistingAddObject );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidAddObject );
    m_StateMachine.ProcessInputs();
    }
}

/** Request for adding annotation */
void View::RequestAddAnnotation2D ( Annotation2D::Pointer annotation )
{
  igstkLogMacro( DEBUG, "igstkView::RequestAddAnnotation2D() called ...\n");

  this->m_Annotation2DToBeAdded = annotation;

  if( !annotation )
    {
    igstkPushInputMacro( NullAddAnnotation2D );
    this->m_StateMachine.ProcessInputs();
    return;
    }

  igstkPushInputMacro( ValidAddAnnotation2D );
  m_StateMachine.ProcessInputs();
}


/** Add an object to the View. This method should only be called by the 
 * state machine. The state machine makes sure that this method is called 
 * with a valid value in the ObjectToBeAdded. */
void View::AddObjectProcessing()
{
  igstkLogMacro( DEBUG, "igstkView::AddObjectProcessing() called ...\n");
  
  this->m_Objects.push_back( this->m_ObjectToBeAdded );
  this->m_RenderWindowInteractor->Modified();
  
  this->m_ObjectToBeAdded->CreateActors();

  ObjectRepresentation::ActorsListType actors = 
    this->m_ObjectToBeAdded->GetActors();

  ObjectRepresentation::ActorsListType::iterator actorIt = actors.begin();

  while(actorIt != actors.end())
    {
    this->RequestAddActor(*actorIt);
    actorIt++;
    } 

  //after the object is added, set the temp smart point to NULL, so that
  //the reference count of the object decrements
  this->m_ObjectToBeAdded = NULL;
}

void View::AddAnnotation2DProcessing( )
{
  igstkLogMacro( DEBUG, "igstkView::AddAnnotation2DProcessing called ...\n");
  
  const int * size = this->m_RenderWindowInteractor->GetSize();
  igstkLogMacro( DEBUG, "RenderWindow size: " << size[0] << "," << size[1]);

  this->m_Annotation2DToBeAdded->RequestSetAnnotationsViewPort( 
    size[0], size[1] );

  this->m_Annotation2DToBeAdded->RequestAddAnnotations( );

  Annotation2D::ActorsListType actors = 
    this->m_Annotation2DToBeAdded->GetActors();

  Annotation2D::ActorsListType::iterator actorIt = actors.begin();

  while(actorIt != actors.end())
    {
    this->RequestAddActor(*actorIt);
    actorIt++;
    } 
}


/** Request for removing a spatial object from the View */
void View::RequestRemoveObject( ObjectRepresentation* pointer )
{
  igstkLogMacro( DEBUG, "igstkView::RequestRemoveObject() called ...\n");

  this->m_ObjectToBeRemoved = pointer;
  
  this->m_IteratorToObjectToBeRemoved = this->m_Objects.end(); 
  
  if( !pointer )
    {
    igstkPushInputMacro( NullRemoveObject );
    m_StateMachine.ProcessInputs();
    return;
    }
  
  this->m_IteratorToObjectToBeRemoved =
    std::find(this->m_Objects.begin(),this->m_Objects.end(),pointer);
  if( this->m_IteratorToObjectToBeRemoved == this->m_Objects.end() )
    {
    igstkPushInputMacro( InexistingRemoveObject );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidRemoveObject );
    m_StateMachine.ProcessInputs();
    }
}


/** Remove a spatial object from the View. This method can only be invoked
 * by the State Machine who will make sure that the content of
 * m_IteratorToObjectToBeRemoved is valid. */
void View::RemoveObjectProcessing()
{
  igstkLogMacro( DEBUG, "igstkView::RemoveObjectProcessing() called ...\n");

  this->m_Objects.erase( this->m_IteratorToObjectToBeRemoved );
  this->m_RenderWindowInteractor->Modified();
  
  ObjectRepresentation::ActorsListType actors =
                                         this->m_ObjectToBeRemoved->GetActors();
  ObjectRepresentation::ActorsListType::iterator actorIt = actors.begin();

  while(actorIt != actors.end())
    {
    this->RequestRemoveActor(*actorIt);
    actorIt++;
    } 

  //after the object is removed, set the temp smart point to NULL, so that
  //the reference count of the object decrements
  this->m_ObjectToBeRemoved = NULL;
}


/** Request to Start the Pulse Generator for periodically refreshing the 
 *  View
 * */
void View::RequestStart()
{
  igstkLogMacro( DEBUG, "igstkView::RequestStart() called ...\n");

  igstkPushInputMacro( StartRefreshing );
  m_StateMachine.ProcessInputs();
}


/** Request to Stop the Pulse Generator for periodically refreshing the 
 *  View
 * */
void View::RequestStop()
{
  igstkLogMacro( DEBUG, "igstkView::RequestStop() called ...\n");

  igstkPushInputMacro( StopRefreshing );
  m_StateMachine.ProcessInputs();
}


/** Request to save the screenshot of the current scene into a file.
 *  The fileformat must be PNG. */
void View::RequestSaveScreenShot( const std::string & filename )
{
  igstkLogMacro( DEBUG, "igstkView::RequestSaveScreenShot() called ...\n");

  std::string fileNameExtension =
        ::itksys::SystemTools::GetFilenameLastExtension( filename );

  if( fileNameExtension == ".png" )
    {
    this->m_ScreenShotFileName = filename;
    igstkPushInputMacro( ValidScreenShotFileName );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( InvalidScreenShotFileName );
    m_StateMachine.ProcessInputs();
    }
}


/** Report that an invalid or suspicious operation has been requested. This 
 * may mean that an error condition has arised in one of the components that
 * interact with this class. */
void View::ReportInvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "ReportInvalidRequestProcessing() called ...\n");
}

/** Report a request to set an invalid render window size set */ 
void View::ReportInvalidRenderWindowSizeProcessing()
{
  igstkLogMacro( WARNING, 
                  "ReportInvalidRenderWindowSizeProcessing() called ...\n");
}

/** Report that an invalid filename for saving the screen shot */
void View::ReportInvalidScreenShotFileNameProcessing()
{
  igstkLogMacro( WARNING,
                "ReportInvalidScreenShotFileNameProcessing() called ...\n");
}

/** Save current screenshot while in refreshing state */
void View::SaveScreenShotWhileRefreshingProcessing()
{
  igstkLogMacro( DEBUG, 
                 "SaveScreenShotWhileRefreshingProcessing() called ...\n");

  this->m_PulseGenerator->RequestStop();
  
  this->SaveScreenShot();

  this->m_PulseGenerator->RequestStart();
}

/** Save current screenshot in idle state */
void View::SaveScreenShotWhileIdleProcessing()
{
  igstkLogMacro( DEBUG,
                 "igstkView::SaveScreenShotWhileIdleProcessing() called ...\n");
  
  this->SaveScreenShot();
}

/** Save current screenshot in InteractorInitialized state */
void View::SaveScreenShotWhileInteractorInitializedProcessing()
{
  igstkLogMacro( DEBUG,
         "igstkView::SaveScreenShotWhileInteractorInitializedProcessing()" 
         << " called" );
  
  this->SaveScreenShot();
}


/** Save current screenshot */
void View::SaveScreenShot()
{
  igstkLogMacro( DEBUG, "igstkView::SaveScreenShot() called ...\n");

  vtkWindowToImageFilter * windowToImageFilter = 
                                              vtkWindowToImageFilter::New();

  vtkPNGWriter * writer = vtkPNGWriter::New();

  windowToImageFilter->SetInput( this->m_RenderWindow );

  windowToImageFilter->Update();

  writer->SetInput( windowToImageFilter->GetOutput() );
  
  writer->SetFileName( this->m_ScreenShotFileName.c_str() );
  
  this->m_RenderWindow->Render();
  
  writer->Write();

  writer->SetInput( NULL );
  windowToImageFilter->SetInput( NULL );

  windowToImageFilter->Delete();
  writer->Delete();
}

void
View
::SetInteractorStyle( vtkInteractorStyle * interactorStyle )
{
  this->m_RenderWindowInteractor->SetInteractorStyle( interactorStyle );
}


void 
View
::Print( std::ostream& os, ::itk::Indent indent ) const
{
  this->PrintSelf(os, indent);
}


/**
 * This operator allows all subclasses of LightObject to be printed via <<.
 * It in turn invokes the Print method, which in turn will invoke the
 * PrintSelf method that all objects should define, if they have anything
 * interesting to print out.
 */
std::ostream& operator<<(std::ostream& os, const View& o)
{
  o.Print(os);
  return os;
}


/** Print object information */
void View::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "RTTI typeinfo:   " << typeid( *this ).name() << std::endl;
  os << indent << "RenderWindow Pointer: " << this->m_RenderWindow;
  os << std::endl;
  os << indent << "RenderWindowInteractor Pointer: " 
               << *(this->m_RenderWindowInteractor) << std::endl;
  os << indent << "Renderer Pointer: " << this->m_Renderer << std::endl;
  os << indent << "Camera Pointer: " << this->m_Camera << std::endl;

  if( this->m_PulseGenerator )
    {
    os << indent;
    this->m_PulseGenerator->Print(os);
    }

  if( this->m_PulseObserver )
    {
    os << indent;
    this->m_PulseObserver->Print(os);
    }

  ObjectListConstIterator itr;

  for( itr = this->m_Objects.begin(); itr != this->m_Objects.end(); ++itr )
    {
    os << indent << *itr << std::endl;
    }

  os << indent << "CoordinateSystemDelegator: ";
  this->m_CoordinateSystemDelegator->PrintSelf( os, indent );
}

void View::RequestSetActiveCamera(vtkCamera * camera )
{
  if ( camera )
  {
    m_Camera = camera;
    this->m_Renderer->SetActiveCamera( m_Camera );
  }
}

} // end namespace igstk
