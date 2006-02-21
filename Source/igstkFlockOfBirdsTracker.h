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
#ifndef __igstkFlockOfBirdsTracker_h
#define __igstkFlockOfBirdsTracker_h

#include "igstkSerialCommunication.h"
#include "igstkFlockOfBirdsTrackerTool.h"
#include "igstkFlockOfBirdsCommandInterpreter.h"
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
class FlockOfBirdsTracker : public Tracker
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( FlockOfBirdsTracker, Tracker )

public:

  /** Number of ports to allow */
  itkStaticConstMacro( NumberOfPorts, unsigned int, 1 );

  /** Command Interpreter */
  typedef igstk::FlockOfBirdsCommandInterpreter CommandInterpreterType;

  /** Communication type */
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
  void operator=(const Self&);        //purposely not implemented

  /** typedefs for the tool */
  typedef igstk::FlockOfBirdsTrackerTool              
                                     FlockOfBirdsTrackerToolType;
  typedef FlockOfBirdsTrackerToolType::Pointer        
                                     FlockOfBirdsTrackerToolPointer;
  typedef FlockOfBirdsTrackerToolType::ConstPointer   
                                     FlockOfBirdsTrackerToolConstPointer;

  /** A buffer for holding tool transforms */
  double m_TransformBuffer[NumberOfPorts][8];

  /** A buffer for holding status of tools */
  int m_StatusBuffer[NumberOfPorts];

  /** A buffer for holding absent status of tools */
  int m_AbsentBuffer[NumberOfPorts];

  /** Load a virtual SROM, given the file name of the ROM file */
  bool LoadVirtualSROM( const unsigned int port, 
                        const std::string SROMFileName);

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

  /** The command interpreter */
  CommandInterpreterType::Pointer  m_CommandInterpreter;

  /** A mutex for multithreaded access to the buffer arrays */
  itk::MutexLock::Pointer  m_BufferLock;

};

}

#endif //__igstk_FlockOfBirdsTracker_h_
