/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTransformBase.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTransformBase.h"


namespace igstk
{

TransformBase
::TransformBase()
{
  // Error is NEVER zero. In the best situation is on the range of the smaller
  // non-zero epsilon that can be represented with the ErrorType. 
  m_Error = itk::NumericTraits< ErrorType >::min();
}

TransformBase
::TransformBase( const TransformBase & inputTransform  )
{
  m_Error        = inputTransform.m_Error;
  m_TimeStamp    = inputTransform.m_TimeStamp;
}


TransformBase
::~TransformBase()
{
}


TransformBase::TimePeriodType 
TransformBase
::GetStartTime() const
{
  return m_TimeStamp.GetStartTime();
}


TransformBase::TimePeriodType 
TransformBase
::GetExpirationTime() const
{
  return m_TimeStamp.GetExpirationTime();
}


bool
TransformBase
::IsValidAtTime( TimePeriodType timeToCheckInMilliseconds ) const
{
  return m_TimeStamp.IsValidAtTime( timeToCheckInMilliseconds );
}

bool
TransformBase
::IsValidNow() const
{
  return m_TimeStamp.IsValidNow();
}


/**
 * This operator allows all subclasses of LightObject to be printed via <<.
 * It in turn invokes the Print method, which in turn will invoke the
 * PrintSelf method that all objects should define, if they have anything
 * interesting to print out.
 */
std::ostream& operator<<( std::ostream& os, const TransformBase& o )
{
  o.Print(os, 0);
  return os;
}

} // end namespace igstk
