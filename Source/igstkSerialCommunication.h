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

#include "igstkCommunication.h"
#include "igstkLogger.h"
#include "igstkMacros.h"
#include "igstkStateMachine.h"

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
  const unsigned int m_ReadBufferSize; // read buffer size in bytes
  const unsigned int m_WriteBufferSize;// write buffer size in bytes
  const unsigned int m_PortRestSpan; // period of rest in communication, in msecs.

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

  typedef igstk::StateMachine< SerialCommunication > StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer ActionType;
  typedef StateMachineType::StateType              StateType;
  typedef StateMachineType::InputType              InputType;

  /** Data type for communication */
  typedef std::string  CommunicationDataType;

  typedef SerialCommunication            Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /**  Run-time type information (and related methods). */
  itkTypeMacro(SerialCommunication, Object);

  /** Method for creation of a reference counted object. */
  NewMacro(Self);  

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

  void OpenCommunication( const void *data  = NULL );

  /** The method CloseCommunication closes the communication. */
  void CloseCommunication( void );

  /**Rests communication port by suspending character transmission  
  and placing the transmission line in a break state, and restarting
  transmission after a short delay.*/
  void RestCommunication( void );

  /** Flushes output buffer of any waiting commands to the hardware */
  void FlushOutputBuffer( void );

  /** SendString method sends the string to the hardware. */
  void SendString( const CommunicationDataType& message );

  /** ReceiveString method sends the string to the hardware. */
  void ReceiveString( void ); //const CommunicationDataType& message );

   /** The SetLogger method is used to attach a logger object to the
   serial communication object for logging. */
  void SetLogger( LoggerType* logger );

  /** Events initiated by SerialCommunication object */
  itkEventMacro( OpenPortFailureEvent, itk::AnyEvent );
  itkEventMacro( SetupCommunicationParametersFailureEvent, itk::AnyEvent );
  itkEventMacro( SetDataBufferSizeFailureEvent, itk::AnyEvent );
  itkEventMacro( CommunicationTimeoutSetupFailureEvent, itk::AnyEvent );
  itkEventMacro( RestCommunicationFailureEvent, itk::AnyEvent );
  itkEventMacro( FlushOutputBufferFailureEvent, itk::AnyEvent );
  itkEventMacro( OverlappedEventCreationFailureEvent, itk::AnyEvent );
  itkEventMacro( SendStringSuccessfulEvent, itk::AnyEvent );
  itkEventMacro( SendStringFailureEvent, itk::AnyEvent );
  itkEventMacro( SendStringWriteTimeoutEvent, itk::AnyEvent );
  itkEventMacro( SendStringWaitTimeoutEvent, itk::AnyEvent );
  itkEventMacro( CommunicationStatusReportFailureEvent, itk::AnyEvent );
  itkEventMacro( ReceiveStringSuccessfulEvent, itk::AnyEvent );
  itkEventMacro( ReceiveStringFailureEvent, itk::AnyEvent );
  itkEventMacro( ReceiveStringReadTimeoutEvent, itk::AnyEvent );
  itkEventMacro( ReceiveStringWaitTimeoutEvent, itk::AnyEvent );

protected:

  SerialCommunication();

  ~SerialCommunication();

  /** Opens serial port for communication; */
  virtual void OpenCommunicationPortProcessing( void );

  /** Closes serial port  */
  virtual void CloseCommunicationPortProcessing( void );
  virtual void ClearBuffersAndCloseCommunicationPortProcessing( void );

  /** Set up communication time out values. */
  virtual void SetCommunicationTimeoutProcessing( void );

  /** Sets up communication on the open port as per the communication parameters. */
  virtual void SetupCommunicationProcessing( void );

  /** Set up data buffer size. */
  virtual void SetDataBufferSizeProcessing( void );

  /**Rests communication port by suspending character transmission  
  and placing the transmission line in a break state, and restarting
  transmission after a short delay.*/
  virtual void RestCommunicationProcessing( void );

  virtual void FlushOutputBufferProcessing( void );

  virtual void SendStringProcessing( void );

  virtual void ReceiveStringProcessing( void );

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
  char *m_InputBuffer;
  char *m_OutputBuffer;

  /** Offset of the current location in read buffer */
  int           m_ReadBufferOffset;

  /** Bytes of data received */
  int           m_ReadDataSize;

  /** The Logger instance */
  LoggerType     *m_pLogger;

  int             m_PortNumber;     // Port Number
  
  /** The "StateMachine" instance */
  StateMachineType         m_StateMachine;

  /** List of States */
  StateType                m_IdleState;
  StateType                m_PortOpenState;
  StateType                m_BufferAllocatedState;
  StateType                m_CommunicationParametersSetState;
  StateType                m_PortReadyForCommunicationState;

  /** List of Inputs */
  InputType                m_OpenPortInput;
  InputType                m_SetBuffersInput;
  InputType                m_SetParametersInput;
  InputType                m_SetTimeoutsInput;
  InputType                m_ClosePortInput;
  InputType                m_RestCommunication;
  InputType                m_FlushOutputBuffer;
  InputType                m_SendString;
  InputType                m_ReceiveString;

  // OS related variables
  HandleType      m_PortHandle;     // com port handle
};

} // end namespace igstk

#endif // __igstkSerialCommunication_h
