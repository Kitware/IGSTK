/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFlockOfBirdsTracker.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
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
 *
 * \brief Provides support for the FlockOfBirds tracker.
 *
 * The FlockOfBirds is a magnetic tracker from Ascension.
 * This class provides an interface to the FlockOfBirds,
 * which is a magnetic tracker from Ascension Technology
 * Corporation.  Currently this class only supports the
 * tracking of a single receiver, but it would be
 * straightforward to modify it to support tracking of
 * additional receivers.
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
  typedef igstk::FlockOfBirdsTrackerTool       FlockOfBirdsTrackerToolType;
  typedef FlockOfBirdsTrackerToolType::Pointer FlockOfBirdsTrackerToolPointer;
  typedef FlockOfBirdsTrackerToolType::ConstPointer
                                     FlockOfBirdsTrackerToolConstPointer;

  /** Enable all tool ports that have tools plugged into them.
   * {The reference tool port is enabled as a static tool.} */
  void EnableToolPorts( void );

  /** Disable all enabled tool ports. */
  void DisableToolPorts( void );

  /** Information about which tool ports are enabled. */
  int m_PortEnabled[NumberOfPorts];

  /** Total number of tools detected. */
  unsigned int   m_NumberOfTools;

  /** The "Communication" instance */
  CommunicationType::Pointer       m_Communication;

  /** The command interpreter */
  CommandInterpreterType::Pointer  m_CommandInterpreter;

  /** A mutex for multithreaded access to the transform buffer */
  itk::MutexLock::Pointer  m_BufferLock;  

  /** The buffers for holding tool transforms */
  TransformType m_TransformBuffer[NumberOfPorts];
};

}

#endif //__igstk_FlockOfBirdsTracker_h_
