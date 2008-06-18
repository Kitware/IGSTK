/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerController.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTrackerController_h
#define __igstkTrackerController_h

#include "igstkSandboxConfigure.h"

#include "igstkTrackerConfiguration.h"
#include "igstkPolarisTrackerConfiguration.h"
#include "igstkAuroraTrackerConfiguration.h"
#ifdef IGSTKSandbox_USE_MicronTracker
#include "igstkMicronTrackerConfiguration.h"
#endif
#include "igstkAscensionTrackerConfiguration.h"

#include "igstkStateMachine.h"
#include "igstkMacros.h"
#include "igstkEvents.h"
#include "igstkObject.h"
#include "igstkSerialCommunication.h"

#include "igstkTracker.h"
#include "igstkTrackerTool.h"
#include "igstkPolarisTrackerTool.h"
#include "igstkAuroraTrackerTool.h"
#include "igstkMicronTrackerTool.h"
#include "igstkFlockOfBirdsTrackerToolNew.h"


namespace igstk
{

/** \class TrackerController
 *
 *  \brief This class encapsulates the startup and shutdown procedures required
 *         for the tracker classes supported by IGSTK.
 *
 *  This class enables the user to intialize a tracker with minimal effort. 
 *  Initialization is performed by passing the controller a TrackerConfiguration
 *  object. The controller identifies the specific tracker internally. The
 *  user can then request to intialize the tracker or to tell the tracker 
 *  controller to shut down a tracker that is running. 
 *
 *  NOTE: This class is usefull only when all tools are connected to the tracker 
 *        in advance (no swapping tools in and out after initialization).
 */
class TrackerController : public Object
{

public:
  /** Macro with standard traits declarations (Self, SuperClass, State 
   *  Machine etc.). */
  igstkStandardClassTraitsMacro( TrackerController, Object )
 
  /**
   * Initialize a tracker using the given configuration. The tracker and 
   * communication are established and the tracker is in tracking mode.
   * NOTE: If a tracker was already initialized using this object it will be 
   *       shut down (stop tracking, close communication, and if using serial
   *       communication that will be closed too).
   *
   * @param configuration A specific tracker configuration (Aurora/Micron...)
   *                      which is identified at runtime.
   * This method generates two events, InitializationSuccessEvent if everything
   * went well and, InitializationFailureEvent if the tracker could not be 
   * initialized (the specific error is described in the string contained in the
   * error object).
   */
  void RequestInitialize(const TrackerConfiguration *configuration);

  void RequestStop(const TrackerConfiguration *configuration);

  void RequestStart(const TrackerConfiguration *configuration);

  /**
   * Stop the tracker and close its communication.
   * This method generates ShutdownSuccessEvent if everything
   * went well and, a ShutdownErrorEvent if there were problems during
   * shutdown.
   */
  void RequestShutdown();
  void RequestGetTracker();
  void RequestGetNonReferenceToolList();
  void RequestGetReferenceTool();

   /** This event is generated if the initialization succeeds. */
  igstkEventMacro( InitializeSuccessEvent, IGSTKEvent );

   /** This event is generated if the start tracking procedure succeeds. */
  igstkEventMacro( StartSuccessEvent, IGSTKEvent );

   /** This event is generated if the stop tracking procedure succeeds. */
  igstkEventMacro( StopSuccessEvent, IGSTKEvent );

   /** This event is generated if the shutdown succeeds. */
  igstkEventMacro( ShutdownSuccessEvent, IGSTKEvent );

   /** This event is generated if the initialization fails*/
  igstkLoadedEventMacro( InitializeFailureEvent, 
                         IGSTKErrorEvent, 
                         EventHelperType::StringType );

  /** This event is generated if the start tracking procedure fails*/
  igstkLoadedEventMacro( StartFailureEvent, 
                         IGSTKErrorEvent, 
                         EventHelperType::StringType );

   /** This event is generated if the user requests the tracker and it is
    *  initialized. */
  igstkLoadedEventMacro( RequestTrackerEvent, 
                         IGSTKEvent, 
                         Tracker::Pointer );

   /** This event is generated if the user requests the tracker tools and the 
    *  tracker is initialized. */
  igstkLoadedEventMacro( RequestToolsEvent, 
                         IGSTKEvent, 
                         std::vector<TrackerTool::Pointer> );

   /** This event is generated if the user requests the reference and the 
    *  tracker is initialized. */
  igstkLoadedEventMacro( RequestReferenceToolEvent, 
                         IGSTKEvent, 
                         TrackerTool::Pointer );

   /** This event is generated if the stop tracking procedure encountered difficulties */
  igstkLoadedEventMacro( StopFailureEvent, 
                         IGSTKErrorEvent, 
                         EventHelperType::StringType );

   /** This event is generated if the shutdown encountered difficulties 
    *   (e.g. tracker communication was already closed from outside). */
  igstkLoadedEventMacro( ShutdownFailureEvent, 
                         IGSTKErrorEvent, 
                         EventHelperType::StringType );

protected:

  TrackerController( void );
  ~TrackerController( void );


private:

  /** List of state machine states */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( AttemptingToInitialize );
  igstkDeclareStateMacro( AttemptingToStart );
  igstkDeclareStateMacro( AttemptingToStop );
  igstkDeclareStateMacro( AttemptingToShutdown );
  
  igstkDeclareStateMacro( AttemptingToInitializePolarisVicra );
  igstkDeclareStateMacro( AttemptingToInitializePolarisHybrid );
  igstkDeclareStateMacro( AttemptingToInitializeAurora );
  igstkDeclareStateMacro( AttemptingToInitializeMicron );
  igstkDeclareStateMacro( AttemptingToInitializeAscension );  
  igstkDeclareStateMacro( Initialized );
  igstkDeclareStateMacro( Started );

  /** List of state machine inputs */
  igstkDeclareInputMacro( TrackerInitialize );
  igstkDeclareInputMacro( TrackerStart );
  igstkDeclareInputMacro( TrackerStop );
  igstkDeclareInputMacro( TrackerShutdown );
  igstkDeclareInputMacro( PolarisVicraInitialize );
  igstkDeclareInputMacro( PolarisHybridInitialize );
  igstkDeclareInputMacro( AuroraInitialize );

  igstkDeclareInputMacro( MicronInitialize );
  igstkDeclareInputMacro( MicronStart );
  igstkDeclareInputMacro( MicronStop );

  igstkDeclareInputMacro( AscensionInitialize );
  igstkDeclareInputMacro( Failed  );
  igstkDeclareInputMacro( Succeeded  );
  igstkDeclareInputMacro( GetTracker  );
  igstkDeclareInputMacro( GetTools  );
  igstkDeclareInputMacro( GetReferenceTool  );

  /**List of state machine actions*/
  void TrackerInitializeProcessing();
  void TrackerStartProcessing();
  void TrackerStopProcessing();
  void TrackerShutdownProcessing();
  
  void PolarisVicraInitializeProcessing();
  void PolarisHybridInitializeProcessing();
  void AuroraInitializeProcessing();
  void MicronInitializeProcessing();
  void AscensionInitializeProcessing();

  void GetTrackerProcessing();
  void GetToolsProcessing();
  void GetReferenceToolProcessing();
  void ReportInvalidRequestProcessing();

  void ReportInitializationFailureProcessing();
  void ReportInitializationSuccessProcessing();

  void ReportStartFailureProcessing();
  void ReportStartSuccessProcessing();

  void ReportStopFailureProcessing();
  void ReportStopSuccessProcessing();

  void ReportShutdownFailureProcessing();
  void ReportShutdownSuccessProcessing();

  bool InitializeSerialCommunication();
  PolarisTrackerTool::Pointer InitializePolarisWirelessTool(
    const PolarisWirelessToolConfiguration *toolConfiguration );
  PolarisTrackerTool::Pointer InitializePolarisWiredTool(
    const PolarisWiredToolConfiguration *toolConfiguration );
  AuroraTrackerTool::Pointer InitializeAuroraTool(
    const AuroraToolConfiguration *toolConfiguration );
#ifdef IGSTKSandbox_USE_MicronTracker
  MicronTrackerTool::Pointer InitializeMicronTool(
    const MicronToolConfiguration *toolConfiguration );
#endif
  FlockOfBirdsTrackerTool::Pointer InitializeAscensionTool(
    const AscensionToolConfiguration *toolConfiguration );


  class ErrorObserver : public itk::Command
  {
  public:
    typedef ErrorObserver                    Self;
    typedef ::itk::Command                   Superclass;
    typedef ::itk::SmartPointer<Self>        Pointer;
    typedef ::itk::SmartPointer<const Self>  ConstPointer;

    igstkNewMacro(Self)
    igstkTypeMacro(ErrorObserver, itk::Command)

    /** When an error occurs in an IGSTK component it will invoke this method 
     *  with the appropriate error event object as a parameter.*/
    virtual void Execute(itk::Object *caller, 
                         const itk::EventObject & event) throw (std::exception);

    /** When an error occurs in an IGSTK component it will invoke this method 
     *  with the appropriate error event object as a parameter.*/
    virtual void Execute(const itk::Object *caller, 
                         const itk::EventObject & event) throw (std::exception);

    /**Clear the current error.*/
    void ClearError() { this->m_ErrorOccured = false; 
                        this->m_ErrorMessage.clear(); }
    /**If an error occurs in one of the observed IGSTK components this method 
     * will return true.*/
    bool ErrorOccured() { return this->m_ErrorOccured; }
    /**Get the error message associated with the last IGSTK error.*/
    void GetErrorMessage(std::string &errorMessage) 
    {
      errorMessage = this->m_ErrorMessage;
    }

  protected:

    /**Construct an error observer for all the possible errors that occur in 
     * the observed IGSTK components.*/
    ErrorObserver();
    virtual ~ErrorObserver(){}
  private:
    bool m_ErrorOccured;
    std::string m_ErrorMessage;
    std::map<std::string,std::string> m_ErrorEvent2ErrorMessage;

             //purposely not implemented
    ErrorObserver(const Self&);
    void operator=(const Self&); 
  };


  ErrorObserver::Pointer m_ErrorObserver;
  std::string m_ErrorMessage;

  TrackerConfiguration  *m_TmpTrackerConfiguration;
  TrackerConfiguration  *m_TrackerConfiguration;
  Tracker::Pointer m_Tracker;
  TrackerTool::Pointer m_ReferenceTool;
  std::vector<TrackerTool::Pointer> m_Tools;
  SerialCommunication::Pointer m_SerialCommunication;  
};

} // end of namespace
#endif
