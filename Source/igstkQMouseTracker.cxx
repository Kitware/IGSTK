/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkQMouseTracker.cxx
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

#include "igstkQMouseTracker.h"
#include "qcursor.h"
#include "qpoint.h"

namespace igstk
{

QMouseTracker::QMouseTracker():m_StateMachine(this)
{
  m_ScaleFactor = 1.0;
}

QMouseTracker::~QMouseTracker()
{
}

void QMouseTracker::Initialize()
{
  igstkLogMacro( DEBUG, "QMouseTracker::Initialize called ...\n");
  m_ValidityTime = 100.0; // 100.0 milliseconds
  ((Tracker*)(this))->RequestInitialize();
}
    
QMouseTracker::ResultType QMouseTracker::InternalOpen( void )
{
  return SUCCESS;
}

QMouseTracker::ResultType QMouseTracker::InternalStartTracking( void )
{
  return SUCCESS;
}

QMouseTracker::ResultType QMouseTracker::InternalReset( void )
{
  return SUCCESS;
}

QMouseTracker::ResultType QMouseTracker::InternalStopTracking( void )
{
  return SUCCESS;
}

QMouseTracker::ResultType QMouseTracker::InternalDeactivateTools( void )
{
  return SUCCESS;
}

QMouseTracker::ResultType QMouseTracker::InternalClose( void )
{
  return SUCCESS;
}

QMouseTracker::ResultType QMouseTracker::InternalActivateTools( void )
{
  igstkLogMacro( DEBUG, "QMouseTracker::InternalActivateTools called ...\n");
  m_ValidityTime = 100.0; // 100.0 milliseconds
  m_Port = TrackerPortType::New();
  m_Tool = TrackerToolType::New();
  m_Port->AddTool( m_Tool );
  this->AddPort( m_Port );
  return SUCCESS;
}
    
QMouseTracker::ResultType QMouseTracker::InternalUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "QMouseTracker::InternalUpdateStatus called ...\n");

  typedef igstk::Transform   TransformType;
  TransformType transform;
  transform.SetToIdentity( m_ValidityTime );
  
  QPoint mousePosition = QCursor::pos();

  typedef TransformType::VectorType PositionType;
  PositionType  position;
  position[0] = mousePosition.x() / m_ScaleFactor;
  position[1] = mousePosition.y() / m_ScaleFactor;
  position[2] = 0;

  typedef TransformType::ErrorType  ErrorType;
  ErrorType errorValue = 0.5; // +/- half Pixel Uncertainty

  transform.SetTranslation( position, errorValue, m_ValidityTime );
  this->SetToolTransform( 0, 0, transform );

  return SUCCESS;
}
 
void QMouseTracker::GetTransform(TransformType & transform)
{
  this->GetToolTransform(0, 0, transform);
}

/** Print Self function */
void QMouseTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
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

  os << indent << "Scale Factor: " << m_ScaleFactor << std::endl;
}

}
