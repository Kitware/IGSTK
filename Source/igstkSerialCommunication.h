/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunication.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkSerialCommunication_h
#define __igstkSerialCommunication_h

// Disabling warning C4355: 'this' : used in base member initializer list
#if defined(_MSC_VER)
#pragma warning ( disable : 4355 )
#endif

#include "itkObject.h"
#include "itkEventObject.h"
#include "itkLogger.h"

#include "igstkMacros.h"
#include "igstkCommunication.h"
#include "igstkStateMachine.h"

namespace igstk
{

/** \class SerialCommunication
 * 
 * \brief This class implements communication over a serial port
 *        via RS-232.
 *
 * \ingroup Communication
 */

class SerialCommunication : public Communication
{
public:

  /** Available port numbers. */
  enum PortNumberType { PortNumber0 = 0,
                        PortNumber1 = 1,
                        PortNumber2 = 2,
                        PortNumber3 = 3 };

  /** Available baud rates. */
  enum BaudRateType { BaudRate9600 = 9600,
                      BaudRate19200 = 19200,
                      BaudRate38400 = 38400,
                      BaudRate57600 = 57600,
                      BaudRate115200 = 115200 };

  /** Available data bits settings. */
  enum DataBitsType { DataBits7 = 7,
                      DataBits8 = 8 };

  /** Available parity settings. */
  enum ParityType { NoParity = 'N',
                    OddParity = 'O',
                    EvenParity = 'E' };

  /** Available stop bits settings. */
  enum StopBitsType { StopBits1 = 1,
                      StopBits2 = 2 };

  /** Available hardware handshaking settings. */
  enum HardwareHandshakeType { HandshakeOff = 0,
                               HandshakeOn = 1 };

  /** Data type for communication */
  typedef SerialCommunication            Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /** Data type for the logger */
  typedef itk::Logger   LoggerType;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(SerialCommunication, Object);

  /** Specify which serial port to use.  If communication is open,
      this has no effect until communication is closed and reopened */
  igstkSetMacro( PortNumber, PortNumberType );
  /** Get the serial port, where the ports are numbered 0 through 3 */
  igstkGetMacro( PortNumber, PortNumberType );

  /** Set the baud rate to use.  Baud rates of 56000 or higher should
      not be used unless some sort of error checking is in place. */
  igstkSetMacro( BaudRate, BaudRateType );
  /** Get the baud rate. */
  igstkGetMacro( BaudRate, BaudRateType );

  /** Set the number of bits per character.  This should usually be
      set to 8, since 7 bits is only valid for pure ASCII data. */
  igstkSetMacro( DataBits, DataBitsType );
  /** Get the number of bits per character. */
  igstkGetMacro( DataBits, DataBitsType );

  /** Set the parity.  The default is no parity. */
  igstkSetMacro( Parity, ParityType );
  /** Get the parity. */
  igstkGetMacro( Parity, ParityType );

  /** Set the number of stop bits.  The default is one stop bit. */
  igstkSetMacro( StopBits, StopBitsType );
  /** Get the number of stop bits. */
  igstkGetMacro( StopBits, StopBitsType );

  /** Set whether to use hardware handshaking. */
  igstkSetMacro( HardwareHandshake, HardwareHandshakeType );
  /** Get whether hardware handshaking is enabled. */
  igstkGetMacro( HardwareHandshake, HardwareHandshakeType );

  /** The method OpenCommunication sets up communication as per the data
      provided. */
  void OpenCommunication( void );

  /** The method CloseCommunication closes the communication. */
  void CloseCommunication();

  /** The method SetTimeoutPeriod sets the amount of time to wait on a reply 
  from the device before generating a timeout event. */
  virtual void SetTimeoutPeriod( int milliseconds ) { };

  /** Send a break in the serial communication, which by definition is
      a series of zeroes that lasts for a 0.3 second duration.  Some
      devices interpret this as a "reset" signal because the device is
      guaranteed to see it even if the baud rate, parity, or data bits
      are not matched between the host and the device. */
  void SendBreak( void );

  /** Write method sends the string via the communication link. */
  void Write( const char *message, int numberOfBytes );

  /** Read method receives the string via the communication link. */
  void Read( char *data, int numberOfBytes, int &bytesRead ); 

  /** Events initiated by SerialCommunication object */
  itkEventMacro( OpenPortFailureEvent, itk::AnyEvent );
  itkEventMacro( SetCommunicationParametersFailureEvent, itk::AnyEvent );
  itkEventMacro( SetDataBufferSizeFailureEvent, itk::AnyEvent );
  itkEventMacro( CommunicationTimeoutSetupFailureEvent, itk::AnyEvent );
  itkEventMacro( SendBreakFailureEvent, itk::AnyEvent );
  itkEventMacro( FlushOutputBufferFailureEvent, itk::AnyEvent );
  itkEventMacro( WriteSuccessEvent, itk::AnyEvent );
  itkEventMacro( WriteFailureEvent, itk::AnyEvent );
  itkEventMacro( WriteTimeoutEvent, itk::AnyEvent );
  itkEventMacro( ReadSuccessEvent, itk::AnyEvent );
  itkEventMacro( ReadFailureEvent, itk::AnyEvent );
  itkEventMacro( ReadTimeoutEvent, itk::AnyEvent );

  /** Declarations related to the State Machine */
  igstkStateMachineMacro();

  /** Declarations related to the Logger */
  igstkLoggerMacro();

protected:

  typedef enum 
  { 
    FAILURE=0, 
    SUCCESS
  } ResultType;

  SerialCommunication();

  ~SerialCommunication();

  // these methods are the interface to the derived classes

  /** Opens serial port for communication; */
  virtual ResultType InternalOpenCommunication( void ) = 0;

  /** Set up data buffer size. */
  virtual ResultType InternalSetUpDataBuffers( void ) = 0;

  /** Set communication parameters on the open port. */
  virtual ResultType InternalSetTransferParameters( void ) = 0;

  /** Closes serial port  */
  virtual ResultType InternalClosePort( void ) = 0;

  /** Closes serial port  */
  virtual ResultType InternalClearBuffersAndClosePort( void ) = 0;

  /** Send a break to the across the serial port */
  virtual void InternalSendBreak( void ) = 0;

  /** Flush the output buffer */
  virtual void InternalFlushOutputBuffer( void ) = 0;

  /** write the data to the serial port */
  virtual void InternalWrite( void ) = 0;

  /** read the data from the serial port */
  virtual void InternalRead( void ) = 0;

  /** Flushes output buffer of any waiting commands to the hardware. 
      Only flushes the transmit buffer, not the receive buffer.*/
  void Flush( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** read buffer size in bytes */
  const unsigned int m_ReadBufferSize;
  /** read buffer size in bytes */
  const unsigned int m_WriteBufferSize;

  // Communication Parameters

  /**  Port Number */
  PortNumberType  m_PortNumber;   

  /** Baud rate of communication */
  BaudRateType    m_BaudRate;  

  /** Number of bits/byte */
  DataBitsType    m_DataBits;

  /** Parity */
  ParityType      m_Parity;

  /** Stop bits: 0,1,2 = 1, 1.5, 2 */
  StopBitsType    m_StopBits;

  /** Hardware handshaking */
  HardwareHandshakeType  m_HardwareHandshake;

  /** Timeout period, in milliseconds */
  int m_TimeoutPeriod;

  /** Input buffer */
  char *m_InputBuffer;

  /** Output buffer */
  char *m_OutputBuffer;

  /** Offset of the current location in read buffer */
  int m_ReadBufferOffset;

  /** Bytes of data received */
  int m_ReadDataSize;

  /** the parameter NumberOfBytes in Write() */
  int m_WriteNumberOfBytes;
  /** the parameter NumberOfBytes in Read() */
  int m_ReadNumberOfBytes;
  /** the parameter ReadBytes in Read() */
  int m_BytesRead;

private:

  /** List of States */
  StateType                m_IdleState;
  StateType                m_AttemptToOpenPortState;
  StateType                m_PortOpenState;
  StateType                m_AttemptToSetUpDataBuffersState;
  StateType                m_DataBuffersSetState;
  StateType                m_AttemptToSetTransferParametersState;
  
  StateType                m_PortReadyForCommunicationState;
  StateType                m_AttemptToClosePortState;

  /** List of Inputs */
  InputType                m_OpenPortInput;
  InputType                m_OpenPortSuccessInput;
  InputType                m_OpenPortFailureInput;

  InputType                m_SetUpDataBuffersInput;
  InputType                m_DataBuffersSetUpSuccessInput;
  InputType                m_DataBuffersSetUpFailureInput;

  InputType                m_SetTransferParametersInput;
  InputType                m_DataTransferParametersSetUpSuccessInput;
  InputType                m_DataTransferParametersSetUpFailureInput;

  InputType                m_SendBreakInput;
  InputType                m_FlushOutputBufferInput;
  InputType                m_ReadInput;
  InputType                m_WriteInput;

  InputType                m_ClosePortInput;
  InputType                m_ClosePortSuccessInput;
  InputType                m_ClosePortFailureInput;

  /** called by state machine serial port is successfully closed */
  virtual void ClosePortSuccessProcessing( void );

  /** called by state machine when serial port fails to close */
  virtual void ClosePortFailureProcessing( void );

  /** Called by the state machine when communication is to be opened */
  void AttemptToOpenCommunication( void );

  /** Called by the state machine when data buffers are to be created */
  void AttemptToSetUpDataBuffers( void );

  /** Called by the state machine when transfer parameters are to be set */
  void AttemptToSetTransferParameters( void );

  /** Called by the state machine when communication is to be closed */
  void AttemptToClosePort( void );

  /** Called by the state machine when communication is to be closed */
  void AttemptToClearBuffersAndClosePort( void );
};

} // end namespace igstk

#endif // __igstkSerialCommunication_h
