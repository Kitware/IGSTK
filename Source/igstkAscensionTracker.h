/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAscensionTracker.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkAscensionTracker_h
#define __igstkAscensionTracker_h

#include "igstkSerialCommunication.h"
#include "igstkAscensionTrackerTool.h"
#include "igstkAscensionCommandInterpreter.h"
#include "igstkTracker.h"

namespace igstk
{
/** \class AscensionTracker
 *
 * \brief Provides support for the Ascension trackers.
 *
 * This class provides an interface to Ascension Technology 
 * Corporation magnetic trackers.
 *
 * \ingroup Tracker
 *
 */

class AscensionTracker : public Tracker
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( AscensionTracker, Tracker )

public:

  /** Command Interpreter */
  typedef igstk::AscensionCommandInterpreter CommandInterpreterType;

  /** Communication type */
  typedef igstk::SerialCommunication     CommunicationType;

  /** The SetCommunication method is used to attach a communication
    * object to the tracker object. */
  void SetCommunication( CommunicationType *communication );

  /** Get the number of tools that have been detected. */
  igstkGetMacro( NumberOfTools, unsigned int );

protected:

  AscensionTracker(void);

  virtual ~AscensionTracker(void);

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

  /** Verify tracker tool information */
  virtual ResultType VerifyTrackerToolInformation( 
    const TrackerToolType * trackerTool );

  /** Add tracker tool entry from internal containers */
  virtual ResultType AddTrackerToolToInternalDataContainers( const
    TrackerToolType * trackerTool );

  /** Remove tracker tool from internal containers */
  virtual ResultType RemoveTrackerToolFromInternalDataContainers( 
    const TrackerToolType * trackerTool );

  /** Reset the tracking device to put it back to its original state. */
  virtual ResultType InternalReset( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, ::itk::Indent indent ) const; 

private:

  AscensionTracker(const Self&);   //purposely not implemented
  void operator=(const Self&);        //purposely not implemented

  /** typedefs for the tool */
  typedef igstk::AscensionTrackerTool       AscensionTrackerToolType;
  typedef AscensionTrackerToolType::Pointer AscensionTrackerToolPointer;
  typedef AscensionTrackerToolType::ConstPointer
                                     AscensionTrackerToolConstPointer;

  /** Enable all tool ports that have tools plugged into them.
   * {The reference tool port is enabled as a static tool.} */
  void EnableToolPorts( void );

  /** Disable all enabled tool ports. */
  void DisableToolPorts( void );

  /** Total number of tools detected. */
  unsigned int   m_NumberOfTools;

  /** The "Communication" instance */
  CommunicationType::Pointer       m_Communication;

  /** The command interpreter */
  CommandInterpreterType::Pointer  m_CommandInterpreter;

  /** A mutex for multithreaded access to the transform buffer */
  itk::MutexLock::Pointer  m_BufferLock;  

  /** The buffers for holding tool transforms */
  //TransformType m_TransformBuffer[NumberOfPorts];

  /** A buffer to hold tool transforms */
  typedef std::map< std::string, std::vector < double > >
      TrackerToolTransformContainerType;

  TrackerToolTransformContainerType     m_ToolTransformBuffer;

  /** Error map container */
  typedef std::map< unsigned int, std::string>  ErrorCodeContainerType;
  static ErrorCodeContainerType   m_ErrorCodeContainer;

  /** boolean to indicate if error code list is created */
  static bool m_ErrorCodeListCreated;

  /** Container holding status of the tools */
  std::map< std::string, int >  m_ToolStatusContainer;

};

}

#endif //__igstk_AscensionTracker_h_
