/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkSerialCommunicationForLinux.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

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

  typedef int HandleType;

  typedef SerialCommunicationForLinux  Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /**  Run-time type information (and related methods). */
  itkTypeMacro(SerialCommunicationForLinux, Object);

  /** Method for creation of a reference counted object. */
  NewMacro(Self);  

protected:

  SerialCommunicationForLinux();

//  ~SerialCommunicationForLinux();

  /** Opens serial port for communication; */
  virtual void OpenCommunicationPortProcessing( void );

  /** Set up data buffer size. */
  virtual void SetUpDataBuffersProcessing( void );

  /** Sets up communication on the open port as per the communication parameters. */
  virtual void SetUpDataTransferParametersProcessing( void );

  /** Closes serial port  */
  virtual void CloseCommunicationPortProcessing( void );
  virtual void ClearBuffersAndCloseCommunicationPortProcessing( void );

  /**Rests communication port by suspending character transmission  
  and placing the transmission line in a break state, and restarting
  transmission after a short delay.*/
  virtual void RestCommunicationProcessing( void );

  virtual void FlushOutputBufferProcessing( void );

  virtual void SendStringProcessing( void );

  virtual void ReceiveStringProcessing( void );

private:

  HandleType      m_PortHandle;     // com port handle
};

} // end namespace igstk

#endif // __igstkSerialCommunicationForLinux_h
