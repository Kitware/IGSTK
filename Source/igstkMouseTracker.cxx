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
  m_ValidityTime = 100.0; // 100.0 milliseconds
  this->SetUpToolsProcessing();
}
    
void MouseTracker::SetUpToolsProcessing( void )
{
  igstkLogMacro( igstk::Logger::DEBUG, "MouseTracker::SetUpToolsProcessing called ...\n");
  m_Port = TrackerPortType::New();
  m_Tool = TrackerToolType::New();
  m_Tool->SetError( 1.0 ); // uncertainty of 1 pixel
  m_Port->AddTool( m_Tool );
  this->AddPort( m_Port );
}
    
void MouseTracker::UpdateStatusProcessing( void )
{
  igstkLogMacro( igstk::Logger::DEBUG, "MouseTracker::UpdateStatusProcessing called ...\n");

  typedef igstk::Transform   TransformType;
  TransformType transform;
  transform.SetToIdentity( m_ValidityTime );
  
  typedef TransformType::VectorType PositionType;
  PositionType  position;
  position[0] = Fl::event_x();
  position[1] = Fl::event_y();
  position[2] = 0;

  typedef TransformType::ErrorType  ErrorType;
  ErrorType errorValue = 0.5; // +/- half Pixel Uncertainty

  transform.SetTranslation( position, errorValue, m_ValidityTime );
  this->SetToolTransform( 0, 0, transform );

}
 
void MouseTracker::GetTransform(TransformType & transform)
{
  this->GetToolTransform(0, 0, transform);
}

}
