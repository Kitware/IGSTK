/////////////////////////////////////////////////////////////////////////////////
//
// CyclicRedundancy.h
//
// Cyclic Redundancy class for NDI Trackers
//
// This routine implements a 16 bit CRC computation using the polynomial
//
//                              X^16 + X^15 + X^2 + 1
//
// Created by: Sohan R Ranjan, ISIS Center, Georgetown University
//
// Date: 16 Sept 2003
//
/////////////////////////////////////////////////////////////////////////////////
#ifndef __CyclicRedundancy_h__
#define __CyclicRedundancy_h__

#include "CommonNDIDefines.h"

class CyclicRedundancy
{
public:

	CyclicRedundancy( void );

	~CyclicRedundancy( void );

	// This method checks if the input string "str" has the correct cyclic redundancy.
	// Return "true" if found correct.
	bool ValidateCyclicRedundancyOfReplyMessage(char *str);

	// This method gets the cyclic redundancy value for the "length" of the string "str".
	unsigned int GetCyclicRedundancy(char *str, int length);

	int AppendCyclicRedundancyTermToCommand( char * pszCommandString );

private:
	
	unsigned int ComputeCyclicRedundancyOfData( unsigned int crc, int data );

	unsigned int ComputeCyclicRedundancyOfString( char *str, int len );

private:

	unsigned int CyclicRedundancyTable[256];

	unsigned char OddParity[16] ; 
};

#endif