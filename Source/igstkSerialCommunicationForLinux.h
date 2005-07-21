/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationForLinux.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkSerialCommunicationForLinux_h
#define __igstkSerialCommunicationForLinux_h

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termio.h> 
#include <sys/time.h>

#include "igstkSerialCommunication.h"

namespace igstk
{

/** \class SerialCommunicationForLinux
 * 
 * \brief This class implements Linux specific methods for 32-bit 
 *        communication over a Serial Port(RS-232 connection).
 * \ingroup Communication
 * \ingroup SerialCommunication
 */

class SerialCommunicationForLinux : public SerialCommunication
{
public:

  typedef SerialCommunicationForLinux  Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(SerialCommunicationForLinux, Object);

  /** Method for creation of a reference counted object. */
  igstkNewMacro(Self);  

protected:

  /** UNIX file handle type */
  typedef int HandleType;

  const HandleType INVALID_HANDLE;
  const int TIMEOUT_PERIOD;

  typedef SerialCommunication::ResultType ResultType;

  SerialCommunicationForLinux();

  // ~SerialCommunicationForLinux();

  /** Opens serial port for communication; */
  virtual ResultType InternalOpenCommunication( void );

  /** Set up data buffer size. */
  virtual ResultType InternalSetUpDataBuffers( void );

  /** Set communication on the open port as per the communication parameters. */
  virtual ResultType InternalSetTransferParameters( void );

  /** Closes serial port  */
  virtual ResultType InternalClosePort( void );

  virtual ResultType InternalClearBuffersAndClosePort( void );

  /** Set the amount of time to wait on a reply from the device before generating a timeout event. */
  virtual ResultType InternalSetTimeoutPeriod( int milliseconds );

  /**Send break, and restarting
  transmission after a short delay.*/
  virtual void InternalSendBreak( void );

  virtual void InternalFlushOutputBuffer( void );

  virtual void InternalWrite( void );

  virtual void InternalRead( void );

private:

  HandleType      m_PortHandle;

  termios         m_SaveTermIOs;
};

} // end namespace igstk

#endif // __igstkSerialCommunicationForLinux_h
