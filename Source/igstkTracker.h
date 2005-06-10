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

#include "igstkCommunication.h"
#include "igstkStateMachine.h"
#include "igstkTrackerPort.h"
#include "igstkTransform.h"
#include "igstkSpatialObject.h"
#include "igstkPulseGenerator.h"
#include "itkVersorTransform.h"

namespace igstk
{
/** \class Tracker
    \brief Generic implementation of the Tracker class.

    This class provides a generic implementation of a tracker
    class. It has two member variables, an instant of a state
    machine, and reference to an instance of "communication"
    class. 

    The state machine implements the basic state transitions
    of a tracker.

    The communications object manages communication, either
    through serial/parallel ports, or through data files (for
    offline execution of tracker.)
*/

class Tracker : public itk::Object
{

public:
  
  typedef igstk::Communication           CommunicationType;
  typedef itk::Logger                    LoggerType;

  /* typedefs from igstk::TrackerPort class */
  typedef igstk::TrackerTool             TrackerToolType;

  /* typedefs from igstk::TrackerTool class */
  typedef Transform                TransformType;
  typedef double                   ErrorType;

  /* typedefs for WorldTransform */
  typedef itk::VersorTransform<double>       WorldTransformType;
  typedef WorldTransformType::Pointer  WorldTransformPointer;

  /* typedefs for ToolCalibrationTransform */
  typedef itk::Transform<double>       ToolCalibrationTransformType;
  typedef ToolCalibrationTransformType::Pointer  ToolCalibrationTransformPointer;

  /** Some required typedefs for itk::Object. */

  typedef Tracker                        Self;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  typedef igstk::TrackerPort             TrackerPortType;
  typedef TrackerPortType::Pointer       TrackerPortPointer;
  typedef std::vector< TrackerPortPointer > TrackerPortVectorType;

  typedef igstk::TrackerTool             TrackerToolType;
  typedef TrackerToolType::Pointer       TrackerToolPointer;
  typedef TrackerToolType::ConstPointer  TrackerToolConstPointer;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro(Tracker, Object);

  /** Method for creation of a reference counted object. */
  igstkNewMacro(Self);  

  /** The SetLogger method is used to attach a logger object to the
  tracker object for logging purposes. */
  void SetLogger( LoggerType * logger );

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

  /** The "EndTracking" stops tracker from tracking the tools. */
  void StopTracking( void );

  /** The "UpdateStatus" method is used for updating the status of 
  ports and tools when the tracker is in tracking state. */
  void UpdateStatus( void );

  /** The "GetToolTransform" gets the position of tool numbered "toolNumber" on
   * port numbered "portNumber" in the variable "position". Note that this
   * variable represents the position and orientation of the tool in 3D space.
   * */
  void GetToolTransform( unsigned int portNumber, unsigned int toolNumber, TransformType &position ) const;

  /** Associate a TrackerTool to an object to be tracked. This is a one-to-one
   * association and cannot be changed during the life of the application */
  void AttachObjectToTrackerTool( unsigned int portNumber, unsigned int toolNumber, SpatialObject * objectToTrack );

  /** The "SetReferenceTool" sets the reference tool. */
//  void SetReferenceTool( bool applyReferenceTool, unsigned int portNumber, unsigned int toolNumber );

  /** The "GetReferenceTool" gets the reference tool.
   * If the reference tool is not applied, it returns false.
   * Otherwise, it returns true. */
//  bool GetReferenceTool( unsigned int &portNumber, unsigned int &toolNumber ) const;

  /** The "SetWorldTransform" sets WorldTransform.

    T ' = W * R^-1 * T

    where:
    " T " is the original tool transform reported by the device,
    " R^-1 " is the inverse of the transform for the reference tool,
    " W " is the world transform (it specifies the position of the reference
    with respect to patient coordinates), and
    " T ' " is the transformation that is reported to the spatial objects
  */
//  void SetWorldTransform( WorldTransformType* _arg );

//  void SetToolCalibrationTransform( ToolCalibrationTransformType* _arg );

  /** The "GetWorldTransform" gets WorldTransform. */
//  const WorldTransformType* GetWorldTransform() const; 

  /** The SetCommunication method is used to attach a communication object to the
  tracker object for communication with the tracker hardware. */
//  void SetCommunication( CommunicationType * communication );

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

protected:

  typedef enum 
  { 
    FAILURE=0, 
    SUCCESS
  } ResultType;

  Tracker(void);

  virtual ~Tracker(void);

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

  /** The "InternalInitialize" method initializes a newly opened device.
      This method is to be overriden by a decendent class 
      and responsible for device-specific processing */
  virtual ResultType InternalInitialize( void );

  /** The "InternalUninitialize" method initializes a newly opened device.
      This method is to be overriden by a decendent class 
      and responsible for device-specific processing */
  virtual ResultType InternalUninitialize( void );

  /** The "InternalStartTracking" method starts tracking.
      This method is to be overriden by a decendent class 
      and responsible for device-specific processing */
  virtual ResultType InternalStartTracking( void );

  /** The "InternalStopTracking" method stops tracking.
      This method is to be overriden by a decendent class 
      and responsible for device-specific processing */
  virtual ResultType InternalStopTracking( void );

  /** The "UpdateStatusProcessing" method updates tracker status.
      This method is to be overriden by a decendent class 
      and responsible for device-specific processing */
  virtual ResultType InternalUpdateStatus( void );

  /** Post-processing after communication setup has been successful. */ 
  virtual void CommunicationEstablishmentSuccessProcessing( void );
  /** Post-processing after communication setup has failed. */ 
  virtual void CommunicationEstablishmentFailureProcessing( void );

  /** Post-processing after ports and tools setup has been successful. */ 
  virtual void ToolsActivationSuccessProcessing( void );
  /** Post-processing after ports and tools setup has failed. */ 
  virtual void ToolsActivationFailureProcessing( void );

  /** Post-processing after start tracking has been successful. */ 
  virtual void StartTrackingSuccessProcessing( void );
  /** Post-processing after start tracking has failed. */ 
  virtual void StartTrackingFailureProcessing( void );

  /** Post-processing after stop tracking has been successful. */ 
  virtual void StopTrackingSuccessProcessing( void );
  /** Post-processing after start tracking has failed. */ 
  virtual void StopTrackingFailureProcessing( void );

  /** Post-processing after close tracking has been successful. */ 
  virtual void CloseTrackingSuccessProcessing( void );
  /** Post-processing after close tracking has failed. */ 
  virtual void CloseTrackingFailureProcessing( void );

  /** The GetLogger method return pointer to the logger object. */
  LoggerType* GetLogger(  void );

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
  
  /** The "Communication" instance */
//  CommunicationType::Pointer    m_Communication;
 
  /** The reference tool */
  /*
  bool                      m_ApplyingReferenceTool;
  TrackerToolPointer        m_ReferenceTool;
  unsigned int              m_ReferenceToolPortNumber;
  unsigned int              m_ReferenceToolNumber;
  */

  /** World Transform */
  // WorldTransformPointer     m_WorldTransform;

  /** List of States */
  StateType                m_IdleState;
  StateType                m_AttemptingToEstablishCommunicationState;
  StateType                m_CommunicationEstablishedState;
  StateType                m_AttemptingToActivateToolsState;
  StateType                m_ToolsActiveState;
  StateType                m_AttemptingToTrackState;
  StateType                m_TrackingState;
  StateType                m_AttemptingToStopTrackingState;
  StateType                m_AttemptingToCloseTrackingState;

  /** List of Inputs */
  InputType                m_SetUpCommunicationInput;
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

  InputType                m_ResetTrackingInput;

  InputType                m_CloseTrackingInput;
  InputType                m_CloseTrackingSuccessInput;
  InputType                m_CloseTrackingFailureInput;

  /** The Logger instance */
  LoggerType::Pointer      m_Logger;

  /** The "AttemptToOpen" method attempts to open communication with a tracking device. */
  void AttemptToOpen( void );

  /** The "AttemptToOpen" method attempts to close communication with a tracking device. */
  void AttemptToClose( void );

  /** The "AttemptToReset" method attempts to bring the tracker
  to some defined default state. */
  void AttemptToReset( void );

  /** The "AttemptToInitialize" method attempts to initialize a newly opened device. */
  void AttemptToInitialize( void );

  /** The "AttemptToUninitialize" method attempts to uninitialize a newly opened device. */
  void AttemptToUninitialize( void );

  /** The "AttemptToStartTracking" method attempts to start tracking. */
  void AttemptToStartTracking( void );

  /** The "AttemptToStopTracking" method attempts to stop tracking. */
  void AttemptToStopTracking( void );

  /** The "AttemptToUpdateStatus" method attempts to update status during tracking. */
  void AttemptToUpdateStatus( void );

  /** The "CloseFromTrackingStateProcessing" method closes tracker in use, when the tracker is
    * in tracking state. */
  void CloseFromTrackingStateProcessing( void );

  /** The "CloseFromToolsActiveStateProcessing" method closes tracker in use, when the tracker is
    * in active tools state. */
  void CloseFromToolsActiveStateProcessing( void);

  /** The "CloseFromCommunicatingStateProcessing" method closes tracker in use, when the tracker is
    * in communicating state. */
  void CloseFromCommunicatingStateProcessing( void );

};

}

#endif //__igstk_Tracker_h_
