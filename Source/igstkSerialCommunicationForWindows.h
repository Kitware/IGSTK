/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunicationForWindows.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

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
 * \ingroup Communication
 * \ingroup SerialCommunication
 */

class SerialCommunicationForWindows : public SerialCommunication
{
public:

  typedef HANDLE HandleType;

  const HandleType NDI_INVALID_HANDLE;
  /* time out period in milliseconds */
  const int TIMEOUT_PERIOD;

  typedef SerialCommunicationForWindows  Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(SerialCommunicationForWindows, Object);

  /** Method for creation of a reference counted object. */
  igstkNewMacro(Self);  

protected:

  typedef SerialCommunication::ResultType ResultType;

  SerialCommunicationForWindows();

//  ~SerialCommunicationForWindows();

  /** Opens serial port for communication; */
  virtual ResultType InternalOpenCommunication( void );

  /** Set up data buffer size. */
  virtual ResultType InternalSetUpDataBuffers( void );

  /** Sets up communication on the open port as per the communication parameters. */
  virtual ResultType InternalSetTransferParameters( void );

  /** Closes serial port  */
  virtual ResultType InternalClosePort( void );

  virtual ResultType InternalClearBuffersAndClosePort( void );

  /**Send break and restarting
  transmission after a short delay.*/
  virtual void InternalSendBreak( void );

  /** Set the amount of time to wait on a reply from the device before generating a timeout event. */
  virtual ResultType InternalSetTimeoutPeriod( int milliseconds );

  virtual void InternalFlushOutputBuffer( void );

  virtual void InternalWrite( void );

  virtual void InternalRead( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  HandleType      m_PortHandle;     // com port handle

  COMMTIMEOUTS    m_SaveTimeout;

  DCB             m_SaveDCB;
};

} // end namespace igstk

#endif // __igstkSerialCommunicationForWindows_h
