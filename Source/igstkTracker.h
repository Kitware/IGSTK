/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTracker.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTracker_h
#define __igstkTracker_h

#include <vector>
#include <map>

#include "itkMutexLock.h"
#include "itkConditionVariable.h"
#include "itkMultiThreader.h"

#include "igstkObject.h"
#include "igstkStateMachine.h"
#include "igstkTransform.h"
#include "igstkPulseGenerator.h"
#include "igstkTrackerTool.h"

#include "igstkCoordinateSystemInterfaceMacros.h"


namespace igstk
{

igstkEventMacro( TrackerEvent,                             StringEvent);
igstkEventMacro( TrackerErrorEvent,                IGSTKErrorWithStringEvent);

igstkEventMacro( TrackerOpenEvent,                         TrackerEvent);
igstkEventMacro( TrackerOpenErrorEvent,                    TrackerErrorEvent);

igstkEventMacro( TrackerCloseEvent,                        TrackerEvent);
igstkEventMacro( TrackerCloseErrorEvent,                   TrackerErrorEvent);

igstkEventMacro( TrackerInitializeEvent,                   TrackerEvent);
igstkEventMacro( TrackerInitializeErrorEvent,              TrackerErrorEvent);

igstkEventMacro( TrackerStartTrackingEvent,                TrackerEvent);
igstkEventMacro( TrackerStartTrackingErrorEvent,           TrackerErrorEvent);

igstkEventMacro( TrackerStopTrackingEvent,                 TrackerEvent);
igstkEventMacro( TrackerStopTrackingErrorEvent,            TrackerErrorEvent);

igstkEventMacro( TrackerUpdateStatusEvent,                 TrackerEvent);
igstkEventMacro( TrackerUpdateStatusErrorEvent,            TrackerErrorEvent);

igstkEventMacro( TrackerToolTransformUpdateEvent,          TrackerEvent);


/** \class Tracker
 *  \brief Abstract superclass for concrete IGSTK Tracker classes.
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
 *  \image html  igstkDummyTracker.png  "Tracker State Machine Diagram"
 *  \image latex igstkDummyTracker.eps  "Tracker State Machine Diagram" 
 *
 *  \ingroup Tracker
 */

class Tracker : public Object
{

public:
  /** Macro with standard traits declarations. */
  igstkStandardAbstractClassTraitsMacro( Tracker, Object ) 

public:

  igstkFriendClassMacro( TrackerTool );

  /** typedefs from TrackerTool class */
  typedef TrackerTool       TrackerToolType;

  /** The "RequestOpen" method attempts to open communication with the 
   *  tracking device. It generates a TrackerOpenEvent if successful,
   *  or a TrackerOpenErrorEvent if not successful.  */
  void RequestOpen( void );

  /** The "RequestClose" method closes communication with the device. 
   *  It generates a TrackerCloseEvent if successful,
   *  or a TrackerCloseErrorEvent if not successful. */
  void RequestClose( void );

  /** The "RequestReset" tracker method should be used to bring the tracker
  to some defined default state. */
  void RequestReset( void );

  /** The "RequestStartTracking" method readies the tracker for tracking the
  tools connected to the tracker. */
  void RequestStartTracking( void );

  /** The "RequestStopTracking" stops tracker from tracking the tools. */
  void RequestStopTracking( void );
  
  /** The "RequestSetFrequency" method defines the frequency at which the
   * Transform information will be queried from the Tracker device. Note that
   * Tracker devices have their own internal frequency rate, and if you set here
   * a frequency that is higher than what the Tracker device is capable to
   * follow, then you will start receiving transforms with repeated values. */
  void RequestSetFrequency( double frequencyInHz );

  /** Set a reference tracker tool */
  void RequestSetReferenceTool( TrackerToolType * trackerTool );

  /** SetThreadingEnabled(bool) : set m_ThreadingEnabled value */
  igstkSetMacro( ThreadingEnabled, bool );

  /** GetThreadingEnabled(bool) : get m_ThreadingEnabled value  */
  igstkGetMacro( ThreadingEnabled, bool );

protected:

  Tracker(void);

  virtual ~Tracker(void);

  /** typedef for times used by the tracker */
  typedef Transform::TimePeriodType         TimePeriodType;

  /** Get the validity time. */
  igstkGetMacro( ValidityTime, TimePeriodType );

  typedef enum 
    { 
    FAILURE=0, 
    SUCCESS
    } ResultType;

  /** typedefs from Transform class */
  typedef Transform                      TransformType;

  /** The "InternalOpen" method opens communication with a tracking device.
      This method is to be implemented by a descendant class 
      and responsible for device-specific processing */
  virtual ResultType InternalOpen( void ) = 0;

  /** The "InternalClose" method closes communication with a tracking device.
      This method is to be implemented by a descendant class 
      and responsible for device-specific processing */
  virtual ResultType InternalClose( void ) = 0;

  /** The "InternalReset" method resets tracker to a known configuration. 
      This method is to be implemented by a descendant class 
      and responsible for device-specific processing */
  virtual ResultType InternalReset( void ) = 0;

  /** The "InternalStartTracking" method starts tracking.
      This method is to be implemented by a descendant class 
      and responsible for device-specific processing */
  virtual ResultType InternalStartTracking( void ) = 0;

  /** The "InternalStopTracking" method stops tracking.
      This method is to be implemented by a descendant class 
      and responsible for device-specific processing */
  virtual ResultType InternalStopTracking( void ) = 0;


  /** The "InternalUpdateStatus" method updates tracker status.
      This method is to be implemented by a descendant class 
      and responsible for device-specific processing */
  virtual ResultType InternalUpdateStatus( void ) = 0;

  /** The "InternalThreadedUpdateStatus" method updates tracker status.
      This method is called in a separate thread.
      This method is to be implemented by a descendant class
      and responsible for device-specific processing */
  virtual ResultType InternalThreadedUpdateStatus( void ) = 0;

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Verify if a tracker tool information is correct before attaching
   *  it to the tracker. This method is used to verify the information supplied
   *  by the user about the tracker tool. The information depends on the
   *  tracker type. For example, during the configuration step of the
   *  MicronTracker, location of the directory containing marker template files
   *  is specified. If the user tries to attach a tracker tool with a marker
   *  type whose template file is not stored in this directory, this method
   *  will return failure. Similarly, for PolarisTracker, the method returns
   *  failure,  if the tool part number specified by the user during the tracker
   *  tool configuration step does not match with the part number read from the
   *  SROM file.
   */
  virtual ResultType 
        VerifyTrackerToolInformation( const TrackerToolType * ) = 0; 

  /** The "ValidateSpecifiedFrequency" method checks if the specified  
   * frequency is valid for the tracking device that is being used. This 
   * method is to be overridden in the derived tracking-device specific  
   * classes to take into account the maximum frequency possible in the 
   * tracking device
   */
  virtual ResultType ValidateSpecifiedFrequency( double frequencyInHz );

  /** This method will remove entries of the traceker tool from internal
    * data containers */
  virtual ResultType RemoveTrackerToolFromInternalDataContainers(
                                     const TrackerToolType * trackerTool ) = 0; 

  /** Add tracker tool entry to internal containers */
  virtual ResultType AddTrackerToolToInternalDataContainers( 
                                    const TrackerToolType * trackerTool ) = 0;

  /** typedefs from TrackerTool class */
  typedef std::map< std::string, TrackerToolType *>  TrackerToolsContainerType;

  /** Access method for the tracker tool container. This method 
    * is useful in the derived classes to access the unique identifiers 
    * of the tracker tools */
  const TrackerToolsContainerType & GetTrackerToolContainer() const;

  /** Report to tracker tool that it is not available for tracking */
  void ReportTrackingToolNotAvailable( TrackerToolType * trackerTool ) const;

  /** Report to tracker tool that it is visible */
  void ReportTrackingToolVisible( TrackerToolType * trackerTool ) const;

  /** Set tracker tool raw transform */
  void SetTrackerToolRawTransform( TrackerToolType * trackerTool, 
                                   const TransformType transform );

  /** Turn on/off update flag of the tracker tool */
  void SetTrackerToolTransformUpdate( TrackerToolType * trackerTool,
                                      bool flag ) const;

  /** Depending on the tracker type, the tracking thread should be 
    * terminated or left untouched when we stop tracking. For example,
    * in the case of MicronTracker, it is better to not terminate the
    * tracking thread. Otherwise, everytime we restart tracking, then 
    * the camera has to be reattached. For NDI trackers, the tracking
    * thread has to be terminated first to send TSTOP command */  

  /** Always called when exiting tracking state. This methold will be
    * overriden in derived classes. */
  void ExitTrackingStateProcessing( void );

  /** Exit tracking without terminating tracking thread */
  void ExitTrackingWithoutTerminatingTrackingThread();

  /** Exit tracking after terminating tracking thread */
  void ExitTrackingTerminatingTrackingThread();


private:
  Tracker(const Self&);           //purposely not implemented
  void operator=(const Self&);    //purposely not implemented

  /** Pulse generator for driving the rate of tracker updates. */
  PulseGenerator::Pointer   m_PulseGenerator;
  
  /** Pulse observer for receiving the events from the pulse generator. */
  typedef itk::SimpleMemberCommand< Self >   ObserverType;
  ObserverType::Pointer     m_PulseObserver;

  // An associative container of TrackerTool Pointer with 
  // TrackerTool identifier used as a Key
  TrackerToolsContainerType           m_TrackerTools;
  
  /** typedefs from TrackerTool class */
  typedef TrackerToolType::Pointer                   TrackerToolPointer;

  /** The reference tool */
  bool                                m_ApplyingReferenceTool;
  TrackerToolPointer                  m_ReferenceTool;

  /** Validity time, and its default value [milliseconds] */
  TimePeriodType                      m_ValidityTime;

  /** Multi-threading enabled flag : The descendant class will use
      multi-threading, if this flag is set as true */
  bool                                m_ThreadingEnabled;

  /** Boolean value to indicate that the tracking thread 
    * has started */
  bool                                m_TrackingThreadStarted;

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
  igstkDeclareStateMacro( AttemptingToAttachTrackerTool );
  igstkDeclareStateMacro( TrackerToolAttached );
  igstkDeclareStateMacro( AttemptingToTrack );
  igstkDeclareStateMacro( Tracking );
  igstkDeclareStateMacro( AttemptingToUpdate );
  igstkDeclareStateMacro( AttemptingToStopTracking );

  /** List of Inputs */
  igstkDeclareInputMacro( EstablishCommunication );
  igstkDeclareInputMacro( StartTracking );
  igstkDeclareInputMacro( AttachTrackerTool );
  igstkDeclareInputMacro( UpdateStatus );
  igstkDeclareInputMacro( StopTracking );
  igstkDeclareInputMacro( Reset );
  igstkDeclareInputMacro( CloseCommunication );
  igstkDeclareInputMacro( ValidFrequency );

  igstkDeclareInputMacro( Success );
  igstkDeclareInputMacro( Failure );

  /** Attach a tracker tool to the tracker. This method
   *  should be called by the tracker tool.  */
  void RequestAttachTool( TrackerToolType * trackerTool );

  /** Request to remove a tracker tool from this tracker  */
  ResultType RequestRemoveTool( TrackerToolType * trackerTool );

  /** Thread function for tracking */
  static ITK_THREAD_RETURN_TYPE TrackingThreadFunction(void* pInfoStruct);

  /** The "UpdateStatus" method is used for updating the status of 
      tools when the tracker is in tracking state. It is a callback
      method that gets invoked when a pulse event is observed */
  void UpdateStatus( void );

  /** The "AttemptToOpenProcessing" method attempts to open communication with a
      tracking device. */
  void AttemptToOpenProcessing( void );
  
  /** The "AttemptToStartTrackingProcessing" method attempts 
   *  to start tracking. */
  void AttemptToStartTrackingProcessing( void );

  /** The "AttemptToStopTrackingProcessing" method attempts to stop tracking. */
  void AttemptToStopTrackingProcessing( void );

  /** The "AttemptToAttachTrackerToolProcessing" method attempts 
   *  to attach a tracker tool to the tracker . */
  void AttemptToAttachTrackerToolProcessing( void );

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

  /** Post-processing after attaching a tracker tool
     has been successful. */ 
  void AttachingTrackerToolSuccessProcessing( void );

  /** Post-processing after an attempt to attach a tracker tool
   *  has failed. */ 
  void AttachingTrackerToolFailureProcessing( void );

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

  /** Detach all tracker tools from the tracker */
  void DetachAllTrackerToolsFromTracker();

  /** Report invalid request */ 
  void ReportInvalidRequestProcessing( void );

  /** Actually set the frequency of update */ 
  void SetFrequencyProcessing( void );

  /** Define the coordinate system interface 
   */
  igstkCoordinateSystemClassInterfaceMacro();

  TrackerToolType   * m_TrackerToolToBeAttached;

  double              m_FrequencyToBeSet;
};

}

#endif //__igstk_Tracker_h_
