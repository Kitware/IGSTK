/////////////////////////////////////////////////////////////////////////////////
//
// CyclicRedundancy.cxx
//
// Cyclic Redundancy class for NDI Trackers
//
// Created by: Sohan R Ranjan, ISIS Center, Georgetown University
//
// Date: 16 Sept 2003
//
/////////////////////////////////////////////////////////////////////////////////

#include "CyclicRedundancy.h"
#include <stdio.h>
#include <string.h>

/////////////////////////////////////////////////////////////////////////////////
// Constructor
// This does some important initialization
/////////////////////////////////////////////////////////////////////////////////
CyclicRedundancy::CyclicRedundancy( void )
{
	 // Initialize Cyclic Redundancy lookup table
	long lvar;
	for( int i=0; i<256; i++ )
	{
		lvar = i;
		for(  int j=0; j<8; j++ )
			lvar = ( lvar >> 1 ) ^ (( lvar & 1 ) ? 0xA001L : 0 );

		CyclicRedundancyTable[i] = (unsigned int) lvar & 0xFFFF;
	} 
	// Values from 0 to 15 with odd parity =1, else 0
	OddParity[0] = 0;	OddParity[1] = 1;	OddParity[2] = 1;
	OddParity[3] = 0;	OddParity[4] = 1;	OddParity[5] = 0; 
	OddParity[6] = 0;	OddParity[7] = 1;	OddParity[8] = 1;
	OddParity[9] = 0;	OddParity[10] = 0;	OddParity[11] = 1;
	OddParity[12] = 0;	OddParity[13] = 1;	OddParity[14] = 1;
	OddParity[15] = 0;
}
/////////////////////////////////////////////////////////////////////////////////
CyclicRedundancy::~CyclicRedundancy( void )
{
}
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
unsigned int CyclicRedundancy::ComputeCyclicRedundancyOfData(unsigned int crc, int data)
{
	crc = CyclicRedundancyTable[ (crc ^ data) & 0xFF] ^ (crc >> 8);
	return (crc & 0xFFFF);
}
/////////////////////////////////////////////////////////////////////////////////
// VISIT HERE AFTER RUNNING ONCE. CAN BE SHORTENED
/////////////////////////////////////////////////////////////////////////////////
unsigned int CyclicRedundancy::ComputeCyclicRedundancyOfString( char *pszString, int nLen )
{
    unsigned int data, uCrc = 0;
    unsigned char *puch = (unsigned char *)pszString;
    int	nCnt = 0;

    while ( nCnt < nLen )
    {
        data = (*puch ^ (uCrc & 0xff)) & 0xff;
        uCrc >>= 8;

        if ( OddParity[data & 0x0f] ^ OddParity[data >> 4] )
        {
            uCrc ^= 0xc001;
        } 

        data <<= 6;
        uCrc ^= data;
        data <<= 1;
        uCrc ^= data;
        puch++;
        nCnt++;
    } 
    return uCrc;
}
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
bool CyclicRedundancy::ValidateCyclicRedundancyOfReplyMessage(char *str)
{
	unsigned int computedCrc, containedCrc = 0;

	// Different checks for BINARY and TEXT format reply messages 
	// The start byte for BINARY message is 0xA5C4

	if ( ((str[0] & 0xff) == 0xc4) && ((str[1] & 0xff) == 0xa5) ) 
	{
		// check the header crc
		containedCrc = (str[4] & 0xff) | ((str[5] & 0xff) << 8); 
		computedCrc  = ComputeCyclicRedundancyOfString(str, 4);
		if ( computedCrc == containedCrc) 
		{
			// check the body crc
			// Reply size at [2] and [3] + 6 header bytes + 2 CRC bytes.
			unsigned int replySize = ((str[2] & 0xff) | ((str[3] & 0xff) << 8)) + 8; 

			containedCrc = (str[replySize-2] & 0xff) | ((str[replySize-1] & 0xff) << 8); 

			computedCrc = ComputeCyclicRedundancyOfString(&str[6], (replySize-8));
			
			return (computedCrc == containedCrc); 
		}
		else
		{
			return false;		
		}
	}
	else	// TEXT format message
	{
		// find the length of the reply string
		int strLength = 0;

		while((str[strLength]!= CARRIAGE_RETURN) && (strLength<MAX_REPLY_MSG))
			strLength++;

		// if measured string length is equal to MAX_REPLY_MSG, report error
		if (strLength>=MAX_REPLY_MSG)
			return false;

		str[strLength+1] = 0;

		// compute CRC of the string
		computedCrc = 0;
		for(int m=0; m<(strLength-4); m++)
		{
			computedCrc = ComputeCyclicRedundancyOfData(computedCrc,str[m]);
		}

		// contained CRC from the reply message
		sscanf(&(str[strLength-4]),"%04x",&containedCrc);
		return (computedCrc == containedCrc);
	}
} 
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
unsigned int CyclicRedundancy::GetCyclicRedundancy(char *str, int length)
{
	unsigned int crc = 0;
	for(int m=0; m<(length);m++)
	{
		crc = ComputeCyclicRedundancyOfData(crc, str[m]);
	}
	return crc;
}
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
int CyclicRedundancy::AppendCyclicRedundancyTermToCommand( char *command )
{
	if(strlen(command) >= (MAX_COMMAND_MSG-6))
		return 0;

	int n = strlen(command);

	// compute the cyclic redundancy value 

	unsigned int crc = 0;
	for( int m=0; m<n; m++)
	{
		 // replace space character with : 
		if(command[m]==' ') command[m]=':';
		crc = ComputeCyclicRedundancyOfData(crc,command[m]);
	} 
	sprintf(&command[n],"%04X",crc);
	return 1;
}
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
