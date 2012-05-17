/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAscensionCommandInterpreter.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================

Copyright (c) 2000-2005 Atamai, Inc.

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
   form, must retain the above copyright notice, this license,
   the following disclaimer, and any notices that refer to this
   license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
   notice, a copy of this license and the following disclaimer
   in the documentation or with other materials provided with the
   distribution.

3) Modified copies of the source code must be clearly marked as such,
   and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=======================================================================*/

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "igstkAscensionCommandInterpreter.h"

namespace igstk
{

/** Get the number of data words for a particular data format. */
inline int ComputeRecordSize(AscensionDataFormat format)
{
  static int formatDataSizeTable[8] = { 3, 3, 9, 6, 12, 0, 4, 7 };

  return formatDataSizeTable[(format) - FB_POSITION];
}

/** Get the maximum allowed parameter, given the revision number. 
 *  Different revisions of the Flock of Birds hardware support different
 *  paramters. */
inline AscensionParameter GetMaxParameterForRevision(int revision)
{
  if (revision >= (3 << 8) + 71)
    {
    return FB_XMTR_SERIAL_NUMBER;
    }
  if (revision >= (3 << 8) + 67)
    {
    return FB_SERIAL_NUMBER;
    }
  if (revision >= (3 << 8) + 66)
    {
    return FB_TRANSMITTER_MODE;
    }
  if (revision >= (3 << 8) + 45)
    {
    return FB_REFERENCE_FRAME;
    }
  return FB_ERROR_CODE_EXPANDED;
} 

/** Constructor */
AscensionCommandInterpreter::AscensionCommandInterpreter()
                                                 :m_StateMachine(this)
{
  m_Error = FB_NO_ERROR;
  m_PointData = false;
  m_StreamData = false;
  m_ErrorText = "";
  m_GroupMode = false;
  m_CurrentBird = 1;
  m_NumberOfBirds = 1;
  m_DataFormat[1] = FB_POSITION_QUATERNION;
  m_ButtonMode[1] = false;
  m_PositionScale[1] = FB_STANDARD; 
  m_FBBAddress = 1;
  m_PhaseErrorLeftoverBytes = 0;
  m_Communication = 0;
  m_MaxParameter = FB_POSITION_SCALING;
}

/** Desctructor */
AscensionCommandInterpreter::~AscensionCommandInterpreter()
{
}


/** Set the communication object to use. */
void AscensionCommandInterpreter
::SetCommunication(CommunicationType* communication)
{
  m_Communication = communication;
}

/** Get the communication object. */
AscensionCommandInterpreter::CommunicationType *
AscensionCommandInterpreter::GetCommunication()
{
  return m_Communication;
}

/** SetupMethods Connecting to the Flock
 *  The flock of birds must be connected to one of the serial ports on the
 *  computer.  The device name for the serial ports are different for
 *  different operating systems, the DeviceName() function can be used
 *  to generate an appropriate device name for the host OS 
 *
 *  Open communication between the computer and the flock.  This will
 *  also probe the flock for information such as the addressing mode.
 *  The serial port is set up to do XON/XOFF handshaking, and the RTS
 *  line is set to low.
 *  If it is unknown which baud rate the flock is set to, then Open() can
 *  be called repeatedly with different baud rates in order to probe for
 *  the flock. */
void AscensionCommandInterpreter::Open()
{
  /* insert code to open serial communication and set baud rate */

  /* Insert code to turn off the Flock by setting the RTS line */
  this->Reset();

  /* reset command will fail if there was no carrier detect */
  if (m_Error)
    {
    this->Close();
    this->SetErrorAndMessage(FB_COM_ERROR,
                             "no device connected to serial port");
    return;
    }

  int rev = this->ExamineValue(FB_REVISION);
  m_Revision = ((rev >> 8) & 0x00ff) | ((rev << 8) & 0xff00);

  m_MaxParameter = GetMaxParameterForRevision(m_Revision);
  if (!m_Error)
    {
    m_PositionScale[1] =
      AscensionPositionScale(this->ExamineValue(FB_POSITION_SCALING));
    }

  int status = 0;
  if (!m_Error)
    {
    status = this->ExamineValue(FB_STATUS);
    m_DataFormat[1] = 
      AscensionDataFormat(((status & FB_STATUS_FORMAT) >> 1) 
                             + (FB_POSITION-1));
    }

  if (!m_Error)
    {
    m_AddressMode = FB_NORMAL;
    if (m_Revision > (3 << 8) + 33)
      {
      if ((status & FB_STATUS_EXPANDED) != 0)
        {
        m_AddressMode = FB_EXPANDED;
        if (m_Revision > (3 << 8) + 67)
          {
          m_AddressMode =
            AscensionAddressMode(this->ExamineValue(FB_FBB_ADDRESS_MODE));
          }
        }
      }
    }

  if (m_Error)
    {
    this->Close();
    }
}

/** Shut down the flock and close communication */
void AscensionCommandInterpreter::Close()
{
  this->EndStreamIfStreaming();

  /* Insert code to turn off the Flock by setting the RTS line */
  this->Stop();

  /* Insert code to close the Serial Communiction */
}

void AscensionCommandInterpreter::Stop()
{
  /* Put system into sleep, stop transmitter */
  this->SendRaw("G",1);
}

void AscensionCommandInterpreter::Run()
{
  /* restart system */
  this->SendRaw("F",1);
}


/** Reset the bird that is connected to the serial port by toggling 
 *  the RTS line.
 *
 * Note that this does not reset the whole flock.  After Reset() is called,
 * FBBReset() can be called to reset the other birds. */
void AscensionCommandInterpreter::Reset()
{
  this->EndStreamIfStreaming();

  /* Insert code to set the RTS line high */
  m_Communication->SetRTS(1);

  /* Insert code to sleep for 1 second */
  m_Communication->Sleep(1000);

  /* Insert code to set the RTS line low */
  m_Communication->SetRTS(0);

  /* Insert code to sleep for 2 seconds */
  m_Communication->Sleep(2000);

  m_FBBAddress = 1;
  m_StreamData = false;
  m_PointData = false;
  m_PhaseErrorLeftoverBytes = 0;
  m_CurrentBird = 1;
  m_Error = FB_NO_ERROR;
  m_ErrorText = "";
  m_GroupMode = false;
  m_NumberOfBirds = 1;
  m_DataFormat[1] = FB_POSITION_QUATERNION;
  m_ButtonMode[1] = false;
  m_PositionScale[1] = FB_STANDARD; 
 
  /* Insert code to purse the serial port buffer */
}  

/** If more than one bird is used, then the flock of birds bus or FBB must
 *  be properly initialized through the use of FBBReset() followed by
 *  FBBAutoConfig().  As well, for commands that must be directed towards
 *  one specific bird the fbRS232ToFBB() function specifies what bird the
 * next command should be sent to. */

/** Reset the flock.  This should be called after Reset() has
 *  been used to reset the first bird in the flock. */
void AscensionCommandInterpreter::FBBReset()
{
  this->EndStreamIfStreaming();

  this->SendRaw("/",1);

  /* Insert code to sleep for 600 milliseconds */
  m_Communication->Sleep(600);
}

/** Configure the flock for the specified number of birds.
 *  \param num        the number of birds to configure for
 *  The number of birds connected to the flock can be determined
 *  by examining the FB_FBB_STATUS parameter via ExamineValueBytes().
 *  For each bird that is attached to the flock, the corresponding
 *  byte will be nonzero. */
void AscensionCommandInterpreter::FBBAutoConfig(unsigned int num)
{
  char text[3];

  m_Communication->Sleep(600);

  m_NumberOfBirds = num;
  for (unsigned int i = 1; i <= num; i++)
    {
    m_DataFormat[i] = FB_POSITION_QUATERNION;
    m_ButtonMode[i] = false;
    m_PositionScale[i] = FB_STANDARD; 
    }
  text[0] = FB_CHANGE_VALUE;
  text[1] = FB_FBB_AUTOCONFIG;
  text[2] = num;
  this->SendRaw(text,3);

  m_Communication->Sleep(600);
}  

/** Inform the flock that the next command is to be sent
 *  to a specific bird.  This is only applicable to certain
 *  commands.
 *  \param bird       the bird address (1 or greater) */
void AscensionCommandInterpreter::RS232ToFBB(unsigned int bird)
{
  char text[2];

  if (m_AddressMode == FB_SUPER_EXPANDED)
    {
    text[0] = FB_RS232_TO_FBB_SE;
    text[1] = bird;
    this->SendRaw(text,2);
    }
  else
    {
    if (bird > 15)
      {
      text[0] = FB_RS232_TO_FBB_E - 0x10 + bird;
      }
    else
      {
      text[0] = FB_RS232_TO_FBB + bird;
      }
    this->SendRaw(text,1);
    }

  m_FBBAddress = bird;
}

/** These methods are used to configure the data records that will be sent
 *  from the flock.  It is necessary to call these methods before data is
 *  requested from the flock, or the data format remains unspecified.
 */

/** Set the tracking hemisphere for the flock.  The flock will only
 *  correctly report the positions for birds within the specified
 * hemisphere.
 * \param hemisphere the hemisphere to use: one of FB_FORWARD,
 *                 FB_AFT, FB_UPPER, FB_LOWER, FB_LEFT, FB_RIGHT */
void AscensionCommandInterpreter::SetHemisphere(
  AscensionHemisphere hemisphere)
{
  short hemiShort = hemisphere;
  this->SendCommandWords(FB_HEMISPHERE,&hemiShort);
}
  
/** Set the data format that will be used by the flock.  This must
 *  be set before data records are requested from the flock.
 *  
 *  \param format     one of the following: FB_POSITION, FB_ANGLES,
 *                   FB_MATRIX, FB_POSITION_ANGLES, FB_POSITION_MATRIX,  
 *                   FB_QUATERNION, FB_POSITION_QUATERNION
 *  
 *  The most common data format is FB_POSITION_ANGLES, which is the most
 *  compact format for the full six degrees of freedom. */
void AscensionCommandInterpreter::SetFormat(
  AscensionDataFormat format)
{
  this->EndStreamIfStreaming();

  m_DataFormat[m_FBBAddress] = format;
  char text[1];
  text[0] = format;

  this->SendRaw(text,1);
}

/** Enable or disable the reporting of button information from the flock.
 *  
 *  \param mode 0 or 1 depending on whether button information is desired 
 */
void AscensionCommandInterpreter::SetButtonMode(bool mode)
{
  this->EndStreamIfStreaming();

  m_ButtonMode[m_FBBAddress] = mode;
  char text[2];
  text[0] = FB_BUTTON_MODE;
  text[1] = mode;

  this->SendRaw(text,2);
}

/** The flock has two primary methods for sending data records to the 
 *  host computer: stream mode and point mode.
 *  
 *  In stream mode, the flock sends data records for all birds in the flock
 *  at a maximum rate of 100Hz.  Stream mode continues indefinitely until
 *  it is interrupted.
 *  
 *  In point mode, the flock will only send exactly one data record per bird
 *  each time that data is requested from the flock.
 *  
 *  By default, the flock will not be set to group mode and data records
 *  will only be sent from one bird.  The flock can be put into group mode
 *  by using ChangeValue() to set the FB_GROUP_MODE parameter to 1. */

/** Request the flock to begin streaming data records.
 *  The default data rate is 100Hz.
 *  
 *  Once the flock is in stream mode, Update() can be used
 *  to prepare a data record for access via GetPosition() and
 *  the other related functions.
 *  
 *  It is the responsibility of the application to call Update()
 *  often enough to ensure that the serial port buffer does not
 *  fill up.
 *  
 *  Stream mode can be turned of by EndStream().  Note that stream
 *  mode is automatically terminated by any of the following functions:
 *  SetFormat(), SetButtonMode(), ExamineValue(), Close(),
 *  Reset(), FBBReset(), Point(), ButtonRead(). */
void AscensionCommandInterpreter::Stream()
{
  m_StreamData = 1;
  m_CurrentBird = 1;

  this->SendRaw("@",1);
}

/** Terminate streaming mode. */
void AscensionCommandInterpreter::EndStream()
{
  m_StreamData = 0;
  this->SendRaw("B",1);

  /* Insert code to purge the serial port buffers to
   * clear any data records that are still in the buffer */
}

/** Terminate streaming mode if streaming. */
void AscensionCommandInterpreter::EndStreamIfStreaming()
{
  if (m_StreamData)
    {
    this->EndStream();
    }
}

/** Request a single data record from the flock.
 *  
 *  In group mode, this requests a single data record from each bird in
 *  the flock.  The data records must then be retrieved by a call to
 *  Update() for each one of the birds in the flock.
 *  
 *  If there are multiple birds but the flock is not in group mode,
 *  precede this command with RS232ToFBB() to get data from a single bird.
 *  
 *  Using point mode to obtain information from the flock is not
 *  as efficient as using stream mode. */
void AscensionCommandInterpreter::Point()
{
  this->EndStreamIfStreaming();

  m_CurrentBird = 1;

  if (!m_GroupMode)
    {
    m_CurrentBird = m_FBBAddress;
    }

  this->SendRaw("B",1);

  if (!m_Error)
    {
    m_PointData = 1;
    }
}   

/** Read the button state.  This method should rarely be used.
 *  
 *  \param val        space to store the button state
 *  
 *  It is much better to use SetButtonMode() to force the
 *  flock to return button information with each Update() call,
 *  and to then use GetButton() to retrieve the button state.
 *  
 *  In particular, the use of ButtonRead() in stream mode will
 *  cause stream mode to terminate.  The use of SetButtonMode()
 *  prior to calling Stream() will cause button information to
 *  be streamed from the flock along with the position information. */
void AscensionCommandInterpreter::ButtonRead(int *val)
{
  this->EndStreamIfStreaming();

  this->SendRaw("N",1);

  if (!m_Error && val != 0)
    {
    char data[16];
    this->ReceiveRaw(data,1);
    *val = data[0] >> 4;
    }
}

/** After a data record has been sent by the flock, the Update() method
 *  can be used to retrieve it.  Data records can be requested from the
 *  flock via either the Stream() or Point() methods.
 *  
 *  After Update() has been called, the various GetXX() methods 
 *  extract various pieces of information from the data record.  The
 *  GetBird() method should always be used to check which bird the
 *  data record is for, unless there is only a single bird. */

/** This is the central function in the flock interface: it retrieves
 *  a single data record from the flock. 
 *  
 *  If Point() is used to request data records from the flock, then
 *  every call to Update() must be preceeded by Point().
 *  
 *  If Stream() is used to put the flock into stream mode, then
 *  Update() is used to obtain the most recent data record that was
 *  sent from the flock. */
void AscensionCommandInterpreter::Update()
{
  int len;

  len = 2*ComputeRecordSize(m_DataFormat[m_CurrentBird]) \
    + m_ButtonMode[m_CurrentBird] + m_GroupMode;
  this->ReceiveRaw(m_DataBuffer,len);

  /* Insert code to get a timestamp */

  if (this->GetBird() != m_CurrentBird)
    {
    if (this->GetBird())
      {
      m_CurrentBird = this->GetBird();
      }
    this->SetErrorAndMessage(FB_PHASE_ERROR,
                             "received malformed data record");
    }

  if (++m_CurrentBird > m_NumberOfBirds)
    {
    m_CurrentBird = 1;
    m_PointData = 0;    /* finished point data */
    }

}

/** Get the position returned in the last Update() data record.
 *  
 *  \param xyz        storage space for the position to be returned in
 *  
 *  The bird positions are only available if SetFormat() was called
 *  with one of the following modes: FB_POSITION, FB_POSITION_ANGLES,
 *  FB_POSITION_MATRIX, FB_POSITION_QUATERNION. */
void AscensionCommandInterpreter::GetPosition(float xyz[3])
{
  AscensionDataFormat format = m_DataFormat[m_CurrentBird];

  if (format == FB_POSITION ||
      format == FB_POSITION_ANGLES ||
      format == FB_POSITION_MATRIX ||
      format == FB_POSITION_QUATERNION)
    {
    char *cp = &m_DataBuffer[0];

    // note: 914.4 mm == 36 inches
    float range = (m_PositionScale[m_CurrentBird]+1)*914.4f;

    xyz[0] = (float)(this->Unpack(&cp)*range*0.000030517578125f);
    xyz[1] = (float)(this->Unpack(&cp)*range*0.000030517578125f);
    xyz[2] = (float)(this->Unpack(&cp)*range*0.000030517578125f);
    }
}

/** Get the euler angles returned in the last Update() data record.
 *  
 *  \param zyx        storage space for the angles to be returned in
 *  
 *  The bird angles are only available if SetFormat() was called
 *  with one of the following modes: FB_ANGLES, FB_POSITION_ANGLES. */
void AscensionCommandInterpreter::GetAngles(float zyx[3])
{
  static float pi = 3.1415926535897931f;

  AscensionDataFormat format = m_DataFormat[m_CurrentBird];

  if (format == FB_ANGLES || format == FB_POSITION_ANGLES)
    {
    char *cp = &m_DataBuffer[0];
    
    if (format == FB_POSITION_ANGLES)
      {
      cp = &m_DataBuffer[6];
      }

    zyx[0] = (float)(this->Unpack(&cp)*pi*0.000030517578125f);
    zyx[1] = (float)(this->Unpack(&cp)*pi*0.000030517578125f);
    zyx[2] = (float)(this->Unpack(&cp)*pi*0.000030517578125f);
    }
}

/** Get the matrix returned in the last Update() data record.
 *  
 *  \param a          storage space for the nine matrix elements,
 *                    where the first three numbers are the first
 *                    column, the next three numbers are the middle
 *                    column, and the final three numbers are the
 *                    final matrix column
 *  
 *  The bird matrix is only available if SetFormat() was called
 *  with one of the following modes: FB_MATRIX, FB_POSITION_MATRIX.
 *  
 *  It is almost always better to request angles from the flock
 *  and convert the angles to a matrix using fbMatrixFromAngles()
 *  because this reduces the amount of information that must be
 *  transferred through the serial port. */
void AscensionCommandInterpreter::GetMatrix(float a[9])
{
  AscensionDataFormat format = m_DataFormat[m_CurrentBird];

  if (format == FB_MATRIX || format == FB_POSITION_MATRIX)
    {
    char *cp = &m_DataBuffer[0];

    if (format == FB_POSITION_MATRIX)
      {
      cp = &m_DataBuffer[6];
      }

    for (unsigned int i = 0; i < 9; i++)
      {
      a[i] = (float)(this->Unpack(&cp)*0.000030517578125f);
      }
    }
}

/** Get the quaternion returned in the last Update() data record.
 *  
 *  \param q          storage space for the quaternion to be returned in
 *  
 *  The bird quaternion is only available if SetFormat() was called
 *  with one of the following modes: FB_QUATERNION, FB_POSITION_QUATERNION. */
void AscensionCommandInterpreter::GetQuaternion(float q[4])
{
  AscensionDataFormat format = m_DataFormat[m_CurrentBird];

  if (format == FB_QUATERNION || format == FB_POSITION_QUATERNION)
    {
    char *cp = &m_DataBuffer[0];

    if (format == FB_POSITION_QUATERNION)
      {
      cp = &m_DataBuffer[6];
      }

    for (unsigned int i = 0; i < 4; i++)
      {
      q[i] = (float)(this->Unpack(&cp)*0.000030517578125f);
      }
    }
}

/** Get the button state returned in the last Update() data record.
 *  
 *  \return           button state:
 *                    - 0x00 no button was pressed
 *                    - 0x10 left button was pressed
 *                    - 0x30 middle (or middle and left) was pressed
 *                    - 0x70 right (or right and any other) was pressed
 *  
 *  The return value is always zero unless ButtonMode() has been
 *  used to turn on button reporting. */
int AscensionCommandInterpreter::GetButton()
{
  int rval = 0;

  if (m_ButtonMode[m_CurrentBird])
    {
    rval = m_DataBuffer[2*ComputeRecordSize(m_DataFormat[m_CurrentBird])] >> 4;
    }

  return rval;
}

/** Get the FBB address of the bird for the data record obtained through
 *  the last Update().
 *  
 *  \return           a value between 1 and \em n where \em n is the
 *                    number of birds in the flock, or 0 if an error
 *                    occurred
 *  
 *  If the flock is not operating in group mode, then the return value
 *  is always 1.  A return value of zero indicates that a phase error
 *  or some other communication problem occurred with the flock. */
unsigned int AscensionCommandInterpreter::GetBird()
{
  unsigned int bird;

  if (m_GroupMode)
    {
    bird = m_DataBuffer[2*ComputeRecordSize(m_DataFormat[m_CurrentBird]) \
                          + m_ButtonMode[m_CurrentBird]];

    /* check for standalone bird */ 
    if (bird == 0)
      {
      bird = 1;
      }
    }
  else
    {
    bird = m_CurrentBird;
    }
  /** if bird > num_birds, then an error has occured: return 0
     to flag the error */
  if (bird > m_NumberOfBirds || bird < 1)
    {
    bird = 0;
    }

  return bird;
}

/** These are helper functions that convert data from one format to
 *  another in order to ease the decoding of data records sent by the flock. */

/** Convert euler angles into a 3x3 matrix.
 *  
 *  \param a        the nine matrix elements are stored here, column by column
 *  \param zyx      the three angles */
void AscensionCommandInterpreter::MatrixFromAngles(float a[9],
                                                      const float zyx[3])
{
  double cx,sx,cy,sy,cz,sz;

  cz = cos((double)(zyx[0]));
  sz = sin((double)(zyx[0]));
  cy = cos((double)(zyx[1]));
  sy = sin((double)(zyx[1]));
  cx = cos((double)(zyx[2]));
  sx = sin((double)(zyx[2]));

  a[0] = (float)(cy*cz);
  a[1] = (float)(-cx*sz + sx*sy*cz);
  a[2] = (float)(sx*sz + cx*sy*cz);

  a[3] = (float)(cy*sz);
  a[4] = (float)(cx*cz + sx*sy*sz);
  a[5] = (float)(-sx*cz + cx*sy*sz);

  a[6] = (float)(-sy);
  a[7] = (float)(sx*cy);
  a[8] = (float)(cx*cy);
}

/** Does the opposite of MatrixFromAngles().
 *  \param zyx        the three angles are stored here
 *  \param a          the matrix */
void AscensionCommandInterpreter::AnglesFromMatrix(float zyx[3],
                                                      const float a[9])
{
  double r,cz,sz;

  r = sqrt((double)(a[0]*a[0] + a[3]*a[3]));
  cz = a[0]/r;
  sz = a[3]/r;

  zyx[0] = (float)atan2(sz, cz);
  zyx[1] = (float)atan2((double)(-a[6]), cz*a[0]+sz*a[3]);
  zyx[2] = (float)atan2(sz*a[2]-cz*a[5], -sz*a[1]+cz*a[4]);
}

/** A helper function that concatenates two characters into an unsigned short.
 *  \param cp     pointer to the two characters
 *  \return       the unsigned short value */
int AscensionCommandInterpreter::GetShort(const char *cp)
{
  unsigned char lsb;
  short msb;

  lsb = *cp++;
  msb = *cp++;
  msb <<= 8;
  msb |= lsb;
  return msb; 
}

/** These are general-purpose methods for communicating with the flock.
 *  The data sent to the flock and recived from the flock usually consists
 *  of a single byte, multiple bytes, a single 2-byte word, or multiple
 *  2-byte words.
 *
 *  Note that there are pre-defined functions to support
 *  many of the flock commands, e.g. Stream() for FB_STREAM
 *  and SetFormat() for handling FB_POSITION_ANGLES and the other
 *  data format mode commands.  As a result, there is rarely any need
 *  to call SendCommand() directly.
 *
 *  The commands are listed in flock.h.  For a description of the
 *  commands, see The Flock of Birds INSTALLATION AND OPERATION GUIDE
 *  from Ascension Technology Corporation. */

/** Examine a flock parameter.
 *  \param parameter  a constant that specifies what parameter to examine
 *  \return           the integer value of the parameter
 *
 *  This function can only be used if the parameter value fits into a single
 *  byte or into a single word, otherwise either ExamineValueWords() or
 *  ExamineValueBytes() should be used instead. */
int AscensionCommandInterpreter::ExamineValue(
  AscensionParameter parameter)
{
  char data[128];
  int len;

  len = this->ExamineValueBytes(parameter,data);
  
  if (len == 2)
    {
    return this->GetShort(data);
    }
  else if (len == 1)
    {
    return (unsigned char)data[0];
    }
  else if (len == 0)
    {
    return 0;
    }
  else
    {
    this->SetErrorAndMessage(FB_PARM_ERROR,
                             "bad parameter for ExamineValue");
    return 0;
    }
}  

/** Examine a flock parameter that consists of 16-bit words.
 *
 *  \param parameter  a constant that specifies what parameter to examine
 *  \param data       space to store the word values
 *
 *  \return           number of word values stored in the array */
int AscensionCommandInterpreter::ExamineValueWords(
  AscensionParameter parameter,
  short *data)
{
  char text[128];

  unsigned int len = this->ExamineValueBytes(parameter,text);
  
  if (len % 2 != 0)
    {
    this->SetErrorAndMessage(FB_PARM_ERROR,
                             "bad parameter for ExamineValueWords");
    return 0;
    }

  unsigned int n = len/2;
  for (unsigned int i = 0; i < n; i++)
    {
    data[i] = this->GetShort(&text[2*i]);
    }

  return len/2;
}  

/** number of bytes in examine/change data */
static int examine_change_len_table[36] = { 2, // FB_STATUS
                                            2, // FB_REVISION 
                                            2, // FB_SPEED
                                            2, // FB_POSITION_SCALING 
                                            2, // FB_FILTER
                                            14,// FB_ALPHA_MIN
                                            2, // FB_RATE_COUNT 
                                            2, // FB_RATE 
                                            1, // FB_DATA_READY
                                            1, // FB_DATA_READY_CHAR
                                            1, // FB_ERROR_CODE
                                            1, // FB_ERROR_BEHAVIOR
                                            14,// FB_VM
                                            14,// FB_ALPHA_MAX
                                            1, // FB_ELIMINATION
                                            10,// FB_IDENTIFICATION
                                            2, // FB_ERROR_CODE_EXPANDED
                                            1, // FB_REFERENCE_FRAME
                                            1, // FB_TRANSMITTER_MODE
                                            1, // FB_FBB_ADDRESS_MODE
                                            1, // FB_LINE_FILTER
                                            1, // FB_FBB_ADDRESS
                                            2, // FB_P_HEMISPHERE
                                            6, // FB_P_ANGLE_ALIGN2
                                            6, // FB_P_REFERENCE_FRAME2
                                            2, // FB_SERIAL_NUMBER
                                            2, // FB_SENSOR_SERIAL_NUMBER
                                            2, // FB_XMTR_SERIAL_NUMBER
                                            0, 0, 0, 0,
                                            2, // FB_FBB_DELAY 
                                            0, 0,
                                            1, // FB_GROUP_MODE
};

/** Examine a flock parameter that consists of bytes.
 *  \param parameter  a constant that specifies what parameter to examine
 *  \param data       space to store the byte values
 *  \return           number of bytes stored in the array */
int AscensionCommandInterpreter::ExamineValueBytes(
  AscensionParameter parameter,
  char *data)
{
  this->EndStreamIfStreaming();

  char text[2];
  text[0] = FB_EXAMINE_VALUE;
  text[1] = parameter;

  int len = 0;

  if (parameter <= m_MaxParameter)
    {
    len = examine_change_len_table[parameter];
    }
  else if (parameter == FB_FBB_DELAY)
    {
    len = 2;
    }
  else if (parameter == FB_GROUP_MODE && m_Revision >= (3 << 8) + 33)
    {
    len = 1;
    }
  else if (parameter == FB_FBB_STATUS)
    {
    len = 14;
    if (m_AddressMode == FB_EXPANDED)
      {
      len = 30;
      }
    if (m_AddressMode == FB_SUPER_EXPANDED)
      {
      len = 126;
      }
    }
  else if (parameter == FB_FBB_AUTOCONFIG)
    {
    len = 5;
    if (m_AddressMode == FB_EXPANDED)
      {
      len = 7;
      }
    if (m_AddressMode == FB_SUPER_EXPANDED)
      {
      len = 19;
      }
  }
  else
    {
    this->SetErrorAndMessage(FB_PARM_ERROR,
                             "bad examine value parameter");
    return 0;
    }

  this->SendRaw(text,2);
  if (m_Error || data == 0)
    {
    return 0;
    }
 
  this->ReceiveRaw(data,len);

  return len;
}

/** Store the low 16 bits of an integer in two bytes.
 *  \param cp      space to store the resulting two bytes
 *  \param val     the integer to convert */
void AscensionCommandInterpreter::PutShort(char *cp, int val)
{
  unsigned char lsb;
  short msb;

  lsb = val;
  msb = val;
  msb >>= 8;
  msb &= 0x00ff;

  *cp++ = lsb;
  *cp++ = (unsigned char)msb;
}

/** Modify an 8-bit or 16-bit flock parameter.
 *  \param parameter  a constant that specifies what parameter to modify
 *  \param val        the new parameter value */
void AscensionCommandInterpreter::ChangeValue(
  AscensionParameter parameter, int val)
{
  char data[16];
  int outputDataLen;
  
  outputDataLen = 0;
  
  if (parameter <= 32)
    {
    outputDataLen = examine_change_len_table[parameter];
    }
  else if (parameter == FB_GROUP_MODE)
    {
    outputDataLen = 1;
    }
  else if (parameter == FB_FBB_AUTOCONFIG)
    {
    outputDataLen = 1;
    }

  if (outputDataLen == 1)
    {
    data[0] = val;
    }
  else if (outputDataLen == 2)
    {
    PutShort(data,val);
    }
  else
    {
    this->SetErrorAndMessage(FB_PARM_ERROR,
                             "bad parameter for ChangeValue");
    return;
    }

  this->ChangeValueBytes(parameter,data);
}

/** Modify a flock parameter that consists of several words.
 *
 *  \param parameter  a constant that specifies what parameter to modify
 *  \param data       the new parameter data */
void AscensionCommandInterpreter::ChangeValueWords(
  AscensionParameter parameter, const short *data)
{
  char text[16];
  unsigned int outputDataLen;
  
  outputDataLen = 0;
  
  if (parameter <= 32)
    {
    outputDataLen = examine_change_len_table[parameter];
    }
  else if (parameter == FB_GROUP_MODE)
    {
    outputDataLen = 1;
    }
  else if (parameter == FB_FBB_AUTOCONFIG)
    {
    outputDataLen = 1;
    }

  if (outputDataLen % 2 != 0)
    {
    this->SetErrorAndMessage(FB_PARM_ERROR,
                             "bad parameter for ChangeValueWords");
    return;
    }

  unsigned int n = outputDataLen/2;
  for (unsigned int i = 0; i < n; i++)
    {
    PutShort(&text[2*i],data[i]);
    }
  this->ChangeValueBytes(parameter,text);
}

/** Modify a flock parameter that consists of several bytes.
 *
 *  \param parameter  a constant that specifies what parameter to modify
 *  \param data       the new parameter data */
void AscensionCommandInterpreter::ChangeValueBytes(
  AscensionParameter parameter, const char *data)
{
  char text[16];
  int outputDataLen;

  text[0] = (char)FB_CHANGE_VALUE;
  text[1] = (char)parameter;

  if (parameter == FB_POSITION_SCALING)
    {
    m_PositionScale[m_FBBAddress] =
      AscensionPositionScale(this->GetShort(data));
    }

  if (parameter <= m_MaxParameter)
    {
    outputDataLen = examine_change_len_table[parameter];
    }
  else if (parameter == FB_GROUP_MODE && m_Revision >= (3 << 8) + 33)
    {
    m_GroupMode = data[0];
    outputDataLen = 1;
    }
  else if (parameter == FB_FBB_STATUS)
    {
    outputDataLen = 0;
    }
  else if (parameter == FB_FBB_AUTOCONFIG)
    {
    this->FBBAutoConfig(data[0]);
    return;
    }
  else
    {
    this->SetErrorAndMessage(FB_PARM_ERROR,
                             "bad parameter for ChangeValueBytes");
    return;
    }

  memcpy(&text[2],data,outputDataLen);

  AscensionCommandInterpreter::SendRaw(text,outputDataLen+2);
}

/** Send a command to the flock with no arguments.
 *
 *  \param command    a constant that specifies what command to send
 *
 *  This function is appropriate for the following commands only:
 *  FB_REPORT_RATE_FULL, FB_REPORT_RATE_DIV2, FB_REPORT_RATE_DIV8,
 *  FB_REPORT_RATE_DIV32, FB_RUN, FB_SLEEP, FB_XOFF, FB_XON. */
void AscensionCommandInterpreter::SendCommand(AscensionCommand c)
{
  this->SendCommandBytes(c,0);
}

/** Send a command to the flock along with the data words associated
 *  with the command.
 *
 *  \param command    a constant that specifies what command to send
 *  \param data       the data to send with the command
 *
 *  This function is only appropriate for the following commands:
 *  FB_ANGLE_ALIGN1 (6 words), FB_REFERENCE_FRAME1 (6 words),
 *  FB_ANGLE_ALIGN2 (3 words), FB_REFERENCE_FRAME2 (3 words),
 *  FB_HEMISPHERE (1 word) or FB_SYNC (1 word). */
void AscensionCommandInterpreter::SendCommandWords(
  AscensionCommand c, const short *outputData)
{
  char text[16];
  int outputDataLen,i;

  outputDataLen = 0;

  switch (c)
    {
    case FB_ANGLE_ALIGN1:
    case FB_REFERENCE_FRAME1:
      outputDataLen = 6;
      break;
    case FB_ANGLE_ALIGN2:
    case FB_REFERENCE_FRAME2:
      outputDataLen = 3;
      break;
    case FB_HEMISPHERE:
    case FB_SYNC:
      outputDataLen = 1;
      break;
    default:
      this->SetErrorAndMessage(FB_COMMAND_ERROR,
                               "bad command for SendCommandWords");
      return;
    }

  for (i = 0; i < outputDataLen; i++)
    {
    PutShort(&text[2*i],outputData[i]);
    }

  this->SendCommandBytes(c,text);
}

/** Send a command to the flock along with the data bytes associated
 *  with the command.
 *
 *  \param command    a constant that specifies what command to send
 *  \param data       the data to send with the command, or NULL if there
 *                   is no data associated with the command
 * This function can be used to send any command to the flock. */
void AscensionCommandInterpreter::SendCommandBytes(
  AscensionCommand c, const char *outputData)
{
  unsigned int outputDataLen;
  char text[16];

  switch (c)
    {
    case FB_EXAMINE_VALUE:
      this->ExamineValueBytes(AscensionParameter(outputData[0]),
                              0);
      return;
    case FB_CHANGE_VALUE:
      this->ChangeValueBytes(AscensionParameter(outputData[0]),
                             &outputData[1]);
      return;
    case FB_POINT:
      this->Point();
      return;
    case FB_BUTTON_READ:
      this->ButtonRead(0);
      return;
    case FB_STREAM:
      this->Stream();
      return;
    case FB_POSITION:
    case FB_ANGLES:
    case FB_MATRIX:
    case FB_POSITION_ANGLES:
    case FB_POSITION_MATRIX:
    case FB_QUATERNION:
    case FB_POSITION_QUATERNION:
      this->SetFormat(AscensionDataFormat(c));
      return;
    case FB_BUTTON_MODE:
      this->SetButtonMode(outputData[0]);
      return;
    case FB_FBB_RESET:
      this->FBBReset();
      return;
    case FB_REPORT_RATE_FULL:
    case FB_REPORT_RATE_DIV2:
    case FB_REPORT_RATE_DIV8:
    case FB_REPORT_RATE_DIV32:
    case FB_RUN:
    case FB_SLEEP:
    case FB_XOFF:
    case FB_XON:
      outputDataLen = 0;
      break;
    case FB_ANGLE_ALIGN1:
    case FB_REFERENCE_FRAME1:
      outputDataLen = 12;
      break;
    case FB_ANGLE_ALIGN2:
    case FB_REFERENCE_FRAME2:
      outputDataLen = 6;
      break;
    case FB_HEMISPHERE:
    case FB_SYNC:
      outputDataLen = 2;
      break;
    case FB_NEXT_TRANSMITTER:
      outputDataLen = 1;
      break;
    case FB_RS232_TO_FBB_SE:
      this->RS232ToFBB(outputData[0]);
      return;
    default:
      if (c >= FB_RS232_TO_FBB && c < FB_RS232_TO_FBB+16)
        {
        this->RS232ToFBB(c-FB_RS232_TO_FBB);
        return;
        }
      if (c >= FB_RS232_TO_FBB_E && c < FB_RS232_TO_FBB)
        {
        this->RS232ToFBB(c - FB_RS232_TO_FBB_E + 0x10);
        return;
        }
      this->SetErrorAndMessage(FB_COMMAND_ERROR,
                               "unrecognized command sent to bird");
      return;
    }

  text[0] = c;
  if (outputData)
    {
    for (unsigned int i = 0; i < outputDataLen; i++)
      {
      text[i+1] = outputData[i];
      }
    }

  this->SendRaw(text,outputDataLen+1);
}

/** These methods send raw data to the flock and read raw data from the flock.
 *  They should only be used by someone who is very familiar both with the
 *  flock of birds and with the driver code. */

/** This function is meant primarily for internal use.  It sends a
 *  raw stream of bytes to the flock.
 *  \param text       the bytes to send to the flock
 *  \param len        the number of bytes to send
 *
 *  If a command is sent to the flock with this function that causes
 *  the state of the flock to change, then communication with the flock
 *  might be disrupted.  The SendCommandBytes() function should be
 *  used instead of SendRaw() and ReceiveRaw() in all circumstances. */
void AscensionCommandInterpreter::SendRaw(const char *text,
                                             unsigned int len)
{
  igstk::Communication::ResultType result = m_Communication->Write(text,len);
   
  if (result == igstk::Communication::FAILURE)
    {
    this->SetErrorAndMessage(FB_IO_ERROR,
                             "I/O error on serial port write");
    }
  else if (result == igstk::Communication::TIMEOUT)
    {
    this->SetErrorAndMessage(FB_TIMEOUT_ERROR,
                             "timeout on serial port write");
    }
}

/** This function is meant primarily for internal use.  It reads a
 *  raw stream of bytes from the flock.
 *  \param reply      the bytes read from the flock
 *  \param len        the number of bytes to read */
void AscensionCommandInterpreter::ReceiveRaw(char *reply,
                                                unsigned int len)
{
  unsigned int n = len;
  unsigned int i = 0;

  if (m_StreamData || m_PointData)
    { /* correct for previous phase error */
    i = m_PhaseErrorLeftoverBytes;
    n -= i;
    }
  m_PhaseErrorLeftoverBytes = 0;

  /* Insert code to read from serial port: in order restart after a
   * phase error, read into &reply[i] and read n bytes. */
  unsigned int bytesRead;
  igstk::Communication::ResultType  result =
               m_Communication->Read(reply,len,bytesRead);

  /* shared code ------------------------*/
  if ((m_StreamData || m_PointData) 
      && result == igstk::Communication::SUCCESS)
    {  /* check for phase errors */
    if (!(reply[0] & 0x80))
      {
      }
    for (i = 1; i < len; i++)
      {
      if (reply[i] & 0x80)
        {
        memmove(reply,&reply[i],len-i);
        m_PhaseErrorLeftoverBytes = len-i;
        break;
        }
      }
    }

  if (result==igstk::Communication::FAILURE)
    {
    this->SetErrorAndMessage(FB_IO_ERROR,
                             "I/O error on serial port read");
    }
  else if (result==igstk::Communication::TIMEOUT)
    {
    this->SetErrorAndMessage(FB_TIMEOUT_ERROR,
                             "timeout while waiting for bird data");
    }
}

/** A helper function: unpack two characters sent from the flock into
 *  a short integer, and advance the character pointer by two.  This
 *  will convert the data from the flock's special 7-bit data record
 *  encoding into conventional 8-bit data.
 *  \param cp    pointer to a data string from the flock
 *  \return      the unpacked data */
int AscensionCommandInterpreter::Unpack(char **cp)
{
  unsigned char lsb;
  short msb;

  lsb = *(*cp)++;
  msb = *(*cp)++;
  lsb <<= 1;
  msb <<= 8;
  msb |= lsb;
  msb <<= 1;
  return msb;
}

/** Set the error indicator.
 *  \param errcode    the error code
 *  \param text       description of the error
 *  \return           the error code that was set */
void AscensionCommandInterpreter::SetErrorAndMessage(
  AscensionErrorCode errorcode, const char *text)
{
  m_Error = errorcode;
  m_ErrorText = text;
}

/** These methods are used to check whether an error occured as a result of
 *  an attempt to communicate with the flock. */

/** Return the last error code and clear the error indicator.
 *  \return           integer error code, or zero if no error
 *
 *  Note that the error codes are generated by the host computer,
 *  not by the flock.  To check the error code for the flock,
 *  use ExamineValue() to get the value of the FB_ERROR_CODE
 *  parameter.
 *
 *  All of the flock functions can generate errors except for
 *  the following:  GetButton(), GetPosition(), GetAngles(),
 *  GetQuaternion(), GetMatrix(). */
AscensionErrorCode AscensionCommandInterpreter::GetError()
{
  AscensionErrorCode error = m_Error;
  m_Error = FB_NO_ERROR;
  return error;
}

/** Return some text that describes the last error. */
const char *AscensionCommandInterpreter::GetErrorMessage()
{
  return m_ErrorText.c_str();
}

/** PrintSelf function. */
void AscensionCommandInterpreter::PrintSelf(std::ostream& os,
                                      itk::Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "ErrorText: " << m_ErrorText << std::endl;
  os << indent << "Error: " << m_Error << std::endl;
 
  os << indent << "PointData: " << (m_PointData ? "true" : "false")
     << std::endl;

  os << indent << "StreamData: " << (m_StreamData ? "true" : "false")
     << std::endl;
  
  os << indent << "GroupMode: " << (m_GroupMode ? "true" : "false")
     << std::endl;
  
  os << indent << "CurrentBird: " << m_CurrentBird << std::endl;
  os << indent << "NumberOfBirds: " << m_NumberOfBirds << std::endl;
  os << indent << "Communication: " << m_Communication << std::endl;
  os << indent << "PhaseErrorLeftoverBytes: "
     << m_PhaseErrorLeftoverBytes << std::endl;
  os << indent << "FBBAddress: " << m_FBBAddress << std::endl;
}


} // end namespace igstk
