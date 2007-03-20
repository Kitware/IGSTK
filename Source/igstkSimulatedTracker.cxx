/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSimulatedTracker.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters in 
// the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkSimulatedTracker.h"
#include "qcursor.h"
#include "qpoint.h"

namespace igstk
{

SimulatedTracker::SimulatedTracker():m_StateMachine(this)
{
  this->m_Parameter = 0.0;
}

SimulatedTracker::~SimulatedTracker()
{
}

void SimulatedTracker::Initialize()
{
  igstkLogMacro( DEBUG, "SimulatedTracker::Initialize called ...\n");
  m_ValidityTime = 100.0; // 100.0 milliseconds
  this->Tracker::RequestInitialize();
}
    
SimulatedTracker::ResultType SimulatedTracker::InternalOpen( void )
{
  return SUCCESS;
}

SimulatedTracker::ResultType SimulatedTracker::InternalStartTracking( void )
{
  return SUCCESS;
}

SimulatedTracker::ResultType SimulatedTracker::InternalReset( void )
{
  return SUCCESS;
}

SimulatedTracker::ResultType SimulatedTracker::InternalStopTracking( void )
{
  return SUCCESS;
}

SimulatedTracker::ResultType SimulatedTracker::InternalDeactivateTools( void )
{
  return SUCCESS;
}

SimulatedTracker::ResultType SimulatedTracker::InternalClose( void )
{
  return SUCCESS;
}

SimulatedTracker::ResultType SimulatedTracker::InternalActivateTools( void )
{
  igstkLogMacro( DEBUG, "SimulatedTracker::InternalActivateTools called ...\n");
  m_ValidityTime = 100.0; // 100.0 milliseconds
  m_Port = TrackerPortType::New();
  m_Tool = TrackerToolType::New();
  m_Port->AddTool( m_Tool );
  this->AddPort( m_Port );
  return SUCCESS;
}
    
SimulatedTracker::ResultType SimulatedTracker::InternalUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "SimulatedTracker::InternalUpdateStatus called ...\n");

  typedef igstk::Transform   TransformType;
  TransformType transform;
  transform.SetToIdentity( m_ValidityTime );
  
  this->IncrementParameter();

  PositionType  position = this->GetNextPosition();
  RotationType  rotation = this->GetNextRotation();

  typedef TransformType::ErrorType  ErrorType;
  ErrorType errorValue = 0.5; // +/- half Pixel Uncertainty

  transform.SetTranslationAndRotation( 
      position, rotation, errorValue, m_ValidityTime );
  this->SetToolTransform( 0, 0, transform );

  return SUCCESS;
}
 
void
SimulatedTracker::IncrementParameter()
{
  this->m_Parameter += 0.01;  // Circular increments
  if( this->m_Parameter > 1.0 )
    {
    this->m_Parameter = 0.0;
    }
}

SimulatedTracker::PositionType
SimulatedTracker::GetNextPosition()
{
  const double PI2 = 8.0 * atan( 1.0 );
  PositionType position;
  position[0] =  90.0 * sin( this->m_Parameter * PI2 ); 
  position[1] =  90.0 * cos( this->m_Parameter * PI2 ); 
  position[2] =  10.0 * sin( this->m_Parameter * PI2 * 20.0 );
  return position;
}

SimulatedTracker::RotationType
SimulatedTracker::GetNextRotation()
{
  const double PI2 = 8.0 * atan( 1.0 );
  RotationType rotation;
  RotationType::ValueType angle = this->m_Parameter * PI2;
  RotationType::VectorType axis;
  axis[0] = 0.0;
  axis[1] = 0.0;
  axis[2] = 1.0;
  rotation.Set( axis, angle );
  return rotation;
}

void SimulatedTracker::GetTransform(TransformType & transform)
{
  this->GetToolTransform(0, 0, transform);
}

/** Print Self function */
void SimulatedTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Validity Time: " << m_ValidityTime << std::endl;
  
  if( this->m_Tool )
    {
    os << indent << *m_Tool << std::endl;
    }
  
  if( this->m_Port )
    {
    os << indent << *m_Port << std::endl;
    }
}

}
