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

#include "igstkSerialCommunicationTokens.h"

namespace igstk
{

/** \class SerialCommunication
 * 
 * \brief This class implements 32-bit communication over a Serial Port
 *        (RS-232 connection).
 *
 * \ingroup Communication
 */

class SerialCommunication : public Communication
{
public:

  /** Data type for communication */
  typedef SerialCommunication            Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /** Data type for the logger */
  typedef itk::Logger   LoggerType;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(SerialCommunication, Object);

  /** Type used for encoding the port number of the serial port */
  typedef SerialCommunicationPortNumber  PortNumberType;

  /** Explicit instantiations for every accepted BaudRate. Using types for the
   * rates enforces safety on the assignment of values because the verification
   * is done at compile time. */
  typedef SerialCommunicationPortNumberValued< 0 > PortNumber0;
  typedef SerialCommunicationPortNumberValued< 1 > PortNumber1;
  typedef SerialCommunicationPortNumberValued< 2 > PortNumber2;
  typedef SerialCommunicationPortNumberValued< 3 > PortNumber3;

  /** Type used for encoding the baud rate of the serial port */
  typedef SerialCommunicationBaudRate  BaudRateType;

  /** Explicit instantiations for every accepted BaudRate. Using types for the
   * rates enforces safety on the assignment of values because the verification
   * is done at compile time. */
  typedef SerialCommunicationBaudRateValued<  2400 > BaudRate2400;
  typedef SerialCommunicationBaudRateValued<  9600 > BaudRate9600;
  typedef SerialCommunicationBaudRateValued< 14400 > BaudRate14400;
  typedef SerialCommunicationBaudRateValued< 19200 > BaudRate19200;

  /** Type used for encoding the number of bits used for data size by the serial port */
  typedef SerialCommunicationDataBits  DataBitsType;

  /** Explicit instantiations for every accepted DataBitsSize. Using types for the
   * sizes enforces safety on the assignment of values because the verification
   * is done at compile time. */
  typedef SerialCommunicationDataBitsValued< 6 > DataBits6;
  typedef SerialCommunicationDataBitsValued< 7 > DataBits7;
  typedef SerialCommunicationDataBitsValued< 8 > DataBits8;

  /** Type used for encoding the parity used by the serial port */
  typedef SerialCommunicationParity  ParityType;

  /** Explicit instantiations for every accepted parity. Using types for the
   * parity enforces safety on the assignment of values because the
   * verification is done at compile time. */
  typedef SerialCommunicationParityValued< 0 > NoParity;
  typedef SerialCommunicationParityValued< 1 > OddParity;
  typedef SerialCommunicationParityValued< 2 > EvenParity;

  /** Type used for encoding the number of stop bits used by the serial port */
  typedef SerialCommunicationStopBits  StopBitsType;

  /** Explicit instantiations for every accepted number of stop bits. Using
   * types for the stop bits enforces safety on the assignment of values
   * because the verification is done at compile time. */
  typedef SerialCommunicationStopBitsValued< 1 > StopBits1;
  typedef SerialCommunicationStopBitsValued< 2 > StopBits2;

  /** Type used for encoding the number of stop bits used by the serial port */
  typedef SerialCommunicationHandshake  HardwareHandshakeType;

  /** Explicit instantiations for every accepted number of stop bits. Using
   * types for the stop bits enforces safety on the assignment of values
   * because the verification is done at compile time. */
  typedef SerialCommunicationHandshakeValued< 0 > HandshakeOff;
  typedef SerialCommunicationHandshakeValued< 1 > HandshakeOn;

  /** Specify which serial port to use.  If communication is open,
   * this has no effect until communication is closed and reopened */
  igstkSetMacro( PortNumber, PortNumberType );
  igstkGetMacro( PortNumber, PortNumberType );

  igstkSetMacro( BaudRate, BaudRateType );
  igstkGetMacro( BaudRate, BaudRateType );

  igstkSetMacro( DataBits, DataBitsType );
  igstkGetMacro( DataBits, DataBitsType );

  igstkSetMacro( Parity, ParityType );
  igstkGetMacro( Parity, ParityType );

  igstkSetMacro( StopBits, StopBitsType );
  igstkGetMacro( StopBits, StopBitsType );

  igstkSetMacro( HardwareHandshake, HardwareHandshakeType );
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
