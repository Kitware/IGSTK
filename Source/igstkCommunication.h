
/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCommunication.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_Communication_h_
#define __igstk_Communication_h_


#include "itkObject.h"
#include "igstkMacros.h"


namespace igstk
{
/** \class Communication
    \brief Class Communication is the base class for communication between
    the tracker class and the hardware tracking device. This communication
    may be established via serial port, parallel port, ethernet, or an 
    offline communication (through a file or class emulating tracker response).
    Classes specific to each such communication would be derived from this 
    base Communication class.
*/

class Communication  : public itk::Object
{
 
public: 

  typedef Communication Self; 
  typedef itk::Object Superclass; 
  typedef itk::SmartPointer< Self > Pointer; 
  typedef itk::SmartPointer< const Self > ConstPointer; 

  /** Method for defining the name of the class */ 
  igstkTypeMacro(Communication, Object); 

  /** Method for creation through the object factory */ 
  igstkNewMacro(Self); 

  /** The method OpenCommunication sets up communication as per the data
  provided. */
  virtual void OpenCommunication( void );

  /** The method CloseCommunication closes the communication. */
  virtual void CloseCommunication( void );

  /** The method SetTimeoutPeriod sets the amount of time to wait on a reply 
  from the device before generating a timeout event. */
  virtual void SetTimeoutPeriod( int milliseconds ) { };

  /** The method SetReadTerminationCharacter sets a special character that 
  the device uses to mark the end of a reply 
  (defaults to end-of-file character, ascii 255). */
  void SetReadTerminationCharacter( char c ) { m_ReadTerminationCharacter = c; };

  /** The method SetUseReadTerminationCharacter sets 
  whether to use the termination character, or not use a termination character. */
  void SetUseReadTerminationCharacter( bool bUse ) { m_UseReadTerminationCharacter = bUse; };

  /** Write method sends the string via communication link. */
  virtual void Write( const char *data, int numberOfBytes ) { };

  /** Read method receives string via communication link. */
  virtual void Read( char *data, int numberOfBytes, int &bytesRead ) { }; 

protected:

  /** Constructor is protected in order to enforce 
   *  the use of the New() operator */
  Communication( void );

  virtual ~Communication( void );

  /** Flush "flushes" out any commands in the buffer through the communication link. */
  virtual void Flush( void ) { }

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  char m_ReadTerminationCharacter;

  bool m_UseReadTerminationCharacter;
};

} // end of namespace igstk

#endif //__igstk_Communication_h_

