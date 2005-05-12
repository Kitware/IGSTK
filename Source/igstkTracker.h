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

#include "igstkCommunication.h"
#include "igstkLogger.h"
#include "igstkStateMachine.h"
#include "igstkTrackerPort.h"
#include "igstkTransform.h"
#include "igstkSpatialObject.h"
#include "igstkPulseGenerator.h"

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
  typedef igstk::Logger                  LoggerType;

  /* typedefs from igstk::TrackerPort class */
  typedef igstk::TrackerTool             TrackerToolType;

  /* typedefs from igstk::TrackerTool class */
  typedef Transform                TransformType;
  typedef double                   ErrorType;

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

  /** The "Initialize" method initializes the tracker. The input 
  is a file in XML format describing the set up configuration. The
  configuration file specifies type of communication between the 
  tracker object and the actual hardware (a file name for offline
  operation), number of ports, number of tools on each port, port
  and tool description, etc.
  */
  virtual void Initialize( const char *fileName = NULL );

  /** The "Reset" tracker method should be used to bring the tracker
  to some defined default state. */
  virtual void Reset( void );

  /** The "StartTracking" method readies the tracker for tracking the
  tools connected to the tracker. */
  virtual void StartTracking( void );

  /** The "EndTracking" stops tracker from tracking the tools. */
  virtual void StopTracking( void );

  /** The "UpdateStatus" method is used for updating the status of 
  port and tools when the tracker is in tracking state. */
  virtual void UpdateStatus( void );

  /** The "Close" method stops the tracker from use. */
  virtual void Close( void );

  /** The "GetToolTransform" gets the position of tool numbered "toolNumber" on
   * port numbered "portNumber" in the variable "position". Note that this
   * variable represents the position and orientation of the tool in 3D space.
   * */
  virtual void GetToolTransform( unsigned int portNumber, unsigned int toolNumber, TransformType &position ) const;

  /** The "SetToolTransform" sets the position of tool numbered "toolNumber" on
   * port numbered "portNumber" by the content of variable "position". Note
   * that this variable represents the position and orientation of the tool in
   * 3D space.  */
  virtual void SetToolTransform( unsigned int portNumber, unsigned int toolNumber, const TransformType & position );

  /** Associate a TrackerTool to an object to be tracked. This is a one-to-one
   * association and cannot be changed during the life of the application */
  virtual void AttachObjectToTrackerTool( unsigned int portNumber, unsigned int toolNumber, SpatialObject * objectToTrack );

  /** The SetLogger method is used to attach a logger object to the
  tracker object for logging purposes. */
  void SetLogger( LoggerType * logger );

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

protected:

  Tracker(void);

  virtual ~Tracker(void);

  /** The "AddPort" method adds a port to the tracker. */
  void AddPort( TrackerPortType * port);

  /** The "ClearPorts" clears all the ports. */
  void ClearPorts( void );

  /** The "AttemptToSetUpCommunicationProcessing" method attempts to sets up communication. */
  virtual void AttemptToSetUpCommunicationProcessing( void );
  /** Post-processing after communication setup has been successful. */ 
  virtual void CommunicationEstablishmentSuccessProcessing( void );
  /** Post-processing after communication setup has failed. */ 
  virtual void CommunicationEstablishmentFailureProcessing( void );


  /** The "AttempToSetUpToolsProcessing" method attemps to sets up ports and tools. */
  virtual void AttemptToSetUpToolsProcessing( void );
  /** Post-processing after ports and tools setup has been successful. */ 
  virtual void ToolsActivationSuccessProcessing( void );
  /** Post-processing after ports and tools setup has failed. */ 
  virtual void ToolsActivationFailureProcessing( void );

  /** The "AttempToStartTrackingProcessing" method attemps to start tracking. */
  virtual void AttemptToStartTrackingProcessing( void );
  /** Post-processing after start tracking has been successful. */ 
  virtual void StartTrackingSuccessProcessing( void );
  /** Post-processing after start tracking has failed. */ 
  virtual void StartTrackingFailureProcessing( void );

  /** The "UpdateStatusProcessing" method updates tracker status. */
  virtual void UpdateStatusProcessing( void );

  /** The "AttempToStopTrackingProcessing" method attemps to stop tracking. */
  virtual void AttemptToStopTrackingProcessing( void );
  /** Post-processing after stop tracking has been successful. */ 
  virtual void StopTrackingSuccessProcessing( void );
  /** Post-processing after start tracking has failed. */ 
  virtual void StopTrackingFailureProcessing( void );

  /** The "ResetTrackingProcessing" method resets tracker to a known configuration. */
  virtual void ResetTrackingProcessing( void );

  /** The "CloseFromTrackingStateProcessing" method closes tracker in use, when the tracker is
    * in tracking state. */
  virtual void CloseFromTrackingStateProcessing( void );

  /** The "CloseFromToolsActiveStateProcessing" method closes tracker in use, when the tracker is
    * in active tools state. */
  virtual void CloseFromToolsActiveStateProcessing( void);

  /** The "CloseFromCommunicatingStateProcessing" method closes tracker in use, when the tracker is
    * in communicating state. */
  virtual void CloseFromCommunicatingStateProcessing( void );

  /** Post-processing after close tracking has been successful. */ 
  virtual void CloseTrackingSuccessProcessing( void );

  /** Post-processing after close tracking has failed. */ 
  virtual void CloseTrackingFailureProcessing( void );

  /** The "DisableCommunicationProcessing" disables communication. */
  virtual void DisableCommunicationProcessing( void );

  /** The "DisableToolsProcessing" method disables tools. */
  virtual void DisableToolsProcessing( void );

  /** The GetLogger method return pointer to the logger object. */
  LoggerType* GetLogger(  void );

private:

  /** Pulse generator for driving the rate of tracker updates. */
  PulseGenerator::Pointer   m_PulseGenerator;
  
  /** Pulse observer for receiving the events from the pulse generator. */
  typedef itk::SimpleMemberCommand< Self >   ObserverType;
  ObserverType::Pointer     m_PulseObserver;

  /** Vector of all tool ports on the tracker */
  TrackerPortVectorType     m_Ports;
  
  /** The "Communication" instance */
  CommunicationType::Pointer    m_Communication;
 
  /** Results of post-transition event actions by the state machine */
  InputType                *m_pSetUpCommunicationResultInput;
  InputType                *m_pActivateToolsResultInput;
  InputType                *m_pStartTrackingResultInput;
  InputType                *m_pStopTrackingResultInput;
  InputType                *m_pCloseTrackingResultInput;

  InputType                m_SetUpCommunicationInput;
  InputType                m_CommunicationEstablishmentSuccessInput;
  InputType                m_CommunicationEstablishmentFailureInput;

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
  InputType                m_ActivateToolsInput;
  InputType                m_ToolsActivationSuccessInput;
  InputType                m_ToolsActivationFailureInput;

  InputType                m_StartTrackingInput;
  InputType                m_StartTrackingSuccessInput;
  InputType                m_StartTrackingFailureInput;

  InputType                m_UpdateStatus;

  InputType                m_StopTracking;
  InputType                m_StopTrackingSuccessInput;
  InputType                m_StopTrackingFailureInput;

  InputType                m_ResetTracking;

  InputType                m_CloseTracking;
  InputType                m_CloseTrackingSuccessInput;
  InputType                m_CloseTrackingFailureInput;

  /** The Logger instance */
  LoggerType::Pointer      m_Logger;

};

}

#endif //__igstk_Tracker_h_
