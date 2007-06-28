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

// FLTK headers
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>

// ITK headers
#include "itkCommand.h"
#include "itkLogger.h"

// IGSTK headers
#include "igstkMacros.h"
#include "igstkStateMachine.h"
#include "igstkViewNew.h"

namespace igstk {

class FLTKWidget : public Fl_Gl_Window
{

public:
    
  typedef FLTKWidget          Self;
  typedef Fl_Gl_Window        Superclass;

  typedef ViewNew             ViewType;

  igstkTypeMacro( FLTKWidget, Fl_Gl_Window );
  
  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

  /** Print the object information in a stream. */
  void Print( std::ostream& os, ::itk::Indent indent=0) const;

  /** Set up variables, types and methods related to the Logger */
  igstkLoggerMacro()

  /** Enable interaction */
  void EnableInteractions();

  /** Disable interaction */
  void DisableInteractions();

protected:

  FLTKWidget( int x, int y, int w, int h, const char *l="");
  virtual ~FLTKWidget( void );

public:
  
  /** Update the display in order to render the new content of the scene */
  void Update();
 
protected:
  
  // Fl_Gl_Window overrides
  void flush(void);
  void draw( void );
  void resize( int x, int y, int w, int h );
  virtual int  handle( int event );
   
  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 
 
private:
  
  /** Refresh GUI */
  void Refresh();

  /** Change the window size */
  void UpdateSize(int x, int y);

private:
  bool m_InteractionHandling;
  
  ViewType::Pointer m_View; 

};

std::ostream& operator<<(std::ostream& os, const FLTKWidget& o);

} // end namespace igstk

#endif