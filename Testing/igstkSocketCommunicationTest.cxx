/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSocketCommunicationTest.cxx
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
//  Warning about: identifier was truncated to '255' characters in the debug 
//  information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1

#include <iostream>
#include <fstream>

#include "stdio.h"
#include "time.h"

#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkMultiThreader.h"

#include "igstkSystemInformation.h"
#include "igstkSocketCommunication.h"

ITK_THREAD_RETURN_TYPE ServerThreadFunction1( void* data)
{
  typedef igstk::SocketCommunication            SocketCommunicationType;
  typedef SocketCommunicationType::Pointer      SocketCommunicationPointerType;
  typedef SocketCommunicationType::ResultType   ResultType;
  typedef itk::MultiThreader::ThreadInfoStruct* ThreadInfoStructPointerType;
  typedef igstk::Object::LoggerType             LoggerType;
  typedef itk::StdStreamLogOutput               LogOutputType;
  

  LoggerType::Pointer                       logger = LoggerType::New();
  LogOutputType::Pointer                    logOutput = LogOutputType::New();  

  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  ThreadInfoStructPointerType info = (ThreadInfoStructPointerType)data;
  bool* flag = (bool*)(info->UserData);

  SocketCommunicationPointerType server = SocketCommunicationType::New();

  server->SetLogger( logger );
  server->SetCaptureFileName("server1.log");
  server->SetCapture( true );

  unsigned int num;
  const char * servermessage = "response message from server!";
  const char * clientmessage = "request message from client!";
  char message[128];
  char buffer[1024];
  ResultType result;
  
  if (!server->RequestOpenCommunication())
    {
    printf("%s","Open communication error!\n");
    *flag = false;
    return ITK_THREAD_RETURN_VALUE;
    }
  if (!server->RequestOpenPort( 1234))
    {
    printf("%s","Open port error!\n");
    *flag = false;
    return ITK_THREAD_RETURN_VALUE;
    }

  int i = 0;
  bool stop = false;
  while ( !stop)
    {
    result = server->RequestQueryForConnection( 1000 );
    
    if (result == SocketCommunicationType::SUCCESS)
      {
      do
        {
        result = server->RequestRead( buffer, 100, num, 10);
        switch (result)
          {
          case SocketCommunicationType::SUCCESS:
            printf("%s", "\n");
            buffer[num] = '\0';
            printf("%s", buffer);
            printf("%s", "\n");

            if (strcmp(buffer, "STOPSERVER") == 0)
              {
              stop = true;
              }
            else if (strncmp(buffer, clientmessage, strlen(clientmessage)) == 0)
              {
              sprintf(message, "%s[%d]\n", servermessage, i);
              server->RequestWrite( message );
              i++;
              }
            else
              {
              printf("%s","Data transfer error!\n");
              *flag = false;
              return ITK_THREAD_RETURN_VALUE;
              }
            break;
          case SocketCommunicationType::TIMEOUT:
            break;
          case SocketCommunicationType::FAILURE:
            printf("%s","Read error!\n");
            *flag = false;
            return ITK_THREAD_RETURN_VALUE;
            break;
          }
        }
      while (!stop);

      server->Print( std::cout);

      server->RequestDisconnectConnectionSocket();
      }
    }

  server->RequestCloseCommunication();

  // Test for NoProcessing
  server->RequestCloseCommunication();

  // Test for DangerousProcessing
  server->RequestRead( buffer, 100, num, 10);

  *flag = true;
  return ITK_THREAD_RETURN_VALUE;

}

ITK_THREAD_RETURN_TYPE ServerThreadFunction2( void* data)
{
  typedef igstk::SocketCommunication            SocketCommunicationType;
  typedef SocketCommunicationType::Pointer      SocketCommunicationPointerType;
  typedef SocketCommunicationType::ResultType   ResultType;
  typedef itk::MultiThreader::ThreadInfoStruct* ThreadInfoStructPointerType;
  typedef igstk::Object::LoggerType             LoggerType;
  typedef itk::StdStreamLogOutput               LogOutputType;
  

  LoggerType::Pointer                       logger = LoggerType::New();
  LogOutputType::Pointer                    logOutput = LogOutputType::New();  

  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  ThreadInfoStructPointerType info = (ThreadInfoStructPointerType)data;
  bool* flag = (bool*)(info->UserData);

  SocketCommunicationPointerType server = SocketCommunicationType::New();

  server->SetLogger( logger );
  server->SetCaptureFileName("server2.log");
  server->SetCapture( true );

  unsigned int num;
  const char * servermessage = "response message from server!";
  const char * clientmessage = "request message from client!";
  char message[128];
  char buffer[1024];
  ResultType result;
  
  if (!server->RequestOpenCommunication())
    {
    printf("%s","Open communication error!\n");
    *flag = false;
    return ITK_THREAD_RETURN_VALUE;
    }
  if (!server->RequestOpenPort( 1234))
    {
    printf("%s","Open port error!\n");
    *flag = false;
    return ITK_THREAD_RETURN_VALUE;
    }

  int i = 0;
  bool stop = false;

  result = server->RequestWaitForConnection();
    
  while ( !stop )
    {
    result = server->RequestRead( buffer, 100, num);
    switch (result)
      {
      case SocketCommunicationType::SUCCESS:
        printf("%s", "\n");
        buffer[num] = '\0';
        printf("%s", buffer);
        printf("%s", "\n");

        if (strncmp(buffer, "STOPSERVER", strlen("STOPSERVER")) == 0)
          {
          stop = true;
          }
        else if (strncmp(buffer, clientmessage, strlen(clientmessage)) == 0)
          {
          sprintf(message, "%s[%d]\n", servermessage, i);
          server->RequestWrite( message, 100 );
          i++;
          }
        else
          {
          printf("%s","Data transfer error!\n");
          *flag = false;
          return ITK_THREAD_RETURN_VALUE;
          }
        break;
      case SocketCommunicationType::TIMEOUT:
        break;
      case SocketCommunicationType::FAILURE:
        printf("%s","Read error!\n");
        *flag = false;
        return ITK_THREAD_RETURN_VALUE;
        break;
      }

    server->Print( std::cout);
    }

  server->RequestDisconnectConnectionSocket();

  server->RequestCloseCommunication();

  *flag = true;
  return ITK_THREAD_RETURN_VALUE;

}

ITK_THREAD_RETURN_TYPE ClientThreadFunction1( void* data)
{
  typedef igstk::SocketCommunication            SocketCommunicationType;
  typedef SocketCommunicationType::Pointer      SocketCommunicationPointerType;
  typedef SocketCommunicationType::ResultType   ResultType;
  typedef itk::MultiThreader::ThreadInfoStruct* ThreadInfoStructPointerType;
  typedef igstk::Object::LoggerType             LoggerType;
  typedef itk::StdStreamLogOutput               LogOutputType;

  LoggerType::Pointer                       logger = LoggerType::New();
  LogOutputType::Pointer                    logOutput = LogOutputType::New();  

  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  ThreadInfoStructPointerType info = (ThreadInfoStructPointerType)data;
  bool* flag = (bool*)(info->UserData);

  SocketCommunicationPointerType client = SocketCommunicationType::New();

  client->SetLogger( logger );
  client->SetCaptureFileName("client1.log");
  client->SetCapture( true );

  unsigned int num;
  const char * servermessage = "response message from server!";
  const char * clientmessage = "request message from client!";
  const char * stopserver = "STOPSERVER";
  char message[128];
  char buffer[1024];
  ResultType result;

  if (!client->RequestOpenCommunication())
    {
    printf("%s","Open communication error!\n");
    *flag = false;
    return ITK_THREAD_RETURN_VALUE;
    }
  if (!client->RequestConnect( "127.0.0.1", 1234))
    {
    printf("%s","Connection error!\n");
    *flag = false;
    return ITK_THREAD_RETURN_VALUE;
    }

  for ( int i = 0; i < 10; i++)
    {
    sprintf(message, "%s[%d]\n", clientmessage, i);
    if (!client->RequestWrite( message ))
      { 
      printf("%s","Write error!\n");
      *flag = false;
      return ITK_THREAD_RETURN_VALUE;
      }
    
    do 
      {
      result = client->RequestRead( buffer, 100, num, 10);
      }
    while (result != SocketCommunicationType::SUCCESS);

    if (strncmp( buffer, servermessage, strlen(servermessage)) == 0)
      {
      printf("%s","\n");
      buffer[num] = '\0';
      printf("%s", buffer);
      }
    else
      {
      printf("%s","Data transfer error!\n");
      *flag = false;
      return ITK_THREAD_RETURN_VALUE;
      }
    }

  client->RequestWrite( stopserver );

  client->Print( std::cout);

  client->RequestCloseCommunication();

  *flag = true;
  return ITK_THREAD_RETURN_VALUE;

}

ITK_THREAD_RETURN_TYPE ClientThreadFunction2( void* data)
{
  typedef igstk::SocketCommunication            SocketCommunicationType;
  typedef SocketCommunicationType::Pointer      SocketCommunicationPointerType;
  typedef SocketCommunicationType::ResultType   ResultType;
  typedef itk::MultiThreader::ThreadInfoStruct* ThreadInfoStructPointerType;
  typedef igstk::Object::LoggerType             LoggerType;
  typedef itk::StdStreamLogOutput               LogOutputType;

  LoggerType::Pointer                       logger = LoggerType::New();
  LogOutputType::Pointer                    logOutput = LogOutputType::New();  

  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );

  ThreadInfoStructPointerType info = (ThreadInfoStructPointerType)data;
  bool* flag = (bool*)(info->UserData);

  SocketCommunicationPointerType client = SocketCommunicationType::New();

  client->SetLogger( logger );
  client->SetCaptureFileName("client2.log");
  client->SetCapture( true );

  unsigned int num;
  const char * servermessage = "response message from server!";
  const char * clientmessage = "request message from client!";
  const char * stopserver = "STOPSERVER";
  char message[128];
  char buffer[1024];
  ResultType result;

  if (!client->RequestOpenCommunication())
    {
    printf("%s","Open communication error!\n");
    *flag = false;
    return ITK_THREAD_RETURN_VALUE;
    }
  if (!client->RequestConnect( "localhost", 1234))
    {
    printf("%s","Connection error!\n");
    *flag = false;
    return ITK_THREAD_RETURN_VALUE;
    }

  for ( int i = 0; i < 10; i++)
    {
    sprintf(message, "%s[%d]\n", clientmessage, i);
    if (!client->RequestWrite( message, 100))
      { 
      printf("%s","Write error!\n");
      *flag = false;
      return ITK_THREAD_RETURN_VALUE;
      }
    
    result = client->RequestRead( buffer, 100, num);

    if (strncmp( buffer, servermessage, strlen(servermessage)) == 0)
      {
      printf("%s","\n");
      buffer[num] = '\0';
      printf("%s", buffer);
      }
    else
      {
      printf("%s","Data transfer error!\n");
      *flag = false;
      return ITK_THREAD_RETURN_VALUE;
      }
    }

  client->RequestWrite( stopserver );

  client->Print( std::cout);

  client->RequestCloseCommunication();

  *flag = true;
  return ITK_THREAD_RETURN_VALUE;

}

int igstkSocketCommunicationTest( int, char * [] )
{
  typedef itk::MultiThreader                MultiThreaderType;
  typedef MultiThreaderType::Pointer        MultiThreaderPointerType;

  MultiThreaderPointerType                  threader;
  bool                                      serverOK;
  bool                                      clientOK;

  threader = MultiThreaderType::New();
  threader->SetNumberOfThreads( 2);

  // Test for non-blocking mode
  serverOK = false;
  clientOK = false;  
  threader->SetMultipleMethod( 1, ServerThreadFunction1, &serverOK);
  threader->SetMultipleMethod( 0, ClientThreadFunction1, &clientOK);
  threader->MultipleMethodExecute();

  if (!(serverOK && clientOK))
    {
    printf("%s","Test failed!\n");
    return EXIT_FAILURE;
    }

  // Test for blocking mode
  serverOK = false;
  clientOK = false;
  threader->SetMultipleMethod( 1, ServerThreadFunction2, &serverOK);
  threader->SetMultipleMethod( 0, ClientThreadFunction2, &clientOK);
  threader->MultipleMethodExecute();

  if (serverOK && clientOK)
    {
    printf("%s","Test succeed!\n");
    return EXIT_SUCCESS;
    }
  else
    {
    printf("%s","Test failed!\n");
    return EXIT_FAILURE;
    }

}
