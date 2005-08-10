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
#include "igstkEvents.h"
#include "igstkCommunication.h"
#include "igstkStateMachine.h"

namespace igstk
{

/** \class SerialCommunication
 * 
 * \brief This class implements communication over a serial port
 *        via RS-232.
 *
 * This class provides a common interface for the interactions with a serial
 * port independently of the platform. The actual interactions are implemented
 * on derived classes that are platform specific. This class is not intended
 * for being instantiated directly, instead, the derived classes should be
 * used.
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
  enum HandshakeType { HandshakeOff = 0,
                       HandshakeOn = 1 };

  typedef SerialCommunication            Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /** Data type for the logger */
  typedef itk::Logger   LoggerType;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(SerialCommunication, Communication);

  /** Specify which serial port to use.  If communication is open,
   *  this has no effect until communication is closed and reopened. */
  void SetPortNumber( PortNumberType port );
  /** Get the serial port, where the ports are numbered 0 through 3. */
  PortNumberType GetPortNumber() { return m_PortNumber; };

  /** Set the baud rate to use.  Baud rates of 57600 or higher should
   *  not be used unless some sort of error checking is in place. */
  void SetBaudRate( BaudRateType );
  /** Get the baud rate. */
  BaudRateType GetBaudRate() { return m_BaudRate; };

  /** Set the number of bits per character.  This should usually be
   *  set to 8, since 7 bits is only valid for pure ASCII data. */
  void SetDataBits( DataBitsType dataBits );
  /** Get the number of bits per character. */
  DataBitsType GetDataBits() { return m_DataBits; };

  /** Set the parity.  The default is no parity. */
  void SetParity( ParityType parity );
  /** Get the parity. */
  ParityType GetParity() { return m_Parity; };

  /** Set the number of stop bits.  The default is one stop bit. */
  void SetStopBits( StopBitsType stopBits );
  /** Get the number of stop bits. */
  StopBitsType GetStopBits() { return m_StopBits; };

  /** Set whether to use hardware handshaking. */
  void SetHardwareHandshake( HandshakeType handshake );
  /** Get whether hardware handshaking is enabled. */
  HandshakeType GetHardwareHandshake() { return m_HardwareHandshake; };

  /** The method SetTimeoutPeriod sets the amount of time to wait on a reply 
  from the device before generating a timeout event. */
  void SetTimeoutPeriod( int milliseconds );
  /** Get the timeout period */
  int GetTimeoutPeriod() { return m_TimeoutPeriod; };

  /** The method OpenCommunication sets up communication as per the data
      provided. */
  void OpenCommunication( void );

  /** The method CloseCommunication closes the communication. */
  void CloseCommunication();

  /** Send a break in the serial communication, which by definition is
   *  a series of zeroes that lasts for a 0.3 second duration.  Some
   *  devices interpret this as a "reset" signal because the device is
   *  guaranteed to see it even if the baud rate, parity, or data bits
   *  are not matched between the host and the device. */
  void SendBreak( void );

  /** Sleep for the specified number of milliseconds. This is useful
   *  after a reset of a device on the other end of the serial port,
   *  if the device is known to take a certain amount of time to
   *  initialize. */
  void Sleep( int milliseconds );

  /** Purge the contents of the buffers.  This is used if the device 
   *  connected to the serial port has just been reset after an error,
   *  and the contents of the serial port buffers has to be thrown out
   *  before communication can continue.*/
  void PurgeBuffers( void );

  /** Write method sends the string via the communication link. */
  void Write( const char *message, int numberOfBytes );

  /** Read method receives the string via the communication link. The
   *  data will always be null-terminated, so ensure that 'data' is at
   *  least numberOfBytes+1 in size. */
  void Read( char *data, int numberOfBytes, int &bytesRead );

  /** Event that denotes that an error has occurred. */
  itkEventMacro( CommunicationFailureEvent, IGSTKEvent);
  /** Event that denotes that some action has been successful. */
  itkEventMacro( CommunicationSuccessEvent, IGSTKEvent);
  /** Serial port could not be opened. */
  itkEventMacro( OpenPortFailureEvent, CommunicationFailureEvent );
  /** Serial port could not be closed. */
  itkEventMacro( ClosePortFailureEvent, CommunicationFailureEvent );
  /** Data was successfully sent. */
  itkEventMacro( WriteSuccessEvent, CommunicationSuccessEvent );
  /** Error while sending data. */
  itkEventMacro( WriteFailureEvent, CommunicationFailureEvent );
  /** Timeout while sending data. */
  itkEventMacro( WriteTimeoutEvent, WriteFailureEvent );
  /** Data was successfully received. */
  itkEventMacro( ReadSuccessEvent, CommunicationSuccessEvent );
  /** Error while receiving data. */
  itkEventMacro( ReadFailureEvent, CommunicationFailureEvent );
  /** Timeout while waiting for data. */
  itkEventMacro( ReadTimeoutEvent, ReadFailureEvent );
  /** Error while setting some communication parameter. */
  itkEventMacro( SetTransferParametersFailureEvent,
                 CommunicationFailureEvent );
  /** Error while sending a serial break. */
  itkEventMacro( SendBreakFailureEvent,
                 CommunicationFailureEvent );

  /** Declarations related to the State Machine. */
  igstkStateMachineMacro();

  /** Declarations related to the Logger. */
  igstkLoggerMacro();

protected:

  typedef enum 
  { 
    FAILURE=0, 
    SUCCESS
  } ResultType;

  SerialCommunication();

  ~SerialCommunication();

  // These methods are the interface to the derived classes.

  /** Opens serial port for communication; */
  virtual ResultType InternalOpenPort( void ) = 0;

  /** Set communication parameters on the open port. */
  virtual ResultType InternalSetTransferParameters( void ) = 0;

  /** Closes serial port. */
  virtual ResultType InternalClosePort( void ) = 0;

  /** Send a break to the across the serial port. */
  virtual void InternalSendBreak( void ) = 0;

  /** Sleep for the period of time stored in m_SleepPeriod, in millisecs. */
  virtual void InternalSleep( void ) = 0;

  /** Purge the buffers. */
  virtual void InternalPurgeBuffers( void ) = 0;

  /** write the data to the serial port. */
  virtual void InternalWrite( void ) = 0;

  /** read the data from the serial port. */
  virtual void InternalRead( void ) = 0;

  /** Print object information. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  // Communication Parameters

  /**  Port Number */
  PortNumberType  m_PortNumber;   

  /** Baud rate of communication */
  BaudRateType    m_BaudRate;  

  /** Number of bits/byte */
  DataBitsType    m_DataBits;

  /** Parity */
  ParityType      m_Parity;

  /** Stop bits */
  StopBitsType    m_StopBits;

  /** Hardware handshaking */
  HandshakeType  m_HardwareHandshake;

  /** Timeout period, in milliseconds. */
  int m_TimeoutPeriod;

  /** Time to sleep for, in milliseconds. */
  int m_SleepPeriod;

  /** Input data */
  char *m_InputData;

  /** Output data */
  const char *m_OutputData;

  /** The number of bytes to write. */
  int m_BytesToWrite;

  /** Number of bytes to try to read. */
  int m_BytesToRead;

  /** Actual number of bytes read. */
  int m_BytesRead;

private:

  // List of States 
  StateType                m_IdleState;
  StateType                m_AttemptingToOpenPortState;
  StateType                m_PortOpenState;
  StateType                m_AttemptingToSetTransferParametersState;
  StateType                m_ReadyForCommunicationState;
  StateType                m_AttemptingToClosePortState;

  // List of Inputs
  InputType                m_OpenPortInput;
  InputType                m_OpenPortSuccessInput;
  InputType                m_OpenPortFailureInput;

  InputType                m_SetTransferParametersInput;
  InputType                m_SetTransferParametersSuccessInput;
  InputType                m_SetTransferParametersFailureInput;

  InputType                m_SendBreakInput;
  InputType                m_PurgeBuffersInput;
  InputType                m_SleepInput;
  InputType                m_ReadInput;
  InputType                m_WriteInput;

  InputType                m_ClosePortInput;
  InputType                m_ClosePortSuccessInput;
  InputType                m_ClosePortFailureInput;

  /** called by state machine serial port is successfully opened */
  void OpenPortSuccessProcessing( void );

  /** called by state machine when serial port fails to open */
  void OpenPortFailureProcessing( void );

  /** called by state machine serial port is successfully closed */
  void ClosePortSuccessProcessing( void );

  /** called by state machine when serial port fails to close */
  void ClosePortFailureProcessing( void );

  /** Called by the state machine when communication is to be opened */
  void AttemptToOpenPort( void );

  /** Called by the state machine when transfer parameters are to be set */
  void AttemptToSetTransferParameters( void );

  /** Called by the state machine when communication is to be closed */
  void AttemptToClosePort( void );
};

} // end namespace igstk

#endif // __igstkSerialCommunication_h
