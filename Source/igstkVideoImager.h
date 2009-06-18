/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoImager.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkVideoImager_h
#define __igstkVideoImager_h

#include <vector>
#include <map>

#include "itkMutexLock.h"
#include "itkConditionVariable.h"
#include "itkMultiThreader.h"

#include "igstkObject.h"
#include "igstkStateMachine.h"
#include "igstkTransform.h"
#include "igstkFrame.h"
#include "igstkPulseGenerator.h"
#include "igstkVideoImagerTool.h"

#include "igstkCoordinateSystemInterfaceMacros.h"

namespace igstk
{

igstkEventMacro( VideoImagerEvent,                   StringEvent);
igstkEventMacro( VideoImagerErrorEvent,              IGSTKErrorWithStringEvent);

igstkEventMacro( VideoImagerOpenEvent,               VideoImagerEvent);
igstkEventMacro( VideoImagerOpenErrorEvent,          VideoImagerErrorEvent);

igstkEventMacro( VideoImagerCloseEvent,              VideoImagerEvent);
igstkEventMacro( VideoImagerCloseErrorEvent,         VideoImagerErrorEvent);

igstkEventMacro( VideoImagerInitializeEvent,         VideoImagerEvent);
igstkEventMacro( VideoImagerInitializeErrorEvent,    VideoImagerErrorEvent);

igstkEventMacro( VideoImagerStartImagingEvent,       VideoImagerEvent);
igstkEventMacro( VideoImagerStartImagingErrorEvent,  VideoImagerErrorEvent);

igstkEventMacro( VideoImagerStopImagingEvent,        VideoImagerEvent);
igstkEventMacro( VideoImagerStopImagingErrorEvent,   VideoImagerErrorEvent);

igstkEventMacro( VideoImagerUpdateStatusEvent,       VideoImagerEvent);
igstkEventMacro( VideoImagerUpdateStatusErrorEvent,  VideoImagerErrorEvent);

/** \class VideoImager
 *  \brief Abstract superclass for concrete IGSTK VideoImager classes.
 *
 *  This class presents a generic interface for grabbing videostream
 *  from video-devices such Ultrasound, Endoscope, Bronchoscope, etc.
 *  The various derived subclasses of this class provide back-ends that
 *  communicate with several standard imaging systems.
 *
 *  The state machine of this class implements the basic
 *  state transitions of an video-imager. Inputs to the state
 *  machine are translated into method calls that can be
 *  overridden by device-specific derive classes that do
 *  the appropriate processing for a particular device.
 *
 *  The following diagram illustrates the state machine of
 *  the video-imager class
 *
 *  \image html  igstkVideoImager.png  "VideoImager State Machine Diagram"
 *  \image latex igstkVideoImager.eps  "VideoImager State Machine Diagram"
 *
 *  \ingroup VideoImager
 */

class VideoImager : public Object
{

public:
  /** Macro with standard traits declarations. */
  igstkStandardAbstractClassTraitsMacro( VideoImager, Object )

public:

  igstkFriendClassMacro( VideoImagerTool );

  /** typedefs from VideoImagerTool class */
  typedef VideoImagerTool       VideoImagerToolType;

  /** The "RequestOpen" method attempts to open communication with the
   *  imaging device. It generates a VideoImagerOpenEvent if successful,
   *  or a VideoImagerOpenErrorEvent if not successful.  */
  void RequestOpen( void );

  /** The "RequestClose" method closes communication with the device.
   *  It generates a VideoImagerCloseEvent if successful,
   *  or a VideoImagerCloseErrorEvent if not successful. */
  void RequestClose( void );

  /** The "RequestReset" method should be used to bring the VideoImager
  to some defined default state. */
  void RequestReset( void );

  /** The "RequestStartImaging" method readies the VideoImager for imaging the
  tools connected to the VideoImager. */
  void RequestStartImaging( void );

  /** The "RequestStopImaging" stops VideoImager from imaging the tools. */
  void RequestStopImaging( void );

  /** The "RequestSetFrequency" method defines the frequency at which a frame
   * will be queried from the VideoImager device. Note that
   * VideoImager devices have their own internal frequency rate, and if you set
   * here a frequency that is higher than what the VideoImager device is capable
   * to follow, then you will start receiving similar frames. */
  void RequestSetFrequency( double frequencyInHz );

protected:

  VideoImager(void);
  virtual ~VideoImager(void);

  /** SetThreadingEnabled(bool) : set m_ThreadingEnabled value */
  igstkSetMacro( ThreadingEnabled, bool );

  /** GetThreadingEnabled(bool) : get m_ThreadingEnabled value  */
  igstkGetMacro( ThreadingEnabled, bool );

  /** typedef for times used by the VideoImager */
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

  /** typedefs from Frame class */
  typedef Frame                          FrameType;

  /** The "InternalOpen" method opens communication with an imaging device.
      This method is to be implemented by a descendant class
      and responsible for device-specific processing */
  virtual ResultType InternalOpen( void ) = 0;

  /** The "InternalClose" method closes communication with an imaging device.
      This method is to be implemented by a descendant class
      and responsible for device-specific processing */
  virtual ResultType InternalClose( void ) = 0;

  /** The "InternalReset" method resets VideoImager to a known configuration.
      This method is to be implemented by a descendant class
      and responsible for device-specific processing */
  virtual ResultType InternalReset( void ) = 0;

  /** The "InternalStartImaging" method starts imaging.
      This method is to be implemented by a descendant class
      and responsible for device-specific processing */
  virtual ResultType InternalStartImaging( void ) = 0;

  /** The "InternalStopImaging" method stops imaging.
      This method is to be implemented by a descendant class
      and responsible for device-specific processing */
  virtual ResultType InternalStopImaging( void ) = 0;


  /** The "InternalUpdateStatus" method updates VideoImager status.
      This method is to be implemented by a descendant class
      and responsible for device-specific processing */
  virtual ResultType InternalUpdateStatus( void ) = 0;

  /** The "InternalThreadedUpdateStatus" method updates VideoImager status.
      This method is called in a separate thread.
      This method is to be implemented by a descendant class
      and responsible for device-specific processing */
  virtual ResultType InternalThreadedUpdateStatus( void ) = 0;

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Verify if a VideoImager tool information is correct before attaching
   *  it to the VideoImager. This method is used to verify the information
   *  supplied by the user about the VideoImager tool. The information depends
   *  on the VideoImager type.
   */
  virtual ResultType
        VerifyVideoImagerToolInformation( const VideoImagerToolType * ) = 0;

  /** The "ValidateSpecifiedFrequency" method checks if the specified frequency
   * is valid for the imaging device that is being used. This method is to be
   * overridden in the derived imaging-device specific classes to take
   * into account the maximum frequency possible in the imaging device
  */
  virtual ResultType ValidateSpecifiedFrequency( double frequencyInHz );

  /** This method will remove entries of the VideoImager tool from internal
    * data containers */
  virtual ResultType RemoveVideoImagerToolFromInternalDataContainers(
                              const VideoImagerToolType * videoImagerTool ) = 0;

  /** Add VideoImager tool entry to internal containers */
  virtual ResultType AddVideoImagerToolToInternalDataContainers(
                              const VideoImagerToolType * videoImagerTool ) = 0;

  /** typedefs from VideoImagerTool class */
  typedef std::map< std::string, VideoImagerToolType *>
                                                  VideoImagerToolsContainerType;

  /** Access method for the VideoImager tool container. This method
    * is useful in the derived classes to access the unique identifiers
    * of the VideoImager tools */
  const VideoImagerToolsContainerType & GetVideoImagerToolContainer() const;

  /** Report to VideoImager tool that it is not available for imaging */
  void ReportImagingToolNotAvailable(
                                  VideoImagerToolType * VideoImagerTool ) const;

  /** Report to VideoImager tool that it is streaming */
  void ReportImagingToolStreaming(
                                  VideoImagerToolType * videoImagerTool ) const;

  void SetVideoImagerToolFrame( VideoImagerToolType * videoImagerTool,
                                   FrameType* frame );

  FrameType* GetVideoImagerToolFrame( VideoImagerToolType * videoImagerTool);

  /** Turn on/off update flag of the VideoImager tool */
  void SetVideoImagerToolUpdate( VideoImagerToolType * videoImagerTool,
                                      bool flag ) const;

  /** Always called when exiting imaging state. This methold will be
    * overriden in derived classes. */
  void ExitImagingStateProcessing( void );

  /** Exit imaging without terminating imaging thread */
  void ExitImagingWithoutTerminatingImagingThread();

  /** Exit imaging after terminating imaging thread */
  void ExitImagingTerminatingImagingThread();

private:
  VideoImager(const Self&);           //purposely not implemented
  void operator=(const Self&);    //purposely not implemented

  /** Pulse generator for driving the rate of VideoImager updates. */
  PulseGenerator::Pointer   m_PulseGenerator;

  /** Pulse observer for receiving the events from the pulse generator. */
  typedef itk::SimpleMemberCommand< Self >   ObserverType;
  ObserverType::Pointer     m_PulseObserver;

  /** An associative container of VideoImagerTool Pointer with
   * VideoImagerTool identifier used as a Key*/
  VideoImagerToolsContainerType           m_VideoImagerTools;

  /** typedefs from VideoImagerTool class */
  typedef VideoImagerToolType::Pointer                   VideoImagerToolPointer;

  /** Validity time, and its default value [milliseconds] */
  TimePeriodType                      m_ValidityTime;

  /** Multi-threading enabled flag : The descendant class will use
      multi-threading, if this flag is set as true */
  bool                                m_ThreadingEnabled;

  /** Boolean value to indicate that the imaging thread
    * has started */
  bool                                m_ImagingThreadStarted;

  /** itk::MultiThreader object pointer */
  itk::MultiThreader::Pointer     m_Threader;

  /** Imaging ThreadID */
  int                             m_ThreadID;

  /** itk::ConditionVariable object pointer to signal for the next
   *  frame */
  itk::ConditionVariable::Pointer m_ConditionNextFrameReceived;

  /** itk::SimpleMutexLock object to be used for
      m_ConditionNextFrameReceived */
  itk::SimpleMutexLock            m_LockForConditionNextFrameReceived;

  /** List of States */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( AttemptingToEstablishCommunication );
  igstkDeclareStateMacro( AttemptingToCloseCommunication );
  igstkDeclareStateMacro( CommunicationEstablished );
  igstkDeclareStateMacro( AttemptingToAttachVideoImagerTool );
  igstkDeclareStateMacro( VideoImagerToolAttached );
  igstkDeclareStateMacro( AttemptingToImaging );
  igstkDeclareStateMacro( Imaging );
  igstkDeclareStateMacro( AttemptingToUpdate );
  igstkDeclareStateMacro( AttemptingToStopImaging );

  /** List of Inputs */
  igstkDeclareInputMacro( EstablishCommunication );
  igstkDeclareInputMacro( StartImaging );
  igstkDeclareInputMacro( AttachVideoImagerTool );
  igstkDeclareInputMacro( UpdateStatus );
  igstkDeclareInputMacro( StopImaging );
  igstkDeclareInputMacro( Reset );
  igstkDeclareInputMacro( CloseCommunication );
  igstkDeclareInputMacro( ValidFrequency );

  igstkDeclareInputMacro( Success );
  igstkDeclareInputMacro( Failure );

  /** Attach a VideoImager tool to the VideoImager. This method
   *  should be called by the VideoImager tool.  */
  void RequestAttachTool( VideoImagerToolType * videoImagerTool );

  /** Request to remove a VideoImager tool from this VideoImager  */
  ResultType RequestRemoveTool( VideoImagerToolType * videoImagerTool );

  /** Thread function for imaging */
  static ITK_THREAD_RETURN_TYPE ImagingThreadFunction(void* pInfoStruct);

  /** The "UpdateStatus" method is used for updating the status of
      tools when the VideoImager is in imaging state. It is a callback
      method that gets invoked when a pulse event is observed */
  void UpdateStatus( void );

  /** The "AttemptToOpenProcessing" method attempts to open communication with a
      imaging device. */
  void AttemptToOpenProcessing( void );

  /** The "AttemptToStartImagingProcessing" method attempts
   *  to start imaging. */
  void AttemptToStartImagingProcessing( void );

  /** The "AttemptToStopImagingProcessing" method attempts to stop imaging. */
  void AttemptToStopImagingProcessing( void );

  /** The "AttemptToAttachVideoImagerToolProcessing" method attempts
   *  to attach a VideoImager tool to the VideoImager . */
  void AttemptToAttachVideoImagerToolProcessing( void );

  /** The "AttemptToUpdateStatusProcessing" method attempts to update status
      during imaging. */
  void AttemptToUpdateStatusProcessing( void );

  /** The "UpdateStatusFailureProcessing" method is called when an
      attempt to update failes. */
  void UpdateStatusSuccessProcessing( void );

  /** The "UpdateStatusFailureProcessing" method is called when an
      attempt to update failes. */
  void UpdateStatusFailureProcessing( void );

  /** The "CloseFromImagingStateProcessing" method closes VideoImager in
      use, when the VideoImager is in imaging state. */
  void CloseFromImagingStateProcessing( void );

  /** The "CloseFromCommunicatingStateProcessing" method closes
      VideoImager in use, when the VideoImager is in communicating state. */
  void CloseFromCommunicatingStateProcessing( void );

  /** The "ResetFromImagingStateProcessing" method resets VideoImager in
      use, when the VideoImager is in imaging state. */
  void ResetFromImagingStateProcessing( void );

  /** The "ResetFromToolsActiveStateProcessing" method resets VideoImager
      in use, when the VideoImager is in active tools state. */
  void ResetFromToolsActiveStateProcessing( void);

  /** The "ResetFromCommunicatingStateProcessing" method resets
      VideoImager in use, when the VideoImager is in communicating state. */
  void ResetFromCommunicatingStateProcessing( void );

  /** Post-processing after communication setup has been successful. */
  void CommunicationEstablishmentSuccessProcessing( void );

  /** Post-processing after communication setup has failed. */
  void CommunicationEstablishmentFailureProcessing( void );

  /** Post-processing after tools setup has been successful. */
  void ToolsActivationSuccessProcessing( void );

  /** Post-processing after tools setup has failed. */
  void ToolsActivationFailureProcessing( void );

  /** Post-processing after start imaging has been successful. */
  void StartImagingSuccessProcessing( void );

  /** Post-processing after start imaging has failed. */
  void StartImagingFailureProcessing( void );

  /** Post-processing after attaching a VideoImager tool
     has been successful. */
  void AttachingVideoImagerToolSuccessProcessing( void );

  /** Post-processing after an attempt to attach a VideoImager tool
   *  has failed. */
  void AttachingVideoImagerToolFailureProcessing( void );

  /** Post-processing after stop imaging has been successful. */
  void StopImagingSuccessProcessing( void );

  /** Post-processing after start imaging has failed. */
  void StopImagingFailureProcessing( void );

  /** Post-processing after close imaging has been successful. */
  void CloseCommunicationSuccessProcessing( void );

  /** Post-processing after close imaging has failed. */
  void CloseCommunicationFailureProcessing( void );

  /** Always called when entering imaging state. */
  void EnterImagingStateProcessing( void );

  /** Detach all VideoImager tools from the VideoImager */
  void DetachAllVideoImagerToolsFromVideoImager();

  /** Report invalid request */
  void ReportInvalidRequestProcessing( void );

  /** Actually set the frequency of update */
  void SetFrequencyProcessing( void );

  /** Define the coordinate system interface
   */
  igstkCoordinateSystemClassInterfaceMacro();

  VideoImagerToolType   * m_VideoImagerToolToBeAttached;

  double                  m_FrequencyToBeSet;
};

}

#endif //__igstk_VideoImager_h_
