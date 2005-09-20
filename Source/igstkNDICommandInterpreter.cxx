/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkNDICommandInterpreter.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkNDICommandInterpreter.h"
#include <stdio.h>
#include <string.h>

// maximum allowed size for a command to the device
const int NDI_MAX_COMMAND_SIZE = 2047;
const int NDI_MAX_REPLY_SIZE = 2047;
// timeouts for tracking and non-tracking modes
const int NDI_NORMAL_TIMEOUT = 5000;
const int NDI_TRACKING_TIMEOUT = 100;

namespace igstk
{


/** Constructor: initialize internal variables. */
NDICommandInterpreter::NDICommandInterpreter()
{
  m_Communication = 0;

  m_SerialCommand = new char[NDI_MAX_COMMAND_SIZE+1];
  m_SerialReply = new char[NDI_MAX_REPLY_SIZE+1];
  m_CommandReply = new char[NDI_MAX_REPLY_SIZE+1];

  m_Tracking = 0;
  this->SetErrorCode(0);
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
 
  /* These are the communication parameters that the NDI devices are
     set up for when they are turned on or reset. */
  communication->SetBaudRate(SerialCommunication::BaudRate9600);
  communication->SetDataBits(SerialCommunication::DataBits8);
  communication->SetParity(SerialCommunication::NoParity);
  communication->SetStopBits(SerialCommunication::StopBits1);
  communication->SetHardwareHandshake(SerialCommunication::HandshakeOff);

  /* The timeouts are tricky to deal with.  The NDI devices reply
     almost immediately after most command, with notable exceptions:
     INIT, RESET, PINIT and some diagnostic commands tie up the
     device for quite some time.  The worst offender is PINIT,
     since port initialization can take up to 5 seconds.

     A long timeout is a problem if a line error (i.e. error caused
     by noise in the serial cable) occurs.  Line errors occur
     infrequently at 57600 baud and frequenty at 115200 baud.
     A line error can cause a loss of the trailing carriage return
     on a data record, and the Read() command will of course keep
     waiting for this nonexistent carriage return. Five seconds is
     a long time to wait!  As a solution, while in tracking mode, the
     timeout period is reduced to just 0.1 seconds so that errors
     can be detected and dealt with swiftly.

     Line errors in tracking mode are no danger, since CRC checking will
     automatically detect and discard bad data records.  However, we
     don't want the system to freeze for 5 seconds each time an error
     like this occurs, which is why we reduce the timeout to 0.1s in
     tracking mode.
  */
  communication->SetTimeoutPeriod(NDI_NORMAL_TIMEOUT);

  /* All replies from the NDI devices end in a carriage return. */
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
    if (c >= 'a' && c <= 'f')
      {
      result = (result << 4) | (c + (10 - 'a'));
      }
    else if (c >= 'A' && c <= 'F')
      {
      result = (result << 4) | (c + (10 - 'A'));
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
    if (c >= 'a' && c <= 'f')
      {
      result = (result << 4) | (c + (10 - 'a'));
      }
    else if (c >= 'A' && c <= 'F')
      {
      result = (result << 4) | (c + (10 - 'A'));
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
  static char* textarrayLow[] = /* values from 0x01 to 0x23 */ 
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

  static char* textarrayHigh[] = /* values from 0xf6 to 0xf4 */
  {
    "Too much environmental infrared",
    "Unrecognized error code",
    "Unrecognized error code",
    "Unable to read Flash EPROM",
    "Unable to write Flash EPROM",
    "Unable to erase Flash EPROM"
  };

  static char* textarrayApi[] = /* values specific to the API */
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
  else if (errnum <= 0xf6 && errnum >= 0xf4)
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

inline void CalcCRC16(int nextchar, unsigned int *puCRC16)
{
  static const int oddparity[16] =    { 0, 1, 1, 0, 1, 0, 0, 1,
                                        1, 0, 0, 1, 0, 1, 1, 0 };
  int data;
  data = nextchar;
  data = (data ^ (*(puCRC16) & 0xff)) & 0xff;
  *puCRC16 >>= 8;
  if ( oddparity[data & 0x0f] ^ oddparity[data >> 4] )
    {
    *(puCRC16) ^= 0xc001;
    }
  data <<= 6;
  *puCRC16 ^= data;
  data <<= 1;
  *puCRC16 ^= data;
}

/*---------------------------------------------------------------------
  Make a 16bits WORD from two bytes.
*/
inline void* MakeWord(void* pWord, unsigned char b1, unsigned char b2)
{
  *(unsigned short *)pWord = (b1 << 8) | (b2);
  return pWord;
}


/*---------------------------------------------------------------------
  Make a 32bits DWORD from four bytes.
*/
inline void* MakeDWord(void* pDWord, unsigned char b1, unsigned char b2,
                                     unsigned char b3, unsigned char b4)
{
  *(unsigned long *)pDWord = (b1 << 24) | (b2 << 16) | (b3 << 8) | (b4);
  return pDWord;
}


/** Send a command to the device via the Communication object. */
const char* NDICommandInterpreter::Command(const char* command)
{
  unsigned int i, m, nc;
  unsigned int CRC16 = 0;
  int useCRC = 0;
  int inCommand = 1;
  int readError;
  char* cp, *rp, *crp;

  cp = m_SerialCommand;      /* text sent to ndicapi */
  rp = m_SerialReply;        /* text received from ndicapi */
  crp = m_CommandReply;      /* received text, with CRC hacked off */
  nc = 0;                    /* length of 'command' part of command */

  m_ErrorCode = 0;           /* clear error */
  rp[0] = '\0';
  crp[0] = '\0';

  /* if the command is NULL, send a break to reset the device */
  if (command == NULL)
    {
    cp[0] = '\0';
    /* serial break will force tracking to stop */
    m_Tracking = 0;
    m_Communication->SetTimeoutPeriod(NDI_NORMAL_TIMEOUT);
    m_Communication->SetBaudRate(SerialCommunication::BaudRate9600);
    m_Communication->SetHardwareHandshake(SerialCommunication::HandshakeOff);
    m_Communication->Sleep(500);

    /* send brain, reset the comm parameters, and sleep for 2 seconds */
    m_Communication->SendBreak();
    m_Communication->Sleep(2000);

    readError = m_Communication->Read(rp, NDI_MAX_REPLY_SIZE, m);
    
    if ( readError == Communication::FAILURE )
      {
      this->SetErrorCode( NDI_READ_ERROR );
      return crp;
      }
    else if ( readError == Communication::TIMEOUT )
      {
      this->SetErrorCode( NDI_TIMEOUT );
      return crp;
      }

    /* check for correct reply */
    if (strncmp(rp, "RESETBE6F\r", 10) != 0)
      { 
      this->SetErrorCode(NDI_RESET_FAIL);
      return crp;
      }

    /* terminate the reply string */
    rp[m] = '\0';
    m -= 5;
    strncpy(crp, rp, m);
    crp[m] = '\0';
    
    /* return the reply string, minus the CRC */
    return crp;
    }

  if (cp != command)
    {
    strncpy(cp, command, NDI_MAX_COMMAND_SIZE);
    }

  CRC16 = 0;                                  /* calculate CRC */
  for (i = 0; cp[i] != '\0'; i++)
    {
    CalcCRC16(cp[i], &CRC16);
    if (inCommand && cp[i] == ':')
      {                                      /* only use CRC if a ':' */
      useCRC = 1;                            /*  follows the command  */
      }
    if (inCommand && !((cp[i] >= 'A' && cp[i] <= 'Z') || 
                       (cp[i] >= '0' && cp[i] <= '9')))
      {
      inCommand = 0;                          /* 'command' part has ended */
      nc = i;                                 /* command length */
      }
    }

  if (useCRC)
    {
    sprintf(&cp[i], "%04X", CRC16);           /* tack on the CRC */
    i += 4;
    }

  cp[i] = '\0';

  cp[i++] = '\r';                             /* tack on carriage return */
  cp[i] = '\0';                               /* terminate for good luck */

  /* send the command directly to the device and get a reply */
  int errcode = 0;

  /* change  m_Tracking  if either TSTOP or TSTART or INIT is sent  */ 
  if ((nc == 5 && strncmp(cp, "TSTOP", nc) == 0) ||
      (nc == 4 && strncmp(cp, "INIT", nc) == 0))
    {
    m_Tracking = 0;
    m_Communication->SetTimeoutPeriod(NDI_NORMAL_TIMEOUT);
    }

  /* flush the input buffer, because anything that we haven't read
       yet is garbage left over by a previously failed command */
  m_Communication->PurgeBuffers();

  /* send the command to the device */
  int writeError = m_Communication->Write(cp, strlen(cp));
  /* is there any easy way to check if the write failed, or
   * is it necessary to catch the events? There should be code
   * here to check, and set errcode = NDI_WRITE_ERROR or 
   * NDI_TIMEOUT */
  if ( writeError == Communication::FAILURE )
    {
    errcode = NDI_WRITE_ERROR;
    }
  else if ( writeError == Communication::TIMEOUT )
    {
    errcode = NDI_TIMEOUT;
    }

  /* read the reply from the device */
  m = 0;
  if (errcode == 0)
    {
    if (cp[0] == 'B' && cp[1] == 'X' && nc == 2)
      { /* the BX command */
      m_Communication->SetUseReadTerminationCharacter( false );
      readError = m_Communication->Read(rp, 6, m);
      if ( readError == Communication::FAILURE )
        {
        this->SetErrorCode(NDI_READ_ERROR);
        return crp;
        }
      else if ( readError == Communication::TIMEOUT )
        {
        this->SetErrorCode(NDI_TIMEOUT);
        return crp;
        }

      if (m == 0)
        {
        this->SetErrorCode(NDI_TIMEOUT);
        return crp;
        }

      for ( i = 0; i < m; ++i )
        {
        crp[i] = rp[i];
        }
      unsigned short ReplyLength;
      char StartBytes[2] = { 0xC4, 0xA5 };
      unsigned int HeaderCRC16, BodyCRC16;

      if( crp[0] != StartBytes[0]  ||  crp[1] != StartBytes[1] )
        {
        /* ERROR : The reply is not for BX! */
        return crp;
        }

      MakeWord( &ReplyLength, crp[3], crp[2] );

      // Check Header CRC16
      HeaderCRC16 = 0;
      for( i = 0; i < 4; ++i )
        {
        CalcCRC16(crp[i], &HeaderCRC16);
        }
      unsigned short ReplyedHeaderCRC16 = 0;
      MakeWord( &ReplyedHeaderCRC16, crp[5], crp[4] );
      if( HeaderCRC16 != ReplyedHeaderCRC16 )
        {
        this->SetErrorCode(NDI_BAD_CRC);
        return crp;
        }

      readError = m_Communication->Read(rp, ReplyLength+2, m);
      if ( readError == Communication::FAILURE )
        {
        this->SetErrorCode(NDI_READ_ERROR);
        return crp;
        }
      else if ( readError == Communication::TIMEOUT )
        {
        this->SetErrorCode(NDI_TIMEOUT);
        return crp;
        }
      
      if (m == 0)
        {
        this->SetErrorCode(NDI_TIMEOUT);
        return crp;
        }

      for ( i = 0; i < m; ++i )
        {
        crp[i] = rp[i];
        }

      // Check Body CRC16
      BodyCRC16 = 0;
      for( i = 0; i < ReplyLength; ++i )
        {
        CalcCRC16(crp[i], &BodyCRC16);
        }
      unsigned short ReplyedBodyCRC16 = 0;
      MakeWord( &ReplyedBodyCRC16, crp[ReplyLength+1], crp[ReplyLength] );
      if( BodyCRC16 != ReplyedBodyCRC16 )
        {
        this->SetErrorCode(NDI_BAD_CRC);
        return crp;
        }

      HelperForBX(cp, (const unsigned char*)crp);
      return crp;
      }
    else
      {
      m_Communication->SetUseReadTerminationCharacter( true );
      readError = m_Communication->Read(rp, NDI_MAX_REPLY_SIZE, m);
      if ( readError == Communication::FAILURE )
        {
        errcode = NDI_READ_ERROR;
        m = 0;
        }
      else if ( readError == Communication::TIMEOUT )
        {
        errcode = NDI_TIMEOUT;
        }

      /* need better code to check for error and set NDI_WRITE_ERROR or
       * NDI_TIMEOUT, since both result in m=0 */

      if (m == 0)
        {
        errcode = NDI_TIMEOUT;
        }
      else if (rp[m-1] != '\r')
        {
        errcode = NDI_READ_ERROR;
        m = 0;
        }

      rp[m] = '\0';   /* terminate string */
      }
    }

  if (errcode != 0)
    {
    this->SetErrorCode(errcode);
    return crp;
    }

  /* back up to before the CRC */
  m -= 5;
  if (m < 0)
    {
    this->SetErrorCode(NDI_BAD_CRC);
    return crp;
    }

  /* calculate the CRC and copy serial_reply to command_reply */
  CRC16 = 0;
  for (i = 0; i < m; i++)
    {
    CalcCRC16(rp[i], &CRC16);
    crp[i] = rp[i];
    }

  /* terminate command_reply before the CRC */
  crp[i] = '\0';           

  /* read and check the CRC value of the reply */
  if (CRC16 != this->HexadecimalStringToUnsignedInt(&rp[m], 4)) {
    this->SetErrorCode(NDI_BAD_CRC);
    return crp;
  }

  /* check for error code */
  if (crp[0] == 'E' && strncmp(crp, "ERROR", 5) == 0)
    {
    this->SetErrorCode(this->HexadecimalStringToUnsignedInt(&crp[5], 2));
    return crp;
    }

  /* anything past here is only executed if no error occurred */

  if (nc == 6 && strncmp(cp, "TSTART", nc) == 0)
    {
    /* decrease the timeout, since otherwise the system will freeze
       for 5 seconds each time there is a CRC error */
    m_Communication->SetTimeoutPeriod(NDI_TRACKING_TIMEOUT);
    m_Tracking = 1;
    }

  /*----------------------------------------*/
  /* special behavior for specific commands */

  if (cp[0] == 'T' && cp[1] == 'X' && nc == 2)
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
  else if (cp[0] == 'S' && nc == 5 && strncmp(cp, "SSTAT", nc) == 0)
    {
    HelperForSSTAT(cp, crp);
    }

  /* return the device's reply, but with the CRC hacked off */
  return crp;
}

/** Use printf-style formatting to create a command. */
const char* NDICommandInterpreter::Command(const char* format, int a)
{
  sprintf(m_SerialCommand, format, a);
  return this->Command(m_SerialCommand);
}

/** Use printf-style formatting to create a command. */
const char* NDICommandInterpreter::Command(const char* format, int a, int b)
{
  sprintf(m_SerialCommand, format, a, b);
  return this->Command(m_SerialCommand);
}

/** Use printf-style formatting to create a command. */
const char* NDICommandInterpreter::Command(const char* format, int a, int b,
                                           int c)
{
  sprintf(m_SerialCommand, format, a, b, c);
  return this->Command(m_SerialCommand);
}

/** Use printf-style formatting to create a command. */
const char* NDICommandInterpreter::Command(const char* format, int a, int b,
                                           int c, int d)
{
  sprintf(m_SerialCommand, format, a, b, c, d);
  return this->Command(m_SerialCommand);
}

/** Use printf-style formatting to create a command. */
const char* NDICommandInterpreter::Command(const char* format, int a, int b,
                                           const char* c)
{
  sprintf(m_SerialCommand, format, a, b, c);
  return this->Command(m_SerialCommand);
}

/** Use printf-style formatting to create a command. */
const char* NDICommandInterpreter::Command(const char* format, const char* a,
                                           const char* b, const char* c,
                                           const char* d, const char* e)
{
  sprintf(m_SerialCommand, format, a, b, c, d, e);
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
int NDICommandInterpreter::GetPHINFToolInfo(char information[31]) const
{
  const char* dp;
  int i;
  
  dp = m_PHINFBasic;

  for (i = 0; i < 31; i++)
    {
    information[i] = *dp++;
    }

  return m_PHINFUnoccupied;
}

/** Return data that was received from a PHINF command. */
unsigned int NDICommandInterpreter::GetPHINFCurrentTest() const
{
  const char* dp;

  dp = m_PHINFTesting;

  return this->HexadecimalStringToInt(dp, 8);  
}

/** Return data that was received from a PHINF command. */
int NDICommandInterpreter::GetPHINFPartNumber(char part[20]) const
{
  const char* dp;
  int i;
  
  dp = m_PHINFPartNumber;

  for (i = 0; i < 20; i++)
    {
    part[i] = *dp++;
    }

  return m_PHINFUnoccupied;
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
int NDICommandInterpreter::GetPHINFPortLocation(char location[14]) const
{
  const char* dp;
  int i;
  
  dp = m_PHINFPortLocation;

  for (i = 0; i < 14; i++)
    {
    location[i] = *dp++;
    }

  return m_PHINFUnoccupied;
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

  dp = m_PHSRReply;
  n = this->HexadecimalStringToInt(dp, 2);
  dp += 2;

  if (i < 0 || i > n)
    {
    return 0;
    }
  dp += 5*i;

  return this->HexadecimalStringToInt(dp, 2);  
}

/** Return data that was received from a PHSR command. */
int NDICommandInterpreter::GetPHSRInformation(int i) const
{
  const char* dp;
  int n;

  dp = m_PHSRReply;
  n = this->HexadecimalStringToInt(dp, 2);
  dp += 2;

  if (i < 0 || i > n)
    {
    return 0;
    }
  dp += 5*i + 2;

  return this->HexadecimalStringToInt(dp, 3);  
}

/** Return a transform that was received after a TX command. */
int NDICommandInterpreter::GetTXTransform(int ph, double transform[8]) const
{
  const char* dp;
  int i, n;
  
  n = m_TXNumberOfHandles;
  for (i = 0; i < n; i++)
    {
    if (m_TXHandles[i] == ph)
      {
      break;
      }
    }
  if (i == n)
    {
    return NDI_DISABLED;
    }

  dp = m_TXTransforms[i];
  if (*dp == 'D' || *dp == '\0')
    {
    return NDI_DISABLED;
    }
  else if (*dp == 'M')
    {
    return NDI_MISSING;
    }
  
  transform[0] = this->SignedStringToInt(&dp[0],  6)*0.0001;
  transform[1] = this->SignedStringToInt(&dp[6],  6)*0.0001;
  transform[2] = this->SignedStringToInt(&dp[12], 6)*0.0001;
  transform[3] = this->SignedStringToInt(&dp[18], 6)*0.0001;
  transform[4] = this->SignedStringToInt(&dp[24], 7)*0.01;
  transform[5] = this->SignedStringToInt(&dp[31], 7)*0.01;
  transform[6] = this->SignedStringToInt(&dp[38], 7)*0.01;
  transform[7] = this->SignedStringToInt(&dp[45], 6)*0.0001;

  return NDI_TRANSFORM_OKAY;
}

/** Return port status info that was received after a TX command. */
int NDICommandInterpreter::GetTXPortStatus(int ph) const
{
  const char* dp;
  int i, n;
  
  n = m_TXNumberOfHandles;
  for (i = 0; i < n; i++)
    {
    if (m_TXHandles[i] == ph)
      {
      break;
      }
    }
  if (i == n)
    {
    return 0;
    }

  dp = m_TXStatus[i];

  return this->HexadecimalStringToInt(dp, 8);
}

/** Return the frame number for data received from a TX command. */
unsigned int NDICommandInterpreter::GetTXFrame(int ph) const
{
  const char* dp;
  int i, n;

  n = m_TXNumberOfHandles;
  for (i = 0; i < n; i++)
    {
    if (m_TXHandles[i] == ph)
      {
      break;
      }
    }
  if (i == n)
    {
    return 0;
    }

  dp = m_TXFrame[i];

  return this->HexadecimalStringToUnsignedInt(dp, 8);
}

/** Return data received from a TX command. */
int NDICommandInterpreter::GetTXToolInfo(int ph) const
{
  const char* dp;
  int i, n;
  
  n = m_TXNumberOfHandles;
  for (i = 0; i < n; i++)
    {
    if (m_TXHandles[i] == ph)
      {
      break;
      }
    }
  if (i == n)
    {
    return 0;
    }

  dp = m_TXInformation[i];
  return this->HexadecimalStringToInt(dp, 2);
}

/** Return data received from a TX command. */
int NDICommandInterpreter::GetTXMarkerInfo(int ph, int marker) const
{
  const char* dp;
  int i, n;
  
  n = m_TXNumberOfHandles;
  for (i = 0; i < n; i++)
    {
    if (m_TXHandles[i] == ph)
      {
      break;
      }
    }
  if (i == n || marker < 0 || marker >= 20)
    {
    return NDI_DISABLED;
    }

  dp = m_TXInformation[2+marker];
  return this->HexadecimalStringToInt(dp, 1);
}

/** Return data received from a TX command. */
int NDICommandInterpreter::GetTXSingleStray(int ph, double coord[3]) const
{
  const char* dp;
  int i, n;
  int status;
  
  n = m_TXNumberOfHandles;
  for (i = 0; i < n; i++)
    {
    if (m_TXHandles[i] == ph)
      {
      break;
      }
    }
  if (i == n)
    {
    return NDI_DISABLED;
    }

  dp = m_TXSingleStray[i];
  status = HexadecimalStringToInt(dp, 2);
  if (status == 0x00)
    {
    return NDI_DISABLED;
    }
  else if (status == 0x02)
    {
    return NDI_MISSING;
    }

  coord[0] = this->SignedStringToInt(&dp[2],  7)*0.01;
  coord[1] = this->SignedStringToInt(&dp[9],  7)*0.01;
  coord[2] = this->SignedStringToInt(&dp[16], 7)*0.01;

  return NDI_OKAY;
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
  int n;

  dp = m_TXPassiveStray;

  if (*dp == '\0')
    {
    return NDI_DISABLED;
    }

  n = m_TXNumberOfPassiveStrays;
  if (n < 0)
    {
    return NDI_MISSING;
    }
  if (n > 50)
    {
    n = 50;
    }

  if (i < 0 || i >= n)
    {
    return NDI_MISSING;
    }

  dp += 7*3*i;
  coord[0] = this->SignedStringToInt(&dp[0],  7)*0.01;
  coord[1] = this->SignedStringToInt(&dp[7],  7)*0.01;
  coord[2] = this->SignedStringToInt(&dp[14], 7)*0.01;

  return NDI_OKAY;
}

/** Return data received from a TX command. */
int NDICommandInterpreter::GetTXSystemStatus() const
{
  const char* dp;

  dp = m_TXSystemStatus;

  return this->HexadecimalStringToInt(dp, 4);
}

/** Return data received from an SSTAT command. */
int NDICommandInterpreter::GetSSTATControl() const
{
  const char* dp;

  dp = m_SSTATControl;

  if (*dp == '\0')
    {
    return 0;
    }

  return this->HexadecimalStringToInt(dp, 2);
}

/** Return data received from an SSTAT command. */
int NDICommandInterpreter::GetSSTATSensors() const
{
  const char* dp;

  dp = m_SSTATSensor;

  if (*dp == '\0')
    {
    return 0;
    }

  return this->HexadecimalStringToInt(dp, 2);
}

/** Return data received from an SSTAT command. */
int NDICommandInterpreter::GetSSTATTIU() const
{
  const char* dp;

  dp = m_SSTATTIU;

  if (*dp == '\0')
    {
    return 0;
    }

  return this->HexadecimalStringToInt(dp, 2);
}

/** Return data received from an SSTAT command. */
int NDICommandInterpreter::GetIRCHKDetected() const
{
  if (m_IRCHKDetected == '1')
    {
    return 1;
    }
  return 0;
}

/** Return data received from an IRCHK command. */
int NDICommandInterpreter::GetIRCHKNumberOfSources(int side) const
{
  const char* dp;
  int n, m;

  dp = m_IRCHKSources;

  if (*dp == '\0')
    {
    return 0;
    }

  n = this->SignedStringToInt(dp, 3);
  if (n < 0 || n > 20)
    {
    return 0;
    }
  m = this->SignedStringToInt(dp + 3 + 2*3*n, 3);
  if (m < 0 || m > 20)
    {
    return 0;
    }

  if (side == NDI_LEFT)
    {
    return n;
    }
  else if (side == NDI_RIGHT)
    {
    return m;
    }

  return 0;
}
 
/** Return data received from an IRCHK command. */
int NDICommandInterpreter::GetIRCHKSourceXY(int side, int i,
                                            double xy[2]) const
{
  const char* dp;
  int n, m;

  dp = m_IRCHKSources;

  if (dp == NULL || *dp == '\0')
    {
    return NDI_MISSING;
    }

  n = this->SignedStringToInt(dp, 3);
  if (n < 0 || n > 20)
    {
    return NDI_MISSING;
    }
  m = this->SignedStringToInt(dp + 3 + 2*3*n, 3);
  if (m < 0 || m > 20)
    {
    return NDI_MISSING;
    }

  if (side == NDI_LEFT)
    {
    if (i < 0 || i >= n)
      {
      return NDI_MISSING;
      }
    dp += 3 + 2*3*i;
    }
  else if (side == NDI_RIGHT)
    {
    if (i < 0 || i >= m)
      {
      return NDI_MISSING;
      }
    dp += 3 + 2*3*n + 3 + 2*3*i;
    }
  else if (side == NDI_RIGHT)
    {
    return NDI_MISSING;
    }

  xy[0] = this->SignedStringToInt(&dp[0], 3)*0.01;
  xy[1] = this->SignedStringToInt(&dp[3], 3)*0.01;

  return NDI_OKAY;
}

/*---------------------------------------------------------------------
  Copy all the PHINF reply information into the ndicapi structure, according
  to the PHINF reply mode that was requested.

  This function is called every time a PHINF command is sent to the
  Measurement System.

  This information can be later extracted through one of the ndiGetPHINFxx()
  functions.
*/
void NDICommandInterpreter::HelperForPHINF(const char* cp, const char* crp)
{
  unsigned int mode = 0x0001; /* the default reply mode */
  char* dp;
  int j;
  int unoccupied = NDI_OKAY;

  /* if the PHINF command had a reply mode, read it */
  if ((cp[5] == ':' && cp[10] != '\r') || (cp[5] == ' ' && cp[6] != '\r'))
    { 
    mode = this->HexadecimalStringToUnsignedInt(&cp[8], 4);
    }

  /* check for unoccupied */
  if (*crp == 'U')
    {
    unoccupied = NDI_UNOCCUPIED;
    }
  m_PHINFUnoccupied = unoccupied;

  /* fprintf(stderr, "mode = %04lx\n", mode); */

  if (mode & NDI_BASIC)
    {
    dp = m_PHINFBasic;
    if (!unoccupied)
      {
      for (j = 0; j < 33 && *crp >= ' '; j++)
        {
        /* fprintf(stderr,"%c",*crp); */
        *dp++ = *crp++;
        }
      /* fprintf(stderr,"\n"); */
      }
    else
      {  /* default "00000000            0000000000000" */
      for (j = 0; j < 8; j++)
        {
        *dp++ = '0';
        }
      for (j = 0; j < 12; j++)
        {
        *dp++ = ' ';
        }
      for (j = 0; j < 13; j++)
        {
        *dp++ = '0';
        }
      }
    }
  
  if (mode & NDI_TESTING)
    {
    dp = m_PHINFTesting;
    if (!unoccupied)
      {
      for (j = 0; j < 8 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
      }
    }
    else
      { /* default "00000000" */
      for (j = 0; j < 8; j++)
        {
        *dp++ = '0';
        }
      }
    }

  if (mode & NDI_PART_NUMBER)
    {
    dp = m_PHINFPartNumber;
    if (!unoccupied)
      {
      for (j = 0; j < 20 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      }
    else
      { /* default "                    " */
      for (j = 0; j < 20; j++)
        {
        *dp++ = ' ';
        }
      }
    }

  if (mode & NDI_ACCESSORIES)
    {
    dp = m_PHINFAccessories;
    if (!unoccupied)
      {
      for (j = 0; j < 2 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      }
    else
      { /* default "00" */
      for (j = 0; j < 2; j++)
        {
        *dp++ = '0';
        }
      }
    }

  if (mode & NDI_MARKER_TYPE)
    {
    dp = m_PHINFMarkerType;
    if (!unoccupied)
      {
      for (j = 0; j < 2 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      }
    else
      { /* default "00" */
      for (j = 0; j < 2; j++)
        {
        *dp++ = '0';
        }
      }
    }

  if (mode & NDI_PORT_LOCATION)
    {
    dp = m_PHINFPortLocation;
    if (!unoccupied)
      {
      for (j = 0; j < 14 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      }
    else
      { /* default "00000000000000" */
      for (j = 0; j < 14; j++)
        {
        *dp++ = '0';
        }
      }
    }
  
  m_PHINFGPIOStatus[0] = 0;
  m_PHINFGPIOStatus[1] = 0;
  if (mode & NDI_GPIO_STATUS)
    {
    dp = m_PHINFGPIOStatus;
    if (!unoccupied)
      {
      for (j = 0; j < 2 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      }
    else
      { /* default "00" */
      for (j = 0; j < 2; j++)
        {
        *dp++ = '0';
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
void NDICommandInterpreter::HelperForPHRQ(const char* cp, const char* crp)
{
  char* dp;
  int j;

  dp = m_PHRQReply;
  for (j = 0; j < 2; j++)
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
void NDICommandInterpreter::HelperForPHSR(const char* cp, const char* crp)
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
  unsigned int mode = 0x0001; /* the default reply mode */
  char* dp;
  int i, j, n;
  int ph, nhandles, nstray, status;

  /* if the TX command had a reply mode, read it */
  if ((cp[2] == ':' && cp[7] != '\r') || (cp[2] == ' ' && cp[3] != '\r'))
    { 
    mode = this->HexadecimalStringToUnsignedInt(&cp[3], 4);
    }

  /* get the number of handles */
  nhandles = this->HexadecimalStringToInt(crp, 2);
  for (j = 0; j < 2 && *crp >= ' '; j++)
    {
    crp++;
    }

  memset(m_TXInformation, 0, sizeof(char)*NDI_MAX_HANDLES*12);
  /* go through the information for each handle */
  for (i = 0; i < nhandles; i++)
    {
    /* get the handle itself (two chars) */
    ph = this->HexadecimalStringToInt(crp,2);
    for (j = 0; j < 2 && *crp >= ' '; j++)
      {
      crp++;
      }

    /* check for "UNOCCUPIED" */
    if (*crp == 'U')
      {
      for (j = 0; j < 10 && *crp >= ' '; j++)
        {
        crp++;
        }
      /* back up and continue (don't store information for unoccupied ports) */
      i--;
      nhandles--;
      continue;
      }

    /* save the port handle in the list */
    m_TXHandles[i] = ph;

    if (mode & NDI_XFORMS_AND_STATUS)
      {
      /* get the transform, MISSING, or DISABLED */
      dp = m_TXTransforms[i];

      if (*crp == 'M')
        {
        /* check for "MISSING" */
        for (j = 0; j < 7 && *crp >= ' '; j++)
          {
          *dp++ = *crp++;
          }
        }
      else if (*crp == 'D')
        {
        /* check for "DISABLED" */
        for (j = 0; j < 8 && *crp >= ' '; j++)
          {
          *dp++ = *crp++;
          }
        }
      else
        {
        /* read the transform */
        for (j = 0; j < 51 && *crp >= ' '; j++)
          {
          *dp++ = *crp++;
          }
        }
      *dp = '\0';

      /* get the status */
      dp = m_TXStatus[i];
      for (j = 0; j < 8 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }

      /* get the frame number */
      dp = m_TXFrame[i];
      for (j = 0; j < 8 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      }

    /* grab additonal information */
    if (mode & NDI_ADDITIONAL_INFO)
      {
      dp = m_TXInformation[i];
      for (j = 0; j < 22 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      }

    /* grab the single marker info */ 
    if (mode & NDI_SINGLE_STRAY)
      {
      status = this->HexadecimalStringToInt(crp, 2);
      dp = m_TXSingleStray[i];
      /* read the status */
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

  /* save the number of handles (minus the unoccupied handles) */
  m_TXNumberOfHandles = nhandles;

  /* get all the passive stray information */
  /* this will be a maximum of 3 + 13 + 50*3*7 = 1066 bytes */
  if (mode & NDI_PASSIVE_STRAY)
    {
    /* get the number of strays */
    nstray = this->StringToInt(crp, 2);
    for (j = 0; j < 2 && *crp >= ' '; j++)
      {
      crp++;
      }
    if (nstray > 50)
      {
      nstray = 50;
      }
    m_TXNumberOfPassiveStrays = nstray;
    /* get the out-of-volume bits */
    dp = m_TXPassiveStrayOutOfVolume;
    n = (nstray + 3)/4;
    for (j = 0; j < n && *crp >= ' '; j++)
      {
      *dp++ = *crp++;
      }
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
void NDICommandInterpreter::HelperForBX(const char* cp, const unsigned char* crp)
{
  unsigned int mode = 0x0001; /* the default reply mode */
  int i, j;
  int handle, nHandles;

  /* if the TX command had a reply mode, read it */
  if ((cp[2] == ':' && cp[7] != '\r') || (cp[2] == ' ' && cp[3] != '\r'))
    { 
    mode = this->HexadecimalStringToUnsignedInt(&cp[3], 4);
    }

  /* get the number of handles */
  m_BXNumberOfHandles = 0;
  nHandles = crp[0];
  ++crp;

  /* go through the information for each handle */
  for (i = 0; i < nHandles; i++)
    {
    /* get the handle itself */
    handle = *crp;
    ++crp;
    /* save the port handle in the list */
    m_BXHandles[i] = handle;
    m_BXHandleStatus[i] = *crp;
    ++crp;

    /* check for DISABLED */
    if (m_BXHandleStatus[i] & 0x04)
      {
      continue;
      }
    
    if (mode & NDI_XFORMS_AND_STATUS )
      {
      if (! (m_BXHandleStatus[i] & 0x02) ) // NOT MISSING
        {
        /* get the transform */
        for ( j = 0; j < 4; ++j )
          {
          MakeDWord( &m_BXRotation[i][j], *(crp+3), *(crp+2), *(crp+1), *crp );
          crp += 4;
          }
        for ( j = 0; j < 3; ++j )
          {
          MakeDWord( &m_BXTranslation[i][j], *(crp+3), *(crp+2), *(crp+1), *crp );
          crp += 4;
          }
        MakeDWord( &m_BXRMSError[i], *(crp+3), *(crp+2), *(crp+1), *crp );
        crp += 4;
        }

      /* get the port status */
      MakeDWord( &m_BXPortStatus[i], *(crp+3), *(crp+2), *(crp+1), *crp );
      crp += 4;

      /* get the frame number */
      MakeDWord( &m_BXFrame[i], *(crp+3), *(crp+2), *(crp+1), *crp );
      crp += 4;

      if ( (m_BXPortStatus[i] & 0x01) == 0 )
        {
        // unoccupied port.
        --i;
        --nHandles;
        continue;
        }
      }

    /* grab additonal information */
    if (mode & NDI_ADDITIONAL_INFO)
      {
      /* get the tool information */
      m_BXToolInformation[i] = *crp;
      ++crp;
      /* get the marker information */
      memcpy(m_BXMarkerInformation[i], crp, 10);
      crp += 10;
      }

    /* grab the single marker info */ 
    if (mode & NDI_SINGLE_STRAY)
      {
      /* read the status */
      m_BXActiveMarkerStatus[i] = *crp;
      ++crp;
      /* read the coordinates if tool isn't missing */
      for (j = 0; j < 3; ++j)
        {
        MakeDWord( &m_BXActiveMarkerPosition[i][j], *(crp+3), *(crp+2), *(crp+1), *(crp) );
        crp += 4;
        }
      }
    }
  /* save the number of handles (minus the unoccupied handles) */
  m_BXNumberOfHandles = nHandles;

  /* get all the passive stray information */
  if (mode & NDI_PASSIVE_STRAY)
    {
    /* get the number of strays */
    m_BXNumberOfPassiveStrays = *crp;
    ++crp;
    /* get the out-of-volume bits */
    int nBytes;
    nBytes = m_BXNumberOfPassiveStrays / 8;
    if( m_BXNumberOfPassiveStrays % 8 != 0 )
      {
      ++nBytes;
      }
    for ( i = 0; i < 7; ++i )
      {
      m_BXPassiveStrayOutOfVolume[i] = 0;
      }
    for ( i = 0; i < nBytes; ++i )
      {
      m_BXPassiveStrayOutOfVolume[i] = *crp;
      ++crp;
      }

    m_BXNumberOfPassiveStraysInsideVolume = m_BXNumberOfPassiveStrays;
    for ( i = 0; i < 7; ++i )
      {
      for ( j = 0; j < 8; ++j )
        {
        if ( m_BXPassiveStrayOutOfVolume[i] & (0x01 << j) )
          {
          --m_BXNumberOfPassiveStraysInsideVolume;
          }
        }
      }
    /* get the coordinates */
    if( mode & 0x0800 )
      {
      m_BXNumberOfAvailableStrayPositions = m_BXNumberOfPassiveStrays;;
      }
    else
      {
      m_BXNumberOfAvailableStrayPositions = m_BXNumberOfPassiveStraysInsideVolume;
      m_BXNumberOfPassiveStrays = m_BXNumberOfPassiveStraysInsideVolume;
      }
    for ( i = 0; i < m_BXNumberOfAvailableStrayPositions; ++i )
      {
      for ( j = 0; j < 3; ++j )
        {
        MakeDWord( &m_BXPassiveStrayPosition[i][j], *(crp+3), *(crp+2), *(crp+1), *crp );
        crp += 4;
        }
      }
    }

  /* get the system status */
  MakeWord( &m_BXSystemStatus, *(crp+1), *(crp) );
  crp += 2;
}


/** Return a transform that was received after a BX command. */
int NDICommandInterpreter::GetBXTransform(int ph, double transform[8]) const
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
  if (i == n)
    {
    return NDI_DISABLED;
    }

  if (m_BXHandleStatus[i] & 0x04 )
    {
    return NDI_DISABLED;
    }

  if (m_BXHandleStatus[i] & 0x02 )
    {
    return NDI_MISSING;
    }

  transform[0] = m_BXRotation[i][0];
  transform[1] = m_BXRotation[i][1];
  transform[2] = m_BXRotation[i][2];
  transform[3] = m_BXRotation[i][3];
  transform[4] = m_BXTranslation[i][0];
  transform[5] = m_BXTranslation[i][1];
  transform[6] = m_BXTranslation[i][2];
  transform[7] = m_BXRMSError[i];
  return NDI_TRANSFORM_OKAY;
}

/** Return port status info that was received after a BX command. */
int NDICommandInterpreter::GetBXPortStatus(int ph) const
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
  if (i == n)
    {
    return 0;
    }

  return m_BXPortStatus[i];
}

/** Return the frame number for data received from a BX command. */
unsigned int NDICommandInterpreter::GetBXFrame(int ph) const
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
  if (i == n)
    {
    return 0;
    }

  return m_BXFrame[i];
}

/** Return data received from a BX command. */
int NDICommandInterpreter::GetBXToolInfo(int ph) const
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
  if (i == n)
    {
    return 0;
    }

  return m_BXToolInformation[i];
}

/** Return data received from a BX command. */
int NDICommandInterpreter::GetBXMarkerInfo(int ph, int marker) const
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
  if (i == n || marker < 0 || marker >= 20)
    {
    return NDI_DISABLED;
    }

  int byte = m_BXMarkerInformation[i][marker/2];
  if( marker % 2 == 0 )
    {
    byte &= 0x0F;
    }
  else
    {
    byte &= 0xF0;
    }
  return byte;
}

/** Return data received from a BX command. */
int NDICommandInterpreter::GetBXSingleStray(int ph, double coord[3]) const
{
  int i, n;
  int status;
  
  n = m_BXNumberOfHandles;
  for (i = 0; i < n; i++)
    {
    if (m_BXHandles[i] == ph)
      {
      break;
      }
    }
  if (i == n)
    {
    return NDI_DISABLED;
    }

  status = m_BXActiveMarkerStatus[i];
  if (status == 0x00)
    {
    return NDI_DISABLED;
    }
  else if (status == 0x02)
    {
    return NDI_MISSING;
    }

  coord[0] = m_BXActiveMarkerPosition[i][0];
  coord[1] = m_BXActiveMarkerPosition[i][1];
  coord[2] = m_BXActiveMarkerPosition[i][2];

  return NDI_OKAY;
}

/** Return data received from a BX command. */
int NDICommandInterpreter::GetBXNumberOfPassiveStrays() const
{
  return m_BXNumberOfPassiveStrays; 
}

/** Return data received from a BX command. */
int NDICommandInterpreter::GetBXPassiveStray(int i, double coord[3]) const
{
  int n;

  n = m_BXNumberOfAvailableStrayPositions;
  if (n < 0)
    {
    return NDI_MISSING;
    }
  if (n > 50)
    {
    n = 50;
    }

  if (i < 0 || i >= n)
    {
    return NDI_MISSING;
    }

  coord[0] = m_BXPassiveStrayPosition[i][0];
  coord[1] = m_BXPassiveStrayPosition[i][1];
  coord[2] = m_BXPassiveStrayPosition[i][2];

  return NDI_OKAY;
}

/** Return data received from a BX command. */
int NDICommandInterpreter::GetBXSystemStatus() const
{
  return m_BXSystemStatus;
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
void NDICommandInterpreter::HelperForCOMM(const char* cp, const char* crp)
{
  static SerialCommunication::BaudRateType convert_baud[6] = {
    SerialCommunication::BaudRate9600,
    SerialCommunication::BaudRate19200,
    SerialCommunication::BaudRate19200,
    SerialCommunication::BaudRate38400,
    SerialCommunication::BaudRate57600,
    SerialCommunication::BaudRate115200 };

  SerialCommunication::BaudRateType
    newspeed = SerialCommunication::BaudRate9600;
  
  SerialCommunication::HandshakeType
    newhand = SerialCommunication::HandshakeOff;

  /* Force to 8N1, don't allow 14400 baud (not supported by UNIX) */
  if (cp[5] < '0' || cp[5] == '1' || cp[5] > '5' ||
      cp[6] != '0' || cp[7] != '0' || cp[8] != '0')
    {
    this->SetErrorCode(NDI_BAD_COMM);
    return;
    }

  if (cp[5] >= '0' && cp[5] <= '5')
    {
    newspeed = convert_baud[cp[5]-'0'];
    }

  if (cp[9] == '1')
    {
    newhand = SerialCommunication::HandshakeOn;
    } 

  m_Communication->Sleep(100);  /* let the device adjust  */
  m_Communication->SetBaudRate(newspeed);
  m_Communication->SetHardwareHandshake(newhand);

  /* should set error code to NDI_BAD_COMM if the above fails */
}

/*---------------------------------------------------------------------
  This function is called any time the error marker is set.
*/
int NDICommandInterpreter::SetErrorCode(int errnum)
{
  m_ErrorCode = errnum;

  this->InvokeEvent( NDIErrorEvent(errnum) );

  return errnum;
}


/*---------------------------------------------------------------------
  PrintSelf function.
*/
void NDICommandInterpreter::PrintSelf( std::ostream& os,
                                       itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Tracking: " << m_Tracking << std::endl;
  os << indent << "ErrorCode: " << m_ErrorCode << std::endl;
}


}

