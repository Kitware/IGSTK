/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationForPosix.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkSerialCommunicationForPosix_h
#define __igstkSerialCommunicationForPosix_h

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termio.h> 
#include <sys/time.h>

#include "igstkSerialCommunication.h"

namespace igstk
{

/** \class SerialCommunicationForPosix
 * 
 * \brief This class implements Posix specific methods for 32-bit 
 *        communication over a Serial Port(RS-232 connection).
 *
 *
 *
 *  \image html  igstkSerialCommunicationForPosix.png  "SerialCommunicationForPosix State Machine Diagram"
 *  \image latex igstkSerialCommunicationForPosix.eps  "SerialCommunicationForPosix State Machine Diagram" 
 *
 *        
 * \ingroup Communication
 * \ingroup SerialCommunication
 */

class SerialCommunicationForPosix : public SerialCommunication
{
public:

  /** Return value type for interface functions */ 
  typedef SerialCommunication::ResultType ResultType;

  typedef SerialCommunicationForPosix    Self;
  typedef SerialCommunication            Superclass;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(SerialCommunicationForPosix, SerialCommunication);

  /** Method for creation of a reference counted object. */
  igstkNewMacro(Self);  

protected:

  /** Constructor */
  SerialCommunicationForPosix();

  /** Destructor */
  ~SerialCommunicationForPosix();

  /** Opens serial port for communication; */
  virtual ResultType InternalOpenPort( void );

  /** Set communication on the open port as per the communication
      parameters. */
  virtual ResultType InternalSetTransferParameters( void );

  /** Closes serial port  */
  virtual ResultType InternalClosePort( void );

  /** Send a serial break */
  virtual void InternalSendBreak( void );

  /** Sleep for the amount of time stored in m_SleepPeriod */
  virtual void InternalSleep( void );

  /** Purge the input and output buffers */
  virtual void InternalPurgeBuffers( void );

  /** Write data */
  virtual ResultType InternalWrite( void );

  /** Read data */
  virtual ResultType InternalRead( void );

  /** Print object information. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** The serial port handle. */
  int             m_PortHandle;

  /** Data structure to save the original serial port parameters. */
  termios         m_SaveTermIOs;
};

} // end namespace igstk

#endif // __igstkSerialCommunicationForPosix_h
