/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerToolObserverToOpenIGTLinkRelayTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include <math.h>
#include <cstdlib>
#include <cstring>

#include "itkMultiThreader.h"
#include "igstkRealTimeClock.h"
#include "igstkCircularSimulatedTracker.h"
#include "igstkSimulatedTrackerTool.h"
#include "igstkTrackerToolObserverToOpenIGTLinkRelay.h"


#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlImageMessage.h"
#include "igtlServerSocket.h"
#include "igtlStatusMessage.h"
#include "igtlPositionMessage.h"

static int ReceiveTransform(igtl::Socket::Pointer& socket, 
                                          igtl::MessageHeader::Pointer& header);
static int ReceivePosition(igtl::Socket::Pointer& socket, 
                                          igtl::MessageHeader::Pointer& header);
static int ReceiveImage(igtl::Socket::Pointer& socket, 
                                          igtl::MessageHeader::Pointer& header);
static int ReceiveStatus(igtl::Socket::Pointer& socket, 
                                          igtl::MessageHeader::Pointer& header);

static ITK_THREAD_RETURN_TYPE ReceiverThreadFunction(void* pInfoStruct);
static ITK_THREAD_RETURN_TYPE ObserverThreadFunction(void* pInfoStruct);

static char **m_Argv;
static int m_Argc;

int igstkTrackerToolObserverToOpenIGTLinkRelayTest( int argc, char * argv [] )
{

  if( argc < 6 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << 
    "hostname portnumber numberOfTransformsToSend angularSpeed(degrees/second) \
                                             trackerFrequency(Hz)" << std::endl;
    return EXIT_FAILURE;
    }
  /** itk::MultiThreader object pointer */
  itk::MultiThreader::Pointer     m_Threader;

  bool                            receiverOK;
  bool                            observerOK;

  m_Argv= argv;
  m_Argc= argc;

  igstk::RealTimeClock::Initialize();

  receiverOK = false;
  observerOK = false;  
  
  m_Threader = itk::MultiThreader::New();
  m_Threader->SetNumberOfThreads( 2);

  m_Threader->SetMultipleMethod( 1, ReceiverThreadFunction, &receiverOK);
  m_Threader->SetMultipleMethod( 0, ObserverThreadFunction, &observerOK);
  m_Threader->MultipleMethodExecute();

  return 0; 
}

/** Thread function for tracking */
ITK_THREAD_RETURN_TYPE ObserverThreadFunction(void* itkNotUsed(pInfoStruct))
{
   
  typedef igstk::CircularSimulatedTracker               TrackerType;
  typedef igstk::SimulatedTrackerTool                   TrackerToolType;
  typedef igstk::TrackerToolObserverToOpenIGTLinkRelay  ObserverType;

  TrackerType::Pointer      tracker      = TrackerType::New();
  TrackerToolType::Pointer  trackerTool  = TrackerToolType::New();
  ObserverType::Pointer     toolObserver = ObserverType::New();

  tracker->RequestOpen();

  tracker->SetRadius( 10.0 );
  tracker->SetAngularSpeed( atof( m_Argv[4] ) );
  tracker->RequestSetFrequency( atof( m_Argv[5] ) );

  trackerTool->RequestSetName("Tool_1");
  trackerTool->RequestConfigure();
  trackerTool->RequestAttachToTracker( tracker );

  toolObserver->RequestSetTrackerTool( trackerTool );
  toolObserver->RequestSetHostName( m_Argv[1] );
  toolObserver->RequestSetPort( atoi( m_Argv[2] ) );

  toolObserver->RequestStart();
  
  tracker->RequestStartTracking();

  const unsigned int numberOfTransformsToSend = atoi( m_Argv[3] );

  for( unsigned int i = 0; i < numberOfTransformsToSend; i++ )
    {
    igstk::PulseGenerator::Sleep(10);
    igstk::PulseGenerator::CheckTimeouts();
    }

  tracker->RequestStopTracking();
  tracker->RequestReset();
  tracker->RequestClose();
  
  //purely for code coverage. 
  toolObserver->Print( std::cout ); 
  
  return EXIT_SUCCESS;
}

/** Thread function for tracking */
ITK_THREAD_RETURN_TYPE ReceiverThreadFunction(void* itkNotUsed(pInfoStruct))
{
  //------------------------------------------------------------
  // Parse Arguments

  int    port     = atoi(m_Argv[2]);

  std::cerr << "Prior to server creation" << std::endl; 

  igtl::ServerSocket::Pointer serverSocket;
  serverSocket = igtl::ServerSocket::New();
  serverSocket->CreateServer(port);

  igtl::Socket::Pointer socket;

  unsigned int countReception = 0; 
  unsigned int countPostReceptionNullSocket = 0; 
  
  socket = serverSocket->WaitForConnection(10000);
  
  std::cerr << "Prior to while loop" << std::endl; 
 
  bool wantToBreak = false; 
  while (1)
    {
    //------------------------------------------------------------
    // Waiting for Connection
    
    if (socket.IsNotNull()) // if client connected
      {
      // Create a message buffer to receive header
      igtl::MessageHeader::Pointer headerMsg;
      headerMsg = igtl::MessageHeader::New();

      //------------------------------------------------------------
      // loop
      for (int i = 0; i < 100; i ++)
        {

        // Initialize receive buffer
        headerMsg->InitPack();

        // Receive generic header from the socket
        int r = socket->Receive(headerMsg->GetPackPointer(), 
                                                      headerMsg->GetPackSize());
        if (r != headerMsg->GetPackSize())
          {
          continue;
          }

        // Deserialize the header
        headerMsg->Unpack();

        // Check data type and receive data body
        if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0)
          {
          std::cerr << "Prior to ReceiveTransform" << std::endl; 
          ReceiveTransform(socket, headerMsg);
          ++countReception;
          }
        else if (strcmp(headerMsg->GetDeviceType(), "POSITION") == 0)
          {
             //std::cerr << "Prior to ReceivePosition" << std::endl; 
          ReceivePosition(socket, headerMsg);
          }
        else if (strcmp(headerMsg->GetDeviceType(), "IMAGE") == 0)
          {
             //std::cerr << "Prior to ReceiveImage" << std::endl; 
          ReceiveImage(socket, headerMsg);
          }
        else if (strcmp(headerMsg->GetDeviceType(), "STATUS") == 0)
          {
             //std::cerr << "Prior to ReceiveStatus" << std::endl; 
          ReceiveStatus(socket, headerMsg);
          }
        else
          {
          // if the data type is unknown, skip reading.
          socket->Skip(headerMsg->GetBodySizeToRead(), 0);
          }
        }
      
        if ((countReception >= 99)||((countReception > 0)&& 
                                      (++countPostReceptionNullSocket >= 10 )))
          {
          wantToBreak = true; 
          break;
          }
      }
    if (wantToBreak)
      {
      break;
      }
    }

    
  //------------------------------------------------------------
  // Close connection (The example code never reachs to this section
  // ...)

  socket->CloseSocket();
  
  return 0; 

}


int ReceiveTransform(igtl::Socket::Pointer& socket, 
                                          igtl::MessageHeader::Pointer& header)
{
  std::cerr << "Receiving TRANSFORM data type." << std::endl;
  
  // Create a message buffer to receive transform data
  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  transMsg->SetMessageHeader(header);
  transMsg->AllocatePack();
  
  // Receive transform data from the socket
  socket->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = transMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
  {
    // Retrive the transform data
    igtl::Matrix4x4 matrix;
    transMsg->GetMatrix(matrix);
    igtl::PrintMatrix(matrix);
    return 1;
  }

  return 0;

}


int ReceivePosition(igtl::Socket::Pointer& socket, 
                                          igtl::MessageHeader::Pointer& header)
{
  std::cerr << "Receiving POSITION data type." << std::endl;
  
  // Create a message buffer to receive transform data
  igtl::PositionMessage::Pointer positionMsg;
  positionMsg = igtl::PositionMessage::New();
  positionMsg->SetMessageHeader(header);
  positionMsg->AllocatePack();
  
  // Receive position position data from the socket
  socket->Receive(positionMsg->GetPackBodyPointer(), 
                                                positionMsg->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = positionMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    // Retrive the transform data
    float position[3];
    float quaternion[4];

    positionMsg->GetPosition(position);
    positionMsg->GetQuaternion(quaternion);

    std::cerr << "position   = (" << position[0] << ", " << position[1] 
                                     << ", " << position[2] << ")" << std::endl;
    std::cerr << "quaternion = (" << quaternion[0] << ", " << quaternion[1] 
          << ", " << quaternion[2] << ", " << quaternion[3] << ")" << std::endl;

    return 1;
    }

  return 0;

}


int ReceiveImage(igtl::Socket::Pointer& socket, 
                                           igtl::MessageHeader::Pointer& header)
{
  std::cerr << "Receiving IMAGE data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::ImageMessage::Pointer imgMsg;
  imgMsg = igtl::ImageMessage::New();
  imgMsg->SetMessageHeader(header);
  imgMsg->AllocatePack();
  
  // Receive transform data from the socket
  socket->Receive(imgMsg->GetPackBodyPointer(), imgMsg->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = imgMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    // Retrive the image data
    int   size[3];          // image dimension
    float spacing[3];       // spacing (mm/pixel)
    int   svsize[3];        // sub-volume size
    int   svoffset[3];      // sub-volume offset
    int   scalarType;       // scalar type

    scalarType = imgMsg->GetScalarType();
    imgMsg->GetDimensions(size);
    imgMsg->GetSpacing(spacing);
    imgMsg->GetSubVolume(svsize, svoffset);

    std::cerr << "Device Name           : " << imgMsg->GetDeviceName() << "\n";
    std::cerr << "Scalar Type           : " << scalarType << std::endl;
    std::cerr << "Dimensions            : ("
              << size[0] << ", " << size[1] << ", " << size[2] << ")" << "\n";
    std::cerr << "Spacing               : ("
              << spacing[0] << ", " << spacing[1] << ", " << spacing[2] << ")" 
              << std::endl;
    std::cerr << "Sub-Volume dimensions : ("
              << svsize[0] << ", " << svsize[1] << ", " << svsize[2] << ")" 
              << std::endl;
    std::cerr << "Sub-Volume offset     : ("
              << svoffset[0] << ", " << svoffset[1] << ", " << svoffset[2] 
              << ")" << std::endl;
    return 1;
    }

  return 0;

}


int ReceiveStatus(igtl::Socket::Pointer& socket, 
                                           igtl::MessageHeader::Pointer& header)
{

  std::cerr << "Receiving STATUS data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::StatusMessage::Pointer statusMsg;
  statusMsg = igtl::StatusMessage::New();
  statusMsg->SetMessageHeader(header);
  statusMsg->AllocatePack();
  
  // Receive transform data from the socket
  socket->Receive(statusMsg->GetPackBodyPointer(),statusMsg->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = statusMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    std::cerr << "========== STATUS ==========" << std::endl;
    std::cerr << " Code      : " << statusMsg->GetCode() << std::endl;
    std::cerr << " SubCode   : " << statusMsg->GetSubCode() << std::endl;
    std::cerr << " Error Name: " << statusMsg->GetErrorName() << std::endl;
    std::cerr << " Status    : " << statusMsg->GetStatusString() << std::endl;
    std::cerr << "============================" << std::endl;
    }

  return 0;

}
