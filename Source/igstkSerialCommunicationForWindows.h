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

  typedef SerialCommunicationForWindows  Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /**  Run-time type information (and related methods). */
  itkTypeMacro(SerialCommunicationForWindows, Object);

  /** Method for creation of a reference counted object. */
  NewMacro(Self);  

protected:

  SerialCommunicationForWindows();

//  ~SerialCommunicationForWindows();

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

  HandleType      m_PortHandle;     // com port handle
};

} // end namespace igstk

#endif // __igstkSerialCommunicationForWindows_h
