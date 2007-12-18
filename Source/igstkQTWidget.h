/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkQTWidget.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __igstkQTWidget_h
#define __igstkQTWidget_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

// QT dared to define macro called DEBUG!!
#define QT_NO_DEBUG 1

// VTK 
#include "vtkInteractorStyle.h"
#include "vtkRenderer.h"
#include "vtkWorldPointPicker.h"

// ITK headers
#include "itkCommand.h"
#include "itkLogger.h"

// IGSTK headers
#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkView.h"
#include "igstkViewProxy.h"

#include "QVTKWidget.h"

namespace igstk {

/** \class QTWidget
 * 
 * \brief Widget class to develop QT based IGSTK application 
 */
class QTWidget : public QVTKWidget
{
public:
    
  typedef QTWidget          Self;
  typedef QVTKWidget        Superclass;
  typedef View           ViewType; 

  typedef vtkWorldPointPicker  PickerType;

  igstkTypeMacro( QTWidget, QVTKWidget );
  
  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

  /** Print the object information in a stream. */
  void Print( std::ostream& os, ::itk::Indent indent=0) const;

  /** Set up variables, types and methods related to the Logger */
  igstkLoggerMacro()

  /** Constructor */
#if QT_VERSION < 0x040000
    //! constructor for Qt 3
    QTWidget(QWidget* parent = NULL, const char* name = NULL, Qt::WFlags f = 0);
#else
    //! constructor for Qt 4
    QTWidget(QWidget* parent = NULL, Qt::WFlags f = 0);
#endif

  /** Destructor */
  virtual ~QTWidget( void );
  
  /** set the view */
  void RequestSetView( const ViewType * view );

  typedef ViewProxy< QTWidget > ProxyType;

  friend class ViewProxy< QTWidget >;

  /** Disable user interactions with the window via mouse and keyboard */
  void RequestDisableInteractions();

  /** Enable user interactions with the window via mouse and keyboard */
  void RequestEnableInteractions();

  /** Add observer */
  unsigned long AddObserver( const ::itk::EventObject & event, 
                              ::itk::Command * observer );
 
  /** Get render window interactor */
  vtkRenderWindowInteractor * GetRenderWindowInteractor();
 
protected:
  
  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Override the method that manages mouse events in order
      to provide picker functionalities */
  void mouseReleaseEvent(QMouseEvent* e);

  /** Override the mouse move event to send transform events
    when the mouse is moved while the left button is down */
  void mouseMoveEvent(QMouseEvent* e);
  void mousePressEvent(QMouseEvent* e);
  void wheelEvent(QWheelEvent* e);
  

private:
  /** Report any invalid request to the logger */
  void ReportInvalidRequestProcessing();

  /** Report Invalid view connected */
  void ReportInvalidViewConnectedProcessing();

  /** Process a valid view component that is connected to the widget */ 
  void ConnectViewProcessing();

  /** Disable keyboard and mouse interactions */
  void DisableInteractionsProcessing();

  /** Enable keyboard and mouse interactions */
  void EnableInteractionsProcessing();

  /** Set VTK renderer */
  void SetRenderer( vtkRenderer * renderer );

  /** Set VTK render window interactor */
  void SetRenderWindowInteractor( vtkRenderWindowInteractor * interactor );
  /** Set the reporter */
  void SetReporter( ::itk::Object * reporter );

  /** Set VTK point picker */ 
  void SetPointPicker( PickerType * picker );
 
private:
  ViewType::Pointer       m_View;


  PickerType                  * m_PointPicker;
  ::itk::Object::Pointer        m_Reporter;
 
  ProxyType                             m_ProxyView;
  vtkRenderer                         * m_Renderer;
  vtkRenderWindowInteractor           * m_RenderWindowInteractor;

  bool                    m_InteractionHandling;

   /** States for the State Machine */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( ViewConnected );

  /** Inputs to the State machine */
  igstkDeclareInputMacro( ValidView );
  igstkDeclareInputMacro( InValidView );
  igstkDeclareInputMacro( EnableInteractions );
  igstkDeclareInputMacro( DisableInteractions );

};

std::ostream& operator<<(std::ostream& os, const QTWidget& o);

} // end namespace igstk

#endif
