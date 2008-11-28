/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    UltrasoundCalibrationWizard.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __UltrasoundCalibrationWizard_h
#define __UltrasoundCalibrationWizard_h

#include "UltrasoundCalibrationWizardGUI.h"

#include "igstkSandboxConfigure.h"

#include "igstkStateMachine.h"
#include "igstkMacros.h"
#include "igstkObject.h"
#include "igstkEvents.h"

#include "igstkTracker.h"
#include "igstkTrackerTool.h"

#include "igstkImagerController.h"
#include "igstkImagerConfiguration.h"

#include "igstkEllipsoidObject.h"
#include "igstkEllipsoidObjectRepresentation.h"

#include "igstkCrossHairObject.h"
#include "igstkCrossHairRepresentation.h"

#include "igstkMeshReader.h"
#include "igstkMeshObjectRepresentation.h"

#include "igstkAxesObject.h"
#include "igstkAxesObjectRepresentation.h"

#include "igstkVideoFrameSpatialObject.h"
#include "igstkVideoFrameRepresentation.h"

#include "itkStdStreamLogOutput.h"

#include "igstkTrackerController.h"
#include "igstkTrackerConfiguration.h"

#include "igstkReslicerPlaneSpatialObject.h"
#include "igstkMeshResliceSpatialObjectRepresentation.h"

#include "igstkMicronTrackerConfiguration.h"
#include "igstkAuroraTrackerConfiguration.h"

#include "igstkTransformFileReader.h"
#include "igstkRigidTransformXMLFileReader.h"
#include "igstkTrackerConfigurationFileReader.h"
#include "igstkTrackerConfigurationXMLFileReaderBase.h"

/** \class UltrasoundCalibrationWizard
* 
* \brief Implementation class for UltrasoundCalibrationWizardGUI.
*
* This class implements the main application.
*
*/
class UltrasoundCalibrationWizard : public UltrasoundCalibrationWizardGUI
{
public:

  /** Typedefs */
  igstkStandardClassBasicTraitsMacro( UltrasoundCalibrationWizard, UltrasoundCalibrationWizardGUI );
  
  igstkLoggerMacro();

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

  /** typedef for mesh readers */
  typedef igstk::MeshReader                           MeshReaderType;

  /** typedef for mesh spatial object */
  typedef MeshReaderType::MeshObjectType              MeshObjectType;

  /** typedef for mesh spatial object representation */
  typedef igstk::MeshObjectRepresentation             MeshRepresentationType;

  /** typedef for ellipsoid spatial object */
  typedef igstk::EllipsoidObject                      EllipsoidType;

  /** typedef for ellipsoid spatial object representation*/
  typedef igstk::EllipsoidObjectRepresentation        EllipsoidRepresentationType;

  /** typedef for a vector of ellipsoid spatial objects and representations*/
  typedef std::vector < EllipsoidType::Pointer >      EllipsoidSpatialObjectVectorType;

  typedef std::vector < EllipsoidRepresentationType::Pointer >    
                                                      EllipsoidRepresentationVectorType;
  /** reslicer plane spatial object */
  typedef igstk::ReslicerPlaneSpatialObject           ReslicerPlaneType;

  /** mesh reslice representation */
  typedef igstk::MeshResliceSpatialObjectRepresentation  MeshResliceRepresentationType;

  /** Cross hair object and representation */
  typedef igstk::CrossHairObject                      CrossHairType;
  typedef igstk::CrossHairRepresentation              CrossHairRepresentationType; 

  /** typedef for axes spatial objects */
  typedef igstk::AxesObject                           AxesObjectType;
  typedef igstk::AxesObjectRepresentation             AxesRepresentationType;

  /** tracker tool spatial object and representation */
  MeshObjectType::Pointer                               m_TrackerToolSpatialObject;
  MeshRepresentationType::Pointer                       m_TrackerToolRepresentation;

    /** video frame spatial object and representation*/
  typedef igstk::VideoFrameSpatialObject                VideoFrameSpatialObjectType;
  VideoFrameSpatialObjectType::Pointer                  m_VideoFrame;

  typedef igstk::VideoFrameRepresentation               VideoFrameRepresentationType;
  VideoFrameRepresentationType::Pointer                 m_VideoFrameRepresentationForVideoView;
  VideoFrameRepresentationType::Pointer                 m_VideoFrameRepresentationFor3DView;

  /** typedef for the vector of tracker tools */
  typedef std::vector < igstk::TrackerTool::Pointer >   ToolVectorType;

    /** imager controller object */
  igstk::ImagerController::Pointer                      m_ImagerController;

  /** imager controller configuration */
  igstk::TerasonImagerConfiguration                    *m_ImagerConfiguration;

  /** imager object and imager tool*/
  igstk::Imager::Pointer                                m_Imager;
  igstk::ImagerTool::Pointer                            m_TerasonImagerTool;


  bool m_VideoEnabled;
  bool m_ImagerInitialized;
  bool m_VideoRunning;


  /** Public request methods from the GUI */
  virtual void RequestLoadTrackerMesh();
  virtual void RequestLoadWorkingVolumeMesh();
  virtual void RequestPrepareToQuit();
  virtual void RequestLoadTrackerToolSpatialObject();
  virtual void RequestToggleEnableVideo();
  virtual void RequestToggleRunVideo();

  void EnableVideo();
  void DisableVideo();

  void StartVideo();
  void StopVideo();

  /**
   * Open the relevant tracker settings dialog (Polaris/Aurora/Micron/Ascension)
   * and get the users input.
   */
  virtual void RequestConfigureTracker();
  virtual void RequestInitializeTracker();  
  virtual void RequestStartTracking();
  virtual void RequestStopTracking();
  virtual void RequestDisconnectTracker();
  virtual void RequestInitializeImager();
  virtual void RequestStartImaging();
  virtual void ReportSuccessStartImagingProcessing();
  virtual void ReportFailureStartImagingProcessing();
  virtual void RequestStopImaging();
  virtual void RequestDisconnectImager();

    /**
   * Observer for the event generated by 
   * TrackerConfigurationFileReader->RequestGetData() method.
   */
  igstkObserverObjectMacro( TrackerConfiguration, 
                   igstk::TrackerConfigurationFileReader::TrackerConfigurationDataEvent,
                   igstk::TrackerConfiguration)

  igstkObserverMacro( TransformData, 
                    igstk::TransformFileReader::TransformDataEvent, 
                    igstk::PrecomputedTransformData::Pointer )

  igstkObserverMacro( TransformationDescription, 
                      igstk::StringEvent, 
                      std::string )

  igstkObserverMacro( TransformationDate, 
                      igstk::PrecomputedTransformData::TransformDateTypeEvent, 
                      std::string )

  typedef igstk::TrackerConfigurationFileReader::ReadTrackerConfigurationFailSuccessObserver
                    ReadTrackerConfigurationFailSuccessObserverType;

  igstkObserverMacro( TransformError, 
                      igstk::PrecomputedTransformData::TransformErrorTypeEvent, 
                      igstk::PrecomputedTransformData::ErrorType )


  /** Define observers for event communication */

  igstkObserverObjectMacro( MeshObject, igstk::MeshReader::MeshModifiedEvent,
                                        igstk::MeshObject);

  igstkEventMacro( InvalidRequestErrorEvent, igstk::IGSTKErrorEvent );

  UltrasoundCalibrationWizard();
  virtual ~UltrasoundCalibrationWizard();

private:

  UltrasoundCalibrationWizard(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented
  
  /** States for the State Machine */
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( LoadingTrackerToolSpatialObject );
  igstkDeclareStateMacro( TrackerToolSpatialObjectReady );  
  igstkDeclareStateMacro( ConfiguringTracker );
  igstkDeclareStateMacro( TrackerConfigurationReady );
  igstkDeclareStateMacro( InitializingTracker );
  igstkDeclareStateMacro( TrackerInitializationReady );
  igstkDeclareStateMacro( LoadingMesh );
  igstkDeclareStateMacro( StartingTracker );
  igstkDeclareStateMacro( StoppingTracker );
  igstkDeclareStateMacro( DisconnectingTracker );
  igstkDeclareStateMacro( Tracking );
  igstkDeclareStateMacro( LoadingImagerToolSpatialObject );
  igstkDeclareStateMacro( InitializingImager );
  igstkDeclareStateMacro( TrackingAndImaging );
  igstkDeclareStateMacro( StartingImager );
  igstkDeclareStateMacro( StoppingImager );
  igstkDeclareStateMacro( DisconnectingImager );
  
  /** Inputs to the state machine and it's designed transitions */
  igstkDeclareInputMacro( Success );
  igstkDeclareInputMacro( Failure );
  igstkDeclareInputMacro( LoadTrackerMesh );
  igstkDeclareInputMacro( LoadWorkingVolumeMesh );
  igstkDeclareInputMacro( ConfigureTracker );
  igstkDeclareInputMacro( LoadTrackerToolSpatialObject );  
  igstkDeclareInputMacro( InitializeTracker );
  igstkDeclareInputMacro( StartTracking );
  igstkDeclareInputMacro( StopTracking );
  igstkDeclareInputMacro( DisconnectTracker );  
  igstkDeclareInputMacro( LoadImagerToolSpatialObject );
  igstkDeclareInputMacro( InitializeImager );
  igstkDeclareInputMacro( StartImaging );
  igstkDeclareInputMacro( StopImaging );
  igstkDeclareInputMacro( DisconnectImager );


  /** Define a initial world coordinate system */
  AxesObjectType::Pointer                               m_WorldReference;
  AxesRepresentationType::Pointer                       m_WorldReferenceRepresentation;

  /** SpatialObject and representation for the tracker´s working volume */
  MeshObjectType::Pointer                               m_MeshSpatialObject;
  MeshRepresentationType::Pointer                       m_MeshRepresentation;

  /** SpatialObject and representation for the tracker´s tip */
  EllipsoidSpatialObjectVectorType                      m_TipSpatialObjectVector;
  EllipsoidRepresentationVectorType                     m_TipRepresentationVector;

  /** tracker configuration object */
  const igstk::TrackerConfiguration *                   m_TrackerConfiguration;

  /** cross hair object and representation */
  CrossHairType::Pointer                                m_CrossHair;
  CrossHairRepresentationType::Pointer                  m_CrossHairRepresentation;

  /** plane reslicer spatial objects */
  ReslicerPlaneType::Pointer                            m_AxialPlaneSpatialObject;
  ReslicerPlaneType::Pointer                            m_SagittalPlaneSpatialObject;
  ReslicerPlaneType::Pointer                            m_CoronalPlaneSpatialObject;
 
  MeshResliceRepresentationType::Pointer                m_AxialMeshResliceRepresentation;
  MeshResliceRepresentationType::Pointer                m_SagittalMeshResliceRepresentation;
  MeshResliceRepresentationType::Pointer                m_CoronalMeshResliceRepresentation;

  /** Class that observes the events generated by the tracker controller. */
  class TrackerControllerObserver : public itk::Command
  {
  public:
    typedef TrackerControllerObserver            Self;
    typedef ::itk::Command                       Superclass;
    typedef ::itk::SmartPointer<Self>            Pointer;
    typedef ::itk::SmartPointer<const Self>      ConstPointer;

    igstkNewMacro(Self)
    igstkTypeMacro(TrackerControllerObserver, itk::Command)

    void SetParent(UltrasoundCalibrationWizard *p); 

      /** Invoke this method when an initialization event occurs. */
    void Execute(itk::Object *caller, const itk::EventObject & event); 
    void Execute(const itk::Object *caller, const itk::EventObject & event);
  
    /**
     * Clear the current error.
     */
    void ClearError() 
    {
      this->m_ErrorOccured = false; 
      this->m_ErrorMessage.clear();
    }
    /**
     * If an error occurs in one of the observed IGSTK components this method will return true.
     */
    bool Error() 
    {
      return this->m_ErrorOccured;
    }

    void GetErrorMessage(std::string &errorMessage) 
    {
      errorMessage = this->m_ErrorMessage;
    }


  protected:
    TrackerControllerObserver() : m_Parent(NULL), m_ErrorOccured(false) {}
    virtual ~TrackerControllerObserver(){}


  private:
    UltrasoundCalibrationWizard *m_Parent;
    bool m_ErrorOccured;
    std::string m_ErrorMessage;

    //purposely not implemented
    TrackerControllerObserver(const Self&);
    void operator=(const Self&); 
  };


  /** Class that observes the events generated by the imager controller. */
  class ImagerControllerObserver : public itk::Command
  {
  public:
    typedef ImagerControllerObserver             Self;
    typedef ::itk::Command                       Superclass;
    typedef ::itk::SmartPointer<Self>            Pointer;
    typedef ::itk::SmartPointer<const Self>      ConstPointer;

    igstkNewMacro(Self)
    igstkTypeMacro(ImagerControllerObserver, itk::Command)

    void SetParent(UltrasoundCalibrationWizard *p); 

      /** Invoke this method when an initialization event occurs. */
    void Execute(itk::Object *caller, const itk::EventObject & event); 
    void Execute(const itk::Object *caller, const itk::EventObject & event);
  
    /**
     * Clear the current error.
     */
    void ClearError() 
    {
      this->m_ErrorOccured = false; 
      this->m_ErrorMessage.clear();
    }
    /**
     * If an error occurs in one of the observed IGSTK components this method will return true.
     */
    bool Error() 
    {
      return this->m_ErrorOccured;
    }

    void GetErrorMessage(std::string &errorMessage) 
    {
      errorMessage = this->m_ErrorMessage;
    }


  protected:
    ImagerControllerObserver() : m_Parent(NULL), m_ErrorOccured(false) {}
    virtual ~ImagerControllerObserver(){}


  private:
    UltrasoundCalibrationWizard *m_Parent;
    bool m_ErrorOccured;
    std::string m_ErrorMessage;

    //purposely not implemented
    ImagerControllerObserver(const Self&);
    void operator=(const Self&); 
  };

  /** Class that observes the events generated by the transform reader. */
  class ReadObserver : public itk::Command
  {
  public:
    typedef ReadObserver                    Self;
    typedef ::itk::Command                   Superclass;
    typedef ::itk::SmartPointer<Self>        Pointer;
    typedef ::itk::SmartPointer<const Self>  ConstPointer;

    igstkNewMacro( Self )
    igstkTypeMacro( ReadObserver, itk::Command )

    virtual void Execute( itk::Object *caller, const itk::EventObject & event )
    {
      const igstk::TransformFileReader::ReadFailureEvent *rfe;
      if( ( rfe = dynamic_cast<const igstk::TransformFileReader::ReadFailureEvent *>( &event ) ) ) 
        std::cerr<<rfe->Get()<<"\n";
      else if( dynamic_cast<const igstk::TransformFileReader::ReadSuccessEvent *>( &event ) )
        ( static_cast<igstk::TransformFileReader *>( caller ) )->RequestGetData();
    }

    virtual void Execute( const itk::Object *caller, 
                          const itk::EventObject & event )
    {
      const itk::Object * constCaller = caller;
      this->Execute( constCaller, event );
    }
  protected:

    ReadObserver(){}
    virtual ~ReadObserver(){}
  private:
    //purposely not implemented
    ReadObserver(const Self&);
    void operator=(const Self&); 
  };

  igstk::TrackerController::Pointer                     m_TrackerController;

  /** tracker controller observer object */
  TrackerControllerObserver::Pointer                    m_TrackerControllerObserver;
  /** imager controller observer object */
  ImagerControllerObserver::Pointer                     m_ImagerControllerObserver;

  ToolVectorType                                        m_ToolVector;
  
  /** Action methods to be invoked only by the state machine */
  void ReportInvalidRequestProcessing();
  void ReportSuccessSpatialObjectLoadedProcessing();
  void ReportFailureSpatialObjectLoadedProcessing();
  void ReportSuccessMeshLoadedProcessing();
  void ReportFailureMeshLoadedProcessing();
  void ReportSuccessImagerInitializationProcessing();
  void ReportFailureImagerInitializationProcessing();
  void ReportSuccessTrackerConfigurationProcessing();
  void ReportFailureTrackerConfigurationProcessing();
  void ReportSuccessTrackerInitializationProcessing();
  void ReportFailureTrackerInitializationProcessing();
  void ReportSuccessTrackerDisconnectionProcessing();
  void ReportFailureTrackerDisconnectionProcessing();  
  void ReportSuccessStopTrackingProcessing();
  void ReportFailureStopTrackingProcessing();
  void ReportSuccessStartTrackingProcessing();
  void ReportFailureStartTrackingProcessing();
  void LoadTrackerMeshProcessing();
  void LoadWorkingVolumeMeshProcessing();
  void ConfigureTrackerProcessing();
  void InitializeTrackerProcessing();
  void StartTrackingProcessing();
  void StopTrackingProcessing();
  void DisconnectTrackerProcessing();
  void LoadTrackerToolSpatialObjectProcessing();
  void ReportFailureTrackerToolSpatialObjectLoadedProcessing();
  void ReportSuccessTrackerToolSpatialObjectLoadedProcessing();
  void ReportSuccessImagerToolSpatialObjectLoadedProcessing();
  void ReportFailureImagerToolSpatialObjectLoadedProcessing();
  void ReportSuccessStopImagingProcessing();
  void ReportFailureStopImagingProcessing();
  void ReportSuccessImagerDisconnectionProcessing();
  void ReportFailureImagerDisconnectionProcessing();
  void StopImagingProcessing();
  void StartImagingProcessing();
  void DisconnectImagerProcessing();
  void InitializeImagerProcessing();

  /** Observer type for loaded event, 
   *  the callback can be set to a member function. */
  typedef itk::ReceptorMemberCommand < Self > LoadedObserverType;
  typedef itk::SimpleMemberCommand < Self > CancelObserverType;
  
  typedef itk::MemberCommand<UltrasoundCalibrationWizard>   ProgressCommandType;
  itk::SmartPointer<ProgressCommandType>                m_SocketProgressCommand;  

  CancelObserverType::Pointer               m_TrackerConfigurationCancelObserver;
      
  /** Log file */
  std::ofstream                                   m_LogFile;  

  bool ReadMicronConfiguration(igstk::TrackerConfigurationFileReader::Pointer baseReader);
  bool ReadAuroraConfiguration(igstk::TrackerConfigurationFileReader::Pointer baseReader);
  bool ReadPolarisVicraConfiguration(igstk::TrackerConfigurationFileReader::Pointer baseReader);
  bool ReadPolarisSpectraConfiguration(igstk::TrackerConfigurationFileReader::Pointer baseReader);
  bool ReadPolarisHybridConfiguration(igstk::TrackerConfigurationFileReader::Pointer baseReader);

  void BuildSceneGraph();

  /** Callback functions for picking and reslicing image events. */
  //void ToolTrackingCallback( const itk::EventObject & event );
  //void ToolNotAvailableCallback( const itk::EventObject & event );
  //void ToolAvailableCallback( const itk::EventObject & event );
  void NullActionCallback(const itk::EventObject & event ){};
  void OnSocketProgressEvent(itk::Object *source, const itk::EventObject &);
};

#endif
