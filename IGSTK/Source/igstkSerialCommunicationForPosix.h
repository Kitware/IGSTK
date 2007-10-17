/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationForPosix.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkSerialCommunicationForPosix_h
#define __igstkSerialCommunicationForPosix_h

#include "igstkSerialCommunication.h"

namespace igstk
{

/** \class SerialCommunicationForPosix
 * 
 * \brief This class implements Posix specific methods for 32-bit 
 *        communication over a Serial Port(RS-232 connection).
 *
 *  \image html  igstkSerialCommunicationForPosix.png  
 *               "SerialCommunicationForPosix State Machine Diagram"
 *  \image latex igstkSerialCommunicationForPosix.eps  
 *               "SerialCommunicationForPosix State Machine Diagram" 
 *
 *
 * \ingroup Communication
 * \ingroup SerialCommunication
 */

class SerialCommunicationForPosix : public SerialCommunication
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( SerialCommunicationForPosix, \
                                 SerialCommunication )

public:

  /** Return value type for interface functions */ 
  typedef SerialCommunication::ResultType ResultType;

protected:

  /** Constructor */
  SerialCommunicationForPosix();

  /** Destructor */
  ~SerialCommunicationForPosix();

  /** Opens serial port for communication; */
  virtual ResultType InternalOpenPort( void );

  /** Set communication on the open port as per the communication
   *  parameters. */
  virtual ResultType InternalUpdateParameters( void );

  /** Closes serial port  */
  virtual ResultType InternalClosePort( void );

  /** Send a serial break */
  virtual ResultType InternalSendBreak( void );

  /** Sleep for the specified number of milliseconds */
  virtual void InternalSleep( unsigned int milliseconds );

  /** Purge the input and output buffers */
  virtual ResultType InternalPurgeBuffers( void );

  /** Write data */
  virtual ResultType InternalWrite( const char *message,
                                    unsigned int numberOfBytes );

  /** Read data */
  virtual ResultType InternalRead( char *data, unsigned int numberOfBytes,
                                   unsigned int &bytesRead );

  /** Set the RTS (ready-to-send) value 
   *  0 : Clear the RTS (request-to-send) signal 
   *  1 : Sends the RTS signal */
  virtual ResultType InternalSetRTS( unsigned int signal );

  /** Print object information. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** value for invalid handle */
  itkStaticConstMacro( INVALID_HANDLE ,int, -1 );

  /** The serial port handle. */
  int             m_PortHandle;

  /** The old timeout value */
  unsigned int    m_OldTimeoutPeriod;
};

} // end namespace igstk

#endif // __igstkSerialCommunicationForPosix_h
