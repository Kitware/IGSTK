////////////////////////////////////////////////////////////////////////////
//
// File Name:		SerialCommunication.cpp
//
// Description:     Implements Serial Communication through COM Port in a 
//					32 bit environment.
//
// Author: Sohan R Ranjan
//
// Affiliation: Georgetown University
//
////////////////////////////////////////////////////////////////////////////

#include "SerialCommunication.h"

////////////////////////////////////////////////////////////////////////////

SerialCommunication::SerialCommunication()
{
	m_HardwareHandshake = false;
    m_BaudRate = CBR_9600;			
    m_ByteSize = 8;					
    m_Parity = NOPARITY;			
    m_StopBits = ONESTOPBIT;    
	
	m_BufferSize = 0;
	m_BufferOffset = 0;
	
	m_PortHandle = INVALID_HANDLE_VALUE;

	// Timeouts settings: 
	// Read: return immediately with number of bytes read 
	// Write: return after time as defined below
				  
	CommTimeouts.ReadIntervalTimeout = MAXWORD;
	CommTimeouts.ReadTotalTimeoutConstant = 0;
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommTimeouts.WriteTotalTimeoutConstant = 500;
	CommTimeouts.WriteTotalTimeoutMultiplier = 10;
	
	memset(m_InputBuffer, '\0', sizeof(m_InputBuffer));
}

////////////////////////////////////////////////////////////////////////////

SerialCommunication::~SerialCommunication()
{
	ClosePort();
}

////////////////////////////////////////////////////////////////////////////
//
// Method:   ClosePort
//
// Inputs:	None.
// Outputs: None.
// Returns: None.
// Description: Closes an open port.
//
////////////////////////////////////////////////////////////////////////////
void SerialCommunication::ClosePort()
{
	if( m_PortHandle != INVALID_HANDLE_VALUE ) 
	{
		CloseHandle( m_PortHandle );
		m_PortHandle = INVALID_HANDLE_VALUE;
	} 
} 


////////////////////////////////////////////////////////////////////////////
//
// Method:   SetCommunicationFormat
//
// Inputs:	int byteSize - the port number to open ( 0 - 8, 0 = COM1 )
//			int parity - the baud rate for the COM Port
//			int stopBits - the format to open the COM port with PORT_7E1, PORT_7O1, PORT_8N1
//			bool hardwareHandshake - whether or not to include hardware handshaking
//
//Returns:  Port Status
//	
//Description:
//	This routine sets up the communication format. If the port is open already,
//  then it resets the port as per the new communication format.
//
////////////////////////////////////////////////////////////////////////////
int  SerialCommunication::SetCommunicationFormat( int byteSize, int parity, 
		int stopBits, bool hardwareHandshake)
{	
	// ByteSize: number of bits/byte, 4-8 
    if ((byteSize>=4) && (byteSize<=8)) 
		m_ByteSize = byteSize;             

	// Parity: 0-4=no,odd,even,mark,space 
	if ((parity>=0) && (parity<=4))
		m_Parity = parity;   
	
	//StopBits: 0,1,2 = 1, 1.5, 2 
	if ((stopBits>=0) && (stopBits<=2))
		m_StopBits = stopBits;  
	
	m_HardwareHandshake = hardwareHandshake;
	
	if (m_PortHandle != INVALID_HANDLE_VALUE)
		return ResetPort();
	else
		return PORT_NOT_PRESENT;	
}


////////////////////////////////////////////////////////////////////////////
//
// Method:   ResetPort
//
// Inputs:	None
// 
// Returns: Port Status
//
//
// Description: This method resets the port to the set communication
//              parameters.
//	
////////////////////////////////////////////////////////////////////////////
int  SerialCommunication::ResetPort( void )
{
	DCB			dcb;

	if ((m_PortHandle == INVALID_HANDLE_VALUE) || (!GetCommState(m_PortHandle, &dcb)))
		return PORT_NOT_PRESENT;	
/*
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.ByteSize = 8;
	dcb.fParity = 0;
    dcb.BaudRate = (DWORD) 9600;

	dcb.fOutxCtsFlow = 0;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
*/
	dcb.Parity = (BYTE)	m_Parity;
	dcb.StopBits = (BYTE) m_StopBits;
	dcb.ByteSize = (BYTE) m_ByteSize;
	dcb.fParity = (dcb.Parity == NOPARITY ? 0 : 1);

    dcb.BaudRate = (DWORD) m_BaudRate;	

	if( m_HardwareHandshake )
	{
		dcb.fOutxCtsFlow = 1;
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
	} 
	else
	{
		dcb.fOutxCtsFlow = 0;
		dcb.fRtsControl = RTS_CONTROL_DISABLE;
	} 

//	printf("Reset Port: Baud Rate = %.2d, Parity = %.2d, Stop Bits = %.2d, Byte Size = %.2d, fParity = %.2d, HandShake = %.2d\n",
//		dcb.BaudRate, dcb.Parity, dcb.StopBits, dcb.ByteSize, dcb.fParity, dcb.fOutxCtsFlow);

	if (!SetCommState(m_PortHandle, &dcb))
		return PORT_NOT_INITIALIZED;

	// Set COMM timeouts
	if (!SetCommTimeouts(m_PortHandle, &CommTimeouts))
		return PORT_NOT_INITIALIZED;

	// Flush out the input and output buffers 
	PurgeComm(m_PortHandle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

	return PORT_OK;

}


////////////////////////////////////////////////////////////////////////////
//
// Method:   OpenPort
//
// Inputs:		unsigned int portNum - the port number to open ( 0 - 8, 0 = COM1 )
//
// Returns: Port Status
//
// Description: This method opens up the selected COM port for communication
//
////////////////////////////////////////////////////////////////////////////
int SerialCommunication::OpenPort( unsigned int portNum )
{
	if (portNum <=0 )
	{
		return PORT_NOT_VALID;
	}

	char sPortName[6];

	sprintf(sPortName, "COM%1d", portNum );

	m_PortNumber = portNum;
		
	m_PortHandle = CreateFile( sPortName,
					 GENERIC_READ | GENERIC_WRITE,
					 0,    // comm devices must be opened w/exclusive-access 
					 0,    // no security attrs 
					 OPEN_EXISTING, // comm devices must use OPEN_EXISTING 
					 FILE_FLAG_OVERLAPPED,    // Required for asynchronous write operations 
					 NULL  // hTemplate must be NULL for comm devices 
					);

	if( m_PortHandle == INVALID_HANDLE_VALUE)
		return 0;

	SetupComm(m_PortHandle, READ_BUFFER_SIZE, WRITE_BUFFER_SIZE );

	return ResetPort();
} 

////////////////////////////////////////////////////////////////////////////
//
// Method:   SetBaudRate
// 
// Inputs:	unsigned long ulBaudRate - the Baud Rate setting for the COM Port
//
// Returns: Port Status
// 
//Description:
//	This routine sets up the communication baud rate. If the port is open 
//  already, then it resets the port as per the new communication format.
//
////////////////////////////////////////////////////////////////////////////
int SerialCommunication::SetBaudRate( unsigned long ulBaudRate )
{
	m_BaudRate  = ulBaudRate;

	return ResetPort();

} 

////////////////////////////////////////////////////////////////////////////
//
// Method:   RestPort()
//
// Inputs: duration (in milliseconds) for which the port is rested.  
//         
// Returns: Port Status
//
// Description: This method suspends character transmission for a specified 
// "duration".
//
////////////////////////////////////////////////////////////////////////////
int SerialCommunication::RestPort( unsigned int duration)
{
	if( m_PortHandle == INVALID_HANDLE_VALUE )
		return PORT_NOT_INITIALIZED;

	if (!SetCommBreak( m_PortHandle ))
		return PORT_ERROR_SETBREAK;

	Sleep( duration );

	if (!ClearCommBreak( m_PortHandle ))
		return PORT_ERROR_CLEARBREAK;

	return PORT_OK;

} 

////////////////////////////////////////////////////////////////////////////
//
// Method:   FlushBuffer
//
// Inputs: None.
//
// Returns: Port Status
//
// Description: This method flushed the output buffer. Any commands in the 
// buffer are sent out.
//
////////////////////////////////////////////////////////////////////////////
int SerialCommunication::FlushBuffer()
{
	if( m_PortHandle == INVALID_HANDLE_VALUE )
		return PORT_NOT_INITIALIZED;

	if (!FlushFileBuffers( m_PortHandle ))
		return PORT_ERROR_CLEARBUFFER;
	
	return PORT_OK;

} 

////////////////////////////////////////////////////////////////////////////
//
// Routine:   AppendStringToBuffer
//
// Inputs:	unsigned char *string : string to be place in the out buffer
//			unsigned long len : Length of the string to be appended
//
// Returns: Port Status
//
// Description: This method takes the string and size to be placed in the out buffer
//	of the serial port and then writes that string to the hardware.  It checks
//	to see if it wrote as much as was requested.
////////////////////////////////////////////////////////////////////////////
int SerialCommunication::AppendStringToBuffer( unsigned char *string, unsigned long len )
{
	OVERLAPPED		osWrite = {0};

	unsigned long	bytesWritten = 0;

	int				result = 0;

	if( m_PortHandle == INVALID_HANDLE_VALUE )
		return PORT_NOT_INITIALIZED;
	
	// Create an overlapped event
	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	if( osWrite.hEvent == NULL )
	   	result = PORT_ERROR_CREATEEVENT;
	
	// Write the string to the buffer 
 	if( !WriteFile(m_PortHandle, string, len, &bytesWritten, &osWrite) )
	{
		if( GetLastError() != ERROR_IO_PENDING )
		{
			result = PORT_ERROR_WRITE; 
		} 
		else
		{
			// Write is pending 
			DWORD dwResult = WaitForSingleObject(osWrite.hEvent, INFINITE); 
			switch(dwResult)
			{
			case WAIT_OBJECT_0:
				if( !GetOverlappedResult(m_PortHandle, &osWrite, &bytesWritten, FALSE))
				{
					result = PORT_ERROR_GETOVERLAPPED;
				} 
				else
				{
					if (bytesWritten == len)
						result = PORT_OK;
					else
					{
						// write operation timed out, abort
						result = PORT_ERROR_WRITETIMEOUT;
					} 
				} 
				break;

			case WAIT_TIMEOUT:
				result = PORT_ERROR_WAITTIMEOUT;
				break;

			default:
				result = PORT_ERROR_WAIT;
				break;
			} 
		} 
    } 
	else
	{
		// WriteFile completed immediately 
		if (bytesWritten == len)
			result = PORT_OK;
		else
		{
			//write operation timed out, abort
			result = PORT_ERROR_WRITETIMEOUT;
		} 
	} 

	// Close the overlapped event handle to avoid a handle leak
	CloseHandle(osWrite.hEvent);

	return result; 
} 

////////////////////////////////////////////////////////////////////////////
//
// Routine:   AppendCharToBuffer
//
// Inputs:	unsigned char ch : char to be place in the out buffer
//
// Returns: Port Status
//
// Description: This method takes puts a single character in the output buffer
//
////////////////////////////////////////////////////////////////////////////
int SerialCommunication::AppendCharToBuffer(unsigned char uch)
{
	int nRes = AppendStringToBuffer(&uch, 1);
	
	if (nRes < 0)
		return nRes;
	else
		return PORT_OK;

} 

////////////////////////////////////////////////////////////////////////////
//
// Method:   PortErrorStatus
//
// Inputs: None.
//
// Returns: (int) Error Code
//
// Description: This method checks the error status of the COM Port (serial)
//		for a chosen set of error condition.
//
////////////////////////////////////////////////////////////////////////////
int SerialCommunication::PortErrorStatus()
{
	COMSTAT comStat  = {0};
	DWORD   dwErrors = 0;
	int nErrorCode  = 0;

	if( m_PortHandle == INVALID_HANDLE_VALUE )
		return PORT_NOT_INITIALIZED;

	if( !ClearCommError( m_PortHandle, &dwErrors, &comStat ))
		return PORT_NOT_PRESENT;

	if( dwErrors & CE_BREAK )
		nErrorCode |= PORT_ERR_BREAK;

	if(dwErrors & CE_FRAME )
		nErrorCode |= PORT_ERR_FRAME;

	if(dwErrors & CE_RXPARITY )
		nErrorCode |= PORT_ERR_PARITY;

	if( dwErrors & CE_OVERRUN )
		nErrorCode |= PORT_ERR_OVERRUN;

	return nErrorCode;

} 


////////////////////////////////////////////////////////////////////////////
//
//  ************ C O M E   B A C K   L A T E R  *************************
//
// Method:   GetStringFromBuffer
//
// Inputs:	unsigned char *string : buffer to hold the read string
//			unsigned long maxLen : maximum length of the read input string 
// 
// Returns:	(int) Port Status
//
// Description: This method reads in a string value from the input buffer of the 
//	COM Port, up to a maximum of maxLen characters
//
////////////////////////////////////////////////////////////////////////////
int SerialCommunication::GetStringFromBuffer(unsigned char *string, unsigned long maxLen )
{
	DWORD		dwResult = 0,
				dwRead   = 0,
				dwErrors = 0;  
	
	unsigned long bytesRead = 0, bytesToRead = 0;

	COMSTAT		comStat = {0};

	BOOL		waitingOnRead = FALSE,
				res = FALSE;

	int			result = 0;

	if( m_PortHandle == INVALID_HANDLE_VALUE )
		return PORT_NOT_INITIALIZED;

	if( !ClearCommError( m_PortHandle, &dwErrors, &comStat ))
		return PORT_NOT_PRESENT;

	if( comStat.cbInQue < maxLen)
		bytesToRead = comStat.cbInQue;
	else
		bytesToRead = maxLen;

	// Create the overlapped event

	OVERLAPPED	osRead = {0};
	osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if( osRead.hEvent == NULL )
	   	return PORT_ERROR_CREATEEVENT;

	if (!waitingOnRead)
	{
		// read 
		if( !ReadFile(m_PortHandle, string, bytesToRead, &bytesRead, &osRead) )
		{
			if( GetLastError() != ERROR_IO_PENDING )
			{
				// read failure
				result = PORT_ERROR_READ;
				res = FALSE; 
			} 
			else
				waitingOnRead = TRUE;
		} 
		else
		{
			// read successful
			res = TRUE;
		} 
	} 

	//detection of completion
	if (waitingOnRead)
	{
		dwResult = WaitForSingleObject(osRead.hEvent, 1000); // 1 second time-out
		switch(dwResult)
		{
		case WAIT_OBJECT_0:
			if( !GetOverlappedResult(m_PortHandle, &osRead, &dwRead, FALSE))
			{
				result = PORT_ERROR_GETOVERLAPPED;
				res = FALSE; // error in communication, report error
			} 
			else
				res = TRUE;
			
			//Reset flag so that another operation can be issued
			waitingOnRead = FALSE;
			break;

		case WAIT_TIMEOUT:
			result = PORT_ERROR_WAITTIMEOUT;
			break; // operation is not completed yet

		default:
			result = PORT_ERROR_WAIT;
			res = FALSE; // error in the waitForSingleObject, abort.
			break;
		} 
	} 

	CloseHandle(osRead.hEvent);

	if (res == TRUE)
		return bytesRead;
	else
		return result;
} 

////////////////////////////////////////////////////////////////////////////
// Method:   GetCharFromBuffer
//
// Inputs:	None.
//
// Returns: (int) char
// Description: This method reads a single char from the input buffer
//
////////////////////////////////////////////////////////////////////////////
int SerialCommunication::GetCharFromBuffer()
{
	int nRetChar = PORT_NO_DATA;

	if( !m_BufferSize )
	{
		m_BufferSize = GetStringFromBuffer( m_InputBuffer, READ_BUFFER_SIZE );
		m_BufferOffset = 0;
	} 

	if( m_BufferSize > 0 )
	{
		nRetChar = m_InputBuffer[m_BufferOffset];
		m_BufferOffset++;
		m_BufferSize--;
	} 

	return nRetChar;
} 


////////////////////////////////////////////////////////////////////////////
//
// Method:   CharsInBuffer
//
// Inputs:	None.
//
// Returns: Number of chars in the input buffer
//
// Description: This method returns the number of characters in the input 
//		buffer
//
////////////////////////////////////////////////////////////////////////////
int SerialCommunication::CharsInBuffer()
{ 
	if( !m_BufferSize )
	{
		m_BufferSize = GetStringFromBuffer( m_InputBuffer, READ_BUFFER_SIZE );
		m_BufferOffset = 0;
	}
	return m_BufferSize;
} 

////////////////////////////////////////////////////////////////////////////
//
// Method:   GetPortNumber
//
// Inputs:	None.
//
// Returns: Communication Port number in use (1, ...)
//
// Description: This method gets the communication port number in use.
//
// 
////////////////////////////////////////////////////////////////////////////
int  SerialCommunication::GetPortNumber( void )
{
	if( m_PortHandle == INVALID_HANDLE_VALUE )
		return PORT_NOT_INITIALIZED;
	else
		return m_PortNumber;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
