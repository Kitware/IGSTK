/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerTool.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTrackerTool.h"
#include "igstkEvents.h"


namespace igstk
{

TrackerTool::TrackerTool(void):m_StateMachine(this)
{
  // Initialize the variables
  m_ValidityPeriod = 0.0; // current values are alreay invalid
  m_Transform.SetToIdentity( m_ValidityPeriod ); 
  m_RawTransform.SetToIdentity( m_ValidityPeriod ); 
  m_ToolCalibrationTransform.SetToIdentity( 1e300 ); 
  m_ToolType = UnknownTool;
  m_Updated = false; // not yet updated

  m_StateMachine.AddInput( m_InitializeInput,  "InitializeInput" );

  m_StateMachine.AddState( m_Invalid,      "Invalid" );
  m_StateMachine.AddState( m_NotAvailable, "NotAvailable" );
  m_StateMachine.AddState( m_Available,    "Available" );
  m_StateMachine.AddState( m_Initialized,  "Initialized" );
  m_StateMachine.AddState( m_Tracking,     "Tracking" );
  m_StateMachine.AddState( m_Visible,      "Visible" );
}

TrackerTool::~TrackerTool(void)
{
}


void 
TrackerTool::SetTransform( const TransformType & transform )
{
  m_Transform = transform;

  TransformModifiedEvent event;
  event.Set( transform );
  this->InvokeEvent( event );
}


/** Print object information */
void TrackerTool::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Transform: " << this->m_Transform << std::endl;
  os << indent << "RawTransform: " << this->m_RawTransform << std::endl;
  os << indent << "ToolCalibrationTransform: "
               << this->m_ToolCalibrationTransform << std::endl;
  os << indent << "ValidityPeriod: " << this->m_ValidityPeriod << std::endl;
}


std::ostream& operator<<(std::ostream& os, const TrackerTool& o)
{
  o.Print(os, 0);
  return os;  
}


}

