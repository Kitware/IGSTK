/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTimeStamp.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkTimeStamp_h
#define __igstkTimeStamp_h


#include <iostream>
#include "igstkRealTimeClock.h"
#include "itkNumericTraits.h"

namespace igstk 
{

/** \class TimeStamp 
 *  \brief TimeStamp provides periods of validity for data.
 * 
 * A time stamp consists of two time values. They indicate respectively when a
 * particular data object was acquired and when its data will no longer be
 * valid. The purpose of the time stamp is to make sure that at any given
 * moment the toolkit is only managing information whose validity has not
 * expired. For example, the information of position for a tracked surgical
 * instrument will be valid from the moment it was acquired until a new data is
 * made available. The acquisition rate of a tracker will make possible to
 * define an expiration time for the position information associated with that
 * particular surgical instrument.
 * 
 * \warning Clock precision is unfortunately platform-dependent. It is known
 * that Unix systems will provide time resolution of fractions of milliseconds,
 * while Microsoft Windows will only provide milliseconds.  Users must be aware
 * of the particular capabilities of the platform they are using for running
 * the software.
 *
 * \warning TimeStamps are a mechanism for holding relative time periods. They
 * are not reliable as a mechanism for absolute time synchronization. For
 * example, they can not be used for networking synchronization or even
 * multi-machine communications. Times will only be valid inside 
 * the current machine, and probably the current process.
 * 
 * */

class TimeStamp
{

public:

  typedef double      TimePeriodType;

  /** Define the longest time period on the machine */
  static TimePeriodType GetLongestPossibleTime(); 

  /** Get the zero time period on the machine */
  static TimePeriodType GetZeroValue(); 

  /** Compute the intersection of two time stamps */
  static TimeStamp ComputeOverlap( TimeStamp t1, TimeStamp t2 );

  /** Constructor and destructor */
  TimeStamp();
  virtual ~TimeStamp();


  /** Copy one time stamp into another */
  const TimeStamp & operator=( const TimeStamp & inputTimeStamp );

  /** This method sets both the Start time and the Expiration time of the
   * TimeStamp. The StartTime is set to the current time value as returned by
   * the function clock(). The ExpirationTime is set to the StartTime plus the
   * number of millisecondsToExpire argument provided by the user */
  void SetStartTimeNowAndExpireAfter( TimePeriodType millisecondsToExpire);

  
  /** Returns the time in milliseconds at which this stamp started to be valid.
   * This is the time at which the SetStartTimeNowAndExpireAfter() was invoked
   * last time. */
  TimePeriodType GetStartTime() const;

  
  /** Returns the time in milliseconds at which this stamp will expire. This
   * time is computed when the SetStartTimeNowAndExpireAfter() method is
   * invoked. The ExpirationTime is equal to the StartTime plus the value of
   * millisecondsToExpire provided as argument to that method. */
   TimePeriodType GetExpirationTime() const;
    

  /** This method returns 'true' is the current TimeStamp is still valid at the
   * time in milliseconds indicated in the argument. The purpose of this method
   * is to evaluate what data objects are usable at a particular time snapshot.
   * For example, this method will be used for selecting the display mode of
   * objects that are intended to be presented in a scene rendering at the time
   * specified in the argument.
   * */
  bool IsValidAtTime(  TimePeriodType milliseconds ) const;

  /** This method returns 'true' is the current TimeStamp is valid at the
   * time when this method is called. 
   * */
  bool IsValidNow( ) const;

  void Print(std::ostream& os, itk::Indent indent) const;

protected:

  void PrintHeader(std::ostream& os, itk::Indent indent) const;

  void PrintTrailer(std::ostream& itkNotUsed(os), 
                    itk::Indent itkNotUsed(indent)) const;

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 


private:

  TimePeriodType         m_StartTime;   
  TimePeriodType         m_ExpirationTime;

};

std::ostream& operator<<(std::ostream& os, const TimeStamp& o);

}

#endif
