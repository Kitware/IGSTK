////////////////////////////////////////////////////////////////////////////
//
// File Name:		SerialCommunication.h
//
// Description:     Implements Serial Communication through COM Port in a 
//					32 bit environment.
//
// Author: Sohan R Ranjan
//
// Affiliation: Georgetown University
//
////////////////////////////////////////////////////////////////////////////

#ifndef SERIAL_COMMUNICATION_H
#define SERIAL_COMMUNICATION_H

#include <stdio.h>
#include <windows.h>

#define PORT_OK                 1
#define PORT_IRQ_IN_USE         -1
#define PORT_OUT_OF_MEMORY      -2
#define PORT_NOT_INITIALIZED    -3
#define PORT_OUT_BUFFER_FULL    -4
#define PORT_NOT_PRESENT        -5
#define PORT_NO_DATA			-6
#define PORT_NOT_VALID			-7

#define PORT_ERROR_SETTIMEOUT		-10
#define PORT_ERROR_SETBREAK			-11
#define PORT_ERROR_CLEARBREAK		-12
#define PORT_ERROR_CLEARBUFFER		-13
#define PORT_ERROR_CREATEEVENT		-14
#define PORT_ERROR_WRITE			-15
#define PORT_ERROR_WRITETIMEOUT		-16
#define PORT_ERROR_READ				-17
#define PORT_ERROR_READTIMEOUT		-18
#define PORT_ERROR_WAIT				-19
#define PORT_ERROR_WAITTIMEOUT		-20
#define PORT_ERROR_GETOVERLAPPED	-21

#define PORT_ERR_OVERRUN        0x02
#define PORT_ERR_PARITY         0x04
#define PORT_ERR_FRAME          0x08
#define PORT_ERR_BREAK          0x10

// Buffer size 

#define READ_BUFFER_SIZE		3000
#define WRITE_BUFFER_SIZE		200

/////////////////////////////////////////////////////////////////////////

class SerialCommunication
{
public:
	
	SerialCommunication();
	
	~SerialCommunication();
	
	// This method is to set up the communication format
	int SetCommunicationFormat( int byteSize = 8, int parity = NOPARITY, 
		int stopBits = ONESTOPBIT, bool hardwareHandshake = false);

	// This method sets the baud rate
	int SetBaudRate( unsigned long baudRate );
	
	// This method opens the COM Port handle 
	int OpenPort( unsigned int portNum );
	
	// This method suspends character transmission  places the transmission line in a break state until the ClearCommBreak function is called
	int RestPort( unsigned int duration = 256 );
	
	// This method flushes the output buffer of the COM Port 
	int FlushBuffer();
	
	// This method appends a single character in the output buffer
	int AppendCharToBuffer( unsigned char ch );
	
	// This method appends a string in the output buffer
	int AppendStringToBuffer( unsigned char *string, unsigned long len );
	
	// This method reads in a string value from the input buffer
	int  GetStringFromBuffer(unsigned char *string, unsigned long maxLen);
	
	// This method gets a single character from the input buffer
	int  GetCharFromBuffer( void );
	
	// This method check if there are characters in the input buffer to be read
	int  CharsInBuffer( void );
	
	// This method checks the error status of the COM Port
	int	PortErrorStatus( void );
	
	// This method closes the COM Port handle 
	void ClosePort( void );

	// This method gets the communication port number in use.
	int  GetPortNumber( void );

	// This method is to reset port as per the communication parameters
	int  ResetPort( void );

	
private:

	// Communication Parameters
    unsigned long	m_BaudRate;		// Baud rate 
    unsigned int	m_ByteSize;		// number of bits/byte, 4-8 
    unsigned int	m_Parity;		// 0-4=no,odd,even,mark,space 
    unsigned int	m_StopBits;		// 0,1,2 = 1, 1.5, 2 
	bool	m_HardwareHandshake;	// hardware handshaking 

	unsigned int	m_PortNumber;	// Port Number
	
	HANDLE	m_PortHandle;			// com port handle 
	
	COMMTIMEOUTS	CommTimeouts;	// com port timeouts 

	
	// Input Buffer
	unsigned char m_InputBuffer[READ_BUFFER_SIZE];		
	int		m_BufferOffset;	
	int		m_BufferSize;	
	
};
#endif
