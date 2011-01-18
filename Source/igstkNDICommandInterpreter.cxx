/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkNDICommandInterpreter.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkNDICommandInterpreter.h"
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
NDICommandInterpreter::NDICommandInterpreter():m_StateMachine(this)
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
NDICommandInterpreter::~NDICommandInterpreter()
{
  delete [] m_SerialCommand;
  delete [] m_SerialReply;
  delete [] m_CommandReply;
}

/** Set the communication object to use. */
void NDICommandInterpreter::SetCommunication(CommunicationType* communication)
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
NDICommandInterpreter::CommunicationType *
NDICommandInterpreter::GetCommunication()
{
  return m_Communication;
}

/** Convert an ASCII hex string of length "n" to an unsigned integer. */
unsigned int
NDICommandInterpreter::HexadecimalStringToUnsignedInt(const char* cp, int n)
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
int NDICommandInterpreter::HexadecimalStringToInt(const char* cp, int n)
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
int NDICommandInterpreter::StringToInt(const char* cp, int n)
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
int NDICommandInterpreter::SignedStringToInt(const char* cp, int n)
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
char* NDICommandInterpreter::HexEncode(char* cp, const void* data, int n)
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

/** Encode binary data in to hexidecimal ASCII. */
void *NDICommandInterpreter::HexDecode(void *data, const char* cp, int n)
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
int NDICommandInterpreter::GetError() const
{
  return m_ErrorCode;
}

/** Get a string that describes an error value. */
const char* NDICommandInterpreter::ErrorString(int errnum)
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

**************************************************************** */

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
int NDICommandInterpreter::WriteSerialBreak()
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
int NDICommandInterpreter::WriteCommand(unsigned int *nc)
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

/* read the binary reply from a BX command */
int NDICommandInterpreter::ReadBinaryReply(unsigned int offset)
{
  unsigned int i;
  unsigned int m = 0;
  char *rp;
  char *crp;
  unsigned int CRC16 = 0;
  int readError;
  unsigned int recordLength;

  rp = m_SerialReply;        /* reply from the device */
  crp = m_CommandReply;      /* received text, with CRC hacked off */

  /* read fixed-length records, rather than checking for CR */
  m_Communication->SetUseReadTerminationCharacter(0);

  /* read the header first */
  if (offset < 6)
    {
    readError = m_Communication->Read(&rp[offset], 6-offset, m);
    offset += m;
    if (readError != Communication::SUCCESS)
      {
      int errcode = NDI_READ_ERROR;
      if (readError == Communication::TIMEOUT)
        {
        errcode = NDI_TIMEOUT;
        }
      return this->SetErrorCode(errcode);
      }
    }

  /* check the magic word to identify a BX reply */
  if(this->BinaryToUnsignedShort(rp) != 0xA5C4)
    {
    return this->ReadAsciiReply(offset);
    }

  /* check the CRC16 of the header */
  CRC16 = 0;
  for (i = 0; i < 4; i++)
    {
    ndiCalcCRC16(rp[i], &CRC16);
    crp[i] = rp[i];
    }
  crp[4] = '\0';

  if(CRC16 != this->BinaryToUnsignedShort(&rp[4]))
    {
    return this->SetErrorCode(NDI_BAD_CRC);
    }

  /* get the length of the data record */
  recordLength = this->BinaryToUnsignedShort(&crp[2]);

  /* the full reply length is recordLength + 6 (for header) + 2 * (for CRC) */
  if (offset < recordLength + 8)
    { 
    readError = m_Communication->Read(&rp[offset],
                                      recordLength + 8 - offset,
                                      m);

    if (readError != Communication::SUCCESS)
      {
      int errcode = NDI_READ_ERROR;
      if (readError == Communication::TIMEOUT)
        {
        errcode = NDI_TIMEOUT;
        }
      return this->SetErrorCode(errcode);
      }
    }

  /* copy to the reply data, after the 0xA5C4 and the length */
  CRC16 = 0;
  for (i = 0; i < recordLength; i++)
    {
    ndiCalcCRC16(rp[6 + i], &CRC16);
    crp[4 + i] = rp[6 + i];
    }
  crp[recordLength+4] = '\0';

  if(CRC16 != this->BinaryToUnsignedShort(&rp[6 + recordLength]))
    {
    return this->SetErrorCode(NDI_BAD_CRC);
    }

  return 0;
}

int NDICommandInterpreter::ReadAsciiReply(unsigned int offset)
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
const char* NDICommandInterpreter::Command(const char* command)
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
    if( ( cp[0] == 'T' && (strncmp(cp, "TSTOP", 5) == 0) ) ||
        ( cp[1] == 'I' && (strncmp(cp, "INIT",  4) == 0) )    )
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
    if (cp[0] == 'B' && cp[1] == 'X' && nc == 2)
      {
      /* the BX command needs special handling */
      this->ReadBinaryReply(0);
      }
    else
      {
      this->ReadAsciiReply(0);
      }
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

    if (cp[0] == 'B' && cp[1] == 'X' && nc == 2)
      { /* the BX command */
      HelperForBX(cp, crp);
      }
    else if (cp[0] == 'T' && cp[1] == 'X' && nc == 2)
      { /* the TX command */
      HelperForTX(cp, crp);
      }
    else if (cp[0] == 'C' && nc == 4 && strncmp(cp, "COMM", nc) == 0)
      {
      HelperForCOMM(cp, crp);
      }
    else if (cp[0] == 'I' && nc == 5 && strncmp(cp, "IRCHK", nc) == 0)
      {
      HelperForIRCHK(cp, crp);
      }
    else if (cp[0] == 'P' && nc == 5 && strncmp(cp, "PHINF", nc) == 0)
      {
      HelperForPHINF(cp, crp);
      }
    else if (cp[0] == 'P' && nc == 4 && strncmp(cp, "PHRQ", nc) == 0)
      {
      HelperForPHRQ(cp, crp);
      }
    else if (cp[0] == 'P' && nc == 4 && strncmp(cp, "PHSR", nc) == 0)
      {
      HelperForPHSR(cp, crp);
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
const char* NDICommandInterpreter::Command(const char* format, int a)
{
  snprintf(m_SerialCommand, NDI_MAX_COMMAND_SIZE, format, a);
  return this->Command(m_SerialCommand);
}

/** Use printf-style formatting to create a command. */
const char* NDICommandInterpreter::Command(const char* format, int a, int b)
{
  snprintf(m_SerialCommand, NDI_MAX_COMMAND_SIZE, format, a, b);
  return this->Command(m_SerialCommand);
}

/** Use printf-style formatting to create a command. */
const char* NDICommandInterpreter::Command(const char* format, int a, int b,
                                           int c)
{
  snprintf(m_SerialCommand, NDI_MAX_COMMAND_SIZE, format, a, b, c);
  return this->Command(m_SerialCommand);
}

/** Use printf-style formatting to create a command. */
const char* NDICommandInterpreter::Command(const char* format, int a, int b,
                                           int c, int d)
{
  snprintf(m_SerialCommand, NDI_MAX_COMMAND_SIZE, format, a, b, c, d);
  return this->Command(m_SerialCommand);
}

/** Use printf-style formatting to create a command. */
const char* NDICommandInterpreter::Command(const char* format, int a, int b,
                                           const char* c)
{
  snprintf(m_SerialCommand, NDI_MAX_COMMAND_SIZE, format, a, b, c);
  return this->Command(m_SerialCommand);
}

/** Use printf-style formatting to create a command. */
const char* NDICommandInterpreter::Command(const char* format, const char* a,
                                           const char* b, const char* c,
                                           const char* d, const char* e)
{
  snprintf(m_SerialCommand, NDI_MAX_COMMAND_SIZE, format, a, b, c, d, e);
  return this->Command(m_SerialCommand);
}

/** Return data that was received from a PHINF command. */
int NDICommandInterpreter::GetPHINFPortStatus() const
{
  const char* dp;

  dp = &m_PHINFBasic[31];

  return this->HexadecimalStringToInt(dp, 2);  
}

/** Return data that was received from a PHINF command. */
int NDICommandInterpreter::GetPHINFToolInfo(char information[32]) const
{
  const char* dp;
  int i;
  
  dp = m_PHINFBasic;

  for (i = 0; i < 31; i++)
    {
    information[i] = *dp++;
    }
  information[31] = '\0';

  return m_PHINFOccupied;
}

/** Return data that was received from a PHINF command. */
int NDICommandInterpreter::GetPHINFToolType() const
{
  const char* dp;
  
  dp = m_PHINFBasic;

  return this->HexadecimalStringToInt(dp, 2);
}

/** Return data that was received from a PHINF command. */
unsigned int NDICommandInterpreter::GetPHINFCurrentTest() const
{
  const char* dp;

  dp = m_PHINFTesting;

  return this->HexadecimalStringToInt(dp, 8);  
}

/** Return data that was received from a PHINF command. */
int NDICommandInterpreter::GetPHINFPartNumber(char part[21]) const
{
  const char* dp;
  int i;
  
  dp = m_PHINFPartNumber;

  for (i = 0; i < 20; i++)
    {
    part[i] = *dp++;
    }
  part[20] = '\0';

  return m_PHINFOccupied;
}

/** Return data that was received from a PHINF command. */
int NDICommandInterpreter::GetPHINFAccessories() const
{
  const char* dp;

  dp = m_PHINFAccessories;

  return this->HexadecimalStringToInt(dp, 2);  
}

/** Return data that was received from a PHINF command. */
int NDICommandInterpreter::GetPHINFMarkerType() const
{
  const char* dp;

  dp = m_PHINFMarkerType;

  return this->HexadecimalStringToInt(dp, 2);  
}

/** Return data that was received from a PHINF command. */
int NDICommandInterpreter::GetPHINFPortLocation(char location[15]) const
{
  const char* dp;
  int i;
  
  dp = m_PHINFPortLocation;

  for (i = 0; i < 14; i++)
    {
    location[i] = *dp++;
    }
  location[14] = '\0';

  return m_PHINFOccupied;
}

/** Return data that was received from a PHINF command. */
int NDICommandInterpreter::GetPHINFGPIOStatus() const
{
  const char* dp;

  dp = m_PHINFGPIOStatus;

  return this->HexadecimalStringToInt(dp, 2);  
}

/** Return data that was received from a PHRQ command. */
int NDICommandInterpreter::GetPHRQHandle() const
{
  const char* dp;

  dp = m_PHRQReply;
  return this->HexadecimalStringToInt(dp, 2);
}

/** Return data that was received from a PHSR command. */
int NDICommandInterpreter::GetPHSRNumberOfHandles() const
{
  const char* dp;

  dp = m_PHSRReply;

  return this->HexadecimalStringToInt(dp, 2);  
}

/** Return data that was received from a PHSR command. */
int NDICommandInterpreter::GetPHSRHandle(int i) const
{
  const char* dp;
  int n;
  int result = 0;

  dp = m_PHSRReply;
  n = this->HexadecimalStringToInt(dp, 2);
  dp += 2;

  if (i >= 0 && i < n)
    {
    dp += 5*i;
    result = this->HexadecimalStringToInt(dp, 2);
    }

  return result;
}

/** Return data that was received from a PHSR command. */
int NDICommandInterpreter::GetPHSRInformation(int i) const
{
  const char* dp;
  int n;
  int result = 0;

  dp = m_PHSRReply;
  n = this->HexadecimalStringToInt(dp, 2);
  dp += 2;

  if (i >= 0 && i < n)
    {
    dp += 5*i + 2;
    result = this->HexadecimalStringToInt(dp, 3);
    }

  return result;
}

/** Get the index \em i into the port handle array for a specific handle */
int NDICommandInterpreter::TXIndexFromPortHandle(int ph, int *ip) const
{
  int i, n;

  n = m_TXNumberOfHandles;
  for (i = 0; i < n; i++)
    {
    if (m_TXHandles[i] == ph)
      {
      break;
      }
    }
  *ip = i;
  
  return (i < n);
}

/** Return a transform that was received after a TX command. */
int NDICommandInterpreter::GetTXTransform(int ph, double transform[8]) const
{
  const char* dp;
  int i;
  int result = 0;
  
  if (this->TXIndexFromPortHandle(ph, &i) && m_TXTransforms[i][0] != '\0')
    {
    dp = m_TXTransforms[i];
    result = m_TXHandleStatus[i];
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

/** Return port status info that was received after a TX command. */
int NDICommandInterpreter::GetTXPortStatus(int ph) const
{
  const char* dp;
  int i;
  int result = 0;
  
  if (this->TXIndexFromPortHandle(ph, &i))
    {
    if (m_TXHandleStatus[i] != NDI_DISABLED)
      {
      dp = m_TXPortStatus[i];
      result = this->HexadecimalStringToInt(dp, 8);
      }
    }
 
  return result;
}

/** Return the frame number for data received from a TX command. */
unsigned int NDICommandInterpreter::GetTXFrame(int ph) const
{
  const char* dp;
  int i;
  int result = 0;

  if (this->TXIndexFromPortHandle(ph, &i))
    {
    if (m_TXHandleStatus[i] != NDI_DISABLED)
      {
      dp = m_TXFrame[i];
      result = this->HexadecimalStringToUnsignedInt(dp, 8);
      }
    }

  return result;
}

/** Return data received from a TX command. */
int NDICommandInterpreter::GetTXToolInfo(int ph) const
{
  const char* dp;
  int i;
  int result = 0;

  if (this->TXIndexFromPortHandle(ph, &i))
    {
    if (m_TXHandleStatus[i] != NDI_DISABLED)
      {
      dp = m_TXInformation[i];
      result = this->HexadecimalStringToInt(dp, 2);
      }
    }

  return result;
}

/** Return data received from a TX command. */
int NDICommandInterpreter::GetTXMarkerInfo(int ph, int marker) const
{
  const char* dp;
  int i;
  int result = 0;
  
  if (this->TXIndexFromPortHandle(ph, &i) && marker >= 0 && marker < 20)
    {
    if (m_TXHandleStatus[i] != NDI_DISABLED)
      {
      dp = &m_TXInformation[i][2+marker];
      result = this->HexadecimalStringToInt(dp, 1);
      }
    }

  return result;
}

/** Return data received from a TX command. */
int NDICommandInterpreter::GetTXSingleStray(int ph, double coord[3]) const
{
  const char* dp;
  int i;
  int result = 0;

  if (this->TXIndexFromPortHandle(ph, &i))
    {
    result = NDI_DISABLED;
    if (m_TXHandleStatus[i] != NDI_DISABLED)
      {
      dp = m_TXSingleStray[i];
      result = this->HexadecimalStringToInt(dp, 2);
      if (result == NDI_VALID || result == 0x08)
        {
        coord[0] = this->SignedStringToInt(&dp[2],  7)*0.01;
        coord[1] = this->SignedStringToInt(&dp[9],  7)*0.01;
        coord[2] = this->SignedStringToInt(&dp[16], 7)*0.01;
        }
      }
    }

  return result;
}

/** Return data received from a TX command. */
int NDICommandInterpreter::GetTXNumberOfPassiveStrays() const
{
  return m_TXNumberOfPassiveStrays; 
}

/** Return data received from a TX command. */
int NDICommandInterpreter::GetTXPassiveStray(int i, double coord[3]) const
{
  const char* dp;
  int n = 0;
  int result = 0;

  dp = m_TXPassiveStray;
  if (m_TXNumberOfPassiveStrays >= 0)
    {
    n = m_TXNumberOfPassiveStrays;
    }

  if (*dp != '\0' && i >= 0 && i < n && i < 50)
    {
    dp += 7*3*i;
    coord[0] = this->SignedStringToInt(&dp[0],  7)*0.01;
    coord[1] = this->SignedStringToInt(&dp[7],  7)*0.01;
    coord[2] = this->SignedStringToInt(&dp[14], 7)*0.01;
    result = NDI_VALID;
    }

  return result;
}
  
/** Return data received from a TX command. */
int NDICommandInterpreter::GetTXPassiveStrayOutOfVolume(int i) const
{
  int j, k;
  int result = 0;
  int val;

  if (i >= 0 && i < 50)
    {
    /* get the out-of-volume bit */
    j = i & 3; /* i % 4 */
    k = i >> 2; /* i / 4 */
    val = this->HexadecimalStringToInt(&m_TXPassiveStrayOutOfVolume[k], 1);
    result = ((val >> j) & 1);
    }

  return result;
}

/** Return data received from a TX command. */
int NDICommandInterpreter::GetTXSystemStatus() const
{
  const char* dp;
  int result = 0;

  dp = m_TXSystemStatus;

  if (*dp != '\0')
    {
    result = this->HexadecimalStringToInt(dp, 4);
    }

  return result;
}

/** Get the index \em i into the port handle array for a specific handle */
int NDICommandInterpreter::BXIndexFromPortHandle(int ph, int *ip) const
{
  int i, n;

  n = m_BXNumberOfHandles;
  for (i = 0; i < n; i++)
    {
    if (m_BXHandles[i] == ph)
      {
      break;
      }
    }
  *ip = i;
  
  return (i < n);
}

/** Return a transform that was received after a BX command. */
int NDICommandInterpreter::GetBXTransform(int ph, double transform[8]) const
{
  int i;
  int result = 0;
  
  if (this->BXIndexFromPortHandle(ph, &i))
    {
    result = m_BXHandleStatus[i];
    if (result == NDI_VALID)
      {
      transform[0] = m_BXTransforms[i][0];
      transform[1] = m_BXTransforms[i][1];
      transform[2] = m_BXTransforms[i][2];
      transform[3] = m_BXTransforms[i][3];
      transform[4] = m_BXTransforms[i][4];
      transform[5] = m_BXTransforms[i][5];
      transform[6] = m_BXTransforms[i][6];
      transform[7] = m_BXTransforms[i][7];
      }
    }

  return result;
}

/** Return port status info that was received after a BX command. */
int NDICommandInterpreter::GetBXPortStatus(int ph) const
{
  int i;
  int result = 0;
  
  if (this->BXIndexFromPortHandle(ph, &i))
    {
    if (m_BXHandleStatus[i] != NDI_DISABLED)
      {
      result = m_BXPortStatus[i];
      }
    }

  return result;
}

/** Return the frame number for data received from a BX command. */
unsigned int NDICommandInterpreter::GetBXFrame(int ph) const
{
  int i;
  int result = 0;
  
  if (this->BXIndexFromPortHandle(ph, &i))
    {
    if (m_BXHandleStatus[i] != NDI_DISABLED)
      {
      result = m_BXFrame[i];
      }
    }

  return result;
}

/** Return data received from a BX command. */
int NDICommandInterpreter::GetBXToolInfo(int ph) const
{
  int i;
  int result = 0;
  
  if (this->BXIndexFromPortHandle(ph, &i))
    {
    if (m_BXHandleStatus[i] != NDI_DISABLED)
      {
      result = m_BXToolInformation[i];
      }
    }

  return result;
}

/** Return data received from a BX command. */
int NDICommandInterpreter::GetBXMarkerInfo(int ph, int marker) const
{
  int i;  
  int result = 0;
  
  if (this->BXIndexFromPortHandle(ph, &i) && marker >= 0 && marker < 20)
    {
    if (m_BXHandleStatus[i] != NDI_DISABLED)
      {
      result = m_BXMarkerInformation[i][marker];
      }
    }

  return result;
}

/** Return data received from a BX command. */
int NDICommandInterpreter::GetBXSingleStray(int ph, double coord[3]) const
{
  int i;
  int result = 0;
  
  if (this->BXIndexFromPortHandle(ph, &i))
    {
    result = NDI_DISABLED;
    if (m_BXHandleStatus[i] != NDI_DISABLED)
      {
      result = m_BXSingleStrayStatus[i];
      if (result == NDI_VALID || result == 0x08)
        {
        coord[0] = m_BXSingleStrayPosition[i][0];
        coord[1] = m_BXSingleStrayPosition[i][1];
        coord[2] = m_BXSingleStrayPosition[i][2];
        }
      }
    }

  return result;
}

/** Return data received from a BX command. */
int NDICommandInterpreter::GetBXNumberOfPassiveStrays() const
{
  return m_BXNumberOfPassiveStrays; 
}

/** Return data received from a BX command. */
int NDICommandInterpreter::GetBXPassiveStray(int i, double coord[3]) const
{
  int n = 0;
  int result = 0;

  if (m_BXNumberOfPassiveStrays >= 0)
    {
    n = m_BXNumberOfPassiveStrays;
    }

  if (i >= 0 && i < n && i < 50)
    {
    coord[0] = m_BXPassiveStrayPosition[i][0];
    coord[1] = m_BXPassiveStrayPosition[i][1];
    coord[2] = m_BXPassiveStrayPosition[i][2];
    result = NDI_VALID;
    }

  return result;
}


/** Return data received from a BX command. */
int NDICommandInterpreter::GetBXPassiveStrayOutOfVolume(int i) const
{
  int j, k;
  int result = 0;

  if (i >= 0 && i < 50)
    {
    /* get the out-of-volume bit */
    j = i & 7; /* i % 8 */
    k = i >> 3; /* i / 8 */
    result = ((m_BXPassiveStrayOutOfVolume[k] >> j) & 1);
    }

  return result;
}

/** Return data received from a BX command. */
int NDICommandInterpreter::GetBXSystemStatus() const
{
  return m_BXSystemStatus;
}

/** Return data received from an SSTAT command. */
int NDICommandInterpreter::GetSSTATControl() const
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
int NDICommandInterpreter::GetSSTATSensors() const
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
int NDICommandInterpreter::GetSSTATTIU() const
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
const char *NDICommandInterpreter::GetVERText() const
{
  return m_VERText;
}

/** Return data received from an SSTAT command. */
int NDICommandInterpreter::GetIRCHKDetected() const
{
  int result = 0;

  if (m_IRCHKDetected == '1')
    {
    result = 1;
    }

  return result;
}

/** Return data received from an IRCHK command. */
int NDICommandInterpreter::GetIRCHKNumberOfSources(int side) const
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
int NDICommandInterpreter::GetIRCHKSourceXY(int side, int i,
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

/** Copy all the PHINF reply information into the ndicapi structure,
 *  according to the PHINF reply mode that was requested.
 *
 *  This function is called every time a PHINF command is sent to the
 *  Measurement System.
 *
 *  This information can be later extracted through one of the ndiGetPHINFxx()
 *  functions.
 */
void NDICommandInterpreter::HelperForPHINF(const char* cp, const char* crp)
{
  unsigned int mode = NDI_BASIC; /* the default reply mode */
  char* dp;
  int j;
  int occupied = 0;

  /* if the PHINF command had a reply mode, read it */
  if ((cp[5] == ':' && cp[10] != '\r') || (cp[5] == ' ' && cp[6] != '\r'))
    {
    mode = this->HexadecimalStringToUnsignedInt(&cp[8], 4);
    }

  /* check for unoccupied */
  if (*crp != 'U')
    {
    occupied = 1;
    }

  /* set unoccupied flag */
  m_PHINFOccupied = occupied;

  if (mode & NDI_BASIC)
    {
    const char *defaultBasicInfo = "00000000            0000000000000";
    
    dp = m_PHINFBasic;
    /* copy in the default value first */
    for (j = 0; j < 33; j++)
      {
      dp[j] = defaultBasicInfo[j];
      }
    /* copy in the actual reply if port is occupied */
    if (occupied)
      {
      for (j = 0; j < 33 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      }
    }
  
  if (mode & NDI_TESTING)
    {
    dp = m_PHINFTesting;
    /* default "00000000" */
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
    dp = m_PHINFPartNumber;
    /* default is all blank */
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
    dp = m_PHINFAccessories;
    /* default "00" */
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
    dp = m_PHINFMarkerType;
    /* default "00" */
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

  if (mode & NDI_PORT_LOCATION)
    {
    dp = m_PHINFPortLocation;
    /* default "00000000000000" */
    for (j = 0; j < 14; j++)
      {
      dp[j] = '0';
      }
    if (occupied)
      {
      for (j = 0; j < 14 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      }
    }
  
  if (mode & NDI_GPIO_STATUS)
    {
    dp = m_PHINFGPIOStatus;
    /* default "00" */
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

/*---------------------------------------------------------------------
  Copy all the PHRQ reply information into the ndicapi structure.

  This function is called every time a PHRQ command is sent to the
  Measurement System.

  This information can be later extracted through one of the ndiGetPHRQHandle()
  functions.
*/
void NDICommandInterpreter::HelperForPHRQ(const char* itkNotUsed(cp), 
                                                    const char* crp )
{
  char* dp;
  int j;

  dp = m_PHRQReply;
  dp[0] = '0';
  dp[1] = '0';
  for (j = 0; j < 2 && *crp >= ' '; j++)
    {
    *dp++ = *crp++;
    }
}

/*---------------------------------------------------------------------
  Copy all the PHSR reply information into the ndicapi structure.

  This function is called every time a PHSR command is sent to the
  Measurement System.

  This information can be later extracted through one of the ndiGetPHSRxx()
  functions.
*/
void NDICommandInterpreter::HelperForPHSR(const char* itkNotUsed(cp), 
                                                     const char* crp )
{
  char* dp;
  int j;

  dp = m_PHSRReply;
  for (j = 0; j < 1282 && *crp >= ' '; j++)
    {
    *dp++ = *crp++;
    }
  *dp++ = '\0';
}

/*---------------------------------------------------------------------
  Copy all the TX reply information into the ndicapi structure, according
  to the TX reply mode that was requested.

  This function is called every time a TX command is sent to the
  Measurement System.

  This information can be later extracted through one of the ndiGetTXxx()
  functions.
*/
void NDICommandInterpreter::HelperForTX(const char* cp, const char* crp)
{
  unsigned int mode = NDI_XFORMS_AND_STATUS; /* the default reply mode */
  char* dp;
  int i, j, n;
  int ph, nhandles, nstray, status;

  /* if the TX command had a reply mode, read it */
  if ((cp[2] == ':' && cp[7] != '\r') || (cp[2] == ' ' && cp[3] != '\r'))
    { 
    mode = this->HexadecimalStringToUnsignedInt(&cp[3], 4);
    }

  /* get the number of handles and skip ahead by 2 chars */
  nhandles = this->HexadecimalStringToInt(crp, 2);
  m_TXNumberOfHandles = nhandles;
  for (j = 0; j < 2 && *crp >= ' '; j++)
    {
    crp++;
    }

  /* go through the information for each handle */
  for (i = 0; i < nhandles; i++)
    {
    /* get the handle itself (two chars) */
    ph = this->HexadecimalStringToInt(crp,2);
    for (j = 0; j < 2 && *crp >= ' '; j++)
      {
      crp++;
      }

    /* save the port handle in the list */
    m_TXHandles[i] = ph;
    m_TXHandleStatus[i] = NDI_DISABLED;

    /* check for "DISABLED" */
    if (*crp != 'D' || strncmp(crp, "DISABLED", 8) != 0)
      {
      if (mode & NDI_XFORMS_AND_STATUS)
        {
        /* get the transform, check for MISSING */
        dp = m_TXTransforms[i];

       /* check for "MISSING" */
        if (*crp == 'M')
          {
          m_TXHandleStatus[i] = NDI_MISSING;
          for (j = 0; j < 7 && *crp >= ' '; j++)
            {
            *dp++ = *crp++;
            }
          }
        else
          {
          m_TXHandleStatus[i] = NDI_VALID;
          /* read the transform */
          for (j = 0; j < 51 && *crp >= ' '; j++)
            {
            *dp++ = *crp++;
            }
          }
        *dp = '\0';

        /* get the status */
        dp = m_TXPortStatus[i];
        for (j = 0; j < 8; j++)
          {
          dp[j] = '0';
          }
        for (j = 0; j < 8 && *crp >= ' '; j++)
          {
          *dp++ = *crp++;
          }

        /* get the frame number */
        dp = m_TXFrame[i];
        for (j = 0; j < 8; j++)
          {
          dp[j] = '0';
          }
        for (j = 0; j < 8 && *crp >= ' '; j++)
          {
          *dp++ = *crp++;
          }
        }

      /* grab additonal information */
      if (mode & NDI_ADDITIONAL_INFO)
        {
        dp = m_TXInformation[i];
        for (j = 0; j < 22; j++)
          {
          dp[j] = '0';
          }
        for (j = 0; j < 22 && *crp >= ' '; j++)
          {
          *dp++ = *crp++;
          }
        }

      /* grab the single marker info */ 
      if (mode & NDI_SINGLE_STRAY)
        {
        dp = m_TXSingleStray[i];
        /* read the status */
        status = this->HexadecimalStringToInt(crp, 2);
        for (j = 0; j < 2; j++)
          {
          dp[j] = '0';
          }
        for (j = 0; j < 2 && *crp >= ' '; j++)
          {
          *dp++ = *crp++;
          }
        /* read the coordinates if tool isn't missing */
        if (!(status == 0) && !(status == 2))
          {
          for (j = 0; j < 21 && *crp >= ' '; j++)
            {
            *dp++ = *crp++;
            }
          }
        *dp = '\0';
        }
      }
      
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

  /* get all the passive stray information */
  /* this will be a maximum of 3 + 13 + 50*3*7 = 1066 bytes */
  if (mode & NDI_PASSIVE_STRAY)
    {
    /* get the number of strays */
    n = this->StringToInt(crp, 2);
    for (j = 0; j < 2 && *crp >= ' '; j++)
      {
      crp++;
      }
    nstray = 50;
    if (n <= 50)
      {
      nstray = n;
      }

    /* get the number of bytes for out-of-volume bits */
    dp = m_TXPassiveStrayOutOfVolume;
    n = (nstray + 3)/4;
    for (j = 0; j < 13; j++)
      {
      dp[j] = '0';
      }
    for (j = 0; j < n && *crp >= ' '; j++)
      {
      *dp++ = *crp++;
      }

    /* if not reporting out-of-volume, reduce nstray */
    if ((mode & NDI_INCLUDE_OUT_OF_VOLUME) == 0)
      {
      for (i = 0; i < n; i++)
        {
        int bits = 
          this->HexadecimalStringToInt(&m_TXPassiveStrayOutOfVolume[i], 1);
        /* clear for proper use by GetTXPassiveStray() */
        m_TXPassiveStrayOutOfVolume[i] = '0';

        for (j = 0; j < 4; j++)
          {
          if (bits & 1)
            {
            nstray--;
            }
          bits >>= 1;
          }
        }
      }

    /* set to the reduced value */
    m_TXNumberOfPassiveStrays = nstray;

    /* get the coordinates */
    dp = m_TXPassiveStray;
    n = nstray*21;
    for (j = 0; j < n && *crp >= ' '; j++)
      {
      *dp++ = *crp++;
      }
    *dp = '\0';
    }

  /* get the system status */
  dp = m_TXSystemStatus;
  for (j = 0; j < 4 && *crp >= ' '; j++)
    {
    *dp++ = *crp++;
    }
}


/*---------------------------------------------------------------------
  Copy all the BX reply information into the ndicapi structure, according
  to the BX reply mode that was requested.

  This function is called every time a BX command is sent to the
  Measurement System.

  This information can be later extracted through one of the ndiGetBXxx()
  functions.
*/
void NDICommandInterpreter::HelperForBX(const char* cp, const char* crp)
{
  unsigned int mode = NDI_XFORMS_AND_STATUS; /* the default reply mode */
  int i, j, n;
  int ph, nhandles, nstray;

  /* if the BX command had a reply mode, read it */
  if ((cp[2] == ':' && cp[7] != '\r') || (cp[2] == ' ' && cp[3] != '\r'))
    {
    mode = this->HexadecimalStringToUnsignedInt(&cp[3], 4);
    }

  /* skip over the header (the header CRC has already been chopped) */
  crp += 4;

  /* get the number of handles */
  nhandles = this->BinaryToUnsignedChar(crp);
  crp++;
  /* save the number of handles */
  m_BXNumberOfHandles = nhandles;

  /* go through the information for each handle */
  for (i = 0; i < nhandles; i++)
    {
    /* get the handle itself */
    ph = this->BinaryToUnsignedChar(crp);
    crp++;

    /* save the port handle in the list */
    m_BXHandles[i] = ph;

    /* get the port handle status */
    m_BXHandleStatus[i] = this->BinaryToUnsignedChar(crp);
    crp++;

    /* check for DISABLED */
    if (m_BXHandleStatus[i] != NDI_DISABLED)
      {
      if (mode & NDI_XFORMS_AND_STATUS )
        {
        if (m_BXHandleStatus[i] == NDI_VALID)
          {
          /* get the transform */
          for (j = 0; j < 8; j++)
            {
            m_BXTransforms[i][j] = this->BinaryToFloat(crp);
            crp += 4;
            }
          }

        /* get the port status */
        m_BXPortStatus[i] = this->BinaryToUnsignedInt(crp);
        crp += 4;

        /* get the frame number */
        m_BXFrame[i] = this->BinaryToUnsignedInt(crp);
        crp += 4;
        }

      /* grab additonal information */
      if (mode & NDI_ADDITIONAL_INFO)
        {
        /* get the tool information */
        m_BXToolInformation[i] = this->BinaryToUnsignedChar(crp);
        crp++;

        /* get the marker information */
        for (j = 0; j < 10; j++)
          {
          n = this->BinaryToUnsignedChar(crp);
          crp++;
          m_BXMarkerInformation[i][2*j] = (n & 0x0F);
          n >>= 4;
          m_BXMarkerInformation[i][2*j + 1] = (n & 0x0F);
          }
        }

      /* grab the single marker info */ 
      if (mode & NDI_SINGLE_STRAY)
        {
        /* read the status */
        m_BXSingleStrayStatus[i] = this->BinaryToUnsignedChar(crp);
        crp++;

        /* read the coordinates if tool isn't missing */
        for (j = 0; j < 3; j++)
          {
          m_BXSingleStrayPosition[i][j] = this->BinaryToFloat(crp);
          crp += 4;
          }
        }
      }
    }

  m_BXNumberOfPassiveStrays = 0;
  /* get all the passive stray information */
  if (mode & NDI_PASSIVE_STRAY)
    {
    /* get the number of strays */
    n = this->BinaryToUnsignedChar(crp);
    crp++;
    nstray = 50;
    if (n <= 50)
      {
      nstray = n;
      }

    /* get the number of bytes for out-of-volume bits */
    n = (nstray + 7)/8;
    for (i = 0; i < 7; i++)
      {
      m_BXPassiveStrayOutOfVolume[i] = 0;
      }
    for (i = 0; i < n; i++)
      {
      m_BXPassiveStrayOutOfVolume[i] = this->BinaryToUnsignedChar(crp);
      crp++;
      }

    /* if not reporting out-of-volume, reduce nstray */
    if (!(mode & NDI_INCLUDE_OUT_OF_VOLUME))
      {
      for (i = 0; i < n; i++)
        {
        int bits = m_BXPassiveStrayOutOfVolume[i];
        /* clear for proper use by GetBXPassiveStray() */
        m_BXPassiveStrayOutOfVolume[i] = 0;

        for (j = 0; j < 8; j++)
          {
          if (bits & 1)
            {
            nstray--;
            }
          bits >>= 1;
          }
        }
      }

    /* set to the reduced value */
    m_BXNumberOfPassiveStrays = nstray;

    for (i = 0; i < nstray; i++)
      {
      for (j = 0; j < 3; j++)
        {
        m_BXPassiveStrayPosition[i][j] = this->BinaryToFloat(crp);
        crp += 4;
        }
      }
    }

  /* get the system status */
  m_BXSystemStatus = this->BinaryToUnsignedShort(crp);
  crp += 2;
}

/*---------------------------------------------------------------------
  Copy all the SSTAT reply information into the ndicapi structure.
*/
void NDICommandInterpreter::HelperForSSTAT(const char* cp, const char* crp)
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
void NDICommandInterpreter::HelperForVER(const char*, const char* crp)
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
void NDICommandInterpreter::HelperForIRCHK(const char* cp, const char* crp)
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
void NDICommandInterpreter::HelperForCOMM(const char* cp, 
                                           const char* itkNotUsed(crp))
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
int NDICommandInterpreter::SetErrorCode(int errnum)
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
void NDICommandInterpreter::PrintSelf(std::ostream& os,
                                      itk::Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Tracking: " << m_Tracking << std::endl;
  os << indent << "ErrorCode: " << m_ErrorCode << std::endl;
}


}
