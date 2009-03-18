/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    VideoFrameGrabberAndViewerQuadrantViews.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// generated by Fast Light User Interface Designer (fluid) version 1.0107

#ifndef __VideoFrameGrabberAndViewerQuadrantViews_h
#define __VideoFrameGrabberAndViewerQuadrantViews_h

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Tabs.H>

#include "igstkFLTKWidget.h"
#include "igstkView2D.h"
#include "igstkEvents.h"
#include "igstkCTImageSpatialObject.h"

namespace igstk {

/** \class VideoView
* 
* \brief Composite FLTK class defines the standard view window.
*
*/

class VideoFrameGrabberAndViewerQuadrantViews : public Fl_Group {
public:

  VideoFrameGrabberAndViewerQuadrantViews(int X, int Y, int W, int H, const char *L = 0);

  virtual ~VideoFrameGrabberAndViewerQuadrantViews(void);

  typedef igstk::View          ViewType;
  typedef igstk::View2D        ViewType2D;
  
  // Declare View objects
  ViewType2D::Pointer          m_VideoView;
  
  // Declare FLTKWidgetNew objects
  igstk::FLTKWidget * m_VideoWidget;

  struct KeyboardCommandType 
  {
    int key;
    int state;
  };

  struct MouseCommandType 
  {
    int button;
    int dx;
    int dy;
    int x;
    int y;
    int state;
    int key;
    int quadrant;
  };

  struct WindowLevelStructType
  {
    int current_x;
    int current_y;
    int prev_x;
    int prev_y;
  };

  igstkLoadedEventMacro( KeyPressedEvent, IGSTKEvent, KeyboardCommandType );
  igstkLoadedEventMacro( MousePressedEvent, IGSTKEvent, MouseCommandType );

  unsigned long AddObserver( 
    const ::itk::EventObject & event, ::itk::Command * observer );

  void RemoveObserver( unsigned long tag );
  void RemoveAllObservers();

protected:

private:

  // Call backs

  ::itk::Object::Pointer    m_Reporter;

  int handle_key(int event, KeyboardCommandType &keyCommand);
  int handle_mouse(int event, MouseCommandType &mouseCommand);

  int m_Width;
  int m_Height;
  int m_X;
  int m_Y;
  int m_WW;
  int m_HH;

  WindowLevelStructType m_wl;

public:

  virtual int handle(int e);
};

} // end namespace igstk

#endif
