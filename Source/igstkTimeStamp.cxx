/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTimeStamp.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTimeStamp.h"

namespace igstk
{

TimeStamp
::TimeStamp()
{
  m_StartTime       = 0;
  m_ExpirationTime  = 0;
}


TimeStamp
::~TimeStamp()
{
  m_StartTime       = 0;
  m_ExpirationTime  = 0;
}


const TimeStamp &
TimeStamp
::operator=( const TimeStamp & inputTimeStamp )
{
  m_StartTime      = inputTimeStamp.m_StartTime;
  m_ExpirationTime = inputTimeStamp.m_ExpirationTime;
  return *this;
}
   

void
TimeStamp
::SetStartTimeNowAndExpireAfter(double millisecondsToExpire) 
{
  const ClickType currentTicks = clock();
  
  m_StartTime = static_cast<double>(currentTicks) /
                static_cast<double>(CLOCKS_PER_SEC);

  m_ExpirationTime = m_StartTime + millisecondsToExpire;
}



double 
TimeStamp
::GetStartTime() const 
{
  return m_StartTime;
}



double 
TimeStamp
::GetExpirationTime() const 
{
  return m_ExpirationTime;
}




bool 
TimeStamp
::IsValidAtTime( double milliseconds ) const
{
  if( m_StartTime > milliseconds )
    {
    return false;
    }

  if( m_ExpirationTime < milliseconds )
    {
    return false;
    }

  return true;
}



} // end namespace igstk


