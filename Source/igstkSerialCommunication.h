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

#include "itkObject.h"
#include "itkEventObject.h"
#include "itkLogger.h"

#include "igstkMacros.h"
#include "igstkCommunication.h"
#include "igstkStateMachine.h"

#include "igstkSerialCommunicationTokens.h"
//#include "igstkSerialCommunicationCommand.h"

namespace igstk
{

// since we can't include the header, define the type and the pointer
class SerialCommunicationCommand;
typedef itk::SmartPointer<SerialCommunicationCommand> SerialCommunicationCommandPointer;

/** \class SerialCommunication
 * 
 * \brief This class implements 32-bit communication over a Serial Port
 *        (RS-232 connection).
 *
 * \ingroup Communication
 */

class SerialCommunication : public Communication
{

protected:

  const unsigned int m_ReadBufferSize;    // read buffer size in bytes.
  const unsigned int m_WriteBufferSize;   // write buffer size in bytes.
  const unsigned int m_PortRestSpan;      // period of rest in communication, in msecs.
//  const int          m_InvalidPortNumber; // Number assigned to m_PortNumber, if port not initialized.

  typedef itk::Logger   LoggerType;

public:

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
  typedef SerialCommunicationDataBitsSize  DataSizeType;

  /** Explicit instantiations for every accepted DataBitsSize. Using types for the
   * sizes enforces safety on the assignment of values because the verification
   * is done at compile time. */
  typedef SerialCommunicationDataBitsSizeValued< 6 > DataBits6;
  typedef SerialCommunicationDataBitsSizeValued< 7 > DataBits7;
  typedef SerialCommunicationDataBitsSizeValued< 8 > DataBits8;



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



  /** Data type for communication */
  typedef SerialCommunication            Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(SerialCommunication, Object);

  /** Abstract class doesn't have a New method */
  //igstkNewMacro(Self);  

  /** Get methods */

  igstkGetMacro( PortNumber, PortNumberType );

  igstkGetMacro( BaudRate, BaudRateType );

  igstkGetMacro( ByteSize, DataSizeType );

  igstkGetMacro( Parity, ParityType );

  igstkGetMacro( StopBits, StopBitsType );

  igstkGetMacro( HardwareHandshake, HardwareHandshakeType );

  /** Set methods */

  igstkSetMacro( PortNumber, PortNumberType );

  igstkSetMacro( BaudRate, BaudRateType );

  igstkSetMacro( ByteSize, DataSizeType );

  igstkSetMacro( Parity, ParityType );

  igstkSetMacro( StopBits, StopBitsType );

  igstkSetMacro( HardwareHandshake, HardwareHandshakeType );


  /** The method OpenCommunication sets up communication as per the data
  provided. */

  bool OpenCommunication( void );

  /** The method CloseCommunication closes the communication. */
  bool CloseCommunication();

  /**Rests communication port by suspending character transmission  
  and placing the transmission line in a break state, and restarting
  transmission after a short delay.*/
  bool RestCommunication();

  /** Flushes output buffer of any waiting commands to the hardware. 
      Only flushes the transmit buffer, not the receive buffer.*/
  bool Flush();

  /** SendString method sends the string via the communication link. */
  bool SendString( const char *message );

  /** ReceiveString method receives the string via the communication link. */
  bool ReceiveString( char *data ); 

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

  /** Declarations related to the State Machine */
  igstkStateMachineMacro();

protected:

  SerialCommunication();

  ~SerialCommunication();

  /** Opens serial port for communication; */
  virtual void OpenPortProcessing( void ) = 0;

  /** Set up data buffer size. */
  virtual void SetUpDataBuffersProcessing( void ) = 0;

  /** Sets up communication on the open port as per the communication parameters. */
  virtual void SetUpDataTransferParametersProcessing( void ) = 0;

  /** Closes serial port  */
  virtual void ClosePortProcessing( void ) = 0;
  virtual void ClearBuffersAndClosePortProcessing( void ) = 0;
  virtual void ClosePortSuccessProcessing( void );
  virtual void ClosePortFailureProcessing( void );

  /**Rests communication port by suspending character transmission  
  and placing the transmission line in a break state, and restarting
  transmission after a short delay.*/
  virtual void RestPortProcessing( void ) = 0;

  virtual void FlushOutputBufferProcessing( void ) = 0;

  virtual void SendStringProcessing( void ) = 0;

  virtual void ReceiveStringProcessing( void ) = 0;

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

protected: 

  /** The GetLogger method return pointer to the logger object. */
  LoggerType* GetLogger(  void );

protected:  // FIXME all these variables should be private
  
  /** Command for the events created. */
  SerialCommunicationCommandPointer  m_pCommand; 

  // Communication Parameters

  /**  Port Number */
  PortNumberType  m_PortNumber;   

  /** Baud rate of communication */
  BaudRateType    m_BaudRate;  

  /** number of bits/byte */
  DataSizeType    m_ByteSize;

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


  
  /** 
   *
   *         State Machine variables 
   *
   **/
  
  /** List of States */
  StateType                m_IdleState;
  StateType                m_AttemptToOpenPortState;
  StateType                m_PortOpenState;
  StateType                m_AttemptToSetUpDataBuffersState;
  StateType                m_DataBuffersSetState;
  StateType                m_AttemptToSetUpDataTransferParametersState;
  
  StateType                m_PortReadyForCommunicationState;
  StateType                m_AttemptToClosePortState;

protected:  // FIXME:  this should be private once the state machine is reorganized.
  /** List of Inputs */
  InputType                m_OpenPortInput;
  InputType                m_OpenPortSuccessInput;
  InputType                m_OpenPortFailureInput;

  InputType                m_SetUpDataBuffersInput;
  InputType                m_DataBuffersSetUpSuccessInput;
  InputType                m_DataBuffersSetUpFailureInput;

  InputType                m_SetUpDataTransferParametersInput;
  InputType                m_DataTransferParametersSetUpSuccessInput;
  InputType                m_DataTransferParametersSetUpFailureInput;

  InputType                m_RestPortInput;
  InputType                m_FlushOutputBufferInput;
  InputType                m_ReceiveStringInput;
  InputType                m_SendStringInput;

  InputType                m_ClosePortInput;
  InputType                m_ClosePortSuccessInput;
  InputType                m_ClosePortFailureInput;

  /** Result of post-transition action taken by the state machine. */
protected:  // FIXME:  this should be private once the state machine is reorganized.

  InputType                *m_pOpenPortResultInput;
  InputType                *m_pDataBuffersSetUpResultInput;
  InputType                *m_pDataTransferParametersSetUpResultInput;
  InputType                *m_pClosePortResultInput;


};

} // end namespace igstk

#endif // __igstkSerialCommunication_h
