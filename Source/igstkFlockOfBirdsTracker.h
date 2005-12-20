/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFlockOfBirdsTracker.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_FlockOfBirdsTracker_h_
#define __igstk_FlockOfBirdsTracker_h_

#include "igstkSerialCommunication.h"
#include "igstkFlockOfBirdsTrackerTool.h"
#include "igstkTracker.h"

namespace igstk
{
/** \class FlockOfBirdsTracker
  * \brief Provides support for the FlockOfBirds optical tracker.
  *
  * The FlockOfBirds is a magnetic tracker from Ascencion.
  * This class provides an interface to the FlockOfBirds.
  *
  * \ingroup Trackers
  *
  */

const unsigned int FoBDataSize[8] = {3,3,9,6,12,0,4,7};

#define POSK36 (float)(36.0/32768.0)    /* integer to inches */
#define POSK72 (float)(72.0/32768.0)    /* integer to inches */
#define POSK144 (float)(144.0/32768.0)  /* integer to inches ER Controller */
#define DTR (float)(3.141593/180.0)     /* degrees to radians */
#define FTW (float)32768.0              /* float to word integer */
#define WTF (float)(1.0/32768.0)        /* float to word integer */
#define ANGK (float)(180.0/32768.0)     /* integer to degrees */
const double I_TO_CM = 2.54;    /* inches to centimeters */
const double I_TO_MM = 25.4;    /* inches to millimeters */
const double CM_TO_I = 1/2.54;  /* centimeters to inches */
const double MM_TO_I = 1/25.4;  /* millimeters to inches */
const double MM_TO_CM = 1/10;   /* millimeters to centimeters */
const double CM_TO_MM = 10;     /* centimeters to millimeters */


class FlockOfBirdsTracker : public Tracker
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( FlockOfBirdsTracker, Tracker )

public:

  typedef enum 
    {
    POINTM=0,
    CONTINUOUS=1, 
    STREAM=2
    } FoBMode;

  typedef enum 
    {
    POS=0, 
    ANGLE=1, 
    MATRIX=2, 
    POSANGLE=3, 
    POSMATRIX=4, 
    FACTORY_USE_ONLY=5,  
    QUATER=6, 
    POSQUATER=7
    } FoBType;

  typedef enum 
    {
    F_UP_RIGHT=0, 
    F_UP_LEFT=1, 
    F_DOWN_LEFT=2, 
    F_DOWN_RIGHT=3, 
    B_UP_RIGHT=4, 
    B_UP_LEFT=5, 
    B_DOWN_LEFT=6, 
    B_DOWN_RIGHT=7
    } FoBHemisphere;

  typedef enum 
    {
    _36_INCHES, 
    _72_INCHES, 
    _144_INCHES = 1
    } FoBPositionScaling;

  typedef enum 
    {
    INCHES=0, 
    CM=1, 
    MM=2
    } FoBUnit;

  /** typedefs for the tool */
  typedef igstk::FlockOfBirdsTrackerTool              FlockOfBirdsTrackerToolType;
  typedef FlockOfBirdsTrackerToolType::Pointer        FlockOfBirdsTrackerToolPointer;
  typedef FlockOfBirdsTrackerToolType::ConstPointer   FlockOfBirdsTrackerToolConstPointer;

  /** number of ports to allow */
  itkStaticConstMacro( NumberOfPorts, unsigned int, 1 );

  /** communication type */
  typedef igstk::SerialCommunication     CommunicationType;

  /** The SetCommunication method is used to attach a communication
    * object to the tracker object. */
  void SetCommunication( CommunicationType *communication );

  /** Get the number of tools that have been detected. */
  igstkGetMacro( NumberOfTools, unsigned int );

  /** Specify an SROM file to be used with a passive or custom tool. */
  void AttachSROMFileNameToPort( const unsigned int portNum, 
                                 std::string  fileName );

protected:

  FlockOfBirdsTracker(void);

  virtual ~FlockOfBirdsTracker(void);

  /** typedef for internal boolean return type */
  typedef Tracker::ResultType   ResultType;

  /** Open communication with the tracking device. */
  virtual ResultType InternalOpen( void );

  /** Close communication with the tracking device. */
  virtual ResultType InternalClose( void );

  /** Activate the tools attached to the tracking device. */
  virtual ResultType InternalActivateTools( void );

  /** Deactivate the tools attached to the tracking device. */
  virtual ResultType InternalDeactivateTools( void );

  /** Put the tracking device into tracking mode. */
  virtual ResultType InternalStartTracking( void );

  /** Take the tracking device out of tracking mode. */
  virtual ResultType InternalStopTracking( void );

  /** Update the status and the transforms for all TrackerTools. */
  virtual ResultType InternalUpdateStatus( void );

  /** Update the status and the transforms. 
      This function is called by a separate thread. */
  virtual ResultType InternalThreadedUpdateStatus( void );

  /** Reset the tracking device to put it back to its original state. */
  virtual ResultType InternalReset( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const; 

private:

  FlockOfBirdsTracker(const Self&);   //purposely not implemented
  void operator=(const Self&);   //purposely not implemented

  /** A mutex for multithreaded access to the buffer arrays */
  ::itk::MutexLock::Pointer  m_BufferLock;

  /** A buffer for holding tool transforms */
  double m_TransformBuffer[NumberOfPorts][8];

  /** A buffer for holding status of tools */
  int m_StatusBuffer[NumberOfPorts];

  /** A buffer for holding absent status of tools */
  int m_AbsentBuffer[NumberOfPorts];

  /** Load a virtual SROM, given the file name of the ROM file */
  bool LoadVirtualSROM( const unsigned int port, 
                        const std::string SROMFileName) ;

  /** Clear the virtual SROM for a tool */
  void ClearVirtualSROM( const unsigned int port );

  /** Enable all tool ports that have tools plugged into them.
   * {The reference tool port is enabled as a static tool.} */
  void EnableToolPorts( void );

  /** Disable all enabled tool ports. */
  void DisableToolPorts( void );

  /** Information about which tool ports are enabled. */
  int m_PortEnabled[NumberOfPorts];

  /** The tool handles that the device has provides us with. */
  int m_PortHandle[NumberOfPorts];

  /** Total number of tools detected. */
  unsigned int   m_NumberOfTools;

  /** Names of the SROM files for passive tools and custom tools. */
  std::string    m_SROMFileNames[NumberOfPorts];

  /** The "Communication" instance */
  CommunicationType::Pointer       m_Communication;

  /** Set the mode of operation of the Flock of Birds */
  ResultType SetMode(const FoBMode mode);

  /** Set the type of operation of the Flock of Birds */
  ResultType SetType(const FoBType type);

  /** Set the position scaling of the Flock of Birds */
  ResultType SetPositionScaling(const FoBPositionScaling scaling);

  /** Mode of the Flock of Birds */
  FoBMode m_Mode;

  /** Type of acquisition */
  FoBType m_Type;

  /** Position Scaling for the Flock of Birds */
  FoBPositionScaling m_PositionScaling;

};

}

#endif //__igstk_FlockOfBirdsTracker_h_
