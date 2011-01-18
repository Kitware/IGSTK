/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCommunication.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCommunication_h
#define __igstkCommunication_h


#include "igstkObject.h"
#include "igstkMacros.h"
#include "igstkStateMachine.h"


namespace igstk
{
/** \class Communication
    \brief Class Communication is the base class for communication between
    the tracker class and the hardware tracking device. This communication
    may be established via serial port, parallel port, ethernet, or an 
    offline communication (through a file or class emulating tracker response).
    Classes specific to each such communication would be derived from this 
    base Communication class. */

class Communication  : public Object
{
 
public: 

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( Communication, Object )

public:

  typedef enum 
    { 
    FAILURE=0, 
    SUCCESS=1,
    TIMEOUT=2
    } ResultType;

  /** The method OpenCommunication sets up communication as per the data
   *  provided. */
  virtual ResultType OpenCommunication( void );

  /** The method CloseCommunication closes the communication. */
  virtual ResultType CloseCommunication( void );

  /** The method SetTimeoutPeriod sets the amount of time, in
   *  milliseconds to wait on a reply from the device before
   *  generating a timeout event.  The default will vary
   *  depending on the communication method that is used.. */
  igstkSetMacro( TimeoutPeriod, unsigned int );
  igstkGetMacro( TimeoutPeriod, unsigned int );

  /** The method SetReadTerminationCharacter sets a special character that 
   *  the device uses to mark the end of a reply 
   *  (defaults to end-of-file character, ascii 255). */
  igstkSetMacro( ReadTerminationCharacter, char );
  igstkGetMacro( ReadTerminationCharacter, char );

  /** The method SetUseReadTerminationCharacter sets whether to use
   *  the termination character, or not use a termination character. */
  igstkSetMacro( UseReadTerminationCharacter, bool );
  igstkGetMacro( UseReadTerminationCharacter, bool );

  /** Write method sends the string via communication link. */
  virtual ResultType Write( const char * /* data */,
                         unsigned int /* numberOfBytes */ ) { return SUCCESS; }

  /** Read method receives string via communication link. */
  virtual ResultType Read( char * /* data */,
                           unsigned int /* numberOfBytes */,
                           unsigned int & /* bytesRead */ ) { return SUCCESS; } 

protected:

  /** Constructor is protected in order to enforce 
   *  the use of the New() operator */
  Communication( void );

  virtual ~Communication( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  unsigned int m_TimeoutPeriod;

  char m_ReadTerminationCharacter;

  bool m_UseReadTerminationCharacter;
};

} // end of namespace igstk

#endif //__igstk_Communication_h_
