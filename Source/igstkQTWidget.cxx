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

#include "vtkVersion.h"
#include "vtkCommand.h"
#include "vtkWindowToImageFilter.h"
#include "vtkPNGWriter.h"
#include "vtkViewport.h"
#include "vtkRenderWindow.h"
#include "vtkPropCollection.h"
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
  m_Reporter    = ::itk::Object::New();
}

/** Destructor */
QTWidget::~QTWidget()
{
  igstkLogMacro( DEBUG, "Destructor called ...\n");

  m_PointPicker->Delete();

}

/** Set View */
void QTWidget::SetView( ViewType::Pointer view)
{
  igstkLogMacro( DEBUG, "SetView called ...\n");

  m_View = view;
  
  this->m_ProxyView.Connect( view );
  this->SetRenderWindow( this->m_VTKRenderer->GetRenderWindow());
  this->GetRenderWindow()->GetInteractor()->SetPicker( m_PointPicker );

  //Add actors to the point picker list
  vtkPropCollection * propList = this->m_VTKRenderer->GetViewProps();
  vtkProp * prop;

  while(prop = propList->GetNextProp())
    {
    this->m_PointPicker->AddPickList(prop);
    }
}


/** Update the display */
void QTWidget::Initialize()
{
  igstkLogMacro( DEBUG, "Initialize() called ...\n");
  this->GetInteractor()->Initialize();
}

/** Update the display */
void QTWidget::Update()
{
  igstkLogMacro( DEBUG, "Update() called ...\n");
  if( !this->GetInteractor()->GetInitialized() )
    {
    this->GetInteractor()->Initialize();
    }
  this->markCachedImageAsDirty();
  this->Render();
}

/** */
void QTWidget::Render()
{
  igstkLogMacro( DEBUG, "Render() called ...\n");
  this->GetInteractor()->Render();
}

/** This method is overloaded from the QWidget class in 
 *  order to insert the invocation of the TransformModifiedEvent.
 */
void 
QTWidget
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
                           0, m_VTKRenderer );
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
void QTWidget::mouseMoveEvent(QMouseEvent *e)
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

 iren->InvokeEvent(vtkCommand::MouseMoveEvent, e);

  if(e->buttons() == Qt::LeftButton)
    {
    // Get x,y,z in world coordinates from the clicked point
    m_PointPicker->Pick(e->x(), this->height() - e->y() - 1, 0, m_VTKRenderer);

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
  os << indent << "InteractionHandling: ";
  os << this->m_InteractionHandling << std::endl;
}

} // end namespace igstk
