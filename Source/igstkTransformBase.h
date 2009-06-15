/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTransformBase.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTransformBase_h
#define __igstkTransformBase_h


#include "igstkTimeStamp.h"
#include "igstkMacros.h"

namespace igstk 
{
 
/** \class TransformBase 
 *  \brief The abstract base class for all of the IGSTK transform types. 
 * 
 * This class is an abstract base class for all of the IGSTK transform types. It
 * provides a time stamp and an error value associated with all igstk 
 * transforms. The time stamp defines the validity period 
 * for the transform, and the error value is the estimation error for the 
 * transform (e.g. an RMS error).
 * 
 * \sa TimeStamp
 *
 * */
class TransformBase
{
public:

  typedef double                      ErrorType;
  typedef TimeStamp::TimePeriodType   TimePeriodType;
  
  /** Constructor and destructor */
  TransformBase();
  TransformBase( const TransformBase & t );
  virtual ~TransformBase();


  /** Returns the estimation error associated with this transform. The correct
   * interpretation of this error depends on the source of the 
   * transformation. */
  igstkGetMacro( Error, ErrorType );
 

  /** Returns the time at which the validity of this transformation starts. The
   * data in this transform should not be used for scenes to be rendered
   * before that validity time. The time is returned in milliseconds. 
   *
   * \sa TimeStamp 
   *
   * */
  TimePeriodType GetStartTime() const;


  /** Returns the time at which the validity of this transformation expires. The
   * data in this transform should not be used for scenes to be rendered
   * after that validity time. The time is returned in milliseconds. 
   *
   * \sa TimeStamp 
   *
   * */
  TimePeriodType GetExpirationTime() const;


  /** Returns the validity status of the transform at the time passed as
   * argument. The transform values should not be used in a scene if the time
   * when the scene is to be rendered returned 'false' when passed to this
   * IsValid() function. The time is passed in milliseconds. 
   *
   * \sa TimeStamp 
   *
   * */
  bool IsValidAtTime( TimePeriodType timeToTestInMilliseconds ) const;

  /** Returns the validity status of the transform when it is called 
   *
   * \sa TimeStamp 
   *
   * */
  bool IsValidNow() const;

  
  /** Method for printing the member variables of this class to an ostream */
  virtual void Print(std::ostream& os, itk::Indent indent) const =0;

protected:

  TimeStamp       m_TimeStamp;
  ErrorType       m_Error;

};

std::ostream& operator<<( std::ostream& os, const igstk::TransformBase& o );
}

#endif
