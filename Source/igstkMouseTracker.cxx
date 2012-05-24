/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMouseTracker.cxx
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

#include <FL/Fl.H>
#include "igstkMouseTracker.h"

namespace igstk
{

MouseTracker::MouseTracker():m_StateMachine(this)
{
  m_ScaleFactor = 1.0;

  double validityTime = 100.0;
  double trackerFrequency = 1000.0 / (validityTime - 10);  
  this->RequestSetFrequency(trackerFrequency ); 
}

MouseTracker::~MouseTracker()
{
}

MouseTracker::ResultType MouseTracker::InternalOpen( void )
{
  return SUCCESS;
}

MouseTracker::ResultType MouseTracker::InternalStartTracking( void )
{
  return SUCCESS;
}

MouseTracker::ResultType MouseTracker::InternalReset( void )
{
  return SUCCESS;
}

MouseTracker::ResultType MouseTracker::InternalStopTracking( void )
{
  return SUCCESS;
}

MouseTracker::ResultType MouseTracker::InternalClose( void )
{
  return SUCCESS;
}
 
MouseTracker::ResultType MouseTracker
::VerifyTrackerToolInformation( const TrackerToolType * itkNotUsed(trackerTool))
{
  return SUCCESS;
}

MouseTracker::ResultType MouseTracker
::RemoveTrackerToolFromInternalDataContainers( 
  const TrackerToolType * itkNotUsed(trackerTool) )
{
  return SUCCESS;
}

MouseTracker::ResultType MouseTracker
::AddTrackerToolToInternalDataContainers( const TrackerToolType * 
                                                 itkNotUsed(trackerTool) )
{
  return SUCCESS;
}
   
MouseTracker::ResultType MouseTracker::InternalUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "MouseTracker::InternalUpdateStatus called ...\n");

  typedef TrackerToolsContainerType::const_iterator  ConstIteratorType;

  TrackerToolsContainerType trackerToolContainer = 
    this->GetTrackerToolContainer();
 
  ConstIteratorType inputItr = trackerToolContainer.begin();
  ConstIteratorType inputEnd = trackerToolContainer.end();
 
  TransformType transform;

  while( inputItr != inputEnd )
    {
    transform.SetToIdentity( this->GetValidityTime() );

    typedef TransformType::VectorType PositionType;
    PositionType  position;

    igstkLogMacro( DEBUG, 
      "FLTK coordinate:" << Fl::event_x() << "," << Fl::event_y());

    position[0] = Fl::event_x() / m_ScaleFactor;
    position[1] = Fl::event_y() / m_ScaleFactor;
    position[2] = 0;

    igstkLogMacro( DEBUG, 
      "Position: " << "(" << position[0] << "," 
                          << position[1] << ")");

    typedef TransformType::ErrorType  ErrorType;
    ErrorType errorValue = 0.5; // +/- half Pixel Uncertainty

    transform.SetTranslation( position, errorValue, this->GetValidityTime() );

    // set the raw transform
    this->SetTrackerToolRawTransform( 
      trackerToolContainer[inputItr->first], transform );

    this->SetTrackerToolTransformUpdate( 
      trackerToolContainer[inputItr->first], true );

    ++inputItr;
    }

  return SUCCESS;
}
 
MouseTracker::ResultType MouseTracker::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG, 
    "MouseTracker::InternalThreadedUpdateStatus called ...\n");
  return SUCCESS;
}

/** Print Self function */
void MouseTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Scale Factor: " << m_ScaleFactor << std::endl;
}

}
