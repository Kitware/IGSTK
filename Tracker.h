/////////////////////////////////////////////////////////////////////////////////
//
// General tracking class
//
// Created by: Sohan R Ranjan, ISIS Center, Georgetown University
//
// Date:	5 Sept 2003
//
/////////////////////////////////////////////////////////////////////////////////
#ifndef __Tracker_h__
#define __Tracker_h__

#include <stdio.h>
#include "IniFileRW.h"

// Position structure
typedef struct PositionStruct
{
    float   x;
    float   y;
    float   z;
} Position;

// Quaternion Structure 
typedef struct QuaternionRotationStruct
{
    float   q0;
    float   qx;
    float   qy;
    float   qz;
} QuaternionRotation;

// Transformation Information Structure 
typedef struct
{
	unsigned long
		ulFlags,
		ulFrameNumber;
	QuaternionRotation		rotation;
	Position				translation;
	float					fError;
}TransformInformation;


class Tracker
{
public:

	typedef enum
    { 
		MODE_PRE_INIT, MODE_INIT, MODE_TRACKING, MODE_ACTIVATED
    } 
    TrackerStateType;

	
	Tracker() 
	{
	}

	~Tracker() 
	{
	}
	
	// Initialize the tracker: Read parameters from the ini file for the tracking 
	// device and tools to be tracked, read instruction sequence for initializing, 
	// and do initialization. 
	virtual int InitializeTracker( const char *fileName = NULL );
	
	// Reset tracker
	virtual int ResetTracker( void ) = NULL;		 
	
	// Start tracking 
	virtual int StartTracking( void ) = NULL;		
	
	// End tracking
	virtual int StopTracking( void ) = NULL;
	
protected:

	int		m_NumberOfTools;	// Number of tools attached to the tracker

	IniFileRW	m_Parameters;	// Parameter from the ini file

};

#endif
