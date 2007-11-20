/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisTrackerNew.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkPolarisTrackerNew_h
#define __igstkPolarisTrackerNew_h

#include "igstkSerialCommunication.h"
#include "igstkNDICommandInterpreter.h"
#include "igstkPolarisTrackerToolNew.h"
#include "igstkTrackerNew.h"

namespace igstk
{
/** \class PolarisTracker
  * \brief Provides support for the Polaris optical tracker.
  *
  * This class provides an interface to the Polaris line of
  * optical tracking systems, including the Vicra and the
  * Spectra.  These devices are manufactured by
  * Northern Digital Inc. of Waterloo, Ontario, Canada.
  *
  * IMPORTANT NOTE ABOUT PASSIVE TOOLS: the mapping that this
  * class provides between igstk::Tracker ports and the actual
  * device ports on the Polaris can be confusing.  Tracker ports
  * numbered 0, 1, and 2 map to wired Polaris ports 1, 2 and 3.
  * Note that the Vicra does not have wired ports, and Tracker
  * ports 0, 1, 2 are completely inactive when the Vicra is used.
  * Tracker ports numbered 3 and above are for use with passive
  * (wireless) tools.  To use passive tools, you must call the
  * PolarisTracker::AttachSROMFilenameToPort() method to associate
  * an SROM with a port that is numbered between 3 and 11.
  *
  * Tracking of the silvered markers individually is not possible,
  * the markers must be configured as a tool before they can
  * be tracked.
  *
  * \ingroup Tracker
  *
  */

class PolarisTrackerNew : public TrackerNew
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( PolarisTrackerNew, TrackerNew )

public:

  /** typedefs for the tool */
  /** FIXME: this needs to be converted to the new TOOL */
  typedef igstk::PolarisTrackerToolNew           PolarisTrackerToolType;
  typedef PolarisTrackerToolType::Pointer        PolarisTrackerToolPointer;
  typedef PolarisTrackerToolType::ConstPointer   PolarisTrackerToolConstPointer;

  /** number of ports to allow */
  itkStaticConstMacro( NumberOfPorts, unsigned int, 12 );

  /** communication type */
  typedef igstk::SerialCommunication     CommunicationType;

  /** The SetCommunication method is used to attach a communication
    * object to the tracker object. */
  void SetCommunication( CommunicationType *communication );

protected:

  PolarisTrackerNew(void);

  virtual ~PolarisTrackerNew(void);

  /** typedef for command interpreter */
  typedef igstk::NDICommandInterpreter   CommandInterpreterType;

  /** typedef for internal boolean return type */
  typedef TrackerNew::ResultType   ResultType;

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

  /** Verify tracker tool information */
  virtual ResultType VerifyTrackerToolInformation( TrackerToolType * );

  /** Remove tracker tool entry from internal containers */ 
  virtual ResultType RemoveTrackerToolFromInternalDataContainers(
                                     std::string trackerToolIdentifier ); 

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const; 

private:

  PolarisTrackerNew(const Self&);   //purposely not implemented
  void operator=(const Self&);   //purposely not implemented

  /** A mutex for multithreaded access to the buffer arrays */
  ::itk::MutexLock::Pointer  m_BufferLock;

  /** A buffer for holding tool transforms
 *   DEPRECATED */
  
  double m_TransformBuffer[NumberOfPorts][8];

  /** A buffer for holding status of tools */
  int m_StatusBuffer[NumberOfPorts];

  /** A buffer for holding absent status of tools */
  int m_AbsentBuffer[NumberOfPorts];

  /** Helper function for reporting interpreter errors. */
  ResultType CheckError( CommandInterpreterType * );

  /** The "Communication" instance */
  CommunicationType::Pointer       m_Communication;

  /** The baud rate to use */
  CommunicationType::BaudRateType  m_BaudRate;

  /** The command interpreter */
  CommandInterpreterType::Pointer  m_CommandInterpreter;

  /** Port handle container indexed by the tracker tool unique 
   * identifier */
  std::map< std::string, int >     m_PortHandleContainer;

  /** Container holding absent status of tools */
  std::map< std::string, int >     m_ToolAbsentStatusContainer; 

  /** Container holding status of the tools */
  std::map< std::string, int >  m_ToolStatusContainer; 

  /** A buffer to hold tool transforms */
  typedef std::map< std::string , std::vector < double > > 
                                TrackerToolTransformContainerType; 

  TrackerToolTransformContainerType     m_ToolTransformBuffer;

};

}

#endif //__igstk_PolarisTrackerNew_h_
