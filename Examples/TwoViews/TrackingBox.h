/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    TrackingBox.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __TrackingBox_h_
#define __TrackingBox_h_

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "igstkMouseTracker.h"
#include "igstkSpatialObject.h"
#include "FL/Fl_Box.h"
#include "igstkView.h"
#include "igstkTransform.h"

class TrackingBox : public Fl_Box
{
public:

  typedef igstk::MouseTracker TrackerType;
  typedef igstk::View  ViewType;
  typedef igstk::SpatialObject ObjectType;

  TrackingBox(int x, int y, int w, int h, const char *l=0) : Fl_Box(x,y,w,h,l) 
    {
    m_Tracking = false;
    m_Tracker = TrackerType::New();
    m_Tracker->Initialize();
    m_Tracker->SetScaleFactor( 1000.0 );
    };

  ~TrackingBox() 
    {
    m_Tracker->Close();
    };

  void SetTracking(bool val) 
    {
    m_Tracking = val;
    if(m_Tracking)
      {
      m_Tracker->StartTracking();
      m_Tracker->Reset();
      }
    }

  void SetObjectToTrack(ObjectType* object) 
    {
    m_Object = object;
    const unsigned int toolPort = 0;
    const unsigned int toolNumber = 0;
    m_Tracker->AttachObjectToTrackerTool( toolPort, toolNumber, m_Object );
    }
  
  void SetView( ViewType * view ) 
    {
    m_View = view;
    }

  void SetView2( ViewType * view ) 
    {
    m_View2 = view;
    }

protected:

  bool m_Tracking;
  TrackerType::Pointer m_Tracker;
  ObjectType* m_Object;
  ViewType*   m_View;
  ViewType*   m_View2;
};
 
#endif
