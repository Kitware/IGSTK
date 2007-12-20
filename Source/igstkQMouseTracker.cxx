/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkQMouseTracker.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

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
  this->SetValidityTime(100.0);
}

QMouseTracker::~QMouseTracker()
{
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
 
QMouseTracker::ResultType QMouseTracker
::VerifyTrackerToolInformation( TrackerToolType * trackerTool )
{
  return SUCCESS;
}

QMouseTracker::ResultType QMouseTracker::InternalUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "QMouseTracker::InternalUpdateStatus called ...\n");

  typedef TrackerToolsContainerType::const_iterator  ConstIteratorType;

  TrackerToolsContainerType trackerToolContainer = this->GetTrackerToolContainer();
 
  ConstIteratorType inputItr = trackerToolContainer.begin();
  ConstIteratorType inputEnd = trackerToolContainer.end();
 
  typedef igstk::Transform   TransformType;
  TransformType transform;

  while( inputItr != inputEnd )
    {
    transform.SetToIdentity( this->GetValidityTime() );


  QPoint mousePosition = QCursor::pos();

  typedef TransformType::VectorType PositionType;
  PositionType  position;
  position[0] = mousePosition.x() / m_ScaleFactor;
  position[1] = mousePosition.y() / m_ScaleFactor;
  position[2] = 0;

  typedef TransformType::ErrorType  ErrorType;
  ErrorType errorValue = 0.5; // +/- half Pixel Uncertainty

  transform.SetTranslation( position, errorValue, this->GetValidityTime() );

    // set the raw transform
    this->SetTrackerToolRawTransform( trackerToolContainer[inputItr->first], transform );
    this->SetTrackerToolTransformUpdate( trackerToolContainer[inputItr->first], true );
    ++inputItr;
    }

  return SUCCESS;
}
 
/** Print Self function */
void QMouseTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Scale Factor: " << m_ScaleFactor << std::endl;
}

}
