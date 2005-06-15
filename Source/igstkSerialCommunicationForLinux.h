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

  const int INVALID_HANDLE_VALUE;
  const int NDI_MAX_SAVE_STATE;
  const int TIMEOUT_PERIOD;

  typedef int HandleType;

  typedef SerialCommunicationForLinux  Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(SerialCommunicationForLinux, Object);

  /** Method for creation of a reference counted object. */
  igstkNewMacro(Self);  

protected:

  SerialCommunicationForLinux();

//  ~SerialCommunicationForLinux();

  /** Opens serial port for communication; */
  virtual void OpenPortProcessing( void );

  /** Set up data buffer size. */
  virtual void SetUpDataBuffersProcessing( void );

  /** Sets up communication on the open port as per the communication parameters. */
  virtual void SetUpDataTransferParametersProcessing( void );

  /** Closes serial port  */
  virtual void ClosePortProcessing( void );
  virtual void ClearBuffersAndClosePortProcessing( void );

  /**Rests communication port by suspending character transmission  
  and placing the transmission line in a break state, and restarting
  transmission after a short delay.*/
  virtual void RestPortProcessing( void );

  virtual void FlushOutputBufferProcessing( void );

  virtual void SendStringProcessing( void );

  virtual void ReceiveStringProcessing( void );

private:

  HandleType      m_PortHandle;

  HandleType      m_OpenHandles[4];

  termios         m_NDISaveTermIOs[4];
};

} // end namespace igstk

#endif // __igstkSerialCommunicationForLinux_h
