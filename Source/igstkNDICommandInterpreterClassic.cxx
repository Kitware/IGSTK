/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkNDICommandInterpreterClassic.h,v $
  Language:  C++
  Date:      $Date: 2007-10-02 15:21:00 $
  Version:   $Revision: 1.0 $

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "igstkNDICommandInterpreterClassic.h"
#include <stdio.h>
#include <string.h>

// On MSVC and Borland, snprintf is not defined but _snprintf is.
// This should probably be checked by CMake instead of here.
#if defined(__BORLANDC__) || defined(_MSC_VER)
#ifndef snprintf
#define snprintf _snprintf
#endif
#endif

namespace igstk
{

// maximum allowed size for a command to the device
const unsigned int NDI_MAX_COMMAND_SIZE = 2047;
const unsigned int NDI_MAX_REPLY_SIZE = 2047;

// timeouts for tracking and non-tracking modes
const unsigned int NDI_NORMAL_TIMEOUT = 5000;
const unsigned int NDI_TRACKING_TIMEOUT = 100;

/** Constructor: initialize internal variables. */
NDICommandInterpreterClassic
::NDICommandInterpreterClassic():m_StateMachine(this)
{
  m_Communication = 0;

  m_SerialCommand = new char[NDI_MAX_COMMAND_SIZE+1];
  m_SerialReply = new char[NDI_MAX_REPLY_SIZE+1];
  m_CommandReply = new char[NDI_MAX_REPLY_SIZE+1];
  m_SerialCommand[NDI_MAX_COMMAND_SIZE] = '\0';
  m_SerialReply[NDI_MAX_REPLY_SIZE] = '\0';
  m_CommandReply[NDI_MAX_REPLY_SIZE] = '\0';

  m_Tracking = 0;
  m_ErrorCode = 0;
}

/** Destructor: free any memory that has been allocated. */
NDICommandInterpreterClassic::~NDICommandInterpreterClassic()
{
  delete [] m_SerialCommand;
  delete [] m_SerialReply;
  delete [] m_CommandReply;
}

/** Set the communication object to use. */
void NDICommandInterpreterClassic
::SetCommunication(CommunicationType* communication)
{
  m_Communication = communication;

  /** These are the communication parameters that the NDI devices are
   * set up for when they are turned on or reset. */
  communication->SetBaudRate(SerialCommunication::BaudRate9600);
  communication->SetDataBits(SerialCommunication::DataBits8);
  communication->SetParity(SerialCommunication::NoParity);
  communication->SetStopBits(SerialCommunication::StopBits1);
  communication->SetHardwareHandshake(SerialCommunication::HandshakeOff);
  communication->UpdateParameters();

  /**  The timeouts are tricky to deal with.  The NDI devices reply
   *  almost immediately after most command, with notable exceptions:
   *  INIT, RESET, PINIT and some diagnostic commands tie up the
   *  device for quite some time.  The worst offender is PINIT,
   *  since port initialization can take up to 5 seconds.
   *
   *  A long timeout is a problem if a line error (i.e. error caused
   *  by noise in the serial cable) occurs.  Line errors occur
   *  infrequently at 57600 baud and frequenty at 115200 baud.
   *  A line error can cause a loss of the trailing carriage return
   *  on a data record, and the Read() command will of course keep
   *  waiting for this nonexistent carriage return. Five seconds is
   *  a long time to wait!  As a solution, while in tracking mode, the
   *  timeout period is reduced to just 0.1 seconds so that errors
   *  can be detected and dealt with swiftly.
   *
   *  Line errors in tracking mode are no danger, since CRC checking will
   *  automatically detect and discard bad data records.  However, we
   *  don't want the system to freeze for 5 seconds each time an error
   *  like this occurs, which is why we reduce the timeout to 0.1s in
   *  tracking mode.
   */
  communication->SetTimeoutPeriod(NDI_NORMAL_TIMEOUT);

  /** All replies from the NDI devices end in a carriage return. */
  communication->SetUseReadTerminationCharacter(1);
  communication->SetReadTerminationCharacter('\r');
}

/** Get the communication object. */
NDICommandInterpreterClassic::CommunicationType *
NDICommandInterpreterClassic::GetCommunication()
{
  return m_Communication;
}

/** Convert an ASCII hex string of length "n" to an unsigned integer. */
unsigned int
NDICommandInterpreterClassic::
HexadecimalStringToUnsignedInt(const char* cp, int n)
{
  int i;
  unsigned int result = 0;
  int c;

  for (i = 0; i < n; i++)
    {
    c = cp[i];

    // convert to lowercase if uppercase
    if (c >= 'A' && c <= 'Z')
      {
      c += ('a' - 'A');
      }

    if (c >= 'a' && c <= 'f')
      {
      result = (result << 4) | (c + (10 - 'a'));
      }
    else if (c >= '0' && c <= '9')
      {
      result = (result << 4) | (c - '0');
      }
    else
      {
      break;
      }
    }

  return result;
}

/** Convert an ASCII hex string of length "n" to an integer. */
int NDICommandInterpreterClassic::HexadecimalStringToInt(const char* cp, int n)
{
  int i;
  int result = 0;
  int c = 0;

  for (i = 0; i < n; i++)
    {
    c = cp[i];

    // convert to lowercase if uppercase
    if (c >= 'A' && c <= 'Z')
      {
      c += ('a' - 'A');
      }

    if (c >= 'a' && c <= 'f')
      {
      result = (result << 4) | (c + (10 - 'a'));
      }
    else if (c >= '0' && c <= '9')
      {
      result = (result << 4) | (c - '0');
      }
    else
      {
      break;
      }
    }

  return result;
}

/** Convert an ASCII decimal string of length "n" to an integer. */
int NDICommandInterpreterClassic::StringToInt(const char* cp, int n)
{
  int i;
  int c;
  int result = 0;

  for (i = 0; i < n; i++)
    {
    c = cp[i];
    if (c >= '0' && c <= '9')
      {
      result = (result * 10) + (c - '0');
      }
    else
      {
      break;
      }
    }

  return result;
}

/** Convert an ASCII decimal string of length "n" to an integer. */
int NDICommandInterpreterClassic::SignedStringToInt(const char* cp, int n)
{
  int i;
  int c;
  int result = 0;
  int sign = 0;

  c = cp[0];

  if (c == '+')
    {
    sign = 1;
    }
  else if (c == '-')
    {
    sign = -1;
    }

  for (i = 1; i < n; i++)
    {
    c = cp[i];
    if (c >= '0' && c <= '9')
      {
      result = (result * 10) + (c - '0');
      }
    else
      {
      break;
      }
    }

  return sign*result;
}

/** Encode binary data in to hexidecimal ASCII. */
char* NDICommandInterpreterClassic::HexEncode(char* cp, const void* data, int n)
{
  const unsigned char* bdata;
  int i, c1, c2;
  unsigned int d;
  char* tcp;

  bdata = (const unsigned char* )data;
  tcp = cp;

  for (i = 0; i < n; i++)
    {
    d = bdata[i];
    c1 = (d & 0xf0) >> 4;
    c2 = (d & 0x0f);
    c1 += '0';
    c2 += '0';
    if (c1 > '9')
      {
      c1 += ('A' - '0' - 10);
      }
    if (c2 > '9')
      {
      c2 += ('A' - '0' - 10);
      }
    *tcp++ = c1;
    *tcp++ = c2;
    }

  return cp;
}

/** Decode binary data in to hexidecimal ASCII. */
void *NDICommandInterpreterClassic::HexDecode(void *data, const char* cp, int n)
{
  unsigned char* bdata;
  int i, c1, c2;
  unsigned int d;

  bdata = (unsigned char* )data;

  for (i = 0; i < n; i++)
    {
    d = 0;
    c1 = *cp++;
    if (c1 >= 'a' && c1 <= 'f')
      {
      d = (c1 + (10 - 'a'));
      }
    else if (c1 >= 'A' && c1 <= 'F')
      {
      d = (c1 + (10 - 'A'));
      }
    else if (c1 >= '0' && c1 <= '9')
      {
      d = (c1 - '0');
      }
    c2 = *cp++;
    d <<= 4;
    if (c2 >= 'a' && c2 <= 'f')
      {
      d |= (c2 + (10 - 'a'));
      }
    else if (c2 >= 'A' && c2 <= 'F')
      {
      d |= (c2 + (10 - 'A'));
      }
    else if (c2 >= '0' && c2 <= '9')
      {
      d |= (c2 - '0');
      }

    bdata[i] = d;
    }

  return data;
}

/** Return an error code (see header file for value definitions). */
int NDICommandInterpreterClassic::GetError() const
{
  return m_ErrorCode;
}

/** Get a string that describes an error value. */
const char* NDICommandInterpreterClassic::ErrorString(int errnum)
{
  static const char* textarrayLow[] = /* values from 0x01 to 0x23 */
  {
    "No error",
    "Invalid command",
    "Command too long",
    "Command too short",
    "Invalid CRC calculated for command",
    "Time-out on command execution",
    "Unable to set up new communication parameters",
    "Incorrect number of command parameters",
    "Invalid port handle selected",
    "Invalid tracking priority selected (must be S, D or B)",
    "Invalid LED selected",
    "Invalid LED state selected (must be B, F or S)",
    "Command is invalid while in the current mode",
    "No tool assigned to the selected port handle",
    "Selected port handle not initialized",
    "Selected port handle not enabled",
    "System not initialized",
    "Unable to stop tracking",
    "Unable to start tracking",
    "Unable to initialize Tool-in-Port",
    "Invalid Position Sensor or Field Generator characterization parameters",
    "Unable to initialize the Measurement System",
    "Unable to start diagnostic mode",
    "Unable to stop diagnostic mode",
    "Unable to determine environmental infrared or magnetic interference",
    "Unable to read device's firmware version information",
    "Internal Measurement System error",
    "Unable to initialize for environmental infrared diagnostics",
    "Unable to set marker firing signature",
    "Unable to search for SROM IDs",
    "Unable to read SROM data",
    "Unable to write SROM data",
    "Unable to select SROM",
    "Unable to perform tool current test",
    "Unable to find camera parameters from the selected volume for the"
    " wavelength of a tool enabled for tracking",
    "Command parameter out of range",
    "Unable to select parameters by volume",
    "Unable to determine Measurement System supported features list",
    "Reserved - Unrecognized Error 0x26",
    "Reserved - Unrecognized Error 0x27",
    "SCU hardware has changed state; a card has been removed or added",
    "Main processor firmware corrupt",
    "No memory available for dynamic allocation (heap is full)",
    "Requested handle has not been allocated",
    "Requested handle has become unoccupied",
    "All handles have been allocated",
    "Invalid port description",
    "Requested port already assigned to a port handle",
    "Invalid input or output state",
    "No camera parameters for this wavelength",
    "Command parameters out of range"
    "No camera parameters for this volume",
    "Failure to determine supported features",
    "Unrecognized error code",
    "Unrecognized error code",
    "SCU has changed state",
    "Main processor firmware corrupt",
    "Memory is full",
    "Requested handle has not been allocated",
    "Requested handle has become unoccupied",
    "All handles have been allocated",
    "Incompatible firmware versions",
    "Invalid port description",
    "Requested port is already assigned",
    "Invalid input or output state",
    "Invalid operation for tool",
    "Feature not available",
  };

  static const char* textarrayHigh[] = /* values from 0xf6 to 0xf4 */
  {
    "Too much environmental infrared",
    "Unrecognized error code",
    "Unrecognized error code",
    "Unable to read Flash EPROM",
    "Unable to write Flash EPROM",
    "Unable to erase Flash EPROM"
  };

  static const char* textarrayApi[] = /* values specific to the API */
  {
    "Bad CRC on reply from Measurement System",
    "Error opening serial connection",
    "Host not capable of given communications parameters",
    "Device->host communication timeout",
    "Serial port write error",
    "Serial port read error",
    "Measurement System failed to reset on break",
    "Measurement System not found on specified port"
  };

  if (errnum >= 0x00 && errnum <= 0x33)
    {
    return textarrayLow[errnum];
    }
  else if (errnum >= 0xf1 && errnum <= 0xf6)
    {
    return textarrayHigh[errnum-0xf1];
    }
  else if (errnum >= 0x100 && errnum <= 0x700)
    {
    return textarrayApi[(errnum >> 8)-1];
    }

  return "Unrecognized error code";
}

/*---------------------------------------------------------------------*/
/* the CalcCRC16 function is taken from the NDI ndicapi documentation  */
/*****************************************************************
Name:                   CalcCRC16

Input Values:
    int
        data        :Data value to add to running CRC16.
    unsigned int
        *puCRC16    :Ptr. to running CRC16.

Output Values:
    None.

Returned Value:
    None.

Description:
    This routine calculates a running CRC16 using the polynomial
    X^16 + X^15 + X^2 + 1.

*****************************************************************/
inline void ndiCalcCRC16(int nextchar, unsigned int *puCRC16)
{
  static const int oddparity[16] =    { 0, 1, 1, 0, 1, 0, 0, 1,
                                        1, 0, 0, 1, 0, 1, 1, 0 };
  int data;
  data = nextchar;
  data = (data ^ (*(puCRC16) & 0xff)) & 0xff;
  *puCRC16 >>= 8;
  if (oddparity[data & 0x0f] ^ oddparity[data >> 4])
    {
    *(puCRC16) ^= 0xc001;
    }
  data <<= 6;
  *puCRC16 ^= data;
  data <<= 1;
  *puCRC16 ^= data;
}

/** Write a serial break to the device */
int NDICommandInterpreterClassic::WriteSerialBreak()
{
  /* serial break will force tracking to stop */
  m_Communication->SetBaudRate(SerialCommunication::BaudRate9600);
  m_Communication->SetDataBits(SerialCommunication::DataBits8);
  m_Communication->SetParity(SerialCommunication::NoParity);
  m_Communication->SetStopBits(SerialCommunication::StopBits1);
  m_Communication->SetHardwareHandshake(SerialCommunication::HandshakeOff);

  int result = NDI_BAD_COMM;

  if (m_Communication->UpdateParameters() == Communication::SUCCESS)
    {
    m_Communication->Sleep(500);

    result = NDI_WRITE_ERROR;
    /* send break, reset the comm parameters */
    if (m_Communication->SendBreak() == Communication::SUCCESS)
      {
      /* sleep time plus timeout time must be > 10 seconds, or
       * a timeout error might occur while waiting for reset */
      int sleepTime = 11000 - NDI_NORMAL_TIMEOUT;
      if (sleepTime > 0)
        {
        m_Communication->Sleep(sleepTime);
        }

      result = 0;
      }
    }

  return this->SetErrorCode(result);
}

/** Add a CRC value to a command and write it, and return the size of
    the command prefix in nc (the part before the ":" or space. */
int NDICommandInterpreterClassic::WriteCommand(unsigned int *nc)
{
  unsigned int i;
  unsigned int n = 0;
  int useCRC = 0;
  int inCommand = 1;
  unsigned int CRC16 = 0;
  char *cp;

  cp = m_SerialCommand;      /* the command to send */

  /* calculate CRC and command prefix size*/
  for (i = 0; cp[i] != '\0' && i < NDI_MAX_COMMAND_SIZE; i++)
    {
    ndiCalcCRC16(cp[i], &CRC16);
    if (inCommand && cp[i] == ':')
      {                                      /* only use CRC if a ':' */
      useCRC = 1;                            /*  follows the command  */
      }
    if (inCommand && !((cp[i] >= 'A' && cp[i] <= 'Z') ||
                       (cp[i] >= '0' && cp[i] <= '9')))
      {
      inCommand = 0;                          /* 'command' part has ended */
      *nc = i;                                /* command length */
      }
    }

  /* do a safety check on length, overlength strings will end up
   *  producing a CRC error because they have no CRC on the end */
  if (i < NDI_MAX_COMMAND_SIZE - 5)
    {
    if (useCRC)
      {
      snprintf(&cp[i], 5, "%04X", CRC16);           /* tack on the CRC */
      i += 4;
      }

    cp[i++] = '\r';                         /* tack on carriage return */
    cp[i] = '\0';                           /* terminate for good luck */
    }

  /* get the length */
  n = i;

  /* send the command to the device */
  int writeError = m_Communication->Write(cp, n);
  int errcode = NDI_TIMEOUT;
  if (writeError != Communication::TIMEOUT)
    {
    errcode = NDI_WRITE_ERROR;
    if (writeError == Communication::SUCCESS)
      {
      errcode = 0;
      }
    }

  return this->SetErrorCode(errcode);
}

int NDICommandInterpreterClassic::ReadAsciiReply(unsigned int offset)
{
  unsigned int i;
  unsigned int m = 0;
  char *rp, *crp;
  unsigned int CRC16 = 0;
  int readError;

  rp = m_SerialReply;        /* reply from the device */
  crp = m_CommandReply;      /* received text, with CRC hacked off */

  m_Communication->SetUseReadTerminationCharacter(1);
  readError = m_Communication->Read(&rp[offset],
                                    NDI_MAX_REPLY_SIZE - offset,
                                    m);
  m += offset;

  if (readError != Communication::SUCCESS)
    {
    int errcode = NDI_READ_ERROR;
    if (readError == Communication::TIMEOUT)
      {
      errcode = NDI_TIMEOUT;
      }
    return this->SetErrorCode(errcode);
    }

  rp[m] = '\0';   /* terminate string */

  /* back up to before the CRC and carriage return */
  if (m < 5)
    {
    return this->SetErrorCode(NDI_BAD_CRC);
    }
  m -= 5;

  /* calculate the CRC and copy serial_reply to command_reply */
  CRC16 = 0;
  for (i = 0; i < m; i++)
    {
    ndiCalcCRC16(rp[i], &CRC16);
    crp[i] = rp[i];
    }

  /* terminate command_reply before the CRC */
  crp[i] = '\0';

  /* read and check the CRC value of the reply */
  if (CRC16 != this->HexadecimalStringToUnsignedInt(&rp[m], 4))
    {
    return this->SetErrorCode(NDI_BAD_CRC);
    }

  /* check for error code */
  if (crp[0] == 'E' && strncmp(crp, "ERROR", 5) == 0)
    {
    int errcode = this->HexadecimalStringToUnsignedInt(&crp[5], 2);
    return this->SetErrorCode(errcode);
    }

  return 0;
}

/** Send a command to the device via the Communication object. */
const char* NDICommandInterpreterClassic::Command(const char* command)
{
  unsigned int i;
  unsigned int nc;
  char* cp, *rp, *crp;

  cp = m_SerialCommand;      /* text sent to device */
  rp = m_SerialReply;        /* text received from device */
  crp = m_CommandReply;      /* received text, with CRC hacked off */
  nc = 0;                    /* length of 'command' part of command */

  rp[0] = '\0';
  crp[0] = '\0';

  /* clear error */
  this->SetErrorCode(0);

  /* purge the buffer, because anything that we haven't read or
     written yet is garbage left over by a previously failed command */
  m_Communication->PurgeBuffers();

  /* if the command is NULL, send a break to reset the device */
  if (command == 0)
    {
    /* serial break will force tracking to stop */
    m_Tracking = 0;
    m_Communication->SetTimeoutPeriod(NDI_NORMAL_TIMEOUT);

    /* set m_SerialCommand to null string */
    cp[0] = '\0';
    this->WriteSerialBreak();
    }
  else
    {
    /* copy command into m_SerialCommand */
    if (cp != command)
      {
      for (i = 0; i < NDI_MAX_COMMAND_SIZE; i++)
        {
        if ((cp[i] = command[i]) == 0)
          {
          break;
          }
        }
      }

    /* change m_Tracking  if either TSTOP or INIT is sent  */
    if (cp[0] == 'T' && (strncmp(cp, "TSTOP", 5) == 0) ||
        cp[1] == 'I' && (strncmp(cp, "INIT", 4) == 0))
      {
      m_Tracking = 0;
      m_Communication->SetTimeoutPeriod(NDI_NORMAL_TIMEOUT);
      }

    /* add a CRC, write the data, and get command prefix size in "nc" */
    this->WriteCommand(&nc);
    }

  /* read the reply from the device */
  if (m_ErrorCode == 0)
    {
      this->ReadAsciiReply(0);
    }

  /* if the command was NULL, check reset reply */
  if (m_ErrorCode == 0)
    {
    if (command == 0)
      {
      if (strncmp(crp, "RESET", 5) != 0)
        {
        this->SetErrorCode(NDI_RESET_FAIL);
        }
      }
    }

  /* do any needed processing of the reply */
  if (m_ErrorCode == 0)
    {

    if (cp[0] == 'T' && nc == 6 && strncmp(cp, "TSTART", nc) == 0)
      {
      /* if TSTART, then decrease the timeout, since otherwise the
         system will freeze for 5 seconds each time there is a error */
      m_Communication->SetTimeoutPeriod(NDI_TRACKING_TIMEOUT);
      m_Tracking = 1;
      }

    /*----------------------------------------*/
    /* special behavior for specific commands */

    if (cp[0] == 'G' && cp[1] == 'X' && nc == 2)
      { /* the GX command */
      HelperForGX(cp, crp);
      }
    else if (cp[0] == '3' && cp[1] == 'D' && nc == 2)
      {
      HelperFor3D(cp, crp);
      }
    else if (cp[0] == 'C' && nc == 4 && strncmp(cp, "COMM", nc) == 0)
      {
      HelperForCOMM(cp, crp);
      }
    else if (cp[0] == 'I' && nc == 5 && strncmp(cp, "IRCHK", nc) == 0)
      {
      HelperForIRCHK(cp, crp);
      }
    else if (cp[0] == 'P' && nc == 5 && strncmp(cp, "PSTAT", nc) == 0)
      {
      HelperForPSTAT(cp, crp);
      }
    else if (cp[0] == 'S' && nc == 5 && strncmp(cp, "SSTAT", nc) == 0)
      {
      HelperForSSTAT(cp, crp);
      }
    else if (cp[0] == 'V' && nc == 3 && strncmp(cp, "VER", nc) == 0)
      {
      HelperForVER(cp, crp);
      }
    }

  /* return the device's reply, but with the CRC hacked off */
  return crp;
}

/** Use printf-style formatting to create a command. */
const char* NDICommandInterpreterClassic::Command(const char* format, int a)
{
  snprintf(m_SerialCommand, NDI_MAX_COMMAND_SIZE, format, a);
  return this->Command(m_SerialCommand);
}

/** Use printf-style formatting to create a command. */
const char* NDICommandInterpreterClassic
::Command(const char* format, int a, int b)
{
  snprintf(m_SerialCommand, NDI_MAX_COMMAND_SIZE, format, a, b);
  return this->Command(m_SerialCommand);
}

/** Use printf-style formatting to create a command. */
const char* NDICommandInterpreterClassic::Command(
                const char* format, int a, int b, int c)
{
  snprintf(m_SerialCommand, NDI_MAX_COMMAND_SIZE, format, a, b, c);
  return this->Command(m_SerialCommand);
}

/** Use printf-style formatting to create a command. */
const char* NDICommandInterpreterClassic::Command(
                       const char* format, int a, int b, int c, int d)
{
  snprintf(m_SerialCommand, NDI_MAX_COMMAND_SIZE, format, a, b, c, d);
  return this->Command(m_SerialCommand);
}

/** Use printf-style formatting to create a command. */
const char* NDICommandInterpreterClassic::Command(
                       const char* format, int a, int b, const char* c)
{
  snprintf(m_SerialCommand, NDI_MAX_COMMAND_SIZE, format, a, b, c);
  return this->Command(m_SerialCommand);
}

/** Use printf-style formatting to create a command. */
const char* NDICommandInterpreterClassic::Command(
                       const char* format, const char* a,
                                           const char* b, const char* c,
                                           const char* d, const char* e)
{
  snprintf(m_SerialCommand, NDI_MAX_COMMAND_SIZE, format, a, b, c, d, e);
  return this->Command(m_SerialCommand);
}

/** Return data that was received from a PSTAT command. */
int NDICommandInterpreterClassic::GetPSTATPortStatus(int port) const
{
  const char* dp;
  dp = &m_PSTATBasic[port][30];
  return this->HexadecimalStringToInt(dp, 2);
}

/** Return data that was received from a PSTAT command. */
int NDICommandInterpreterClassic::GetPSTATToolInfo(
                    int port, char information[32]) const
{
  const char* dp;
  int i;

  dp = m_PSTATBasic[port];

  for (i = 0; i < 32; i++)
    {
    information[i] = *dp++;
    }
  information[32] = '\0';

  return m_PSTATOccupied[port];
}

/** Return data that was received from a PSTAT command. */
int NDICommandInterpreterClassic::GetPSTAToccupied(int port) const
{
  return m_PSTATOccupied[port];
}

/** Return data that was received from a PSTAT command. */
int NDICommandInterpreterClassic::GetPSTATToolType(int port) const
{
  const char* dp;

  dp = m_PSTATBasic[port];
  return this->HexadecimalStringToInt(dp, 1);
}

/** Return data that was received from a PSTAT command. */
unsigned int NDICommandInterpreterClassic::GetPSTATCurrentTest(int port) const
{
  const char* dp;
  dp = m_PSTATTesting[port];

  return this->HexadecimalStringToInt(dp, 8);
}

/** Return data that was received from a PSTAT command. */
int NDICommandInterpreterClassic::GetPSTATPartNumber(
                      int port, char part[21]) const
{
  const char* dp;
  int i;
  dp = m_PSTATPartNumber[port];

  for (i = 0; i < 20; i++)
    {
    part[i] = *dp++;
    }
  part[20] = '\0';

  return m_PSTATOccupied[port];
}

/** Return data that was received from a PSTAT command. */
int NDICommandInterpreterClassic::GetPSTATAccessories(int port) const
{
  const char* dp;
  dp = m_PSTATAccessories[port];

  return this->HexadecimalStringToInt(dp, 2);
}

/** Return data that was received from a PSTAT command. */
int NDICommandInterpreterClassic::GetPSTATMarkerType(int port) const
{
  const char* dp;
  dp = m_PSTATMarkerType[port];

  return this->HexadecimalStringToInt(dp, 2);
}

/** Return a transform that was received after a GX command. */
int NDICommandInterpreterClassic::GetGXTransform(
                  int port, double transform[8]) const
{
  const char* dp;
  int i = port;
  int result = 0;

  if (m_GXTransforms[i][0] != '\0')
  {
    dp = m_GXTransforms[i];
    result = m_GXHandleStatus[i];
    if (result == NDI_VALID)
    {
      transform[0] = this->SignedStringToInt(&dp[0],  6)*0.0001;
      transform[1] = this->SignedStringToInt(&dp[6],  6)*0.0001;
      transform[2] = this->SignedStringToInt(&dp[12], 6)*0.0001;
      transform[3] = this->SignedStringToInt(&dp[18], 6)*0.0001;
      transform[4] = this->SignedStringToInt(&dp[24], 7)*0.01;
      transform[5] = this->SignedStringToInt(&dp[31], 7)*0.01;
      transform[6] = this->SignedStringToInt(&dp[38], 7)*0.01;
      transform[7] = this->SignedStringToInt(&dp[45], 6)*0.0001;
      }
    }
  return result;
}

/** Return port status info that was received after a GX command. */
int NDICommandInterpreterClassic::GetGXPortStatus(int port) const
{
  const char* dp;
  int i;
  int result = 0;

  if (m_GXHandleStatus[port] != NDI_DISABLED)
    {
    dp = m_GXPortStatus[port];
    result = this->HexadecimalStringToInt(dp, 2);
    }

  return result;
}

/** Return the frame number for data received from a GX command. */
unsigned int NDICommandInterpreterClassic::GetGXFrame(int port) const
{
  const char* dp;
  int i=port;
  int result = 0;

  if (m_GXHandleStatus[i] != NDI_DISABLED)
    {
    dp = m_GXFrame[i];
    result = this->HexadecimalStringToUnsignedInt(dp, 8);
    }

  return result;
}

/** Return data received from a GX command. */
int NDICommandInterpreterClassic::GetGXToolInfo(int port) const
{
  const char* dp;
  int i=port;
  int result = 0;

  if (m_GXHandleStatus[i] != NDI_DISABLED)
    {
    dp = m_GXInformation[i];
    result = this->HexadecimalStringToInt(dp, 2);
    }

  return result;
}

/** Return data received from a GX command. */
int NDICommandInterpreterClassic::GetGXMarkerInfo(int port, int marker) const
{
  const char* dp;
  int i;
  int result = 0;

  if (marker >= 0 && marker < 20)
    {
    if (m_GXHandleStatus[i] != NDI_DISABLED)
      {
      dp = &m_GXInformation[i][2+marker];
      result = this->HexadecimalStringToInt(dp, 1);
      }
    }

  return result;
}

/** Return data received from a GX command. */
int NDICommandInterpreterClassic::GetGXSingleStray(
                    int port, double coord[3]) const
{
  const char* dp;
  int i=port;
  int result = 0;

  result = NDI_DISABLED;
  if (m_GXHandleStatus[i] != NDI_DISABLED)
  {
  dp = m_GXSingleStray[i];
    result = this->HexadecimalStringToInt(dp, 2);
    if (result == NDI_VALID || result == 0x08)
    {
    coord[0] = this->SignedStringToInt(&dp[2],  7)*0.01;
      coord[1] = this->SignedStringToInt(&dp[9],  7)*0.01;
      coord[2] = this->SignedStringToInt(&dp[16], 7)*0.01;
      }
    }

  return result;
}

/** Return data received from a GX command. */
int NDICommandInterpreterClassic::GetGXSystemStatus() const
{
  const char* dp;
  int result = 0;

  dp = m_GXSystemStatus;

  if (*dp != '\0')
  {
    result = this->HexadecimalStringToInt(dp, 2);
    }

  return result;
}

/** Return data received from an SSTAT command. */
int NDICommandInterpreterClassic::GetSSTATControl() const
{
  const char* dp;
  int result = 0;

  dp = m_SSTATControl;

  if (*dp != '\0')
    {
    result = this->HexadecimalStringToInt(dp, 2);
    }

  return result;
}

/** Return data received from an SSTAT command. */
int NDICommandInterpreterClassic::GetSSTATSensors() const
{
  const char* dp;
  int result = 0;

  dp = m_SSTATSensor;

  if (*dp != '\0')
    {
    result = this->HexadecimalStringToInt(dp, 2);
    }

  return result;
}

/** Return data received from an SSTAT command. */
int NDICommandInterpreterClassic::GetSSTATTIU() const
{
  const char* dp;
  int result = 0;

  dp = m_SSTATTIU;

  if (*dp != '\0')
    {
    result = this->HexadecimalStringToInt(dp, 2);
    }

  return result;
}

/** Return data received from an VER command. */
const char *NDICommandInterpreterClassic::GetVERText() const
{
  return m_VERText;
}

/** Return data received from an IRCHK command. */
int NDICommandInterpreterClassic::GetIRCHKDetected() const
{
  int result = 0;

  if (m_IRCHKDetected == '1')
    {
    result = 1;
    }

  return result;
}

/** Return data received from an IRCHK command. */
int NDICommandInterpreterClassic::GetIRCHKNumberOfSources(int side) const
{
  const char* dp;
  int n, m;
  int result = 0;

  dp = m_IRCHKSources;

  if (*dp != '\0')
    {
    n = this->SignedStringToInt(dp, 3);
    if (n >= 0 && n <= 20)
      {
      m = this->SignedStringToInt(dp + 3 + 2*3*n, 3);
      if (m >= 0 && m <= 20)
        {
        if (side == NDI_LEFT)
          {
          result = n;
          }
        if (side == NDI_RIGHT)
          {
          result = m;
          }
        }
      }
    }

  return result;
}

/** Return data received from an IRCHK command. */
int NDICommandInterpreterClassic::GetIRCHKSourceXY(int side, int i,
                                            double xy[2]) const
{
  const char* dp;
  int n, m;
  int result = 0;

  dp = m_IRCHKSources;

  if (dp != NULL && *dp != '\0')
    {
    n = this->SignedStringToInt(dp, 3);
    if (n >= 0 && n <= 20)
      {
      m = this->SignedStringToInt(dp + 3 + 2*3*n, 3);
      if (m >= 0 && m <= 20)
        {
        if (side == NDI_LEFT && i >= 0 && i < n)
          {
          dp += 3 + 2*3*i;
          xy[0] = this->SignedStringToInt(&dp[0], 3)*0.01;
          xy[1] = this->SignedStringToInt(&dp[3], 3)*0.01;
          result = 1;
          }

        if (side == NDI_RIGHT && i >= 0 && i < m)
          {
          dp += 3 + 2*3*n + 3 + 2*3*i;
          xy[0] = this->SignedStringToInt(&dp[0], 3)*0.01;
          xy[1] = this->SignedStringToInt(&dp[3], 3)*0.01;
          result = 1;
          }
        }
      }
    }

  return result;
}

/** Copy all the PSTAT reply information into the ndicapi structure,
 *  according to the PSTAT reply mode that was requested.
 *
 *  This function is called every time a PSTAT command is sent to the
 *  Measurement System.
 *
 *  This information can be later extracted through one of the ndiGetPSTATxx()
 *  functions.
 */
void NDICommandInterpreterClassic::HelperForPSTAT(
                      const char* cp, const char* crp)
{
  unsigned int mode = NDI_BASIC; /* the default reply mode */
  char* dp;
  int j;
  int occupied = 0;

  /* if the PSTAT command had a reply mode, read it */
  if ((cp[5] == ':' && cp[10] != '\r') || (cp[5] == ' ' && cp[6] != '\r'))
    {
      mode = this->HexadecimalStringToUnsignedInt(&cp[6], 4);
    }

  /* go through the information for each handle */
  for (int i = 0; i < 3; i++)
  {
    /* check for unoccupied */
      if (*crp != 'U')
      {
        occupied = 1;

        if (mode & NDI_BASIC)
        {
          const char *defaultBasicInfo = "0000000            0000000000000";

          dp = m_PSTATBasic[i];
          /* copy in the default value first */
          for (j = 0; j < 32; j++)
            {
            dp[j] = defaultBasicInfo[j];
            }
          /* copy in the actual reply if port is occupied */
          if (occupied)
            {
            for (j = 0; j < 32 && *crp >= ' '; j++)
              {
              *dp++ = *crp++;
              }
            }
            *dp = '\0';
        }
      if (mode & NDI_TESTING)
        {
          dp = m_PSTATTesting[i];
          // default "00000000"
        for (j = 0; j < 8; j++)
          {
            dp[j] = '0';
          }
        if (occupied)
          {
            for (j = 0; j < 8 && *crp >= ' '; j++)
              {
                *dp++ = *crp++;
              }
            }
        }
        if (mode & NDI_PART_NUMBER)
        {
          dp = m_PSTATPartNumber[i];
           // default is all blank
          for (j = 0; j < 20; j++)
            {
              dp[j] = ' ';
            }
          if (occupied)
            {
              for (j = 0; j < 20 && *crp >= ' '; j++)
              {
                *dp++ = *crp++;
              }
            }
        }
        if (mode & NDI_ACCESSORIES)
        {
          dp = m_PSTATAccessories[i];
          // default "00"
          for (j = 0; j < 2; j++)
            {
              dp[j] = '0';
            }
          if (occupied)
            {
              for (j = 0; j < 2 && *crp >= ' '; j++)
              {
                *dp++ = *crp++;
              }
            }
        }
        if (mode & NDI_MARKER_TYPE)
        {
          dp = m_PSTATMarkerType[i];
          // default "00"
          for (j = 0; j < 2; j++)
            {
              dp[j] = '0';
            }
          if (occupied)
            {
              for (j = 0; j < 2 && *crp >= ' '; j++)
              {
                *dp++ = *crp++;
              }
            }
        }
      }
      else
      {
        occupied = 0;
        for (j = 0; j < 10 && *crp >= ' '; j++)
          {
            *dp++ = *crp++;
      }
      }
      /* set unoccupied flag */
      m_PSTATOccupied[i] = occupied;
      /* skip over any unsupported information */
      while (*crp >= ' ')
      {
        crp++;
      }
      /* eat the trailing newline */
      if (*crp == '\n')
      {
        crp++;
      }
  }
}

/*---------------------------------------------------------------------
  Copy all the GX reply information into the ndicapi structure, according
  to the GX reply mode that was requested.

  This function is called every time a GX command is sent to the
  Measurement System.

  This information can be later extracted through one of the ndiGetGXxx()
  functions.
*/
void NDICommandInterpreterClassic::HelperForGX(const char* cp, const char* crp)
{
  unsigned int mode = NDI_XFORMS_AND_STATUS; /* the default reply mode 0x0001*/
  char* dp;
  int i, j, n;
  int port, nhandles, nstray, status;

  /* if the GX command had a reply mode, read it */
  if ((cp[2] == ':' && cp[7] != '\r') || (cp[2] == ' ' && cp[3] != '\r'))
  {
    mode = this->HexadecimalStringToUnsignedInt(&cp[3], 4);
  }

  /* get the number of tools plugged in */
  nhandles = 3;
  m_GXNumberOfHandles = nhandles;
  /* go through the information for each handle */
  for (i = 0; i < nhandles; i++)
  {
      /* save the port handle in the list */
      port = i+1;
      m_GXHandles[i] = port;
      m_GXHandleStatus[i] = NDI_DISABLED;
      /*
       * return of GX could be
       * DISABLED or MISSING or
       * +05518-04480+07031-00197-002406-001311-127737+01688 = 51 Characters
       * example output
       * +05700+01760+07972-00918+017504-003103-127131+00898
       * -07258+01044-06795+00204-002682-005511-125025+00267
       * DISABLED
       * 000071730107    */

      /* check for "DISABLED" */
      if (*crp != 'D' || strncmp(crp, "DISABLED", 8) != 0)
      {
          if (mode & NDI_XFORMS_AND_STATUS)
          {
        /* get the transform, check for MISSING */
            dp = m_GXTransforms[i]; //ein 2D Array mit 24x52

             /* check for "MISSING" */
            if (*crp == 'M')
              {
                m_GXHandleStatus[i] = NDI_MISSING;
                for (j = 0; j < 7 && *crp >= ' '; j++)
                {
                  *dp++ = *crp++;
          }
        }
          else
          {
            m_GXHandleStatus[i] = NDI_VALID;

              /* read the transform */
              for (j = 0; j < 51 && *crp >= ' '; j++)
              {
                *dp++ = *crp++;
              }
        }
          *dp = '\0';

          /* here ends the returned output
           * when the tool is missed then we get "MISSING" else
           * we get the transformation
         * e.g. +05700+01760+07972-00918+017504-003103-127131+00898*/
      } //NDI_XFORMS_AND_STATUS

      /*grab additional information here
       * mode NDI_ADDITIONAL_INFO store here m_GXInformation[i]
       * mode NDI_SINGLE_STRAY store here GXSingleStray[i]
       */
    }//end if not disabled

      /* skip over any unsupported information */
      while (*crp >= ' ')
      {
        crp++;
      }
      /* eat the trailing newline */
      if (*crp == '\n')
      {
        crp++;
      }
  } // end for loop 3 lines read


  /*STATUS CODE e.g. 000071 73 D4 1A
  0000 0000 0000 0000 0111 0001 0111 0011 1100 0011 0001 0110
    SYSTEM PORT 3 PORT 2 PORT 1
    31  24 23  16 15   8 7    0
    SYSTEM STATUS
    0 = system communication sync error
    1 = too much external infrared detected
    2 = system comm CRC error
    3 = system processing exception
    4,5,6,7 reserved*/
  /* get the system status */
  dp = m_GXSystemStatus;
  for (j = 0; j < 2; j++)
  {
       dp[j] = '0';
  }
  for (j = 0; j < 2 && *crp >= ' '; j++)
  {
    *dp++ = *crp++;
  }

  /* go through the rest of the information for each handle */
  /*PORT STATUS
    0 TOOL IN PORT (1=OCCUPIED)
    1 SWITCH 1 (1=CLOSED)
    2 SWITCH 2 (1=CLOSED)
    3 SWITCH 3 (1=CLOSED)
    4 PORT INITIALIZED (1 = INITIALIZED)
    5 PORT ENABLED (1 = ENABLED)
    6 TOOL OUT OF VOLUME
    7 TOOL PARITY OUT OF VOLUME */
  for (i = nhandles-1; i >= 0; i--)
  {
    dp = m_GXPortStatus[i];
    for (j = 0; j < 2; j++)
    {
         dp[j] = '0';
    }
    for (j = 0; j < 2 && *crp >= ' '; j++)
    {
      *dp++ = *crp++;
    }
  }
}

void NDICommandInterpreterClassic::HelperFor3D(const char* cp, const char* crp)
{
   char m_3DTransforms[27];
   char* dp;
   dp = m_3DTransforms;

  /* read the transform */
  crp += 4;
  for(int j=0; j<27; j++)
  {
    *dp++ = *crp++;
  }

  double transform[3];
  transform[0] = this->SignedStringToInt(&m_3DTransforms[0],  9)*0.0001;
    transform[1] = this->SignedStringToInt(&m_3DTransforms[9],  9)*0.0001;
    transform[2] = this->SignedStringToInt(&m_3DTransforms[18], 9)*0.0001;
}

/*---------------------------------------------------------------------
  Copy all the SSTAT reply information into the ndicapi structure.
*/
void NDICommandInterpreterClassic::HelperForSSTAT(
                      const char* cp, const char* crp)
{
  unsigned int mode;
  char* dp;

  /* read the SSTAT command reply mode */
  mode = this->HexadecimalStringToUnsignedInt(&cp[6], 4);

  if (mode & NDI_CONTROL)
    {
    dp = m_SSTATControl;
    *dp++ = *crp++;
    *dp++ = *crp++;
    }

  if (mode & NDI_SENSORS)
    {
    dp = m_SSTATSensor;
    *dp++ = *crp++;
    *dp++ = *crp++;
    }

  if (mode & NDI_TIU)
    {
    dp = m_SSTATTIU;
    *dp++ = *crp++;
    *dp++ = *crp++;
    }
}

/*---------------------------------------------------------------------
  Copy all the VER reply information into the ndicapi structure.
*/
void NDICommandInterpreterClassic::HelperForVER(const char*, const char* crp)
{
  char* dp;
  int i;

  dp = m_VERText;

  for (i = 0; *crp != '\0' && i < 1024; i++)
    {
    *dp++ = *crp++;
    }
  *dp = '\0';
}

/*---------------------------------------------------------------------
  Copy all the IRCHK reply information into the ndicapi structure.
*/
void NDICommandInterpreterClassic::HelperForIRCHK(
                    const char* cp, const char* crp)
{
  unsigned int mode = 0x0001; /* the default reply mode */
  int j;

  m_IRCHKSources[0] = '\0';
  m_IRCHKDetected = 0;

  /* if the IRCHK command had a reply mode, read it */
  if ((cp[5] == ':' && cp[10] != '\r') || (cp[5] == ' ' && cp[6] != '\r'))
    {
    mode = this->HexadecimalStringToUnsignedInt(&cp[6], 4);
    }

  /* a single character, '0' or '1' */
  if (mode & NDI_DETECTED)
    {
    m_IRCHKDetected = *crp++;
    }

  /* maximum string length for 20 sources is 2*(3 + 20*3) = 126 */
  /* copy until a control char (less than 0x20) is found */
  if (mode & NDI_SOURCES)
    {
    for (j = 0; j < 126 && *crp >= ' '; j++)
      {
      m_IRCHKSources[j] = *crp++;
      }
    }
}

/*---------------------------------------------------------------------
  Adjust the host to match a COMM command.
*/
void NDICommandInterpreterClassic::HelperForCOMM(
                    const char* cp, const char* crp)
{
  int errcode = NDI_BAD_COMM;

  static SerialCommunication::BaudRateType convertBaud[6] = {
    SerialCommunication::BaudRate9600,
    SerialCommunication::BaudRate19200,
    SerialCommunication::BaudRate19200,
    SerialCommunication::BaudRate38400,
    SerialCommunication::BaudRate57600,
    SerialCommunication::BaudRate115200 };

  static SerialCommunication::ParityType convertParity[3] = {
    SerialCommunication::NoParity,
    SerialCommunication::OddParity,
    SerialCommunication::EvenParity };

  SerialCommunication::BaudRateType
    newspeed = SerialCommunication::BaudRate9600;

  SerialCommunication::DataBitsType
    newdata = SerialCommunication::DataBits7;

  SerialCommunication::ParityType
    newparity = SerialCommunication::NoParity;

  SerialCommunication::StopBitsType
    newstop = SerialCommunication::StopBits2;

  SerialCommunication::HandshakeType
    newhand = SerialCommunication::HandshakeOn;

  /* baud rate of 14400 is not allowed by SerialCommunication */
  if (cp[5] >= '0' && cp[5] <= '5' && cp[5] != '1' &&
      cp[6] >= '0' && cp[6] <= '1' &&
      cp[7] >= '0' && cp[7] <= '2' &&
      cp[8] >= '0' && cp[8] <= '1' &&
      cp[9] >= '0' && cp[9] <= '1')
    {
    newspeed = convertBaud[cp[5]-'0'];
    newparity = convertParity[cp[7]-'0'];

    if (cp[6] == '0')
      {
      newdata = SerialCommunication::DataBits8;
      }

    if (cp[8] == '0')
      {
      newstop = SerialCommunication::StopBits1;
      }

    if (cp[9] == '0')
      {
      newhand = SerialCommunication::HandshakeOff;
      }

    /* let the device sleep a bit */
    m_Communication->Sleep(100);

    m_Communication->SetBaudRate(newspeed);
    m_Communication->SetDataBits(newdata);
    m_Communication->SetParity(newparity);
    m_Communication->SetStopBits(newstop);
    m_Communication->SetHardwareHandshake(newhand);

    if (m_Communication->UpdateParameters() == Communication::SUCCESS)
      {
      errcode = 0;
      }
    }

  this->SetErrorCode(errcode);
}

/*---------------------------------------------------------------------
  This function is called any time the error marker is set.
*/
int NDICommandInterpreterClassic::SetErrorCode(int errnum)
{
  m_ErrorCode = errnum;

  if (errnum != 0)
    {
    this->InvokeEvent(NDIErrorEvent(errnum));
    }

  return errnum;
}

/*---------------------------------------------------------------------
  PrintSelf function.
*/
void NDICommandInterpreterClassic::PrintSelf(std::ostream& os,
                                      itk::Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Tracking: " << m_Tracking << std::endl;
  os << indent << "ErrorCode: " << m_ErrorCode << std::endl;
}


}
