/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkSerialCommunication.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkSerialCommunication_h
#define __igstkSerialCommunication_h

#include <windows.h>

#include "itkObject.h"
#include "itkEventObject.h"

#include "igstkLogger.h"
#include "igstkMacros.h"
#include "igstkCommunication.h"

namespace igstk
{

/** \class SerialCommunication
 * 
 * \brief This class implements 32-bit communication over a Serial Port
 *        (RS-232 connection).
 * \ingroup Communication
 */

/** INHERIT Communication class from itk::Object **/

class SerialCommunication : public Communication, public itk::Object
{
  const unsigned int m_ReadBufferSize;
  const unsigned int m_WriteBufferSize;

  typedef igstk::Logger   LoggerType;

public:

  typedef enum { 
    BAUD9600  = 9600,  BAUD14400 = 14400, BAUD19200 = 19200,
    BAUD38400 = 38400, BAUD57600 = 57600, BAUD115200 = 115200
  } BaudRateType;

  typedef enum { 
    SEVEN_BITS, EIGHT_BITS
  } ByteSizeType;

  typedef enum {
    NO_PARITY, ODD_PARITY, EVEN_PARITY
  } ParityType;

  typedef enum { 
    ONE_STOP_BIT, TWO_STOP_BITS
  } StopBitsType;

  typedef enum { 
    HANDSHAKE_OFF, HANDSHAKE_ON
  } HardwareHandshakeType;

  typedef HANDLE HandleType;

  SerialCommunication();

  ~SerialCommunication();

  GetMacro(BaudRate, BaudRateType);

  GetMacro(ByteSize, ByteSizeType);

  GetMacro(Parity, ParityType);

  GetMacro(StopBits, StopBitsType);

  GetMacro(HardwareHandshake, HardwareHandshakeType);

  SetMacro(BaudRate, BaudRateType);

  SetMacro(ByteSize, ByteSizeType);

  SetMacro(Parity, ParityType);

  SetMacro(StopBits, StopBitsType);

  SetMacro(HardwareHandshake, HardwareHandshakeType);

  /** The method OpenCommunication sets up communication as per the data
  provided. */

  void OpenCommunication( const void *data );

  /** The method CloseCommunication closes the communication. */
  void CloseCommunication( void );

   /** The SetLogger method is used to attach a logger object to the
   serial communication object for logging. */
  void SetLogger( LoggerType* logger );

  /** Events initiated by SerialCommunication object */
  itkEventMacro( OpenPortFailureEvent, itk::AnyEvent );
  itkEventMacro( SetupCommunicationParametersFailureEvent, itk::AnyEvent );
  itkEventMacro( SetDataBufferSizeFailureEvent, itk::AnyEvent );
  itkEventMacro( CommunicationTimeoutSetupFailureEvent, itk::AnyEvent );

  // FOLLOWING METHODS WOULD BE MADE PROTECTED LATER 
  //================================================

  /** Opens serial port for communication; portNum 0->COM1, 1->COM2, ... */
  virtual void OpenCommunicationPort( const unsigned int portNum );

  /** Closes serial port , if open for communication */
  virtual void CloseCommunicationPort( void );

  /** Set up communication time out values. */
  virtual void SetCommunicationTimeoutParameters( void );

  /** Sets up communication on the open port as per the communication parameters. */
  void SetupCommunicationParameters( void );

  /** Set up data buffer size. */
  virtual void SetDataBufferSizeParameters( void );

private:

  /** The GetLogger method return pointer to the logger object. */
  LoggerType* GetLogger(  void );

  // Communication Parameters

  /** Baud rate of communication */
  BaudRateType    m_BaudRate;  

  /** number of bits/byte */
  ByteSizeType    m_ByteSize;

  /** parity */
  ParityType      m_Parity;

  /** 0,1,2 = 1, 1.5, 2 */
  StopBitsType    m_StopBits;

  /** hardware handshaking */
  HardwareHandshakeType  m_HardwareHandshake;

  // Communication time out parameters
  //** Read operation timeout parameters */
  //ReadTimeout = m_ReadTotalTimeoutConstant + m_ReadTotalTimeoutMultiplier*Number_Of_Bytes_Read 
  /** Maximum time allowed to elapse between the arrival of two characters 
  on the communications line, in milliseconds. */
  unsigned int m_ReadIntervalTimeout;
  /**  Total time-out period for read operations per byte, in milliseconds. */
  unsigned int m_ReadTotalTimeoutMultiplier;
  /** Constant used to calculate the total time-out period for read 
  operations for every red operation, in milliseconds. */
  unsigned int m_ReadTotalTimeoutConstant;
  //** Write operation timeout parameters */
  //WriteTimeout = m_WriteTotalTimeoutConstant + m_WriteTotalTimeoutMultiplier*Number_Of_Bytes_Written 
  /**  Total time-out period for write operations per byte, in milliseconds. */
  unsigned int m_WriteTotalTimeoutMultiplier;
  /** Constant used to calculate the total time-out period for write 
  operations for every write operation, in milliseconds. */
  unsigned int m_WriteTotalTimeoutConstant;

  // Input Buffer
  /** Input buffer */
  unsigned char *m_InputBuffer;

  /** Offset of the current location in read buffer */
  int           m_BufferOffset;

  /** Bytes of data received */
  int           m_DataSize;

  /** The Logger instance */
  LoggerType     *m_pLogger;

  int             m_PortNumber;     // Port Number

  // OS related variables
  HandleType      m_PortHandle;     // com port handle
};

} // end namespace igstk

#endif // __igstkSerialCommunication_h
