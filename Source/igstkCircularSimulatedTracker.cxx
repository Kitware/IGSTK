/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkCircularSimulatedTracker.cxx
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

#include "igstkCircularSimulatedTracker.h"

namespace igstk
{

CircularSimulatedTracker::CircularSimulatedTracker():m_StateMachine(this)
{
  this->m_Radius = 1.0;
  this->m_Angle = 0.0;
  this->m_AngularSpeed = 0.01;   // degrees per second
  this->m_TimeOfLastUpdate = RealTimeClock::GetTimeStamp();

  double validityTime = 1000.0;
  double trackerFrequency = 1000.0 / (validityTime - 10);  
  this->RequestSetFrequency(trackerFrequency ); 
}

CircularSimulatedTracker::~CircularSimulatedTracker()
{
}

CircularSimulatedTracker::ResultType
CircularSimulatedTracker::InternalOpen( void )
{
  return SUCCESS;
}

CircularSimulatedTracker::ResultType
CircularSimulatedTracker::InternalStartTracking( void )
{
  return SUCCESS;
}

CircularSimulatedTracker::ResultType
CircularSimulatedTracker::InternalReset( void )
{
  return SUCCESS;
}

CircularSimulatedTracker::ResultType
CircularSimulatedTracker::InternalStopTracking( void )
{
  return SUCCESS;
}

CircularSimulatedTracker::ResultType
CircularSimulatedTracker::InternalClose( void )
{
  return SUCCESS;
}

CircularSimulatedTracker::ResultType
CircularSimulatedTracker::VerifyTrackerToolInformation(
  TrackerToolType * itkNotUsed(trackerTool) )
{
  return SUCCESS;
}


CircularSimulatedTracker::ResultType
CircularSimulatedTracker::InternalUpdateStatus( void )
{
  igstkLogMacro( DEBUG,
    "CircularSimulatedTracker::InternalUpdateStatus called ...\n");

  typedef TrackerToolsContainerType::const_iterator  ConstIteratorType;

  TrackerToolsContainerType trackerToolContainer =
    this->GetTrackerToolContainer();

  ConstIteratorType inputItr = trackerToolContainer.begin();
  ConstIteratorType inputEnd = trackerToolContainer.end();

  TransformType transform;

  transform.SetToIdentity( this->GetValidityTime() );

  typedef TransformType::VectorType PositionType;
  PositionType  position;
  position[0] = cos( this->m_Angle ) * this->m_Radius;
  position[1] = sin( this->m_Angle ) * this->m_Radius;
  position[2] = 0;

  typedef TransformType::ErrorType  ErrorType;
  ErrorType errorValue = 0.5; // +/- half millimeter Uncertainty

  transform.SetTranslation( position, errorValue, this->GetValidityTime() );

  // set the raw transform in all the tracker tools
  while( inputItr != inputEnd )
    {
    this->SetTrackerToolRawTransform(
      trackerToolContainer[inputItr->first], transform );

    this->SetTrackerToolTransformUpdate(
      trackerToolContainer[inputItr->first], true );

    ++inputItr;
    }

  const double MillisecondsToSeconds = 1.0 / 1000.0;
  const double DegreesToRadians = ( atan(1.0) / 45.0 );

  const RealTimeClock::TimeStampType timeNow = RealTimeClock::GetTimeStamp();
  const RealTimeClock::TimeStampType timeLapseInSeconds =
    MillisecondsToSeconds * ( timeNow - this->m_TimeOfLastUpdate );


  this->m_Angle += 
    DegreesToRadians * this->m_AngularSpeed * timeLapseInSeconds;

  this->m_TimeOfLastUpdate = timeNow;

  return SUCCESS;
}

SimulatedTracker::ResultType
CircularSimulatedTracker::InternalThreadedUpdateStatus( void )
{
  return FAILURE;
}

CircularSimulatedTracker::ResultType
CircularSimulatedTracker
::RemoveTrackerToolFromInternalDataContainers
( const TrackerToolType * itkNotUsed(trackerTool) )
{
  return FAILURE;
}

/** Print Self function */
void CircularSimulatedTracker::PrintSelf(
  std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Radius: " << this->m_Radius << std::endl;
  os << indent << "Angle: " << this->m_Angle << std::endl;
  os << indent << "Angular Speed: " << this->m_AngularSpeed << std::endl;

  os << indent << "Time of Last Update : ";
  os << this->m_TimeOfLastUpdate << std::endl;
}

}
