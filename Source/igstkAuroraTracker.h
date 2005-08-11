/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTracker.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_AuroraTracker_h_
#define __igstk_AuroraTracker_h_

#include "igstkSerialCommunication.h"
#include "igstkNDICommandInterpreter.h"
#include "igstkAuroraTool.h"
#include "igstkTracker.h"

namespace igstk
{
/** \class AuroraTracker
  * \brief Provides support for the AURORA magnetic tracker.
  *
  * The Aurora is a magnetic tracker from Northern Digital Inc.
  * in Waterloo, Ontario, Canada.  This class provides an
  * an interface to the Aurora.
*/


// the number of ports to allow
#define NDI_NUMBER_OF_PORTS  4


class AuroraTracker : public Tracker
{
public:

  /** typedefs for the tool */
  typedef igstk::AuroraTool              AuroraToolType;
  typedef AuroraToolType::Pointer        AuroraToolPointer;
  typedef AuroraToolType::ConstPointer   AuroraToolConstPointer;

  /** typedef for command interpreter */
  typedef igstk::NDICommandInterpreter   CommandInterpreterType;

  /** communication type */
  typedef igstk::SerialCommunication     CommunicationType;

  /** typedef for internal boolean return type */
  typedef Tracker::ResultType   ResultType;

  /** Some required typedefs for itk::Object. */
  typedef AuroraTracker                  Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(AuroraTracker, Tracker);

  /** Method for creation of a reference counted object. */
  igstkNewMacro(Self);  

  /** The SetCommunication method is used to attach a communication
    * object to the tracker object. */
  void SetCommunication( CommunicationType *communication );

  /** Get the number of tools that have been detected. */
  igstkGetMacro( NumberOfTools, unsigned int );

  /** Specify an SROM file to be used with a passive or custom tool. */
  void AttachSROMFileNameToPort( const int portNum, std::string fileName );

protected:

  AuroraTracker(void);

  virtual ~AuroraTracker(void);

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

  /** Reset the tracking device to put it back to its original state. */
  virtual ResultType InternalReset( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Load a virtual SROM, given the file name of the ROM file */
  bool LoadVirtualSROM( const int i, std::string SROMFileName) ;

  /** Clear the virtual SROM for a tool */
  void ClearVirtualSROM( int tool );

  /** Enable all tool ports that have tools plugged into them.
   * {The reference tool port is enabled as a static tool.} */
  void EnableToolPorts( void );

  /** Disable all enabled tool ports. */
  void DisableToolPorts( void );

  /** Find the tool for a specific port handle (-1 if not found). */
  int GetToolFromHandle( int handle );

  /** Information about which tool ports are enabled. */
  int m_PortEnabled[NDI_NUMBER_OF_PORTS];

  /** The tool handles that the device has provides us with. */
  int m_PortHandle[NDI_NUMBER_OF_PORTS];

  /** Total number of tools detected. */
  unsigned int   m_NumberOfTools;

  /** Names of the SROM files for passive tools and custom tools. */
  std::string    m_SROMFileNames[NDI_NUMBER_OF_PORTS];

  /** The "Communication" instance */
  CommunicationType::Pointer       m_Communication;

  /** The command interpreter */
  CommandInterpreterType::Pointer  m_CommandInterpreter;
};

}

#endif //__igstk_AuroraTracker_h_
