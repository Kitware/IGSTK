/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAscensionCommandInterpreter.h
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

#ifndef __igstkAscensionCommandInterpreter_h
#define __igstkAscensionCommandInterpreter_h

#include "igstkObject.h"
#include "igstkSerialCommunication.h"

#include <string>

namespace igstk
{

/** \enum AscensionCommand
 *
 * Every bird command consists of a single byte.  The commands can be
 * sent using either SendCommand(), SendCommandBytes(), or 
 * SendCommandWords() depending on whether there is any data
 * associated with the command and whether the data is formatted as
 * 8-bit bytes or 16-bit words.
 */
enum AscensionCommand
{
  FB_POINT               = 'B',
  FB_STREAM              = '@',
  FB_REPORT_RATE_FULL    = 'Q',
  FB_REPORT_RATE_DIV2    = 'R',
  FB_REPORT_RATE_DIV8    = 'S',
  FB_REPORT_RATE_DIV32   = 'T',

  FB_C_POSITION          = 'V',
  FB_C_ANGLES            = 'W',
  FB_C_MATRIX            = 'X',
  FB_C_POSITION_ANGLES   = 'Y',
  FB_C_POSITION_MATRIX   = 'Z',
  FB_C_QUATERNION        = '\\',
  FB_C_POSITION_QUATERNION = ']',

  FB_BUTTON_MODE         = 'M',
  FB_BUTTON_READ         = 'N',

  FB_ANGLE_ALIGN1        = 'J',
  FB_REFERENCE_FRAME1    = 'H',
  FB_ANGLE_ALIGN2        = 'q',
  FB_REFERENCE_FRAME2    = 'r',

  FB_FBB_RESET           = '/',
  FB_HEMISPHERE          = 'L',
  FB_NEXT_TRANSMITTER    = '0',
  FB_RUN                 = 'F',
  FB_SLEEP               = 'G',
  FB_SYNC                = 'A',
  FB_XOFF                = '\x13',
  FB_XON                 = '\x11',
 
  FB_RS232_TO_FBB        = '\xF0',
  FB_RS232_TO_FBB_E      = '\xE0',
  FB_RS232_TO_FBB_SE     = '\xA0',

  FB_EXAMINE_VALUE       = 'O',
  FB_CHANGE_VALUE        = 'P',
};

/** \enum AscensionDataFormat
 *
 * The Flock of Birds supports several different data formats, which
 * can be set with the SetFormat() method.  Since the native format
 * is FB_POSITION_ANGLES, it is the one you should generally choose.
 * It is also the most compact six-degree-of-freedom format.
 */
enum AscensionDataFormat
{
  FB_POSITION            = 'V',
  FB_ANGLES              = 'W',
  FB_MATRIX              = 'X',
  FB_POSITION_ANGLES     = 'Y',
  FB_POSITION_MATRIX     = 'Z',
  FB_QUATERNION          = '\\',
  FB_POSITION_QUATERNION = ']',
};

/** \enum AscensionStatusBits
 *
 * The bird status parameter FB_STATUS can be examined using the
 * ExamineValue() function.  The status consists of 16 bits as
 * defined below:
 */
enum AscensionStatusBits
{
  FB_STATUS_STREAMING   = 0x0001,
  FB_STATUS_FORMAT      = 0x001E,
  FB_STATUS_SLEEPING    = 0x0020,
  FB_STATUS_XOFF        = 0x0040,
  FB_STATUS_FACTORY     = 0x0080,
  FB_STATUS_NO_SYNC     = 0x0100,
  FB_STATUS_CRT_SYNC    = 0x0200,
  FB_STATUS_EXPANDED    = 0x0400,  /**< expanded addressing mode */
  FB_STATUS_HOST_SYNC   = 0x0800,
  FB_STATUS_RUNNING     = 0x1000,
  FB_STATUS_ERROR       = 0x2000,
  FB_STATUS_INITIALIZED = 0x4000,
  FB_STATUS_MASTER      = 0x8000,
};

/** \enum AscensionBusStatusBits
 *
 * The FB_FBB_STATUS parameter can be examined using ExamineValueBytes(),
 * which will provide one status byte for each bird supported by
 * the current addressing mode: 14 in FB_NORMAL addressing mode, 
 * 30 in FB_EXPANDED addressing mode, and 126 in FB_SUPER_EXPANDED
 * addressing mode.  The bits for each bird are defined below.
 */
enum AscensionBusStatusBits
{
  FB_FBB_STATUS_ERT0        = 0x01,
  FB_FBB_STATUS_ERT1        = 0x02,
  FB_FBB_STATUS_ERT2        = 0x04,
  FB_FBB_STATUS_ERT3        = 0x08,
  FB_FBB_STATUS_ERT         = 0x10,
  FB_FBB_STATUS_SENSOR      = 0x20,
  FB_FBB_STATUS_RUNNING     = 0x40,
  FB_FBB_STATUS_ACCESSIBLE  = 0x80,
};

/** \enum AscensionErrorCode
 *
 * The following error codes are returned by GetError():
 */
enum AscensionErrorCode
{
  FB_NO_ERROR       = 0,     /**< no error indicated */
  FB_OPEN_ERROR     = 1,     /**< error opening serial port */
  FB_COM_ERROR      = 2,     /**< error setting COM port parameters */
  FB_IO_ERROR       = 3,     /**< some sort of I/O error */
  FB_TIMEOUT_ERROR  = 4,     /**< communications timeout error */
  FB_PARM_ERROR     = 5,     /**< bad examine/change parameter */
  FB_COMMAND_ERROR  = 6,     /**< unrecognized bird command */
  FB_ILLEGAL_ERROR  = 7,     /**< action is illegal in this state */ 
  FB_PHASE_ERROR    = 8,     /**< phase error: comm port is dropping bytes */
  FB_RESOURCE_ERROR = 9,     /**< out of system resources */
};

/** \enum AscensionAddressMode
 *
 * The FB_FBB_ADDRESS_MODE parameter can be examined with ExamineValue().
 * It is set via the dip switches on the bird units.
 * The FB_NORMAL mode supports 14 birds, FB_EXPANDED supports 30 birds, and
 * FB_SUPER_EXPANDED supports 126 birds.
 */
enum AscensionAddressMode
{
  FB_NORMAL           = 0,
  FB_EXPANDED         = 1,
  FB_SUPER_EXPANDED   = 3,
};

/** \enum AscensionHemisphere
 *
 * The FB_P_HEMISPHERE parameter can be examined with ExamineValue() and
 * changed with fbChangeValue().  Alternatively, it can be changed by
 * sending the FB_HEMISPHERE command to the flock.
 */
enum AscensionHemisphere
{
  FB_FORWARD = 0x0000,
  FB_AFT     = 0x0100,
  FB_UPPER   = 0x010C,
  FB_LOWER   = 0x000C,
  FB_LEFT    = 0x0106,
  FB_RIGHT   = 0x0006,
};

/** \enum AscensionPositionScale
 *
 * The FB_POSITION_SCALING parameter is specified here in millimeters
 * rather than inches.  The extended range transmitters are not yet
 * supported, so only FB_STANDARD and FB_EXTENDED modes are available.
 * The mode can be examined with ExamineValue() or changed with
 * ChangeValue().
 */
enum AscensionPositionScale
{
  FB_STANDARD = 0,
  FB_EXTENDED = 1,
};

/** \enum AscensionParameters
 *
 * The parameters can be examined using ExamineValue(), 
 * ExamineValueBytes(), or ExamineValueWords().  Certain
 * parameters can be changed with ChangeValue(),
 * ChangeValueBytes(), or ChangeValueWords().
 */
enum AscensionParameter
{
  FB_STATUS            = 0,     /**< Bird status */
  FB_REVISION          = 1,     /**< Software revision number */
  FB_SPEED             = 2,     /**< Bird computer crystal speed */
  FB_POSITION_SCALING  = 3,     /**< Position scaling */

  FB_FILTER            = 4,     /**< Filter on/off status */
  FB_ALPHA_MIN         = 5,     /**< DC Filter constant table ALPHA_MIN */
  FB_RATE_COUNT        = 6,     /**< Bird measurement rate count */
  FB_RATE              = 7,     /**< Bird measurement rate */

  FB_DATA_READY        = 8,     /**< Disable/Enable data ready output */
  FB_DATA_READY_CHAR   = 9,     /**< Changes data ready character */
  FB_ERROR_CODE        = 10,    /**< Bird outputs an error code */
  FB_ERROR_BEHAVIOR    = 11,    /**< On error: stop or don't stop Bird  */

  FB_VM                = 12,    /**< DC filter constant table Vm */
  FB_ALPHA_MAX         = 13,    /**< DC filter constant table ALPHA_MAX */
  FB_ELIMINATION       = 14,    /**< Sudden output change elimination */
  FB_IDENTIFICATION    = 15,    /**< System Model Identification */

  FB_ERROR_CODE_EXPANDED = 16,  /**< Expanded Error Code */
  FB_REFERENCE_FRAME   = 17,    /**< XYZ Reference Frame */
  FB_TRANSMITTER_MODE  = 18,    /**< Transmitter Operation Mode */
  FB_FBB_ADDRESS_MODE  = 19,    /**< FBB addressing mode */

  FB_LINE_FILTER       = 20,    /**< Filter line frequency */
  FB_FBB_ADDRESS       = 21,    /**< FBB address */
  FB_P_HEMISPHERE      = 22,    /**< Change / Examine Hemisphere */
  FB_P_ANGLE_ALIGN2    = 23,    /**< Change / Examine Angle Align2 */

  FB_P_REFERENCE_FRAME2 = 24,   /**< Change / Examine Reference Frame2 */
  FB_SERIAL_NUMBER     = 25,    /**< Bird Serial Number */
  FB_SENSOR_SERIAL_NUMBER = 26, /**< Sensor Serial Number */
  FB_XMTR_SERIAL_NUMBER = 27,   /**< Xmtr Serial Number */

  FB_FBB_DELAY         = 32,    /**< FBB Host Delay */
  FB_GROUP_MODE        = 35,    /**< Group Mode */

  FB_FBB_STATUS        = 36,    /**< Flock System Status */

  FB_FBB_AUTOCONFIG    = 50,    /**< FBB Auto-Configuration, 1 xmtr/N snsrs */
};

/** \enum AscensionBaudRates
 *
 * The following baud rates are supported by Open():
 */
enum AscensionBaudRate
{
  FB_2400   = 0,
  FB_4800   = 1,
  FB_9600   = 2,
  FB_19200  = 3,
  FB_38400  = 4,
  FB_57600  = 5,
  FB_115200 = 6,
};

class AscensionCommandInterpreter : public Object
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( AscensionCommandInterpreter, Object )

public:

  /** Some required typedefs. */
  typedef SerialCommunication            CommunicationType;

  /** Set the communication object that commands will be sent to */
  void SetCommunication(CommunicationType* communication);
  
  /** Get the communication object */
  CommunicationType* GetCommunication();

  /** Open communication with the flock. */
  void Open();

  /** Stop and restart the system */
  void Stop();
  void Run();

  /** Close communication with the flock. */
  void Close();

  /** Do a hardware reset of the flock. */ 
  void Reset();

  /** Set the operational hemisphere of the flock. */
  void SetHemisphere(AscensionHemisphere hemisphere);

  /** Set the data format. */
  void SetFormat(AscensionDataFormat format);

  /** Set button state reporting on or off */
  void SetButtonMode(bool mode);

  /** Request a single data record. */
  void Point();

  /** Request that the flock starts streaming data records. */
  void Stream();

  /** Request that the flock stops streaming data records. */
  void EndStream();

  /** Request that the flock stops streaming data records, but
   *  only if it is currently streaming. */
  void EndStreamIfStreaming();

  /** Get the next data record from the flock.
   *
   * This should only be called after a call to Point(), or after
   * streaming has been started.
   */
  void Update();

  /** Get the bird that the most recent data record is for. 
   *
   * It is very important to check this value to make sure that
   * you are getting data for the right bird.
   */ 
  unsigned int GetBird();

  /** Get the position from the most recent data record, if present.
   *
   * The data format must be FB_POSITION_ANGLES, FB_POSITION_MATRIX,
   * FB_POSITION_QUATERNION, or FB_POSITION.
   */ 
  void GetPosition(float xyz[3]);

  /** Get the angles from the most recent data record, if present.
   *
   * The data format must be FB_POSITION_ANGLES or FB_ANGLES.
   */ 
  void GetAngles(float zyx[3]);

  /** Get the rotation matrix from the most recent data record, if present.
   *
   * The data format must be FB_POSITION_MATRIX or FB_MATRIX.  Since
   * FB_POSITION_ANGLES is the native format and is also the most
   * compact, angles should be preferred over matrices for data transfer.
   */ 
  void GetMatrix(float a[9]);

  /** Get the quaternion from the most recent data record, if present.
   *
   * The data format must be FB_POSITION_QUATERNION or FB_QUATERNION. Since
   * FB_POSITION_ANGLES is the native format and is also the most
   * compact, angles should be preferred over quaternions for data transfer.
   */
  void GetQuaternion(float q[4]);

  /** Get the button state for a transducer that has a button attached.
   *
   * This will return zero unless ButtonMode() has been called to turn
   * on button reporting. 
   */
  int GetButton();

  /** Convenience method to convert angles to a matrix. */
  void MatrixFromAngles(float a[9], const float zyx[3]);

  /** Convenience method to convert a matrix into angles. */
  void AnglesFromMatrix(float zyx[3], const float a[9]);

  /** Pass a reset to the Flock of Birds Bus to reset all the birds. */
  void FBBReset();

  /** Configure the flock to work with a certain number of birds. */
  void FBBAutoConfig(unsigned int num);

  /** Direct the next command to a specific bird on the Flock Of Birds Bus. */
  void RS232ToFBB(unsigned int bird);

  /** Send a command to the Flock of Birds. */
  void SendCommand(AscensionCommand command);

  /** Send a command to the Flock of Birds along with some 16-bit data. */
  void SendCommandWords(AscensionCommand command, const short *data);

  /** Send a command to the Flock of Birds along with some 8-bit data. */
  void SendCommandBytes(AscensionCommand command, const char *data);

  /** Examine a parameter value and return the result. */
  int ExamineValue(AscensionParameter parm);

  /** Examine a parameter value and store the result in the supplied array. */
  int ExamineValueWords(AscensionParameter parm, short *data);

  /** Examine a parameter value and store the result in the supplied array. */
  int ExamineValueBytes(AscensionParameter parm, char *data);

  /** Change a parameter to the specified value. */
  void ChangeValue(AscensionParameter parm, int data);

  /** Change a parameter to values in the supplied array. */
  void ChangeValueWords(AscensionParameter parm, const short *data);

  /** Change a parameter to values in the supplied array. */
  void ChangeValueBytes(AscensionParameter parm, const char *data);

  /** Get an integer error code for the flock, zero if no error. 
   *
   * If the error is FB_TIMEOUT_ERROR, the bird itself probably has an error
   * that has halted the streaming of data.  You can check the status of
   * the flock with ExamineValue(fFB_ERROR_CODE).
   */
  AscensionErrorCode GetError();

  /** Get a textual description of the last error from the flock. */
  const char *GetErrorMessage();

  /** Read the button state.
   *
   * You should usually use SetButtonMode() and get the button state
   * with the tracking data records.
   */
  void ButtonRead(int *val);

  /** Utilitity function to unpack two chars from the flock into a short. */
  int GetShort(const char *cp);

  /** Utility function to convert a short into two chars for the flock. */
  void PutShort(char *cp, int val);

protected:

  /** Constructor */
  AscensionCommandInterpreter();

  /** Destructor */
  virtual ~AscensionCommandInterpreter();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Receive a number of raw characters over the serial port. */
  void ReceiveRaw(char *reply, unsigned int len);

  /** Send a number of raw characters over the serial port. */
  void SendRaw(const char *text, unsigned int len);

  /** Utilitity function to unpack two phase-packed chars into a short. */
  int Unpack(char **cp);

  /** Set the error indicator. */
  void SetErrorAndMessage(AscensionErrorCode errorcode, const char *text);

  /* stores last error */
  AscensionErrorCode m_Error;

  /* stores text for last error */
  std::string m_ErrorText;

  /* revision number (quite important) */
  int m_Revision;
  
  /* max examine/change parameter number for this rev */
  AscensionParameter m_MaxParameter;

  /* FBB addressing mode */
  AscensionAddressMode m_AddressMode;

  /* birds are in group mode */
  bool m_GroupMode;

  /* number of running birds */
  unsigned int m_NumberOfBirds;

  /* true if currently stream */
  bool m_StreamData;

  /* true if currently point */
  bool m_PointData;

  /* stores bird address after RS232_TO_FBB */
  unsigned int m_FBBAddress;

  /* the bird which this data set is for */
  unsigned int m_CurrentBird;

  /* position scale mode for each bird */
  AscensionPositionScale m_PositionScale[128];

  /* button mode for each bird */
  bool m_ButtonMode[128];

  /* FB_POSITION, FB_ANGLES, etc for each bird */
  AscensionDataFormat m_DataFormat[128];

  /* place where data is stored after being read */
  char m_DataBuffer[256];

  /* leftover chars after a phase error */
  unsigned int m_PhaseErrorLeftoverBytes;

  /** Serial communication */
  CommunicationType::Pointer m_Communication;
};

} /* end namespace igstk */

#endif /* __igstkAscensionCommandInterpreter_h */
