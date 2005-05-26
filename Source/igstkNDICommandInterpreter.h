/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkNDICommandInterpreter.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __NDICommandInterpreter_h__
#define __NDICommandInterpreter_h__

#include "itkObject.h"
#include "igstkCommunication.h"

namespace igstk
{
/** max tools is 12 active plus 9 passive, so 24 is a safe number  */
/* (note that we are only counting the number of handles that can */
/* be simultaneously occupied)                                    */
#define NDI_MAX_HANDLES 24

/*=====================================================================*/
/** \defgroup NDIErrorCodes NDICommandInterpreter Error Codes 
  The error code is set only by Command() or by 
  macros and functions that call Command().

  Error codes that equal to or less than 0xff are error codes reported
  by the device itself.  Error codes greater than 0xff are
  errors that are reported by the host computer.

  The error code is returned by NDICommandInterpreter::GetError() and
  the corresponding text is available by passing the code to
  NDICommandInterpreter::ErrorString().
*/
  
/*\{*/
#define NDI_OKAY            0x00  /*!<\brief No error */     
#define NDI_INVALID         0x01  /*!<\brief Invalid command */
#define NDI_TOO_LONG        0x02  /*!<\brief Command too long */
#define NDI_TOO_SHORT       0x03  /*!<\brief Command too short */
#define NDI_BAD_COMMAND_CRC 0x04  /*!<\brief Bad CRC calculated for command */
#define NDI_INTERN_TIMEOUT  0x05  /*!<\brief Timeout on command execution */
#define NDI_COMM_FAIL       0x06  /*!<\brief New communication parameters failed */
#define NDI_PARAMETERS      0x07  /*!<\brief Incorrect number of command parameters */
#define NDI_INVALID_PORT    0x08  /*!<\brief Invalid port selected */
#define NDI_INVALID_MODE    0x09  /*!<\brief Invalid mode selected */
#define NDI_INVALID_LED     0x0a  /*!<\brief Invalid LED selected */
#define NDI_LED_STATE       0x0b  /*!<\brief Invalid LED state selected */
#define NDI_BAD_MODE        0x0c  /*!<\brief Command invalid for current mode */
#define NDI_NO_TOOL         0x0d  /*!<\brief No tool plugged in selected port */
#define NDI_PORT_NOT_INIT   0x0e  /*!<\brief Selected port not initialized */
#define NDI_PORT_DISABLED   0x0f  /*!<\brief Selected port not enabled */
#define NDI_INITIALIZATION  0x10  /*!<\brief System not initialized */
#define NDI_TSTOP_FAIL      0x11  /*!<\brief Failure to stop tracking */
#define NDI_TSTART_FAIL     0x12  /*!<\brief Failure to start tracking */
#define NDI_PINIT_FAIL      0x13  /*!<\brief Failure to initialize tool in port */
#define NDI_CAMERA          0x14  /*!<\brief Invalid camera parameters */
#define NDI_INIT_FAIL       0x15  /*!<\brief Failure to initialize */
#define NDI_DSTART_FAIL     0x16  /*!<\brief Failure to start diagnostic mode */
#define NDI_DSTOP_FAIL      0x17  /*!<\brief Failure to stop diagnostic mode */
#define NDI_IRCHK_FAIL      0x18  /*!<\brief Failure to determine environmental IR */
#define NDI_FIRMWARE        0x19  /*!<\brief Failure to read firmware version */
#define NDI_INTERNAL        0x1a  /*!<\brief Internal device error */
#define NDI_IRINIT_FAIL     0x1b /*!<\brief Failure to initialize for IR diagnostics*/
#define NDI_IRED_FAIL       0x1c  /*!<\brief Failure to set marker firing signature */
#define NDI_SROM_FAIL       0x1d  /*!<\brief Failure to search for SROM IDs */
#define NDI_SROM_READ       0x1e  /*!<\brief Failure to read SROM data */
#define NDI_SROM_WRITE      0x1f  /*!<\brief Failure to write SROM data */
#define NDI_SROM_SELECT     0x20  /*!<\brief Failure to select SROM */
#define NDI_PORT_CURRENT    0x21  /*!<\brief Failure to perform tool current test */ 
#define NDI_WAVELENGTH      0x22 /*!<\brief No camera parameters for this wavelength*/
#define NDI_PARAMETER_RANGE 0x23  /*!<\brief Command parameter out of range */
#define NDI_VOLUME          0x24  /*!<\brief No camera parameters for this volume */
#define NDI_FEATURES        0x25  /*!<\brief Failure to determine supported features*/

#define NDI_ENVIRONMENT     0xf1  /*!<\brief Too much environmental infrared */

#define NDI_EPROM_READ      0xf6  /*!<\brief Failure to read Flash EPROM */
#define NDI_EPROM_WRITE     0xf5  /*!<\brief Failure to write Flash EPROM */
#define NDI_EPROM_ERASE     0xf4  /*!<\brief Failure to erase Flash EPROM */

/* error codes returned by the C api */

#define NDI_BAD_CRC         0x0100  /*!<\brief Bad CRC received from device */
#define NDI_OPEN_ERROR      0x0200  /*!<\brief Error opening serial device */
#define NDI_BAD_COMM        0x0300  /*!<\brief Bad communication parameters for host*/
#define NDI_TIMEOUT         0x0400  /*!<\brief device took >5 secs to reply */
#define NDI_WRITE_ERROR     0x0500  /*!<\brief Device write error */
#define NDI_READ_ERROR      0x0600  /*!<\brief Device read error */
#define NDI_RESET_FAIL      0x0700  /*!<\brief device failed to reset on break */
#define NDI_PROBE_FAIL      0x0800  /*!<\brief device not found on specified port */
/*\}*/


/* COMM() baud rates */
/*\{*/
#define  NDI_9600     0          
#define  NDI_14400    1 
#define  NDI_19200    2
#define  NDI_38400    3
#define  NDI_57600    4
#define  NDI_115200   5
/*\}*/

/* COMM() data bits, parity and stop bits */
/*\{*/
#define  NDI_8N1      0              /* 8 data bits, no parity, 1 stop bit */
#define  NDI_8N2      1              /* etc. */
#define  NDI_8O1     10
#define  NDI_8O2     11
#define  NDI_8E1     20
#define  NDI_8E2     21
#define  NDI_7N1    100
#define  NDI_7N2    101
#define  NDI_7O1    110
#define  NDI_7O2    111
#define  NDI_7E1    120
#define  NDI_7E2    121 
/*\}*/

/* COMM() hardware handshaking */
/*\{*/
#define  NDI_NOHANDSHAKE  0
#define  NDI_HANDSHAKE    1
/*\}*/

/* PHSR() handle types */
/*\{*/
#define  NDI_ALL_HANDLES            0x00
#define  NDI_STALE_HANDLES          0x01
#define  NDI_UNINITIALIZED_HANDLES  0x02
#define  NDI_UNENABLED_HANDLES      0x03
#define  NDI_ENABLED_HANDLES        0x04
/*\}*/

/* PENA() tracking modes */
/*\{*/
#define  NDI_STATIC      'S'    /* relatively immobile tool */ 
#define  NDI_DYNAMIC     'D'    /* dynamic tool (e.g. probe) */
#define  NDI_BUTTON_BOX  'B'    /* tool with no IREDs */
/*\}*/

/* GX() reply mode bit definitions */
/*\{*/
#define  NDI_XFORMS_AND_STATUS  0x0001  /* transforms and status */
#define  NDI_ADDITIONAL_INFO    0x0002  /* additional tool transform info */
#define  NDI_SINGLE_STRAY       0x0004  /* stray active marker reporting */
#define  NDI_FRAME_NUMBER       0x0008  /* frame number for each tool */
#define  NDI_PASSIVE            0x8000  /* report passive tool information */
#define  NDI_PASSIVE_EXTRA      0x2000  /* add 6 extra passive tools */
#define  NDI_PASSIVE_STRAY      0x1000  /* stray passive marker reporting */
/*\}*/

/* return values that give the reason behind missing data */
/*\{*/
#define NDI_DISABLED        1  
#define NDI_MISSING         2  
#define NDI_UNOCCUPIED      3
/*\}*/

/* GetTXPortStatus() and GetPSTATPortStatus() return value bits */
/*\{*/
#define  NDI_TOOL_IN_PORT        0x01
#define  NDI_SWITCH_1_ON         0x02
#define  NDI_SWITCH_2_ON         0x04
#define  NDI_SWITCH_3_ON         0x08
#define  NDI_INITIALIZED         0x10
#define  NDI_ENABLED             0x20
#define  NDI_OUT_OF_VOLUME       0x40 /* only for GetGXPortStatus() */
#define  NDI_PARTIALLY_IN_VOLUME 0x80 /* only for GetGXPortStatus() */ 
#define  NDI_CURRENT_DETECT      0x80 /* only for GetPSTATPortStatus() */
/*\}*/

/* GetTXSystemStatus() return value bits */
/*\{*/
#define  NDI_COMM_SYNC_ERROR            0x0001
#define  NDI_TOO_MUCH_EXTERNAL_INFRARED 0x0002
#define  NDI_COMM_CRC_ERROR             0x0004
#define  NDI_COMM_RECOVERABLE           0x0008
#define  NDI_HARDWARE_FAILURE           0x0010
#define  NDI_HARDWARE_CHANGE            0x0020
#define  NDI_PORT_OCCUPIED              0x0040
#define  NDI_PORT_UNOCCUPIED            0x0080
/*\}*/

/* GetGXToolInfo() return value bits */
/*\{*/
#define  NDI_BAD_TRANSFORM_FIT   0x01
#define  NDI_NOT_ENOUGH_MARKERS  0x02
#define  NDI_TOOL_FACE_USED      0x70
/*\}*/

/* GetTXMarkerInfo() return value bits */
/*\{*/
#define  NDI_MARKER_MISSING             0
#define  NDI_MARKER_EXCEEDED_MAX_ANGLE  1
#define  NDI_MARKER_EXCEEDED_MAX_ERROR  2
#define  NDI_MARKER_USED                3
/*\}*/

/* LED() states */
/*\{*/
#define  NDI_BLANK 'B'    /* off */
#define  NDI_FLASH 'F'    /* flashing */
#define  NDI_SOLID 'S'    /* on */
/*\}*/

/* PSTAT() reply mode bits */
/*\{*/
#define  NDI_BASIC           0x0001
#define  NDI_TESTING         0x0002
#define  NDI_PART_NUMBER     0x0004
#define  NDI_ACCESSORIES     0x0008
#define  NDI_MARKER_TYPE     0x0010
#define  NDI_PORT_LOCATION   0x0020
#define  NDI_GPIO_STATUS     0x0040
/*\}*/

/* GetPSTATAccessories() return value bits */
/*\{*/
#define  NDI_TOOL_IN_PORT_SWITCH   0x01  /* tool has tool-in-port switch */
#define  NDI_SWITCH_1              0x02  /* tool has button #1 */
#define  NDI_SWITCH_2              0x04  /* tool has button #2 */
#define  NDI_SWITCH_3              0x08  /* tool has button #3 */
#define  NDI_TOOL_TRACKING_LED     0x10  /* tool has tracking LED */
#define  NDI_LED_1                 0x20  /* tool has LED #1 */
#define  NDI_LED_2                 0x40  /* tool has LED #2 */
#define  NDI_LED_3                 0x80  /* tool has LED #3 */
/*\}*/
  
/* GetPSTATMarkerType() return value, the first 3 bits */
/*\{*/
#define NDI_950NM            0x00
#define NDI_850NM            0x01
/*\}*/

/* GetPSTATMarkerType() return value, the last 5 bits */
/*\{*/
#define NDI_NDI_ACTIVE       0x08
#define NDI_NDI_CERAMIC      0x10
#define NDI_PASSIVE_ANY      0x20
#define NDI_PASSIVE_SPHERE   0x28
#define NDI_PASSIVE_DISC     0x30
/*\}*/

/* tool type specifiers */
/*\{*/
#define NDI_TYPE_REFERENCE   0x01
#define NDI_TYPE_POINTER     0x02
#define NDI_TYPE_BUTTON      0x03
#define NDI_TYPE_SOFTWARE    0x04
#define NDI_TYPE_MICROSCOPE  0x05
#define NDI_TYPE_DOCK        0x08
#define NDI_TYPE_CARM        0x0A
#define NDI_TYPE_CATHETER    0x0B
/*\}*/

/* SSTAT() reply format bits */
/*\{*/
#define NDI_CONTROL         0x0001  /* control processor information */
#define NDI_SENSORS         0x0002  /* sensor processors */
#define NDI_TIU             0x0004  /* TIU processor */
/*\}*/

/* GetSSTATControl() return value bits */
/*\{*/
#define NDI_EPROM_CODE_CHECKSUM     0x01
#define NDI_EPROM_SYSTEM_CHECKSUM   0x02
/*\}*/

/* GetSSTATSensor() return value bits */
/*\{*/
#define NDI_LEFT_ROM_CHECKSUM   0x01
#define NDI_LEFT_SYNC_TYPE_1    0x02
#define NDI_LEFT_SYNC_TYPE_2    0x04
#define NDI_RIGHT_ROM_CHECKSUM  0x10
#define NDI_RIGHT_SYNC_TYPE_1   0x20
#define NDI_RIGHT_SYNC_TYPE_2   0x40
/*\}*/

/* GetSSTATTIU() return value bits */
/*\{*/
#define NDI_ROM_CHECKSUM        0x01
#define NDI_OPERATING_VOLTAGES  0x02
#define NDI_MARKER_SEQUENCING   0x04
#define NDI_SYNC                0x08
#define NDI_COOLING_FAN         0x10
#define NDI_INTERNAL_ERROR      0x20
/*\}*/

/* IRCHK() reply mode bits */
/*\{*/
#define  NDI_DETECTED   0x0001   /* simple yes/no whether IR detected */
#define  NDI_SOURCES    0x0002   /* locations of up to 20 sources per camera */
/*\}*/

/* GetIRCHKNumberOfSources(), GetIRCHKSourceXY() sensor arguments */
/*\{*/
#define  NDI_LEFT   0            /* left sensor */
#define  NDI_RIGHT  1            /* right sensor */
/*\}*/


class NDICommandInterpreter : public itk::Object
{
/** \class NDICommandInterpreter
    \brief Mediate between a Tracker and its Communication object.

    The NDICommandInterpreter serves to purposes: it formats and
    builds a CRC value for commands that are sent to the Polaris or
    Aurora, and it decomponses the replies from the Polaris or Aurora
    into numerical values.

    The AuroraTracker and PolarisTracker classes should create a
    private instance of NDICommandInterpreter, and then pass
    their Communication objects to the NDICommandInterpreter.
*/

public:

  /** Some required typedefs for itk::Object. */

  typedef NDICommandInterpreter          Self;
  typedef itk::Object                    Superclass; 
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  typedef Communication                  CommunicationType;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(NDICommandInterpreter, Object);

  /** Method for creation of a reference counted object. */
  igstkNewMacro(Self);  

  /** Set the communication object that commands will be sent to */
  void SetCommunication(CommunicationType *communication);

  /**
  Send a text command to the device and receive a text reply.

  \param command the command to send, without the trailing CRC

  \return       the text reply from the device with the 
                CRC chopped off

  The standard format of an NDI API command is, for example, "INIT:" or
  "PENA:AD".  A CRC value and a carriage return will be appended to the
  command before it is sent to the device.
  
  This function will automatically recogize certain commands and behave
  accordingly:
  - 0 - A serial break will be sent to the device if the command is a
        null string
  - "COMM:" - After the COMM is sent, the host computer serial port is
           adjusted to match the device.
  - "INIT:" - After the INIT is sent, communication will be paused
           for 100ms.
  - "PHSR:" - The information returned by the PHSR command is stored and can
           be retrieved though the GetPHSR() functions.
  - "PHINF:" - The information returned by the PHINF command is stored and can
           be retrieved though the GetPHINF() functions.
  - "TX:"   - The information returned by the GX command is stored and can
           be retrieved though the GetTX() functions.
  - "GX:"   - The information returned by the GX command is stored and can
           be retrieved though the GetGX() functions.
  - "PSTAT:" - The information returned by the PSTAT command is stored and
           can be retrieved through one of the GetPSTAT() functions.
  - "SSTAT:" - The information returned by the SSTAT command is stored and
           can be retrieved through one of the GetSSTAT() functions.    
  - "IRCHK:" - The information returned by the IRCHK command is stored and
           can be retrieved through the GetIRCHK() functions.

  <p>The GetError() function can be used to check whether an error
  occured.
  */
  const char *Command(const char *command);


  /**
  Send a command to the device using printf-style format string.
  */
  const char *Command(const char *format, int a);
  /**
  Send a command to the device using printf-style format string.
  */
  const char *Command(const char *format, int a, int b);
  /**
  Send a command to the device using printf-style format string.
  */
  const char *Command(const char *format, int a, int b, int c);
  /**
  Send a command to the device using printf-style format string.
  */
  const char *Command(const char *format, int a, int b, int c, int d);
  /**
  Send a command to the device using printf-style format string.
  */
  const char *Command(const char *format, int a, int b, const char *c);
  /**
  Send a command to the device using printf-style format string.
  */
  const char *Command(const char *format, const char *a, const char *b,
                      const char *c, const char *d, const char *e);

  /**
  Cause the device to beep.

  \param n   the number of times to beep, an integer between 1 and 9

  A reply of "0" means that the device is already beeping
  and cannot service this beep request.

  This command can be used in tracking mode.
  */
  void BEEP(int n) {
    this->Command("BEEP:%i", n); }

  /**
  Change the device communication parameters.  The host parameters
  will automatically be adjusted to match.  If the specified baud rate is
  not supported by the serial port, then the error code will be set to
  NDI_BAD_COMM and the device will have to be reset before
  communication can continue.  Most modern UNIX systems accept all baud
  rates except 14400, and Windows systems support all baud rates.

  \param baud        one of NDI_9600, NDI_14400, NDI_19200, NDI_38400,
                     NDI_57600, NDI_115200
  \param dps         should usually be NDI_8N1, the most common mode
  \param handshake   one of NDI_HANDSHAKE or NDI_NOHANDSHAKE
  */
  void COMM(int baud, int dps, int handshake)  {
    this->Command("COMM:%d%03d%d", baud, dps, handshake); }

  /**
  Put the Measurement System into diagnostic mode.  This must be done prior
  to executing the IRCHK() command.  Diagnostic mode is only useful on the POLARIS.
  */
  void DSTART() {
    this->Command("DSTART:"); }

  /**
  Take the Measurement System out of diagnostic mode.
  */
  void DSTOP() {
    this->Command("DSTOP:"); }

  /**
  Request tracking information from the system.  This command is
  only available in tracking mode.  Please note that this command has
  been deprecated in favor of the TX command.

  \param mode a reply mode containing the following bits:
  - NDI_XFORMS_AND_STATUS  0x0001 - transforms and status
  - NDI_ADDITIONAL_INFO    0x0002 - additional tool transform info
  - NDI_SINGLE_STRAY       0x0004 - stray active marker reporting
  - NDI_FRAME_NUMBER       0x0008 - frame number for each tool
  - NDI_PASSIVE            0x8000 - include information for ports 'A', 'B', 'C'
  - NDI_PASSIVE_EXTRA      0x2000 - with NDI_PASSIVE, ports 'A' to 'I'
  - NDI_PASSIVE_STRAY      0x1000 - stray passive marker reporting

  <p>The GX command with the appropriate reply mode is used to update the
  data that is returned by the following functions:
  - int \ref GetGXTransform(int port, double transform[8])
  - int \ref GetGXPortStatus(int port)
  - int \ref GetGXSystemStatus()
  - int \ref GetGXToolInfo(int port)
  - int \ref GetGXMarkerInfo(int port, int marker)
  - int \ref GetGXSingleStray(int port, double coord[3])
  - unsigned long \ref GetGXFrame(int port)
  - int \ref GetGXNumberOfPassiveStrays()
  - int \ref GetGXPassiveStray(int i, double coord[3])
  */
  void GX(int mode) {
    this->Command("GX:%04X", mode); }

  /**
  Initialize the device.  The device must be
  initialized before any other commands are sent.
  */
  void INIT() {
    this->Command("INIT:"); }

  /**
  Check for sources of environmental infrared.

  This command is only valid in diagnostic mode after an IRINIT command.

  \param mode  reply mode bits:
  - NDI_DETECTED   0x0001 - return '1' if IR detected, else '0'
  - NDI_SOURCES    0x0002 - locations of up to 20 sources per camera

  <p>The IRCHK command is used to update the information returned by the
  GetIRCHKDetected() and GetIRCHKSourceXY() functions.
  */
  void IRCHK(int mode) {
    this->Command("IRCHK:%04X", mode); }

  /**
  Initialize the diagnostic environmental infrared checking system.
  This command must be called prior to using the IRCHK command.
  */
  void IRINIT() {
    this->Command("IRINIT:"); }

  /**
  Set a tool LED to a particular state.

  \param ph     valid port handle in the range 0x01 to 0xFF
  \param led    an integer between 1 and 3
  \param state  desired state: NDI_BLANK 'B', NDI_FLASH 'F' or NDI_SOLID 'S'

  This command can be used in tracking mode.
  */
  void LED(int ph, int led, int state) {
    this->Command("LED:%02X%d%c", ph, led, state); }

  /**
  Disable transform reporting on the specified port handle.

  \param ph valid port handle in the range 0x01 to 0xFF
  */
  void PDIS(int ph) {
    this->Command("PDIS:%02X", ph); }

  /**
  Enable transform reporting on the specified port handle.

  \param ph valid port handle in the range 0x01 to 0xFF
  \param mode one of NDI_STATIC 'S', NDI_DYNAMIC 'D' or NDI_BUTTON_BOX 'B'
  */
  void PENA(int ph, int mode) {
    this->Command("PENA:%02X%c", ph, mode); }

  /**
  Free the specified port handle.

  \param ph valid port handle in the range 0x01 to 0xFF 
  */
  void PHF(int ph) {
    this->Command("PHF:%02X", ph); }

  /**
  Ask the device for information about a tool handle.

  \param ph valid port handle in the range 0x01 to 0xFF 

  \param format  a reply format mode composed of the following bits:
  - NDI_BASIC           0x0001 - get port status and basic tool information
  - NDI_TESTING         0x0002 - get current test for active tools
  - NDI_PART_NUMBER     0x0004 - get a 20 character part number
  - NDI_ACCESSORIES     0x0008 - get a summary of the tool accessories
  - NDI_MARKER_TYPE     0x0010 - get the tool marker type
  - NDI_PORT_LOCATION   0x0020 - get the physical port location
  - NDI_GPIO_STATUS     0x0040 - get AURORA GPIO status

  <p>The use of the PHINF command with the appropriate reply format updates
  the information returned by the following commands:
  - int \ref GetPHINFPortStatus()
  - int \ref GetPHINFToolInfo(char information[30])
  - unsigned long \ref GetPHINFCurrentTest()
  - int \ref GetPHINFAccessories()
  - int \ref GetPHINFMarkerType()

  <p>This command is not available during tracking mode.
  */
  void PHINF(int ph, int format) {
    this->Command("PHINF:%02X%04X", ph, format); }

  /**
  Requeset a port handle given specific tool criteria.

  \param  num    8-digit device number or wildcard "********"
  \param  sys    system type TIU "0" or AURORA SCU "1" or wildcard "*"
  \param  tool   wired "0" or wireless "1" or wildcard "*" 
  \param  port   wired "01" to "10", wireless "0A" to "0I" or wildcard "**"
  \param  chan   AURORA tool channel "00" or "01" or wildcard "**"

  <p>The use of the PHRQ command updates the information returned by the
  following commands:
  - int \ref GetPHRQHandle()
  */
  void PHRQ(const char *num, const char *sys, const char *tool,
            const char *port, const char *chan) {
    this->Command("PHRQ:%-8.8s%1.1s%1.1s%2.2s%2.2s", num, sys, tool, port, chan); }

  /**
  List the port handles.

  \param mode   the reply mode:
  - NDI_ALL_HANDLES            0x00 - return all handles
  - NDI_STALE_HANDLES          0x01 - only handles waiting to be freed
  - NDI_UNINITIALIZED_HANDLES  0x02 - handles needing initialization
  - NDI_UNENABLED_HANDLES      0x03 - handles needing enabling
  - NDI_ENABLED_HANDLES        0x04 - handles that are enabled

  <p>The use of the PHSR command with the appropriate reply format updates
  the information returned by the following commands:
  - int \ref GetPHSRNumberOfHandles()
  - int \ref GetPHSRHandle(int i)
  - int \ref GetPHSRInformation(int i)

  <p>This command is not available during tracking mode.
  */
  void PHSR(int mode) {
    this->Command("PHSR:%02X", mode); }

  /**
  Initialize the tool on the specified port handle.

  \param ph valid port handle in the range 0x01 to 0xFF 
  */
  void PINIT(int ph) {
    this->Command("PINIT:%02X", ph); }

  /**
  Set the three GPIO wire states for an AURORA tool.
  The states available are 'N' (no change), 'S' (solid on),
  'P' (pulse) and 'O' (off).

  \param ph  valid port handle in the range 0x01 to 0xFF
  \param a   GPIO 1 state
  \param b   GPIO 2 state
  \param c   GPIO 3 state
  */
  void PSOUT(int ph, int a, int b, int c) {
    this->Command("PSOUT:%02X%c%c%c", ph, a, b, c); };

  /**
  Ask for information about the tool ports.  This command has been
  deprecated in favor of the PHINF command.  

  \param format  a reply format mode composed of the following bits:
  - NDI_BASIC           0x0001 - get port status and basic tool information
  - NDI_TESTING         0x0002 - get current test for active tools
  - NDI_PART_NUMBER     0x0004 - get a 20 character part number
  - NDI_ACCESSORIES     0x0008 - get a summary of the tool accessories
  - NDI_MARKER_TYPE     0x0010 - get the tool marker type
  - NDI_PASSIVE         0x8000 - include information for ports 'A', 'B', 'C'
  - NDI_PASSIVE_EXTRA   0x2000 - with NDI_PASSIVE, ports 'A' to 'I'

  <p>The use of the PSTAT command with the appropriate reply format updates
  the information returned by the following commands:
  - int \ref GetPSTATPortStatus(int port)
  - int \ref GetPSTATToolInfo(int port,
                                   char information[30])
  - unsigned long \ref GetPSTATCurrentTest(int port)
  - int \ref GetPSTATAccessories(int port)
  - int \ref GetPSTATMarkerType(int port)

  <p>This command is not available during tracking mode.
  */
  void PSTAT(int format) {
    this->Command("PSTAT:%04X", format); }

  /**
  Clear the virtual SROM for the specified port.  For a passive tool,
  this is equivalent to unplugging the tool.  This command has been
  deprecated in favor of PHF.

  \param port one of '1', '2', '3' or 'A' to 'I'
  */
  void PVCLR(int port) {
    this->Command("PVCLR:%c", port); }

  /**
  Write to a virtual SROM address on the specified port handle.
  Note that the PVWRFromFile() allows direct reading of the SROM
  information from a .rom file..

  \param ph valid port handle in the range 0x01 to 0xFF 
  \param a an address between 0x0000 and 0x07C0
  \param x 64-byte data array encoded as a 128-character hexadecimal string
  */
  void PVWR(int ph, int a, const char *x) {
    this->Command("PVWR:%02X%04X%.128s", ph, a, x); }

  /**
  Send a serial break to reset the device.  If the reset was not
  successful, the error code will be set to NDI_RESET_FAIL.
  */
  void RESET() {
    this->Command(0); }

  /**
  Get a feature list for this device.

  \param mode  the desired reply mode
  - 0x00 - 32-bit feature summary encoded as 8 hexadecimal digits
  - 0x01 - the number of active tool ports as a single digit
  - 0x02 - the number of passive tool ports as a single digit
  - 0x03 - list of volumes available (see NDI documentation)
  - 0x04 - the number of ports that support tool-in-port current sensing

  <p>The feature summary bits are defined as follow:
  - 0x00000001 - active tool ports are available
  - 0x00000002 - passive tool ports are available
  - 0x00000004 - multiple volumes are available
  - 0x00000008 - tool-in-port current sensing is available
  */
  void SFLIST(int mode) {
    this->Command("SFLIST:%02X", mode); }

  /**
  Put the device into tracking mode.
  */
  void TSTART() {
    this->Command("TSTART:"); }

  /**
  Take the device out of tracking mode.
  */
  void TSTOP() {
    this->Command("TSTOP:"); }

  /**
  Request tracking information from the device.  This command is
  only available in tracking mode.

  \param mode a reply mode containing the following bits:
  - NDI_XFORMS_AND_STATUS  0x0001 - transforms and status
  - NDI_ADDITIONAL_INFO    0x0002 - additional tool transform info
  - NDI_SINGLE_STRAY       0x0004 - stray active marker reporting
  - NDI_PASSIVE_STRAY      0x1000 - stray passive marker reporting

  <p>The TX command with the appropriate reply mode is used to update the
  data that is returned by the following functions:
  - int \ref GetTXTransform(int ph, double transform[8])
  - int \ref GetTXPortStatus(int ph)
  - unsigned long \ref GetTXFrame(int ph)
  - int \ref GetTXToolInfo(int ph)
  - int \ref GetTXMarkerInfo(int ph, int marker)
  - int \ref GetTXSingleStray(int ph, double coord[3])
  - int \ref GetTXNumberOfPassiveStrays()
  - int \ref GetTXPassiveStray(int i, double coord[3])
  - int \ref GetTXSystemStatus()
  */
  void TX(int mode) {
    this->Command("TX:%04X", mode); }

  /**
  Get a string that describes the device firmware version.

  \param n   the processor to get the firmware revision of:
  - 0 - control firmware
  - 1 - left sensor firmware
  - 2 - right sensor firmware
  - 3 - TIU firmware
  - 4 - control firmware with enhanced versioning
  */
  const char *VER(int n) {
    return this->Command("VER:%d", n); }

  /** 
  Write data from a ROM file into the virtual SROM for the specified port.

  \param ph        valid port handle in the range 0x01 to 0xFF
  \param filename  file to read the SROM data from

  \return  NDI_OKAY if the write was successful

  If the return value is not NDI_OKAY but GetError() returns NDI_OKAY,
  then the ROM file could not be read and no information was written
  to the device.

  This function uses the PVWR command to write the SROM.  The total size
  of the virtual SROM is 1024 bytes.  If the file is shorter than this,
  then zeros will be written to the remaining space in the SROM.
  */
  int PVWRFromFile(int ph, const char *filename);

  /**
  Get error code from the last command.  An error code of NDI_OKAY signals
  that no error occurred.  The error codes are listed in \ref NDIErrorCodes.
  */
  int GetError() const;

  /**
  Get the port handle returned by a PHRQ command.

  \return  a port handle between 0x01 and 0xFF

  <p>An SROM can be written to the port handle wit the PVWR command.
  */
  int GetPHRQHandle() const;

  /**
  Get the number of port handles as returned by a PHSR command.

  \return  an integer, the maximum possible value is 255
  */
  int GetPHSRNumberOfHandles() const;

  /**
  Get one of the port handles returned by a PHSR command.

  \param i         a value between 0 and \em n where \em n is the
                   value returned by GetPHSRNumberOfHandles().

  \return  a port handle between 0x01 and 0xFF

  <p>The PHINF command can be used to get detailed information about the
   port handle.
  */
  int GetPHSRHandle(int i) const;

  /**
  Get the information for a port handle returned by a PHSR command.

  \param i         a value between 0 and \em n where \em n is the
                   value returned by GetPHSRNumberOfHandles().

  \return  a 12-bit bitfield where the following bits are defined:
  - NDI_TOOL_IN_PORT        0x01 - there is a tool in the port
  - NDI_SWITCH_1_ON         0x02 - button 1 is pressed
  - NDI_SWITCH_2_ON         0x04 - button 2 is pressed
  - NDI_SWITCH_3_ON         0x08 - button 3 is pressed
  - NDI_INITIALIZED         0x10 - tool port has been initialized
  - NDI_ENABLED             0x20 - tool port is enabled for tracking
  - NDI_CURRENT_DETECT      0x80 - tool sensed through current detection

  <p>The PHINF command can be used to get detailed information about the
   port handle.
  */
  int GetPHSRInformation(int i) const;

  /**
  Get the 8-bit status value for the port handle.

   \return an integer composed of the following bits:
  - NDI_TOOL_IN_PORT        0x01 - there is a tool in the port
  - NDI_SWITCH_1_ON         0x02 - button 1 is pressed
  - NDI_SWITCH_2_ON         0x04 - button 2 is pressed
  - NDI_SWITCH_3_ON         0x08 - button 3 is pressed
  - NDI_INITIALIZED         0x10 - tool port has been initialized
  - NDI_ENABLED             0x20 - tool port is enabled for tracking
  - NDI_CURRENT_DETECT      0x80 - tool sensed through current detection

  <p>The return value is updated only when a PHINF command is sent with
  the NDI_BASIC (0x0001) bit set in the reply mode.
  */
  int GetPHINFPortStatus() const;

  /**
  Get a 31-byte string describing the tool.

  \param information array that information is returned in (the
                     resulting string is not null-terminated)

  \return  one of:
  - NDI_OKAY - information was returned
  - NDI_UNOCCUPIED - port is unoccupied or no information is available

  <p>The returned string will not be null-terminated by default.  You
  must set information[31] to 0 in order to terminate the string.
  If the port is unoccupied then the contents of the \em information
  string are undefined.
  
  The information is updated only when a PHINF command is sent with
  the NDI_BASIC (0x0001) bit set in the reply mode.
  */
  int GetPHINFToolInfo(char information[31]) const;

  /**
  Return the results of a current test on the IREDS on an active 
  POLARIS tool.

  \return 32-bit integer (see NDI documentation)

  The information is updated only when a PHINF command is sent with
  the NDI_TESTING (0x0002) bit set in the reply mode.
  */
  unsigned long GetPHINFCurrentTest() const;

  /**
  Get a 20-byte string that contains the part number of the tool.

  \param part        array that part number is returned in (the
                     resulting string is not null-terminated)

  \return  one of:
  - NDI_OKAY - information was returned
  - NDI_UNOCCUPIED - port is unoccupied or no information is available

  <p>If a terminated string is required, then set part[20] to 0
  before calling this function.

  The information is updated only when a PHINF command is sent with
  the NDI_PART_NUMBER (0x0004) bit set in the reply mode.
  */
  int GetPHINFPartNumber(char part[20]) const;

  /**
  Get the 8-bit value specifying the tool accessories.

  \return an integer composed of the following bits:
  - NDI_TOOL_IN_PORT_SWITCH   0x01  - tool has tool-in-port switch
  - NDI_SWITCH_1              0x02  - tool has button 1
  - NDI_SWITCH_2              0x04  - tool has button 2
  - NDI_SWITCH_3              0x08  - tool has button 3
  - NDI_TOOL_TRACKING_LED     0x10  - tool has tracking LED
  - NDI_LED_1                 0x20  - tool has LED 1
  - NDI_LED_2                 0x40  - tool has LED 2
  - NDI_LED_3                 0x80  - tool has LED 3

  <p>If there is no available information for the specified port, or if
  an illegal port specifier value is used, the return value is zero.

  The return value is updated only when a PHINF command is sent with
  the NDI_ACCESSORIES (0x0008) bit set in the reply mode.
  */
  int GetPHINFAccessories() const;

  /**
  Get an 8-bit value describing the marker type for the tool.
  The low three bits descibe the wavelength, and the high three
  bits are the marker type code.

  \return  see NDI documentation for more information:
  - low bits:
    - NDI_950NM            0x00
    - NDI_850NM            0x01
  - high bits:
    - NDI_NDI_ACTIVE       0x08
    - NDI_NDI_CERAMIC      0x10
    - NDI_PASSIVE_ANY      0x20
    - NDI_PASSIVE_SPHERE   0x28
    - NDI_PASSIVE_DISC     0x30

  <p>The return value is updated only when a PHINF command is sent with
  the NDI_MARKER_TYPE (0x0010) bit set in the reply mode.
  */
  int GetPHINFMarkerType() const;

  /**
  Get a 14-byte description of the physical location of the tool
  on the system.

  \return  see NDI documentation for more information:
  - 8 chars: device number
  - 1 char: 0 = POLARIS, 1 = AURORA
  - 1 char: 0 = wired, 1 = wireless
  - 2 chars: port number 01-12 or 0A-0I
  - 2 chars: hexadecimal channel number

  <p>The return value is updated only when a PHINF command is sent with
  the NDI_PORT_LOCATION_TYPE (0x0020) bit set in the reply mode.
  */
  int GetPHINFPortLocation(char location[14]) const;

  /**
  Get the 8-bit GPIO status for this tool.

  \return  an 8-bit integer, see NDI documentation for more information.

  <p>The return value is updated only when a PHINF command is sent with
  the NDI_GPIO_STATUS (0x0040) bit set in the reply mode.
  */
  int GetPHINFGPIOStatus() const;

  /**
  Get the transformation for the specified port.
  The first four numbers are a quaternion, the next three numbers are
  the coodinates in millimetres, and the final number
  is a unitless error estimate.

  \param ph        valid port handle in range 0x01 to 0xFF
  \param transform space for the 8 numbers in the transformation
  
  \return one of the following: 
  - NDI_OKAY if successful
  - NDI_DISABLED if tool port is nonexistent or disabled
  - NDI_MISSING if tool transform cannot be computed

  <p>If NDI_DISABLED or NDI_MISSING is returned, then the values in the
  supplied transform array will be left unchanged.

  The transformations for each of the port handles remain the same
  until the next TX command is sent to the device.
  */ 
  int GetTXTransform(int ph, double transform[8]) const;

  /**
  Get the 16-bit status value for the specified port handle.

  \param ph        valid port handle in range 0x01 to 0xFF

  \return status bits or zero if there is no information:
  - NDI_TOOL_IN_PORT        0x0001
  - NDI_SWITCH_1_ON         0x0002
  - NDI_SWITCH_2_ON         0x0004
  - NDI_SWITCH_3_ON         0x0008
  - NDI_INITIALIZED         0x0010
  - NDI_ENABLED             0x0020
  - NDI_OUT_OF_VOLUME       0x0040
  - NDI_PARTIALLY_IN_VOLUME 0x0080

  This information is updated each time that the TX command
  is sent to the device.
  */
  int GetTXPortStatus(int ph) const;

  /**
  Get the camera frame number for the latest transform.

  \param ph        valid port handle in range 0x01 to 0xFF

  \return a 32-bit frame number, or zero if no information was available

  This information is updated each time that the TX command
  is sent to the device.
  */
  unsigned long GetTXFrame(int ph) const;

  /**
  Get additional information about the tool transformation.

  \param ph        valid port handle in range 0x01 to 0xFF

  \return status bits, or zero if there is no information available
  - NDI_BAD_TRANSFORM_FIT   0x01
  - NDI_NOT_ENOUGH_MARKERS  0x02
  - NDI_TOOL_FACE_USED      0x70 - 3 bits give 8 possible faces

  <p>The tool information is only updated when the TX command is called with
  the NDI_ADDITIONAL_INFO (0x0002) mode bit.
  */
  int GetTXToolInfo(int ph) const;

  /**
  Get additional information about the tool markers.

  \param ph        valid port handle in range 0x01 to 0xFF
  \param marker    one of 'A' through 'T' for the 20 markers

  \return status bits, or zero if there is no information available
  - NDI_MARKER_MISSING             0
  - NDI_MARKER_EXCEEDED_MAX_ANGLE  1
  - NDI_MARKER_EXCEEDED_MAX_ERROR  2
  - NDI_MARKER_USED                3  

  <p>The tool marker information is only updated when the TX command is
  called with the NDI_ADDITIONAL_INFO (0x0002) mode bit set.
  */
  int GetTXMarkerInfo(int ph, int marker) const;

  /**
  Get the coordinates of a stray marker on a wired POLARIS tool.
  This command is only meaningful for tools that have a stray
  marker.

  \param ph        valid port handle in range 0x01 to 0xFF
  \param coord     array to hold the three coordinates

  \return the return value will be one of
  - NDI_OKAY - values returned in coord
  - NDI_DISABLED - port disabled or illegal port specified
  - NDI_MISSING - stray marker is not visible to the device

  <p>The stray marker position is only updated when the GX command is
  called with the NDI_SINGLE_STRAY (0x0004) bit set.
  */
  int GetTXSingleStray(int ph, double coord[3]) const;

  /**
  Get the number of passive stray markers detected.

  \return          a number between 0 and 20
  
  The passive stray marker coordinates are updated when a TX command
  is sent with the NDI_PASSIVE_STRAY (0x1000) bit set in the reply mode.
  */
  int GetTXNumberOfPassiveStrays() const;

  /**
  Copy the coordinates of the specified stray marker into the
  supplied array.

  \param i         a number between 0 and 19
  \param coord     array to hold the coordinates
  \return          one of:
  - NDI_OKAY - information was returned in coord
  - NDI_DISABLED - no stray marker reporting is available
  - NDI_MISSING - marker number i is not visible

  <p>Use GetTXNumberOfPassiveStrays() to get the number of stray
  markers that are visible.
  
  The passive stray marker coordinates are updated when a TX command
  is sent with the NDI_PASSIVE_STRAY (0x1000) bit set in the reply mode.
  */
  int GetTXPassiveStray(int i, double coord[3]) const;

  /**
  Get an 16-bit status bitfield for the system.

  \return status bits or zero if there is no information:
  - NDI_COMM_SYNC_ERROR            0x0001
  - NDI_TOO_MUCH_EXTERNAL_INFRARED 0x0002
  - NDI_COMM_CRC_ERROR             0x0004
  - NDI_COMM_RECOVERABLE           0x0008
  - NDI_HARDWARE_FAILURE           0x0010
  - NDI_HARDWARE_CHANGE            0x0020
  - NDI_PORT_OCCUPIED              0x0040
  - NDI_PORT_UNOCCUPIED            0x0080

  <p>The system stutus information is updated whenever the TX command is
  called with the NDI_XFORMS_AND_STATUS (0x0001) bit set in the reply mode.
  */
  int GetTXSystemStatus() const;

  /**
  Get the transformation for the specified port.
  The first four numbers are a quaternion, the next three numbers are
  the coodinates in millimetres, and the final number
  is a unitless error estimate.

  \param port      one of '1', '2', '3' or 'A' to 'I'
  \param transform space for the 8 numbers in the transformation
  
  \return one of the following: 
  - NDI_OKAY if successful
  - NDI_DISABLED if tool port is nonexistent or disabled
  - NDI_MISSING if tool transform cannot be computed

  <p>If NDI_DISABLED or NDI_MISSING is returned, then the values in the
  supplied transform array will be left unchanged.

  The transforms for ports '1', '2', '3' are updated whenever the
  GX comand is sent with the NDI_XFORMS_AND_STATUS (0x0001) bit set
  in the reply mode.
  The passive ports 'A', 'B', 'C' are updated if the NDI_PASSIVE (0x8000)
  bit is also set, and ports 'A' though 'I' are updated if both
  NDI_PASSIVE (0x8000) and NDI_PASSIVE_EXTRA (0x2000) are also set.

  The transformation for any particular port will remain unchanged
  until it is updated by a GX command with an appropriate reply mode
  as specified above.
  */ 
  int GetGXTransform(int port, double transform[8]) const;

  /**
  Get the 8-bit status value for the specified port.

  \param port      one of '1', '2', '3' or 'A' to 'I'

  \return status bits or zero if there is no information:
  - NDI_TOOL_IN_PORT        0x01
  - NDI_SWITCH_1_ON         0x02
  - NDI_SWITCH_2_ON         0x04
  - NDI_SWITCH_3_ON         0x08
  - NDI_INITIALIZED         0x10
  - NDI_ENABLED             0x20
  - NDI_OUT_OF_VOLUME       0x40
  - NDI_PARTIALLY_IN_VOLUME 0x80

  The status of the ports is updated according to the same rules as
  specified for GetGXTransform().
  */
  int GetGXPortStatus(int port) const;

  /**
  Get an 8-bit status bitfield for the system.

  \return status bits or zero if there is no information:
  - NDI_COMM_SYNC_ERROR            0x01
  - NDI_TOO_MUCH_EXTERNAL_INFRARED 0x02
  - NDI_COMM_CRC_ERROR             0x04

  <p>The system stutus information is updated whenever the GX command is
  called with the NDI_XFORMS_AND_STATUS (0x0001) bit set in the reply mode.
  */
  int GetGXSystemStatus() const;

  /**
  Get additional information about the tool transformation.

  \param port      one of '1', '2', '3' or 'A' to 'I'

  \return status bits, or zero if there is no information available
  - NDI_BAD_TRANSFORM_FIT   0x01
  - NDI_NOT_ENOUGH_MARKERS  0x02
  - NDI_TOOL_FACE_USED      0x70 - 3 bits give 8 possible faces

  <p>The tool information is only updated when the GX command is called with
  the NDI_ADDITIONAL_INFO (0x0002) mode bit, and then only for the ports
  specified by the NDI_PASSIVE (0x8000) and NDI_PASSIVE_EXTRA (0x2000) bits.
  */
  int GetGXToolInfo(int port) const;

  /**
  Get additional information about the tool markers.

  \param port      one of '1', '2', '3' or 'A' to 'I'
  \param marker    one of 'A' through 'T' for the 20 markers

  \return status bits, or zero if there is no information available
  - NDI_MARKER_MISSING             0
  - NDI_MARKER_EXCEEDED_MAX_ANGLE  1
  - NDI_MARKER_EXCEEDED_MAX_ERROR  2
  - NDI_MARKER_USED                3  

  <p>The tool marker information is only updated when the GX command is
  called with the NDI_ADDITIONAL_INFO (0x0002) mode bit set, and then only
  for the ports specified by the NDI_PASSIVE (0x8000) and
  NDI_PASSIVE_EXTRA (0x2000) bits.
  */
  int GetGXMarkerInfo(int port, int marker) const;

  /**
  Get the coordinates of a stray marker on an active tool.
  This command is only meaningful for active tools that have a stray
  marker.

  \param port      one of '1', '2', '3'
  \param coord     array to hold the three coordinates

  \return the return value will be one of
  - NDI_OKAY - values returned in coord
  - NDI_DISABLED - port disabled or illegal port specified
  - NDI_MISSING - stray marker is not visible to the device

  <p>The stray marker position is only updated when the GX command is
  called with the NDI_SINGLE_STRAY (0x0004) bit set.
  */
  int GetGXSingleStray(int port, double coord[3]) const;

  /**
  Get the camera frame number for the latest transform.

  \param port      one of '1', '2', '3' (active ports only)

  \return a 32-bit frame number, or zero if no information was available

  The frame number is only updated when the GX command is called with
  the NDI_FRAME_NUMBER (0x0008) bit, and then only for the ports specified
  by the NDI_PASSIVE (0x8000) and NDI_PASSIVE_EXTRA (0x2000) bits.
  */
  unsigned long GetGXFrame(int port) const;

  /**
  Get the number of passive stray markers detected.

  \return          a number between 0 and 20
  
  The passive stray information is updated when a GX command is sent
  with the NDI_PASSIVE_STRAY (0x1000) and NDI_PASSIVE (0x8000) bits set
  in the reply mode.  The information persists until the next time GX is
  sent with these bits set.

  If no information is available, the return value is zero.
  */
  int GetGXNumberOfPassiveStrays() const;

  /**
  Copy the coordinates of the specified stray marker into the
  supplied array.

  \param i         a number between 0 and 19
  \param coord     array to hold the coordinates
  \return          one of:
  - NDI_OKAY - information was returned in coord
  - NDI_DISABLED - no stray marker reporting is available
  - NDI_MISSING - marker number i is not visible

  <p>Use GetGXNumberOfPassiveStrays() to get the number of stray
  markers that are visible.
  
  The passive stray marker coordinates are updated when a GX command
  is sent with the NDI_PASSIVE_STRAY (0x1000) and NDI_PASSIVE (0x8000)
  bits set in the reply mode.  The information persists until the next
  time GX is sent with these bits set.
  */
  int GetGXPassiveStray(int i, double coord[3]) const;

  /**
  Get the 8-bit status value for the specified port.

  \param port      one of '1', '2', '3' or 'A' to 'I'

  \return an integer composed of the following bits:
  - NDI_TOOL_IN_PORT        0x01 - there is a tool in the port
  - NDI_SWITCH_1_ON         0x02 - button 1 is pressed
  - NDI_SWITCH_2_ON         0x04 - button 2 is pressed
  - NDI_SWITCH_3_ON         0x08 - button 3 is pressed
  - NDI_INITIALIZED         0x10 - tool port has been initialized
  - NDI_ENABLED             0x20 - tool port is enabled for tracking
  - NDI_CURRENT_DETECT      0x80 - tool sensed through current detection

  <p>If there is no available information for the specified port, or if
  an illegal port specifier value is used, the return value is zero.
  The return value is updated only when a PSTAT command is sent with
  the NDI_BASIC (0x0001) bit set in the reply mode.
  */
  int GetPSTATPortStatus(int port) const;

  /**
  Get a 30-byte string describing the tool in the specified port.

  \param port        one of '1', '2', '3' or 'A' to 'I'
  \param information array that information is returned in (the
                     resulting string is not null-terminated)

  \return  one of:
  - NDI_OKAY - information was returned
  - NDI_UNOCCUPIED - port is unoccupied or no information is available

  <p>The returned string will not be null-terminated by default.  You
  must set information[30] to 0 in order to terminate the string.
  If the port is unoccupied then the contents of the \em information
  string are undefined.
  
  The information is updated only when a PSTAT command is sent with
  the NDI_BASIC (0x0001) bit set in the reply mode.
  */
  int GetPSTATToolInfo(int port, char information[30]) const;

  /**
  Return the results of a current test on the IREDS on the specified
  tool.  

  \param  port        one of '1', '2', '3'

  \return 32-bit integer (see NDI documentation)

  The information is updated only when a PSTAT command is sent with
  the NDI_TESTING (0x0002) bit set in the reply mode.
  */
  unsigned long GetPSTATCurrentTest(int port) const;

  /**
  Get a 20-byte string that contains the part number of the tool.

  \param port        one of '1', '2', '3' or 'A' to 'I'
  \param part        array that part number is returned in (the
                     resulting string is not null-terminated)

  \return  one of:
  - NDI_OKAY - information was returned
  - NDI_UNOCCUPIED - port is unoccupied or no information is available

  <p>If a terminated string is required, then set part[20] to 0
  before calling this function.

  The information is updated only when a PSTAT command is sent with
  the NDI_PART_NUMBER (0x0004) bit set in the reply mode.
  */
  int GetPSTATPartNumber(int port, char part[20]) const;

  /**
  Get the 8-bit value specifying the accessories for the specified tool.

  \param port      one of '1', '2', '3' (active ports only)

  \return an integer composed of the following bits:
  - NDI_TOOL_IN_PORT_SWITCH   0x01  - tool has tool-in-port switch
  - NDI_SWITCH_1              0x02  - tool has button 1
  - NDI_SWITCH_2              0x04  - tool has button 2
  - NDI_SWITCH_3              0x08  - tool has button 3
  - NDI_TOOL_TRACKING_LED     0x10  - tool has tracking LED
  - NDI_LED_1                 0x20  - tool has LED 1
  - NDI_LED_2                 0x40  - tool has LED 2
  - NDI_LED_3                 0x80  - tool has LED 3

  <p>If there is no available information for the specified port, or if
  an illegal port specifier value is used, the return value is zero.
  The return value is always zero for passive ports.

  The return value is updated only when a PSTAT command is sent with
  the NDI_ACCESSORIES (0x0008) bit set in the reply mode.
  */
  int GetPSTATAccessories(int port) const;

  /**
  Get an 8-bit value describing the marker type for the tool.
  The low three bits descibe the wavelength, and the high three
  bits are the marker type code.

  \param port      one of '1', '2', '3' (active ports only)

  \return  see NDI documentation for more information:
  - low bits:
    - NDI_950NM            0x00
    - NDI_850NM            0x01
  - high bits:
    - NDI_NDI_ACTIVE       0x08
    - NDI_NDI_CERAMIC      0x10
    - NDI_PASSIVE_ANY      0x20
    - NDI_PASSIVE_SPHERE   0x28
    - NDI_PASSIVE_DISC     0x30

  <p>If there is no available information for the specified port, or if
  an illegal port specifier value is used, the return value is zero.

  The return value is updated only when a PSTAT command is sent with
  the NDI_MARKER_TYPE (0x0010) bit set in the reply mode.
  */
  int GetPSTATMarkerType(int port) const;

  /**
  Get the status of the control processor.

  \return an int with the following bit definitions for errors:
  - NDI_EPROM_CODE_CHECKSUM     0x01
  - NDI_EPROM_SYSTEM_CHECKSUM   0x02

  <p>This information is updated only when the SSTAT command is sent
  with the NDI_CONTROL (0x0001) bit set in the reply mode.
  */
  int GetSSTATControl() const;

  /**
  Get the status of the sensor processors.

  \return an int with the following bit definitions for errors:
  - NDI_LEFT_ROM_CHECKSUM   0x01
  - NDI_LEFT_SYNC_TYPE_1    0x02
  - NDI_LEFT_SYNC_TYPE_2    0x04
  - NDI_RIGHT_ROM_CHECKSUM  0x10
  - NDI_RIGHT_SYNC_TYPE_1   0x20
  - NDI_RIGHT_SYNC_TYPE_2   0x40

  <p>This information is updated only when the SSTAT command is sent
  with the NDI_SENSORS (0x0002) bit set in the reply mode.
  */
  int GetSSTATSensors() const;

  /**
  Get the status of the sensor processors.

  \return an int with the following bit definitions for errors:
  - NDI_ROM_CHECKSUM        0x01
  - NDI_OPERATING_VOLTAGES  0x02
  - NDI_MARKER_SEQUENCING   0x04
  - NDI_SYNC                0x08
  - NDI_COOLING_FAN         0x10
  - NDI_INTERNAL_ERROR      0x20

  <p>This information is updated only when the SSTAT command is sent
  with the NDI_TIU (0x0004) bit set in the reply mode.
  */
  int GetSSTATTIU() const;

  /**
  Check to see whether environmental infrared was detected.
 
  \return       1 if infrared was detected and 0 otherwise.

  This information is only updated if the IRCHK command is called
  with the NDI_DETECTED (0x0001) format bit set.
  */
  int GetIRCHKDetected() const;

  /**
  Get the number of infrared sources seen by one of the two sensors.

  \param side   one of NDI_LEFT or NDI_RIGHT

  return  the number of infrared sources seen by the specified sensor

  This information is valid only immediately after the IRCHK command
  has been called with the NDI_SOURCES (0x0002) format bit set.  Otherwise,
  the return value will be zero.
  */
  int GetIRCHKNumberOfSources(int side) const;

  /**
  Get the coordinates of one of the infrared sources seen by one of
  the two sensors. 

  \param side   one of NDI_LEFT or NDI_RIGHT
  \param i      the source to get the coordinates for
  \param xy     space to store the returned coordinates

  return  NDI_OKAY or NDI_MISSING

  If there is no available information, then the xy array will be left
  unchanged and the value NDI_MISSING will be returned.  Otherwise,
  the return value will be NDI_OKAY.

  This information is valid only immediately after the IRCHK command
  has been called with the NDI_SOURCES (0x0002) format bit set.
  */
  int GetIRCHKSourceXY(int side, int i, double xy[2]) const;

  /**
  Convert an error code returned by ndiGetError() into a string that
  describes the error.  The error codes are listed in \ref NDIErrorCodes.

  An unrecognized error code will return "Unrecognized error code".
  */
  static const char *ErrorString(int errnum);

  /*!
  This function is used to convert raw binary data into a stream of
  hexadecimal digits that can be sent to the device.
  The length of the output string will be twice the number of bytes
  in the input data, since each byte will be represented by two
  hexadecimal digits.

  As a convenience, the return value is a pointer to the hexadecimal
  string.  If the string must be terminated, then set cp[2*n] to 0
  before calling this function, otherwise the string will be left
  unterminated.
  */
  static char *HexEncode(char *cp, const void *data, int n);

  /*!
  This function converts a hex-encoded string into binary data.
  This can be used to decode the SROM data sent from the device.
  The length of the input string must be twice the expected number
  of bytes in the output data, since each binary byte is ecoded by
  two hexadecimal digits.

  As a convenience, the return value is a pointer to the decoded data.
  */
  static void *HexDecode(void *data, const char *cp, int n);
  
protected:
  /** Constructor */
  NDICommandInterpreter();

  /** Destructor */
  virtual ~NDICommandInterpreter();

  /** Get the communication object */
  CommunicationType *GetCommunication();

private:

  /** the communication object */
  CommunicationType::Pointer m_Communication;

  /** command reply -- this is the return value from Command() */

  char *m_CommandReply;                    /* reply without CRC and <CR> */
  char *m_SerialCommand;                   /* raw text to send to device */
  char *m_SerialReply;                     /* raw reply from device */

  /** this is set to 1 during tracking mode */

  bool m_Tracking;                           /* 'is tracking' flag */

  /** error handling information */

  int m_ErrorCode;                         /* error code (zero if no error) */

  /** GX command reply data */

  char m_GXTransforms[3][52];              /* 3 active tool transforms */
  char m_GXStatus[8];                      /* tool and system status */
  char m_GXInformation[3][12];             /* extra transform information */
  char m_GXSingleStray[3][24];             /* one stray marker per tool */
  char m_GXFrame[3][8];                    /* frame number for each tool */

  char m_GXPassiveTransforms[9][52];      /* 9 passive tool transforms */
  char m_GXPassiveStatus[24];             /* tool and system status */
  char m_GXPassiveInformation[9][12];     /* extra transform information */
  char m_GXPassiveFrame[9][8];            /* frame number for each tool */

  char m_GXPassiveStray[424];             /* all passive stray markers */

  /** PSTAT command reply data */

  char m_PSTATBasic[3][32];               /* basic pstat info */
  char m_PSTATTesting[3][8];              /* testing results */
  char m_PSTATPartNumber[3][20];          /* part number */
  char m_PSTATAccessories[3][2];          /* accessory information */
  char m_PSTATMarkerType[3][2];           /* marker information */

  char m_PSTATPassiveBasic[9][32];        /* basic passive pstat info */
  char m_PSTATPassiveTesting[9][8];       /* meaningless info */
  char m_PSTATPassivePartNumber[9][20];   /* virtual srom part number */
  char m_PSTATPassiveAccessories[9][2];   /* virtual srom accessories */
  char m_PSTATPassiveMarkerType[9][2];    /* meaningless for passive */

  /** SSTAT command reply data */

  char m_SSTATControl[2];                  /* control processor status */
  char m_SSTATSensor[2];                   /* sensor processors status */
  char m_SSTATTIU[2];                      /* tiu processor status */

  /** IRCHK command reply data */

  int m_IRCHKDetected;                     /* irchk detected infrared */
  char m_IRCHKSources[128];                /* coordinates of sources */

  /** PHRQ comand reply data */

  char m_PHRQReply[2];

  /** PHSR comand reply data */

  char m_PHSRReply[1284];

  /** PHINF command reply data */

  int m_PHINFUnoccupied;
  char m_PHINFBasic[34];
  char m_PHINFTesting[8];
  char m_PHINFPartNumber[20];
  char m_PHINFAccessories[2];
  char m_PHINFMarkerType[2];
  char m_PHINFPortLocation[14];
  char m_PHINFGPIOStatus[2];

  /** TX command reply data */

  int m_TXNumberOfHandles;
  unsigned char m_TXHandles[NDI_MAX_HANDLES];
  char m_TXTransforms[NDI_MAX_HANDLES][52];
  char m_TXStatus[NDI_MAX_HANDLES][8];
  char m_TXFrame[NDI_MAX_HANDLES][8];
  char m_TXInformation[NDI_MAX_HANDLES][12];
  char m_TXSingleStray[NDI_MAX_HANDLES][24];
  char m_TXSystemStatus[4];

  int m_TXNumberOfPassiveStrays;
  char m_TXPassiveStrayOutOfVolume[14];
  char m_TXPassiveStray[1052];

  /** set error indicator */
  int SetErrorCode(int errnum);

  /** helper functions that set the appropriate ivar as a result of
     information that is received from the AURORA or POLARIS */
  
  void HelperForCOMM(const char *cp, const char *crp);
  void HelperForPHINF(const char *cp, const char *crp);
  void HelperForPHSR(const char *cp, const char *crp);
  void HelperForTX(const char *cp, const char *crp);
  void HelperForGX(const char *cp, const char *crp);
  void HelperForINIT(const char *cp, const char *crp);
  void HelperForIRCHK(const char *cp, const char *crp);
  void HelperForPSTAT(const char *cp, const char *crp);
  void HelperForSSTAT(const char *cp, const char *crp);
  void HelperForPHRQ(const char *cp, const char *crp);

  /*!
  Convert \em n characters of a hexadecimal string into an unsigned long.
  The conversion halts if a non-hexadecimal digit is found.

  The primary use of this function is decoding replies from the
  device.
  */
  static unsigned long HexadecimalStringToUnsignedLong(const char *cp,
                                                       int n);

  /*!
  Convert \em n characters of a hexadecimal string into an integer..
  The conversion halts if a non-hexadecimal digit is found.

  The primary use of this function is decoding replies from the
  device.
  */
  static int HexadecimalStringToInt(const char *cp, int n);

  /*!
  Convert \em n characters of a signed decimal string to a long.
  The first character in the string must be '+' or '-', otherwise
  the result will be zero.

  The primary use of this function is decoding replies from the
  device.
  */
  static int SignedStringToInt(const char *cp, int n);

  NDICommandInterpreter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

}
#endif
