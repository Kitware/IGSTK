/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    VideoTemporalCalibrationWizard.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __VideoTemporalCalibrationWizard_h
#define __VideoTemporalCalibrationWizard_h

#include "VideoTemporalCalibrationWizardGUI.h"

#include "igstkSandboxConfigure.h"

#include "igstkStateMachine.h"
#include "igstkMacros.h"
#include "igstkObject.h"
#include "igstkEvents.h"

#include "igstkTracker.h"
#include "igstkTrackerTool.h"

#include "igstkVideoVideoImagerController.h"
#include "igstkVideoVideoImagerConfiguration.h"

#include "igstkVideoTemporalCalibration.h"

#include "igstkEllipsoidObject.h"
#include "igstkEllipsoidObjectRepresentation.h"

#include "igstkCrossHairSpatialObject.h"
#include "igstkCrossHairObjectRepresentation.h"

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
#include "igstkMeshResliceObjectRepresentation.h"

#include "igstkMicronTrackerConfiguration.h"
#include "igstkAuroraTrackerConfiguration.h"
#include "igstkPolarisTrackerConfiguration.h"

#include "igstkTransformFileReader.h"
#include "igstkRigidTransformXMLFileReader.h"
#include "igstkTrackerConfigurationFileReader.h"
#include "igstkTrackerConfigurationXMLFileReaderBase.h"

/** \class VideoTemporalCalibrationWizard
*
* \brief Implementation class for VideoTemporalCalibrationWizardGUI.
*
* This class implements the main application.
*
*/
class VideoTemporalCalibrationWizard : public VideoTemporalCalibrationWizardGUI
{
public:

  /** Typedefs */
  igstkStandardClassBasicTraitsMacro( VideoTemporalCalibrationWizard, VideoTemporalCalibrationWizardGUI );

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
//  typedef std::vector < EllipsoidType::Pointer >      EllipsoidSpatialObjectVectorType;

//  typedef std::vector < EllipsoidRepresentationType::Pointer >
//                                                      EllipsoidRepresentationVectorType;
  /** reslicer plane spatial object */
  typedef igstk::ReslicerPlaneSpatialObject           ReslicerPlaneType;

  /** mesh reslice representation */
  typedef igstk::MeshResliceObjectRepresentation  MeshResliceRepresentationType;

  /** Cross hair object and representation */
  typedef igstk::CrossHairSpatialObject                      CrossHairType;
  typedef igstk::CrossHairObjectRepresentation              CrossHairRepresentationType;

  /** typedef for axes spatial objects */
  typedef igstk::AxesObject                           AxesObjectType;
  typedef igstk::AxesObjectRepresentation             AxesRepresentationType;

  /** typedef for a point type */
  typedef igstk::Transform::PointType                 PointType;

  /** tracker tool spatial object and representation */
  MeshObjectType::Pointer                               m_TrackerToolSpatialObject;
  MeshRepresentationType::Pointer                       m_TrackerToolRepresentation;

  /** VideoVideoImager tool spatial object and representation */
  EllipsoidType::Pointer                                m_VideoVideoImagerToolSpatialObject;
  EllipsoidRepresentationType::Pointer                  m_VideoImagerToolRepresentation;

  /** video frame spatial object and representation*/
  typedef igstk::VideoFrameSpatialObject<unsigned char, 3 >   VideoFrameSpatialObjectType;
  VideoFrameSpatialObjectType::Pointer                  m_VideoFrame;

  typedef igstk::VideoFrameRepresentation<VideoFrameSpatialObjectType>
                                VideoFrameRepresentationType;
  VideoFrameRepresentationType::Pointer                 m_VideoFrameRepresentationForVideoView;
  VideoFrameRepresentationType::Pointer                 m_VideoFrameRepresentationFor3DView;

  double                                                m_WindowLevelVideo;
  double                                                m_WindowWidthVideo;

  /** typedef for the vector of tracker tools */
  typedef std::vector < igstk::TrackerTool::Pointer >   ToolVectorType;

  /** VideoImager object and VideoImager tool*/
  igstk::VideoImager::Pointer                                m_VideoImager;
  igstk::VideoImagerTool::Pointer                            m_ImagingSourceVideoImagerTool;

  /** Observer type for loaded event,
   *  the callback can be set to a member function. */
  typedef itk::ReceptorMemberCommand < Self > LoadedObserverType;

  /** observer for key pressed event */
  LoadedObserverType::Pointer                           m_KeyPressedObserver;

  /** observer for mouse button pressed event */
  LoadedObserverType::Pointer                           m_MousePressedObserver;

  /** observers for the tracker tool */
  LoadedObserverType::Pointer               m_TrackerToolUpdateObserver;
  LoadedObserverType::Pointer               m_TrackerToolNotAvailableObserver;
  LoadedObserverType::Pointer               m_TrackerToolAvailableObserver;

  /** observers for the VideoImager tool */
  LoadedObserverType::Pointer               m_VideoImagerToolUpdateObserver;
  LoadedObserverType::Pointer               m_VideoImagerToolNotAvailableObserver;
  LoadedObserverType::Pointer               m_VideoImagerToolAvailableObserver;

  bool m_CollectorEnabled;
  bool m_VideoEnabled;
  bool m_VideoImagerInitialized;
  bool m_VideoRunning;
  bool m_CollectingProbeSamples;
  bool m_CollectingPointerSamples;

  unsigned int m_TrackerToolUpdateObserverID;
  unsigned int m_VideoImagerToolUpdateObserverID;

  unsigned int m_SnapShotCounter;

  /** Public request methods from the GUI */
  virtual void RequestLoadTrackerMesh();
  virtual void RequestLoadWorkingVolumeMesh();
  virtual void RequestPrepareToQuit();
  virtual void RequestLoadTrackerToolSpatialObject();
  virtual void RequestToggleEnableVideo();
  virtual void RequestToggleEnableCollector();
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
  virtual void RequestInitializeVideoImager();
  virtual void RequestStartImaging();
  virtual void ReportSuccessStartImagingProcessing();
  virtual void ReportFailureStartImagingProcessing();
  virtual void RequestStopImaging();
  virtual void RequestDisconnectVideoImager();

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

  VideoTemporalCalibrationWizard();
  virtual ~VideoTemporalCalibrationWizard();

private:

  VideoTemporalCalibrationWizard(const Self&); // purposely not implemented
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
  igstkDeclareStateMacro( LoadingVideoImagerToolSpatialObject );
  igstkDeclareStateMacro( InitializingVideoImager );
  igstkDeclareStateMacro( TrackingAndImaging );
  igstkDeclareStateMacro( StartingVideoImager );
  igstkDeclareStateMacro( StoppingVideoImager );
  igstkDeclareStateMacro( DisconnectingVideoImager );

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
  igstkDeclareInputMacro( LoadVideoImagerToolSpatialObject );
  igstkDeclareInputMacro( InitializeVideoImager );
  igstkDeclareInputMacro( StartImaging );
  igstkDeclareInputMacro( StopImaging );
  igstkDeclareInputMacro( DisconnectVideoImager );


  /** Define a initial world coordinate system */
  AxesObjectType::Pointer                               m_WorldReference;
  AxesRepresentationType::Pointer                       m_WorldReferenceRepresentation;

  /** SpatialObject and representation for the tracker�s working volume */
  MeshObjectType::Pointer                               m_MeshSpatialObject;
  MeshRepresentationType::Pointer                       m_MeshRepresentation;

  /** cross hair object and representation */
  CrossHairType::Pointer                                m_CrossHair;
  CrossHairRepresentationType::Pointer                  m_CrossHairRepresentation;

  /** VideoTemporal calibration object, in which we collect the samples*/
  igstk::VideoTemporalCalibration                        * m_VideoTemporalCalibrationSamples;

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

    void SetParent(VideoTemporalCalibrationWizard *p);

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
    VideoTemporalCalibrationWizard *m_Parent;
    bool m_ErrorOccured;
    std::string m_ErrorMessage;

    //purposely not implemented
    TrackerControllerObserver(const Self&);
    void operator=(const Self&);
  };


  /** Class that observes the events generated by the VideoImager controller. */
  class VideoImagerControllerObserver : public itk::Command
  {
  public:
    typedef VideoImagerControllerObserver             Self;
    typedef ::itk::Command                       Superclass;
    typedef ::itk::SmartPointer<Self>            Pointer;
    typedef ::itk::SmartPointer<const Self>      ConstPointer;

    igstkNewMacro(Self)
    igstkTypeMacro(VideoImagerControllerObserver, itk::Command)

    void SetParent(VideoTemporalCalibrationWizard *p);

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
    VideoImagerControllerObserver() : m_Parent(NULL), m_ErrorOccured(false) {}
    virtual ~VideoImagerControllerObserver(){}


  private:
    VideoTemporalCalibrationWizard *m_Parent;
    bool m_ErrorOccured;
    std::string m_ErrorMessage;

    //purposely not implemented
    VideoImagerControllerObserver(const Self&);
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

  inline
  PointType
  TransformToPoint( igstk::Transform transform)
  {
    PointType point;
    for (unsigned int i=0; i<3; i++)
      {
      point[i] = transform.GetTranslation()[i];
      }
    return point;
  }

  /** tracker controller object */
  igstk::TrackerController::Pointer                     m_TrackerController;

  /** VideoImager controller object */
  igstk::VideoImagerController::Pointer                      m_VideoImagerController;

  /** tracker controller observer object */
  TrackerControllerObserver::Pointer                    m_TrackerControllerObserver;

  /** VideoImager controller observer object */
  VideoImagerControllerObserver::Pointer                     m_VideoImagerControllerObserver;

  /** tracker configuration object */
  igstk::TrackerConfiguration *                         m_TrackerConfiguration;

  /** VideoImager controller configuration */
  igstk::ImagingSourceVideoImagerConfiguration *             m_VideoImagerConfiguration;

  /** tracker tool object */
  igstk::TrackerTool::Pointer                           m_TrackerTool;

  /** VideoImager tool object */
  igstk::TrackerTool::Pointer                           m_VideoImagerTool;

  /** Action methods to be invoked only by the state machine */
  void ReportInvalidRequestProcessing();
  void ReportSuccessSpatialObjectLoadedProcessing();
  void ReportFailureSpatialObjectLoadedProcessing();
  void ReportSuccessMeshLoadedProcessing();
  void ReportFailureMeshLoadedProcessing();
  void ReportSuccessVideoImagerInitializationProcessing();
  void ReportFailureVideoImagerInitializationProcessing();
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
  void ReportSuccessVideoImagerToolSpatialObjectLoadedProcessing();
  void ReportFailureVideoImagerToolSpatialObjectLoadedProcessing();
  void ReportSuccessStopImagingProcessing();
  void ReportFailureStopImagingProcessing();
  void ReportSuccessVideoImagerDisconnectionProcessing();
  void ReportFailureVideoImagerDisconnectionProcessing();
  void StopImagingProcessing();
  void StartImagingProcessing();
  void DisconnectVideoImagerProcessing();
  void InitializeVideoImagerProcessing();


  typedef itk::MemberCommand<VideoTemporalCalibrationWizard>   ProgressCommandType;
  itk::SmartPointer<ProgressCommandType>                m_SocketProgressCommand;

  /** Log file */
  std::ofstream                                   m_LogFile;

  bool ReadMicronConfiguration(igstk::TrackerConfigurationFileReader::Pointer baseReader);
  bool ReadAuroraConfiguration(igstk::TrackerConfigurationFileReader::Pointer baseReader);
  bool ReadPolarisVicraConfiguration(igstk::TrackerConfigurationFileReader::Pointer baseReader);
  bool ReadPolarisSpectraConfiguration(igstk::TrackerConfigurationFileReader::Pointer baseReader);
  bool ReadPolarisHybridConfiguration(igstk::TrackerConfigurationFileReader::Pointer baseReader);

  void WriteSampledPoints( const char *filename );

  void HandleKeyPressed(igstk::VideoTemporalCalibrationWizardQuadrantViews::KeyboardCommandType keyCommand);

  /** Callback functions for picking and reslicing image events. */
  void TrackerToolNotAvailableCallback( const itk::EventObject & event );
  void TrackerToolAvailableCallback( const itk::EventObject & event );
  void VideoImagerToolNotAvailableCallback( const itk::EventObject & event );
  void VideoImagerToolAvailableCallback( const itk::EventObject & event );
  void HandleKeyPressedCallback( const itk::EventObject & event );
  void HandleMousePressedCallback( const itk::EventObject & event );
  void VideoImagerToolUpdateTransformCallback( const itk::EventObject & event );
  void TrackerToolUpdateTransformCallback( const itk::EventObject & event );
  void NullActionCallback(const itk::EventObject & event ){};
  void OnSocketProgressEvent(itk::Object *source, const itk::EventObject &);
  void HandleMousePressed(igstk::VideoTemporalCalibrationWizardQuadrantViews::MouseCommandType mouseCommand);

};

#endif
