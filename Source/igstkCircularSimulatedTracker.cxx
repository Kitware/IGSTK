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
  this->SetValidityTime(1000.0); // one second (1000 milliseconds).
}

CircularSimulatedTracker::~CircularSimulatedTracker()
{
}

CircularSimulatedTracker::ResultType 
CircularSimulatedTracker::InternalUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "CircularSimulatedTracker::InternalUpdateStatus called ...\n");

  typedef TrackerToolsContainerType::const_iterator  ConstIteratorType;

  TrackerToolsContainerType trackerToolContainer = this->GetTrackerToolContainer();
 
  ConstIteratorType inputItr = trackerToolContainer.begin();
  ConstIteratorType inputEnd = trackerToolContainer.end();
 
  typedef igstk::Transform   TransformType;
  TransformType transform;

  while( inputItr != inputEnd )
    {
    transform.SetToIdentity( this->GetValidityTime() );

    typedef TransformType::VectorType PositionType;
    PositionType  position;
    position[0] = cos( this->m_Angle ) * this->m_Radius;
    position[1] = sin( this->m_Angle ) * this->m_Radius;
    position[2] = 0;

    typedef TransformType::ErrorType  ErrorType;
    ErrorType errorValue = 0.5; // +/- half Pixel Uncertainty

    transform.SetTranslation( position, errorValue, this->GetValidityTime() );

    // set the raw transform
    this->SetTrackerToolRawTransform( trackerToolContainer[inputItr->first], transform );
    this->SetTrackerToolTransformUpdate( trackerToolContainer[inputItr->first], true );
    ++inputItr;
    }

  std::cout << transform << std::endl;

  this->m_Angle += 0.01; // increment angle

  return SUCCESS;
}
 
/** Print Self function */
void CircularSimulatedTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Radius: " << this->m_Radius << std::endl;
}

}
