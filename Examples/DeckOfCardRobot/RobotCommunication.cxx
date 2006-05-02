/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    RobotCommunication.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "RobotCommunication.h"
#include "time.h"
#include <string.h>

// Captures ResFlag after appropriate ";" in robot response
int RobotCommunication::GetResFlag( const char buf[100], int numSC ) 
{                         
  int i = 0;
  int p1, p2;
  int ResFlag;
  char * pch;
  char temp[100];

  pch=strchr(buf,';');
  while (pch!=NULL)
  {
    i++;
    if (i == numSC) p1 = pch-buf+1;
    if (i == (numSC+1)) p2 = pch-buf+1;
    pch=strchr(pch+1,';');
  }

  for (i=0;i<100;i++) temp[i]=buf[i];
  temp[p2-1]='\0';
  pch = &temp[p1];
  ResFlag = atoi(pch);

  return ResFlag;
}

// Captures coordinate after appropriate ";" in robot response  
float RobotCommunication::GetCoord( const char buf[100], int numSC ) 
{
  int i = 0;
  int p1, p2;
  float result;
  char * pch;
  char temp[100];

  pch=strchr(buf,';');
  while (pch!=NULL)
  {
    i++;
    if (i == numSC) p1 = pch-buf+1;
    if (i == (numSC+1)) p2 = pch-buf+1;
    pch=strchr(pch+1,';');
  }

  for (i=0;i<100;i++) 
  {
    temp[i]=buf[i];
  }

  temp[p2-1]='\0';
  pch = &temp[p1];
  result = atof(pch);

  return result;  
} 

// opens socket, logs in to robot and turns off asynchronous mode 
// if ASYNCMODE == false
bool RobotCommunication::Init() 
{
  char buffer[1024];
  unsigned int num;
  bool LoginSuccessful = false;
  bool AsyncCommandSuccessful = false;
  m_Client = SocketCommunicationType::New();

  m_Logger = LoggerType::New();
  m_LogOutput = LogOutputType::New();  

  m_LogOutput->SetStream( std::cout );
  m_Logger->AddLogOutput( m_LogOutput );
  m_Logger->SetPriorityLevel( itk::Logger::DEBUG );

  m_Client->SetLogger( m_Logger );
  m_Client->SetCaptureFileName("client1.log");
  m_Client->SetCapture( true );

  // initializes socket communications
  if (!m_Client->RequestOpenCommunication()) 
  { 
    printf("Open communication error!\n");
    return false;
  }

  // attempts to connect to HWServer @ localhost
  if (!m_Client->RequestConnect( IPADDRESS, PORT)) 
  {
    printf("Connection error!\n");
    return false;
  }

  m_Client->RequestWrite("@AUTH;Team;A\r\n");             // logs into robot
  m_Client->RequestRead(buffer, 100, num, READ_TIMEOUT);  // reads response into
  buffer[num]='\0';


  // checks to see if robot command was successful
  if ( GetResFlag(buffer, 2) == APIRESFLAG_OK )
  {
    LoginSuccessful = true;
  }

  // to prevent communication timeout
  Sleep(3000);

  if (!ASYNCMODE) 
  {
    // turns off asynchronous mode if ASYNCMODE is false
    m_Client->RequestWrite("@SEND_ASYNC_EVENTS;0\r\n");
    m_Client->RequestRead(buffer, 100, num, READ_TIMEOUT);
    buffer[num]='\0';
    // checks to see if command was successful
    if ( GetResFlag(buffer, 2) == APIRESFLAG_OK )
    {
      AsyncCommandSuccessful = true;
    }
    else
      AsyncCommandSuccessful = true;

    if ( LoginSuccessful && AsyncCommandSuccessful )   
    {
      return true;
    }
    else
    {
      return false;
    }
  }
}

// logs out of robot and closes socket
bool RobotCommunication::UnInit() 
{  
  // logs out of robot
  m_Client->RequestWrite("@QUIT\r\n");
  
  // closes socket communication
  if (!m_Client->RequestCloseCommunication())
  {
    return false;
  }
  else
  {
    return true;
  }
}

// Homes robot, must be done before any move command
bool RobotCommunication::Home() 
{                                                  
  unsigned int num;
  char buffer[1024];
  char sendmessage[100];

  sprintf(sendmessage, "@HOME;%d;%d\r\n", TRIGGER_IMMEDIATE, WRITE_TIMEOUT);
  m_Client->RequestWrite(sendmessage);
  m_Client->RequestRead(buffer, 100, num, READ_TIMEOUT);
  buffer[num]='\0';

  if ( GetResFlag(buffer, 2) == APIRESFLAG_OK ) 
  {
    return true;
  }
  else
  {
    return false;
  }
}

// Homes robot in one axis described by JointNr
bool RobotCommunication::HomeJoint( int JointNr ) 
{
  unsigned int num;
  char sendmessage[1024];
  char buffer[1024];

  sprintf(sendmessage, "@HOMEJ;%d;%d;%d\r\n", 
                                      TRIGGER_IMMEDIATE, WRITE_TIMEOUT, JointNr);

  m_Client->RequestWrite(sendmessage);
  m_Client->RequestRead(buffer, 100, num, READ_TIMEOUT);
  buffer[num]='\0';

  if ( GetResFlag(buffer, 2) == APIRESFLAG_OK ) 
  {
    return true;
  }
  else
  {
    return false;
  }
}

// Moves robot in X, Y (translation) and A, B (rotation) in absolute 
// robot coordinates robot must be homed prior to executing this command, 
// otherwise it will not work! Z and C must equal zero 
// for current robot configuration
bool RobotCommunication::MoveRobotCoordinates ( float X, float Y, float Z, 
                                                float A, float B, float C ) 
{
  unsigned int num;
  char buffer[1024];
  char sendmessage[1024];

  sprintf( sendmessage, "@MAW;%d;%d;%d;%f;%f;%f;%f;%f;%f\r\n", 
                           TRIGGER_IMMEDIATE, INTERRUPT_IMMEDIATE, WRITE_TIMEOUT,
                                                               X, Y, Z, A, B, C);

  m_Client->RequestWrite( sendmessage );
  m_Client->RequestRead(buffer, 100, num, READ_TIMEOUT);
  buffer[num]='\0';

  if ( GetResFlag(buffer, 2) == APIRESFLAG_OK ) 
  {
    return true;
  }
  else
  {
    return false;
  }
}

// checks to see if X, Y, Z, A, B, C is reachable in robot coordinates, 
// Z and C must equal zero, returns -1 if error
int RobotCommunication::GetReachable ( float X, float Y, float Z,
                                       float A, float B, float C ) 
{

  char buffer[1024];
  unsigned int result, num;
  char sendmessage[1024];

  sprintf(sendmessage, "@POS_WC_REACHABLE;%f;%f;%f;%f;%f;%f\r\n",
                                                               X, Y, Z, A, B, C);
  m_Client->RequestWrite( sendmessage );
  m_Client->RequestRead(buffer, 100, num, READ_TIMEOUT);
  buffer[num]='\0';

  result = GetResFlag(buffer, 2);
  if ( result == APIRESFLAG_OK ) 
  {
    return true;
  }

  if ( result == APIRESFLAG_POS_NOT_REACHABLE )
  {
    return false;
  }
  else
  {
    return -1;
  }

}
// interrupts all movement commands
// this function will stop immediately if mode = 1
// or come to a smooth stop if mode = 0
bool RobotCommunication::Stop( int mode ) 
{
  unsigned int num;
  char buffer[1024];

  m_Client->RequestWrite("@STOP;%d\r\n", mode);
  m_Client->RequestRead(buffer, 100, num, READ_TIMEOUT);
  buffer[num]='\0';

  if ( GetResFlag(buffer, 2) == APIRESFLAG_OK ) 
  {
    return true;
  }
  else 
  {
    return false;
  }
}

// Gets current robot position and returns a pointer to an array of 
// coordinates (X,Y,Z,A,B,C)
void RobotCommunication::GetCurrentRobotPosition( float * pos )  
{
  unsigned int num;
  char buffer[1024];

  m_Client->RequestWrite("@GET_POS_WC\r\n");
  m_Client->RequestRead(buffer, 100, num, READ_TIMEOUT);
  buffer[num]='\0';

  pos[0] = GetCoord(buffer, 3);
  pos[1] = GetCoord(buffer, 4);
  pos[2] = GetCoord(buffer, 5);
  pos[3] = GetCoord(buffer, 6);
  pos[4] = GetCoord(buffer, 7);
  pos[5] = GetCoord(buffer, 8);

}
