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

#include "igstkConfigure.h"

#include "igstkTrackerConfiguration.h"
#include "igstkPolarisTrackerConfiguration.h"
#include "igstkAuroraTrackerConfiguration.h"
#include "igstkAscensionTrackerConfiguration.h"

#include "igstkMacros.h"
#include "igstkEvents.h"
#include "igstkObject.h"
#include "igstkSerialCommunication.h"
#include "igstkTransform.h"

#include "igstkTracker.h"
#include "igstkTrackerTool.h"
#include "igstkPolarisTrackerTool.h"
#include "igstkAuroraTrackerTool.h"
#include "igstkAscensionTrackerTool.h"

#include "igstkSpatialObject.h"

#ifdef IGSTK_USE_MicronTracker
#include "igstkMicronTrackerConfiguration.h"
#include "igstkMicronTrackerTool.h"
#endif

#ifdef IGSTK_USE_Ascension3DGTracker
#include "igstkAscension3DGTrackerConfiguration.h"
#include "igstkAscension3DGTrackerTool.h"
#endif

#ifdef IGSTK_USE_NDICertusTracker
#include "igstkCertusTrackerConfiguration.h"
#include "igstkNDICertusTrackerTool.h"
#endif



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
  /**This is the container type in which the tools are found.*/   
  typedef std::map < std::string, igstk::TrackerTool::Pointer > 
    ToolContainerType;

  typedef std::pair < std::string, igstk::TrackerTool::Pointer > 
    ToolEntryType;

  typedef Transform                                        TransformType;

  typedef SpatialObject                                    SpatialObjectType;

  /** Macro with standard traits declarations (Self, SuperClass, State 
   *  Machine etc.). */
  igstkStandardClassTraitsMacro( TrackerController, Object )
 
  /**
   * Initialize a tracker using the given configuration. Communication with the
   * haredware is established and the tracker is initialized.
   * NOTE: If a tracker was already initialized using this object it will be 
   *       shut down (stop tracking, close communication, and if using serial
   *       communication that will be closed too).
   *
   * @param configuration A specific tracker configuration (Aurora/Micron...)
   *                      which is identified at runtime.
   * This method generates two events, InitializeEvent if everything
   * went well and, InitializeErrorEvent if the tracker could not be 
   * initialized (the specific error is described in the string contained in the
   * error object).
   */
  void RequestInitialize(const TrackerConfiguration *configuration);

  /**
   * Start the tracking. The method generates igstk::TrackerStartTrackingEvent 
   * and igstk::TrackerStartTrackingErrorEvent (see igstkTracker.h).
   */
  void RequestStartTracking();

  /**
   * Stop the tracking. The method generates igstk::TrackerStopTrackingEvent and
   * igstk::TrackerStopTrackingErrorEvent (see igstkTracker.h).
   */
  void RequestStopTracking();

  /**
   * This method stops the tracking and closes communication.
   * The method generates igstk::TrackerStopTrackingEvent or 
   * igstk::TrackerStopTrackingErrorEvent followed by 
   * CloseCommunicationEvent or CloseCommunicationErrorEvent.
   */
  void RequestShutdown();

  /**
   * Get the container (ToolContainerType) with all the tools.   
   * The method generates a RequestToolsEvent if the tracker was previously 
   * initialized successfuly, otherwise it generates an 
   * InvalidRequestErrorEvent.*/
  void RequestGetNonReferenceToolList();

  /**
   * Get a specific tracker tool according to its unique name.   
   * The method generates a RequestToolEvent if the tracker was 
   * previously initialized successfuly and the tool exists, otherwise it 
   * generates a RequestToolErrorEvent if the tool doesn't exist, or
   * InvalidRequestErrorEvent if the tracker wasn't initialized.*/
  void RequestGetTool( const std::string &toolName );

  /**
   * Get the reference tool it it exists.   
   * The method generates a RequestToolEvent if the tracker was 
   * previously initialized successfuly, otherwise it generates an 
   * InvalidRequestErrorEvent.
   * Note that when no reference tool is used the payload of the event
   * will be a NULL pointer.*/
  void RequestGetReferenceTool();

  /** 
   * 
   * Sets the tracker's parent spatial object
   */
  void RequestSetParentSpatialObject( TransformType transform, 
                                      SpatialObjectType * spatialObject);

  /** 
  * Adds a Spatial Object as a child of the tracker. For example, if you want 
  * to display the tracker's working volume (e.g. frustum or cube).*/
  void RequestAddChildSpatialObject( TransformType transform, 
                                     SpatialObjectType * spatialObject);

  /** This event is generated if the initialization succeeds. */
  igstkEventMacro( InitializeEvent, IGSTKEvent );

  /** This event is generated if the initialization fails.*/
  igstkEventMacro( InitializeErrorEvent, IGSTKErrorWithStringEvent ); 

  /** This event is generated if the communication was closed successfully. */
  igstkEventMacro( CloseCommunicationEvent, IGSTKEvent );

  /** This event is generated if closing communication failed.*/
  igstkEventMacro( CloseCommunicationErrorEvent, IGSTKErrorWithStringEvent ); 

  /** This event is generated if the user requests the tracker tools and the 
   *  tracker is initialized. */
  igstkLoadedEventMacro( RequestToolsEvent, 
                         IGSTKEvent, 
                         ToolContainerType );

  /** This event is generated if the user requests a specific tracker tool and 
   *  the tracker is initialized. */
  igstkLoadedEventMacro( RequestToolEvent, 
                         IGSTKEvent, 
                         ToolEntryType );

  /** This event is generated if the user requested a specific tracker tool and
   *  the tool does not exist.*/
  igstkEventMacro( RequestToolErrorEvent, 
                   IGSTKErrorEvent );

protected:

  TrackerController( void );
  virtual ~TrackerController( void );


private:   

  /** List of state machine states */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( AttemptingToInitialize );
  igstkDeclareStateMacro( AttemptingToInitializePolarisVicra );
  igstkDeclareStateMacro( AttemptingToInitializePolarisHybrid );
  igstkDeclareStateMacro( AttemptingToInitializeAurora );
  igstkDeclareStateMacro( AttemptingToInitializeAscension );
  igstkDeclareStateMacro( AttemptingToInitializeMicron );
  igstkDeclareStateMacro( AttemptingToInitializeAscension3DG );
  igstkDeclareStateMacro( AttemptingToInitializeCertus );
  igstkDeclareStateMacro( Initialized );
  igstkDeclareStateMacro( AttemptingToStartTracking );
  igstkDeclareStateMacro( Tracking );
  igstkDeclareStateMacro( AttemptingToCloseCommunication );
  igstkDeclareStateMacro( AttemptingToStopTracking );

  /** List of state machine inputs */
  igstkDeclareInputMacro( TrackerInitialize );
  igstkDeclareInputMacro( PolarisVicraInitialize );
  igstkDeclareInputMacro( PolarisHybridInitialize );
  igstkDeclareInputMacro( AuroraInitialize );
  igstkDeclareInputMacro( AscensionInitialize );
  igstkDeclareInputMacro( MicronInitialize );  
  igstkDeclareInputMacro( Ascension3DGInitialize );  
  igstkDeclareInputMacro( CertusInitialize );
  igstkDeclareInputMacro( StartTracking );
  igstkDeclareInputMacro( StopTracking );
  igstkDeclareInputMacro( Failed  );
  igstkDeclareInputMacro( Succeeded  );
  igstkDeclareInputMacro( CloseCommunication );
  igstkDeclareInputMacro( GetTools  );
  igstkDeclareInputMacro( GetTool  );
  igstkDeclareInputMacro( GetReferenceTool  );
  igstkDeclareInputMacro( SetParentSpatialObject  );
  igstkDeclareInputMacro( SetChildSpatialObject  );


  /**List of state machine actions*/
  void TrackerInitializeProcessing();  
  void PolarisVicraInitializeProcessing();
  void PolarisHybridInitializeProcessing();
  void AuroraInitializeProcessing();
  void AscensionInitializeProcessing();
  void MicronInitializeProcessing();
  void Ascension3DGInitializeProcessing();
  void CertusInitializeProcessing();
  void StartTrackingProcessing();
  void StopTrackingProcessing();
  void CloseCommunicationProcessing();
  void GetToolsProcessing();
  void GetToolProcessing();
  void GetReferenceToolProcessing();
  void SetParentSpatialObjectProcessing();
  void SetChildSpatialObjectProcessing();

  void ReportInvalidRequestProcessing();

  void ReportInitializationFailureProcessing();
  void ReportInitializationSuccessProcessing();

  void ReportStartTrackingFailureProcessing();
  void ReportStartTrackingSuccessProcessing();

  void ReportStopTrackingFailureProcessing();
  void ReportStopTrackingSuccessProcessing();

  void ReportCloseCommunicationFailureProcessing();
  void ReportCloseCommunicationSuccessProcessing();

  bool InitializeSerialCommunication();
  PolarisTrackerTool::Pointer InitializePolarisWirelessTool(
    const PolarisWirelessToolConfiguration *toolConfiguration );
  PolarisTrackerTool::Pointer InitializePolarisWiredTool(
    const PolarisWiredToolConfiguration *toolConfiguration );
  AuroraTrackerTool::Pointer InitializeAuroraTool(
    const AuroraToolConfiguration *toolConfiguration );

#ifdef IGSTK_USE_MicronTracker

  MicronTrackerTool::Pointer InitializeMicronTool(
    const MicronToolConfiguration *toolConfiguration );

#endif

#ifdef IGSTK_USE_Ascension3DGTracker

  Ascension3DGTrackerTool::Pointer InitializeAscension3DGTool(
    const Ascension3DGToolConfiguration *toolConfiguration );

#endif

  AscensionTrackerTool::Pointer InitializeAscensionTool(
    const AscensionToolConfiguration *toolConfiguration );

#ifdef IGSTK_USE_NDICertusTracker
  NDICertusTrackerTool::Pointer InitializeCertusTool(
	const CertusToolConfiguration *toolConfiguration );
#endif

  /**
   * \class Observer for the TrackerToolAttachmentToTrackerErrorEvent.
   */
  class ToolAttachErrorObserver : public ::itk::Command 
  { 
  public:
    typedef  ToolAttachErrorObserver    Self; 
    typedef  ::itk::Command             Superclass;
    typedef  ::itk::SmartPointer<Self>  Pointer;
    itkNewMacro( Self );

    void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    const itk::Object * constCaller = caller;
    this->Execute( constCaller, event );
    }


    void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & event)
    {
      if( dynamic_cast<const TrackerToolAttachmentToTrackerErrorEvent *> ( 
          &event ) )
      {                
        m_GotObject = true;
      }
    }

    bool GotToolAttachError() const
    {
      return m_GotObject;
    }
    void Reset() 
    {
      m_GotObject = false; 
    }

  protected:
    ToolAttachErrorObserver()
    {
      m_GotObject = false;
    }
    ~ToolAttachErrorObserver() { }
  private:
    bool m_GotObject;
  };

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
    bool                               m_ErrorOccured;
    std::string                        m_ErrorMessage;
    std::map<std::string,std::string>  m_ErrorEvent2ErrorMessage;

    //purposely not implemented
    ErrorObserver(const Self&);
    void operator=(const Self&); 
    };

  class TrackerUpdateObserver : public itk::Command
    {

    public:
    typedef TrackerUpdateObserver                    Self;
    typedef ::itk::Command                           Superclass;
    typedef ::itk::SmartPointer<Self>                Pointer;
    typedef ::itk::SmartPointer<const Self>          ConstPointer;

    void SetParent( TrackerController *parent );

    igstkNewMacro(Self)
    igstkTypeMacro(TrackerUpdateObserver, itk::Command)

    /** When an error occurs in an IGSTK component it will invoke this method 
     *  with the appropriate error event object as a parameter.*/
    virtual void Execute(itk::Object *caller, 
                         const itk::EventObject & event) throw (std::exception);

    /** When an error occurs in an IGSTK component it will invoke this method 
     *  with the appropriate error event object as a parameter.*/
    virtual void Execute(const itk::Object *caller, 
                         const itk::EventObject & event) throw (std::exception);
    private:

    TrackerController *m_parent;
    };

  //this object observes the tracker's igstk::TrackerUpdateStatusEvent and
  //igstk::TrackerUpdateStatusErrorEvent it then causes its parent to invoke
  //these events too. In this manner the user is inderictly observing the 
  //tracker
  TrackerUpdateObserver::Pointer m_TrackerUpdateStatusObserver;

  ErrorObserver::Pointer m_ErrorObserver;
  std::string m_ErrorMessage;

  SpatialObjectType     *m_TmpSpatialObject;
  TransformType          m_TmpTransform;

  TrackerConfiguration  *m_TmpTrackerConfiguration;
  TrackerConfiguration  *m_TrackerConfiguration;
  std::string m_RequestedToolName;

  Tracker::Pointer m_Tracker;
  std::map<std::string, TrackerTool::Pointer> m_Tools; 
  TrackerTool::Pointer m_ReferenceTool;
  SerialCommunication::Pointer m_SerialCommunication;     
};

} // end of namespace
#endif
