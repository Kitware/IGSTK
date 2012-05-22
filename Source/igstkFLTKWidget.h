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

class vtkRenderer;
class vtkRenderWindowInteractor;

// FLTK headers
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>

// IGSTK headers
#include "igstkView.h"
#include "igstkViewProxy.h"

namespace igstk {
/** \class FLTKWidget
 * 
 * \brief Display IGSTK graphical representation in a FLTK window.
 * 
 * This class is useful to develop FLTK based IGSTK application. Using this
 * class, graphical represenation of a surgical scene can be displayed in
 * FLTK window. FLTK mouse events are captured and translated into VTK events.
 * \image html igstkFLTKWidget.png  "State Machine Diagram"
 * \image latex igstkFLTKWidget.eps "State Machine Diagram" 
 *
 * \sa QTWidget
 * \sa View
 *
 * \ingroup View
 *
 *
 */
class FLTKWidget : public Fl_Gl_Window
{

public:
    
  typedef FLTKWidget          Self;
  typedef Fl_Gl_Window        Superclass;

  typedef View                ViewType;

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

  /** Get render window interactor */
  vtkRenderWindowInteractor * GetRenderWindowInteractor() const;

  typedef ViewProxy< FLTKWidget > ProxyType;

  friend class ViewProxy< FLTKWidget >;

  FLTKWidget( int xPos, int yPos, int width, int height, const char *l="");
  virtual ~FLTKWidget( void );

protected:

  // Fl_Gl_Window overrides
  void flush(void);
  void draw( void );
  void hide( void );
  void resize( int xPos, int yPos, int width, int height );
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

  /** Set VTK renderer. This method is used in
   *  Connect() method in ViewProxy */
  void SetRenderer( vtkRenderer * renderer );

  /** Set VTK render window interactor. this method
    * is used in connect() method in ViewProxy class */
  void SetRenderWindowInteractor( vtkRenderWindowInteractor * interactor );

  /** Set render window ID */
  void SetRenderWindowID();

private:

  bool                            m_InteractionHandling;
  
  ViewType::Pointer               m_View; 

  ProxyType                       m_ProxyView;

  vtkRenderer                   * m_Renderer; 

  bool                            m_RenderWindowIDSet;

  vtkRenderWindowInteractor     * m_RenderWindowInteractor; 

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
