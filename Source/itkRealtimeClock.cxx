/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkRealtimeClock.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include "itkRealtimeClock.h"

#if defined(WIN32) || defined(_WIN32)

#include <windows.h>

#else

#include <sys/time.h>

#endif  // defined(WIN32) || defined(_WIN32)


namespace itk
{

/** Constructor */
RealtimeClock::RealtimeClock():m_Frequency(1)
{
#if defined(WIN32) || defined(_WIN32)

  LARGE_INTEGER frequency;
  QueryPerformanceFrequency(&frequency);

  this->m_Frequency = double((__int64)frequency.QuadPart);

#else

  this->m_Frequency = 1000000;

#endif  // defined(WIN32) || defined(_WIN32)
}

/** Destructor */
RealtimeClock::~RealtimeClock()
{
}

/** Returns a timestamp in seconds */
double RealtimeClock::GetTimestamp() const
{
#if defined(WIN32) || defined(_WIN32)

  LARGE_INTEGER tick;
  QueryPerformanceCounter(&tick);
  return double((__int64)tick.QuadPart) / this->m_Frequency;

#else

  struct timeval tval;
  gettimeofday(&tval, 0);
  return double(tval.tv_sec) + double(tval.tv_usec) / this->m_Frequency;

#endif  // defined(WIN32) || defined(_WIN32)
}


/** Print the object */
void RealtimeClock::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Frequency of the clock: "
    << this->m_Frequency << std::endl;
}

}
