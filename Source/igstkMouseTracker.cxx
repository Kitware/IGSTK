
/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMouseTracker.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more DEBUGrmation.

=========================================================================*/

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <FL/Fl.H>

#include "igstkMouseTracker.h"

namespace igstk
{

void MouseTracker::Initialize( const char *fileName )
{
  igstkLogMacro( igstk::Logger::DEBUG, "MouseTracker::Initialize called ...\n");
  Tracker::Initialize();
  Fl::run();
}
    
void MouseTracker::SetUpToolsProcessing( void )
{
    igstkLogMacro( igstk::Logger::DEBUG, "MouseTracker::SetUpToolsProcessing called ...\n");
    TrackerPortType    port;
    igstk::TrackerTool tool;
    tool.SetError( 0 );
    port.AddTool( tool );
    this->AddPort( port );
}
    
void MouseTracker::UpdateStatusProcessing( void )
{
    igstkLogMacro( igstk::Logger::DEBUG, "MouseTracker::UpdateStatusProcessing called ...\n");
    typedef itk::Point< double, 3 >  PositionType;
    PositionType  pos;
    pos[0] = Fl::event_x();
    pos[1] = Fl::event_y();
    pos[2] = 0;
    this->SetToolPosition( 0, 0, pos );
}
    
}
