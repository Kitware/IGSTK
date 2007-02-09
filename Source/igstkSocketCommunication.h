/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSocketCommunication.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkSocketCommunication_h
#define __igstkSocketCommunication_h

// Disabling warning C4355: 'this' : used in base member initializer list
#if defined(_MSC_VER)
#pragma warning ( disable : 4355 )
#endif

#include "itkObject.h"
#include "itkEventObject.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "igstkMacros.h"
#include "igstkEvents.h"
#include "igstkCommunication.h"
#include "igstkStateMachine.h"

#include <string>

#if defined(_WIN32) && !defined(__CYGWIN__)
#include "winsock2.h"
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#define IGSTK_SOCKET_TYPE SOCKET
#define IGSTK_INVALID_SOCKET INVALID_SOCKET
#else
#define IGSTK_SOCKET_TYPE int
#define IGSTK_INVALID_SOCKET -1
#endif

/** \class SocketCommunication
 * 
 * \brief This class implements communication over a TCP/IP protocal
 *        via socket.
 *
 * This class provides a common interface for the interactions with a TCP/IP
 * port independently of the platform. It can work as a server or client
 * socket interface;
 *
 * \ingroup Communication
 */

namespace igstk
{

class SocketCommunication : public Communication
{
public:

  /** General declaration for IGSTK. */
  igstkStandardClassTraitsMacro( SocketCommunication, Communication )

  /** Type for TCP/IP address. */
  typedef std::string                    AddressType;
  
  /** Type for port number. */
  typedef int                            PortNumberType;

  /** Type for return result. */
  typedef Communication::ResultType      ResultType;

  /** Type for the socket itself */
  typedef IGSTK_SOCKET_TYPE              SocketType;

  /** Type for socket usage. */
  typedef enum
    {
    NONE_SOCKET = 0,
    SERVER_SOCKET,
    CLIENT_SOCKET
    }
  SocketUsageType;
  
public:

  /** Function to get the TCP/IP address. */
  igstkGetMacro( Address, AddressType );

  /** Function to get the port number. */
  igstkGetMacro( PortNumber, PortNumberType );  

  /** Function to get the socket. */
  igstkGetMacro( Socket, SocketType );

  /** Function to get the connection socket in server mode. */
  igstkGetMacro( ConnectionSocket, SocketType );

  /** Function to get the socket type. */
  igstkGetMacro( SocketUsageType, SocketUsageType );

  /** Set the name of the file into which the data stream is recorded. */
  void SetCaptureFileName(const char* filename);

  /** Get the filename into which the data stream is recorded. */
  const char* GetCaptureFileName() const;

  /** Set whether to record the data. */
  igstkSetMacro( Capture, bool );

  /** Get whether the data is being recorded. */
  igstkGetMacro( Capture, bool );

  /** Function to open the communication. */
  /** Server and client. */
  ResultType RequestOpenCommunication();

  /** Function to close the communication. */
  /** Server and client. */
  ResultType RequestCloseCommunication();

  /** Function to open the port for connection. */
  /** Server only. */
  ResultType RequestOpenPort( PortNumberType port );

  /** Blocking function to wait for client's connection. */
  /** Server only. */
  ResultType RequestWaitForConnection();

  /** Non-blocking function to wait for client's connection. */
  /** Server only. */
  ResultType RequestQueryForConnection( unsigned int msec );

  /** Function to disconnect the connection socket with client. */
  /** Server only. */
  ResultType RequestDisconnectConnectionSocket();

  /** Function to connect to server. */
  /** Client only. */
  ResultType RequestConnect( AddressType address, PortNumberType port );

  /** Function to write data. */
  /** Server and client. */
  ResultType RequestWrite( const char* message, unsigned int numberOfBytes );

  /** Function to write data. */
  /** Server and client. */
  ResultType RequestWrite( const char* message );

  /** Function to read data. */
  /** Server and client. */
  ResultType RequestRead( char* data, unsigned int numberOfBytes, 
                                                    unsigned int &bytesRead );

  /** Function to read data. */
  /** Server and client. */
  ResultType RequestRead( char* data, unsigned int numberOfBytes, 
                                 unsigned int &bytesRead, unsigned int msec );

protected:

  /** Constructor */
  SocketCommunication();

  /** Destructor */
  ~SocketCommunication();

  /** Printself */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** State function for no processing. */
  void NoProcessing();

  /** State function for dangerous processing. */
  void DangerousProcessing();

  /** State function for success processing. */
  void SuccessProcessing();

  /** State function for failure processing. */
  void FailureProcessing();

  /** State function to attempt to open communicaton. */
  void AttemptToOpenCommunicationProcessing();

  /** State function to attempt to oepn port. */
  void AttemptToOpenPortProcessing();

  /** State function to attempt to wait for connection. */
  void AttemptToWaitForConnectionProcessing();

  /** State function to attempt to query for connection. */
  void AttemptToQueryForConnectionProcessing();

  /** State function to attempt to connect. */
  void AttemptToConnectProcessing();

  /** State function to write. */
  void WriteProcessing(); 

  /** State function to read. */
  void WaitToReadProcessing();

  /** State function to read. */
  void QueryToReadProcessing();

  /** State function to disconnect connection socket. */
  void DisconnectConnectionSocketProcessing();  

  /** State function to close communication. */
  void CloseCommunicationProcessing();

  /** Internal function to open communication. */
  virtual ResultType InternalOpenCommunicationProcessing();

  /** Internal function to close communication. */
  virtual ResultType InternalCloseCommunicationProcessing();

  /** Internal function to open port. */
  virtual ResultType InternalOpenPortProcessing( PortNumberType port );

  /** Internal function to select socket. */
  virtual ResultType InternalSelectSocketProcessing( SocketType socket, 
                                                     unsigned int msec );

  /** Internal function to wait for connection. */
  virtual ResultType InternalWaitForConnectionProcessing();

  /** Internal function to query for connection. */
  virtual ResultType InternalQueryForConnectionProcessing( unsigned int msec );

  /** Internal function to connect. */
  virtual ResultType InternalConnectProcessing( AddressType address, 
                                                PortNumberType port );

  /** Internal function to write. */
  virtual ResultType InternalWriteProcessing( const char* data, 
                                              unsigned int numberOfBytes ); 

  /** Internal function to read. */
  virtual ResultType InternalReadProcessing( char* data,
                                             unsigned int numberOfBytes, 
                                             unsigned int& bytesRead );

  /** Internal function to read. */
  virtual ResultType InternalReadProcessing( char* data, 
     unsigned int numberOfBytes, unsigned int& bytesRead, unsigned int msec );

  /** Internal function to disconenct connection socket. */
  virtual ResultType InternalDisconnectConnectionSocketProcessing(); 

private:

  // List of States 
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( AttemptingToOpenCommunication );
  igstkDeclareStateMacro( CommunicationOpened );
  igstkDeclareStateMacro( AttemptingToServerOpenPort );
  igstkDeclareStateMacro( ServerPortOpened );
  igstkDeclareStateMacro( AttemptingToServerConnect );
  igstkDeclareStateMacro( ServerConnected );
  igstkDeclareStateMacro( AttemptingToClientConnect );
  igstkDeclareStateMacro( ClientConnected );

  // List of Inputs
  igstkDeclareInputMacro( Success );
  igstkDeclareInputMacro( Failure );
  igstkDeclareInputMacro( OpenCommunication );
  igstkDeclareInputMacro( ServerOpenPort );
  igstkDeclareInputMacro( ServerWaitForConnection );
  igstkDeclareInputMacro( ServerQueryForConnection );
  igstkDeclareInputMacro( ClientConnect );
  igstkDeclareInputMacro( WaitToRead );
  igstkDeclareInputMacro( QueryToRead );
  igstkDeclareInputMacro( Write );
  igstkDeclareInputMacro( ServerDisconnectConnectionSocket );
  igstkDeclareInputMacro( CloseCommunication );  

  // Temporary variables used for state machine
  const char*             m_BufferPointerToSend;

  int                     m_NumberToSend;

  AddressType             m_AddressToSend;

  PortNumberType          m_PortToSend;

  int                     m_MillisecondToSend;

  char*                   m_BufferPointerToReceive;  

  unsigned int            m_NumberToReceive;
  
  /** Variable for TCP/IP address. */
  AddressType             m_Address;

  /** Variable for port number. */
  PortNumberType          m_PortNumber;  

  /** Variable for socket. */
  SocketType              m_Socket;

  /** Variable for connection socket. */
  SocketType              m_ConnectionSocket;

  /** Variable for socket type. */
  SocketUsageType          m_SocketUsageType;

  /** Variable for return value. */
  ResultType              m_ReturnValue;  

  /** Record file name */
  std::string             m_CaptureFileName;

  /** File output stream for recording stream into a file */
  std::ofstream           m_CaptureFileStream;

  /** Current message number */
  unsigned int            m_CaptureMessageNumber;

  /** Recording flag */
  bool                    m_Capture;
  
  /** Logger for recording */
  itk::Logger::Pointer    m_Recorder;
  
  /** LogOutput for File output stream */
  itk::StdStreamLogOutput::Pointer  m_CaptureFileOutput;

};

} // end namespace igstk

#endif // __igstkSocketCommunication_h
