////////////////////////////////////////////////////////////////////////////
//
// Aurora tracking class
//
// Created by: Sohan R Ranjan, ISIS Center, Georgetown University
//
// Date:	5 Sept 2003
//
////////////////////////////////////////////////////////////////////////////

#include "auroratracker.h"
#include "Conversions.h"

////////////////////////////////////////////////////////////////////////////
/// Constructor
////////////////////////////////////////////////////////////////////////////
AuroraTracker::AuroraTracker()
{
	m_TrackerState = MODE_PRE_INIT;
	m_NumberOfEnabledTools = 0;
	m_TimeOut = 3;
	m_MaximumNumberOfTrials = 3;
	m_TrackingModeIsBinary = false;
	m_TrackOutOfVolume = true;
}

////////////////////////////////////////////////////////////////////////////
/// Destructor
////////////////////////////////////////////////////////////////////////////

AuroraTracker::~AuroraTracker()
{
	if ( m_TrackerState == MODE_TRACKING )
		StopTracking();

	m_ComPort.ClosePort();
}


////////////////////////////////////////////////////////////////////////////
//
/// Method:  StopTracking
//
/// Inputs:	None
// 
/// Returns: ???
///
//
/// Description: This method stops tracking
//	
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::StopTracking()
{
	memset(m_Command, 0, sizeof(m_Command));
	sprintf(m_Command, "TSTOP " );

	if(SendCommand( m_Command, true ))
	{
		if (VerifyResponse(true, true))
		{
			m_TrackerState = MODE_ACTIVATED;
			return 1;
		}
	} 
	return 0;
}


////////////////////////////////////////////////////////////////////////////
//
/// Method:  ResetTracker
//
/// Inputs:	None
// 
/// Returns: ???
//
//
/// Description: This method resets the tracking system.
//	
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::ResetTracker()
{
	if (m_ComPort.ResetPort() != PORT_OK)
	{
		printf("COM Port could not be opened.\nCheck your COM Port settings and\n");
		printf("make sure you system is turned on.\n");
		return 0;
	} 

	m_ComPort.RestPort();	// Rest the port for a while

	Sleep(500); 
	
	memset(m_Command, 0, sizeof(m_Command));
	if (!VerifyResponse(true, true))
		return 0;

//	if ( !this->SetAuroraCommunicationParameters( 0, 0, 0, 0, 0))
//			return 0;

	m_TrackerState = MODE_PRE_INIT;

	printf("System reset successful.\n");

	return 1;
}

////////////////////////////////////////////////////////////////////////////
//
/// Method:  InitializeTracker
//
/// Inputs:	None
// 
/// Returns: ???
//
//
/// Description: This method resets the tracking system.
//	
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::InitializeTracker( const char *parameterFileName )
{
	// *** ADD CHECKING for a valid FILENAME HERE *******

	Tracker::InitializeTracker( parameterFileName );
	
	m_TrackerState = MODE_PRE_INIT;

	int		baudRate = 0, stopBits = 0, parity = 0, dataBits = 0, comPortNum = 1;
	bool	hardwareHandshake = false;

//	COME BACK LATER: There seems to be some goof-up in setting communication parameters.
//	m_Parameters.ReadParameter("Communication", "Baud Rate", "0", sizeof(baudRate), &baudRate );
//	m_Parameters.ReadParameter("Communication", "Stop Bits", "0", sizeof(stopBits), &stopBits );
//	m_Parameters.ReadParameter("Communication", "Parity",    "0", sizeof(parity), &parity );
//	m_Parameters.ReadParameter("Communication", "Data Bits", "0", sizeof(dataBits), &dataBits );
//	m_Parameters.ReadParameter("Communication", "Hardware",  "0", sizeof(hardwareHandshake), &hardwareHandshake );
//	m_Parameters.ReadParameter("Communication", "COM Port",  "0", sizeof(comPortNum), &comPortNum );


	m_ComPort.ClosePort(); 

	if (m_ComPort.OpenPort( comPortNum ) != PORT_OK)
	{
		printf("COM Port could not be opened. Check your COM Port settings and\n");
		printf("Make sure you system is turned on.\n");
		return 0;
		
	} 

	if (!this->ResetTracker())
	{
		return 0;
	}


	// Set the System COM Port parameters, then the computers COM Port parameters.
	// If successful, initialize the system
	 
	if(this->SetAuroraCommunicationParameters( baudRate, dataBits, parity, stopBits, hardwareHandshake ))
	{
		if(this->SetComputerCommunicationParameters( baudRate, dataBits, parity, stopBits, hardwareHandshake ))
		{
			// clear the handle information 
			for ( int i = 0; i < NO_HANDLES; i++ )
			{
				memset(m_HandleInformation[i].szPhysicalPort, 0, 5);
				m_HandleInformation[i].HandleInfo.bInitialized = false;
				m_HandleInformation[i].HandleInfo.bEnabled = false;
			} 
			
			// send the message 
			memset(m_Command, 0, sizeof(m_Command));
			sprintf( m_Command, "INIT " );
			
			if(SendCommand( m_Command, true ) && VerifyResponse(true, true))
			{
				// get the system information
			
				if (!this->GetSystemInfo())
				{
					// Check system type: Polaris, Polaris Accedo, and Aurora
					printf( "Could not determine type of system (POLARIS, POLARIS ACCEDO or AURORA)\n");
					return 0;
				} 

				m_TrackerState = MODE_INIT;

				printf( "System successfully initialized\n");
				return 1;
			} 
			else
			{
				printf("System could not be initialized.\n");
			}
		}
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::StartTracking()
{
	memset(m_Command, 0, sizeof(m_Command));
	sprintf( m_Command, "TSTART " );

	if(SendCommand( m_Command, true ))
	{
		if (VerifyResponse(true, true))
		{
			m_TrackerState = MODE_TRACKING;
			return 1;
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::GetSystemTransformData( void )
{
	if (m_TrackerState != MODE_TRACKING)
		return 0;

	 // if tracking mode is 0, we are asking for TX data, else we are
	 // asking for BX data.
	 
	if (m_TrackingModeIsBinary )
	{
		if ( !this->UpdateBinaryModeTransforms( m_TrackOutOfVolume ) )
			return 0;
	} 
	else 
	{
		if ( !this->UpdateTextModeTransforms( m_TrackOutOfVolume ) ) 
			return 0;
	} 

	 // if the port has become occupied, activate ports again and restart tracking
	 
	if ( this->m_SystemInformation.bPortOccupied )
	{
		if ( this->StopTracking() && ActivateTools() && this->StartTracking() )
		{
			return 1;
		}

		m_TrackerState = MODE_ACTIVATED;
		return 0;
	} 

	for ( int i = 0; i < NO_HANDLES; i ++ )
	{
		if ( this->m_HandleInformation[i].HandleInfo.bInitialized > 0 &&
			 this->m_HandleInformation[i].szToolType[1] != '8' )
		{			
			if ( this->m_HandleInformation[i].Xfrms.ulFlags == TRANSFORM_VALID )
			{
				printf("X = %.2f, Y = %.2f, Z = %.2f, q0 = %.4f, q1 = %.4f, q2 = %.4f, q3 = %.4f ", 
					this->m_HandleInformation[i].Xfrms.translation.x,
					this->m_HandleInformation[i].Xfrms.translation.y,
					this->m_HandleInformation[i].Xfrms.translation.z,
					this->m_HandleInformation[i].Xfrms.rotation.q0, 
					this->m_HandleInformation[i].Xfrms.rotation.qx, 
					this->m_HandleInformation[i].Xfrms.rotation.qy, 
					this->m_HandleInformation[i].Xfrms.rotation.qz );

				if ( this->m_HandleInformation[i].HandleInfo.bPartiallyOutOfVolume )
					printf("POOV\n" );
				else if ( this->m_HandleInformation[i].HandleInfo.bOutOfVolume )
					printf("OOV\n");
				else
					printf("OK\n");
			}
			else 
			{
				if ( this->m_HandleInformation[i].Xfrms.ulFlags == TRANSFORM_MISSING )
					printf("MISSING\n" );
				else
				{
					printf("DISABLED\n" );
				}

				if ( this->m_HandleInformation[i].HandleInfo.bPartiallyOutOfVolume )
					printf("POOV\n" );
				else if ( this->m_HandleInformation[i].HandleInfo.bOutOfVolume )
					printf("OOV\n");
				else
					printf("---\n" );
			}				
		}
	}

	return 1;
} 
////////////////////////////////////////////////////////////////////////////
bool AuroraTracker::UpdateToolStatus( void )
{
	if (m_TrackerState != MODE_TRACKING)
		return false;

	 // if tracking mode is 0, we are asking for TX data, else we are
	 // asking for BX data.
	 
	if (m_TrackingModeIsBinary )
	{
		if ( !this->UpdateBinaryModeTransforms( m_TrackOutOfVolume ) )
			return true;
	} 
	else 
	{
		if ( !this->UpdateTextModeTransforms( m_TrackOutOfVolume ) ) 
			return true;
	} 

	 // if the port has become occupied, activate ports again and restart tracking
	 
	if ( this->m_SystemInformation.bPortOccupied )
	{
		if ( this->StopTracking() && ActivateTools() && this->StartTracking() )
		{
			return true;
		}

		m_TrackerState = MODE_ACTIVATED;
		return false;
	} 
	return false;
}
////////////////////////////////////////////////////////////////////////////
//
// Method:	SetAuroraCommunicationParameters
//
// Inputs: baudRate, dateBits, parity, stopBits, bool hardwareHandshake
//
// Return Value:
//
// Description: This method sets the Aurora System Communication Parameters
//
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::SetAuroraCommunicationParameters( int baudRate, int dataBits, 
		int parity, int stopBits, bool hardwareHandshake)
{
	memset(m_Command, 0, sizeof(m_Command));
	sprintf( m_Command, "COMM %d%d%d%d%d", baudRate, dataBits, parity, stopBits, 
				hardwareHandshake);

	printf("SetAuroraCommunicationParameters: %s\n", m_Command);

	if (SendCommand( m_Command, true ))
		return VerifyResponse(true, true);

	return 0;
} 


////////////////////////////////////////////////////////////////////////////
int AuroraTracker::SetComputerCommunicationParameters( int baudRate, int dataBits,
		int parity, int stopBits, bool hardwareHandshake)
{
	switch( baudRate )
	{
		case 0:
			baudRate = 9600;
			break;
		case 1:
			baudRate = 14400;
			break;
		case 2:
			baudRate = 19200;
			break;
		case 3:
			baudRate = 38400;
			break;
		case 4:
			baudRate = 57600;
			break;
		case 5:
			baudRate = 115200;
			break;
	} 
	
	switch(dataBits)
	{
	case 1: 
		dataBits = 7;
		break;
	case 0: 
	default:
		dataBits = 8;
		break;
	}

	switch(parity)
	{
	case 1: // odd
		parity = ODDPARITY;
		break;
	case 2: // even
		parity = EVENPARITY;
		break;
	case 0: // none
	default:
		parity = NOPARITY;
		break;
	}

	switch(stopBits)
	{
	case 1: // 2 bits
		stopBits = TWOSTOPBITS;
		break;
	case 0: // 1 bit
	default:
		stopBits = ONESTOPBIT;
		break;
	}

	printf("SetComputerCommunicationParameters: dataBits = %.2d, parity = %.2d, stopBits = %.2d, hardwareHandshake = %.2d);\n", dataBits, parity, stopBits, hardwareHandshake);

	m_ComPort.SetCommunicationFormat(dataBits, parity, stopBits, hardwareHandshake);

	if ( m_ComPort.SetBaudRate( baudRate )) 
		return 1;

	return 0;
} 

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::BeepTracker( int nBeeps )
{
	memset( m_Command, 0, sizeof(m_Command));
	sprintf( m_Command, "BEEP %d", nBeeps );

	if(SendCommand( m_Command, true ))
	{
		return VerifyResponse(true, true);
	} 
	return 0;
} 
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//
// Method:				GetSystemInfo
//
// Inputs: None.
//
// Return: ???
//
// Description: This methodsgets the Aurora information through the VER and 
// SFLIST commands. 
//
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::GetSystemInfo()
{

	// version Information 
	memset(m_Command, 0, sizeof(m_Command));
	sprintf( m_Command, "VER 4" );

	if(SendCommand( m_Command, true ))
	{
		if ( !VerifyResponse(true, true))
		{
			return 0;
		}
		
		if ( !strncmp( m_Response, "POLARIS", 7 )||
			 !strncmp( m_Response, "polaris", 7 )||
			 !strncmp( m_Response, "Polaris", 7 ) )
		{
			if ( strstr( m_Response, "ACCEDO" ) )
			{
				m_SystemInformation.nTypeofSystem = ACCEDO_SYSTEM;
			}
			else
			{
				m_SystemInformation.nTypeofSystem = POLARIS_SYSTEM;
			}
		}
		else if ( !strncmp( m_Response, "AURORA", 6 )||
			 !strncmp( m_Response, "aurora", 6 )||
			 !strncmp( m_Response, "Aurora", 6 ) )
		{
			m_SystemInformation.nTypeofSystem = AURORA_SYSTEM;
		}
		else
		{
			return 0;
		}

		sprintf( m_SystemInformation.szVersionInfo, m_Response );
		m_SystemInformation.szVersionInfo[strlen(m_SystemInformation.szVersionInfo) - 5] = 0;
	} 
	// For POLARIS abd ACCEDO systems
	if ( (m_SystemInformation.nTypeofSystem == POLARIS_SYSTEM) ||
		 (m_SystemInformation.nTypeofSystem == ACCEDO_SYSTEM) )
	{
		memset(m_Command, 0, sizeof(m_Command));
		sprintf( m_Command, "SFLIST 00" );
		if(SendCommand( m_Command, true ))
		{
			if (!VerifyResponse(true, true))
				return 0;
			// supported features summary
			int hexResponse = uASCIIToHex( m_Response, 8 );
			m_SystemInformation.bActivePortsAvail = (0x01 & hexResponse ? 1 : 0);
			m_SystemInformation.bPassivePortsAvail = (0x02 & hexResponse ? 1 : 0);
			m_SystemInformation.bMultiVolumeParms = (0x04 & hexResponse ? 1 : 0);
			m_SystemInformation.bTIPSensing = (0x08 & hexResponse ? 1 : 0);
			m_SystemInformation.bActiveWirelessAvail = (0x10 & hexResponse ? 1 : 0);
			m_SystemInformation.bMagneticPortsAvail = (0x8000 & hexResponse ? 1 : 0);
			m_SystemInformation.bFieldGeneratorAvail = (0x40000 & hexResponse ? 1 : 0);
		} 

		// Active ports
		sprintf( m_Command, "SFLIST 01" );
		if(SendCommand( m_Command, true ))
		{
			if (!VerifyResponse(true, true ))
				return 0;
			m_SystemInformation.nNoActivePorts = uASCIIToHex( &m_Response[0], 1 );
		} 
		// Passive ports
		memset(m_Command, 0, sizeof(m_Command));
		sprintf( m_Command, "SFLIST 02" );
		if(SendCommand( m_Command, true ))
		{
			if (!VerifyResponse(true, true ))
				return 0;
			m_SystemInformation.nNoPassivePorts = uASCIIToHex( &m_Response[0], 1 );
		} 
		// Active tool ports supporting TIP detection
		memset(m_Command, 0, sizeof(m_Command));
		sprintf( m_Command, "SFLIST 04" );
		if(SendCommand( m_Command, true ))
		{
			if (!VerifyResponse(true, true ))
				return 0;
			m_SystemInformation.nNoActTIPPorts = uASCIIToHex( &m_Response[0], 1 );
		} 
		// Active wireless ports
		memset(m_Command, 0, sizeof(m_Command));
		sprintf( m_Command, "SFLIST 05" );
		if(SendCommand( m_Command, true ))
		{
			if (!VerifyResponse(true, true ))
				return 0;
			m_SystemInformation.nNoActWirelessPorts = uASCIIToHex( &m_Response[0], 1 );
		} 
	} 
	// for AURORA system
	else
	{
		// Magnetic Ports
		memset(m_Command, 0, sizeof(m_Command));
		sprintf( m_Command, "SFLIST 10" );
		if(SendCommand( m_Command, true ))
		{
			if (!VerifyResponse(true, true ))
				return 0;
			m_SystemInformation.nNoMagneticPorts = uASCIIToHex( &m_Response[0], 2 );
		} 
		// Field Generator
		memset(m_Command, 0, sizeof(m_Command));
		sprintf( m_Command, "SFLIST 12" );
		if(SendCommand( m_Command, true ))
		{
			if (!VerifyResponse(true, true ))
				return 0;
			m_SystemInformation.nNoFGCards = uASCIIToHex( &m_Response[0], 1 );
			m_SystemInformation.nNoFGs = uASCIIToHex( &m_Response[1], 1 );
		} 

		// Field Generator Version 
		memset(m_Command, 0, sizeof(m_Command));
		sprintf( m_Command, "VER 7" );

		if(SendCommand( m_Command, true ))
		{
			if ( !VerifyResponse(true, true))
			{
				return 0;
			}
			
			strcat( m_SystemInformation.szVersionInfo, "\n" );
			strcat( m_SystemInformation.szVersionInfo, m_Response );
			m_SystemInformation.szVersionInfo[strlen(m_SystemInformation.szVersionInfo) - 5] = 0;
		} 

		// SIU Version Information
		memset(m_Command, 0, sizeof(m_Command));
		sprintf( m_Command, "VER 8" );

		if(SendCommand( m_Command, true ))
		{
			if ( !VerifyResponse(true, true))
			{
				return 0;
			}
			
			strcat( m_SystemInformation.szVersionInfo, "\n" );
			strcat( m_SystemInformation.szVersionInfo, m_Response );
			m_SystemInformation.szVersionInfo[strlen(m_SystemInformation.szVersionInfo) - 5] = 0;
		} 
	} 
	return 1;
} 
////////////////////////////////////////////////////////////////////////////
// Method:	InitializeTools
//
// Inputs: None.
//
// Return Value:	int - 1 is successful, 0 otherwise
//
// Description: This method intializes all the tools .
//
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::InitializeTools()
{
	char	toolID[8], ROMFileName[MAX_PATH], handleListString[MAX_REPLY_MSG];

	// passive ports
	for ( int i = 0; i < m_SystemInformation.nNoPassivePorts; i++ )
	{
		// load the ROM if one is specified 
		sprintf(toolID, "Port %c", (i + 65) );
//		if ( *ROMFileName )
//		{
//			sprintf(toolID, "%02c", (i + 65 ) );
//			LoadVirtualSROM( ROMFileName,toolID, true );
//		} 
	} 

	// other ports

	int numberOfHandles = 0, n = 0;
	do
	{
		n = 0;
		// get the handles that need to be initialized 
		memset(m_Command, 0, sizeof(m_Command));
		sprintf( m_Command, "PHSR 02" );

		if (!SendCommand( m_Command, true ))
			return 0;

		if (!VerifyResponse(true, true ))
			return 0;

		sprintf( handleListString, m_Response );
		numberOfHandles = uASCIIToHex( &m_Response[n], 2 );
		n+=2;

		int handleNumber;

		if ( numberOfHandles > 0 )
		{
			// active
			int numberOfTools = (m_SystemInformation.nNoActivePorts > 0 ?
							  m_SystemInformation.nNoActivePorts :
							  m_SystemInformation.nNoMagneticPorts > 0 ?
							  m_SystemInformation.nNoMagneticPorts : 0);

			if ( m_SystemInformation.nNoActivePorts == 4 )
				numberOfTools = 12;

			char 	INISectionName[256];
			sprintf( INISectionName, m_SystemInformation.nNoActivePorts > 0 ?
									"POLARIS SROM Image Files\0" :
									m_SystemInformation.nNoMagneticPorts > 0 ?
									"AURORA SROM Image Files\0" : "" );

			for ( i = 0; i < numberOfTools; i++ )
			{
				sprintf( toolID, "Port %d", i+1 );
				m_Parameters.ReadParameter( INISectionName, toolID, "", MAX_PATH, ROMFileName );
				if ( *ROMFileName )
				{
					sprintf(toolID, "%02d", i+1 );
					if (!strncmp(ROMFileName, "TTCFG", 5 ))
						LoadTestToolConfiguration(toolID );
					else
						LoadVirtualSROM( ROMFileName,toolID, false );
					handleNumber = GetToolHandle(toolID );
					if ( handleNumber != 0 )
					{
						if (!InitializeTool( handleNumber ))
						{
							// Inform user which port fails on PINIT */
							printf("Physical Port %s could not be initialized.\nCheck your SROM image file settings.",toolID );
							return 0;
						}
					} 
				} 
			} 

			for ( int i = 0; i < numberOfHandles; i++ )
			{
				handleNumber = uASCIIToHex( &handleListString[n], 2 );
				if ( !GetToolInformation( handleNumber ) )
					return 0;

				if ( !m_HandleInformation[handleNumber].HandleInfo.bInitialized )
				{
					if (!InitializeTool( handleNumber ))
					{
						// Inform user which port fails on PINIT */
						printf("Port %s could not be initialized.\nCheck your SROM image file settings.", m_HandleInformation[handleNumber].szPhysicalPort );
						return 0;
					}
					n+=5;
				} 
			} 
		} 
		// do this until there are no new handles 
	}while( numberOfHandles > 0 );

	return 1;
} 
////////////////////////////////////////////////////////////////////////////
// Method: InitializeTool
//
// Inputs: int nHandle - the handle to be intialized
//
// Return Value: int - 1 if successful, otherwise 0
//
// Description:   This routine initializes the specified tool handle.
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::InitializeTool( int toolHandle )
{
	memset(m_Command, 0, sizeof(m_Command));
	sprintf( m_Command, "PINIT %02X", toolHandle );

	if ( !SendCommand( m_Command, true ))
		return 0;

	if (!VerifyResponse(true, true ))
	{
		return 0;
	} 
	m_HandleInformation[toolHandle].HandleInfo.bInitialized = true;

	return 1;
} 
////////////////////////////////////////////////////////////////////////////
// Method:	EnableTools
//
// Inputs: None.
//
// Return Value: int - 1 if successful, 0 otherwise
//
// Description: This method enables all the tool handles
//
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::EnableTools()
{
	char	handleListString[MAX_REPLY_MSG];

	m_NumberOfEnabledTools = 0;
	// get all the ports that need to be enabled 
	memset(m_Command, 0, sizeof(m_Command));
	sprintf( m_Command, "PHSR 03" );

	int	toolHandle = 0, n = 0;
	
	if(SendCommand( m_Command, true ))
	{
		if (!VerifyResponse(true, true ))
			return 0;

		sprintf( handleListString, m_Response );
		int numberOfHandles = uASCIIToHex( &handleListString[n], 2 );
		n+=2;

		for ( int i = 0; i < numberOfHandles; i++ )
		{
			toolHandle = uASCIIToHex( &handleListString[n], 2 );
			memset(m_Command, 0, sizeof(m_Command));
			sprintf( m_Command, "PENA %02X%c", toolHandle, 'D' );
			n+=5;
			if (!SendCommand( m_Command, true ))
				return 0;
			if (!VerifyResponse(true, true ))
				return 0;
			GetToolInformation( toolHandle );
			m_NumberOfEnabledTools++;
		} 
		return 1;
	} 
	return 0;
} 
////////////////////////////////////////////////////////////////////////////
// Method:	EnableTool
//
// Inputs: int toolHandle - handle to be enabled
//
// Return Value: int - 1 if successful, 0 otherwise
//
// Description: This method enables the tool handle specified.
//
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::EnableTool( int toolHandle )
{
	memset(m_Command, 0, sizeof(m_Command));
	sprintf( m_Command, "PENA %02X%c", toolHandle, 'D' );
	if (!SendCommand( m_Command, true ))
		return 0;
	if (!VerifyResponse(true, true ))
		return 0;
	GetToolInformation( toolHandle );
	return 1;
} 
////////////////////////////////////////////////////////////////////////////
// Method:	DisableTool
//
// Inputs: int toolHandle - handle to be disabled
//
// Return Value: int - 1 if successful, 0 otherwise
// 
// Description: This method disables the tool handle specified.
//				
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::DisableTool( int toolHandle )
{
	memset(m_Command, 0, sizeof(m_Command));
	sprintf( m_Command, "PDIS %02X", toolHandle );

	if(SendCommand( m_Command, true ))
	{
		if (VerifyResponse(true, true) )
		{
			GetToolInformation(toolHandle);
			return 1;
		} 
	} 
	return 0;
} 
////////////////////////////////////////////////////////////////////////////
// Method:	ActivateTools
//
// Inputs: None.
//
// Return Value: int - 1 if successful, 0 otherwise
//
// Description:This method activates all ports.
				
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::ActivateTools()
{
	if (FreeToolHandles() && InitializeTools() && EnableTools())
	{
		m_TrackerState = MODE_ACTIVATED;
		printf( "Tools successfully activated.\n" );
		return 1;
	} 
	else
	{
		m_TrackerState = MODE_PRE_INIT;
		printf("Tools could not be activated.\nCheck your SROM image file settings.\n");
		return 0;
	}
} 
////////////////////////////////////////////////////////////////////////////
// Method:	LoadVirtualSROM
//
// Inputs: char * fileName - the file to be loaded
//	char * toolID - the tool id that is being loaded to.
//	bool passive - is this a passive port or not
//
// Return Value: int - 1 if successful, 0 otherwise.
//
// Description: This method virtual loads a SROM file to the specified port.
//	
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::LoadVirtualSROM( char * fileName, char * toolID, bool passive )
{
	static unsigned char charBuffer[ 1024 ];

	if ( !*fileName )
		return 0;

	int toolHandle;
	// get the tool handle
	if ( passive )
	{
		for ( int i = 0; i < NO_HANDLES; i++ )
		{
			if ( !strncmp( m_HandleInformation[i].szPhysicalPort, toolID, 2 ) )
				return 0;
		}
		// if passive we need a tool handle 
		memset(m_Command, 0, sizeof(m_Command));
		sprintf( m_Command, "PHRQ ********01%s**", toolID ); 
		if (!SendCommand( m_Command, true ))
			return 0;

		if (!VerifyResponse(true, true))
			return 0;
		toolHandle = uASCIIToHex(&m_Response[0], 2);
		if ( m_HandleInformation[toolHandle].HandleInfo.bInitialized == 1 )
			return 0;
	}
	else
	{
		// if active a handle has already been assigned 
		toolHandle = GetToolHandle( toolID );
		if ( toolHandle == 0 || m_HandleInformation[toolHandle].HandleInfo.bInitialized == 1 )
			return 0;
	}

	FILE	*fileHandle = NULL;
    if( !(fileHandle = fopen( fileName, "rb" )) )
    {
		return 0;
    } 

	int numberOfBytesRead;
    if( (numberOfBytesRead = fread( charBuffer, 1, sizeof(charBuffer), fileHandle )) < 1 )
    {
		printf("Unable to read ROM image file %s.", fileName );
		fclose( fileHandle );
        return 0;
    } 

    for( int count = 0; count < numberOfBytesRead; )
    {
		 //write the data to the tool description section of 
		 // the virtual SROM on a per port basis
		 
		memset(m_Command, 0, sizeof(m_Command));
        sprintf( m_Command, "PVWR:%02X%04X", toolHandle, count );

        for( int i = 0; i < 64; i++, count++ )
        {
			// (plus eleven for the PVWR:XX0000 ) 
            sprintf( m_Command + 11 + 2 * i, "%02X", charBuffer[count] ); 
        } 

        if ((!SendCommand( m_Command, true)) || (!VerifyResponse(true, true)))
        {
 			printf("Failed writing to virtual port %s.\n", toolID );
			fclose(fileHandle);
			return 0;
		} 
    } 

	fclose( fileHandle );
	return 1;
} 
////////////////////////////////////////////////////////////////////////////
// Method:	LoadTestToolConfiguration	
//
// Inputs: char *toolID - the physical port to be loaded with test tool config.
//
// Return Value: int - 1 if successful, 0 otherwise
//
// Description: This method loads the Test Tool ConFiGuration to the specified
//
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::LoadTestToolConfiguration( char *toolID )
{
	int	toolHandle = 0;

	// get the handle for the port 
	toolHandle = GetToolHandle(toolID );
	if ( toolHandle == 0 )
		return 0;

	memset(m_Command, 0, sizeof(m_Command));
	sprintf( m_Command, "TTCFG %02X", toolHandle );
	if ( SendCommand( m_Command, true ) )
	{
		return( VerifyResponse(true, true));
	} 
	return 0;	
} 
////////////////////////////////////////////////////////////////////////////
/// THIS METHOD NEEDS TO BE CLEARED.
int AuroraTracker::GetMyToolHandle( void )
{
	int toolHandle = -1;
	for ( int i = 0; i < NO_HANDLES; i ++ )
	{
		if ( this->m_HandleInformation[i].HandleInfo.bInitialized > 0 &&
			 this->m_HandleInformation[i].szToolType[1] != '8' )
		{
			toolHandle = i;
			break;
		}
	}
	return toolHandle;
}
int AuroraTracker::GetMyToolHandle( char *toolID )
{
	int toolHandle = -1;
	for ( int i = 0; i < NO_HANDLES; i ++ )
	{
		if ( this->m_HandleInformation[i].HandleInfo.bInitialized > 0 &&
			 this->m_HandleInformation[i].szToolType[1] != '8' &&
			 !strncmp(m_HandleInformation[i].szPhysicalPort,toolID, 2) )
		{
			toolHandle = i;
			break;
		}
	}
	return toolHandle;
}
////////////////////////////////////////////////////////////////////////////
// Method:	GetToolHandle
//
// Inputs: char * toolID : the tool whose handle is required
//
// Return Value: int - 0 if fails, tool handle if passes
// 
// Description: This method takes a tool location and gets its handle
//  
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::GetToolHandle( char *toolID )
{
	char	handleListString[MAX_REPLY_MSG];

	memset(m_Command, 0, sizeof(m_Command));
	sprintf( m_Command, "PHSR 00" );

	if ( SendCommand( m_Command, true ) )
	{
		if (!VerifyResponse(true, true ))
			return 0;
	} 

	int  handleIndex = 0;
	sprintf(handleListString, m_Response);

	int numberOfHandles = uASCIIToHex( &handleListString[handleIndex], 2 );
	handleIndex += 2;

	int   toolHandle;

	for ( int i = 0; i < numberOfHandles; i++ )
	{
		// for all the handles, get their physical location with PHINF 
		toolHandle = uASCIIToHex(&handleListString[handleIndex], 2);
		handleIndex += 5;
		GetToolInformation(toolHandle);
		// if the physical location matchtoolID, return the handle 
		if ( !strncmp(m_HandleInformation[toolHandle].szPhysicalPort,toolID, 2) )
			return toolHandle;
	} 
	return 0;
} 
////////////////////////////////////////////////////////////////////////////
// Method: FreeToolHandles
//
// Inputs:	None.
//
// Return Value: int - 0 if fails, 1 if passes
//
// Description: This method frees all port handles that need to be freed 
//
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::FreeToolHandles()
{
	char	handleListString[MAX_REPLY_MSG];

	// get all the handles that need freeing 
	memset(m_Command, 0, sizeof(m_Command));
	sprintf( m_Command, "PHSR 01" );

	if(SendCommand( m_Command, true ))
	{
		if (!VerifyResponse(true, true ))
			return 0;

		sprintf(handleListString, m_Response);

		int handleIndex = 0;
		int numberOfHandles = uASCIIToHex(&handleListString[handleIndex], 2);
		handleIndex += 2;

		int  handle;

		for ( int i = 0; i < numberOfHandles; i++ )
		{
			handle = uASCIIToHex( &handleListString[handleIndex], 2 );
			memset(m_Command, 0, sizeof(m_Command));
			sprintf( m_Command, "PHF %02X", handle);
			handleIndex += 5;
			if (!SendCommand( m_Command, true ))
				return 0;
			if (!VerifyResponse(true, true ))
				return 0;
			m_HandleInformation[handle].HandleInfo.bInitialized = false;
			m_HandleInformation[handle].HandleInfo.bEnabled = false;
			memset(m_HandleInformation[handle].szPhysicalPort, 0, 5);
		} 
		return 1;
	} 
	return 0;
} 
////////////////////////////////////////////////////////////////////////////
// Method:	GetToolInformation
//
// Inputs: int portHandle - the handle to get information for
//
// Return Value:	int - 1 if successful, 0 otherwise
//
// Description: This method gets the port handling information using PHINF.
//
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::GetToolInformation(int toolHandle)
{
	unsigned int	uASCIIConv = 0; 
	char			*toolInformation = NULL;
	
	memset(m_Command, 0, sizeof(m_Command));
	sprintf( m_Command, "PHINF %02X0021", toolHandle );
	
	if ( SendCommand( m_Command, true ) )
	{
		if ( !VerifyResponse(true, true ))
			return 0;
		
		toolInformation = m_Response;
		
		strncpy( m_HandleInformation[toolHandle].szToolType, toolInformation, 8 );
		m_HandleInformation[toolHandle].szToolType[8] = '\0';
		toolInformation += 8;

		strncpy( m_HandleInformation[toolHandle].szManufact, toolInformation, 12 );
		m_HandleInformation[toolHandle].szManufact[12] = '\0';
		toolInformation += 12;

		strncpy( m_HandleInformation[toolHandle].szRev, toolInformation, 3 );
		m_HandleInformation[toolHandle].szRev[3] = '\0';
		toolInformation += 3;

		strncpy( m_HandleInformation[toolHandle].szSerialNo, toolInformation, 8 );
		m_HandleInformation[toolHandle].szSerialNo[8] = '\0';
		toolInformation += 8;

		uASCIIConv = uASCIIToHex( toolInformation, 2 );
		toolInformation += 2;

		m_HandleInformation[toolHandle].HandleInfo.bToolInPort = ( uASCIIConv & 0x01 ? 1 : 0 );
		m_HandleInformation[toolHandle].HandleInfo.bGPIO1 = ( uASCIIConv & 0x02 ? 1 : 0 );
		m_HandleInformation[toolHandle].HandleInfo.bGPIO2 = ( uASCIIConv & 0x04 ? 1 : 0 );
		m_HandleInformation[toolHandle].HandleInfo.bGPIO3 = ( uASCIIConv & 0x08 ? 1 : 0 );
		m_HandleInformation[toolHandle].HandleInfo.bInitialized = ( uASCIIConv & 0x10 ? 1 : 0 );
		m_HandleInformation[toolHandle].HandleInfo.bEnabled = ( uASCIIConv & 0x20 ? 1 : 0 );
		m_HandleInformation[toolHandle].HandleInfo.bTIPCurrentSensing = ( uASCIIConv & 0x80 ? 1 : 0 );
		
		toolInformation += 10;

		strncpy( m_HandleInformation[toolHandle].szPhysicalPort, toolInformation, 2 );
		toolInformation += 2;

		strncpy( m_HandleInformation[toolHandle].szChannel, toolInformation, 2 );
		m_HandleInformation[toolHandle].szChannel[2] = '\0';
		if ( !strncmp( m_HandleInformation[toolHandle].szChannel, "01", 2 ) )
		{
			strncpy(&m_HandleInformation[toolHandle].szPhysicalPort[2], "-b", 2 );
			for ( int i = 0; i < NO_HANDLES; i++ )
			{
				if ( strncmp( m_HandleInformation[i].szPhysicalPort, m_HandleInformation[toolHandle].szPhysicalPort, 4 ) &&
					!strncmp( m_HandleInformation[i].szPhysicalPort, m_HandleInformation[toolHandle].szPhysicalPort, 2 ) )
					strncpy(&m_HandleInformation[i].szPhysicalPort[2], "-a", 2 );
			} 
		} 
	} 
	return 1;
} 
////////////////////////////////////////////////////////////////////////////
// Method:	UpdateTextModeTransforms
//
// Inputs:
//		bool checkOutOfVolume: whether to check if tools have gone out of volume.
//
// Return Value:
//		int - 1 if successful, 0 otherwise.
//
// Description: This method updates the transformation information using the TX
//	command.  
//
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::UpdateTextModeTransforms(bool checkOutOfVolume)
{
	int		numberOfHandles = 0, handle = 0;
	bool	disabled = false;
	char	*transformInformation = NULL;

	// report in volume only or out of volume as well 
	int replyMode = checkOutOfVolume ? 0x0801 : 0x0001;

	memset(m_Command, 0, sizeof(m_Command));
	sprintf(m_Command, "TX %04X", replyMode );

	if(SendCommand( m_Command, true ))
	{
		if (!VerifyResponse( true, false ))
				return 0;

		// Text Mode Parsing Routines
		transformInformation = m_Response;

		numberOfHandles = uASCIIToHex( transformInformation, 2 );
		transformInformation += 2;

		for ( int i = 0; i < numberOfHandles; i++ )
		{
			handle = uASCIIToHex( transformInformation, 2 );
			transformInformation += 2;

			disabled = false;

			if ( strlen( transformInformation ) < 18 )
				return 0;

			if( !strncmp( transformInformation, "MISSING", 7 ) ||
				!strncmp( transformInformation, "DISABLED", 8 ) || 
				!strncmp( transformInformation, "UNOCCUPIED", 10 ))
			{
				if ( !strncmp( transformInformation, "UNOCCUPIED", 10 ))
				{
					
					m_HandleInformation[handle].Xfrms.ulFlags = TRANSFORM_UNOCCUPIED;
					transformInformation += 10;	// length of "UNOCCUPIED"				
					disabled = true;
				} 
				else if ( !strncmp( transformInformation, "DISABLED", 8 ))
				{
					m_HandleInformation[handle].Xfrms.ulFlags = TRANSFORM_DISABLED;
					transformInformation += 8;
					disabled = true;
				} 
				else
				{
					m_HandleInformation[handle].Xfrms.ulFlags = TRANSFORM_MISSING;
					transformInformation += 7;
				} 
				m_HandleInformation[handle].Xfrms.rotation.q0 =
				m_HandleInformation[handle].Xfrms.rotation.qx =
				m_HandleInformation[handle].Xfrms.rotation.qy =
				m_HandleInformation[handle].Xfrms.rotation.qz =
				m_HandleInformation[handle].Xfrms.translation.x =
				m_HandleInformation[handle].Xfrms.translation.y =
				m_HandleInformation[handle].Xfrms.translation.z =
				m_HandleInformation[handle].Xfrms.fError = BAD_FLOAT;
			} 
			else
			{
				m_HandleInformation[handle].Xfrms.ulFlags = TRANSFORM_VALID;

				if (!bExtractValue( transformInformation, 6, 10000., 
									&m_HandleInformation[handle].Xfrms.rotation.q0 ) ||
					!bExtractValue( transformInformation + 6, 6, 10000., 
									&m_HandleInformation[handle].Xfrms.rotation.qx ) ||
					!bExtractValue( transformInformation + 12, 6, 10000., 
									&m_HandleInformation[handle].Xfrms.rotation.qy ) ||
					!bExtractValue( transformInformation + 18, 6, 10000., 
									&m_HandleInformation[handle].Xfrms.rotation.qz ) ||
					!bExtractValue( transformInformation + 24, 7, 100., 
									&m_HandleInformation[handle].Xfrms.translation.x ) ||
					!bExtractValue( transformInformation + 31, 7, 100., 
									&m_HandleInformation[handle].Xfrms.translation.y ) ||
					!bExtractValue( transformInformation + 38, 7, 100., 
									&m_HandleInformation[handle].Xfrms.translation.z ) ||
					!bExtractValue( transformInformation + 45, 6, 10000., 
									&m_HandleInformation[handle].Xfrms.fError ) )
				{
					m_HandleInformation[handle].Xfrms.ulFlags = TRANSFORM_MISSING;
					return 0;
				} 
				else
				{
					transformInformation += 51;
				} 
			} 
			
			// get handle status...
			if ( !disabled )
			{
				unsigned int handleStatus = uASCIIToHex( transformInformation, 8 );
				transformInformation += 8;
				m_HandleInformation[handle].HandleInfo.bToolInPort = ( handleStatus & 0x01 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bGPIO1 = ( handleStatus & 0x02 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bGPIO2 = ( handleStatus & 0x04 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bGPIO3 = ( handleStatus & 0x08 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bInitialized = ( handleStatus & 0x10 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bEnabled = ( handleStatus & 0x20 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bOutOfVolume = ( handleStatus & 0x40 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bPartiallyOutOfVolume = ( handleStatus & 0x80 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bDisturbanceDet = ( handleStatus & 0x200 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bSignalTooSmall = ( handleStatus & 0x400 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bSignalTooBig = ( handleStatus & 0x800 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bProcessingException = ( handleStatus & 0x1000 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bHardwareFailure = ( handleStatus & 0x2000 ? 1 : 0 );

				// get frame number...
				m_HandleInformation[handle].Xfrms.ulFrameNumber = 
					uASCIIToHex( transformInformation, 8 );
				transformInformation += 8;
			} 
			transformInformation++; //for the carriage return
		} 

		unsigned int systemStatus = uASCIIToHex( transformInformation, 4 );
		m_SystemInformation.bCommunicationSyncError = ( systemStatus & 0x01 ? 1 : 0 );
		m_SystemInformation.bTooMuchInterference = ( systemStatus & 0x02 ? 1 : 0 );
		m_SystemInformation.bSystemCRCError = ( systemStatus & 0x04 ? 1 : 0 );
		m_SystemInformation.bRecoverableException = ( systemStatus & 0x08 ? 1 : 0 );
		m_SystemInformation.bHardwareFailure = ( systemStatus & 0x10 ? 1 : 0 );
		m_SystemInformation.bHardwareChange = ( systemStatus & 0x20 ? 1 : 0 );
		m_SystemInformation.bPortOccupied = ( systemStatus & 0x40 ? 1 : 0 );
		m_SystemInformation.bPortUnoccupied = ( systemStatus & 0x80 ? 1 : 0 );

	} 
	return 1;
} 
////////////////////////////////////////////////////////////////////////////
// Method:	UpdateBinaryModeTransforms
//
// Inputs:
//		bool checkOutOfVolume: whether to check if tools have gone out of volume.
//
// Return Value:
//		int - 1 if successful, 0 otherwise.
//
// Description: This method updates the transformation information using the BX
//	command.  
//
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::UpdateBinaryModeTransforms(bool checkOutOfVolume)
{
	char	*transformInformation = NULL;

	// set reply mode depending on bReturnOOV 
	int replyMode = checkOutOfVolume ? 0x0801 : 0x0001;
	
	memset(m_Command, 0, sizeof(m_Command));
	sprintf( m_Command, "BX %04X", replyMode );

	int		index = 0;

	if(SendCommand( m_Command, true ))
	{
		if (!VerifyResponse( true, false ))
			return 0;

		transformInformation = m_Response;
		
		unsigned int computedCRC = m_CyclicRedundancy.GetCyclicRedundancy(m_Response, 4 );

		// check for preamble ( A5C4 ) 
        while(((transformInformation[0]&0xff)!=0xc4))
		{
            transformInformation++;
		}

		if ( (transformInformation[0]&0xff)!=0xc4 || (transformInformation[1]&0xff)!=0xa5 )
		{
			return REPLY_INVALID;
		}

		// parse the header
        index += 2;

		//int replySize = nGetHex2(&transformInformation[index]);
        index += 2;
		
		unsigned int headerCRC = nGetHex2(&transformInformation[index]); 
        index += 2;

		if ( computedCRC != headerCRC )
		{
			return REPLY_BADCRC;
		} 
		
		int numOfHandles = nGetHex1(&transformInformation[index]);
		index++;

		int handle;
		for ( int i = 0; i < numOfHandles; i++ )
		{
			handle = nGetHex1(&transformInformation[index]);
			index++;

			unsigned int transformationStatus = nGetHex1(&transformInformation[index]);
			index++;

			unsigned int handleStatus = 0;
			
			if ( transformationStatus == 1 ) // one means that the transformation was returned 
			{
				// parse out the individual components by converting binary to floats 
				m_HandleInformation[handle].Xfrms.rotation.q0 = fGetFloat(&transformInformation[index]);
				index += 4;
				m_HandleInformation[handle].Xfrms.rotation.qx = fGetFloat(&transformInformation[index]);
				index += 4;
				m_HandleInformation[handle].Xfrms.rotation.qy = fGetFloat(&transformInformation[index]);
				index += 4;
				m_HandleInformation[handle].Xfrms.rotation.qz = fGetFloat(&transformInformation[index]);
				index += 4;
				m_HandleInformation[handle].Xfrms.translation.x = fGetFloat(&transformInformation[index]);
				index += 4;
				m_HandleInformation[handle].Xfrms.translation.y = fGetFloat(&transformInformation[index]);
				index += 4;
				m_HandleInformation[handle].Xfrms.translation.z = fGetFloat(&transformInformation[index]);
				index += 4;
				m_HandleInformation[handle].Xfrms.fError = fGetFloat(&transformInformation[index]);
				index += 4;
				handleStatus = nGetHex4(&transformInformation[index]);
				index += 4;
				m_HandleInformation[handle].Xfrms.ulFrameNumber = nGetHex4(&transformInformation[index]);
				index += 4;
				m_HandleInformation[handle].Xfrms.ulFlags = TRANSFORM_VALID;
			} 

			if ( transformationStatus == 2 || transformationStatus == 4 ) // 2 means the tool is missing and 
														  // 4 means DISABLED 
			{
				 // no transformation information is returned 
				if ( transformationStatus == 2 )
				{
					handleStatus = nGetHex4(&transformInformation[index]);
					index += 4;
					m_HandleInformation[handle].Xfrms.ulFrameNumber = nGetHex4(&transformInformation[index]);
					index += 4;
					m_HandleInformation[handle].Xfrms.ulFlags = TRANSFORM_MISSING;
				} 
				else
					m_HandleInformation[handle].Xfrms.ulFlags = TRANSFORM_DISABLED;

				m_HandleInformation[handle].Xfrms.rotation.q0 =
				m_HandleInformation[handle].Xfrms.rotation.qx =
				m_HandleInformation[handle].Xfrms.rotation.qy =
				m_HandleInformation[handle].Xfrms.rotation.qz =
				m_HandleInformation[handle].Xfrms.translation.x =
				m_HandleInformation[handle].Xfrms.translation.y =
				m_HandleInformation[handle].Xfrms.translation.z =
				m_HandleInformation[handle].Xfrms.fError = BAD_FLOAT;
			}

			if ( transformationStatus == 1 || transformationStatus == 2 )
			{
				m_HandleInformation[handle].HandleInfo.bToolInPort = ( handleStatus & 0x01 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bGPIO1 = ( handleStatus & 0x02 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bGPIO2 = ( handleStatus & 0x04 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bGPIO3 = ( handleStatus & 0x08 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bInitialized = ( handleStatus & 0x10 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bEnabled = ( handleStatus & 0x20 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bOutOfVolume = ( handleStatus & 0x40 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bPartiallyOutOfVolume = ( handleStatus & 0x80 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bDisturbanceDet = ( handleStatus & 0x200 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bSignalTooSmall = ( handleStatus & 0x400 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bSignalTooBig = ( handleStatus & 0x800 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bProcessingException = ( handleStatus & 0x1000 ? 1 : 0 );
				m_HandleInformation[handle].HandleInfo.bHardwareFailure = ( handleStatus & 0x2000 ? 1 : 0 );
			}
		} 

		unsigned int systemStatus = nGetHex2( &transformInformation[index] );
		index += 2;

		unsigned int bodyCRC = nGetHex2(&transformInformation[index]); 
		m_SystemInformation.bCommunicationSyncError = ( systemStatus & 0x01 ? 1 : 0 );
		m_SystemInformation.bTooMuchInterference = ( systemStatus & 0x02 ? 1 : 0 );
		m_SystemInformation.bSystemCRCError = ( systemStatus & 0x04 ? 1 : 0 );
		m_SystemInformation.bRecoverableException = ( systemStatus & 0x08 ? 1 : 0 );
		m_SystemInformation.bHardwareFailure = ( systemStatus & 0x10 ? 1 : 0 );
		m_SystemInformation.bHardwareChange = ( systemStatus & 0x20 ? 1 : 0 );
		m_SystemInformation.bPortOccupied = ( systemStatus & 0x40 ? 1 : 0 );
		m_SystemInformation.bPortUnoccupied = ( systemStatus & 0x80 ? 1 : 0 );

		computedCRC = m_CyclicRedundancy.GetCyclicRedundancy(transformInformation+=6, index-6 );
		if ( computedCRC != bodyCRC )
		{
			return REPLY_BADCRC;
		} 
	} 
	return 1;
} 
////////////////////////////////////////////////////////////////////////////
// Method:				SendCommand
//
// Inputs:
//	char * m_Command - the command string, coming in.  The command
//						 to be sent to the System
//	bool bAddCRC - if true, we add the CRC to the command and replace the
//				   space with the :
//
// Return Value: int -  0 if fails, 1 if passes.
//
// Description:  This command takes in a command string and parses it depending
//	on the value of addCRC.  If addCRC is true, then the space is replaced with 
//  a : and calculate and add the CRC to the command.
//  The command is then send to the NDI system.
////////////////////////////////////////////////////////////////////////////
int AuroraTracker::SendCommand( char *m_Command, bool addCRC )
{
	bool  result = false;

	// complete the command, by adding a carraige return to it and crc if specified 
	if (!BuildCommand( m_Command, addCRC ))
		return result;

	if(strlen(m_Command) >= (MAX_COMMAND_MSG))
	{
		return result;
	} 

	for( unsigned int i = 0; i < strlen(m_Command); i++ )
	{
		if ( m_ComPort.AppendCharToBuffer( m_Command[i] ) == 0 )
		{
			result = false;
			break;
		} 
		else if( m_Command[i] == CARRIAGE_RETURN )
		{
			result = true;
			break;
		}
	} 
	// flush the COM Port...this sends out any info still sitting in the buffer
	m_ComPort.FlushBuffer();
	return result;
} 
////////////////////////////////////////////////////////////////////////////
//
// Method: GetTextModeResponse
//
// Inputs: None.
//
// Output: Fills in the first reply message from the system output buffer 
//         into m_Response
// Return Value: true, if found a message, else returns false.
//
// Description: This method poles the output buffer of the system for
//     a "response". A poling may result in receiving a "reponse" or getting
//     "timed-out". If "timed-out", the last message is re-sent to the system,
//     and the poling restarts. This process continues till a "response" is
//     received, or m_MaximumNumberOfTrials is reached. If a "response" is
//     received, it is stored in the m_Response buffer and a "true" is
//     returned. Otherwise a "false" is returned.
//     (Use this response routine for all calls except the BX call)
// 
// NOTE: This method is used by VerifyResponse method.
//
////////////////////////////////////////////////////////////////////////////
bool AuroraTracker::GetTextModeResponse()
{
	char	ch;
	time_t	currenttime, starttime;
	bool	success = false;
	unsigned int count = 0, numTrials = 0;
	
    memset(m_Response, 0, sizeof( m_Response ) );
	
	// record the start time
	time( &starttime );
	
	do
	{
		// copy the text mode response from buffer, if available
		while ( (m_ComPort.CharsInBuffer() > 0) && (!success) )
		{
			ch = m_ComPort.GetCharFromBuffer();
			// a "carriage return" marks end of a response
			if ( ch == '\r' )
			{
				m_Response[count++] = CARRIAGE_RETURN;
				m_Response[count] = '\0';
				success = true;
			} 
			else
			{
				m_Response[count++] = ch;
			} 
		} 
		// if no complete response is found, check for time-out
		if ( !success )
		{
			// check if a time-out has occurred
			time( &currenttime );
			if ( difftime( currenttime, starttime ) >= m_TimeOut ) 
			{
				// check if maximum number of retrials is reached
				if( numTrials < m_MaximumNumberOfTrials )
				{
					numTrials++;
					memset( m_Response, 0, sizeof(m_Response) );
					count = 0;
					// resend the command
					SendCommand( m_Command, false ); 
					// reset the start time
					time( &starttime );
				}
				else
				{
					return false;
				}
			}
		} 
	} while ( !success );
	
	return true;
} 
////////////////////////////////////////////////////////////////////////////
//
// Method: GetBinaryModeResponse
//
// Inputs: None.
//
// Output: Fills in the first reply message from the system output buffer 
//         into m_Response
// Return Value: true, if found a message, else returns false.
//
// Description: This method poles the output buffer of the system for
//     a "response". A poling may result in receiving a "reponse" or getting
//     "timed-out". If "timed-out", the last message is re-sent to the system,
//     and the poling restarts. This process continues till a "response" is
//     received, or m_MaximumNumberOfTrials is reached. If a "response" is
//     received, it is stored in the m_Response buffer and a "true" is
//     returned. Otherwise a "false" is returned.
//     (Use this response routine for all calls except the BX call)
// 
// NOTE: This method is used by VerifyResponse method.
//
////////////////////////////////////////////////////////////////////////////
bool AuroraTracker::GetBinaryModeResponse( )
{
	char	ch;
	time_t	currenttime,	starttime;
	bool	success = false;
	int		totalResponseLength = -1;
	int		count = 0;
	unsigned int numTrials = 0;

    memset(m_Response, 0, sizeof( m_Response ) );

	// record the start time
	time( &starttime );

	do
	{
		// copy the binary mode response from buffer, if available
		while ( (m_ComPort.CharsInBuffer() > 0) && (!success) )
		{
			ch = m_ComPort.GetCharFromBuffer();

			m_Response[count] = ch;

			// record the total length of the response (header size is 7) 
			if ( count == 3 )  
			{
				totalResponseLength = nGetHex2(&m_Response[2]) + 7 + 1; 
			}

			count++;

			if ( count == totalResponseLength )
			{
			   	success = true;			
			}
		} 
		// if no complete response is found, check for time-out
		if ( !success )
		{
			// check if a time-out has occurred
			time( &currenttime );
			if ( difftime( currenttime, starttime ) >= m_TimeOut ) 
			{
				// check if maximum number of retrials is reached
				if( numTrials < m_MaximumNumberOfTrials )
				{
					numTrials++;
					memset( m_Response, 0, sizeof(m_Response) );
					count = 0;
					// resend the command
					SendCommand( m_Command, false ); 
					 // reset the start time
					time( &starttime );
				}
				else
				{
					return false;
				}
			}
		} 
	} while ( !success );

	return true;
} 
////////////////////////////////////////////////////////////////////////////
//
// Method:   VerifyResponse
//
// Inputs:	bool * textMode: System Mode is Text or Binary 
//			bool checkCRC : to perform CRC check on the response
//
// Return Value: bool: from the possible 

// Description:    This routine gets the NDI system response and checks it for 
//  the predetermined response values. 
//  Internally, it calls either GetTextModeResponse() or GetBinaryModeResponse()
//  depending on the input textMode.
//  
// SIDE EFFECT:The NDI system response is stored in m_Response;
//
//

////////////////////////////////////////////////////////////////////////////
bool AuroraTracker::VerifyResponse( bool textMode, bool checkCRC )
{
	int		result = 0;

	if (textMode)
		result = GetTextModeResponse();
	else
		result = GetBinaryModeResponse();
		
	if (result==0)
	{
		return 0;
	}

	// comparing with possible responses

	if (!strnicmp(m_Response, "RESET",5))
		result = REPLY_RESET;
	else if (!strnicmp(m_Response, "OKAY",4))
		result = REPLY_OKAY;
	else if (!strnicmp(m_Response, "ERROR",5))
		result = REPLY_ERROR;
	else if (!strnicmp(m_Response, "WARNING",7))
		result = REPLY_WARNING;
	else if ( strlen( m_Response ) > 0 )
		result = REPLY_OTHER;
	else
		return false;

	if ( result & REPLY_OKAY  || result & REPLY_OTHER || result & REPLY_RESET && checkCRC )
	{
		if (!m_CyclicRedundancy.ValidateCyclicRedundancyOfReplyMessage( m_Response ) )
		{
			result = REPLY_BADCRC;
			return false;
		}
		else
		{
			return true;
		}
	} 
	else
	{
		return false;
	}
} 

////////////////////////////////////////////////////////////////////////////
//
// Method:	BuildCommand
//
// Input Values:		
//	char *command: partial command string.
//	bool addCRC: to add or not to add CRC to the command
//								
// Output Values:
//	char *command: completed command string.
//
// Return Value: true if successful, else false.
//
// Description:	If already a complete message (confirmed if the message already
//      contains a carriage-return), return by adding a "end-of-the-string" marker
//      else complete the command, which is done as follows:
//      a. If addCRC is true, append CRC.
//		b. Append a "carriage-return" and a "end-of-the-string" marker
//
////////////////////////////////////////////////////////////////////////////
bool AuroraTracker::BuildCommand( char *command, bool addCRC )
{
	//  If the command is re-sent, add "end-of-string-marker" and return
	for ( int len = 0; len < MAX_COMMAND_MSG; len++)
	{
		if( command[len++] == CARRIAGE_RETURN )
		{
			command[len++] = '\0';
			return true;
		}
	}
	// Add CRC if asked to
	if ( addCRC )
	{
		if (!m_CyclicRedundancy.AppendCyclicRedundancyTermToCommand( command ))
			return false;
	}
	// Add "carriage-return" and "end-of-string-marker"
	len = strlen(command);
	if (len>= (MAX_COMMAND_MSG-1))
		return false;
	command[len++] = CARRIAGE_RETURN; 
	command[len++] = '\0';
	return true;
} 

bool AuroraTracker::GetOffsetCorrectedToolPosition(const int toolHandle, const float offset[3], 
												   float position[3])
{
	if ( this->m_HandleInformation[toolHandle].Xfrms.ulFlags == TRANSFORM_VALID )
	{
		QuatRotation   quat;
		RotationMatrix rotMatrix;

		quat.m_Q0 = this->m_HandleInformation[toolHandle].Xfrms.rotation.q0;
		quat.m_Qx = this->m_HandleInformation[toolHandle].Xfrms.rotation.qx;
		quat.m_Qy = this->m_HandleInformation[toolHandle].Xfrms.rotation.qy;
		quat.m_Qz = this->m_HandleInformation[toolHandle].Xfrms.rotation.qz;

		QuatToRotationMatrix( &quat, rotMatrix );
	
		position[0] = this->m_HandleInformation[toolHandle].Xfrms.translation.x;
		position[1] = this->m_HandleInformation[toolHandle].Xfrms.translation.y;
		position[2] = this->m_HandleInformation[toolHandle].Xfrms.translation.z;

		for(int i=0; i<3; i++)
		{
			for(int j=0; j<3; j++)
			{
				position[i] += rotMatrix[i][j] * offset[j];
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool AuroraTracker::GetToolPosition(const int toolHandle, float position[3])
{
	if ( this->m_HandleInformation[toolHandle].Xfrms.ulFlags == TRANSFORM_VALID )
	{
		position[0] = this->m_HandleInformation[toolHandle].Xfrms.translation.x;
		position[1] = this->m_HandleInformation[toolHandle].Xfrms.translation.y;
		position[2] = this->m_HandleInformation[toolHandle].Xfrms.translation.z;
		return true;
	}
	else
	{
		return false;
	}
}

bool AuroraTracker::GetToolOrientation(const int toolHandle, float quad[4])
{
	if ( this->m_HandleInformation[toolHandle].Xfrms.ulFlags == TRANSFORM_VALID )
	{
		quad[0] = this->m_HandleInformation[toolHandle].Xfrms.rotation.q0;
		quad[1] = this->m_HandleInformation[toolHandle].Xfrms.rotation.qx;
		quad[2] = this->m_HandleInformation[toolHandle].Xfrms.rotation.qy;
		quad[3] = this->m_HandleInformation[toolHandle].Xfrms.rotation.qz;
		return true;
	}
	else
	{
		return false;
	}
}



////////////////////////////////////////////////////////////////////////////
