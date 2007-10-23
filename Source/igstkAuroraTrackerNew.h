/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTrackerNew.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkAuroraTrackerNew_h
#define __igstkAuroraTrackerNew_h

#include "igstkSerialCommunication.h"
#include "igstkNDICommandInterpreter.h"
#include "igstkAuroraTrackerTool.h"
#include "igstkTrackerNew.h"

namespace igstk
{
/** \class AuroraTracker
  * \brief Provides support for the AURORA magnetic tracker.
  *
  * This class provides an interface to the Aurora magnetic
  * tracking system, which is manufactured by
  * Northern Digital Inc. in Waterloo, Ontario, Canada.
  * 
  * The use of two 5DOF tools on a single port is supported.
  * In order to use the Aurora in this configuration, a 
  * splitter for the port is required, as well as an SROM
  * file that can work with the two tools in question.  The
  * AttachSROMFilenameToPort() method is used to associate
  * the SROM with the port.
  *
  * \ingroup Tracker
  *
  */


class AuroraTrackerNew : public TrackerNew
{
public:

  /** typedefs for the tool */
 // FIXME: this needs to be converted to the new Aurora tracker tool
  typedef igstk::AuroraTrackerTool              AuroraTrackerToolType;
  typedef AuroraTrackerToolType::Pointer        AuroraTrackerToolPointer;
  typedef AuroraTrackerToolType::ConstPointer   AuroraTrackerToolConstPointer;

  /** number of ports to allow */
  itkStaticConstMacro( NumberOfPorts, unsigned int, 4 );
  itkStaticConstMacro( NumberOfChannels, unsigned int, 2 );

  /** typedef for command interpreter */
  typedef igstk::NDICommandInterpreter   CommandInterpreterType;

  /** communication type */
  typedef igstk::SerialCommunication     CommunicationType;

  /** typedef for internal boolean return type */
  typedef TrackerNew::ResultType   ResultType;

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( AuroraTrackerNew, TrackerNew )

  /** The SetCommunication method is used to attach a communication
    * object to the tracker object. */
  void SetCommunication( CommunicationType *communication );

  /** Get the number of tools that have been detected. */
  igstkGetMacro( NumberOfTools, unsigned int );

  /** Specify an SROM file to be used with a custom tool. */
  void AttachSROMFileNameToPort( const unsigned int portNum,
                                 std::string  fileName );

protected:

  AuroraTrackerNew(void);

  virtual ~AuroraTrackerNew(void);

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
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** A mutex for multithreaded access to the buffer arrays */
  itk::MutexLock::Pointer  m_BufferLock;

  /** A buffer for holding tool transforms */
  double m_TransformBuffer[NumberOfPorts][NumberOfChannels][8];

  /** A buffer for holding status of tools */
  int m_StatusBuffer[NumberOfPorts][NumberOfChannels];

  /** A buffer for holding absent status of tools */
  int m_AbsentBuffer[NumberOfPorts][NumberOfChannels];

  /** Load a virtual SROM onto the tools.
   * Called from EnableToolPorts() */
  bool LoadVirtualSROM( const unsigned int port,
                        const std::string SROMFileName );

  /** Enable all tool ports that have tools plugged into them.
   * {The reference tool port is enabled as a static tool.} */
  void EnableToolPorts( void );

  /** Disable all enabled tool ports. */
  void DisableToolPorts( void );

  /** Helper function for reporting interpreter errors. */
  ResultType CheckError( CommandInterpreterType * );

  /** Information about which tool ports are enabled. */
  int m_PortEnabled[NumberOfPorts][NumberOfChannels];

  /** The tool handles that the device has provides us with. */
  int m_PortHandle[NumberOfPorts][NumberOfChannels];

  /** Total number of tools detected. */
  unsigned int   m_NumberOfTools;

  /** Names of the SROM files for special tools. */
  std::string    m_PortSROMFileNames[NumberOfPorts];

  /** The "Communication" instance */
  CommunicationType::Pointer       m_Communication;

  /** The baud rate to use */
  CommunicationType::BaudRateType  m_BaudRate;

  /** The command interpreter */
  CommandInterpreterType::Pointer  m_CommandInterpreter;
};

}

#endif //__igstk_AuroraTrackerNew_h_