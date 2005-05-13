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
#define NDI_MAX_COMMAND_SIZE 2047

namespace igstk
{
/*---------------------------------------------------------------------*/
NDICommandInterpreter::NDICommandInterpreter()
{
  m_Communication = 0;

  m_SerialCommand = new char[NDI_MAX_COMMAND_SIZE+1];
  m_SerialReply = new char[NDI_MAX_COMMAND_SIZE+1];
  m_CommandReply = new char[NDI_MAX_COMMAND_SIZE+1];

  m_Tracking = 0;
  m_ErrorCode = 0;
}

/*---------------------------------------------------------------------*/
NDICommandInterpreter::~NDICommandInterpreter()
{
  delete [] m_SerialCommand;
  delete [] m_SerialReply;
  delete [] m_CommandReply;
}

/*---------------------------------------------------------------------*/
unsigned long
NDICommandInterpreter::HexadecimalStringToUnsignedLong(const char *cp, int n)
{
  int i;
  unsigned long result = 0;
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

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::HexadecimalStringToInt(const char *cp, int n)
{
  int i;
  int result = 0;
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

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::SignedStringToInt(const char *cp, int n)
{
  int i;
  int c;
  int result = 0;
  int sign = 1;

  c = cp[0];

  if (c == '+')
    {
    sign = 1;
    }
  else if (c == '-')
    {
    sign = -1;
    }
  else
    {
    return 0;
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

/*---------------------------------------------------------------------*/
char *NDICommandInterpreter::HexEncode(char *cp, const void *data, int n)
{
  const unsigned char *bdata;
  int i, c1, c2;
  unsigned int d;
  char *tcp;

  bdata = (const unsigned char *)data;
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

/*---------------------------------------------------------------------*/
void *NDICommandInterpreter::HexDecode(void *data, const char *cp, int n)
{
  unsigned char *bdata;
  int i, c1, c2;
  unsigned int d;

  bdata = (unsigned char *)data;

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

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::PVWRFromFile(int port, const char *filename)
{
  unsigned char buffer[1024];
  char hexdata[128];
  FILE *file;
  int addr;

  m_ErrorCode = 0;

  file = fopen(filename,"rb");
  if (file == NULL)
    {
    return -1;
    }

  memset(buffer, 0, 1024);      /* clear buffer to zero */
  fread(buffer, 1, 1024, file); /* read at most 1k from file */
  if (ferror(file))
    {
    fclose(file);
    return -1;
    }
  fclose(file);

  for (addr = 0; addr < 1024; addr += 64)
    { /* write in chunks of 64 bytes */
    this->PVWR(port, addr, this->HexEncode(hexdata, &buffer[addr], 64));
    if (this->GetError() != NDI_OKAY)
      {
      return -1;
      }
    }
 
  return 0;
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetError() const
{
  return m_ErrorCode;
}

/*---------------------------------------------------------------------*/
const char *NDICommandInterpreter::ErrorString(int errnum)
{
  static char *textarrayLow[] = /* values from 0x01 to 0x21 */ 
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
  };

  static char *textarrayHigh[] = /* values from 0xf6 to 0xf4 */
  {
    "Too much environmental infrared",
    "Unrecognized error code",
    "Unrecognized error code",
    "Unable to read Flash EPROM",
    "Unable to write Flash EPROM",
    "Unable to erase Flash EPROM"
  };

  static char *textarrayApi[] = /* values specific to the API */
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

  if (errnum >= 0x00 && errnum <= 0x31)
    {
    return textarrayLow[errnum];
    }
  else if (errnum <= 0xf6 && errnum >= 0xf1)
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
static const int oddparity[16] =    { 0, 1, 1, 0, 1, 0, 0, 1,
                                      1, 0, 0, 1, 0, 1, 1, 0 };

#define CalcCRC16(nextchar, puCRC16) \
{ \
    int data; \
    data = nextchar; \
    data = (data ^ (*(puCRC16) & 0xff)) & 0xff; \
    *puCRC16 >>= 8; \
    if ( oddparity[data & 0x0f] ^ oddparity[data >> 4] ) \
      { \
      *(puCRC16) ^= 0xc001; \
      } /* if */ \
    data <<= 6; \
    *puCRC16 ^= data; \
    data <<= 1; \
    *puCRC16 ^= data; \
} /* CalcCRC16 */ 

/*---------------------------------------------------------------------*/
const char *NDICommandInterpreter::Command(const char *command)
{
  int i, m, nc;
  unsigned int CRC16 = 0;
  int useCRC = 0;
  int inCommand = 1;
  char *cp, *rp, *crp;

  cp = m_SerialCommand;      /* text sent to ndicapi */
  rp = m_SerialReply;        /* text received from ndicapi */
  crp = m_CommandReply;      /* received text, with CRC hacked off */
  nc = 0;                    /* length of 'command' part of command */

  m_ErrorCode = 0;           /* clear error */
  cp[0] = '\0';
  rp[0] = '\0';
  crp[0] = '\0';

  /* if the command is NULL, send a break to reset the Measurement System */
  if (command == NULL)
    {
    m_Tracking = 0;

    /* need to replace with SerialCommunication calls */
    // ndiSerialComm(pol->serial_device, 9600, "8N1", 0);
    // ndiSerialFlush(pol->serial_device, NDI_IOFLUSH);
    // ndiSerialBreak(pol->serial_device);
    // m = ndiSerialRead(pol->serial_device, rp, 2047);

    /* check for correct reply */
    if (strncmp(rp, "RESETBE6F\r", 8) != 0)
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
    }
  else if (nc == 6 && strncmp(cp, "TSTART", nc) == 0)
    {
    m_Tracking = 1;
    }

  /* flush the input buffer, because anything that we haven't read
       yet is garbage left over by a previously failed command */
  m_Communication->Flush();

  /* send the command to the device */
  if (!m_Communication->SendString(cp))
    {
    errcode = NDI_WRITE_ERROR;
    }

  /* here is the old code from ndicapi.c */
  //if (errcode == 0) {
  //  m = ndiSerialWrite(pol->serial_device, cp, i);
  //  if (m < 0) {
  //    errcode = NDI_WRITE_ERROR;
  //  }
  //  else if (m < i) {
  //    errcode = NDI_TIMEOUT;
  //  }
  //}

  /* read the reply from the device */
  if (errcode == 0)
    {
    if (!m_Communication->ReceiveString(rp))
      {
      errcode = NDI_WRITE_ERROR;
      }
    }
  m = strlen(rp);

  /* here is the old code from ndicapi.c */
  //m = 0;
  //if (errcode == 0) {
  //m = ndiSerialRead(pol->serial_device, rp, 2047);
  //  if (m < 0) {
  //    errcode = NDI_WRITE_ERROR;
  //    m = 0;
  //  }
  //  else if (m == 0) {
  //    errcode = NDI_TIMEOUT;
  //  }
  //  rp[m] = '\0';   /* terminate string */
  //}

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
  if (CRC16 != this->HexadecimalStringToUnsignedLong(&rp[m], 4)) {
    this->SetErrorCode(NDI_BAD_CRC);
    return crp;
  }

  /* check for error code */
  if (crp[0] == 'E' && strncmp(crp, "ERROR", 5) == 0)
    {
    this->SetErrorCode(this->HexadecimalStringToUnsignedLong(&crp[5], 2));
    return crp;
    }

  /*----------------------------------------*/
  /* special behavior for specific commands */

  if (cp[0] == 'T' && cp[1] == 'X' && nc == 2)
    { /* the TX command */
    HelperForTX(cp, crp);
    }
  else if (cp[0] == 'G' && cp[1] == 'X' && nc == 2)
    { /* the GX command */
    HelperForGX(cp, crp);
    }
  else if (cp[0] == 'C' && nc == 4 && strncmp(cp, "COMM", nc) == 0)
    {
    HelperForCOMM(cp, crp);
    }
  else if (cp[0] == 'I' && nc == 4 && strncmp(cp, "INIT", nc) == 0)
    {
    HelperForINIT(cp, crp);
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
  else if (cp[0] == 'P' && nc == 5 && strncmp(cp, "PSTAT", nc) == 0)
    {
    HelperForPSTAT(cp, crp);
    }
  else if (cp[0] == 'S' && nc == 5 && strncmp(cp, "SSTAT", nc) == 0)
    {
    HelperForSSTAT(cp, crp);
    }

  /* return the device's reply, but with the CRC hacked off */
  return crp;
}

/*---------------------------------------------------------------------*/
const char *NDICommandInterpreter::Command(const char *format, int a)
{
  sprintf(m_SerialCommand, format, a);
  return this->Command(m_SerialCommand);
}

/*---------------------------------------------------------------------*/
const char *NDICommandInterpreter::Command(const char *format, int a, int b)
{
  sprintf(m_SerialCommand, format, a, b);
  return this->Command(m_SerialCommand);
}

/*---------------------------------------------------------------------*/
const char *NDICommandInterpreter::Command(const char *format, int a, int b,
                                           int c)
{
  sprintf(m_SerialCommand, format, a, b, c);
  return this->Command(m_SerialCommand);
}

/*---------------------------------------------------------------------*/
const char *NDICommandInterpreter::Command(const char *format, int a, int b,
                                           int c, int d)
{
  sprintf(m_SerialCommand, format, a, b, c, d);
  return this->Command(m_SerialCommand);
}

/*---------------------------------------------------------------------*/
const char *NDICommandInterpreter::Command(const char *format, int a, int b,
                                           const char *c)
{
  sprintf(m_SerialCommand, format, a, b, c);
  return this->Command(m_SerialCommand);
}

/*---------------------------------------------------------------------*/
const char *NDICommandInterpreter::Command(const char *format, const char *a,
                                           const char *b, const char *c,
                                           const char *d, const char *e)
{
  sprintf(m_SerialCommand, format, a, b, c, d, e);
  return this->Command(m_SerialCommand);
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetPHINFPortStatus() const
{
  const char *dp;

  dp = &m_PHINFBasic[31];

  return this->HexadecimalStringToInt(dp, 2);  
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetPHINFToolInfo(char information[31]) const
{
  const char *dp;
  int i;
  
  dp = m_PHINFBasic;

  for (i = 0; i < 31; i++)
    {
    information[i] = *dp++;
    }

  return m_PHINFUnoccupied;
}

/*---------------------------------------------------------------------*/
unsigned long NDICommandInterpreter::GetPHINFCurrentTest() const
{
  const char *dp;

  dp = m_PHINFTesting;

  return this->HexadecimalStringToInt(dp, 8);  
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetPHINFPartNumber(char part[20]) const
{
  const char *dp;
  int i;
  
  dp = m_PHINFPartNumber;

  for (i = 0; i < 20; i++)
    {
    part[i] = *dp++;
    }

  return m_PHINFUnoccupied;
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetPHINFAccessories() const
{
  const char *dp;

  dp = m_PHINFAccessories;

  return this->HexadecimalStringToInt(dp, 2);  
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetPHINFMarkerType() const
{
  const char *dp;

  dp = m_PHINFMarkerType;

  return this->HexadecimalStringToInt(dp, 2);  
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetPHINFPortLocation(char location[14]) const
{
  const char *dp;
  int i;
  
  dp = m_PHINFPortLocation;

  for (i = 0; i < 14; i++)
    {
    location[i] = *dp++;
    }

  return m_PHINFUnoccupied;
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetPHINFGPIOStatus() const
{
  const char *dp;

  dp = m_PHINFGPIOStatus;

  return this->HexadecimalStringToInt(dp, 2);  
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetPHRQHandle() const
{
  const char *dp;

  dp = m_PHRQReply;
  return this->HexadecimalStringToInt(dp, 2);
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetPHSRNumberOfHandles() const
{
  const char *dp;

  dp = m_PHSRReply;

  return this->HexadecimalStringToInt(dp, 2);  
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetPHSRHandle(int i) const
{
  const char *dp;
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

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetPHSRInformation(int i) const
{
  const char *dp;
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

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetTXTransform(int ph, double transform[8]) const
{
  const char *dp;
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

  return NDI_OKAY;
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetTXPortStatus(int ph) const
{
  const char *dp;
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

/*---------------------------------------------------------------------*/
unsigned long NDICommandInterpreter::GetTXFrame(int ph) const
{
  const char *dp;
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

  return this->HexadecimalStringToUnsignedLong(dp, 8);
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetTXToolInfo(int ph) const
{
  const char *dp;
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

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetTXMarkerInfo(int ph, int marker) const
{
  const char *dp;
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

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetTXSingleStray(int ph, double coord[3]) const
{
  const char *dp;
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

  dp = m_TXSingleStray[i];
  if (*dp == 'D' || *dp == '\0')
    {
    return NDI_DISABLED;
    }
  else if (*dp == 'M')
    {
    return NDI_MISSING;
    }

  coord[0] = this->SignedStringToInt(&dp[0],  7)*0.01;
  coord[1] = this->SignedStringToInt(&dp[7],  7)*0.01;
  coord[2] = this->SignedStringToInt(&dp[14], 7)*0.01;

  return NDI_OKAY;
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetTXNumberOfPassiveStrays() const
{
  return m_TXNumberOfPassiveStrays; 
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetTXPassiveStray(int i, double coord[3]) const
{
  const char *dp;
  int n;

  dp = m_TXPassiveStray;

  if (*dp == '\0')
    {
    return NDI_DISABLED;
    }

  n = m_TXNumberOfPassiveStrays;
  dp += 3;
  if (n < 0)
    {
    return NDI_MISSING;
    }
  if (n > 20)
    {
    n = 20;
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

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetTXSystemStatus() const
{
  const char *dp;

  dp = m_TXSystemStatus;

  return this->HexadecimalStringToInt(dp, 4);
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetGXTransform(int port, double transform[8]) const
{
  const char *dp;
  
  if (port >= '1' && port <= '3')
    {
    dp = m_GXTransforms[port - '1'];
    }
  else if (port >= 'A' && port <= 'I')
    {
    dp = m_GXPassiveTransforms[port - 'A'];
    }
  else
    {
    return NDI_DISABLED;
    }

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

  return NDI_OKAY;
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetGXPortStatus(int port) const
{
  const char *dp;

  if (port >= '1' && port <= '3')
    {
    dp = &m_GXStatus[6 - 2*(port - '1')];
    }
  else if (port >= 'A' && port <= 'C')
    {
    dp = &m_GXPassiveStatus[6 - 2*(port - 'A')];
    }
  else if (port >= 'D' && port <= 'F')
    {
    dp = &m_GXPassiveStatus[14 - 2*(port - 'D')];
    }
  else if (port >= 'G' && port <= 'I')
    {
    dp = &m_GXPassiveStatus[22 - 2*(port - 'G')];
    }
  else
    {
    return 0;
    }

  return this->HexadecimalStringToInt(dp, 2);
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetGXSystemStatus() const
{
  const char *dp;

  dp = m_GXStatus;

  if (*dp == '\0')
    {
    dp = m_GXPassiveStatus;
    }

  return this->HexadecimalStringToInt(dp, 2);
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetGXToolInfo(int port) const
{
  const char *dp;

  if (port >= '1' && port <= '3')
    {
    dp = m_GXInformation[port - '1'];
    }
  else if (port >= 'A' && port <= 'I')
    {
    dp = m_GXPassiveInformation[port - 'A'];
    }
  else
    {
    return 0;
    }

  return this->HexadecimalStringToInt(dp, 2);
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetGXMarkerInfo(int port, int marker) const
{
  const char *dp;

  if (marker < 'A' || marker > 'T')
    {
    return 0;
    }

  if (port >= '1' && port <= '3')
    {
    dp = m_GXInformation[port - '1'];
    }
  else if (port >= 'A' && port <= 'I')
    {
    dp = m_GXPassiveInformation[port - 'A'];
    }
  else
    {
    return 0;
    }
  dp += 11 - (marker - 'A');

  return this->HexadecimalStringToInt(dp, 1);
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetGXSingleStray(int port, double coord[3]) const
{
  const char *dp;
  
  if (port >= '1' && port <= '3')
    {
    dp = m_GXSingleStray[port - '1'];
    }
  else
    {
    return NDI_DISABLED;
    }

  if (*dp == 'D' || *dp == '\0')
    {
    return NDI_DISABLED;
    }
  else if (*dp == 'M')
    {
    return NDI_MISSING;
    }

  coord[0] = this->SignedStringToInt(&dp[0],  7)*0.01;
  coord[1] = this->SignedStringToInt(&dp[7],  7)*0.01;
  coord[2] = this->SignedStringToInt(&dp[14], 7)*0.01;

  return NDI_OKAY;
}

/*---------------------------------------------------------------------*/
unsigned long NDICommandInterpreter::GetGXFrame(int port) const
{
  const char *dp;

  if (port >= '1' && port <= '3')
    {
    dp = m_GXFrame[port - '1'];
    }
  else if (port >= 'A' && port <= 'I')
    {
    dp = m_GXPassiveFrame[port - 'A'];
    }
  else
    {
    return 0;
    }

  return this->HexadecimalStringToUnsignedLong(dp, 8);
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetGXNumberOfPassiveStrays() const
{
  const char *dp;
  int n;

  dp = m_GXPassiveStray;

  if (*dp == '\0')
    {
    return 0;
    }
  
  n = this->SignedStringToInt(dp, 3);
  if (n < 0)
    {
    return 0;
    }
  if (n > 20)
    {
    return 20;
    }

  return n; 
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetGXPassiveStray(int i, double coord[3]) const
{
  const char *dp;
  int n;

  dp = m_GXPassiveStray;

  if (*dp == '\0')
    {
    return NDI_DISABLED;
    }

  n = this->SignedStringToInt(dp, 3);
  dp += 3;
  if (n < 0)
    {
    return NDI_MISSING;
    }
  if (n > 20)
    {
    n = 20;
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

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetPSTATPortStatus(int port) const
{
  const char *dp;

  if (port >= '1' && port <= '3')
    {
    dp = m_PSTATBasic[port - '1'];
    }
  else if (port >= 'A' && port <= 'I')
    {
    dp = m_PSTATPassiveBasic[port - 'A'];
    }
  else
    {
    return 0;
    }

  /* the 'U' is for UNOCCUPIED */
  if (*dp == 'U' || *dp == '\0')
    {
    return 0;
    }

  /* skip to the last two characters */
  dp += 30;

  return this->HexadecimalStringToInt(dp, 2);
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetPSTATToolInfo(int port,
                                            char information[30]) const
{
  const char *dp;

  if (port >= '1' && port <= '3')
    {
    dp = m_PSTATBasic[port - '1'];
    }
  else if (port >= 'A' && port <= 'I')
    {
    dp = m_PSTATPassiveBasic[port - 'A'];
    }
  else
    {
    return NDI_UNOCCUPIED;
    }

  /* the 'U' is for UNOCCUPIED */
  if (*dp == 'U' || *dp == '\0')
    {
    return NDI_UNOCCUPIED;
    }

  strncpy(information, dp, 30);

  return NDI_OKAY;
}

/*---------------------------------------------------------------------*/
unsigned long NDICommandInterpreter::GetPSTATCurrentTest(int port) const
{
  const char *dp;

  if (port >= '1' && port <= '3')
    {
    dp = m_PSTATTesting[port - '1'];
    }
  else if (port >= 'A' && port <= 'I')
    {
    dp = m_PSTATPassiveTesting[port - 'A'];
    }
  else
    {
    return 0;
    }

  if (*dp == '\0')
    {
    return 0;
    }

  return this->HexadecimalStringToUnsignedLong(dp, 8);
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetPSTATPartNumber(int port, char part[20]) const
{
  const char *dp;

  if (port >= '1' && port <= '3')
    {
    dp = m_PSTATPartNumber[port - '1'];
    }
  else if (port >= 'A' && port <= 'I')
    {
    dp = m_PSTATPassivePartNumber[port - 'A'];
    }
  else
    {
    return NDI_UNOCCUPIED;
    }

  if (*dp == '\0')
    {
    return NDI_UNOCCUPIED;
    }

  strncpy(part, dp, 20);

  return NDI_OKAY;
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetPSTATAccessories(int port) const
{
  const char *dp;

  if (port >= '1' && port <= '3')
    {
    dp = m_PSTATAccessories[port - '1'];
    }
  else if (port >= 'A' && port <= 'I')
    {
    dp = m_PSTATPassiveAccessories[port - 'A'];
    }
  else
    {
    return 0;
    }

  if (*dp == '\0')
    {
    return 0;
    }

  return this->HexadecimalStringToInt(dp, 2);
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetPSTATMarkerType(int port) const
{
  const char *dp;

  if (port >= '1' && port <= '3')
    {
    dp = m_PSTATMarkerType[port - '1'];
    }
  else if (port >= 'A' && port <= 'I')
    {
    dp = m_PSTATPassiveMarkerType[port - 'A'];
    }
  else
    {
    return 0;
    }

  if (*dp == '\0')
    {
    return 0;
    }

  return this->HexadecimalStringToInt(dp, 2);
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetSSTATControl() const
{
  const char *dp;

  dp = m_SSTATControl;

  if (*dp == '\0')
    {
    return 0;
    }

  return this->HexadecimalStringToInt(dp, 2);
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetSSTATSensors() const
{
  const char *dp;

  dp = m_SSTATSensor;

  if (*dp == '\0')
    {
    return 0;
    }

  return this->HexadecimalStringToInt(dp, 2);
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetSSTATTIU() const
{
  const char *dp;

  dp = m_SSTATTIU;

  if (*dp == '\0')
    {
    return 0;
    }

  return this->HexadecimalStringToInt(dp, 2);
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetIRCHKDetected() const
{
  if (m_IRCHKDetected == '1')
    {
    return 1;
    }
  return 0;
}

/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetIRCHKNumberOfSources(int side) const
{
  const char *dp;
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
 
/*---------------------------------------------------------------------*/
int NDICommandInterpreter::GetIRCHKSourceXY(int side, int i,
                                            double xy[2]) const
{
  const char *dp;
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
void NDICommandInterpreter::HelperForPHINF(const char *cp, const char *crp)
{
  unsigned long mode = 0x0001; /* the default reply mode */
  char *dp;
  int j;
  int unoccupied = NDI_OKAY;

  /* if the PHINF command had a reply mode, read it */
  if ((cp[5] == ':' && cp[10] != '\r') || (cp[5] == ' ' && cp[6] != '\r'))
    { 
    mode = this->HexadecimalStringToUnsignedLong(&cp[8], 4);
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
void NDICommandInterpreter::HelperForPHRQ(const char *cp, const char *crp)
{
  char *dp;
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
void NDICommandInterpreter::HelperForPHSR(const char *cp, const char *crp)
{
  char *dp;
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
void NDICommandInterpreter::HelperForTX(const char *cp, const char *crp)
{
  unsigned long mode = 0x0001; /* the default reply mode */
  char *dp;
  int i, j, n;
  int ph, nhandles, nstray;

  /* if the TX command had a reply mode, read it */
  if ((cp[2] == ':' && cp[7] != '\r') || (cp[2] == ' ' && cp[3] != '\r'))
    { 
    mode = this->HexadecimalStringToUnsignedLong(&cp[3], 4);
    }

  /* get the number of handles */
  nhandles = this->HexadecimalStringToInt(crp, 2);
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
      for (j = 0; j < 20 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      }

    /* grab the single marker info */ 
    if (mode & NDI_SINGLE_STRAY)
      {
      dp = m_TXSingleStray[i];
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
        /* read the single stray position */
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
    nstray = this->SignedStringToInt(crp, 3);
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
  Copy all the GX reply information into the ndicapi structure, according
  to the GX reply mode that was requested.

  This function is called every time a GX command is sent to the
  Measurement System.

  This information can be later extracted through one of the ndiGetGXxx()
  functions.
*/
void NDICommandInterpreter::HelperForGX(const char *cp, const char *crp)
{
  unsigned long mode = 0x0001; /* the default reply mode */
  char *dp;
  int i, j, k;
  int npassive, nactive;

  /* if the GX command had a reply mode, read it */
  if ((cp[2] == ':' && cp[7] != '\r') || (cp[2] == ' ' && cp[3] != '\r'))
    { 
    mode = this->HexadecimalStringToUnsignedLong(&cp[3], 4);
    }

  /* always three active ports */
  nactive = 3;

  if (mode & NDI_XFORMS_AND_STATUS)
    {
    for (k = 0; k < nactive; k += 3)
      {
      /* grab the three transforms */
      for (i = 0; i < 3; i++)
        {
        dp = m_GXTransforms[i];
        for (j = 0; j < 51 && *crp >= ' '; j++)
          {
          *dp++ = *crp++;
          }
        *dp = '\0';
        /* fprintf(stderr, "xf %.51s\n", m_GXTransforms[i]); */
        /* eat the trailing newline */
        if (*crp == '\n')
          {
          crp++;
          }
        }
      /* grab the status flags */
      dp = m_GXStatus + k/3*8;
      for (j = 0; j < 8 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      /* fprintf(stderr, "st %.8s\n", m_GXStatus); */
      }
    /* eat the trailing newline */
    if (*crp == '\n')
      {
      crp++;
      }
    }

  if (mode & NDI_ADDITIONAL_INFO)
    {
    /* grab information for each port */
    for (i = 0; i < nactive; i++)
      {
      dp = m_GXInformation[i];
      for (j = 0; j < 12 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      /* fprintf(stderr, "ai %.12s\n", m_GXInformation[i]); */
      }
    /* eat the trailing newline */
    if (*crp == '\n')
      {
      crp++;
      }
    }

  if (mode & NDI_SINGLE_STRAY)
    {
    /* grab stray marker for each port */
    for (i = 0; i < nactive; i++)
      {
      dp = m_GXSingleStray[i];
      for (j = 0; j < 21 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      *dp = '\0';
      /* fprintf(stderr, "ss %.21s\n", m_GXSingleStray[i]); */      
      /* eat the trailing newline */
      if (*crp == '\n')
        {
        crp++;
        }
      }
    }

  if (mode & NDI_FRAME_NUMBER)
    {
    /* get frame number for each port */
    for (i = 0; i < nactive; i++)
      {
      dp = m_GXFrame[i];
      for (j = 0; j < 8 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      /* fprintf(stderr, "fn %.8s\n", m_GXFrame[i]); */
    }
    /* eat the trailing newline */
    if (*crp == '\n')
      {
      crp++;
      }
    }

  /* if there is no passive information, stop here */
  if (!(mode & NDI_PASSIVE))
    {
    return;
    }

  /* in case there are 9 passive tools instead of just 3 */
  npassive = 3;
  if (mode & NDI_PASSIVE_EXTRA)
    {
    npassive = 9;
    }

  if ((mode & NDI_XFORMS_AND_STATUS) || (mode == NDI_PASSIVE))
    {
    /* the information is grouped in threes */
    for (k = 0; k < npassive; k += 3)
      {
      /* grab the three transforms */
      for (i = 0; i < 3; i++)
        {
        dp = m_GXPassiveTransforms[k+i];
        for (j = 0; j < 51 && *crp >= ' '; j++)
          {
          *dp++ = *crp++;
          }
        *dp = '\0';
        /* fprintf(stderr, "pxf %.31s\n", m_GXPassiveTransforms[k+i]); */
        /* eat the trailing newline */
        if (*crp == '\n')
          {
          crp++;
          }
        }
      /* grab the status flags */
      dp = m_GXPassiveStatus + k/3*8;
      for (j = 0; j < 8 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      /* fprintf(stderr, "pst %.8s\n", m_GXPassiveStatus + k/3*8); */
      /* skip the newline */
      if (*crp == '\n')
        {
        crp++;
        }
      else
        { /* no newline: no more passive transforms */
        npassive = k + 3;
        }
      }
    /* eat the trailing newline */
    if (*crp == '\n')
      {
      crp++;
      }
    }

  if (mode & NDI_ADDITIONAL_INFO)
    {
    /* grab information for each port */
    for (i = 0; i < npassive; i++)
      {
      dp = m_GXPassiveInformation[i];
      for (j = 0; j < 12 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      /* fprintf(stderr, "pai %.12s\n", m_GXPassiveInformation[i]); */
      }
    /* eat the trailing newline */
    if (*crp == '\n')
      {
      crp++;
      }
    }

  if (mode & NDI_FRAME_NUMBER)
    {
    /* get frame number for each port */
    for (i = 0; i < npassive; i++)
      {
      dp = m_GXPassiveFrame[i];
      for (j = 0; j < 8 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      /* fprintf(stderr, "pfn %.8s\n", m_GXPassiveFrame[i]); */      
      }
    /* eat the trailing newline */
    if (*crp == '\n')
      {
      crp++;
      }
    }

  if (mode & NDI_PASSIVE_STRAY)
    {
    /* get all the passive stray information */
    /* this will be a maximum of 3 + 20*3*7 = 423 bytes */
    dp = m_GXPassiveStray;
    for (j = 0; j < 423 && *crp >= ' '; j++)
      {
      *dp++ = *crp++;
      }
    /* fprintf(stderr, "psm %s\n", m_GXPassiveStray); */
    }
}

/*---------------------------------------------------------------------
  Copy all the PSTAT reply information into the ndicapi structure.
*/
void NDICommandInterpreter::HelperForPSTAT(const char *cp, const char *crp)
{
  unsigned long mode = 0x0001; /* the default reply mode */
  char *dp;
  int i, j;
  int npassive, nactive;

  /* if the PSTAT command had a reply mode, read it */
  if ((cp[5] == ':' && cp[10] != '\r') || (cp[5] == ' ' && cp[6] != '\r'))
    { 
    mode = this->HexadecimalStringToUnsignedLong(&cp[6], 4);
    }

  /* always three active ports */
  nactive = 3;

  /* information for each port is separated by a newline */
  for (i = 0; i < nactive; i++)
    {
    /* basic tool information and port status */
    if (mode & NDI_BASIC)
      {
      dp = m_PSTATBasic[i];
      for (j = 0; j < 32 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      /* terminate if UNOCCUPIED */
      if (j < 32)
        {
        *dp = '\0';
        }
      /* fprintf(stderr, "ba %.32s\n", m_pstat_basic[i]); */
      }

    /* current testing */
    if (mode & NDI_TESTING)
      {
      dp = m_PSTATTesting[i];
      *dp = '\0';
      for (j = 0; j < 8 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      /* fprintf(stderr, "ai %.8s\n", m_pstat_testing[i]); */
      }

    /* part number */
    if (mode & NDI_PART_NUMBER)
      {
      dp = m_PSTATPartNumber[i];
      *dp = '\0';
      for (j = 0; j < 20 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      /* fprintf(stderr, "pn %.20s\n", m_pstat_part_number[i]); */
      }
    
    /* accessories */
    if (mode & NDI_ACCESSORIES)
      {
      dp = m_PSTATAccessories[i];
      *dp = '\0';
      for (j = 0; j < 2 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      /* fprintf(stderr, "ac %.2s\n", m_pstat_accessories[i]); */
      }

    /* marker type */
    if (mode & NDI_MARKER_TYPE)
      {
      dp = m_PSTATMarkerType[i];
      *dp = '\0';
      for (j = 0; j < 2 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      /* fprintf(stderr, "mt %.2s\n", m_pstat_marker_type[i]); */
      }

    /* skip any other information that might be present */
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

  /* if there is no passive information, stop here */
  if (!(mode & NDI_PASSIVE))
    {
    return;
    }

  /* in case there are 9 passive tools instead of just 3 */
  npassive = 3;
  if (mode & NDI_PASSIVE_EXTRA)
    {
    npassive = 9;
    }

  /* information for each port is separated by a newline */
  for (i = 0; i < npassive; i++)
    {
    /* basic tool information and port status */
    if (mode & NDI_BASIC)
      {
      dp = m_PSTATPassiveBasic[i];
      *dp = '\0';
      for (j = 0; j < 32 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      /* terminate if UNOCCUPIED */
      if (j < 32)
        {
        *dp = '\0';
        }
      /* fprintf(stderr, "pba %.32s\n", m_pstat_passive_basic[i]); */
      }

    /* current testing */
    if (mode & NDI_TESTING)
      {
      dp = m_PSTATPassiveTesting[i];
      *dp = '\0';
      for (j = 0; j < 8 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      /* fprintf(stderr, "pai %.8s\n", m_pstat_passive_testing[i]); */
      }

    /* part number */
    if (mode & NDI_PART_NUMBER)
      {
      dp = m_PSTATPassivePartNumber[i];
      *dp = '\0';
      for (j = 0; j < 20 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      /* fprintf(stderr, "ppn %.20s\n", m_pstat_passive_part_number[i]); */
      }
    
    /* accessories */
    if (mode & NDI_ACCESSORIES)
      {
      dp = m_PSTATPassiveAccessories[i];
      *dp = '\0';
      for (j = 0; j < 2 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      /* fprintf(stderr, "pac %.2s\n", m_pstat_passive_accessories[i]); */
      }

    /* marker type */
    if (mode & NDI_MARKER_TYPE)
      {
      dp = m_PSTATPassiveMarkerType[i];
      *dp = '\0';
      for (j = 0; j < 2 && *crp >= ' '; j++)
        {
        *dp++ = *crp++;
        }
      /* fprintf(stderr, "pmt %.2s\n", m_pstat_passive_marker_type[i]); */
      }

    /* skip any other information that might be present */
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
  Copy all the SSTAT reply information into the ndicapi structure.
*/
void NDICommandInterpreter::HelperForSSTAT(const char *cp, const char *crp)
{
  unsigned long mode;
  char *dp;

  /* read the SSTAT command reply mode */
  mode = this->HexadecimalStringToUnsignedLong(&cp[6], 4);

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
void NDICommandInterpreter::HelperForIRCHK(const char *cp, const char *crp)
{
  unsigned long mode = 0x0001; /* the default reply mode */
  int j;

  /* if the IRCHK command had a reply mode, read it */
  if ((cp[5] == ':' && cp[10] != '\r') || (cp[5] == ' ' && cp[6] != '\r'))
    { 
    mode = this->HexadecimalStringToUnsignedLong(&cp[6], 4);
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
void NDICommandInterpreter::HelperForCOMM(const char *cp, const char *crp)
{
  static int convert_baud[6] = { 9600, 14400, 19200, 38400, 57600, 115200 };
  char newdps[4] = "8N1";
  int newspeed = 9600;
  int newhand = 0;

  if (cp[5] >= '0' && cp[5] <= '5')
    {
    newspeed = convert_baud[cp[5]-'0'];
    }
  if (cp[6] == '1')
    {
    newdps[0] = '7';
    }
  if (cp[7] == '1')
    {
    newdps[1] = 'O';
    }
  else if (cp[7] == '2')
    {
    newdps[1] = 'E';
    }
  if (cp[8] == '1')
    {
    newdps[2] = '2';
    }
  if (cp[9] == '1')
    {
    newhand = 1;
    } 

  /* this code must be replace with SerialCommunication calls */
  // ndiSerialSleep(pol->serial_device, 100);  /* let the device adjust  */
  //if (ndiSerialComm(pol->serial_device, newspeed, newdps, newhand) != 0) {
  //  this->SetErrorCode(NDI_BAD_COMM);
  //}
}

/*---------------------------------------------------------------------
  Sleep for 100 milliseconds after an INIT command.
*/
void NDICommandInterpreter::HelperForINIT(const char *cp, const char *crp)
{
  /* need to go to sleep for 100 milliseconds somehow */
  //ndiSerialSleep(serial_device, 100);
}

/*---------------------------------------------------------------------
  This function is called any time the error marker is set.
*/
int NDICommandInterpreter::SetErrorCode(int errnum)
{
  m_ErrorCode = errnum;

  return errnum;
}

}

