/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkQView.cxx
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

#include "vtkVersion.h"
#include "vtkCommand.h"
#include "vtkWindowToImageFilter.h"
#include "vtkPNGWriter.h"
#include "vtkViewport.h"
#include "vtkRenderWindow.h"

#if defined(__APPLE__) && defined(VTK_USE_CARBON)
#include "vtkCarbonRenderWindow.h"
#endif

#include "qevent.h"

#include "itksys/SystemTools.hxx"

#include "igstkRenderWindowInteractor.h"   
#include "igstkEvents.h"

#include "igstkQView.h"


namespace igstk
{

/** Constructor */
QView::
#if QT_VERSION < 0x040000
//! constructor for Qt 3
QView(QWidget* parent, const char* name, Qt::WFlags f):
QVTKWidget( parent, name, f ), m_StateMachine(this)
#else
//! constructor for Qt 4
QView(QWidget* parent, Qt::WFlags f):
QVTKWidget( parent, f ), m_StateMachine(this)
#endif

{ 
  m_Logger = NULL;
  
  m_Renderer = vtkRenderer::New();
  m_PointPicker = PickerType::New();

  this->GetRenderWindow()->AddRenderer( m_Renderer );
  m_Camera = m_Renderer->GetActiveCamera();
  this->GetRenderWindow()->BordersOff();
  m_Renderer->SetBackground(0.5,0.5,0.5);

  int * size = this->GetRenderWindow()->GetSize();
    
  this->GetInteractor()->SetSize( size );

  this->GetInteractor()->SetPicker( m_PointPicker );

  this->GetInteractor()->SetRenderWindow( this->GetRenderWindow() );

  // Set the default view to be axial
  m_Camera->SetPosition ( 0, 0, -1 );
  m_Camera->SetViewUp( 0, -1, 0 );
  m_Renderer->ResetCamera();

  m_InteractionHandling = true;

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
  igstkAddInputMacro( EnableInteractions  );
  igstkAddInputMacro( DisableInteractions  );
  igstkAddInputMacro( StartRefreshing  );
  igstkAddInputMacro( StopRefreshing  );
  igstkAddInputMacro( ValidScreenShotFileName  );
  igstkAddInputMacro( InvalidScreenShotFileName  );


  igstkAddStateMacro( Idle       );
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
  igstkAddTransitionMacro( Idle, EnableInteractions,
                           Idle,  EnableInteractions );
  igstkAddTransitionMacro( Idle, DisableInteractions,
                           Idle,  DisableInteractions );
  igstkAddTransitionMacro( Idle, StartRefreshing,
                           Refreshing,  Start );
  igstkAddTransitionMacro( Idle, StopRefreshing,
                           Idle,  ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, ValidScreenShotFileName,
                           Idle, SaveScreenShotWhileIdle )
  igstkAddTransitionMacro( Idle, InvalidScreenShotFileName,
                           Idle, ReportInvalidScreenShotFileName );
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
  igstkAddTransitionMacro( Refreshing, EnableInteractions,
                           Refreshing,  EnableInteractions );
  igstkAddTransitionMacro( Refreshing, DisableInteractions,
                           Refreshing,  DisableInteractions );
  igstkAddTransitionMacro( Refreshing, StartRefreshing,
                           Refreshing,  ReportInvalidRequest );
  igstkAddTransitionMacro( Refreshing, StopRefreshing,
                           Idle,  Stop );
  igstkAddTransitionMacro( Refreshing, ValidScreenShotFileName,
                           Refreshing, SaveScreenShotWhileRefreshing )
  igstkAddTransitionMacro( Refreshing, InvalidScreenShotFileName,
                           Refreshing, ReportInvalidScreenShotFileName );

  igstkSetInitialStateMacro( Idle );

  m_StateMachine.SetReadyToRun();

  m_ActorToBeAdded = 0;
  m_ActorToBeRemoved = 0;

  m_PulseGenerator = PulseGenerator::New();
  m_Reporter = ::itk::Object::New();

  m_PulseObserver = ObserverType::New();
  m_PulseObserver->SetCallbackFunction( this, & QView::RefreshRender );

  m_PulseGenerator->AddObserver( PulseEvent(), m_PulseObserver );

  this->RequestSetRefreshRate( 30 ); // 30 Hz is rather low frequency for video.

}

/** Destructor */
QView::~QView()
{
  igstkLogMacro( DEBUG, "Destructor called ...\n");

  m_PointPicker->Delete();

  m_Renderer->Delete();

}

/** Update the display */
void QView::Initialize()
{
  igstkLogMacro( DEBUG, "Initialize() called ...\n");
  this->GetInteractor()->Initialize();
}

/** Update the display */
void QView::Update()
{
  igstkLogMacro( DEBUG, "Update() called ...\n");
  if( !this->GetInteractor()->GetInitialized() )
    {
    this->GetInteractor()->Initialize();
    }
  this->markCachedImageAsDirty();
  this->Render();
}


void QView::AddObserver( const ::itk::EventObject & event, 
                              ::itk::Command * observer )
{
  m_Reporter->AddObserver( event, observer );
}


/** */
void QView::RequestAddActor( vtkProp * actor )
{
  igstkLogMacro( DEBUG, "RequestAddActor() called ...\n");
  m_ActorToBeAdded = actor;
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
void QView::AddActorProcessing()
{
  igstkLogMacro( DEBUG, "AddActorProcessing() called ...\n");
  m_Renderer->AddActor( m_ActorToBeAdded );
  m_PointPicker->AddPickList( m_ActorToBeAdded );
}


/** */
void QView::RequestRemoveActor( vtkProp * actor )
{
  igstkLogMacro( DEBUG, "RequestRemoveActor() called ...\n");
  m_ActorToBeRemoved = actor;
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
void QView::RemoveActorProcessing()
{
  igstkLogMacro( DEBUG, "RemoveActorProcessing() called ...\n");
  m_Renderer->RemoveActor( m_ActorToBeRemoved );
}


/** */
void QView::RequestEnableInteractions()
{
  igstkPushInputMacro( EnableInteractions );
  m_StateMachine.ProcessInputs();
}


/** */
void QView::RequestDisableInteractions()
{
  igstkPushInputMacro( DisableInteractions );
  m_StateMachine.ProcessInputs();
}


/** */
void QView::Enable()
{
  igstkLogMacro( DEBUG, "Enable() called ...\n");
  this->GetInteractor()->Enable();
}

/** */
void QView::Render()
{
  igstkLogMacro( DEBUG, "Render() called ...\n");
  this->GetInteractor()->Render();
}

/** */
void QView::EnableInteractionsProcessing()
{
  m_InteractionHandling = true;
}

/** */
void QView::DisableInteractionsProcessing()
{
  m_InteractionHandling = false;
}

/** */
void QView::RequestResetCamera()
{
  igstkLogMacro( DEBUG, "RequestResetCamera() called ...\n");
  igstkPushInputMacro( ResetCamera );
  m_StateMachine.ProcessInputs();
}

/** */
void QView::ResetCameraProcessing()
{
  igstkLogMacro( DEBUG, "ResetCameraProcessing() called ...\n");
  m_Renderer->ResetCamera();
}

/** */
void QView::StartProcessing()
{
  igstkLogMacro( DEBUG, "StartProcessing() called ...\n");
  // the internal pulse generator will control the redraws
  m_PulseGenerator->RequestStart();
}

/** */
void QView::StopProcessing()
{
  igstkLogMacro( DEBUG, "StopProcessing() called ...\n");
  // the internal pulse generator will control the redraws
  m_PulseGenerator->RequestStop();
}

/** Define the refresh rate by programming the internal pulse generator */
void QView::RequestSetRefreshRate( double frequencyHz )
{
  igstkLogMacro( DEBUG, "RequestSetRefreshRate() called ...\n");
  // Let the state machine of the pulse generator manage this request
  m_PulseGenerator->RequestSetFrequency( frequencyHz );
}

/** Refresh the rendering. This function is called in response to pulses from
 * the pulse generator. */
void QView::RefreshRender()
{
  igstkLogMacro( DEBUG, "RefreshRender() called ...\n");

  // First, compute the time at which we
  // estimate that the scene will be rendered
  TimeStamp renderTime;
  double frequency = m_PulseGenerator->GetFrequency();
  // Frequency is in hertz but period is expected to be in milliseconds
  renderTime.SetStartTimeNowAndExpireAfter( 1000.0 / frequency );

  // Second, notify all the representation object of the time at which this
  // scene will be rendered.
  ObjectListType::iterator itr    = m_Objects.begin();
  ObjectListType::iterator endItr = m_Objects.end();
  while( itr != endItr )
    {
    (*itr)->RequestUpdateRepresentation( renderTime );
    (*itr)->RequestUpdatePosition( renderTime );
    ++itr;
    }

  // Third, trigger VTK rendering by invoking a refresh of the GUI.
  this->markCachedImageAsDirty();
  this->Render();

  // Last, report to observers that a refresh event took place.
  m_Reporter->InvokeEvent( RefreshEvent() );
}

/** Request for Adding an object to the View */
void QView::RequestAddObject( ObjectRepresentation* pointer )
{
  igstkLogMacro( DEBUG, "RequestAddObject() called ...\n");
  m_ObjectToBeAdded = pointer;

  if( !pointer )
    {
    igstkPushInputMacro( NullAddObject );
    m_StateMachine.ProcessInputs();
    return;
    }

  ObjectListType::iterator it =
    std::find(m_Objects.begin(),m_Objects.end(),pointer);
  if( it != m_Objects.end() )
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
void QView::RequestAddAnnotation2D ( Annotation2D * annotation )
{
  m_Annotation2DToBeAdded = annotation;

  if( !annotation )
    {
    igstkPushInputMacro( NullAddAnnotation2D );
    m_StateMachine.ProcessInputs();
    return;
    }

  igstkPushInputMacro( ValidAddAnnotation2D );
  m_StateMachine.ProcessInputs();
}


/** Add an object to the View. This method should only be called by the state
 * machine. The state machine makes sure that this method is called with a valid
 * value in the ObjectToBeAdded. */
void QView::AddObjectProcessing()
{
  igstkLogMacro( DEBUG, "AddObjectProcessing() called ...\n");
  m_Objects.push_back( m_ObjectToBeAdded );
  this->GetInteractor()->Modified();
  
  m_ObjectToBeAdded->CreateActors();

  ObjectRepresentation::ActorsListType actors = m_ObjectToBeAdded->GetActors();
  ObjectRepresentation::ActorsListType::iterator actorIt = actors.begin();
  while(actorIt != actors.end())
    {
    this->RequestAddActor(*actorIt);
    actorIt++;
    } 
}

void QView::AddAnnotation2DProcessing( )
{
  const int * size = this->GetInteractor()->GetSize();
  m_Annotation2DToBeAdded->RequestSetAnnotationsViewPort( size[0], size[1] );
  m_Annotation2DToBeAdded->RequestAddAnnotations( );
  Annotation2D::ActorsListType actors = m_Annotation2DToBeAdded->GetActors();
  Annotation2D::ActorsListType::iterator actorIt = actors.begin();

  while(actorIt != actors.end())
    {
    this->RequestAddActor(*actorIt);
    actorIt++;
    } 
}


/** Request for removing a spatial object from the View */
void QView::RequestRemoveObject( ObjectRepresentation* pointer )
{
  igstkLogMacro( DEBUG, "RequestRemoveObject() called ...\n");
  m_ObjectToBeRemoved = pointer;
  
  m_IteratorToObjectToBeRemoved = m_Objects.end(); 
  
  if( !pointer )
    {
    igstkPushInputMacro( NullRemoveObject );
    m_StateMachine.ProcessInputs();
    return;
    }
  
  m_IteratorToObjectToBeRemoved =
    std::find(m_Objects.begin(),m_Objects.end(),pointer);
  if( m_IteratorToObjectToBeRemoved == m_Objects.end() )
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


/** Remove a spatial object from the View. This method can only be invoked by
 * the State Machine who will make sure that the content of
 * m_IteratorToObjectToBeRemoved is valid. */
void QView::RemoveObjectProcessing()
{
  igstkLogMacro( DEBUG, "RemoveObjectProcessing() called ...\n");
  m_Objects.erase( m_IteratorToObjectToBeRemoved );
  this->GetInteractor()->Modified();
  
  ObjectRepresentation::ActorsListType actors =
                                         m_ObjectToBeRemoved->GetActors();
  ObjectRepresentation::ActorsListType::iterator actorIt = actors.begin();

  while(actorIt != actors.end())
    {
    this->RequestRemoveActor(*actorIt);
    actorIt++;
    } 
}


/** Request to Start the Pulse Generator for periodically refreshing the View
 * */
void QView::RequestStart()
{
  igstkLogMacro( DEBUG, "RequestStart() called ...\n");
  igstkPushInputMacro( StartRefreshing );
  m_StateMachine.ProcessInputs();
}


/** Request to Stop the Pulse Generator for periodically refreshing the View
 * */
void QView::RequestStop()
{
  igstkLogMacro( DEBUG, "RequestStop() called ...\n");
  igstkPushInputMacro( StopRefreshing );
  m_StateMachine.ProcessInputs();
}


/** Request to save the screenshot of the current scene into a file.
 *  The fileformat must be PNG. */
void QView::RequestSaveScreenShot( const std::string & filename )
{
  std::string fileNameExtension =
        ::itksys::SystemTools::GetFilenameLastExtension( filename );

  if( fileNameExtension == ".png" )
    {
    m_ScreenShotFileName = filename;
    igstkPushInputMacro( ValidScreenShotFileName );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( InvalidScreenShotFileName );
    m_StateMachine.ProcessInputs();
    }
}


/** Report that an invalid or suspicious operation has been requested. This may
 * mean that an error condition has arised in one of the componenta that
 * interact with this class. */
void QView::ReportInvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "ReportInvalidRequestProcessing() called ...\n");
}


/** Report that an invalid filename for saving the screen shot */
void QView::ReportInvalidScreenShotFileNameProcessing()
{
  igstkLogMacro( WARNING,
                "ReportInvalidScreenShotFileNameProcessing() called ...\n");
}

/** Save current screenshot while in refreshing state */
void QView::SaveScreenShotWhileRefreshingProcessing()
{
  m_PulseGenerator->RequestStop();
  
  this->SaveScreenShot();

  m_PulseGenerator->RequestStart();
}

/** Save current screenshot in idle state */
void QView::SaveScreenShotWhileIdleProcessing()
{
  this->SaveScreenShot();
}

/** Save current screenshot */
void QView::SaveScreenShot()
{
  vtkWindowToImageFilter * windowToImageFilter = vtkWindowToImageFilter::New();

  vtkPNGWriter * writer = vtkPNGWriter::New();

  windowToImageFilter->SetInput( this->GetRenderWindow() );

  windowToImageFilter->Update();

  writer->SetInput( windowToImageFilter->GetOutput() );
  
  writer->SetFileName( m_ScreenShotFileName.c_str() );
  
  this->GetRenderWindow()->Render();
  
  writer->Write();

  writer->SetInput( NULL );
  windowToImageFilter->SetInput( NULL );

  windowToImageFilter->Delete();
  writer->Delete();
}


void
QView
::SetInteractorStyle( vtkInteractorStyle * interactorStyle )
{
  this->GetInteractor()->SetInteractorStyle( interactorStyle );
}

void 
QView
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
std::ostream& operator<<(std::ostream& os, const QView& o)
{
  o.Print(os);
  return os;
}

/** This method is overloaded from the QWidget class in 
 *  order to insert the invocation of the TransformModifiedEvent.
 */
void 
QView
::mouseReleaseEvent(QMouseEvent* e)
{
  vtkRenderWindowInteractor* iren = NULL;
  if(this->mRenWin)
    {
    iren = this->mRenWin->GetInteractor();
    }
  
  if(!iren || !iren->GetEnabled())
    {
    return;
    }
  
  // give vtk event information
#if QT_VERSION < 0x040000
  iren->SetEventInformationFlipY(e->x(), e->y(), 
                                 (e->state() & Qt::ControlButton), 
                                 (e->state() & Qt::ShiftButton ));
#else
  iren->SetEventInformationFlipY(e->x(), e->y(), 
                                 (e->modifiers() & Qt::ControlModifier), 
                                 (e->modifiers() & Qt::ShiftModifier ));
#endif
  
  // invoke appropriate vtk event
  switch(e->button())
    {
    case Qt::LeftButton:
      {
      iren->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, e);

      m_PointPicker->Pick( e->x(), 
                           this->height() - e->y() - 1, 
                           0, m_Renderer );
      double data[3];
      m_PointPicker->GetPickPosition( data );
      Transform::VectorType pickedPoint;
      pickedPoint[0] = data[0];
      pickedPoint[1] = data[1];
      pickedPoint[2] = data[2];
      
      double validityTime = itk::NumericTraits<double>::max();
      double errorValue = 1.0; // this should be obtained from 
                               // the picked object.

      igstk::Transform transform;
      transform.SetTranslation( pickedPoint, errorValue, validityTime );

      igstk::TransformModifiedEvent transformEvent;
      transformEvent.Set( transform );

      m_Reporter->InvokeEvent( transformEvent );

      break;
      }
    case Qt::MidButton:
      iren->InvokeEvent(vtkCommand::MiddleButtonReleaseEvent, e);
      break;

    case Qt::RightButton:
      iren->InvokeEvent(vtkCommand::RightButtonReleaseEvent, e);
      break;

    default:
      break;
    }
}

/**
 * This method is overridden from the QWidget class in order to provide
 * transform events when the mouse is moved and the left button is pressed.
 */
void QView::mouseMoveEvent(QMouseEvent *e)
{
  vtkRenderWindowInteractor* iren = NULL;
  if(this->mRenWin)
    {
    iren = this->mRenWin->GetInteractor();
    }
    
  if(!iren || !iren->GetEnabled())
    {
    return;
    }
  
  // give VTK event information
#if QT_VERSION < 0x040000
  iren->SetEventInformationFlipY(e->x(), e->y(), 
    (e->state() & Qt::ControlButton) > 0 ? 1 : 0, 
    (e->state() & Qt::ShiftButton  ) > 0 ? 1 : 0);
#else
  iren->SetEventInformationFlipY(e->x(), e->y(), 
    (e->modifiers() & Qt::ControlModifier) > 0 ? 1 : 0, 
    (e->modifiers() & Qt::ShiftModifier  ) > 0 ? 1 : 0);
#endif
  
  if(e->buttons() == Qt::LeftButton)
    {
    iren->InvokeEvent(vtkCommand::MouseMoveEvent, e);

    // Get x,y,z in world coordinates from the clicked point
    m_PointPicker->Pick(e->x(), this->height() - e->y() - 1, 0, m_Renderer);

    double data[3];
    m_PointPicker->GetPickPosition(data);
    igstk::Transform::VectorType pickedPoint;
    pickedPoint[0] = data[0];
    pickedPoint[1] = data[1];
    pickedPoint[2] = data[2];

    // Valid unitl next click
    double validityTime = itk::NumericTraits<double>::max(); 

    double errorValue = 1.0; // @TODO: Should be obtained from picked object.

    igstk::Transform transform;
    transform.SetTranslation(pickedPoint, errorValue, validityTime);
  
    igstk::TransformModifiedEvent transformEvent;
    transformEvent.Set(transform);
    
    m_Reporter->InvokeEvent(transformEvent);
    }
}

/** Print object information */
void 
QView
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "RTTI typeinfo:   " << typeid( *this ).name() << std::endl;
  os << indent << "Renderer Pointer: " << this->m_Renderer << std::endl;
  os << indent << "Camera Pointer: " << this->m_Camera << std::endl;
  os << indent << "InteractionHandling: ";
  os << this->m_InteractionHandling << std::endl;

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
}

} // end namespace igstk
