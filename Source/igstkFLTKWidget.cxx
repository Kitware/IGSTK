/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFLTKWidget.cxx
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

#include "igstkFLTKWidget.h"
#include <FL/x.H>

namespace igstk
{

/** Constructor */
FLTKWidget::FLTKWidget( int x, int y, int w, int h, const char *l ) : 
Fl_Gl_Window( x, y, w, h, l ), m_StateMachine(this)
{ 
  igstkLogMacro( DEBUG, "Constructor() called ...\n");
  
  m_Logger = NULL;
  
  this->end();

  m_InteractionHandling = true;
  
}

/** Destructor */
FLTKWidget::~FLTKWidget()
{
  igstkLogMacro( DEBUG, "Destructor() called ...\n");
  
  // according to the fltk docs, destroying a widget does NOT remove it from
  // its parent, so we have to do that explicitly at destruction
  // (and remember, NEVER delete() an instance of this class)
  if (parent())
    {
    ((Fl_Group*)parent())->remove(*(Fl_Gl_Window*)this);
    }

}

/** Update the display */
void FLTKWidget::Update()
{
  igstkLogMacro( DEBUG, "Update() called ...\n");
  this->redraw();
}

/** */
void FLTKWidget::Initialize()
{
}

/** */
void FLTKWidget::Enable()
{
}

/** */
void FLTKWidget::Render()
{
}

/** */
void FLTKWidget::EnableInteractions()
{
  m_InteractionHandling = true;
}

/** */
void FLTKWidget::DisableInteractions()
{
  m_InteractionHandling = false;
}

/** this gets called during FLTK window draw()s and resize()s */
void FLTKWidget::UpdateSize(int W, int H)
{
  igstkLogMacro( DEBUG, "UpdateSize() called ...\n");
}

/** Refresh the rendering. This function is called in response to pulses from
 * the pulse generator. */
void FLTKWidget::Refresh()
{
  igstkLogMacro( DEBUG, "Refresh() called ...\n");

  // Third, trigger VTK rendering by invoking a refresh of the GUI.
  this->redraw();
}

/** FLTK event handlers */
void FLTKWidget::flush(void)
{
  igstkLogMacro( DEBUG, "flush() called ...\n");
  // err, we don't want to do any fansy pansy Fl_Gl_Window stuff, so we
  // bypass all of it (else we'll get our front and back buffers in all
  // kinds of tangles, and need extra glXSwapBuffers() calls and all that)
  this->draw();
}

/** Draw function */
void FLTKWidget::draw(void)
{
  igstkLogMacro( DEBUG, "draw() called ...\n");
}

/** Resize function */
void FLTKWidget::resize( int x, int y, int w, int h ) 
{
  igstkLogMacro( DEBUG, "resize() called ...\n");

  // make sure VTK knows about the new situation
  UpdateSize( w, h );
  // resize the FLTK window by calling ancestor method
  Fl_Gl_Window::resize( x, y, w, h ); 
}


/** main FLTK event handler */
int FLTKWidget::handle( int event ) 
{
  igstkLogMacro( DEBUG, "handle() called ...\n");
  
  switch( event ) 
    {
    case FL_FOCUS:
    case FL_UNFOCUS:
      break;

    case FL_KEYBOARD:   // keypress
      return 0;
      break;
     
    case FL_PUSH: // mouse down
      this->take_focus();  // this allows key events to work
      switch( Fl::event_button() ) 
        {
        case FL_LEFT_MOUSE:
          break;
        case FL_MIDDLE_MOUSE:
          break;
        case FL_RIGHT_MOUSE:
          break;
        }
      break; // this break should be here, at least according to 
    case FL_DRAG:
    case FL_MOVE:
    break;

    case FL_RELEASE:    // mouse up
      switch( Fl::event_button() ) 
        {
        case FL_LEFT_MOUSE:
          {
          }
          break;
        case FL_MIDDLE_MOUSE:
          break;
        case FL_RIGHT_MOUSE:
          break;
        }
      break;

    default:    // let the base class handle everything else 
    return Fl_Gl_Window::handle( event );
    } // switch(event)...

  return 1; // we handled the event if we didn't return earlier
}


void 
FLTKWidget
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
std::ostream& operator<<(std::ostream& os, const FLTKWidget & o)
{
  o.Print(os);
  return os;
}


/** Print object information */
void FLTKWidget::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "RTTI typeinfo:   " << typeid( *this ).name() << std::endl;
  os << indent << "InteractionHandling: ";
  os << this->m_InteractionHandling << std::endl;

}

} // end namespace igstk
