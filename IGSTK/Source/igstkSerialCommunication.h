/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSerialCommunication.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

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
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

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
 * \image html  igstkSerialCommunication.png 
 *               "Serial Communication State Machine Diagram"
 *
 * \image latex igstkSerialCommunication.eps 
 *               "Serial Communication State Machine Diagram"
 * 
 *
 *
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
                        PortNumber3 = 3,
                        PortNumber4 = 4,
                        PortNumber5 = 5,
                        PortNumber6 = 6,
                        PortNumber7 = 7 };

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

  typedef Communication::ResultType      ResultType;

  /** Standard traits of a basic class */
  igstkStandardClassBasicTraitsMacro( SerialCommunication, Communication );

  /** Customized New method that will return the implementation of
   * SerialCommunication that is appropriate for this platform.  
   * \sa SerialCommunicationForWindows 
   * \sa SerialCommunicationForPosix
   */
  static Pointer New(void);


  /** Specify which serial port to use.  If communication is open,
   *  this has no effect until communication is closed and reopened. */
  igstkSetMacro( PortNumber, PortNumberType );
  /** Get the serial port, where the ports are numbered 0 through 3. */
  igstkGetMacro( PortNumber, PortNumberType );

  /** Set the baud rate to use.  Baud rates of 57600 or higher should
   *  not be used unless some sort of error checking is in place. */
  igstkSetMacro( BaudRate, BaudRateType );
  /** Get the baud rate. */
  igstkGetMacro( BaudRate, BaudRateType );

  /** Set the number of bits per character.  This should usually be
   *  set to 8, since 7 bits is only valid for pure ASCII data. */
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
  igstkSetMacro( HardwareHandshake, HandshakeType );
  /** Get whether hardware handshaking is enabled. */
  igstkGetMacro( HardwareHandshake, HandshakeType );

  /** Set the name of the file into which the data stream is recorded. */
  void SetCaptureFileName(const char* filename);
  /** Get the filename into which the data stream is recorded. */
  const char* GetCaptureFileName() const;

  /** Set whether to record the data. */
  igstkSetMacro( Capture, bool );
  /** Get whether the data is being recorded. */
  igstkGetMacro( Capture, bool );

  /** Update the communication parameters, in case you need to change
   *  the baud rate, handshaking, timeout, etc. after opening the port */
  ResultType UpdateParameters( void );

  /** The method OpenCommunication sets up communication as per the data
   *  provided. */
  ResultType OpenCommunication( void );

  /** The method CloseCommunication closes the communication. */
  ResultType CloseCommunication( void );

  /** Set the RTS value 
   *  0 : Clear the RTS (request-to-send) signal 
   *  1 : Sends the RTS signal */
  ResultType SetRTS( unsigned int signal );

  /** Write method sends the string via the communication link. */
  ResultType Write( const char *message, unsigned int numberOfBytes );

  /** Read method receives the string via the communication link. The
   *  data will always be null-terminated, so ensure that 'data' is at
   *  least numberOfBytes+1 in size. */
  ResultType Read( char *data, unsigned int numberOfBytes,
                   unsigned int &bytesRead );

  /** Send a break in the serial communication, which by definition is
   *  a series of zeroes that lasts for a 0.3 second duration.  Some
   *  devices interpret this as a "reset" signal because the device is
   *  guaranteed to see it even if the baud rate, parity, or data bits
   *  are not matched between the host and the device. */
  ResultType SendBreak( void );

  /** Purge the contents of the buffers.  This is used if the device 
   *  connected to the serial port has just been reset after an error,
   *  and the contents of the serial port buffers has to be thrown out
   *  before communication can continue. */
  ResultType PurgeBuffers( void );

  /** Sleep for the specified number of milliseconds. This is useful
   *  after a reset of a device on the other end of the serial port,
   *  if the device is known to take a certain amount of time to
   *  initialize. */
  void Sleep( unsigned int milliseconds );

  /** Declarations related to the State Machine. */
  igstkStateMachineMacro();

  /** Declarations related to the Logger. */
  igstkLoggerMacro();

protected:

  SerialCommunication();

  ~SerialCommunication();

  // These methods are the interface to the derived classes.

  /** Opens serial port for communication; */
  virtual ResultType InternalOpenPort( void ) { return SUCCESS; }

  /** Set communication parameters on the open port. */
  virtual ResultType InternalUpdateParameters( void ) { return SUCCESS; }

  /** Closes serial port. */
  virtual ResultType InternalClosePort( void ) { return SUCCESS; }

  /** Closes serial port. */
  virtual ResultType InternalSetRTS( unsigned int ) { return SUCCESS; }

  /** write the data to the serial port. */
  virtual ResultType InternalWrite( const char *, unsigned int ) {
    return TIMEOUT; }

  /** read the data from the serial port. */
  virtual ResultType InternalRead( char *, unsigned int, unsigned int &) {
    return TIMEOUT; }

  /** Send a break to the across the serial port. */
  virtual ResultType InternalSendBreak( void ) { return SUCCESS; }

  /** Purge the buffers. */
  virtual ResultType InternalPurgeBuffers( void ) { return SUCCESS; }

  /** Sleep for the period of time specified, in milliseconds. */
  virtual void InternalSleep( unsigned int ) {};

  /** Print object information. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

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

  /** Time to sleep for, in milliseconds. */
  unsigned int m_SleepPeriod;

  /** Input data */
  char *m_InputData;

  /** Output data */
  const char *m_OutputData;

  /** The number of bytes to write. */
  unsigned int m_BytesToWrite;

  /** Number of bytes to try to read. */
  unsigned int m_BytesToRead;

  /** Actual number of bytes read. */
  unsigned int m_BytesRead;

  /** Type used to map between integer values and inputs */
  typedef std::map<int, InputType>     IntegerInputMapType;
  
  /** Record file name */
  std::string              m_CaptureFileName;

  /** File output stream for recording stream into a file */
  std::ofstream            m_CaptureFileStream;

  /** Current message number */
  unsigned int             m_CaptureMessageNumber;

  /** Recording flag */
  bool                     m_Capture;
  
  /** Logger for recording */
  igstk::Object::LoggerType::Pointer     m_Recorder;
  
  /** RTS Signal type */
  unsigned int             m_RTSSignal;

  /** LogOutput for File output stream */
  itk::StdStreamLogOutput::Pointer  m_CaptureFileOutput;
  
  /** For storing return values */
  ResultType                m_ReturnValue;
  
  /** For mapping return values to state machine inputs */
  IntegerInputMapType       m_ResultInputMap;
  
  // List of States 
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( AttemptingToOpenPort );
  igstkDeclareStateMacro( PortOpen );
  igstkDeclareStateMacro( AttemptingToUpdateParameters );
  igstkDeclareStateMacro( ReadyForCommunication );
  igstkDeclareStateMacro( AttemptingToClosePort );
  igstkDeclareStateMacro( AttemptingToRead );
  igstkDeclareStateMacro( AttemptingToWrite );
  igstkDeclareStateMacro( AttemptingToSendBreak );
  igstkDeclareStateMacro( AttemptingToPurgeBuffers );
  igstkDeclareStateMacro( AttemptingToSetRTS );
  igstkDeclareStateMacro( Sleep );

  // List of Inputs
  igstkDeclareInputMacro( Success );
  igstkDeclareInputMacro( Failure );
  igstkDeclareInputMacro( Timeout );
  igstkDeclareInputMacro( OpenPort );
  igstkDeclareInputMacro( ClosePort );
  igstkDeclareInputMacro( UpdateParameters );
  igstkDeclareInputMacro( Read );
  igstkDeclareInputMacro( Write );
  igstkDeclareInputMacro( SendBreak );
  igstkDeclareInputMacro( PurgeBuffers );
  igstkDeclareInputMacro( Sleep );
  igstkDeclareInputMacro( SetRTS );

  /** called by state machine serial port is successfully opened */
  void OpenPortSuccessProcessing( void );

  /** called by state machine when serial port fails to open */
  void OpenPortFailureProcessing( void );

  /** called by state machine serial port is successfully closed */
  void ClosePortSuccessProcessing( void );

  /** called by state machine when serial port fails to close */
  void ClosePortFailureProcessing( void );
  
  /** called by state machine when writing succeeded */
  void SuccessProcessing( void );
  
  /** called by state machine when writing failed */
  void FailureProcessing( void );
  
  /** called by state machine when writing was timed out */
  void TimeoutProcessing( void );
  
  /** Null operation for a state machine transition */
  void NoProcessing();

  /** Called by the state machine when communication is to be opened */
  void AttemptToOpenPortProcessing( void );

  /** Called by the state machine when transfer parameters are to be set */
  void AttemptToUpdateParametersProcessing( void );

  /** Called by the state machine when communication is to be closed */
  void AttemptToClosePortProcessing( void );
    
  /** Called by the state machine when communication is to be closed */
  void AttemptToSetRTSProcessing( void );

  /** Called by the state machine when writing is to be done */
  void AttemptToWriteProcessing( void );
  
  /** Called by the state machine when reading is to be done */
  void AttemptToReadProcessing( void );

  /** Called by the state machine to send a break */
  void AttemptToSendBreakProcessing( void );

  /** Called by the state machine to purge the buffers */
  void AttemptToPurgeBuffersProcessing( void );

  /** Called by the state machine to purge the buffers */
  void SleepProcessing( void );

  /** Helper function to map a return value to an input */
  const InputType &MapResultToInput( int condition );
};

} // end namespace igstk

#endif // __igstkSerialCommunication_h
