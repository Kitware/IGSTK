/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerNew.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTrackerNew_h
#define __igstkTrackerNew_h

#include <vector>

#include "itkMutexLock.h"
#include "itkConditionVariable.h"
#include "itkMultiThreader.h"
#include "itkVersorTransform.h"

#include "igstkObject.h"
#include "igstkStateMachine.h"
#include "igstkTrackerPort.h"
#include "igstkTransform.h"
#include "igstkAxesObject.h"
#include "igstkPulseGenerator.h"

namespace igstk
{

itkEventMacro( TrackerEvent,                               StringEvent);
itkEventMacro( TrackerErrorEvent,                          TrackerEvent);

itkEventMacro( TrackerOpenEvent,                           TrackerEvent);
itkEventMacro( TrackerOpenErrorEvent,                      TrackerErrorEvent);

itkEventMacro( TrackerCloseEvent,                          TrackerEvent);
itkEventMacro( TrackerCloseErrorEvent,                     TrackerErrorEvent);

itkEventMacro( TrackerInitializeEvent,                     TrackerEvent);
itkEventMacro( TrackerInitializeErrorEvent,                TrackerErrorEvent);

itkEventMacro( TrackerStartTrackingEvent,                  TrackerEvent);
itkEventMacro( TrackerStartTrackingErrorEvent,             TrackerErrorEvent);

itkEventMacro( TrackerStopTrackingEvent,                   TrackerEvent);
itkEventMacro( TrackerStopTrackingErrorEvent,              TrackerErrorEvent);

itkEventMacro( TrackerUpdateStatusEvent,                   TrackerEvent);
itkEventMacro( TrackerUpdateStatusErrorEvent,              TrackerErrorEvent);


/** \class Tracker
 *  \brief Superclass for concrete IGSTK Tracker classes.
 *
 *  This class presents a generic interface for tracking the
 *  positions of objects in IGSTK.  The various derived
 *  subclasses of this class provide back-ends that communicate
 *  with several of the tracking systems that are available on
 *  the market.
 *
 *  The state machine of this class implements the basic
 *  state transitions of a tracker.  Inputs to the state
 *  machine are translated into method calls that can be
 *  overridden by device-specific derive classes that do
 *  the appropriate processing for a particular device.
 *
 *  Most (but not all) of the derived classes utilize a
 *  communication object to mediate all communication between
 *  the computer and the device.  When a communication object
 *  is used, all communication can be logged, and furthermore,
 *  the communication log can be used to drive an offline
 *  simulation of a particular device 
 *  (See SerialCommunicationSimulator).
 *
 *  The following diagram illustrates the state machine of 
 *  the tracker class
 *
 *  \image html  igstkTrackerNew.png  "TrackerNew State Machine Diagram"
 *  \image latex igstkTrackerNew.eps  "TrackerNew State Machine Diagram" 
 *
 *  \ingroup TrackerNew
 */

class TrackerNew : public Object
{

public:
  
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( TrackerNew, Object )

public:

  /** typedef for times used by the tracker */
  typedef Transform::TimePeriodType      TimePeriodType;

  /** typedefs from igstk::TrackerTool class */
  typedef Transform                      TransformType;
  typedef double                         ErrorType;

  /** typedefs for PatientTransform */
  typedef Transform                      PatientTransformType;

  /** typedefs for ToolCalibrationTransform */
  typedef Transform                      ToolCalibrationTransformType;

  /** typedefs from igstk::TrackerTool class */
  typedef igstk::TrackerTool                TrackerToolType;
  typedef TrackerToolType::Pointer          TrackerToolPointer;
  typedef TrackerToolType::ConstPointer     TrackerToolConstPointer;
  typedef std::vector< TrackerToolPointer > TrackerToolVectorType;

  /** typedefs for the TrackerPort class */
  typedef igstk::TrackerPort                TrackerPortType;
  typedef TrackerPortType::Pointer          TrackerPortPointer;
  typedef std::vector< TrackerPortPointer > TrackerPortVectorType; // FIXME: DEPRECATED: Ports are specific to Polaris and Aurora.

  /** typedefs for the coordinate reference system */
  typedef AxesObject                        CoordinateReferenceSystemType;
  
  /** The "RequestOpen" method attempts to open communication with the 
   *  tracking device. It generates a TrackerOpenEvent if successful,
   *  or a TrackerOpenErrorEvent if not successful.  */
  void RequestOpen( void );

  /** The "RequestClose" method closes communication with the device. 
   *  It generates a TrackerCloseEvent if successful,
   *  or a TrackerCloseErrorEvent if not successful. */
  void RequestClose( void );

  /** The "RequestInitialize" method initializes a newly opened device. */
  void RequestInitialize( void );

  /** The "RequestReset" tracker method should be used to bring the tracker
  to some defined default state. */
  void RequestReset( void );

  /** The "RequestStartTracking" method readies the tracker for tracking the
  tools connected to the tracker. */
  void RequestStartTracking( void );

  /** The "RequestStopTracking" stops tracker from tracking the tools. */
  void RequestStopTracking( void );
  
  /** The "RequestUpdateStatus" method is used for updating the status of 
  ports and tools when the tracker is in tracking state. */
  void RequestUpdateStatus( void );

  /** Add a tracker tool to this tracker 
   * FIXME: This has yet to be inserted in the State Machine. */
  void RequestAddTool( TrackerToolType * trackerTool );

  /** Set the time period over which a tool transform should be considered
   *  valid. */
  igstkSetMacro( ValidityTime, TimePeriodType );

  /** Get the validity time. */
  igstkGetMacro( ValidityTime, TimePeriodType );


protected:

  typedef enum 
    { 
    FAILURE=0, 
    SUCCESS
    } ResultType;

  TrackerNew(void);

  virtual ~TrackerNew(void);

  /** SetThreadingEnabled(bool) : set m_ThreadingEnabled value */
  igstkSetMacro( ThreadingEnabled, bool );

  /** GetThreadingEnabled(bool) : get m_ThreadingEnabled value  */
  igstkGetMacro( ThreadingEnabled, bool );

  /** The "InternalOpen" method opens communication with a tracking device.
      This method is to be overridden by a descendant class 
      and responsible for device-specific processing */
  virtual ResultType InternalOpen( void );

  /** The "InternalClose" method closes communication with a tracking device.
      This method is to be overridden by a descendant class 
      and responsible for device-specific processing */
  virtual ResultType InternalClose( void );

  /** The "InternalReset" method resets tracker to a known configuration. 
      This method is to be overridden by a descendant class 
      and responsible for device-specific processing */
  virtual ResultType InternalReset( void );

  /** The "InternalActivateTools" method activates tools.
      This method is to be overridden by a descendant class 
      and responsible for device-specific processing */
  virtual ResultType InternalActivateTools( void );

  /** The "InternalDeactivateTools" method deactivates tools.
      This method is to be overridden by a descendant class 
      and responsible for device-specific processing */
  virtual ResultType InternalDeactivateTools( void );

  /** The "InternalStartTracking" method starts tracking.
      This method is to be overridden by a descendant class 
      and responsible for device-specific processing */
  virtual ResultType InternalStartTracking( void );

  /** The "InternalStopTracking" method stops tracking.
      This method is to be overridden by a descendant class 
      and responsible for device-specific processing */
  virtual ResultType InternalStopTracking( void );

  /** The "InternalUpdateStatus" method updates tracker status.
      This method is to be overridden by a descendant class 
      and responsible for device-specific processing */
  virtual ResultType InternalUpdateStatus( void );

  /** The "InternalThreadedUpdateStatus" method updates tracker status.
      This method is called in a separate thread.
      This method is to be overridden by a descendant class
      and responsible for device-specific processing */
  virtual ResultType InternalThreadedUpdateStatus( void );

  /** The "AddPort" method adds a port to the tracker. */
  void AddPort( TrackerPortType * port);

  /** The "ClearPorts" clears all the ports. */
  void ClearPorts( void );

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Get reference system. This is the reference frame of the Tracker itself.
   * Tracker tools need access to this reference system in order to use it
   * as parent in the scene graph. */
  const CoordinateReferenceSystemType * GetCoordinateReferenceSystem() const;
 
private:

  /** Pulse generator for driving the rate of tracker updates. */
  PulseGenerator::Pointer   m_PulseGenerator;
  
  /** Pulse observer for receiving the events from the pulse generator. */
  typedef itk::SimpleMemberCommand< Self >   ObserverType;
  ObserverType::Pointer     m_PulseObserver;

  /** Vector of all tool ports on the tracker */
  // FIXME: THIS IS DEPRECATED. Ports are only for Polaris and Aurora and are
  // now contained in their corresponding tracker tools.
  TrackerPortVectorType     m_Ports; // DEPRECATED

  // NEW: an array of tracker tools replace the array of ports
  TrackerToolVectorType     m_TrackerTools;
  
  /** The reference tool */
  bool                      m_ApplyingReferenceTool;
  TrackerToolPointer        m_ReferenceTool;
  unsigned int              m_ReferenceToolPortNumber;
  unsigned int              m_ReferenceToolNumber;

  /** Coordinate Reference System */
  CoordinateReferenceSystemType::Pointer    m_CoordinateReferenceSystem;

  /** Validity time */
  TimePeriodType            m_ValidityTime;

  /** Multi-threading enabled flag : The descendant class will use
      multi-threading, if this flag is set as true */
  bool                            m_ThreadingEnabled;

  /** itk::MultiThreader object pointer */
  itk::MultiThreader::Pointer     m_Threader;

  /** Tracking ThreadID */
  int                             m_ThreadID;

  /** itk::ConditionVariable object pointer to signal for the next
   *  transform */
  itk::ConditionVariable::Pointer m_ConditionNextTransformReceived;
  
  /** itk::SimpleMutexLock object to be used for
      m_ConditionNextTransformReceived */
  itk::SimpleMutexLock            m_LockForConditionNextTransformReceived;

  /** List of States */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( AttemptingToEstablishCommunication );
  igstkDeclareStateMacro( AttemptingToCloseCommunication );
  igstkDeclareStateMacro( CommunicationEstablished );
  igstkDeclareStateMacro( AttemptingToActivateTools );
  igstkDeclareStateMacro( ToolsActive );
  igstkDeclareStateMacro( AttemptingToTrack );
  igstkDeclareStateMacro( Tracking );
  igstkDeclareStateMacro( AttemptingToUpdate );
  igstkDeclareStateMacro( AttemptingToStopTracking );

  /** List of Inputs */
  igstkDeclareInputMacro( EstablishCommunication );
  igstkDeclareInputMacro( ActivateTools );
  igstkDeclareInputMacro( StartTracking );
  igstkDeclareInputMacro( UpdateStatus );
  igstkDeclareInputMacro( StopTracking );
  igstkDeclareInputMacro( Reset );
  igstkDeclareInputMacro( CloseCommunication );

  igstkDeclareInputMacro( Success );
  igstkDeclareInputMacro( Failure );

  /** Thread function for tracking */
  static ITK_THREAD_RETURN_TYPE TrackingThreadFunction(void* pInfoStruct);

  /** The "AttemptToOpenProcessing" method attempts to open communication with a
      tracking device. */
  void AttemptToOpenProcessing( void );
  
  /** The "AttemptToActivateToolsProcessing" method attempts 
   *  to activate tools. */
  void AttemptToActivateToolsProcessing( void );

  /** The "AttemptToStartTrackingProcessing" method attempts 
   *  to start tracking. */
  void AttemptToStartTrackingProcessing( void );

  /** The "AttemptToStopTrackingProcessing" method attempts to stop tracking. */
  void AttemptToStopTrackingProcessing( void );

  /** The "AttemptToUpdateStatusProcessing" method attempts to update status
      during tracking. */
  void AttemptToUpdateStatusProcessing( void );

  /** The "UpdateStatusFailureProcessing" method is called when an
      attempt to update failes. */
  void UpdateStatusSuccessProcessing( void );

  /** The "UpdateStatusFailureProcessing" method is called when an
      attempt to update failes. */
  void UpdateStatusFailureProcessing( void );

  /** The "CloseFromTrackingStateProcessing" method closes tracker in
      use, when the tracker is in tracking state. */
  void CloseFromTrackingStateProcessing( void );

  /** The "CloseFromToolsActiveStateProcessing" method closes tracker
      in use, when the tracker is in active tools state. */
  void CloseFromToolsActiveStateProcessing( void);

  /** The "CloseFromCommunicatingStateProcessing" method closes
      tracker in use, when the tracker is in communicating state. */
  void CloseFromCommunicatingStateProcessing( void );

  /** The "ResetFromTrackingStateProcessing" method resets tracker in
      use, when the tracker is in tracking state. */
  void ResetFromTrackingStateProcessing( void );

  /** The "ResetFromToolsActiveStateProcessing" method resets tracker
      in use, when the tracker is in active tools state. */
  void ResetFromToolsActiveStateProcessing( void);

  /** The "ResetFromCommunicatingStateProcessing" method resets
      tracker in use, when the tracker is in communicating state. */
  void ResetFromCommunicatingStateProcessing( void );

  /** Post-processing after communication setup has been successful. */ 
  void CommunicationEstablishmentSuccessProcessing( void );

  /** Post-processing after communication setup has failed. */ 
  void CommunicationEstablishmentFailureProcessing( void );

  /** Post-processing after ports and tools setup has been successful. */ 
  void ToolsActivationSuccessProcessing( void );

  /** Post-processing after ports and tools setup has failed. */ 
  void ToolsActivationFailureProcessing( void );

  /** Post-processing after start tracking has been successful. */ 
  void StartTrackingSuccessProcessing( void );

  /** Post-processing after start tracking has failed. */ 
  void StartTrackingFailureProcessing( void );

  /** Post-processing after stop tracking has been successful. */ 
  void StopTrackingSuccessProcessing( void );

  /** Post-processing after start tracking has failed. */ 
  void StopTrackingFailureProcessing( void );

  /** Post-processing after close tracking has been successful. */ 
  void CloseCommunicationSuccessProcessing( void );

  /** Post-processing after close tracking has failed. */ 
  void CloseCommunicationFailureProcessing( void );

  /** Always called when entering tracking state. */
  void EnterTrackingStateProcessing( void );

  /** Always called when entering tracking state. */
  void ExitTrackingStateProcessing( void );

};

}

#endif //__igstk_TrackerNew_h_
