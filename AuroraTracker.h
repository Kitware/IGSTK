/////////////////////////////////////////////////////////////////////////////////
//
// Aurora tracking class
//
// Created by: Sohan R Ranjan, ISIS Center, Georgetown University
//
// Date:	5 Sept 2003
//
/////////////////////////////////////////////////////////////////////////////////
#ifndef __AuroraTracker_h__
#define __AuroraTracker_h__

#include "Tracker.h"
#include <stdlib.h>
#include "SerialCommunication.h"
#include <stdio.h>
#include <time.h>
#include "Conversions.h"
#include "CyclicRedundancy.h"

enum SystemType
{
	POLARIS_SYSTEM = 0x01, AURORA_SYSTEM = 0x02, ACCEDO_SYSTEM = 0x04
};

enum ResponseType
{
	REPLY_ERROR = 0x00, REPLY_OKAY = 0x01, REPLY_RESET = 0x02, REPLY_OTHER = 0x04,
	REPLY_BADCRC= 0x08, REPLY_WARNING = 0x10, REPLY_INVALID	= 0x20
};

#define NO_HANDLES				0xFF

enum TransformStaus
{
	TRANSFORM_VALID	= 0x0000, TRANSFORM_MISSING = 0x1000, TRANSFORM_UNOCCUPIED = 0x2000,
	TRANSFORM_DISABLED = 0x3000, TRANSFORM_ERROR = 0x4000
};

// Handle Status Structure 
typedef struct
{
	int
		bToolInPort,
		bGPIO1,
		bGPIO2,
		bGPIO3,
		bInitialized,
		bEnabled,
		bOutOfVolume,
		bPartiallyOutOfVolume,
		bDisturbanceDet,
		bSignalTooSmall,
		bSignalTooBig,
		bProcessingException,
		bHardwareFailure,
		bTIPCurrentSensing;
}HandleStatus;

// Handle Information Structure 
typedef struct
{
	char
		szPhysicalPort[5],
		szToolType[9],
		szManufact[13],
		szSerialNo[9],
		szRev[4],
		szChannel[3];
	TransformInformation
		Xfrms;
	HandleStatus
		HandleInfo;
}HandleInformation;


typedef struct
{
	int
		nTypeofSystem;
	char
		szVersionInfo[1024];
	int
		bActivePortsAvail,
		bPassivePortsAvail,
		bMultiVolumeParms,
		bTIPSensing,
		bActiveWirelessAvail,
		bMagneticPortsAvail,
		bFieldGeneratorAvail;

	// POLARIS ONLY FIELDS 
	int
		nNoActivePorts,
		nNoPassivePorts,
		nNoActTIPPorts,
		nNoActWirelessPorts;

	// AURORA ONLY FIELDS 
	int
		nNoMagneticPorts,
		nNoFGCards,
		nNoFGs;

	// TRACKING INFORMATION 
	int	bCommunicationSyncError;
	int bTooMuchInterference;
	int bSystemCRCError;
	int bRecoverableException;
	int bHardwareFailure;
	int bHardwareChange;
	int bPortOccupied;
	int bPortUnoccupied;
}SystemInformation;


class AuroraTracker : public Tracker
{
public:
	AuroraTracker( void );

	~AuroraTracker( void );

	int InitializeTracker( const char *parameterFileName = NULL );
	
	// Activate all tools 
	int ActivateTools();	// Make it a private method	

	// Start tracking
	int StartTracking();	
	
	// Reset tracker
	int ResetTracker( void );		 
	
	// Stop tracking
	int StopTracking( void );	

	int GetSystemTransformData( void );

	// THIS ROUTINE NEEDS TO BE CHANGED BECAUSE WE ASSUME THAT ONLY ONE TOOL 
	int GetMyToolHandle( void );

	int GetMyToolHandle( char * toolID );

	int GetToolHandle( char * toolID );

	bool UpdateToolStatus( void );
	
	bool GetToolPosition(const int toolHandle, float position[3]);

	bool GetToolOrientation(const int toolHandle, float quad[4]);

	bool GetOffsetCorrectedToolPosition(const int toolHandle, const float offset[3], float position[3]);

private:

	int SetAuroraCommunicationParameters( int baudRate, int dataBits, 
			int parity, int stopBits, bool hardwareHandshake);
	
	int SetComputerCommunicationParameters(int baudRate, int dataBits,
			int parity, int stopBits, bool hardwareHandshake);

	int BeepTracker( int numberOfBeeps );
	
	int GetSystemInfo( void );

	int InitializeTools( void );

	int InitializeTool( int handle );

	int EnableTools( void );

	int EnableTool( int toolHandle );

	int DisableTool( int toolHandle );

	int LoadTestToolConfiguration( char *toolID );

	int LoadVirtualSROM( char * fileName, char * toolID, bool passive );

	int FreeToolHandles( void );

	int GetToolInformation(int toolHandle);

	int UpdateTextModeTransforms(bool reportOutOfVolume);

	int UpdateBinaryModeTransforms(bool reportOutOfVolume);

	int SendCommand( char * pszCommand, bool addCRC );

	// Fills m_Response with system response (text mode). Return true if successful.
	bool GetTextModeResponse( void );

	// Fills m_Response with system response (binary mode). Return true if successful.
	bool GetBinaryModeResponse( void );

	// Verifies the system response
	bool VerifyResponse( bool textMode, bool checkCRC );

	bool BuildCommand( char * command, bool addCRC );

public:

	TrackerStateType	m_TrackerState;		// Status of the tracker (defined in tracker.h)

	HandleInformation	m_HandleInformation[NO_HANDLES];// Handle Information variable

private:

	CyclicRedundancy	m_CyclicRedundancy;

	SystemInformation	m_SystemInformation;			// System Information 

	SerialCommunication	m_ComPort;			// Serial Communication class 

	char	m_Response[MAX_REPLY_MSG];		// buffer for storing response from the system 
		
	char	m_Command[MAX_COMMAND_MSG];		// buffer for forming command to the system 

	int		m_NumberOfEnabledTools;			// the number of tools enabled by EnableTools 

	unsigned int m_MaximumNumberOfTrials;	// maximum trials for receiving a "response"
		
	int		m_TimeOut;						// timeout value in seconds for getting response from the box 

	bool	m_TrackingModeIsBinary;			// true if tracking mode is binary, false if text

	bool	m_TrackOutOfVolume;				// to track tools if they are gone out of volume 

};

#endif

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
