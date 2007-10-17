/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationForWindows.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkSerialCommunicationForWindows_h
#define __igstkSerialCommunicationForWindows_h

#include <windows.h>

#include "igstkSerialCommunication.h"

namespace igstk
{

/** \class SerialCommunicationForWindows
 * 
 * \brief This class implements Windows specific methods for 32-bit 
 *        communication over a Serial Port(RS-232 connection).
 *
 *
 *  \image html  igstkSerialCommunicationForWindows.png 
 *              "SerialCommunicationForWindows State Machine Diagram"
 *  \image latex igstkSerialCommunicationForWindows.eps  
 *               "SerialCommunicationForWindows State Machine Diagram" 
 *
 *
 * \ingroup Communication
 * \ingroup SerialCommunication
 */

class SerialCommunicationForWindows : public SerialCommunication
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( SerialCommunicationForWindows, 
                                 SerialCommunication )

public:

  /** Return value type for interface functions */ 
  typedef SerialCommunication::ResultType ResultType;

protected:

  /** Windows file handle type */
  typedef HANDLE HandleType;

  /** Constructor */
  SerialCommunicationForWindows();

  /** Destructor */
  ~SerialCommunicationForWindows();

  /** Opens serial port for communication; */
  virtual ResultType InternalOpenPort( void );

  /** Sets up communication on the open port as per the communication
      parameters. */
  virtual ResultType InternalUpdateParameters( void );

  /** Closes serial port  */
  virtual ResultType InternalClosePort( void );

  /** Send a serial break */
  virtual ResultType InternalSendBreak( void );

  /** Sleep for the number of milliseconds specified */
  virtual void InternalSleep( int milliseconds );

  /** Purge the input and output buffers */
  virtual ResultType InternalPurgeBuffers( void );

  /** Write data */
  virtual ResultType InternalWrite( const char *message,
                                    unsigned int numberOfBytes );

  /** Read data */
  virtual ResultType InternalRead( char *data, unsigned int numberOfBytes,
                                   unsigned int &bytesRead );

  /** Set the RTS value 
   *  0 : Clear the RTS (request-to-send) signal 
   *  1 : Sends the RTS signal */
  virtual ResultType InternalSetRTS(unsigned int signal);

  /** Set the timeout */
  virtual ResultType InternalSetTimeout( unsigned int timeoutPeriod );

  /** Print object information. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  unsigned int    m_OldTimeoutPeriod;

  HandleType      m_PortHandle;     // com port handle
};

} // end namespace igstk

#endif // __igstkSerialCommunicationForWindows_h
