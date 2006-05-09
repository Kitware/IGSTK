/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    RobotCommunication.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkSocketCommunication.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkMultiThreader.h"


#define READ_TIMEOUT 1000          // timeouts are in msec
#define READ_BUFFER_SIZE 100
#define WRITE_TIMEOUT 30000
#define IPADDRESS "127.0.0.1"      // localhost
#define PORT 8888                  // default HWServer value

// TRIGGER
const int TRIGGER_IMMEDIATE = 0;
const int TRIGGER_IF_OK =     1;
const int TRIGGER_WHILE_OK =  2;

// INTERRUPT
const int INTERRUPT_IMMEDIATE = 1;

// JOINT NUMBER
const int JOINT_Y_ANGLE = 1;
const int JOINT_X_ANGLE = 2;
const int JOINT_Y_POSITION = 3;
const int JOINT_X_POSITION = 4;

// API_RES_FLAGs

// everything went ok (command accepted, command successfully executed)
const int APIRESFLAG_OK = 0;

// client has no rights to execute this command
const int APIRESFLAG_NO_RIGHTS = 1;

// client is already authenticated
const int APIRESFLAG_ALREADY_AUTHENTIFIED = 2; 

// syntax error in API command
const int APIRESFLAG_SYNTAX_ERROR = 3; 

// Timeout on Client Logon
const int APIRESFLAG_LOGON_TIMEOUT = 4; 

// cannot execute cmd System/axis is busy
const int APIRESFLAG_BUSY = 5; 

// wrong axis Nr
const int APIRESFLAG_ADDRESS_ERROR = 6; 

// system/axis not homed
const int APIRESFLAG_NOT_HOMED = 7; 

// unhandled error, should not occur
const int APIRESFLAG_VAR_ERROR = 8; 

// timeout in execution of the whole API command
const int APIRESFLAG_EXECUTION_TIMEOUT = 9; 

// serial port timeout BUS_ERR_TIMEOUT
const int APIRESFLAG_COMM_TIMEOUT = 10;

// bus not connected BUS_ERR_NOTCONNECTED
const int APIRESFLAG_COMM_NOT_CONNECTED = 11;

// serial bus CRC error BUS_ERR_CRC
const int APIRESFLAG_COMM_CRC_ERROR = 12; 

// error in serial communication BUS_ERR_COM
const int APIRESFLAG_COMM_ERROR = 13; 

// controller did not accept the command BUS_NACK
const int APIRESFLAG_COMM_NOT_ACCEPTED = 14; 

// position cannot be reached by the robot/axis
const int APIRESFLAG_POS_NOT_REACHABLE = 15; 

// coordinate transformation cannot be done
const int APIRESFLAG_CANNOT_TRANSFORM = 16; 

// invalid count of joint coordinates given
const int APIRESFLAG_INVALID_JOINTCOUNT = 17;

// action command overruled by a new AC
const int APIRESFLAG_CMD_OVERRULED = 18;

// parameter value out of limits (vel,acc,curr,OutState)
const int APIRESFLAG_LIMIT_EXEEDED  = 19; 

// homing is active
const int APIRESFLAG_HOMING_ACTIVE = 20; 

// flags for emergency events
// Emergency stop is/has been pressed
const int APIRESFLAG_EMERGENCY_STOP_PRESSED = 21;
// Emergency stop has been released
const int APIRESFLAG_EMERGENCY_STOP_RELEASED = 22; 
// Limit switch is/has been pressed
const int APIRESFLAG_LIMIT_SWITCH_PRESSED = 23; 
// Limit switch has been released
const int APIRESFLAG_LIMIT_SWITCH_RELEASED = 24; 

// Asynchronous mode 
const bool ASYNCMODE = false;

class RobotCommunication
{
public:

  typedef igstk::SocketCommunication            SocketCommunicationType;
  typedef SocketCommunicationType::Pointer      SocketCommunicationPointerType;
  typedef SocketCommunicationType::ResultType   ResultType;

  typedef itk::Logger                           LoggerType; 
  typedef itk::StdStreamLogOutput               LogOutputType;
  
  RobotCommunication() {
  }
       
  ~RobotCommunication() {
  }

  LoggerType::Pointer                       m_Logger;
  LogOutputType::Pointer                    m_LogOutput; 
  
  SocketCommunicationPointerType            m_Client;
  
  // Captures ResFlag after appropriate ";" in robot response
  int GetResFlag( const char * buf, int size, int numSC );                        
  
  // Captures coordinate after appropriate ";" in robot response  
  // FIXME -- DOES NOT WORK YET
  float GetCoord( const char * buf, int numSC );
  
  // opens socket, logs in to robot and turns off asynchronous mode 
  // if ASYNCMODE == false
  bool Init();

  // logs out of robot and closes socket
  bool UnInit();

  // Homes robot, must be done before any move command
  bool Home();

  // Homes robot in one axis described by JointNr
  bool HomeJoint( int JointNr );
       
  // Moves robot in X, Y (translation) and A, B (rotation) in absolute 
  // robot coordinates. 
  // Robot must be homed prior to executing this command, otherwise it 
  // will not work!
  // Z and C must equal zero for current robot configuration
  bool MoveRobotCoordinates ( float T[], float R[] );
  bool MoveRobotCoordinates ( float X, float Y, float Z, 
                              float A, float B, float C );

  // checks to see if X, Y, Z, A, B, C is reachable in robot coordinates, 
  // Z and C must equal zero, returns -1 if error
  int GetReachable ( float T[], float R[] );
  int GetReachable ( float X, float Y, float Z, float A, float B, float C );

  // interrupts all movement commands
  // this function will stop immediately if mode = 1
  // or come to a smooth stop if mode = 0
  bool Stop( int mode );
  
  // Gets current robot position and returns a pointer to an array 
  // of coordinates (X,Y,Z,A,B,C)
  // need to check - buffer function works, and return is correct
  void GetCurrentRobotPosition( float * pos );

};
