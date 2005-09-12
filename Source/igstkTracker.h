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

#ifndef __igstk_Tracker_h_
#define __igstk_Tracker_h_

#include <vector>

#include "itkObject.h"
#include "itkLogger.h"
#include "itkMutexLock.h"
#include "itkMultiThreader.h"
#include "itkVersorTransform.h"

#include "igstkStateMachine.h"
#include "igstkTrackerPort.h"
#include "igstkTransform.h"
#include "igstkSpatialObject.h"
#include "igstkPulseGenerator.h"

namespace igstk
{
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
 */

class Tracker : public itk::Object
{

public:
  
  /** typedef for LoggerType */
  typedef itk::Logger                    LoggerType;

  /** typedefs from igstk::TrackerPort class */
  typedef igstk::TrackerTool             TrackerToolType;

  /** typedefs from igstk::TrackerTool class */
  typedef Transform                TransformType;
  typedef double                   ErrorType;

  /** typedefs for PatientTransform */
  typedef Transform                      PatientTransformType;

  /** typedefs for ToolCalibrationTransform */
  typedef Transform                               ToolCalibrationTransformType;

  /** Some required typedefs for itk::Object. */
  typedef Tracker                        Self;
  typedef itk::Object                    Superclass;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  typedef igstk::TrackerPort                TrackerPortType;
  typedef TrackerPortType::Pointer          TrackerPortPointer;
  typedef std::vector< TrackerPortPointer > TrackerPortVectorType;

  typedef igstk::TrackerTool             TrackerToolType;
  typedef TrackerToolType::Pointer       TrackerToolPointer;
  typedef TrackerToolType::ConstPointer  TrackerToolConstPointer;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(Tracker, Object);

  /** Method for creation of a reference counted object. */
  igstkNewMacro(Self);  

  /** The "Open" method attempts to open communication with the tracking device. */
  void Open( void );

  /** The "Close" method closes communication with the device. */
  void Close( void );

  /** The "Initialize" method initializes a newly opened device. */
  void Initialize( void );

  /** The "Reset" tracker method should be used to bring the tracker
  to some defined default state. */
  void Reset( void );

  /** The "StartTracking" method readies the tracker for tracking the
  tools connected to the tracker. */
  void StartTracking( void );

  /** The "StopTracking" stops tracker from tracking the tools. */
  void StopTracking( void );

  /** The "UpdateStatus" method is used for updating the status of 
  ports and tools when the tracker is in tracking state. */
  void UpdateStatus( void );

  /** The "GetToolTransform" gets the position of tool numbered "toolNumber" on
   * port numbered "portNumber" in the variable "position". Note that this
   * variable represents the position and orientation of the tool in 3D space.
   * */
  void GetToolTransform( unsigned int portNumber, unsigned int toolNumber,
                         TransformType &position ) const;

  /** Associate a TrackerTool to an object to be tracked. This is a one-to-one
   * association and cannot be changed during the life of the application */
  void AttachObjectToTrackerTool( unsigned int portNumber, unsigned int toolNumber,
                                  SpatialObject * objectToTrack );

  /** The "SetReferenceTool" sets the reference tool. */
  void SetReferenceTool( bool applyReferenceTool, unsigned int portNumber,
                         unsigned int toolNumber );

  /** The "GetReferenceTool" gets the reference tool.
   * If the reference tool is not applied, it returns false.
   * Otherwise, it returns true. */
  bool GetReferenceTool( unsigned int &portNumber, unsigned int &toolNumber ) const;

  /** The "SetPatientTransform" sets PatientTransform.

    T ' = W * R^-1 * T * C

    where:
    " T " is the original tool transform reported by the device,
    " R^-1 " is the inverse of the transform for the reference tool,
    " W " is the Patient transform (it specifies the position of the reference
    with respect to patient coordinates), and
    " T ' " is the transformation that is reported to the spatial objects
    " C " is the tool calibration transform.
  */
  void SetPatientTransform( const PatientTransformType& _arg );

  /** The "GetPatientTransform" gets PatientTransform. */
  PatientTransformType GetPatientTransform() const; 

  /** The "SetToolCalibrationTransform" sets the tool calibration transform */
  void SetToolCalibrationTransform( const ToolCalibrationTransformType& _arg );

  /** The "GetToolCalibrationTransform" gets the tool calibration transform */
  ToolCalibrationTransformType GetToolCalibrationTransform() const;

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();
  /** Declarations needed for the Logger */
  igstkLoggerMacro();

protected:

  typedef enum 
  { 
    FAILURE=0, 
    SUCCESS
  } ResultType;

  /** itk::MutexLock object pointer */
  itk::MutexLock::Pointer         m_TrackingThreadLock;

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
  void SetToolTransform( unsigned int portNumber, unsigned int toolNumber, const TransformType & position );

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

  /** ToolCalibration Transform */
  ToolCalibrationTransformType    m_ToolCalibrationTransform;

  /** Multi-threading enabled flag : The decendent class will use multi-threading,
      if this flag is set as true */
  bool                            m_ThreadingEnabled;

  /** itk::MultiThreader object pointer */
  itk::MultiThreader::Pointer     m_Threader;

  /** Tracking ThreadID */
  int                             m_ThreadID;

  /** List of States */
  StateType                m_IdleState;
  StateType                m_AttemptingToEstablishCommunicationState;
  StateType                m_AttemptingToCloseCommunicationState;
  StateType                m_CommunicationEstablishedState;
  StateType                m_AttemptingToActivateToolsState;
  StateType                m_ToolsActiveState;
  StateType                m_AttemptingToTrackState;
  StateType                m_TrackingState;
  StateType                m_AttemptingToStopTrackingState;

  /** List of Inputs */
  InputType                m_EstablishCommunicationInput;
  InputType                m_CommunicationEstablishmentSuccessInput;
  InputType                m_CommunicationEstablishmentFailureInput;

  InputType                m_ActivateToolsInput;
  InputType                m_ToolsActivationSuccessInput;
  InputType                m_ToolsActivationFailureInput;

  InputType                m_StartTrackingInput;
  InputType                m_StartTrackingSuccessInput;
  InputType                m_StartTrackingFailureInput;

  InputType                m_UpdateStatusInput;

  InputType                m_StopTrackingInput;
  InputType                m_StopTrackingSuccessInput;
  InputType                m_StopTrackingFailureInput;

  InputType                m_ResetInput;

  InputType                m_CloseCommunicationInput;
  InputType                m_CloseCommunicationSuccessInput;
  InputType                m_CloseCommunicationFailureInput;

  /** Thread function for tracking */
  static ITK_THREAD_RETURN_TYPE TrackingThreadFunction(void* pInfoStruct);

  /** The "AttemptToOpen" method attempts to open communication with a
      tracking device. */
  void AttemptToOpen( void );
  
  /** The "AttemptToActivateTools" method attempts to activate tools. */
  void AttemptToActivateTools( void );

  /** The "AttemptToStartTracking" method attempts to start tracking. */
  void AttemptToStartTracking( void );

  /** The "AttemptToStopTracking" method attempts to stop tracking. */
  void AttemptToStopTracking( void );

  /** The "AttemptToUpdateStatus" method attempts to update status
      during tracking. */
  void AttemptToUpdateStatus( void );

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
