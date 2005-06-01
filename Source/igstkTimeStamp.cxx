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


void 
TimeStamp
::Print(std::ostream& os, itk::Indent indent) const
{
  this->PrintHeader(os, indent); 
  this->PrintSelf(os, indent.GetNextIndent());
  this->PrintTrailer(os, indent);
}


/**
 * Define a default print header for all objects.
 */
void 
TimeStamp
::PrintHeader(std::ostream& os, itk::Indent indent) const
{
  os << indent << "Transform" << " (" << this << ")\n";
}


/**
 * Define a default print trailer for all objects.
 */
void 
TimeStamp
::PrintTrailer(std::ostream& itkNotUsed(os), itk::Indent itkNotUsed(indent)) const
{
}


/**
 * This operator allows all subclasses of LightObject to be printed via <<.
 * It in turn invokes the Print method, which in turn will invoke the
 * PrintSelf method that all objects should define, if they have anything
 * interesting to print out.
 */
std::ostream& operator<<(std::ostream& os, const TimeStamp& o)
{
  o.Print(os, 0);
  return os;
}


/** Print Self function */
void TimeStamp::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "RTTI typeinfo:   " << typeid( *this ).name() << std::endl;

  os << indent << this->m_StartTime << std::endl;
  os << indent << this->m_ExpirationTime << std::endl;
}



} // end namespace igstk


