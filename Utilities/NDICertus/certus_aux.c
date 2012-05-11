/*****************************************************************
Name:      certus_aux.c

Description:
  Auxiliary functions for the Optotrak Certus sample programs.
  This file contains commonly used functions for the Certus,
  including device detection and memory allocation.

*****************************************************************/

/*****************************************************************
C Library Files Included
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

/*****************************************************************
ND Library Files Included
*****************************************************************/
#include "ndtypes.h"
#include "ndpack.h"
#include "ndopto.h"

/*****************************************************************
Application Files Included
*****************************************************************/
#include "certus_aux.h"


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
unsigned int uDetermineSystem( )
{
  int
    nFlags;
  unsigned int
    uSystemVersion;

    /*
     * Determine the default Optotrak status.
     * Retrieve the default Optotrak settings.
     * The nFlags variable will contain information about the version of
     * the SCU to which we are connected.  Once we know the hardware
     * version, we know what functions can be called.
     */
    if( OptotrakGetStatus(  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
              &nFlags ) != OPTO_NO_ERROR_CODE )
  {
    return OPTOTRAK_REVISION_UNKNOWN_FLAG;
  } /* if */

  /*
   * if the system detected is not an Optotrak Certus, do not continue
   */
    uSystemVersion = ( nFlags & ( OPTOTRAK_3020_FLAG | OPTOTRAK_CERTUS_FLAG ) );

  if( uSystemVersion & OPTOTRAK_REVISIOND_FLAG )
  {
    fprintf( stdout, "\tSystem Detected: Optotrak Rev D.\nThis sample is intended for Optotrak Certus systems.\n" );
    return OPTOTRAK_REVISIOND_FLAG;
  } /* if */

  if( uSystemVersion & OPTOTRAK_CERTUS_FLAG )
  {
    fprintf( stdout, "\tSystem Detected: Optotrak Certus\n" );
    return OPTOTRAK_CERTUS_FLAG;
  } /* if */

  fprintf( stdout, "\tUnknown System detected.\n" );
  return OPTOTRAK_REVISION_UNKNOWN_FLAG;

} /* uDetermineSystem */


/*****************************************************************

Name:    DetermineStroberConfiguration

Input Values:
  <none>

Output Values:
  DeviceHandle**
    ppdtDeviceHandles    pointer to device handles
    DeviceHandleInfo**
        ppdtDeviceHandlesInfo  pointer to device handles information
  int*
    pnDevices        pointer to number of device handles

Return Values:
  int
    OPTO_NO_ERROR_CODE    success
    ERROR_CODE        if there is an error

Description:
  Determine the current strober configuration.  Query the
  system to determine the number of strobers and each strober's
  properties.

*****************************************************************/
int DetermineStroberConfiguration( DeviceHandle** ppdtDeviceHandles, DeviceHandleInfo** ppdtDeviceHandlesInfo, int *pnDevices )
{
  boolean
        bAllDeviceHandlesSetUp;
  int
    i,
    nRet;

  /*
   * Delete any previous memory allocated for device handles and
   * device handles information.  New memory will be allocated
   * in this routine for these data arrays.
   */
  AllocateMemoryDeviceHandles( ppdtDeviceHandles, 0 );
  AllocateMemoryDeviceHandlesInfo( ppdtDeviceHandlesInfo, *ppdtDeviceHandles, 0 );

    /*
     * loop until all device handles are set up
     * - retrieve the number of device handles
     * - free any device handles that are no longer being used
     * - enable any devices that have been recently initialized,
     *   but have not yet been enabled
     * - once all devices have been enabled, determine the final
     *   strober configuration
     *
     * the strober configuration can change at any point in time
     * so the application must continously check if the
     * configuration has changed and re-start the setup loop
     * when necessary.
     */
    bAllDeviceHandlesSetUp = FALSE;
    while( !bAllDeviceHandlesSetUp )
    {
        /*
         * The bAllDeviceHandlesSetUp indicates if the all strober devices have
         * been initialized and enabled, and no device handles need to be freed
         * Make the assumption that all device handles are set up and change
         * the flag when any change is made
         */
        bAllDeviceHandlesSetUp = TRUE;

        /*
         * ask the system to provide all the device handles
         */
    nRet = GetDeviceHandlesFromSystem( ppdtDeviceHandles, pnDevices );
    if( nRet != OPTO_NO_ERROR_CODE )
    {
      goto ERROR_EXIT;
    } /* if */

    /*
     * allocate memory to maintain all the device handle information
     */
    nRet = AllocateMemoryDeviceHandlesInfo( ppdtDeviceHandlesInfo, *ppdtDeviceHandles, *pnDevices );
        if( nRet != OPTO_NO_ERROR_CODE )
    {
      goto ERROR_EXIT;
    } /* if */

        /*
         * Iterate through each of the device handles in order to retrieve
         * all the device properties.
         * Ensure that all devices have been enabled.  If any device handles
         * need to be freed or any devices need to be enabled, set a flag
         * to indicate that the device handle search loop must run again.
         */
        for( i = 0; i < *pnDevices; i++ )
        {
            /*
             * Check the device handle status, and perform the appropriate task
             * If a device has become unoccupied, the device handle needs to be freed.
             * If a device handle has been recently initialized (this would happen during
             * the OptotrakGetNumberDeviceHandles function), the device needs to be enabled.
             */
            switch( (*ppdtDeviceHandlesInfo)[i].pdtHandle->dtStatus )
            {
                /*
                 * If a port has become unoccupied, free the device handle previously
                 * associated with that port and indicate that a change has occurred
                 */
              case DH_STATUS_UNOCCUPIED:
        nRet = OptotrakDeviceHandleFree( (*ppdtDeviceHandlesInfo)[i].pdtHandle->nID );
                if( nRet != OPTO_NO_ERROR_CODE )
        {
          goto ERROR_EXIT;
        } /* if */
                bAllDeviceHandlesSetUp = FALSE;
                break;

                /*
                 * If a device has been recently initialized, it needs to be enabled.
                 * To enable the device, retrieve all the device handle properties,
                 * change any properties as appropriate, set the device handle properties,
                 * and enable the device handle.
                 */
              case DH_STATUS_INITIALIZED:
        nRet = GetDevicePropertiesFromSystem( &((*ppdtDeviceHandlesInfo)[i]) );
        if( nRet != OPTO_NO_ERROR_CODE )
        {
          goto ERROR_EXIT;
        } /* if */

        nRet = OptotrakDeviceHandleEnable( (*ppdtDeviceHandlesInfo)[i].pdtHandle->nID );
                if( nRet != OPTO_NO_ERROR_CODE )
        {
          goto ERROR_EXIT;
        } /* if */
                bAllDeviceHandlesSetUp = FALSE;
                break;

                /*
                 * If this device has been enabled, retrieve all the device properties.
                 * In the final run through device handle setup loop, all devices
                 * will have been enabled.  Thus, at the end of the loop, the application
                 * will have all the device handles and all the device properties.
                 */
              case DH_STATUS_ENABLED:
        nRet = GetDevicePropertiesFromSystem( &((*ppdtDeviceHandlesInfo)[i]) );
        if( nRet != OPTO_NO_ERROR_CODE )
        {
          goto ERROR_EXIT;
        } /* if */
                break;

        default:
        fprintf( stdout, "Unable to determine Device Handle status (Device_%d)", i );
        goto ERROR_EXIT;
            } /* switch */
        } /* for */
    } /* while */

  return OPTO_NO_ERROR_CODE;

ERROR_EXIT:
  /*
   * if there is an error while executing this fuction,
   * delete all the memory allocated for device handles
   */
  if( *ppdtDeviceHandlesInfo )
  {
    for( i = 0; i < *pnDevices; i++ )
    {
      AllocateMemoryDeviceHandleProperties( &(((*ppdtDeviceHandlesInfo)[i]).grProperties), 0 );
    } /* for */
  } /* if */
  AllocateMemoryDeviceHandles( ppdtDeviceHandles, 0 );
  AllocateMemoryDeviceHandlesInfo( ppdtDeviceHandlesInfo, *ppdtDeviceHandles, 0 );

  return nRet;

} /* DetermineStroberConfiguration */


/*****************************************************************

Name:    GetDeviceHandlesFromSystem

Input Values:
  <none>

Output Values:
  DeviceHandle**
    ppdtDeviceHandles    pointer to device handles array
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
int GetDeviceHandlesFromSystem( DeviceHandle **ppdtDeviceHandles, int *pnDevices )
{
  unsigned int
    uFlags;
  int
    i,
    nRet;

  uFlags = OPTO_TOOL_CONFIG_CHANGED_FLAG;
    while( uFlags & OPTO_TOOL_CONFIG_CHANGED_FLAG )
    {
    /*
     * retrieve the number of device handles known by the system
     */
    *pnDevices = 0;
    nRet = OptotrakGetNumberDeviceHandles( pnDevices );
        if( nRet != OPTO_NO_ERROR_CODE )
    {
      fprintf( stdout, "Error retrieving the number of device handles in the system\n" );
      return nRet;
    } /* if */

    /*
     * display information to screen
     */
    fprintf( stdout, ".........Found %d devices\n", *pnDevices );

    /*
     * allocate enough memory to hold the device handles
     */
    nRet = AllocateMemoryDeviceHandles( ppdtDeviceHandles, *pnDevices );
        if( nRet != OPTO_NO_ERROR_CODE )
    {
      return nRet;
    } /* if */

    /*
     * retrieve the device handles from the system
     */
    nRet = OptotrakGetDeviceHandles( *ppdtDeviceHandles, *pnDevices, &uFlags );
        if( nRet != OPTO_NO_ERROR_CODE )
    {
      fprintf( stdout, "Error retrieving the device handles from the system\n" );
      return nRet;
    } /* if */

    /*
     * display devices information to screen
     */
    for( i = 0; i < *pnDevices; i++ )
    {
      fprintf( stdout, ".........Device %.3d (ID %d) status: ", i + 1, (*ppdtDeviceHandles)[i].nID );
      switch( (*ppdtDeviceHandles)[i].dtStatus )
      {
      case DH_STATUS_UNOCCUPIED:
        fprintf( stdout, "DH_STATUS_UNOCCUPIED\n" );
        break;

      case DH_STATUS_OCCUPIED:
        fprintf( stdout, "DH_STATUS_OCCUPIED\n" );
        break;

      case DH_STATUS_INITIALIZED:
        fprintf( stdout, "DH_STATUS_INITIALIZED\n" );
        break;

      case DH_STATUS_ENABLED:
        fprintf( stdout, "DH_STATUS_ENABLED\n" );
        break;

      case DH_STATUS_FREE:
        fprintf( stdout, "DH_STATUS_FREE\n" );
        break;

      default:
        fprintf( stdout, "*unknown*\n" );
        break;

      } /* switch */
    } /* for */


    } /* if */

  return OPTO_NO_ERROR_CODE;

} /* GetDeviceHandlesFromSystem */


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
int GetDevicePropertiesFromSystem( DeviceHandleInfo* pdtDevice )
{
  int
    nRet;

  /*
   * retrieve the number of properties available for the specified device
   */
  nRet = OptotrakDeviceHandleGetNumberProperties( pdtDevice->pdtHandle->nID,  &pdtDevice->nProperties );
    if( nRet != OPTO_NO_ERROR_CODE )
  {
    return nRet;
  } /* if */

  /*
   * allocate the appropriate memory for the specified device
   */
  nRet = AllocateMemoryDeviceHandleProperties( &(pdtDevice->grProperties), pdtDevice->nProperties );
    if( nRet != OPTO_NO_ERROR_CODE )
  {
    return nRet;
  } /* if */

  /*
   * retrieve the actual device properties for the specified device
   */
  nRet = OptotrakDeviceHandleGetProperties( pdtDevice->pdtHandle->nID, pdtDevice->grProperties, pdtDevice->nProperties );
    if( nRet != OPTO_NO_ERROR_CODE )
  {
    return nRet;
  } /* if */

  return OPTO_NO_ERROR_CODE;

} /* GetDevicePropertiesFromSystem */


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
int AllocateMemoryDeviceHandles( DeviceHandle **ppdtDeviceHandles, int nDevices )
{
  int
    nSize;

  /*
   * delete any memory previously allocated for the devicehandles
   */
  if( *ppdtDeviceHandles )
  {
    free( *ppdtDeviceHandles );
    *ppdtDeviceHandles = NULL;
  } /* if */

  /*
   * allocate new memory for the specified number of device handles
   */
  if( nDevices > 0 )
  {
    nSize = nDevices * sizeof( DeviceHandle );
    *ppdtDeviceHandles = (DeviceHandle*)malloc( nSize );
    if( !*ppdtDeviceHandles )
    {
      fprintf( stdout, "Error allocating memory for device handles (AllocateMemoryDeviceHandles)\n" );
      return OPTO_APP_ERROR_CODE;
    } /* if */

    memset( *ppdtDeviceHandles, 0, nSize );
  } /* if */

  return OPTO_NO_ERROR_CODE;

} /* AllocateMemoryDeviceHandles */


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
int AllocateMemoryDeviceHandlesInfo( DeviceHandleInfo **ppdtDeviceHandlesInfo, DeviceHandle *pdtDeviceHandles, int nDevices )
{
  int
    i,
    nSize;

  /*
   * delete any memory previously allocated for devices
   */
  if( *ppdtDeviceHandlesInfo )
  {
    if( (*ppdtDeviceHandlesInfo)->grProperties )
    {
      free( (*ppdtDeviceHandlesInfo)->grProperties );
    } /* if */

    free( *ppdtDeviceHandlesInfo );
    *ppdtDeviceHandlesInfo = NULL;
  } /* if */

  /*
   * allocate new memory for the specified number of devices
   */
  if( nDevices > 0 )
  {
    nSize = nDevices * sizeof( DeviceHandleInfo );
    *ppdtDeviceHandlesInfo = (DeviceHandleInfo*) malloc( nSize );
    if( !*ppdtDeviceHandlesInfo )
    {
      fprintf( stdout, "Error allocating memory for devices (AllocateMemoryDeviceHandlesInfo)\n" );
      return OPTO_APP_ERROR_CODE;
    } /* if */

    memset( *ppdtDeviceHandlesInfo, 0, nSize );
    (*ppdtDeviceHandlesInfo)->grProperties = NULL;
  } /* if */

  /*
   * set the pointer to the device handle for each device
   */
  for( i = 0; i < nDevices; i++ )
  {
    ((*ppdtDeviceHandlesInfo)[i]).pdtHandle = &(pdtDeviceHandles[i]);
  } /* for */

  return OPTO_NO_ERROR_CODE;

} /* AllocateMemoryDeviceHandlesInfo */


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
int AllocateMemoryDeviceHandleProperties( DeviceHandleProperty **ppdtProperties, int nProperties )
{
  int
    nSize;

  if( *ppdtProperties )
  {
    free( *ppdtProperties );
    *ppdtProperties = NULL;
  } /* if */

  /*
   * allocate new memory for the specified number of properties
   */
  if( nProperties > 0 )
  {
    nSize = nProperties * sizeof( DeviceHandleProperty );
    *ppdtProperties = (DeviceHandleProperty*) malloc( nSize );
    if( !*ppdtProperties )
    {
      fprintf( stdout, "Error allocating memory for device handle properties (AllocateMemoryDeviceHandleProperties)\n" );
      return OPTO_APP_ERROR_CODE;
    } /* if */

    memset( *ppdtProperties, 0, nSize );
  } /* if */

  return OPTO_NO_ERROR_CODE;

} /* AllocateMemoryDeviceHandleProperties */


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
                    DeviceHandleInfo *pdtDeviceHandleInfo, int nDevices )
{
  int
    i, j, ch,
    nSize,
    nProperties;
  DeviceHandleProperty
    *pdtProperty;

  /*
   * free the memory previously allocated for the application devices
   */
  if( *ppdtAppDevices )
  {
    free( *ppdtAppDevices );
    *ppdtAppDevices = NULL;
  } /* if */

  /*
   * allocate new memory for the application devices based on the number of devices specified
   */
  if( nDevices > 0 )
  {
    nSize = nDevices * sizeof( ApplicationDeviceInformation );
    *ppdtAppDevices = (ApplicationDeviceInformation*) malloc( nSize );
    if( !*ppdtAppDevices )
    {
      fprintf( stdout, "Error allocating memory for devices (ApplicationStoreDeviceProperties)\n" );
      return OPTO_APP_ERROR_CODE;
    } /* if */

    memset( *ppdtAppDevices, 0, nSize );
  } /* if */

  /*
   * iterate through each of the devices and extract the device properties
   */
  for( i = 0; i < nDevices; i++ )
  {
    nProperties = pdtDeviceHandleInfo[i].nProperties;

    for( j = 0; j < nProperties; j++ )
    {
      pdtProperty = &(pdtDeviceHandleInfo[i].grProperties[j]);
      switch( pdtProperty->uPropertyID )
      {
      case DH_PROPERTY_NAME:
        if( pdtProperty->dtPropertyType == DH_PROPERTY_TYPE_STRING )
        {
          for( ch = 0; ch < DEVICE_MAX_PROPERTY_STRING; ch++ )
          {
            ((*ppdtAppDevices)[i]).szName[ch] = pdtProperty->dtData.szData[ch];
          } /* for */
        } /* if */
        break;

      case DH_PROPERTY_MARKERSTOFIRE:
        if( pdtProperty->dtPropertyType == DH_PROPERTY_TYPE_INT )
        {
          ((*ppdtAppDevices)[i]).nMarkersToActivate = pdtProperty->dtData.nData;
        } /* if */
        break;

      case DH_PROPERTY_MAXMARKERS:
        if( pdtProperty->dtPropertyType == DH_PROPERTY_TYPE_INT )
        {
          ((*ppdtAppDevices)[i]).nMarkersAvailable = pdtProperty->dtData.nData;
        } /* if */
        break;

      case DH_PROPERTY_STARTMARKERPERIOD:
        if( pdtProperty->dtPropertyType == DH_PROPERTY_TYPE_INT )
        {
          ((*ppdtAppDevices)[i]).nStartMarkerPeriod = pdtProperty->dtData.nData;
        } /* if */
        break;

      case DH_PROPERTY_SWITCHES:
        if( pdtProperty->dtPropertyType == DH_PROPERTY_TYPE_INT )
        {
          ((*ppdtAppDevices)[i]).nSwitches = pdtProperty->dtData.nData;
        } /* if */
        break;

      case DH_PROPERTY_VLEDS:
        if( pdtProperty->dtPropertyType == DH_PROPERTY_TYPE_INT )
        {
          ((*ppdtAppDevices)[i]).nVLEDs = pdtProperty->dtData.nData;
        } /* if */
        break;

      case DH_PROPERTY_TOOLPORTS:
        if( pdtProperty->dtPropertyType == DH_PROPERTY_TYPE_INT )
        {
          ((*ppdtAppDevices)[i]).nToolPorts = pdtProperty->dtData.nData;
        } /* if */
        break;

      case DH_PROPERTY_PORT:
        if( pdtProperty->dtPropertyType == DH_PROPERTY_TYPE_INT )
        {
          ((*ppdtAppDevices)[i]).nPort = pdtProperty->dtData.nData;
        } /* if */
        break;

      case DH_PROPERTY_ORDER:
        if( pdtProperty->dtPropertyType == DH_PROPERTY_TYPE_INT )
        {
          ((*ppdtAppDevices)[i]).nOrder = pdtProperty->dtData.nData;
        } /* if */
        break;

      case DH_PROPERTY_SUBPORT:
        if( pdtProperty->dtPropertyType == DH_PROPERTY_TYPE_INT )
        {
          ((*ppdtAppDevices)[i]).nSubPort = pdtProperty->dtData.nData;
        } /* if */
        break;

      case DH_PROPERTY_FIRINGSEQUENCE:
        if( pdtProperty->dtPropertyType == DH_PROPERTY_TYPE_STRING )
        {
          for( ch = 0; ch < DEVICE_MAX_MARKERS; ch++ )
          {
            ((*ppdtAppDevices)[i]).szFiringSequence[ch] = pdtProperty->dtData.szData[ch];
          } /* for */
        } /* if */
        break;

      case DH_PROPERTY_HAS_ROM:
        if( pdtProperty->dtPropertyType == DH_PROPERTY_TYPE_INT )
        {
          ((*ppdtAppDevices)[i]).bHasROM = ( pdtProperty->dtData.nData != 0 );
        } /* if */
        break;

      case DH_PROPERTY_3020_CAPABILITY:
        if( pdtProperty->dtPropertyType == DH_PROPERTY_TYPE_INT )
        {
          ((*ppdtAppDevices)[i]).b3020Capability = ( pdtProperty->dtData.nData != 0 );
        } /* if */
        break;

      case DH_PROPERTY_3020_MARKERSTOFIRE:
        if( pdtProperty->dtPropertyType == DH_PROPERTY_TYPE_INT )
        {
          ((*ppdtAppDevices)[i]).n3020MarkersToActivate = pdtProperty->dtData.nData;
        } /* if */
        break;

      case DH_PROPERTY_3020_STARTMARKERPERIOD:
        if( pdtProperty->dtPropertyType == DH_PROPERTY_TYPE_INT )
        {
          ((*ppdtAppDevices)[i]).n3020StartMarkerPeriod = pdtProperty->dtData.nData;
        } /* if */
        break;

      case DH_PROPERTY_UNKNOWN:
      default:
        break;
      } /* switch */
    } /* for */
  } /* for */

  return OPTO_NO_ERROR_CODE;

} /* ApplicationStoreDeviceProperties */


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
                        OptotrakSettings* pdtSettings )
{
  int
    i;

  /*
   * determine the total number of markers to be fired
   */
  pdtSettings->nMarkers = 0;
  for( i = 0; i < nDevices; i++ )
  {
    pdtSettings->nMarkers += pdtDevices[i].nMarkersToActivate + pdtDevices[i].n3020MarkersToActivate;
  } /* for */

  return OPTO_NO_ERROR_CODE;

} /* ApplicationDetermineCollectionParameters */


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
int SetMarkersToActivateForDevice( ApplicationDeviceInformation* pdtDevice, int nDeviceHandle, int nMarkers )
{
  DeviceHandleProperty
    dtProperty;
  int
    i;

  /*
   * if this device has tool ports, do not fire any markers
   */
  if( pdtDevice->nToolPorts > 0 )
  {
    nMarkers = 0;
  } /* if */

  /*
   * if this device has an SROM, do not change the number of markers to fire
   * the SCU will read the SROM information and set the number to fire automatically
   */
  if( pdtDevice->bHasROM )
  {
    return OPTO_NO_ERROR_CODE;
  } /* if */

  fprintf( stdout, "\n" );

  /*
   * set the number of markers to fire based on the device capability
   */
  dtProperty.uPropertyID = pdtDevice->b3020Capability? DH_PROPERTY_3020_MARKERSTOFIRE : DH_PROPERTY_MARKERSTOFIRE;
  dtProperty.dtPropertyType = DH_PROPERTY_TYPE_INT;
  dtProperty.dtData.nData = nMarkers;

  fprintf( stdout, "...OptotrakDeviceHandleSetProperties (MarkersToActivate) (Device %s)\n", pdtDevice->szName );
  if( OptotrakDeviceHandleSetProperties( nDeviceHandle, &dtProperty, 1 )!= OPTO_NO_ERROR_CODE )
  {
    return OPTO_APP_ERROR_CODE;
  } /* if */

  if( pdtDevice->b3020Capability )
  {
    fprintf( stdout, ".....Set 3020 Markers To Activate = %d\n", dtProperty.dtData.nData );
    pdtDevice->n3020MarkersToActivate = nMarkers;
  }
  else
  {
    fprintf( stdout, ".....Set Markers To Activate = %d\n", dtProperty.dtData.nData );
    pdtDevice->nMarkersToActivate = nMarkers;
  } /* if */

  if( !pdtDevice->b3020Capability && ( nMarkers > 0 ) )
  {
    dtProperty.uPropertyID = DH_PROPERTY_FIRINGSEQUENCE;
    dtProperty.dtPropertyType = DH_PROPERTY_TYPE_STRING;
    for( i = 0; i < DEVICE_MAX_PROPERTY_STRING; i++ )
    {
      if( i < nMarkers )
      {
        dtProperty.dtData.szData[i] = i + 1;
      }
      else
      {
        dtProperty.dtData.szData[i] = 0;
      } /* if */
    } /* for */
    fprintf( stdout, "...OptotrakDeviceHandleSetProperties (FiringSequence) (Device %s)\n", pdtDevice->szName );
    fprintf( stdout, ".....Set Firing Sequence = " );
    for( i = 0; i < nMarkers; i++ )
    {
      fprintf( stdout, "%.3d, ", dtProperty.dtData.szData[i] );
    } /* for */
    fprintf( stdout, "\n" );
    if( OptotrakDeviceHandleSetProperties( nDeviceHandle, &dtProperty, 1 )!= OPTO_NO_ERROR_CODE )
    {
      return OPTO_APP_ERROR_CODE;
    } /* if */
  } /* if */

  return OPTO_NO_ERROR_CODE;

} /* SetMarkersToActivateForDevice */


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
int SetMarkersToActivateForToolDevice( ApplicationDeviceInformation* pdtDevice, int nDeviceHandle, int nMarkers, ApplicationDeviceInformation* pdtDevices, int nDevices )
{
  int
    nCurDevice;
  DeviceHandleProperty
    dtProperty;

  /*
   * if this is not a tool device, do nothing
   */
  if( pdtDevice->nToolPorts == 0 )
  {
    return OPTO_NO_ERROR_CODE;
  } /* if */

  /*
   * iterate through all the devices, looking for tools plugged into
   * the tool device
   */
  for( nCurDevice = 0; nCurDevice < nDevices; nCurDevice++ )
  {
    /*
     * check if this device is a tool of the tool device
     */
    if( ( pdtDevices[nCurDevice].nSubPort != 0 ) &&
      ( pdtDevice->nPort == pdtDevices[nCurDevice].nPort ) &&
      ( pdtDevice->nOrder == pdtDevices[nCurDevice].nOrder ) )

    {
      /*
       * if this is a tool plugged into the tool device,
       * then do nothing with the tool device
       */
      return OPTO_NO_ERROR_CODE;
    } /* if */
  } /* for */

  /*
   * if we get here, then there is no tool plugged into the tool device
   */

  /*
   * set the number of markers to fire based on the device capability
   */
  dtProperty.uPropertyID = DH_PROPERTY_MARKERSTOFIRE;
  dtProperty.dtPropertyType = DH_PROPERTY_TYPE_INT;
  dtProperty.dtData.nData = nMarkers;

  fprintf( stdout, "...OptotrakDeviceHandleSetProperties (MarkersToActivate) (Device %s)\n", pdtDevice->szName );
  if( OptotrakDeviceHandleSetProperties( nDeviceHandle, &dtProperty, 1 )!= OPTO_NO_ERROR_CODE )
  {
    return OPTO_APP_ERROR_CODE;
  } /* if */

  fprintf( stdout, ".....Set Markers To Activate = %d\n", dtProperty.dtData.nData );
  pdtDevice->nMarkersToActivate = nMarkers;

  return OPTO_NO_ERROR_CODE;

} /* SetMarkersToActivateForToolDevice */
