/*****************************************************************
Name:      datatypes.h

Description:
  Data types used by the Optotrak sample programs.  

*****************************************************************/

#define OPTO_APP_ERROR_CODE          3000
#define OPTOTRAK_REVISION_UNKNOWN_FLAG    0

typedef struct OptotrakSettingsStruct
{
  int
    nMarkers,
    nThreshold,
    nMinimumGain,
    nStreamData;
  float
    fFrameFrequency,
    fMarkerFrequency,
    fDutyCycle,
    fVoltage,
    fCollectionTime,
    fPreTriggerTime;
  unsigned int
    uFlags;

} OptotrakSettings;


typedef struct OptotrakStatusStruct
{
  int
    nSensors,
    nOdaus,
    nRigidBodies;
  OptotrakSettings
    dtSettings;

} OptotrakStatus;


typedef struct ApplicationDeviceInformationStruct
{
  char
    szName[DEVICE_MAX_PROPERTY_STRING],
    szFiringSequence[DEVICE_MAX_MARKERS];
  int
    nMarkersAvailable,
    nMarkersToActivate,
    n3020MarkersToActivate,
    nStartMarkerPeriod,
    n3020StartMarkerPeriod,
    nPort,
    nOrder,
    nSubPort,
    nVLEDs,
    nSwitches,
    nToolPorts;
  boolean
    b3020Capability,
    bHasROM;
} ApplicationDeviceInformation;
