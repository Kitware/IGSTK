#ifndef INCLUDE_NDOPTO 
#define INCLUDE_NDOPTO 
/***************************************************************

     Name:            NDOPTO.H

     Description:
     Main OPTOTRAK API include file.

  Copyright (C) 1983-2005, Northern Digital Inc. All rights reserved.

****************************************************************/

/***************************************************************
Name:            APP_PARM.H

Description:
    Header file for APP_PARM C file.
    This module contains routines for reading and saving
    application parameters.

Modified:

****************************************************************/

#ifndef INCLUDE_APP_PARM_H
#define INCLUDE_APP_PARM_H

/****************************************************************
  Defines
****************************************************************/
#define APP_PARM_STRING     0x0001
#define APP_PARM_INT        0x0002
#define APP_PARM_UNSIGNED   0x0004
#define APP_PARM_LONG       0x0008
#define APP_PARM_ULONG      0x0010
#define APP_PARM_FLOAT      0x0020
#define APP_PARM_DOUBLE     0x0040
#define APP_PARM_BOOLEAN    0x0080
#define APP_PARM_OPTIONAL   0x1000
#define APP_PARM_COUNT      0x3000  /* Always an optional parameter */

/*
 * These are used internally by ReadAppParms().  Do not use.
 */
#define APP_PARM_TYPES      0x00ff
#define APP_PARM_NUMERIC    0x007e
#define APP_PARM_UNREAD     0x8000

/****************************************************************
  Macros
****************************************************************/

/****************************************************************
 Global Structures
****************************************************************/
typedef struct
{
    char        *pszParam;
    void        *pBuffer;
    unsigned    uBuffLen;
    unsigned    uFlags;
    float       fMinVal;
    float       fMaxVal;
} AppParmInfo;

/****************************************************************
 External Variables
****************************************************************/

/****************************************************************
 Routine Definitions
****************************************************************/
 
CPLUSPLUS_START
boolean ReadAppParms( char *pszParmFile, char *pszSection,
                      AppParmInfo *pParmInfo );
boolean SaveAppParms( char *pszParmFile, char *pszSection,
                      AppParmInfo *pParmInfo );
/*
 * SCO-98-0071
 *  added DeleteAppParms
 */
boolean DeleteAppParms( char *pszParmFile, char *pszSection );
CPLUSPLUS_END

#endif
/*****************************************************************
Name:      general_int.h

Description:
  General section describing Optotrak interface.

Creator:    Christian C. Monterroso    05/2005

*****************************************************************/

/*
 * Constants defining limits on certain objects in the OPTOTRAK system.
 */
#define MAX_SENSORS                      10
#define MAX_OPTOSCOPES                    4
#define MAX_RIGID_BODIES                 10
#define MAX_HOST_RIGID_BODIES           170    /* ( OPTOTRAK_MAX_MARKERS / 3 ) */
#define MAX_SENSOR_NAME_LENGTH           20
#define MAX_TEMPSENSORS_PER_SENSOR        8
#define OPTOTRAK_MAX_MARKERS            256
#define OPTOTRAK_INTERNAL_DATA_SOURCES    8

#define _MAX_FILENAME              1024

#define OPTO_HW_TYPE_SENSOR      0x4D41435FL    /* '_CAM' */
#define OPTO_HW_TYPE_SU        0x55535953L    /* 'SYSU' */
#define OPTO_HW_TYPE_ODAU      0x5541444FL    /* 'ODAU' */
#define OPTO_HW_TYPE_REALTIME    0x5D544C52L    /* 'RLTM' */
#define OPTO_HW_TYPE_CERTUS_SENSOR  0x43323258L    /* 'X22C' */
#define OPTO_HW_TYPE_CERTUS_SU    0x53323258L    /* 'X22S' */


/*****************************************************************
Static Typedefs and Structures
*****************************************************************/
struct OptoNodeInfoStruct
{
  unsigned long  ulHWType,
          ulHWRev,
          ulMemSize;
  char      szFreezeId[ 80 ],
          szSwDesc[ 80 ],
          szSerialNo[ 32 ];
};

/*
 * These are flags and constants used by OPTOTRAK and OPTOSCOPE for any of
 * the buffer messages
 */
#define OPTO_BUFFER_OVERRUN_FLAG        0x0001
#define OPTO_FRAME_OVERRUN_FLAG         0x0002
#define OPTO_NO_PRE_FRAMES_FLAG         0x0004
#define OPTO_SWITCH_DATA_CHANGED_FLAG   0x0008
#define OPTO_TOOL_CONFIG_CHANGED_FLAG   0x0010
#define OPTO_FRAME_DATA_MISSED_FLAG    0x0020

#define OPTO_SWITCH_CONFIG_CHANGED_MASK (OPTO_SWITCH_DATA_CHANGED_FLAG | OPTO_TOOL_CONFIG_CHANGED_FLAG)

/***************************************************************
Name: DATAPROP   -Details general interface to the data proprietor
****************************************************************/

/*
 * Messages specific to the OPTOTRAK data proprietor
 */

/* OPTO_LATEST_RAW_FRAME_MESSAGE
 * DATAPROP_SEND_DATA_MESSAGE
 * OPTO_LATEST_WAVE_FRAME_MESSAGE
 */
#define WAVEFORM_MODE_WAVEFORM_WIDTH  31

struct optotrak_wave_head_struct
{
  float       centroid;        /* Calculated centroid */
  char        peak_value;      /* Peak value at peak pixel */
  char        gain;            /* Gain setting for this collection */
  char        error;           /* Error in centtroid calculation */
  char        dummy;
/*  char        peak_offset[2];*/  /* Pixel Number of peak */
  unsigned    start_pixel;  /* Pixel number of first pixel in wave */
};

/* OPTO_TRANSFORM_DATA_MESSAGE
 */

#define OPTO_TRANSFORM_DATA_SIZE           8
struct OptoTransformDataStruct
{
  long int markers;       /* Total # of markers to convert */
  long int FullDataInfo;  /* Data contains peak info as well */
};


/*
 * Data ids you can receive back from the OPTOTRAK
 */

/* OPTO_TRANS_BUFFER_MESSAGE
 */
/***************************************************************
Name: ODAU       -Section detailing interfacing with the ODAU unit
****************************************************************/
#define OPTOSCOPE_MAX_CHANNELS_IN       16
#define OPTOSCOPE_MIN_CHANNEL_NUM        1
#define OPTOSCOPE_MAX_TABLE_ENTRIES     OPTOSCOPE_MAX_CHANNELS_IN + 1

/*
 * SCO-95-0015
 * Added    ODAU_DIGITAL_OFFB_MUXA
 *          ODAU_DIGITAL_INPB_MUXA
 *          ODAU_DIGITAL_OUTPB_MUXA
 * Changed constants to act as 2 4bit flags fields:
 *      Hex     Binary      Meaning
 *      x00     0000        Port off
 *      x01     0001        Port set to Input
 *      x02     0010        Port set to output
 *      x04     0100        Port set for MUXER
 *          
 */
#define ODAU_DIGITAL_PORT_OFF     0x00
#define ODAU_DIGITAL_INPB_INPA    0x11
#define ODAU_DIGITAL_OUTPB_INPA   0x21
#define ODAU_DIGITAL_OUTPB_OUTPA  0x22
#define ODAU_DIGITAL_OFFB_MUXA    0x04
#define ODAU_DIGITAL_INPB_MUXA    0x14
#define ODAU_DIGITAL_OUTPB_MUXA   0x24

/*
 * SCO-97-0020
 * New constants for user timer mode
 */
#define ODAU_TIMER_ONE_SHOT     1
#define ODAU_TIMER_RATE_GEN     2
#define ODAU_TIMER_SQUARE_WAVE  3
#define ODAU_TIMER_RETRIGGER    4

/*
 * SCO-97-0020
 *  New constants for selecting which timer to set
 */
#define ODAU_TIMER2             2

/*
 * Constants used to test ODAU digital port bits
 */
#define ODAU_DGTL_PT_OFF          0x00
#define ODAU_DGTL_PT_IN           0x01
#define ODAU_DGTL_PT_OUT          0x02
#define ODAU_DGTL_PT_MUX          0x04
#define ODAU_DGTL_INPUT           0x11

/* OPTOSCOPE_SETUP_COLL_MESSAGE
 * OPTOSCOPE_SETUP_COLL_COMMAND
 * SCO-95-0015
 * Added ulFlags field
 *      Flag ODAU_DIFFERENTIAL_FLAG defined
 * Added fScanFrequency field.
 */

struct OptoscopeSetupCollStruct
{
    long int        DataId;
    float           fFrameFrequency;
    float           fScanFrequency;
    long int        AnalogChannels;
    long int        AnalogGain;
    long int        DigitalMode;
    float           CollectionTime;
    float           PreTriggerTime;
    long int        StreamData;
    unsigned long   ulFlags;
};

#define ODAU_DIFFERENTIAL_FLAG  0x0001

/*
 * The following is a global structure is passed from the OPTOSCOPE the the
 * OPTOTRAK so that it may record the required collection information.
 */
struct ScopeCollectionParmsStruct
{
    long int    DataId;
    float       fFrequency;
    long int    PacketSize;
};

/* OPTOSCOPE_START_BUFF_MESSAGE
 * OPTOSCOPE_START_BUFF_COMMAND
 *
 * OPTOSCOPE_STOP_BUFF_MESSAGE
 * OPTOSCOPE_STOP_BUFF_COMMAND
 *
 * OPTOSCOPE_SHUTDOWN_MESSAGE
 * OPTOSCOPE_SHUTDOWN_COMMAND
 */

struct OptoscopeSetChnlAllStruct
{
    long int    status;
    long int    gain;
};

struct OptoscopeSetChnlSglStruct
{
    long int    status;
    long int    gain;
    long int    channel;
};

struct OptoscopeSetGainAllStruct
{
    long int    gain;
};

struct OptoscopeSetGainSglStruct
{
    long int    gain;
    long int    channel;
};

/* OPTOSCOPE_STATUS_MESSAGE
 * SCO-95-0015
 * Added ulFlags field
 * Added fScanFrequency field.
 */

struct OptoscopeStatusStruct
{
    long int        DataId;
    float           fFrameFrequency;
    float           fScanFrequency;
    long int        AnalogGain;
    long int        AnalogChannels;
    long int        DigitalMode;
    float           CollectionTime;
    float           PreTriggerTime;
    long int        StreamData;
    long int        ErrorFlags;
    unsigned long   ulFlags;
};

/*
 * SCO-95-0015
 * OPTOSCOPE_ANALOG_OUT_MESSAGE
 */
struct OptoscopeAnalogOutStruct
{
    float           fCh1Volts,
                    fCh2Volts;
    unsigned long   ulUpdateMask;
};

/*
 * SCO-95-0015
 * OPTOSCOPE_DIGITAL_OUT_MESSAGE
 */
struct OptoscopeDigitalOutStruct
{
    unsigned long   ulDigitalOut,
                    ulUpdateMask;
};

/*
 * SCO-97-0020
 * OPTOSCOPE_SET_TIMER_MESSAGE
 */
struct OptoscopeTimerDataStruct
{
    unsigned long   ulTimer,
                    ulMode,
                    ulVal;
};

/*
 * SCO-97-0020
 *  OPTOSCOPE_LOAD_FIFO_MESSAGE
 */
struct OptoscopeFiFoDataStruct
{
    unsigned long   ulEntries,
                    grulEntries[256];
};

/*
 * SCO-97-0020
 *   New data structure for OdauLoadControlFiFo()
 */
struct OdauControlWordStruct
{
    boolean          bDigitalInput,
                    bDifferential;
    unsigned        uGain,
                    uMuxer,
                    uChannel;
};
/***************************************************************
Name: REALTIME   -Details advanced data properitor calls for realtime option
****************************************************************/
/*
 * These are flags which are used with rigid bodies and the determination
 * of their transformations.
 *
 * SCO-99-0010: Added the flag OPTOTRAK_RIGID_ERR_MKR_SPREAD.
 */
#define OPTOTRAK_UNDETERMINED_FLAG      0x0001
#define OPTOTRAK_STATIC_XFRM_FLAG       0x0002
#define OPTOTRAK_STATIC_RIGID_FLAG      0x0004
#define OPTOTRAK_CONSTANT_RIGID_FLAG    0x0008
#define OPTOTRAK_NO_RIGID_CALCS_FLAG    0x0010
#define OPTOTRAK_DO_RIGID_CALCS_FLAG    0x0020
#define OPTOTRAK_QUATERN_RIGID_FLAG     0x0040
#define OPTOTRAK_ITERATIVE_RIGID_FLAG   0x0080
#define OPTOTRAK_SET_QUATERN_ERROR_FLAG 0x0100
#define OPTOTRAK_SET_MIN_MARKERS_FLAG   0x0200
#define OPTOTRAK_RIGID_ERR_MKR_SPREAD   0x0400
#define OPTOTRAK_RETURN_QUATERN_FLAG    0x1000
#define OPTOTRAK_RETURN_MATRIX_FLAG     0x2000
#define OPTOTRAK_RETURN_EULER_FLAG      0x4000
/*
 * Messages specific to the OPTOTRAK data proprietor
 */

/* OPTO_LATEST_RIGID_FRAME_MESSAGE
 *
 * OPTOTRAK_ADD_RIGID_MESSAGE
 */

struct OptotrakRigidDescrStruct
{
    long int lnRigidId;            /* Unique identifier for rigid body. */
    long int lnStartMarker;          /* Start marker for rigid body. */
    long int lnNumberOfMarkers;        /* Number of markers in rigid body. */
    long int lnMinimumMarkers;        /* Minimum number of markers for calcs. */
    float    fMax3dError;          /* Max allowable quaternion error. */
    long int lnFlags;            /* Flags for this rigid body. */
  char   szName[_MAX_FILENAME];      /* Name of the rigid body */
};

struct OptotrakAddRigidStruct
{
    long int lnRigidId;            /* Unique identifier for rigid body. */
    long int lnStartMarker;          /* Start marker for rigid body. */
    long int lnNumberOfMarkers;        /* Number of markers in rigid body. */
    long int lnMinimumMarkers;        /* Minimum number of markers for calcs. */
    float    fMax3dError;          /* Max allowable quaternion error. */
    long int lnFlags;            /* Flags for this rigid body. */
};

/* OPTOTRAK_DEL_RIGID_MESSAGE
 */

struct OptotrakDelRigidStruct
{
    long int lnRigidId;     /* Unique identifier for rigid body. */
};

/* OPTOTRAK_SET_RIGID_MESSAGE
 */

struct OptotrakSetRigidStruct
{
    long int lnRigidId;            /* Unique identifier for rigid body. */
    long int lnMarkerAngle;
    long int lnMinimumMarkers;     /* Minimum number of markers for calcs. */
    float    fMax3dError;
    float    fMaxSensorError;
    float    fMax3dRmsError;
    float    fMaxSensorRmsError;
    long int lnFlags;              /* New status flags for this rigid body. */
};

/* OPTOTRAK_ROTATE_RIGIDS_MESSAGE
 */

struct OptotrakRotateRigidsStruct
{
    long int        lnRotationMethod; /* Flags to control xfrm rotations. */
    long int        lnRigidId;        /* Rigid body to base xfrm rotations on. */
    transformation  dtEulerXfrm;      /* XFRM to base xfrm rotations on. */
};

/* OPTOTRAK_STOP_ROTATING_MESSAGE
 *
 * OPTOTRAK_ADD_NORMALS_MESSAGE
 */

struct OptotrakAddNormalsStruct
{
    long int lnRigidId;         /* Unique identifier for rigid body. */
};

/* OPTOTRAK_GET_RIG_STATUS_MESSAGE
 */

/*
 * Data ids you can receive back from the OPTOTRAK data proprietor
 */

/* OPTOTRAK_RIGID_STATUS_MESSAGE
 */

struct OptotrakRigidStatusStruct
{
    long int lnRigidBodies;     /* Number of rigid bodies in use */
};

/* OPTO_LATEST_RIGID_MESSAGE
 */

#define OPTO_RIGID_HEADER_SIZE            16
struct OptotrakRigidHeaderStruct
{
    long int    NumberOfRigids;   /* number of transforms following header */
    long int    StartFrameNumber; /* frame number of 3D data used */
    long int    flags;            /* current flag settings for OPTOTRAK */

  /* EC-02-0548 DMS added following field */
  long int NumberOfSwitchDataBytes; /* Number of switch data bytes */
};

union TransformationUnion
{
    RotationTransformation          rotation;
    transformation                  euler;
    QuatTransformation              quaternion;
};

struct OptotrakRigidStruct
{
    long int                    RigidId;         /* rigid body id xfrm is for */
    long int                    flags;           /* flags for this rigid body */
    float                       QuaternionError; /* quat rms error for xfrm */
    float                       IterativeError;  /* iter rms error for xfrm */
    union TransformationUnion   transformation;  /* latest calculated xfrm */
};
/*****************************************************************
Name:      admin.h

Description:
  Defines for commands to pass to the Optotrak Administrator.

Creator:    Christian C. Monterroso    05/2005

*****************************************************************/

/*****************************************************************
Defines
*****************************************************************/
/*
 * Flag definitions used in the OPTOTRAK_SETUP_COLL_MESSAGE.
 * Note: don't use the flag 0x1000 as it is taken by the constant
 *       OPTOTRAK_REALTIME_PRESENT_FLAG.
 */
#define OPTOTRAK_NO_INTERPOLATION_FLAG    0x00001
#define OPTOTRAK_FULL_DATA_FLAG           0x00002
#define OPTOTRAK_PIXEL_DATA_FLAG          0x00004
#define OPTOTRAK_MARKER_BY_MARKER_FLAG    0x00008
#define OPTOTRAK_ECHO_CALIBRATE_FLAG      0x00010
#define OPTOTRAK_BUFFER_RAW_FLAG          0x00020
#define OPTOTRAK_NO_FIRE_MARKERS_FLAG     0x00040
#define OPTOTRAK_STATIC_THRESHOLD_FLAG    0x00080
#define OPTOTRAK_WAVEFORM_DATA_FLAG       0x00100
#define OPTOTRAK_AUTO_DUTY_CYCLE_FLAG     0x00200
#define OPTOTRAK_EXTERNAL_CLOCK_FLAG      0x00400
#define OPTOTRAK_EXTERNAL_TRIGGER_FLAG    0x00800
#define OPTOTRAK_GET_NEXT_FRAME_FLAG      0x02000
#define OPTOTRAK_SWITCH_AND_CONFIG_FLAG   0x04000
#define OPTOTRAK_USE_COLPARMS_ONLY_FLAG    0x08000
#define OPTOTRAK_BACKGROUND_SUBTRACT_FLAG  0x10000

/*
 * The following flags are set by the OPTOTRAK system itself.
 * They indicate (1) if the system has revision D/E Sensors,
 *               (2) if the system can perform real-time rigid bodies.
 *               (3) if the markers are on in the system.
 */
#define OPTOTRAK_REVISIOND_FLAG         0x80000000
#define OPTOTRAK_3020_FLAG        0x80000000
#define OPTOTRAK_REVISION_X22_FLAG      0x40000000
#define OPTOTRAK_CERTUS_FLAG      0x40000000
#define OPTOTRAK_RIGID_CAPABLE_FLAG     0x08000000
#define OPTOTRAK_MARKERS_ACTIVE         0x04000000

/*****************************************************************
Static Typedefs and Structures
*****************************************************************/
struct OdauSetupStruct
{
  float       fOdauFrequency;
  long int    OdauChannels;
  long int    OdauGain;
  long int    OdauFlags;
};

struct OptotrakSetupCollStruct
{
  long int    DataId;
  long int    NumMarkers;
  float       fFrameFrequency;
  float       CollectionTime;
  float       PreTriggerTime;
  long int    StreamData;
  long int    flags;
  float       fOdauFrequency;
  long int    OdauChannels;
  long int    OdauGain;
  long int    OdauFlags;
};

struct OptotrakStroberTableStruct
{
  long int    Port1;
  long int    Port2;
  long int    Port3;
  long int    Port4;
};

/*****************************************************************

Name:               CENTPROD.H


*****************************************************************/
#define CENTPROD_INCLUDE

#define MAX_BUFFER_SIZE                 512
#define OPTIMAL_PEAK                    200

#ifndef CENTROID_OK
#define CENTROID_OK                     0
#define CENTROID_WAVEFORM_TOO_WIDE      1
#define CENTROID_PEAK_TOO_SMALL         2
#define CENTROID_PEAK_TOO_LARGE         3
#define CENTROID_WAVEFORM_OFF_DEVICE    4
#define CENTROID_FELL_BEHIND            5
#define CENTROID_LAST_CENTROID_BAD      6
#define CENTROID_BUFFER_OVERFLOW        7
#define CENTROID_MISSED_CCD             8
#define  CENTROID_BAD_CRC        9 /* [EC-02-0548] EJG added for JR */
#endif

/*****************************************************************
External Variables and Routines
*****************************************************************/
struct SensorStatusStruct
{
  char
    chPeak,
    chDRC,
    chError,
    chNibble;
};

struct  CentroidBufferStruct
{
    float       centroid;             /* Calculated centroid */
    char        Peak;                 /* Peak value 0 to 255 */
    char        gain;                 /* Gain setting for this collection */
    char        ErrorCode;            /* Error in centtroid calculation */
    char        PeakNibble;           /* Low Nibble of Peak value (Revd only) */
};
/*****************************************************************
Name:               DHANDLES.H

Description:
    This header file contains the defines and structures
  required for device handles.

Modifications:
    EC-02-0548  CCM  11/02

*****************************************************************/


/*****************************************************************
Defines
*****************************************************************/
#define DEVICE_MAX_PROPERTY_STRING    256
#define DEVICE_MAX_STROBER_PORTS_CERTUS  3
#define DEVICE_MAX_SWITCHES             4
#define DEVICE_MAX_VLEDS                4
#define DEVICE_MAX_BEEPERS              1
#define DEVICE_MAX_MARKERS              255

enum DeviceHandleStatus
{
  DH_STATUS_UNOCCUPIED = 0,
  DH_STATUS_OCCUPIED = 1,
  DH_STATUS_INITIALIZED = 2,
  DH_STATUS_ENABLED = 3,
  DH_STATUS_FREE = 4
}; /* enum DeviceHandleStatus */

enum DeviceHandlePropertyType
{
  DH_PROPERTY_TYPE_INT = 0,
  DH_PROPERTY_TYPE_FLOAT = 1,
  DH_PROPERTY_TYPE_DOUBLE = 2,
  DH_PROPERTY_TYPE_STRING = 3,
  DH_PROPERTY_TYPE_CHAR = 4
}; /* enum DeviceHandlePropertyType */

enum DeviceHandleProperties
{
  DH_PROPERTY_UNKNOWN = 0,
  DH_PROPERTY_NAME = 1,
  DH_PROPERTY_MARKERSTOFIRE = 2,
  DH_PROPERTY_MAXMARKERS = 3,
  DH_PROPERTY_STARTMARKERPERIOD = 4,
  DH_PROPERTY_SWITCHES = 5,
  DH_PROPERTY_VLEDS = 6,
  DH_PROPERTY_PORT = 7,
  DH_PROPERTY_ORDER = 8,
  DH_PROPERTY_SUBPORT = 9,
  DH_PROPERTY_FIRINGSEQUENCE = 10,
  DH_PROPERTY_HAS_ROM = 11,
  DH_PROPERTY_TOOLPORTS = 12,
  DH_PROPERTY_3020_CAPABILITY = 13,
  DH_PROPERTY_3020_MARKERSTOFIRE = 14,
  DH_PROPERTY_3020_STARTMARKERPERIOD = 15,
  DH_PROPERTY_STATUS = 16
}; /* enum DeviceHandleProperties */

enum VLEDState 
{
    VLEDST_OFF = 0,
    VLEDST_ON = 1,
    VLEDST_BLINK = 2
}; /* enum VLEDState */


/*****************************************************************
Structures
*****************************************************************/

/*****************************************************************

Name:    DeviceHandle

Description:
  Basic device handle information.  Contains a device handle ID
  and the current status of the device.

  nID      integer specifying the device handle ID.
  dtStatus  current status of the device associated with this device handle.
        possible device status:
          DH_STATUS_UNOCCUPIED  device has been unplugged and should be freed
          DH_STATUS_OCCUPIED    device has been plugged and should be initialized
          DH_STATUS_INITIALIZED  device has been initialized and should be enabled
          DH_STATUS_ENABLED    device has been enabled and is ready for use
          DH_STATUS_FREE      no device associated with this device handle

*****************************************************************/
typedef struct
{
  int
    nID;
  enum DeviceHandleStatus
    dtStatus;
} DeviceHandle;


/*****************************************************************

Name:    DeviceHandleProperty

Description:
  Information about a single property for a device.

  uProppertyID  unsigned integer identifying the property
          stored in the structure.
          possible properties:
            DH_PROPERTY_NAME        device name
            DH_PROPERTY_MARKERSTOFIRE    number of markers to fire
            DH_PROPERTY_MAXMARKERS      maximum number of markers on the device.
                            these markers can be fired in any order.
            DH_PROPERTY_STARTMARKERPERIOD  index of the first marker period within a frame
                            where the first marker for this device will be fired.
            DH_PROPERTY_SWITCHES      number of switches available on the device
            DH_PROPERTY_VLEDS        number of visible LEDs available on the device
            DH_PROPERTY_PORT        strober port into which the device is
                            physically located.
            DH_PROPERTY_ORDER        ordering within the strober port
            DH_PROPERTY_SUBPORT        if the device is plugged into a subport of
                            another device, the index of the subport into
                            which the device is plugged in.
            DH_PROPERTY_FIRINGSEQUENCE    marker firing sequence
            DH_PROPERTY_HAS_ROM        indicates if the device has SROM information
                            that describes a rigid body.
            DH_PROPERTY_TOOLPORTS      number of tool ports on the device.
            DH_PROPERTY_3020_CAPABILITY    indicates if the device has 3020 strober
                            capability.
            DH_PROPERTY_3020_MARKERSTOFIRE  if the device has 3020 strober capability,
                            number of 3020 markers to fire.
            DH_PROPERTY_3020_STARTMARKERPERIOD  index of the marker period within a frame
                              where the first 3020 marker for this device
                              will be fired.
            DH_PROPERTY_STATUS        status of the device (OCCUPIED, INITIALIZED,
                            ENABLED, UNOCCUPIED, FREE)
  dtPropertyType  specifies the type of data in which the 
          property information is stored.  check this
          value to determine which member of the dtData
          union to use to retrieve the property information.
          possible property types:
            DH_PROPERTY_TYPE_INT    integer property
            DH_PROPERTY_TYPE_FLOAT    float property
            DH_PROPERTY_TYPE_DOUBLE    double property
            DH_PROPERTY_TYPE_STRING    string property
            DH_PROPERTY_TYPE_CHAR    character property
  dtData      the property information for the specified property.
          the data is contained in a union.  check the
          dtPropertyType value to determine which member of
          the union should be read.
            nData    contains information for DH_PROPERTY_TYPE_INT property type.
            fData    contains information for DH_PROPERTY_TYPE_FLOAT
            dData    contains information for DH_PROPERTY_TYPE_DOUBLE
            szData    contains information for DH_PROPERTY_TYPE_STRING
            cData    contains information for DH_PROPERTY_TYPE_CHAR

*****************************************************************/
typedef struct
{
  unsigned int
    uPropertyID;
  enum DeviceHandlePropertyType
    dtPropertyType;
  union
  {
    int
      nData;
    float
      fData;
    double
      dData;
    char
      cData;
    char
      szData[DEVICE_MAX_PROPERTY_STRING];
  } dtData;
} DeviceHandleProperty;


/*****************************************************************

Name:    DeviceHandleInfo

Description:
  Complete device handle information.  Contains the current information
  associated with a device handle.

  pdtHandle    pointer to a DeviceHandle structure, which identifies the
          device handle ID and the status of the device.
  grProperties  array of device handle properties.  each element of the
          array contains information about a single device
          property in a DeviceHandleProperty structure
  nProperties    size of the device handle properties array.
  bRigidBody    indicates if the device can be tracked as a rigid body.

*****************************************************************/
typedef struct
{
  DeviceHandle
    *pdtHandle;
  DeviceHandleProperty
    *grProperties;
  int
    nProperties;
  boolean
    bRigidBody;
} DeviceHandleInfo;


/*****************************************************************
Name:      regalg.h

Description:
  Definitions of classes and functions used to register
  and align the Optotrak system.

Creator:    Christian C. Monterroso  03/2003

Changes:
EC-02-0458
- Created regalg.h

*****************************************************************/
#ifndef __NDOPTO_REGALG_H__
#define __NDOPTO_REGALG_H__

/****************************************************************
  Defines
****************************************************************/
#define REG_NO_LOG_FILE        0
#define REG_SUMMARY_LOG_FILE    1  
#define REG_DETAILED_LOG_FILE    2

#define ALIGN_NO_LOG_FILE      0
#define ALIGN_DETAILED_LOG_FILE    2

#define CALIBRIG_NO_LOG_FILE      0
#define CALIBRIG_DETAILED_LOG_FILE    2

#define ALIGN_SUCCESS            0
#define ALIGN_ERROR_PERFORMING_ALIGNMENT  1
#define ALIGN_ERROR_LOADING_INPUT_FILES    2
#define ALIGN_ERROR_ALLOCATING_MEMORY    3
#define ALIGN_ERROR_PROCESSING_DATA      4
#define ALIGN_ERROR_PARAMETERS        5

#define REG_SUCCESS              0
#define REG_ERROR_PERFORMING_REGISTRATION  1
#define REG_ERROR_LOADING_INPUT_FILES    2
#define REG_ERROR_ALLOCATING_MEMORY      3
#define REG_ERROR_PROCESSING_DATA      4
#define REG_ERROR_WORKING_FILES        5

#define CALIBRIG_SUCCESS            0
#define CALIBRIG_ERROR_PERFORMING_CALIBRATION  1
#define CALIBRIG_ERROR_LOADING_INPUT_FILES    2
#define CALIBRIG_ERROR_PARAMETERS        3

#define CAMFILE_STANDARD    "standard.cam"
#define CAMFILE_ALIGN      "align_.cam"
#define CAMFILE_REGISTER    "register_.cam"
#define CAMFILE_CALIBRIG    "calibrig_.cam"


/****************************************************************
 Global Structures
****************************************************************/
typedef struct AlignParametersStruct
{
  char 
    szDataFile[_MAX_FILENAME],
    szRigidBodyFile[_MAX_FILENAME],
    szInputCamFile[_MAX_FILENAME],
    szOutputCamFile[_MAX_FILENAME],
    szLogFileName[_MAX_FILENAME];
  int
    nLogFileLevel;
  boolean
    bInputIsRawData,
    bVerbose;
} AlignParms;

typedef struct RegisterParametersStruct
{
  char 
    szRawDataFile[_MAX_FILENAME],
    szRigidBodyFile[_MAX_FILENAME],
    szInputCamFile[_MAX_FILENAME],
    szOutputCamFile[_MAX_FILENAME],
    szLogFileName[_MAX_FILENAME];

  float    
    fXfrmMaxError,
    fXfrm3dRmsError,
    fSpread1,
    fSpread2,
    fSpread3;
  int    
    nMinNumberOfXfrms,
    nLogFileLevel;
  boolean  
    bCheckCalibration,
    bVerbose;
} RegisterParms;

typedef struct CalibrigParametersStruct
{
  char 
    szRawDataFile[_MAX_FILENAME],
    szRigidBodyFile[_MAX_FILENAME],
    szInputCamFile[_MAX_FILENAME],
    szOutputCamFile[_MAX_FILENAME],
    szLogFileName[_MAX_FILENAME];
  boolean  
    bVerbose;
  int
    nLogFileLevel;
} CalibrigParms;


#endif /* __NDOPTO_REGALG_H__ */
/*****************************************************************
Name:      optoappl.h

Description:
  Definitions and prototypes used by Optotrak applications.

Creator:    

*****************************************************************/

/**************************************************************************
  Defines
**************************************************************************/

/*
 * The error codes that the optotrak may return
 */
#define OPTO_NO_ERROR_CODE          0
#define OPTO_SYSTEM_ERROR_CODE      1000
#define OPTO_USER_ERROR_CODE        2000

/*
 * Flags for controlling the setup of the message passing layer on the
 * PC side.
 */
#define OPTO_LOG_ERRORS_FLAG          0x0001
#define OPTO_SECONDARY_HOST_FLAG      0x0002
#define OPTO_ASCII_RESPONSE_FLAG      0x0004
#define OPTO_LOG_MESSAGES_FLAG        0x0008
#define OPTO_LOG_DEBUG_FLAG           0x0010

/*
 * Minimum and maximum values for various system settings
 */
#define FLOAT_COMP_EPS            ( (float)1E-10 )
#define OPTO_MIN_STROBERPORT_MARKERS    0
#define OPTO_MAX_STROBERPORT_MARKERS    256
#define OPTO_MAX_STROBERPORT_MARKERS_CERTUS  512
#define OPTO_MIN_MARKERS          1
#define OPTO_MAX_MARKERS          256
#define OPTO_MAX_MARKERS_CERTUS        512
#define OPTO_MIN_MARKERFREQ          ( 1.0f - FLOAT_COMP_EPS )
#define OPTO_MAX_MARKERFREQ          ( 3500.0f + FLOAT_COMP_EPS )
#define OPTO_MAX_MARKERFREQ_CERTUS      ( 4600.0f + FLOAT_COMP_EPS )
#define OPTO_MIN_FRAMEFREQ          OPTO_MIN_MARKERFREQ
#define OPTO_MAX_FRAMEFREQ          ( OPTO_MAX_MARKERFREQ / ( OPTO_MIN_MARKERS + 1 ) )
#define OPTO_MAX_FRAMEFREQ_CERTUS      ( OPTO_MAX_MARKERFREQ_CERTUS / ( OPTO_MIN_MARKERS + 2 ) )
#define OPTO_MIN_THRESHOLD          0
#define OPTO_MAX_THRESHOLD          255
#define OPTO_MIN_MINGAIN          0
#define OPTO_MAX_MINGAIN          255
#define OPTO_MIN_DUTYCYCLE          ( 0.10f - FLOAT_COMP_EPS )
#define OPTO_MAX_DUTYCYCLE          ( 0.85f + FLOAT_COMP_EPS )
#define OPTO_MIN_VOLTAGE          ( 5.5f - FLOAT_COMP_EPS )
#define OPTO_MAX_VOLTAGE          ( 12.0f + FLOAT_COMP_EPS )
#define OPTO_MIN_COLLTIME          0.1f
#define OPTO_MAX_COLLTIME          99999

/*
 * Constants for raw files which can be converted.
 */
#define OPTOTRAK_RAW    1
#define ANALOG_RAW      2

/*
 * Constants for modes in which files can be opened.
 */
#define OPEN_READ       1
#define OPEN_WRITE      2

/*
 * Maximum constants for the processes we will have to keep track
 * of in the optoappl software.  Make room for one address per node,
 * and an extra for our data proprietor.
 */
#define MAX_OPTOTRAKS         1
#define MAX_DATA_PROPRIETORS  1
#define MAX_ODAUS             4
#define MAX_PROCESS_ADDRESSES (MAX_NODES + 1)

/*
 * Constants for the node HW Revisions
 */
#define OPTO_NODE_HWREV_SCU_3020      0
#define OPTO_NODE_HWREV_SCU_CERTUS      0
#define OPTO_NODE_HWREV_PS_3020        3
#define OPTO_NODE_HWREV_PS_CERTUS      4

/*
 * Constants for keeping track of whick process the application wants to
 * communicate with.
 */
#define OPTOTRAK                            0
#define DATA_PROPRIETOR                     1
#define ODAU1                               2
#define ODAU2                               3
#define ODAU3                               4
#define ODAU4                               5
#define SENSOR_PROP1                        6
#define OPTOTRAK_DATA_BUFFER_OVERWRITE_FLAG 0x8000

/*
 * Pointer definitions for spooling to memory.
 */
#if IBMPC
typedef void huge *         SpoolPtrType;
#else
typedef void *              SpoolPtrType;
#endif

/*
 * Flag assignments for controlling blocking in
 * real-time data retrieval routines and for on-host 3D and 6D
 * conversions
 */
#define OPTO_LIB_POLL_REAL_DATA    0x0001
#define OPTO_CONVERT_ON_HOST       0x0002
#define OPTO_RIGID_ON_HOST         0x0004
#define OPTO_USE_INTERNAL_NIF      0x0008

/*
 * String length
 */
#define MAX_ERROR_STRING_LENGTH 2047


/**************************************************************************
 Routine Definitions
**************************************************************************/

CPLUSPLUS_START

/*
 * Function to retrieve OAPI version
 */
NDI_DECL1 int
  NDI_DECL2 OAPIGetVersionString( char* szVersionString, int nBufferSize );

/*
 * System initialization routines
 */
NDI_DECL1 int
    NDI_DECL2 TransputerLoadSystem( char *pszNifFile ),
    NDI_DECL2 TransputerInitializeSystem( unsigned int uFlags ),
    NDI_DECL2 TransputerShutdownSystem( void ),
    NDI_DECL2 TransputerDetermineSystemCfg( char *pszInputLogFile );

NDI_DECL1 int
    NDI_DECL2 OptotrakLoadCameraParameters( char *pszCamFile ),
    NDI_DECL2 OptotrakSetupCollectionFromFile( char *pszCollectFile ),
    NDI_DECL2 OptotrakSetupCollection( int   nMarkers,
                                       float fFrameFrequency,
                                       float fMarkerFrequency,
                                       int   nThreshold,
                                       int   nMinimumGain,
                                       int   nStreamData,
                                       float fDutyCycle,
                                       float fVoltage,
                                       float fCollectionTime,
                                       float fPreTriggerTime,
                                       int   nFlags ),
    NDI_DECL2 OptotrakActivateMarkers( void ),
    NDI_DECL2 OptotrakDeActivateMarkers( void ),
    NDI_DECL2 OptotrakGetStatus( int   *pnNumSensors,
                                 int   *pnNumOdaus,
                                 int   *pnNumRigidBodies,
                                 int   *pnMarkers,
                                 float *pfFrameFrequency,
                                 float *pfMarkerFrequency,
                                 int   *pnThreshold,
                                 int   *pnMinimumGain,
                                 int   *pnStreamData,
                                 float *pfDutyCycle,
                                 float *pfVoltage,
                                 float *pfCollectionTime,
                                 float *pfPreTriggerTime,
                                 int   *pnFlags ),
    NDI_DECL2 OptotrakChangeCameraFOR( char       *pszInputCamFile,
                                       int         nNumMarkers,
                                       Position3d *pdtMeasuredPositions,
                                       Position3d *pdtAlignedPositions,
                                       char       *pszAlignedCamFile,
                                       Position3d *pdt3dErrors,
                                       float      *pfRmsError ),
    NDI_DECL2 OptotrakSetStroberPortTable( int nPort1,
                                           int nPort2,
                                           int nPort3,
                                           int nPort4 ),
    NDI_DECL2 OptotrakSaveCollectionToFile( char *pszCollectFile ),
    NDI_DECL2 OptotrakGetErrorString( char* szErrorString, int nBufferSize ),
    NDI_DECL2 OptotrakSetProcessingFlags( unsigned int uFlags ),
    NDI_DECL2 OptotrakConvertRawTo3D( unsigned int *puElements,
                                      void         *pSensorReadings,
                                      Position3d   *pdt3DPositions ),
    NDI_DECL2 OptotrakConvertTransforms( unsigned int               *puElements,
                                         struct OptotrakRigidStruct *pDataDest6D,
                                         Position3d                 *pDataDest3 ),
    NDI_DECL2 OptotrakGetNodeInfo( int   nNodeId,
                                   struct OptoNodeInfoStruct *pdtNodeInfo ),
    NDI_DECL2 OptotrakSetCameraParameters( int nMarkerType,
                                           int nWaveLength,
                                           int nModelType ),
    NDI_DECL2 OptotrakGetCameraParameterStatus( int  *pnCurrentMarkerType,
                                                int  *pnCurrentWaveLength,
                                                int  *pnCurrentModelType,
                                                char *szStatus,
                                                int   nStatusLength ),
    NDI_DECL2 OptotrakReadAppParms( char        *pszParmFile,
                                    char        *pszSection,
                                    AppParmInfo *pParmInfo ),
    NDI_DECL2 OptotrakSaveAppParms( char        *pszParmFile,
                                    char        *pszSection,
                                    AppParmInfo *pParmInfo ),
    NDI_DECL2 OptotrakDeleteAppParms( char *pszParmFile,
                                      char *pszSection ),
    NDI_DECL2 OdauSaveCollectionToFile( char *pszCollectFile ),
    NDI_DECL2 OdauSetupCollectionFromFile( char *pszCollectFile ),
    NDI_DECL2 OdauSetTimer( int           nOdauId,
                            unsigned      uTimer,
                            unsigned      uMode,
                            unsigned long ulVal ),
    NDI_DECL2 OdauLoadControlFiFo( int                           nOdauId,
                                   unsigned                      uEntries,
                                   struct OdauControlWordStruct *pCtlWords ),
    NDI_DECL2 OdauSetAnalogOutputs( int       nOdauId,
                                    float    *pfVoltage1,
                                    float    *pfVoltage2,
                                    unsigned  uChangeMask ),
    NDI_DECL2 OdauSetDigitalOutputs( int       nOdauId,
                                     unsigned *puDigitalOut,
                                     unsigned  uUpdateMask ),
    NDI_DECL2 OdauSetupCollection( int      nOdauId,
                                   int      nChannels,
                                   int      nGain,
                                   int      nDigitalMode,
                                   float    fFrameFrequency,
                                   float    fScanFrequency,
                                   int      nStreamData,
                                   float    fCollectionTime,
                                   float    fPreTriggerTime,
                                   unsigned uFlags ),
  NDI_DECL2 OptotrakStopCollection( void ),      /* [EC-02-0548] - EJG added */
    NDI_DECL2 OdauGetStatus( int       nOdauId,
                             int      *pnChannels,
                             int      *pnGain,
                             int      *pnDigitalMode,
                             float    *pfFrameFrequency,
                             float    *pfScanFrequency,
                             int      *pnStreamData,
                             float    *pfCollectionTime,
                             float    *pfPreTriggerTime,
                             unsigned *puCollFlags,
                             int      *pnFlags ),
    NDI_DECL2 RigidBodyAdd( int    nRigidBodyId,
                            int    nStartMarker,
                            int    nNumMarkers,
                            float *pRigidCoordinates,
                            float *pNormalCoordinates,
                            int    nFlags ),
    NDI_DECL2 RigidBodyAddFromFile( int   nRigidBodyId,
                                    int   nStartMarker,
                                    char *pszRigFile,
                                    int   nFlags ),
    NDI_DECL2 RigidBodyChangeSettings( int   nRigidBodyId,
                                       int   nMinMarkers,
                                       int   nMaxMarkersAngle,
                                       float fMax3dError,
                                       float fMaxSensorError,
                                       float fMax3dRmsError,
                                       float fMaxSensorRmsError,
                                       int   nFlags ),
    NDI_DECL2 RigidBodyDelete( int nRigidBodyId ),
    NDI_DECL2 RigidBodyChangeFOR( int nRigidId, int nRotationMethod ),
    NDI_DECL2 DataGetLatest3D( unsigned int *puFrameNumber,
                               unsigned int *puElements,
                               unsigned int *puFlags,
                               void         *pDataDest ),
    NDI_DECL2 DataGetLatestCentroid( unsigned int *puFrameNumber,
                               unsigned int *puElements,
                               unsigned int *puFlags,
                               void         *pDataDest ),
    NDI_DECL2 DataGetLatestRaw( unsigned int *puFrameNumber,
                                unsigned int *puElements,
                                unsigned int *puFlags,
                                void         *pDataDest ),
    NDI_DECL2 DataGetLatestTransforms( unsigned int *puFrameNumber,
                                       unsigned int *puElements,
                                       unsigned int *puFlags,
                                       void         *pDataDest ),
    NDI_DECL2 DataGetLatestOdauRaw( int nOdauId,
                                    unsigned int *puFrameNumber,
                                    unsigned int *puElements,
                                    unsigned int *puFlags,
                                    void         *pDataDest ),
    NDI_DECL2 RequestLatest3D( void ),
    NDI_DECL2 RequestLatestRaw( void ),
    NDI_DECL2 RequestLatestCentroid( void ),
    NDI_DECL2 RequestLatestTransforms( void ),
    NDI_DECL2 RequestLatestOdauRaw( int nOdauId ),
    NDI_DECL2 DataIsReady( void ),
    NDI_DECL2 ReceiveLatestData( unsigned int *uFrameNumber,
                                 unsigned int *uElements,
                                 unsigned int *uFlags,
                                 void         *pDataDest ),
    NDI_DECL2 DataReceiveLatest3D( unsigned int *puFrameNumber,
                                   unsigned int *puElements,
                                   unsigned int *puFlags,
                                   Position3d   *pDataDest ),
    NDI_DECL2 DataReceiveLatestRaw( unsigned int *puFrameNumber,
                                    unsigned int *puElements,
                                    unsigned int *puFlags,
                                    void         *pDataDest ),
    NDI_DECL2 DataReceiveLatestCentroid( unsigned int *puFrameNumber,
                                    unsigned int *puElements,
                                    unsigned int *puFlags,
                                    void         *pDataBuff ),
    NDI_DECL2 DataReceiveLatestTransforms( unsigned int *puFrameNumber,
                                           unsigned int *puElements,
                                           unsigned int *puFlags,
                                           void         *pDataDest ),
    NDI_DECL2 DataReceiveLatestOdauRaw( unsigned int *puFrameNumber,
                                        unsigned int *puElements,
                                        unsigned int *puFlags,
                                        int          *pDataDest ),
    NDI_DECL2
        DataGetLatestTransforms2( unsigned int               *puFrameNumber,
                                  unsigned int               *puElements,
                                  unsigned int               *puFlags,
                                  struct OptotrakRigidStruct *pDataDest6D,
                                  Position3d                 *pDataDest3D ),
    NDI_DECL2
        DataReceiveLatestTransforms2( unsigned int               *puFrameNumber,
                                      unsigned int               *puElements,
                                      unsigned int               *puFlags,
                                      struct OptotrakRigidStruct *pDataDest6D,
                                      Position3d                 *pDataDest3D ),
    NDI_DECL2 
    RetrieveSwitchData( int     nNumSwitches,
                            boolean *pbSwitchData );



/*
 * Device Handle functions for Optotrak Certus
 */
NDI_DECL1 int
  NDI_DECL2 OptotrakGetNumberDeviceHandles( int *pnDeviceHandles ),
  NDI_DECL2 OptotrakGetDeviceHandles( DeviceHandle *pdtDeviceHandles,
                                        int           nDeviceHandles,
                                        unsigned int *puFlags ),
  NDI_DECL2 OptotrakDeviceHandleFree( int nDeviceHandleID ),
  NDI_DECL2 OptotrakDeviceHandleGetNumberProperties( int nDeviceHandleID,
                                                     int *pnProperties ),
  NDI_DECL2 OptotrakDeviceHandleGetProperties( int                   nDeviceHandleID,
                                               DeviceHandleProperty *pdtProperties,
                         int                   nProperties ),
  NDI_DECL2 OptotrakDeviceHandleGetProperty( int                   nDeviceHandleID,
                                             DeviceHandleProperty *pdtProperty,
                         unsigned int          uProperty ),
  NDI_DECL2 OptotrakDeviceHandleSetProperties( int                   nDeviceHandleID,
                                               DeviceHandleProperty* grdtProperties,
                         int                   nProperties ),
  NDI_DECL2 OptotrakDeviceHandleEnable( int nDeviceHandleID ),
  NDI_DECL2 RigidBodyAddFromDeviceHandle( int       nDeviceHandleID,
                      int          nRigidBodyId,
                      unsigned int uFlags ),
  NDI_DECL2 OptotrakDeviceHandleSetVisibleLED( int nDeviceHandleID,
                         int nLED,
                         enum VLEDState dtState ),
  NDI_DECL2 OptotrakDeviceHandleSetBeeper( int nDeviceHandleID,
                       int nDuration_ms ),


  NDI_DECL2 OptotrakDeviceHandleSROMRead( int nDeviceHandleID,
                      unsigned long ulAddress,
                      void* pData,
                      int nDataLength ),
  NDI_DECL2 OptotrakDeviceHandleSROMWrite( int nDeviceHandleID,
                       unsigned long ulAddress,
                       void* pData,
                       int nDataLength ),
  NDI_DECL2 OptotrakDeviceHandleUserSROMRead( int nDeviceHandleID,
                        unsigned long ulAddress,
                        void* pData,
                        int nDataLength ),
  NDI_DECL2 OptotrakDeviceHandleUserSROMWrite( int nDeviceHandleID,
                         unsigned long ulAddress,
                         void* pData,
                         int nDataLength );


NDI_DECL1 int
    NDI_DECL2 RequestNext3D( void ),
    NDI_DECL2 RequestNextCentroid( void ),
    NDI_DECL2 RequestNextRaw( void ),
    NDI_DECL2 RequestNextTransforms( void ),
    NDI_DECL2 RequestNextOdauRaw( int nOdauId ),
    NDI_DECL2 DataGetNext3D( unsigned int *puFrameNumber,
                             unsigned int *puElements,
                             unsigned int *puFlags,
                             void         *pDataDest ),
    NDI_DECL2 DataGetNextCentroid( unsigned int *puFrameNumber,
                                   unsigned int *puElements,
                                   unsigned int *puFlags,
                                   void         *pDataDest ),
    NDI_DECL2 DataGetNextRaw( unsigned int *puFrameNumber,
                              unsigned int *puElements,
                              unsigned int *puFlags,
                              void         *pDataDest ),
    NDI_DECL2 DataGetNextTransforms( unsigned int *puFrameNumber,
                                     unsigned int *puElements,
                                     unsigned int *puFlags,
                                     void         *pDataDest ),
    NDI_DECL2
        DataGetNextTransforms2( unsigned int               *puFrameNumber,
                                unsigned int               *puElements,
                                unsigned int               *puFlags,
                                struct OptotrakRigidStruct *pDataDest6D,
                                Position3d                 *pDataDest3D ),
    NDI_DECL2 DataGetNextOdauRaw( int nOdauId,
                                  unsigned int *puFrameNumber,
                                  unsigned int *puElements,
                                  unsigned int *puFlags,
                                  void         *pDataDest );

NDI_DECL1 int
    NDI_DECL2 DataBufferInitializeFile( unsigned int  uDataId,
                                        char         *pszFileName ),
    NDI_DECL2 DataBufferInitializeMem( unsigned int uDataId,
                                       SpoolPtrType pMemory ),
    NDI_DECL2 DataBufferStart( void ),
    NDI_DECL2 DataBufferStop( void ),
    NDI_DECL2 DataBufferSpoolData( unsigned int  *puSpoolStatus ),
    NDI_DECL2 DataBufferWriteData( unsigned int  *puRealtimeData,
                                   unsigned int  *puSpoolComplete,
                                   unsigned int  *puSpoolStatus,
                                   unsigned long *pulFramesBuffered ),
    NDI_DECL2 DataBufferAbortSpooling( void );

NDI_DECL1 int
    NDI_DECL2 FileConvert( char         *pszInputFilename,
                           char         *pszOutputFilename,
                           unsigned int  uFileType ),
    NDI_DECL2 FileOpen( char          *pszFilename,
                        unsigned int   uFileId,
                        unsigned int   uFileMode,
                        int           *pnItems,
                        int           *pnSubItems,
                        long int      *plnFrames,
                        float         *pfFrequency,
                        char          *pszComments,
                        void         **pFileHeader ),
    NDI_DECL2 FileRead( unsigned int  uFileId,
                        long int      lnStartFrame,
                        unsigned int  uNumberOfFrames,
                        void         *pDataDest ),
    NDI_DECL2 FileWrite( unsigned int  uFileId,
                         long int      lnStartFrame,
                         unsigned int  uNumberOfFrames,
                         void         *pDataSrc ),
    NDI_DECL2 FileClose( unsigned int uFileId ),
    NDI_DECL2 FileOpenAll( char          *pszFilename,
                           unsigned int   uFileId,
                           unsigned int   uFileMode,
                           int           *pnItems,
                           int           *pnSubItems,
                           int           *pnCharSubItems,
                           int           *pnIntSubItems,
                           int           *pnDoubleSubItems,
                           long int      *plnFrames,
                           float         *pfFrequency,
                           char          *pszComments,
                           void         **pFileHeader ),
    NDI_DECL2 FileReadAll( unsigned int  uFileId,
                           long int      lnStartFrame,
                           unsigned int  uNumberOfFrames,
                           void         *pDataDestFloat,
                           void         *pDataDestChar,
                           void         *pDataDestInt,
                           void         *pDataDestDouble ),
    NDI_DECL2 FileWriteAll( unsigned int  uFileId,
                            long int      lnStartFrame,
                            unsigned int  uNumberOfFrames,
                            void         *pDataSrcFloat,
                            void         *pDataSrcChar,
                            void         *pDataSrcInt,
                            void         *pDataSrcDouble ),
    NDI_DECL2 FileCloseAll( unsigned int uFileId );

NDI_DECL1 void
    NDI_DECL2 CombineXfrms( transformation *xfrm1_ptr,
                            transformation *xfrm2_ptr,
                            transformation *NewXfrmPtr ),
    NDI_DECL2 CvtQuatToRotationMatrix( struct QuatRotationStruct *QuatPtr,
                                       RotationMatrixType         r ),
    NDI_DECL2 CvtRotationMatrixToQuat( struct QuatRotationStruct *QuatPtr,
                                       RotationMatrixType         r ),
    NDI_DECL2 DetermineEuler( RotationMatrixType  r,
                              RotationAngleType           *RotationAnglePtr ),
    NDI_DECL2 DetermineR( RotationAngleType *RotationAnglePtr,
                            float( *RotationMatrix )[ 3] ),
    NDI_DECL2 DetermineR_d( rotation_d *RotationAnglePtr,
                  double   ( *RotationMatrix )[ 3] ),
    NDI_DECL2 InverseXfrm( transformation *InputXfrmPtr,
                           transformation *OutputXfrmPtr ),
  NDI_DECL2 TransformPoint( float     ( *RotationMatrix )[ 3],
                                Position3d *translation,
                                Position3d *OriginalPositionPtr,
                              Position3d *RotatedPositionPtr ),
  NDI_DECL2 TransformPoint_d( double      ( *RotationMatrix )[ 3],
                                  Position3d_d *translation,
                                  Position3d_d *OriginalPositionPtr,
                                Position3d_d *RotatedPositionPtr );

NDI_DECL1 int
  NDI_DECL2 nOptotrakAlignSystem( AlignParms dtAlignParms,
                  float* fRMSError ),
  NDI_DECL2 nOptotrakRegisterSystem(  RegisterParms dtRegisterParms,
                    float* fRMSError ),
  NDI_DECL2 nOptotrakCalibrigSystem(  CalibrigParms dtCalibrigParms,
                    float * fRMSError );

CPLUSPLUS_END



#endif /* INCLUDE_NDOPTO */ 
