/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImager.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkImager_h
#define __igstkImager_h

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
#include "igstkImagerTool.h"

#include "igstkCoordinateSystemInterfaceMacros.h"


namespace igstk
{

igstkEventMacro( ImagerEvent,                             StringEvent);
igstkEventMacro( ImagerErrorEvent,                IGSTKErrorWithStringEvent);

igstkEventMacro( ImagerOpenEvent,                         ImagerEvent);
igstkEventMacro( ImagerOpenErrorEvent,                    ImagerErrorEvent);

igstkEventMacro( ImagerCloseEvent,                        ImagerEvent);
igstkEventMacro( ImagerCloseErrorEvent,                   ImagerErrorEvent);

igstkEventMacro( ImagerInitializeEvent,                   ImagerEvent);
igstkEventMacro( ImagerInitializeErrorEvent,              ImagerErrorEvent);

igstkEventMacro( ImagerStartImagingEvent,                 ImagerEvent);
igstkEventMacro( ImagerStartImagingErrorEvent,            ImagerErrorEvent);

igstkEventMacro( ImagerStopImagingEvent,                  ImagerEvent);
igstkEventMacro( ImagerStopImagingErrorEvent,             ImagerErrorEvent);

igstkEventMacro( ImagerUpdateStatusEvent,                 ImagerEvent);
igstkEventMacro( ImagerUpdateStatusErrorEvent,            ImagerErrorEvent);

igstkEventMacro( ImagerToolFrameUpdateEvent,              ImagerEvent);

/** \class Imager
 *  \brief Abstract superclass for concrete IGSTK Imager classes.
 *
 *  This class presents a generic interface for grabbing videostream
 *  from video-devices such Ultrasound, Endoscope, Bronchoscope, etc.
 *  The various derived subclasses of this class provide back-ends that
 *  communicate with several standard imaging systems.
 *
 *  The state machine of this class implements the basic
 *  state transitions of an imager. Inputs to the state
 *  machine are translated into method calls that can be
 *  overridden by device-specific derive classes that do
 *  the appropriate processing for a particular device.
 *
 *  The following diagram illustrates the state machine of
 *  the imager class
 *
 *  \image html  igstkImager.png  "Imager State Machine Diagram"
 *  \image latex igstkImager.eps  "Imager State Machine Diagram"
 *
 *  \ingroup Imager
 */

class Imager : public Object
{

public:
  /** Macro with standard traits declarations. */
  igstkStandardAbstractClassTraitsMacro( Imager, Object )

public:

  igstkFriendClassMacro( ImagerTool );

  /** typedefs from ImagerTool class */
  typedef ImagerTool       ImagerToolType;

  /** The "RequestOpen" method attempts to open communication with the
   *  imaging device. It generates a ImagerOpenEvent if successful,
   *  or a ImagerOpenErrorEvent if not successful.  */
  void RequestOpen( void );

  /** The "RequestClose" method closes communication with the device.
   *  It generates a ImagerCloseEvent if successful,
   *  or a ImagerCloseErrorEvent if not successful. */
  void RequestClose( void );

  /** The "RequestReset" imager method should be used to bring the imager
  to some defined default state. */
  void RequestReset( void );

  /** The "RequestStartImaging" method readies the imager for imaging the
  tools connected to the imager. */
  void RequestStartImaging( void );

  /** The "RequestStopImaging" stops imager from imaging the tools. */
  void RequestStopImaging( void );


  /** The "RequestSetFrequency" method defines the frequency at which the
   * Transform information will be queried from the Imager device. Note that
   * Imager devices have their own internal frequency rate, and if you set here
   * a frequency that is higher than what the Imager device is capable to
   * follow, then you will start receiving similar frames. */
  void RequestSetFrequency( double frequencyInHz );

protected:

  Imager(void);
  virtual ~Imager(void);

  /** SetThreadingEnabled(bool) : set m_ThreadingEnabled value */
  igstkSetMacro( ThreadingEnabled, bool );

  /** GetThreadingEnabled(bool) : get m_ThreadingEnabled value  */
  igstkGetMacro( ThreadingEnabled, bool );

  /** typedef for times used by the imager */
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

  /** The "InternalReset" method resets imager to a known configuration.
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


  /** The "InternalUpdateStatus" method updates imager status.
      This method is to be implemented by a descendant class
      and responsible for device-specific processing */
  virtual ResultType InternalUpdateStatus( void ) = 0;

  /** The "InternalThreadedUpdateStatus" method updates imager status.
      This method is called in a separate thread.
      This method is to be implemented by a descendant class
      and responsible for device-specific processing */
  virtual ResultType InternalThreadedUpdateStatus( void ) = 0;

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Verify if a imager tool information is correct before attaching
   *  it to the imager. This method is used to verify the information supplied
   *  by the user about the imager tool. The information depends on the
   *  imager type.
   */
  virtual ResultType
        VerifyImagerToolInformation( const ImagerToolType * ) = 0;

  /** The "ValidateSpecifiedFrequency" method checks if the specified frequency is
   * valid for the imaging device that is being used. This method is to be
   * overridden in the derived imaging-device specific classes to take
   * into account the maximum frequency possible in the imaging device
  */
  virtual ResultType ValidateSpecifiedFrequency( double frequencyInHz );

  /** This method will remove entries of the tracker tool from internal
    * data containers */
  virtual ResultType RemoveImagerToolFromInternalDataContainers(
                                     const ImagerToolType * imagerTool ) = 0;

  /** Add imager tool entry to internal containers */
  virtual ResultType AddImagerToolToInternalDataContainers(
                                    const ImagerToolType * imagerTool ) = 0;

  /** typedefs from ImagerTool class */
  typedef std::map< std::string, ImagerToolType *>  ImagerToolsContainerType;

  /** Access method for the imager tool container. This method
    * is useful in the derived classes to access the unique identifiers
    * of the imager tools */
  const ImagerToolsContainerType & GetImagerToolContainer() const;

  /** Report to imager tool that it is not available for imaging */
  void ReportImagingToolNotAvailable( ImagerToolType * imagerTool ) const;

  /** Report to imager tool that it is visible */
  void ReportImagingToolVisible( ImagerToolType * imagerTool ) const;

  void SetImagerToolFrame( ImagerToolType * imagerTool,
                                   const FrameType & frame );

  void GetImagerToolFrame( ImagerToolType * imagerTool,
                                   FrameType & frame );

  /** Turn on/off update flag of the imager tool */
  void SetImagerToolUpdate( ImagerToolType * imagerTool,
                                      bool flag ) const;

  /** Depending on the imager type, the imaging thread should be
    * terminated or left untouched when we stop imaging. */

  /** Always called when exiting imaging state. This methold will be
    * overriden in derived classes. */
  void ExitImagingStateProcessing( void );

  /** Exit imaging without terminating imaging thread */
  void ExitImagingWithoutTerminatingImagingThread();

  /** Exit imaging after terminating imaging thread */
  void ExitImagingTerminatingImagingThread();


private:
  Imager(const Self&);           //purposely not implemented
  void operator=(const Self&);    //purposely not implemented

  /** Pulse generator for driving the rate of imager updates. */
  PulseGenerator::Pointer   m_PulseGenerator;

  /** Pulse observer for receiving the events from the pulse generator. */
  typedef itk::SimpleMemberCommand< Self >   ObserverType;
  ObserverType::Pointer     m_PulseObserver;

  /** An associative container of ImagerTool Pointer with
   * ImagerTool identifier used as a Key*/
  ImagerToolsContainerType           m_ImagerTools;

  /** typedefs from ImagerTool class */
  typedef ImagerToolType::Pointer                   ImagerToolPointer;

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

  //TODO rename
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
  igstkDeclareStateMacro( AttemptingToAttachImagerTool );
  igstkDeclareStateMacro( ImagerToolAttached );
  igstkDeclareStateMacro( AttemptingToTrack );
  igstkDeclareStateMacro( Imaging );
  igstkDeclareStateMacro( AttemptingToUpdate );
  igstkDeclareStateMacro( AttemptingToStopImaging );

  /** List of Inputs */
  igstkDeclareInputMacro( EstablishCommunication );
  igstkDeclareInputMacro( StartImaging );
  igstkDeclareInputMacro( AttachImagerTool );
  igstkDeclareInputMacro( UpdateStatus );
  igstkDeclareInputMacro( StopImaging );
  igstkDeclareInputMacro( Reset );
  igstkDeclareInputMacro( CloseCommunication );
  igstkDeclareInputMacro( ValidFrequency );

  igstkDeclareInputMacro( Success );
  igstkDeclareInputMacro( Failure );

  /** Attach a imager tool to the imager. This method
   *  should be called by the imager tool.  */
  void RequestAttachTool( ImagerToolType * imagerTool );

  /** Request to remove a imager tool from this imager  */
  ResultType RequestRemoveTool( ImagerToolType * imagerTool );

  /** Thread function for imaging */
  static ITK_THREAD_RETURN_TYPE ImagingThreadFunction(void* pInfoStruct);

  /** The "UpdateStatus" method is used for updating the status of
      tools when the imager is in imaging state. It is a callback
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

  /** The "AttemptToAttachImagerToolProcessing" method attempts
   *  to attach a imager tool to the imager . */
  void AttemptToAttachImagerToolProcessing( void );

  /** The "AttemptToUpdateStatusProcessing" method attempts to update status
      during imaging. */
  void AttemptToUpdateStatusProcessing( void );

  /** The "UpdateStatusFailureProcessing" method is called when an
      attempt to update failes. */
  void UpdateStatusSuccessProcessing( void );

  /** The "UpdateStatusFailureProcessing" method is called when an
      attempt to update failes. */
  void UpdateStatusFailureProcessing( void );

  /** The "CloseFromImagingStateProcessing" method closes imager in
      use, when the imager is in imaging state. */
  void CloseFromImagingStateProcessing( void );

  /** The "CloseFromCommunicatingStateProcessing" method closes
      imager in use, when the imager is in communicating state. */
  void CloseFromCommunicatingStateProcessing( void );

  /** The "ResetFromImagingStateProcessing" method resets imager in
      use, when the imager is in imaging state. */
  void ResetFromImagingStateProcessing( void );

  /** The "ResetFromToolsActiveStateProcessing" method resets imager
      in use, when the imager is in active tools state. */
  void ResetFromToolsActiveStateProcessing( void);

  /** The "ResetFromCommunicatingStateProcessing" method resets
      imager in use, when the imager is in communicating state. */
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

  /** Post-processing after attaching a imager tool
     has been successful. */
  void AttachingImagerToolSuccessProcessing( void );

  /** Post-processing after an attempt to attach a imager tool
   *  has failed. */
  void AttachingImagerToolFailureProcessing( void );

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

  /** Detach all imager tools from the imager */
  void DetachAllImagerToolsFromImager();

  /** Report invalid request */
  void ReportInvalidRequestProcessing( void );

  /** Actually set the frequency of update */
  void SetFrequencyProcessing( void );

  /** Define the coordinate system interface
   */
  igstkCoordinateSystemClassInterfaceMacro();

  ImagerToolType   * m_ImagerToolToBeAttached;

  double              m_FrequencyToBeSet;
};

}

#endif //__igstk_Imager_h_
