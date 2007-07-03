/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkViewNew.cxx
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

#include "igstkViewNew.h"
#include <vtkVersion.h>
#include <vtkCommand.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <igstkEvents.h>
#include "itksys/SystemTools.hxx"
#include "vtkViewport.h"

#if defined(__APPLE__) && defined(VTK_USE_CARBON)
#include "vtkCarbonRenderWindow.h"
#endif

namespace igstk
{

/** Constructor */
ViewNew::ViewNew() : 
m_StateMachine(this)
{ 
  igstkLogMacro( DEBUG, "Constructor() called ...\n");
  
  m_Logger = NULL;
  
  // Create a default render window
  m_RenderWindow = vtkRenderWindow::New();
  m_Renderer = vtkRenderer::New();
  m_PointPicker = PickerType::New();
  m_RenderWindowInteractor = RenderWindowInteractor::New();

  m_RenderWindow->AddRenderer( m_Renderer );
  m_Camera = m_Renderer->GetActiveCamera();
  m_RenderWindow->BordersOff();
  m_Renderer->SetBackground(0.5,0.5,0.5);

  //SET THE SIZE PROPERLY
  m_RenderWindow->SetSize( 256, 256);

  int * size = m_RenderWindow->GetSize();
    
  m_RenderWindowInteractor->SetSize( size );

  m_RenderWindowInteractor->SetPicker( m_PointPicker );

  m_RenderWindowInteractor->SetRenderWindow( m_RenderWindow );

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
  m_PulseObserver->SetCallbackFunction( this, & ViewNew::RefreshRender );

  m_PulseGenerator->AddObserver( PulseEvent(), m_PulseObserver );

  this->RequestSetRefreshRate( 30 ); // 30 Hz is rather low frequency for video.
  
}

/** Destructor */
ViewNew::~ViewNew()
{
  igstkLogMacro( DEBUG, "Destructor() called ...\n");
  
  m_RenderWindowInteractor->SetRenderWindow( NULL ); 

  m_RenderWindowInteractor->SetPicker( NULL );

  m_RenderWindowInteractor->Delete();
  
  m_PointPicker->Delete();

  m_Renderer->Delete();

  m_RenderWindow->Delete();

}

/** Update the display */
void ViewNew::Update()
{
  igstkLogMacro( DEBUG, "Update() called ...\n");
  if( !m_RenderWindowInteractor->GetInitialized() )
    {
    m_RenderWindowInteractor->Initialize();
    }
}


unsigned long ViewNew::AddObserver( const ::itk::EventObject & event, 
                              ::itk::Command * observer )
{
  igstkLogMacro( DEBUG, "AddObserver() called ...\n");
  return m_Reporter->AddObserver( event, observer );
}

void ViewNew::RemoveObserver( unsigned long tag )
{
  igstkLogMacro( DEBUG, "RemoveObserver() called ...\n");
  m_Reporter->RemoveObserver( tag );
}

/** */
void ViewNew::RequestAddActor( vtkProp * actor )
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

/** this gets called during FLTK window draw()s and resize()s */
void ViewNew::UpdateSize(int W, int H)
{
  igstkLogMacro( DEBUG, "UpdateSize() called ...\n");
  if ( m_RenderWindow != NULL)
    {
    const int * size = m_RenderWindowInteractor->GetSize();
    
    // if the size changed tell render window
    if ( (W != size[0]) || (H != size[1]) )
      {
      // adjust our (m_RenderWindowInteractor size)
      m_RenderWindowInteractor->UpdateSize( W, H );
      // and our RenderWindow's size
      m_RenderWindow->SetSize(W, H);
      }
    }
}

/** */
void ViewNew::AddActorProcessing()
{
  igstkLogMacro( DEBUG, "AddActorProcessing() called ...\n");
  m_Renderer->AddActor( m_ActorToBeAdded );
  m_PointPicker->AddPickList( m_ActorToBeAdded );
}


/** */
void ViewNew::RequestRemoveActor( vtkProp * actor )
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
void ViewNew::RemoveActorProcessing()
{
  igstkLogMacro( DEBUG, "RemoveActorProcessing() called ...\n");
  m_Renderer->RemoveActor( m_ActorToBeRemoved );
  m_PointPicker->DeletePickList( m_ActorToBeRemoved );
}


/** */
void ViewNew::RequestEnableInteractions()
{
  igstkLogMacro( DEBUG, "RequestEnableInteractions() called ...\n");
  igstkPushInputMacro( EnableInteractions );
  m_StateMachine.ProcessInputs();
}


/** */
void ViewNew::RequestDisableInteractions()
{
  igstkLogMacro( DEBUG, "RequestDisableInteractions() called ...\n");
  igstkPushInputMacro( DisableInteractions );
  m_StateMachine.ProcessInputs();
}

/** */
void ViewNew::EnableInteractionsProcessing()
{
  igstkLogMacro( DEBUG, "EnableInteractionsProcessing() called ...\n");
  m_InteractionHandling = true;
}

/** */
void ViewNew::DisableInteractionsProcessing()
{
  igstkLogMacro( DEBUG, "DisableInteractionsProcessing() called ...\n");
  m_InteractionHandling = false;
}

/** */
void ViewNew::RequestResetCamera()
{
  igstkLogMacro( DEBUG, "RequestResetCamera() called ...\n");
  igstkPushInputMacro( ResetCamera );
  m_StateMachine.ProcessInputs();
}

/** */
void ViewNew::ResetCameraProcessing()
{
  igstkLogMacro( DEBUG, "ResetCameraProcessing() called ...\n");
  m_Renderer->ResetCamera();
  m_Camera->SetClippingRange( 0.1, 10000);
}

/** */
void ViewNew::StartProcessing()
{
  igstkLogMacro( DEBUG, "StartProcessing() called ...\n");
  // the internal pulse generator will control the redraws
  m_PulseGenerator->RequestStart();
}

/** */
void ViewNew::StopProcessing()
{
  igstkLogMacro( DEBUG, "StopProcessing() called ...\n");
  // the internal pulse generator will control the redraws
  m_PulseGenerator->RequestStop();
}

/** Define the refresh rate by programming the internal pulse generator */
void ViewNew::RequestSetRefreshRate( double frequencyHz )
{
  igstkLogMacro( DEBUG, "RequestSetRefreshRate() called ...\n");
  // Let the state machine of the pulse generator manage this request
  m_PulseGenerator->RequestSetFrequency( frequencyHz );
}

/** Refresh the rendering. This function is called in response to pulses from
 * the pulse generator. */
void ViewNew::RefreshRender()
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

  //Third, trigger VTK rendering
  m_RenderWindowInteractor->Render();

  // Last, report to observers that a refresh event took place.
  m_Reporter->InvokeEvent( RefreshEvent() );
}

/** Request for Adding an object to the ViewNew */
void ViewNew::RequestAddObject( ObjectRepresentation* pointer )
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
void ViewNew::RequestAddAnnotation2D ( Annotation2D * annotation )
{
  igstkLogMacro( DEBUG, "RequestAddAnnotation2D() called ...\n");

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


/** Add an object to the ViewNew. This method should only be called by the state
 * machine. The state machine makes sure that this method is called with a valid
 * value in the ObjectToBeAdded. */
void ViewNew::AddObjectProcessing()
{
  igstkLogMacro( DEBUG, "AddObjectProcessing() called ...\n");
  
  m_Objects.push_back( m_ObjectToBeAdded );
  m_RenderWindowInteractor->Modified();
  
  m_ObjectToBeAdded->CreateActors();

  ObjectRepresentation::ActorsListType actors = m_ObjectToBeAdded->GetActors();
  ObjectRepresentation::ActorsListType::iterator actorIt = actors.begin();
  while(actorIt != actors.end())
    {
    this->RequestAddActor(*actorIt);
    actorIt++;
    } 
}

void ViewNew::AddAnnotation2DProcessing( )
{
  igstkLogMacro( DEBUG, "AddAnnotation2DProcessing called ...\n");
  
  const int * size = m_RenderWindowInteractor->GetSize();
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


/** Request for removing a spatial object from the ViewNew */
void ViewNew::RequestRemoveObject( ObjectRepresentation* pointer )
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


/** Remove a spatial object from the ViewNew. This method can only be invoked by
 * the State Machine who will make sure that the content of
 * m_IteratorToObjectToBeRemoved is valid. */
void ViewNew::RemoveObjectProcessing()
{
  igstkLogMacro( DEBUG, "RemoveObjectProcessing() called ...\n");

  m_Objects.erase( m_IteratorToObjectToBeRemoved );
  m_RenderWindowInteractor->Modified();
  
  ObjectRepresentation::ActorsListType actors =
                                         m_ObjectToBeRemoved->GetActors();
  ObjectRepresentation::ActorsListType::iterator actorIt = actors.begin();

  while(actorIt != actors.end())
    {
    this->RequestRemoveActor(*actorIt);
    actorIt++;
    } 
}


/** Request to Start the Pulse Generator for periodically refreshing the ViewNew
 * */
void ViewNew::RequestStart()
{
  igstkLogMacro( DEBUG, "RequestStart() called ...\n");

  igstkPushInputMacro( StartRefreshing );
  m_StateMachine.ProcessInputs();
}


/** Request to Stop the Pulse Generator for periodically refreshing the ViewNew
 * */
void ViewNew::RequestStop()
{
  igstkLogMacro( DEBUG, "RequestStop() called ...\n");

  igstkPushInputMacro( StopRefreshing );
  m_StateMachine.ProcessInputs();
}


/** Request to save the screenshot of the current scene into a file.
 *  The fileformat must be PNG. */
void ViewNew::RequestSaveScreenShot( const std::string & filename )
{
  igstkLogMacro( DEBUG, "RequestSaveScreenShot() called ...\n");

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
void ViewNew::ReportInvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "ReportInvalidRequestProcessing() called ...\n");
}


/** Report that an invalid filename for saving the screen shot */
void ViewNew::ReportInvalidScreenShotFileNameProcessing()
{
  igstkLogMacro( WARNING,
                "ReportInvalidScreenShotFileNameProcessing() called ...\n");
}

/** Save current screenshot while in refreshing state */
void ViewNew::SaveScreenShotWhileRefreshingProcessing()
{
  igstkLogMacro( DEBUG, 
                 "SaveScreenShotWhileRefreshingProcessing() called ...\n");

  m_PulseGenerator->RequestStop();
  
  this->SaveScreenShot();

  m_PulseGenerator->RequestStart();
}

/** Save current screenshot in idle state */
void ViewNew::SaveScreenShotWhileIdleProcessing()
{
  igstkLogMacro( DEBUG, "SaveScreenShotWhileIdleProcessing() called ...\n");
  
  this->SaveScreenShot();
}

/** Save current screenshot */
void ViewNew::SaveScreenShot()
{
  igstkLogMacro( DEBUG, "SaveScreenShot() called ...\n");

  vtkWindowToImageFilter * windowToImageFilter = vtkWindowToImageFilter::New();

  vtkPNGWriter * writer = vtkPNGWriter::New();

  windowToImageFilter->SetInput( m_RenderWindow );

  windowToImageFilter->Update();

  writer->SetInput( windowToImageFilter->GetOutput() );
  
  writer->SetFileName( m_ScreenShotFileName.c_str() );
  
  m_RenderWindow->Render();
  
  writer->Write();

  writer->SetInput( NULL );
  windowToImageFilter->SetInput( NULL );

  windowToImageFilter->Delete();
  writer->Delete();
}

void
ViewNew
::SetInteractorStyle( vtkInteractorStyle * interactorStyle )
{
  m_RenderWindowInteractor->SetInteractorStyle( interactorStyle );
}


void 
ViewNew
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
std::ostream& operator<<(std::ostream& os, const ViewNew& o)
{
  o.Print(os);
  return os;
}


/** Print object information */
void ViewNew::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "RTTI typeinfo:   " << typeid( *this ).name() << std::endl;
  os << indent << "RenderWindow Pointer: " << this->m_RenderWindow << std::endl;
  os << indent << "RenderWindowInteractor Pointer: " 
               << *(this->m_RenderWindowInteractor) << std::endl;
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
