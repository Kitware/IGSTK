/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTracker.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTracker_h
#define __igstkTracker_h

#include <vector>

#include "itkMutexLock.h"
#include "itkConditionVariable.h"
#include "itkMultiThreader.h"
#include "itkVersorTransform.h"

#include "igstkObject.h"
#include "igstkStateMachine.h"
#include "igstkTrackerPort.h"
#include "igstkTransform.h"
#include "igstkSpatialObject.h"
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
 *  \brief Generic implementation of the Tracker class.
 *
 *  This class provides a generic implementation of a tracker
 *  class. It has two member variables, an instant of a state
 *  machine, and reference to an instance of "communication"
 *  class. 
 *
 *  The state machine implements the basic state transitions
 *  of a tracker.
 *
 *  The communications object manages communication, either
 *  through serial/parallel ports, or through data files (for
 *  offline execution of tracker.)
 *
 *  The following diagram illustrates the state machine of 
 *  the tracker class
 *
 *  \image html  igstkTracker.png  "Tracker State Machine Diagram"
 *  \image latex igstkTracker.eps  "Tracker State Machine Diagram" 
 *
 */

class Tracker : public Object
{

public:
  
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( Tracker, Object )

public:

  /** typedefs from igstk::TrackerTool class */
  typedef Transform                      TransformType;
  typedef double                         ErrorType;

  /** typedefs for PatientTransform */
  typedef Transform                      PatientTransformType;

  /** typedefs for ToolCalibrationTransform */
  typedef Transform                      ToolCalibrationTransformType;

  /** typedefs from igstk::TrackerTool class */
  typedef igstk::TrackerTool             TrackerToolType;
  typedef TrackerToolType::Pointer       TrackerToolPointer;
  typedef TrackerToolType::ConstPointer  TrackerToolConstPointer;

  /** typedefs for the TrackerPort class */
  typedef igstk::TrackerPort                TrackerPortType;
  typedef TrackerPortType::Pointer          TrackerPortPointer;
  typedef std::vector< TrackerPortPointer > TrackerPortVectorType;

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

  /** The "GetToolTransform" gets the position of tool numbered "toolNumber" on
   * port numbered "portNumber" in the variable "position". Note that this
   * variable represents the position and orientation of the tool in 3D space.
   * */
  void GetToolTransform( unsigned int portNumber, unsigned int toolNumber,
                         TransformType &position ) const;

  /** Associate a TrackerTool to an object to be tracked. This is a one-to-one
   * association and cannot be changed during the life of the application */
  void AttachObjectToTrackerTool( unsigned int portNumber,
                                  unsigned int toolNumber,
                                  SpatialObject * objectToTrack );

  /** The "SetReferenceTool" sets the reference tool. */
  void SetReferenceTool( bool applyReferenceTool, unsigned int portNumber,
                         unsigned int toolNumber );

  /** The "GetReferenceTool" gets the reference tool.
   * If the reference tool is not applied, it returns false.
   * Otherwise, it returns true. */
  bool GetReferenceTool( unsigned int &portNumber,
                         unsigned int &toolNumber ) const;

  /** The "SetPatientTransform" sets PatientTransform.

    T ' = P * R^-1 * T * C

    where:
    " T " is the original tool transform reported by the device,
    " R^-1 " is the inverse of the transform for the reference tool,
    " P " is the Patient transform (it specifies the position of the reference
    with respect to patient coordinates), and
    " T ' " is the transformation that is reported to the spatial objects
    " C " is the tool calibration transform.
  */
  void SetPatientTransform( const PatientTransformType& _arg );

  /** The "GetPatientTransform" gets PatientTransform. */
  PatientTransformType GetPatientTransform() const; 

  /** The "SetToolCalibrationTransform" sets the tool calibration transform */
  void SetToolCalibrationTransform( unsigned int portNumber,
                                    unsigned int toolNumber,
                                    const ToolCalibrationTransformType& t );

  /** Get the tool calibration transform. */
  ToolCalibrationTransformType GetToolCalibrationTransform(
                             unsigned int portNumber,
                             unsigned int toolNumber) const;

protected:

  typedef enum 
    { 
    FAILURE=0, 
    SUCCESS
    } ResultType;

  Tracker(void);

  virtual ~Tracker(void);

  /** SetThreadingEnabled(bool) : set m_ThreadingEnabled value */
  igstkSetMacro( ThreadingEnabled, bool );

  /** GetThreadingEnabled(bool) : get m_ThreadingEnabled value  */
  igstkGetMacro( ThreadingEnabled, bool );

  /** The "InternalOpen" method opens communication with a tracking device.
      This method is to be overriden by a decendent class 
      and responsible for device-specific processing */
  virtual ResultType InternalOpen( void );

  /** The "InternalClose" method closes communication with a tracking device.
      This method is to be overriden by a decendent class 
      and responsible for device-specific processing */
  virtual ResultType InternalClose( void );

  /** The "InternalReset" method resets tracker to a known configuration. 
      This method is to be overriden by a decendent class 
      and responsible for device-specific processing */
  virtual ResultType InternalReset( void );

  /** The "InternalActivateTools" method activates tools.
      This method is to be overriden by a decendent class 
      and responsible for device-specific processing */
  virtual ResultType InternalActivateTools( void );

  /** The "InternalDeactivateTools" method deactivates tools.
      This method is to be overriden by a decendent class 
      and responsible for device-specific processing */
  virtual ResultType InternalDeactivateTools( void );

  /** The "InternalStartTracking" method starts tracking.
      This method is to be overriden by a decendent class 
      and responsible for device-specific processing */
  virtual ResultType InternalStartTracking( void );

  /** The "InternalStopTracking" method stops tracking.
      This method is to be overriden by a decendent class 
      and responsible for device-specific processing */
  virtual ResultType InternalStopTracking( void );

  /** The "InternalUpdateStatus" method updates tracker status.
      This method is to be overriden by a decendent class 
      and responsible for device-specific processing */
  virtual ResultType InternalUpdateStatus( void );

  /** The "InternalThreadedUpdateStatus" method updates tracker status.
      This method is called in a separate thread.
      This method is to be overriden by a decendent class
      and responsible for device-specific processing */
  virtual ResultType InternalThreadedUpdateStatus( void );

  /** The "AddPort" method adds a port to the tracker. */
  void AddPort( TrackerPortType * port);

  /** The "ClearPorts" clears all the ports. */
  void ClearPorts( void );

  /** The "SetToolTransform" sets the position of tool numbered "toolNumber" on
   * port numbered "portNumber" by the content of variable "position". Note
   * that this variable represents the position and orientation of the tool in
   * 3D space.  */
  void SetToolTransform( unsigned int portNumber, unsigned int toolNumber,
                         const TransformType & position );

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Pulse generator for driving the rate of tracker updates. */
  PulseGenerator::Pointer   m_PulseGenerator;
  
  /** Pulse observer for receiving the events from the pulse generator. */
  typedef itk::SimpleMemberCommand< Self >   ObserverType;
  ObserverType::Pointer     m_PulseObserver;

  /** Vector of all tool ports on the tracker */
  TrackerPortVectorType     m_Ports;
  
  /** The reference tool */
  bool                      m_ApplyingReferenceTool;
  TrackerToolPointer        m_ReferenceTool;
  unsigned int              m_ReferenceToolPortNumber;
  unsigned int              m_ReferenceToolNumber;

  /** Patient Transform */
  PatientTransformType      m_PatientTransform;

  /** Multi-threading enabled flag : The decendent class will use
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

#endif //__igstk_Tracker_h_
