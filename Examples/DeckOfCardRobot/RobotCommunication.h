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
#ifndef __RobotCommunication_h
#define __RobotCommunication_h

#include "igstkSocketCommunication.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkMultiThreader.h"

namespace igstk
{

#define READ_TIMEOUT 1000          // timeouts are in msec
#define READ_BUFFER_SIZE 100
#define WRITE_TIMEOUT 30000
#define IPADDRESS "127.0.0.1"      // localhost
#define PORT 8888                  // default HWServer value

// TRIGGER
const unsigned int TRIGGER_IMMEDIATE = 0;
const unsigned int TRIGGER_IF_OK =     1;
const unsigned int TRIGGER_WHILE_OK =  2;

// INTERRUPT
const unsigned int INTERRUPT_IMMEDIATE = 1;

// JOINT NUMBER
const unsigned int JOINT_Y_ANGLE = 1;
const unsigned int JOINT_X_ANGLE = 2;
const unsigned int JOINT_Y_POSITION = 3;
const unsigned int JOINT_X_POSITION = 4;

// API_RES_FLAGs

// everything went ok (command accepted, command successfully executed)
const unsigned int APIRESFLAG_OK = 0;

// client has no rights to execute this command
const unsigned int APIRESFLAG_NO_RIGHTS = 1;

// client is already authenticated
const unsigned int APIRESFLAG_ALREADY_AUTHENTIFIED = 2; 

// syntax error in API command
const unsigned int APIRESFLAG_SYNTAX_ERROR = 3; 

// Timeout on Client Logon
const unsigned int APIRESFLAG_LOGON_TIMEOUT = 4; 

// cannot execute cmd System/axis is busy
const unsigned int APIRESFLAG_BUSY = 5; 

// wrong axis Nr
const unsigned int APIRESFLAG_ADDRESS_ERROR = 6; 

// system/axis not homed
const unsigned int APIRESFLAG_NOT_HOMED = 7; 

// unhandled error, should not occur
const unsigned int APIRESFLAG_VAR_ERROR = 8; 

// timeout in execution of the whole API command
const unsigned int APIRESFLAG_EXECUTION_TIMEOUT = 9; 

// serial port timeout BUS_ERR_TIMEOUT
const unsigned int APIRESFLAG_COMM_TIMEOUT = 10;

// bus not connected BUS_ERR_NOTCONNECTED
const unsigned int APIRESFLAG_COMM_NOT_CONNECTED = 11;

// serial bus CRC error BUS_ERR_CRC
const unsigned int APIRESFLAG_COMM_CRC_ERROR = 12; 

// error in serial communication BUS_ERR_COM
const unsigned int APIRESFLAG_COMM_ERROR = 13; 

// controller did not accept the command BUS_NACK
const unsigned int APIRESFLAG_COMM_NOT_ACCEPTED = 14; 

// position cannot be reached by the robot/axis
const unsigned int APIRESFLAG_POS_NOT_REACHABLE = 15; 

// coordinate transformation cannot be done
const unsigned int APIRESFLAG_CANNOT_TRANSFORM = 16; 

// invalid count of joint coordinates given
const unsigned int APIRESFLAG_INVALID_JOINTCOUNT = 17;

// action command overruled by a new AC
const unsigned int APIRESFLAG_CMD_OVERRULED = 18;

// parameter value out of limits (vel,acc,curr,OutState)
const unsigned int APIRESFLAG_LIMIT_EXEEDED  = 19; 

// homing is active
const unsigned int APIRESFLAG_HOMING_ACTIVE = 20; 

// flags for emergency events
// Emergency stop is/has been pressed
const unsigned int APIRESFLAG_EMERGENCY_STOP_PRESSED = 21;
// Emergency stop has been released
const unsigned int APIRESFLAG_EMERGENCY_STOP_RELEASED = 22; 
// Limit switch is/has been pressed
const unsigned int APIRESFLAG_LIMIT_SWITCH_PRESSED = 23; 
// Limit switch has been released
const unsigned int APIRESFLAG_LIMIT_SWITCH_RELEASED = 24; 

// Asynchronous mode 
const bool ASYNCMODE = false;


/** \brief Class that encapsulates the communication with the robot.
 *
 *  \class RobotCommunication This class manages the flow of information
 *  between the robot and the application.
 *
 */
class RobotCommunication
{
public:

  typedef igstk::SocketCommunication            SocketCommunicationType;
  typedef SocketCommunicationType::Pointer      SocketCommunicationPointerType;
  typedef SocketCommunicationType::ResultType   ResultType;

  typedef itk::Logger                           LoggerType; 
  typedef itk::StdStreamLogOutput               LogOutputType;
  
  RobotCommunication() {}
       
  ~RobotCommunication() {}

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

} // end namespace igstk

#endif
