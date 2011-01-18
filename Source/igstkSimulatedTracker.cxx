/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkSimulatedTracker.cxx
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

#include "igstkSimulatedTracker.h"

namespace igstk
{

SimulatedTracker::SimulatedTracker():m_StateMachine(this)
{
}

SimulatedTracker::~SimulatedTracker()
{
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

SimulatedTracker::ResultType SimulatedTracker::InternalClose( void )
{
  return SUCCESS;
}

SimulatedTracker::ResultType 
SimulatedTracker
::VerifyTrackerToolInformation( const TrackerToolType * itkNotUsed(trackerTool))
{
  return SUCCESS;
}

SimulatedTracker::ResultType 
SimulatedTracker::InternalUpdateStatus( void )
{
  return FAILURE;
}

SimulatedTracker::ResultType 
SimulatedTracker::InternalThreadedUpdateStatus( void )
{
  return FAILURE;
}

SimulatedTracker::ResultType 
SimulatedTracker
::RemoveTrackerToolFromInternalDataContainers
( const TrackerToolType * itkNotUsed(trackerTool) )
{
  return FAILURE;
}

SimulatedTracker::ResultType 
SimulatedTracker
::AddTrackerToolToInternalDataContainers
( const TrackerToolType * itkNotUsed(trackerTool) )
{
  return FAILURE;
}

/** Print Self function */
void SimulatedTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

}
