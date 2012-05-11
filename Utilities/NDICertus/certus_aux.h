/*****************************************************************
Name:      certus_aux.h

Description:
  Auxiliary functions for the Optotrak Certus sample programs.
  This file contains commonly used functions for the Certus,
  including device detection and memory allocation.

*****************************************************************/

/*****************************************************************
Project Files Included
*****************************************************************/
#include "datatypes.h"

/*****************************************************************
ND Library Files Included
*****************************************************************/

/*****************************************************************

Name:    AllocateMemoryDeviceHandles

Input Values:
  int
    nDevices        number of devices for which to allocate memory

Output Values:
  DeviceHandle*
    *ppdtDeviceHandles    pointer to the newly allocated memory for
                the specified number of device handles

Return Values:
  int
    OPTO_NO_ERROR_CODE    success
    OPTO_APP_ERROR_CODE    error allocating memory

Description:
  Allocate memory to contain the specified number of device handles;

*****************************************************************/
int AllocateMemoryDeviceHandles( DeviceHandle **ppdtDeviceHandles, int nDevices );


/*****************************************************************

Name:    AllocateMemoryDeviceHandlesInfo

Input Values:
  int
    nDevices        number of devices for which to allocate memory
  DeviceHandle*
    pDeviceHandles      array of existing device handles

Output Values:
  DeviceHandle*
    *ppdtDeviceHandlesInfo  pointer to newly allocated memory for device handles information

Return Values:
  int
    OPTO_NO_ERROR_CODE  success
    OPTO_APP_ERROR_CODE  error allocating memory

Description:
  Allocate memory to contain the specified number of devices.
  The device handle information contains a pointer to the
  associated device handle.  This function also sets up the
  DeviceHandleInfo structures' pointers.

*****************************************************************/
int AllocateMemoryDeviceHandlesInfo( DeviceHandleInfo **ppdtDeviceHandlesInfo, DeviceHandle *pdtDeviceHandles, int nDevices );



/*****************************************************************

Name:    AllocateMemoryDeviceHandleProperties

Input Values:
  int
    nProperties      number of properties

Output Values:
  DeviceHandleProperty*
    *ppdtProperties    pointer to newly allocated array of properties

Return Values:
  int
    OPTO_NO_ERROR_CODE  success
    OPTO_APP_ERROR_CODE  error allocating memory

Description:
  allocate memory to conatin the specified number of properties.

*****************************************************************/
int AllocateMemoryDeviceHandleProperties( DeviceHandleProperty **pdtProperties, int nProperties );


/*****************************************************************

Name:    ApplicationStoreDeviceProperties

Input Values:
  int
    nDevices        number of devices available in the system
  DeviceHandleInfo*
    pdtDeviceHandleInfo    array containing information about each 
                device handle in native format

Output Values:
  ApplicationDeviceInformation*
    *ppdtAppDevices      pointer to array containing information about 
                each device for the application

Return Values:
  int
    OPTO_NO_ERROR_CODE  success
    OPTO_APP_ERROR_CODE  error storing device information

Description:
  Extract the device information from the native format and store
  it in the application structure.

*****************************************************************/
int ApplicationStoreDeviceProperties( ApplicationDeviceInformation **ppdtAppDevices,
                    DeviceHandleInfo *pdtDeviceHandleInfo, int nDevices );


/*****************************************************************

Name:    uDetermineSystem

Input Values:
  <none>

Output Values:
  <none>

Return Values:
  unsigned int
    System version as indicated by the OptotrakGetStatus call.

Description:
  Determine the version of the system that is attached.
  The system version is returned in the flags field of the
  OptotrakGetStatus function call.

*****************************************************************/
unsigned int uDetermineSystem( );


/*****************************************************************

Name:    DetermineStroberConfiguration

Input Values:
  <none>

Output Values:
  DeviceHandle*
    *ppdtDeviceHandles    pointer to array of device handles.
                this function will allocate the necessary
                memory for the device handles
  DeviceHandleInfo*
    *ppdtDeviceHandlesInfo  pointer to array of device handle information.
                this function will allocate the necessary
                memory for the device handle information
  int*
    pnDevices        pointer to integer containing the number
                of devices detected on the system.

Return Values:
  int
    OPTO_NO_ERROR_CODE    success
    ERROR_CODE        if there is an error

Description:
  Determine the current strober configuration.  Query the
  system to determine the number of strobers and each strober's
  properties.

*****************************************************************/
int DetermineStroberConfiguration( DeviceHandle** ppdtDeviceHandles, 
                   DeviceHandleInfo** ppdtDeviceHandlesInfo,
                   int* pnDevices );


/*****************************************************************

Name:    GetDeviceHandlesFromSystem

Input Values:
  <none>

Output Values:
  DeviceHandle**
    ppdtDeviceHandles    pointer to device handles array.
                this function will request memory be
                allocated for the devices available
                in the system.
  int*
    pnDevices        pointer to number of device handles

Return Values:
  int
    OPTO_NO_ERROR_CODE  success
    OPTO_ERROR_CODE    if there is an error

Description:
  Retrieve the current device handles in the system.
  Request the device handles from the system and store the
  information in a new DeviceHandles buffer pointed to
  by the specified DeviceHandles pointer.

*****************************************************************/
int GetDeviceHandlesFromSystem( DeviceHandle **ppdtDeviceHandles, int *pnDevices );


/*****************************************************************

Name:    GetDevicePropertiesFromSystem

Input Values:
  DeviceHandleInfo*
    pdtDevice    pointer to the device for which to retrieve the properties.

Output Values:
  <none>

Return Values:
  int
    OPTO_NO_ERROR_CODE  success
    OPTO_ERROR_CODE    if there is an error

Description:
  Retrieve the device properties from the system for the
  specified device.

*****************************************************************/
int GetDevicePropertiesFromSystem( DeviceHandleInfo* pdtDevice );


/*****************************************************************

Name:    ApplicationDetermineCollectionParameters

Input Values:
  int
    nDevices      number of devices in the devices array
  ApplicationDeviceInformation
    *pdtDevices      array of known devices

Output Values:
  OptotrakSettings*
    pdtSettings      optotrak collection settings

Return Values:
  int
    OPTO_NO_ERROR_CODE  success
    OPTO_APP_ERROR_CODE  error storing device information

Description:
  Determine the collection parameters to use based on the
  devices present in the system.

*****************************************************************/
int ApplicationDetermineCollectionParameters( int nDevices, ApplicationDeviceInformation* pdtDevices, 
                        OptotrakSettings* pdtSettings );


/*****************************************************************

Name:    SetMarkersToActivateForDevice

Input Values:
  ApplicationDeviceInformation
    *pdtDevice      device to set up
  int
    nDeviceHandle    device handle for the specified device
  int
    nMarkers      number of markers to fire on this device

Output Values:
  <none>

Return Values:
  int
    OPTO_NO_ERROR_CODE  success
    OPTO_APP_ERROR_CODE  error setting device properties

Description:
  Set the number of markers to fire and the firing sequence
  (where appropriate) for the specified device.

*****************************************************************/
int SetMarkersToActivateForDevice( ApplicationDeviceInformation* pdtDevice, int nDeviceHandle, int nMarkers );


/*****************************************************************

Name:    SetMarkersToActivateForToolDevice

Input Values:
  ApplicationDeviceInformation
    *pdtDevice      device to set up
  int
    nDeviceHandle    device handle for the specified device
  int
    nMarkers      number of markers to fire on this device
  ApplicationDeviceInformation
    *pdtDevices      all application devices
  int
    nDevices      number of application devices

Output Values:
  <none>

Return Values:
  int
    OPTO_NO_ERROR_CODE  success
    OPTO_APP_ERROR_CODE  error setting device properties

Description:
  Set the number of markers to fire for the specified device.

*****************************************************************/
int SetMarkersToActivateForToolDevice( ApplicationDeviceInformation* pdtDevice, int nDeviceHandle, int nMarkers, ApplicationDeviceInformation* pdtDevices, int nDevices );
