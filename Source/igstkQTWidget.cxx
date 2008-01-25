/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkQTWidget.cxx
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

#include "vtkCommand.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkPropCollection.h"
#include "vtkWorldPointPicker.h"
#include "vtkProp.h"

#if defined(__APPLE__) && defined(VTK_USE_CARBON)
#include "vtkCarbonRenderWindow.h"
#endif

#include "qevent.h"

#include "itksys/SystemTools.hxx"

#include "igstkRenderWindowInteractor.h"   
#include "igstkEvents.h"

#include "igstkQTWidget.h"


namespace igstk
{

/** Constructor */
QTWidget::
#if QT_VERSION < 0x040000
//! constructor for Qt 3
QTWidget(QWidget* parent, const char* name, Qt::WFlags f):
QVTKWidget( parent, name, f ), m_StateMachine(this), m_ProxyView(this)
#else
//! constructor for Qt 4
QTWidget(QWidget* parent, Qt::WFlags f):
QVTKWidget( parent, f ), m_StateMachine(this), m_ProxyView(this)
#endif
{ 
  m_Logger = NULL;
  m_View = ViewType::New();

  m_PointPicker = PickerType::New(); 

  m_Renderer = NULL;
  m_RenderWindowInteractor = NULL;

  igstkAddInputMacro( ValidView );
  igstkAddInputMacro( InValidView );
  igstkAddInputMacro( EnableInteractions );
  igstkAddInputMacro( DisableInteractions );

  igstkAddStateMacro( Idle );
  igstkAddStateMacro( ViewConnected );

  igstkAddTransitionMacro( Idle, ValidView, ViewConnected, ConnectView );   

  igstkAddTransitionMacro( Idle, EnableInteractions,
                           Idle,  ReportInvalidRequest );
  igstkAddTransitionMacro( Idle, DisableInteractions,
                           Idle,  ReportInvalidRequest );

  igstkAddTransitionMacro( ViewConnected, EnableInteractions,
                           ViewConnected,  EnableInteractions );
  igstkAddTransitionMacro( ViewConnected, DisableInteractions,
                           ViewConnected,  DisableInteractions );

  igstkSetInitialStateMacro( Idle );
  m_StateMachine.SetReadyToRun();

  m_InteractionHandling  = true;

}

/** Destructor */
QTWidget::~QTWidget()
{
  igstkLogMacro( DEBUG, "igstkQTWidget::Destructor called ...\n");

  if ( m_PointPicker != NULL )
    {
    m_PointPicker->Delete();
    }
}

/** Set VTK renderer */
void QTWidget::SetRenderer( vtkRenderer * renderer )
{
  this->m_Renderer = renderer;
}

/** Set VTK render window interactor */
void 
QTWidget::SetRenderWindowInteractor( vtkRenderWindowInteractor * interactor )
{
  this->m_RenderWindowInteractor = interactor;
}

/** Add observer */
unsigned long QTWidget::AddObserver( const ::itk::EventObject & event, 
                              ::itk::Command * observer )
{
  igstkLogMacro( DEBUG, "igstkQTWidget::AddObserver() called ...\n");
  
  if ( m_View.IsNull() )
    {
    return 0;
    }
  return m_View->AddObserver( event, observer );
}

/** Remove observer */
void QTWidget::RemoveObserver( unsigned long observerTag )
{
  igstkLogMacro( DEBUG, "igstkQTWidget::RemoveObserver() called ...\n");
  
  if ( m_View.IsNull() )
    {
    return; 
    }

  m_View->RemoveObserver( observerTag ); 
}

/** Request set view */
void QTWidget::RequestSetView( const ViewType* view)
{
  igstkLogMacro( DEBUG, "igstkQTWidget::RequestSetView called ...\n");

  if ( view == NULL )
    {
    igstkPushInputMacro( InValidView );
    }
  else
    {
    m_View = const_cast< ViewType*  >( view );
    igstkPushInputMacro( ValidView );
    }

  m_StateMachine.ProcessInputs();
}
 
/** Set View */
void QTWidget::ConnectViewProcessing( )
{
  igstkLogMacro( DEBUG, "igstkQTWidget::ConnectViewProcessing called ...\n");

  this->m_ProxyView.Connect( m_View );
  this->SetRenderWindow( this->m_Renderer->GetRenderWindow());
  this->GetRenderWindow()->GetInteractor()->SetPicker( m_PointPicker );

  //Add actors to the point picker list
  vtkPropCollection * propList = this->m_Renderer->GetViewProps();
  vtkProp * prop;

  while(prop = propList->GetNextProp())
    {
    this->m_PointPicker->AddPickList(prop);
    }
}

/** Request enable interactions */
void QTWidget::RequestEnableInteractions()
{
  igstkLogMacro( DEBUG, 
          "igstkQTWidget::RequestEnableInteractions() called ...\n");
  igstkPushInputMacro( EnableInteractions );
  m_StateMachine.ProcessInputs();
}


/** Request disable interactions */
void QTWidget::RequestDisableInteractions()
{
  igstkLogMacro( DEBUG, 
          "igstkQTWidget::RequestDisableInteractions() called ...\n");
  igstkPushInputMacro( DisableInteractions );
  m_StateMachine.ProcessInputs();
}

/** */
void QTWidget::EnableInteractionsProcessing()
{
  igstkLogMacro( DEBUG,
         "igstkQTWidget::EnableInteractionsProcessing() called ...\n");
  m_InteractionHandling = true;
}

/** */
void QTWidget::DisableInteractionsProcessing()
{
  igstkLogMacro( DEBUG, "igstkQTWidget::DisableInteractionsProcessing() called ...\n");
  m_InteractionHandling = false;
}

/** This method is overridden to be able to enable/disable interaction 
 */
void QTWidget::mousePressEvent(QMouseEvent* e)
{

  vtkRenderWindowInteractor* interactor = NULL;
  if(this->mRenWin)
    {
    interactor = this->mRenWin->GetInteractor();
    }
  
  if(!interactor || !interactor->GetEnabled() || !m_InteractionHandling)
    {
    return;
    }

  // give interactor the event information
#if QT_VERSION < 0x040000
  interactor->SetEventInformationFlipY(e->x(), e->y(), 
                              (e->state() & Qt::ControlButton) > 0 ? 1 : 0, 
                              (e->state() & Qt::ShiftButton ) > 0 ? 1 : 0, 0,
                              e->type() == QEvent::MouseButtonDblClick ? 1 : 0);
#else
  interactor->SetEventInformationFlipY(e->x(), e->y(), 
                              (e->modifiers() & Qt::ControlModifier) > 0 ? 1 : 0, 
                              (e->modifiers() & Qt::ShiftModifier ) > 0 ? 1 : 0,
                              0,
                              e->type() == QEvent::MouseButtonDblClick ? 1 : 0);
#endif

  // invoke appropriate vtk event
  switch(e->button())
    {
    case Qt::LeftButton:
      interactor->InvokeEvent(vtkCommand::LeftButtonPressEvent, e);
      break;

    case Qt::MidButton:
      interactor->InvokeEvent(vtkCommand::MiddleButtonPressEvent, e);
      break;

    case Qt::RightButton:
      interactor->InvokeEvent(vtkCommand::RightButtonPressEvent, e);
      break;

    default:
      break;
    }
}


/** This method is overloaded from the QWidget class in 
 *  order to insert the invocation of the TransformModifiedEvent.
 */
void 
QTWidget
::mouseReleaseEvent(QMouseEvent* e)
{
  vtkRenderWindowInteractor* interactor = NULL;
  if(this->mRenWin)
    {
    interactor = this->mRenWin->GetInteractor();
    }
  
  if(!interactor || !interactor->GetEnabled() || !m_InteractionHandling)
    {
    return;
    }
  
  // give vtk event information
#if QT_VERSION < 0x040000
  interactor->SetEventInformationFlipY(e->x(), e->y(), 
                                 (e->state() & Qt::ControlButton), 
                                 (e->state() & Qt::ShiftButton ));
#else
  interactor->SetEventInformationFlipY(e->x(), e->y(), 
                                 (e->modifiers() & Qt::ControlModifier), 
                                 (e->modifiers() & Qt::ShiftModifier ));
#endif
  
  // invoke appropriate vtk event
  switch(e->button())
    {
    case Qt::LeftButton:
      {
      interactor->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, e);

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

      m_View->InvokeEvent( transformEvent );

      break;
      }
    case Qt::MidButton:
      interactor->InvokeEvent(vtkCommand::MiddleButtonReleaseEvent, e);
      break;

    case Qt::RightButton:
      interactor->InvokeEvent(vtkCommand::RightButtonReleaseEvent, e);
      break;

    default:
      break;
    }
}

/**
 * This method is overridden from the QWidget class in order to provide
 * transform events when the mouse is moved and the left button is pressed.
 */
void QTWidget::mouseMoveEvent(QMouseEvent *e)
{
  vtkRenderWindowInteractor* interactor = NULL;
  if(this->mRenWin)
    {
    interactor = this->mRenWin->GetInteractor();
    }
    
  if(!interactor || !interactor->GetEnabled() || !m_InteractionHandling)
    {
    return;
    }
  
  // give VTK event information
#if QT_VERSION < 0x040000
  interactor->SetEventInformationFlipY(e->x(), e->y(), 
    (e->state() & Qt::ControlButton) > 0 ? 1 : 0, 
    (e->state() & Qt::ShiftButton  ) > 0 ? 1 : 0);
#else
  interactor->SetEventInformationFlipY(e->x(), e->y(), 
    (e->modifiers() & Qt::ControlModifier) > 0 ? 1 : 0, 
    (e->modifiers() & Qt::ShiftModifier  ) > 0 ? 1 : 0);
#endif

 interactor->InvokeEvent(vtkCommand::MouseMoveEvent, e);

  if(e->buttons() == Qt::LeftButton)
    {
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
    
    m_View->InvokeEvent(transformEvent);
    }
}

/** This method is overridden to be able to enable/disable interaction 
 */
#ifndef QT_NO_WHEELEVENT
void QTWidget::wheelEvent(QWheelEvent* e)
{
  vtkRenderWindowInteractor* interactor = NULL;
  if(this->mRenWin)
    {
    interactor = this->mRenWin->GetInteractor();
    }
  
  if(!interactor || !interactor->GetEnabled() || !m_InteractionHandling)
    {
    return;
    }

// VTK supports wheel mouse events only in version 4.5 or greater
  // give event information to interactor
#if QT_VERSION < 0x040000
  interactor->SetEventInformationFlipY(e->x(), e->y(), 
                             (e->state() & Qt::ControlButton) > 0 ? 1 : 0, 
                             (e->state() & Qt::ShiftButton ) > 0 ? 1 : 0);
#else
  interactor->SetEventInformationFlipY(e->x(), e->y(), 
                             (e->modifiers() & Qt::ControlModifier) > 0 ? 1 : 0, 
                             (e->modifiers() & Qt::ShiftModifier ) > 0 ? 1 : 0);
#endif
  
  // invoke vtk event
  // if delta is positive, it is a forward wheel event
  if(e->delta() > 0)
    {
    interactor->InvokeEvent(vtkCommand::MouseWheelForwardEvent, e);
    }
  else
    {
    interactor->InvokeEvent(vtkCommand::MouseWheelBackwardEvent, e);
    }
}
#endif


/** Report that an invalid or suspicious operation has been requested. This may
 * mean that an error condition has arised in one of the componenta that
 * interact with this class. */
void QTWidget::ReportInvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "ReportInvalidRequestProcessing() called ...\n");
}

/** Report that an invalid view component is specified */
void QTWidget::ReportInvalidViewConnectedProcessing()
{
  igstkLogMacro( WARNING,
                "ReportInvalidViewConnectedProcessing() called ...\n");
}

void 
QTWidget
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
std::ostream& operator<<(std::ostream& os, const QTWidget& o)
{
  o.Print(os);
  return os;
}

/** Print object information */
void 
QTWidget
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "RTTI typeinfo:   " << typeid( *this ).name() << std::endl;
}

} // end namespace igstk
