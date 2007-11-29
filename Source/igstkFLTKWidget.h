/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFLTKWidget.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkFLTKWidget_h
#define __igstkFLTKWidget_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

// VTK 
#include "vtkWorldPointPicker.h" // needed for typedef
class vtkRenderer;
class vtkRenderWindowInteractor;

// FLTK headers
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>

// ITK headers
#include "itkCommand.h"
#include "igstkLogger.h"

// IGSTK headers
#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkViewNew.h"
#include "igstkViewProxy.h"

namespace igstk {
/** \class FLTKWidget
 * 
 * \brief Widget class to develop FLTK based GUI applications.
 */
class FLTKWidget : public Fl_Gl_Window
{

public:
    
  typedef FLTKWidget          Self;
  typedef Fl_Gl_Window        Superclass;

  typedef ViewNew             ViewType;

  typedef vtkWorldPointPicker  PickerType;

  igstkTypeMacro( FLTKWidget, Fl_Gl_Window );
  
  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

  /** Print the object information in a stream. */
  void Print( std::ostream& os, ::itk::Indent indent=0) const;

  /** Set up variables, types and methods related to the Logger */
  igstkLoggerMacro()

  /** Set view */
  void RequestSetView( const ViewType * view );

  /** Disable user interactions with the window via mouse and keyboard */
  void RequestDisableInteractions();

  /** Enable user interactions with the window via mouse and keyboard */
  void RequestEnableInteractions();

  /** Add observer */
  unsigned long AddObserver( const ::itk::EventObject & event, 
                              ::itk::Command * observer );

  /** Get render window interactor */
  vtkRenderWindowInteractor * GetRenderWindowInteractor() const;
  
  typedef ViewProxy< FLTKWidget > ProxyType;

  friend class ViewProxy< FLTKWidget >;

  FLTKWidget( int x, int y, int w, int h, const char *l="");
  virtual ~FLTKWidget( void );

protected:

  // Fl_Gl_Window overrides
  void flush(void);
  void draw( void );
  void hide( void );
  void resize( int x, int y, int w, int h );
  virtual int  handle( int event );
   
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

  /** Set VTK renderw window interactor */
  void SetRenderWindowInteractor( vtkRenderWindowInteractor * interactor );

  /** Set render window ID */
  void SetRenderWindowID();

  /** Set the reporter */
  void SetReporter( ::itk::Object * reporter );

private:

  bool m_InteractionHandling;
  
  ViewType::Pointer m_View; 

  ProxyType         m_ProxyView;

  vtkRenderer       *m_Renderer; 

  bool               m_RenderWindowIDSet;

  vtkRenderWindowInteractor       *m_RenderWindowInteractor; 


  PickerType                  * m_PointPicker;
  ::itk::Object::Pointer        m_Reporter;
 
   /** States for the State Machine */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( ViewConnected );

  /** Inputs to the State machine */
  igstkDeclareInputMacro( ValidView );
  igstkDeclareInputMacro( InValidView );
  igstkDeclareInputMacro( EnableInteractions );
  igstkDeclareInputMacro( DisableInteractions );


};

std::ostream& operator<<(std::ostream& os, const FLTKWidget& o);

} // end namespace igstk

#endif
