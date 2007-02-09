/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkRealTimeClock.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __igstkRealTimeClock_h
#define __igstkRealTimeClock_h

#include "itkIndent.h"

namespace igstk
{

/** \class RealTimeClock
 * \brief The RealTimeClock provides a timestamp from a real-time clock
 *
 * This class represents a real-time clock object 
 * and provides a timestamp in platform-independent format.
 *
 * \author Hee-Su Kim, Compute Science Dept. Kyungpook National University,
  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
 */
class RealTimeClock 
{
  
public:
  
  /** Define the type for the timestamp */
  typedef double        TimeStampType;

  /** Returns a timestamp in milliseconds   e.g. 52.341243 milliseconds */
  static TimeStampType  GetTimeStamp();

  /** Initialize internal variables on the Clock service.
   *  This method must be called at the begining of every
   *  IGSTK application. */
  static void Initialize();

  /** Print the object */
  static void Print(std::ostream& os, itk::Indent indent=0);

  /** Define the type for the frequency of the clock */
  typedef double        FrequencyType;

protected:

  /** constructor */
  RealTimeClock();

  /** destructor */
  virtual ~RealTimeClock();

  static void PrintSelf( std::ostream& os, itk::Indent indent );

private:
  
  static  FrequencyType    m_Frequency;
  static  TimeStampType    m_Difference;
  static  TimeStampType    m_Origin;

};

} // end of namespace itk


#endif  // __itkRealTimeClock_h
