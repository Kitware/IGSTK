/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSocketCommunicationTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
//  Warning about: identifier was truncated to '255' characters in the debug 
//  information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include <fstream>

#include "stdio.h"
#include "time.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkMultiThreader.h"

#include "igstkSystemInformation.h"
#include "igstkSocketCommunication.h"

ITK_THREAD_RETURN_TYPE ServerThreadFunction( void* data)
{
  typedef igstk::SocketCommunication        SocketCommunicationType;
  typedef itk::Logger                       LoggerType; 
  typedef itk::StdStreamLogOutput           LogOutputType;

  LoggerType::Pointer                       logger = LoggerType::New();
  LogOutputType::Pointer                    logOutput = LogOutputType::New();  

  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  itk::MultiThreader::ThreadInfoStruct* info = (itk::MultiThreader::ThreadInfoStruct*)data;

  SocketCommunicationType::Pointer server = SocketCommunicationType::New();

  server->SetLogger( logger );

  unsigned int num;
  char* servermessage = "message from server!";
  char* clientmessage = "message from client!";
  char message[128];
  char buffer[1024];
  
  if (!server->RequestOpenCommunication())
    {
    printf("Open communication error!\n");
    *((bool*)info->UserData) = false;
    return ITK_THREAD_RETURN_VALUE;
    }
  if (!server->RequestOpenPort( 1234))
    {
    printf("Open port error!\n");
    *((bool*)info->UserData) = false;
    return ITK_THREAD_RETURN_VALUE;
    }

  SocketCommunicationType::ResultType result;

  static int i = 0;
  bool stop = false;
  while (i >= 0 && !stop)
    {
    result = server->RequestQueryForConnection( 1000 );
    
    if (result == SocketCommunicationType::SUCCESS)
      {
      do
        {
        server->RequestRead( buffer, 100, num);
        printf( "\n");
        buffer[num] = '\0';
        printf( buffer);
        printf( "\n");

        if (strcmp(buffer, "STOPSERVER") == 0)
          {
          stop = true;
          }
        else if (strncmp(buffer, clientmessage, strlen(clientmessage)) == 0)
          {
          sprintf(message, "%s:%d\n", servermessage, i);      
          server->RequestWrite( message, 100);
          i++;
          }
        else
          {
          printf("Data transfer error!\n");
          *((bool*)info->UserData) = false;
          return ITK_THREAD_RETURN_VALUE;
          }
        }
      while (num != 0 && !stop);
      server->RequestDisconnectConnectionSocket();
      }
    }

  server->RequestCloseCommunication();

  *((bool*)info->UserData) = true;
  return ITK_THREAD_RETURN_VALUE;

}

ITK_THREAD_RETURN_TYPE ClientThreadFunction( void* data)
{
  typedef igstk::SocketCommunication        SocketCommunicationType;
  typedef itk::Logger                       LoggerType; 
  typedef itk::StdStreamLogOutput           LogOutputType;

  LoggerType::Pointer                       logger = LoggerType::New();
  LogOutputType::Pointer                    logOutput = LogOutputType::New();  

  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  itk::MultiThreader::ThreadInfoStruct* info = (itk::MultiThreader::ThreadInfoStruct*)data;

  SocketCommunicationType::Pointer client = SocketCommunicationType::New();

  client->SetLogger( logger );

  unsigned int num;
  char* servermessage = "message from server!";
  char* clientmessage = "message from client!";
  char* stopserver = "STOPSERVER";
  char message[128];
  char buffer[1024];
  
  if (!client->RequestOpenCommunication())
    {
    printf("Open communication error!\n");
    *((bool*)info->UserData) = false;
    return ITK_THREAD_RETURN_VALUE;
    }
  if (!client->RequestConnect( "127.0.0.1", 1234))
    {
    printf("Connection error!\n");
    *((bool*)info->UserData) = false;
    return ITK_THREAD_RETURN_VALUE;
    }

  for ( int i = 0; i < 10; i++)
    {
    sprintf(message, "%s:%d\n", clientmessage, i);
    if (client->RequestWrite( message, 100) && client->RequestRead( buffer, 100, num) && strncmp( buffer, servermessage, strlen(servermessage)) == 0)
      {
      printf("\n");
      buffer[num] = '\0';
      printf( buffer);
      }
    else
      {
      printf("Data transfer error!", i);
      *((bool*)info->UserData) = false;
      return ITK_THREAD_RETURN_VALUE;
      }
    }

  client->RequestWrite( stopserver, strlen(stopserver));

  client->RequestCloseCommunication();

  *((bool*)info->UserData) = true;
  return ITK_THREAD_RETURN_VALUE;

}

int igstkSocketCommunicationTest( int, char * [] )
{
  typedef itk::MultiThreader                MultiThreaderType;
  typedef MultiThreaderType::Pointer        MultiThreaderPointerType;

  MultiThreaderPointerType                  threader;
  bool                                      serverOK = false;
  bool                                      clientOK = false;

  threader = MultiThreaderType::New();

  threader->SetNumberOfThreads( 2);
  threader->SetMultipleMethod( 0, ServerThreadFunction, &serverOK);
  threader->SetMultipleMethod( 1, ClientThreadFunction, &clientOK);

  threader->MultipleMethodExecute();

  if (serverOK && clientOK)
    {
    printf("Test succeed!\n");
    return EXIT_SUCCESS;
    }
  else
    {
    printf("Test failed!\n");
    return EXIT_FAILURE;
    }

}
