/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    UltrasoundNavigator.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __Navigator_h
#define __Navigator_h

#include "UltrasoundNavigatorGUI.h"

#include "igstkSandboxConfigure.h"

#include "igstkStateMachine.h"
#include "igstkMacros.h"
#include "igstkObject.h"
#include "igstkEvents.h"

#include "igstkCTImageReader.h"
#include "igstkCTImageSpatialObjectRepresentation.h"

#include "igstkReslicerPlaneSpatialObject.h"
#include "igstkImageResliceObjectRepresentation.h"
#include "igstkMeshResliceObjectRepresentation.h"

#include "igstkTracker.h"
#include "igstkTrackerTool.h"

#include "igstkLandmark3DRegistration.h"

#include "igstkEllipsoidObject.h"
#include "igstkEllipsoidObjectRepresentation.h"

#include "igstkMeshReader.h"
#include "igstkMeshObjectRepresentation.h"

#include "igstkCylinderObject.h"
#include "igstkCylinderObjectRepresentation.h"

#include "igstkAxesObject.h"

#include "igstkToolProjectionSpatialObject.h"
#include "igstkToolProjectionObjectRepresentation.h"

#include "igstkCrossHairSpatialObject.h"
#include "igstkCrossHairObjectRepresentation.h"

#include "igstkAnnotation2D.h"

#include "itkStdStreamLogOutput.h"

#include "igstkFiducialsPlanIO.h"
#include "igstkPathPlanIO.h"

#include "igstkVideoFrameSpatialObject.h"
#include "igstkVideoFrameRepresentation.h"

#include "igstkImagerController.h"
#include "igstkImagerConfiguration.h"

#include "igstkTrackerController.h"
#include "igstkTrackerConfiguration.h"
#include "igstkCoordinateSystemTransformToResult.h"

#include "igstkTransformFileReader.h"
#include "igstkRigidTransformXMLFileReader.h"
#include "igstkTrackerConfigurationFileReader.h"
#include "igstkTrackerConfigurationXMLFileReaderBase.h"


/** \class UltrasoundNavigator
* 
* \brief Implementation class for UltrasoundNavigatorGUI.
*
* This class implements the main application.
*
*/
class UltrasoundNavigator : public UltrasoundNavigatorGUI
{
public:

  /** Typedefs */
  igstkStandardClassBasicTraitsMacro( UltrasoundNavigator, UltrasoundNavigatorGUI );
  
  igstkLoggerMacro();

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();  

  /** typedef for image reader */
  typedef igstk::CTImageReader                        ImageReaderType;

  /** typedef for mesh readers */
  typedef igstk::MeshReader                           MeshReaderType;

  /** typedef for image spatial objects */
  typedef ImageReaderType::ImageSpatialObjectType     ImageSpatialObjectType;

  /** typedef for mesh spatial objects */
  typedef MeshReaderType::MeshObjectType              MeshType;

  /** typedef for a cylinder spatial object and representation
  * used to represent the tracked tool */
  typedef igstk::CylinderObject                       CylinderType;
  typedef igstk::CylinderObjectRepresentation         CylinderRepresentationType;  

  /** typedef for an image voxel index and world coordinates */
  typedef ImageSpatialObjectType::IndexType           IndexType;
  typedef ImageSpatialObjectType::PointType           PointType;

  /** Cross hair spatial object and representation */
  typedef igstk::CrossHairSpatialObject               CrossHairType;
  typedef igstk::CrossHairObjectRepresentation        CrossHairRepresentationType;

  /** tool projection spatial object and representation */
  typedef igstk::ToolProjectionSpatialObject          ToolProjectionType;
  typedef igstk::ToolProjectionObjectRepresentation   ToolProjectionRepresentationType;

  /** reslicer plane spatial object */
  typedef igstk::ReslicerPlaneSpatialObject           ReslicerPlaneType;

  /** tool spatial object and representation */
  typedef igstk::MeshObjectRepresentation             MeshRepresentationType;

  /** mesh reslice representation */
  typedef igstk::MeshResliceObjectRepresentation  MeshResliceRepresentationType;

  /** image reslice representation */
  typedef igstk::ImageResliceObjectRepresentation< ImageSpatialObjectType >
                                                          ImageRepresentationType;

  /** typedef for landmark registration types */
  typedef igstk::Landmark3DRegistration               RegistrationType;

  typedef RegistrationType::LandmarkPointContainerType  
                                                      LandmarkPointContainerType;

  typedef std::map< unsigned int, bool >              AcceptedLandmarkPointContainerType;

  typedef itk::MemberCommand<UltrasoundNavigator>               ProgressCommandType;

  /** Public request methods from the GUI */
  virtual void RequestLoadImage();
  virtual void RequestCancelImageLoad();
  virtual void RequestAcceptImageLoad();
  virtual void RequestLoadMesh();
  virtual void RequestLoadTrackerToolSpatialObject();
  virtual void RequestToggleSetImageFiducials();
  virtual void RequestStartSetTrackerFiducials();
  virtual void RequestEndSetTrackerFiducials();
  virtual void RequestAcceptTrackerFiducial();
  virtual void RequestPrepareToQuit();

  void RequestLoadImagerToolSpatialObject();

  void EnableVideo();
  void DisableVideo();

  void StartVideo();
  void StopVideo();

  virtual void RequestToggleEnableVideo();
  virtual void RequestToggleRunVideo();

  /**
   * Open the relevant tracker settings dialog (Polaris/Aurora/Micron/Ascension)
   * and get the users input.
   */
  virtual void RequestConfigureTracker();
  virtual void RequestInitializeTracker();
  virtual void RequestTrackerRegistration();
  virtual void RequestStartTracking();
  virtual void RequestStopTracking();
  virtual void RequestAcceptRegistration();
  virtual void RequestRejectRegistration();
  virtual void RequestChangeSelectedFiducial();
  virtual void RequestChangeSelectedViewMode();
  virtual void RequestDisconnectTracker();
  virtual void RequestInitializeImager();
  virtual void RequestStartImaging();
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

  igstkObserverMacro( CoordinateSystemTransform, 
                      igstk::CoordinateSystemTransformToEvent, 
                      igstk::CoordinateSystemTransformToResult )

  igstkObserverMacro( TransformationDescription, 
                      igstk::StringEvent, 
                      std::string )

  igstkObserverMacro( TransformationDate, 
                      igstk::PrecomputedTransformData::TransformDateTypeEvent, 
                      std::string )

  typedef igstk::TrackerConfigurationFileReader::ReadTrackerConfigurationFailSuccessObserver
                    ReadTrackerConfigurationFailSuccessObserverType;

  typedef igstk::PrecomputedTransformData::TransformType *  TransformTypePointer;

  igstkObserverMacro( TransformRequest,
                    igstk::PrecomputedTransformData::TransformTypeEvent,
                    TransformTypePointer )

  igstkObserverMacro( TransformError, 
                      igstk::PrecomputedTransformData::TransformErrorTypeEvent, 
                      igstk::PrecomputedTransformData::ErrorType )


  /** Define observers for event communication */
  igstkObserverObjectMacro( Image, igstk::CTImageReader::ImageModifiedEvent,
                                   igstk::CTImageSpatialObject );

  igstkObserverObjectMacro( MeshObject, igstk::MeshReader::MeshModifiedEvent,
                                        igstk::MeshObject );

  igstkObserverMacro( Registration, igstk::CoordinateSystemTransformToEvent,
                                    igstk::CoordinateSystemTransformToResult );

  igstkObserverMacro( RegistrationError, igstk::DoubleTypeEvent, double );

  igstkObserverMacro( ImageExtent, igstk::ImageExtentEvent, 
                      igstk::EventHelperType::ImageExtentType );

  igstkObserverMacro( VTKImage, igstk::VTKImageModifiedEvent,
                      igstk::EventHelperType::VTKImagePointerType);

  igstkEventMacro( InvalidRequestErrorEvent, igstk::IGSTKErrorEvent );

  UltrasoundNavigator();
  virtual ~UltrasoundNavigator();

private:

  UltrasoundNavigator(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented
  
   /** States for the State Machine */
  
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( LoadingImage );
  igstkDeclareStateMacro( ConfirmingImagePatientName );
  igstkDeclareStateMacro( ImageReady );
  igstkDeclareStateMacro( LoadingTrackerToolSpatialObject );
  igstkDeclareStateMacro( LoadingMesh );
  igstkDeclareStateMacro( SettingImageFiducials );
  igstkDeclareStateMacro( TrackerToolSpatialObjectReady );
  igstkDeclareStateMacro( SettingTrackerFiducials );
  igstkDeclareStateMacro( EndingSetTrackerFiducials );
  igstkDeclareStateMacro( TrackerFiducialsReady );
  igstkDeclareStateMacro( ConfiguringTracker );
  igstkDeclareStateMacro( TrackerConfigurationReady );
  igstkDeclareStateMacro( InitializingTracker );
  igstkDeclareStateMacro( TrackerInitializationReady );
  igstkDeclareStateMacro( RegisteringTracker );
  igstkDeclareStateMacro( AcceptingRegistration );
  igstkDeclareStateMacro( RegistrationReady );
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
  igstkDeclareInputMacro( LoadImage );
  igstkDeclareInputMacro( ConfirmImagePatientName );
  igstkDeclareInputMacro( LoadMesh );
  igstkDeclareInputMacro( LoadTrackerToolSpatialObject );
  igstkDeclareInputMacro( LoadImagerToolSpatialObject );
  igstkDeclareInputMacro( ConfigureTracker );
  igstkDeclareInputMacro( StartSetImageFiducials );
  igstkDeclareInputMacro( SetPickingPosition );
  igstkDeclareInputMacro( EndSetImageFiducials );
  igstkDeclareInputMacro( InitializeTracker );
  igstkDeclareInputMacro( StartSetTrackerFiducials );
  igstkDeclareInputMacro( AcceptTrackerFiducial );
  igstkDeclareInputMacro( EndSetTrackerFiducials );
  igstkDeclareInputMacro( RegisterTracker );
  igstkDeclareInputMacro( StartTracking );
  igstkDeclareInputMacro( StopTracking );
  igstkDeclareInputMacro( DisconnectTracker );
  igstkDeclareInputMacro( InitializeImager );
  igstkDeclareInputMacro( StartImaging );
  igstkDeclareInputMacro( StopImaging );
  igstkDeclareInputMacro( DisconnectImager );

  /** DICOM image reader */
  ImageReaderType::Pointer                              m_ImageReader;
  
  /** DICOM image observers */
  ImageObserver::Pointer                                m_ImageObserver;

  /** Container with points collected in the registration */
  LandmarkPointContainerType                            m_CandidateLandmarks;

  std::string                                           m_ImageDir;
  std::string                                           m_PlanFilename;
  std::string                                           m_PlanFilenamePath;
  
  igstk::FiducialsPlan                                * m_Plan;

  double                                                m_WindowLevel;
  double                                                m_WindowWidth;

  double                                                m_WindowLevelVideo;
  double                                                m_WindowWidthVideo;

  double                                                m_TrackerRMS;
  bool                                                  m_VideoEnabled;
  bool                                                  m_VideoRunning;
  bool                                                  m_VideoConnected;
  bool                                                  m_ImagerInitialized;
  bool                                                  m_ImagePlanesIn3DViewEnabled;
  bool                                                  m_ModifyImageFiducialsEnabled;

  /* Command used for progress tracking */
  itk::SmartPointer<ProgressCommandType>                m_DICOMProgressCommand;  
  itk::SmartPointer<ProgressCommandType>                m_SocketProgressCommand;  
  
  /** image spatial object */
  ImageSpatialObjectType::Pointer                       m_ImageSpatialObject;

  /** video frame spatial object and representation*/
  typedef igstk::VideoFrameSpatialObject                VideoFrameSpatialObjectType;
  VideoFrameSpatialObjectType::Pointer                  m_VideoFrame;

  typedef igstk::VideoFrameRepresentation               VideoFrameRepresentationType;
  VideoFrameRepresentationType::Pointer                 m_VideoFrameRepresentationForVideoView;
  VideoFrameRepresentationType::Pointer                 m_VideoFrameRepresentationFor3DView;

  /** tool projection spatial object */
  ToolProjectionType::Pointer                           m_ToolProjection;

  /** cross hair spatial object */
  CrossHairType::Pointer                                m_CrossHair;

  /** tool projection representations */
  ToolProjectionRepresentationType::Pointer             m_CTView2ToolProjectionRepresentation;

  /** cross hair representation */
  CrossHairRepresentationType::Pointer                  m_CrossHairRepresentationForCTView2;
  CrossHairRepresentationType::Pointer                  m_CrossHairRepresentationFor3DView;

  /** a vector of mesh spatial objects */
  std::vector< MeshType::Pointer >                      m_MeshVector;

  /** tracker tool spatial object and representation */
  MeshType::Pointer                                     m_TrackerToolSpatialObject;
  MeshRepresentationType::Pointer                       m_TrackerToolRepresentation;

  /** imager tool spatial object and representation */
  MeshType::Pointer                                     m_ImagerToolSpatialObject;
  MeshRepresentationType::Pointer                       m_ImagerToolRepresentationForVideoView;
  MeshRepresentationType::Pointer                       m_ImagerToolRepresentationFor3DView;

  /** reslicer plane spatial object */
  ReslicerPlaneType::Pointer                            m_CTView1PlaneSpatialObject;
  ReslicerPlaneType::Pointer                            m_CTView2PlaneSpatialObject;

  /** reslicer plane representations for the 2D views */
  ImageRepresentationType::Pointer                      m_CTView1ImageRepresentation;
  ImageRepresentationType::Pointer                      m_CTView2ImageRepresentation;

  /** reslicer plane representations for the 3D views */
  ImageRepresentationType::Pointer                      m_CTView1ImageRepresentation2;
  ImageRepresentationType::Pointer                      m_CTView2ImageRepresentation2;

  /** vector of loaded meshes */
  std::vector< MeshRepresentationType::Pointer >        m_MeshRepresentationVector;

  /** vector of mesh contours */
  std::vector< MeshResliceRepresentationType::Pointer > m_CTView1MeshResliceRepresentationVector;
  std::vector< MeshResliceRepresentationType::Pointer > m_CTView2MeshResliceRepresentationVector;
  std::vector< MeshResliceRepresentationType::Pointer > m_VideoViewMeshResliceRepresentationVector;
  
  /** Landmark registration points containers */
  LandmarkPointContainerType                            m_LandmarksContainer;
  AcceptedLandmarkPointContainerType                    m_AcceptedLandmarksContainer;

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

    void SetParent(UltrasoundNavigator *p); 

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
    UltrasoundNavigator *m_Parent;
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

    void SetParent(UltrasoundNavigator *p); 

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
    UltrasoundNavigator *m_Parent;
    bool m_ErrorOccured;
    std::string m_ErrorMessage;

    //purposely not implemented
    ImagerControllerObserver(const Self&);
    void operator=(const Self&); 
  };

  /** tracker controller object */
  igstk::TrackerController::Pointer                     m_TrackerController;

  /** tracker controller object */
  TrackerControllerObserver::Pointer                    m_TrackerControllerObserver;

  /** tracker configuration object */
  const igstk::TrackerConfiguration *                   m_TrackerConfiguration;

  /** imager controller object */
  igstk::ImagerController::Pointer                      m_ImagerController;

  /** imager controller object */
  ImagerControllerObserver::Pointer                     m_ImagerControllerObserver;

  /** imager controller configuration */
  igstk::TerasonImagerConfiguration                    *m_ImagerConfiguration;

  /** imager object and imager tool*/
  igstk::Imager::Pointer                                m_Imager;
  igstk::ImagerTool::Pointer                            m_TerasonImagerTool;

  /** tracker tool object */
  igstk::TrackerTool::Pointer                           m_TrackerTool;

  /** imager tool object */
  igstk::TrackerTool::Pointer                           m_ImagerTool;

  /** reference tool object */
  igstk::TrackerTool::Pointer                           m_ReferenceTool;
  
  /** Action methods to be invoked only by the state machine */
  void ReportInvalidRequestProcessing();
  void ReportSuccessImageLoadedProcessing();
  void ReportFailureImageLoadedProcessing();
  void ReportSuccessTrackerToolSpatialObjectLoadedProcessing();
  void ReportFailureTrackerToolSpatialObjectLoadedProcessing();
  void ReportSuccessImagerToolSpatialObjectLoadedProcessing();
  void ReportFailureImagerToolSpatialObjectLoadedProcessing();
  void ReportSuccessMeshLoadedProcessing();
  void ReportFailureMeshLoadedProcessing();
  void ReportSuccessStartSetImageFiducialsProcessing();
  void ReportSuccessEndSetImageFiducialsProcessing();
  void ReportSuccessStartSetTrackerFiducialsProcessing();
  void ReportSuccessEndSetTrackerFiducialsProcessing();
  void ReportFailureEndSetTrackerFiducialsProcessing();
  void ReportSuccessPinpointingTrackerFiducialProcessing();
  void ReportFailurePinpointingTrackerFiducialProcessing();
  void ReportSuccessTrackerConfigurationProcessing();
  void ReportFailureTrackerConfigurationProcessing();
  void ReportSuccessTrackerInitializationProcessing();
  void ReportFailureTrackerInitializationProcessing();  
  void ReportSuccessTrackerRegistrationProcessing();
  void ReportFailureTrackerRegistrationProcessing();
  void ReportSuccessAcceptingRegistrationProcessing();
  void ReportFailureAcceptingRegistrationProcessing();
  void ReportSuccessTrackerDisconnectionProcessing();
  void ReportFailureTrackerDisconnectionProcessing();  
  void ReportSuccessImagerDisconnectionProcessing();
  void ReportFailureImagerDisconnectionProcessing();
  void ReportSuccessStopTrackingProcessing();
  void ReportFailureStopTrackingProcessing();
  void ReportSuccessStartTrackingProcessing();
  void ReportFailureStartTrackingProcessing();
  void LoadImageProcessing();
  void ConfirmPatientNameProcessing();  
  void LoadMeshProcessing();
  void LoadTrackerToolSpatialObjectProcessing();
  void SetImageFiducialProcessing();
  void SetImagePickingProcessing();
  void StartSetTrackerFiducialsProcessing();
  void AcceptTrackerFiducialProcessing();
  void EndSetTrackerFiducialsProcessing();  
  void ConfigureTrackerProcessing();
  void InitializeTrackerProcessing();
  void TrackerRegistrationProcessing();
  void StartTrackingProcessing();
  void StopTrackingProcessing();
  void DisconnectTrackerProcessing();

  void InitializeImagerProcessing();
  void StartImagingProcessing();
  void StopImagingProcessing();
  void DisconnectImagerProcessing();

  //imager-related stuff
  void LoadImagerToolSpatialObjectProcessing();
  void ReportSuccessImagerConfigurationProcessing();
  void ReportFailureImagerConfigurationProcessing();
  void ReportSuccessImagerInitializationProcessing();
  void ReportFailureImagerInitializationProcessing();
  void ReportSuccessStartImagingProcessing();
  void ReportFailureStartImagingProcessing();
  void ReportSuccessStopImagingProcessing();
  void ReportFailureStopImagingProcessing();


  /** Observer type for loaded event, 
   *  the callback can be set to a member function. */
  typedef itk::ReceptorMemberCommand < Self > LoadedObserverType;
  typedef itk::SimpleMemberCommand < Self > CancelObserverType;
  
  LoadedObserverType::Pointer               m_CTView1PickerObserver;
  LoadedObserverType::Pointer               m_CTView2PickerObserver;
  LoadedObserverType::Pointer               m_VideoViewPickerObserver;

  LoadedObserverType::Pointer               m_ManualReslicingObserver;
  LoadedObserverType::Pointer               m_KeyPressedObserver;
  LoadedObserverType::Pointer               m_MousePressedObserver;
  LoadedObserverType::Pointer               m_TrackerToolNotAvailableObserver;
  LoadedObserverType::Pointer               m_TrackerToolAvailableObserver;
  LoadedObserverType::Pointer               m_ImagerToolNotAvailableObserver;
  LoadedObserverType::Pointer               m_ImagerToolAvailableObserver;
  LoadedObserverType::Pointer               m_ReferenceNotAvailableObserver;
  LoadedObserverType::Pointer               m_ReferenceAvailableObserver;

  /** Ellipsoid spatial object, used to represent the fiducial points*/
  typedef igstk::EllipsoidObject                  EllipsoidType;
  typedef igstk::EllipsoidObjectRepresentation    EllipsoidRepresentationType;  

  /** Objects for path planning and fiducial selection */
  std::vector< EllipsoidType::Pointer >                m_FiducialPointVector;
  std::vector< EllipsoidRepresentationType::Pointer >  m_AxialFiducialRepresentationVector;
  std::vector< EllipsoidRepresentationType::Pointer >  m_SagittalFiducialRepresentationVector;
  std::vector< EllipsoidRepresentationType::Pointer >  m_CoronalFiducialRepresentationVector;
  std::vector< EllipsoidRepresentationType::Pointer >  m_3DViewFiducialRepresentationVector;      

  /** Define a initial world coordinate system */
  typedef igstk::AxesObject                       AxesObjectType;
  AxesObjectType::Pointer                         m_WorldReference;

  igstk::Transform                                m_RegistrationTransform;
  igstk::Transform                                m_PickingTransform;

  /** Log file */
  std::ofstream                                   m_LogFile;  

  unsigned int                                    m_xslice;
  unsigned int                                    m_yslice;
  unsigned int                                    m_zslice;


  /** Utility functions, conversion between points and transform */
  inline 
  igstk::Transform 
  PointToTransform( PointType point)
  {
    igstk::Transform transform;
    igstk::Transform::VectorType translation;
    for (unsigned int i=0; i<3; i++)
      {
      translation[i] = point[i];
      }
    transform.SetTranslation( translation, 0.1, 
                                igstk::TimeStamp::GetLongestPossibleTime() );
    return transform;
  }

  inline 
  ImageSpatialObjectType::PointType 
  TransformToPoint( igstk::Transform transform)
  {
    ImageSpatialObjectType::PointType point;
    for (unsigned int i=0; i<3; i++)
      {
      point[i] = transform.GetTranslation()[i];
      }
    return point;
  }

  /** Internal functions */
  void ReadFiducials();
  void WriteFiducials(); 
  void ConnectImageRepresentation();
  void UpdateFiducialPoint();
  void RequestToggleOrthogonalPlanes();
  void ResliceImage(IndexType index);
  void HandleKeyPressed(igstk::UltrasoundNavigatorQuadrantViews::KeyboardCommandType keyCommand);
  void HandleMousePressed(igstk::UltrasoundNavigatorQuadrantViews::MouseCommandType mouseCommand);
  void AddImagePlanesTo3DView();
  void RemoveImagePlanesTo3DView();

  /** Callback functions for picking and reslicing image events. */  

  
  void CTView1PickingCallback( const itk::EventObject & event );
  void CTView2PickingCallback( const itk::EventObject & event );
  void VideoViewPickingCallback( const itk::EventObject & event );
  void ResliceImageCallback( const itk::EventObject & event );
  void HandleKeyPressedCallback( const itk::EventObject & event );
  void HandleMousePressedCallback( const itk::EventObject & event ); 
  void TrackerToolNotAvailableCallback( const itk::EventObject & event ); 
  void TrackerToolAvailableCallback( const itk::EventObject & event ); 
  void ImagerToolNotAvailableCallback( const itk::EventObject & event ); 
  void ImagerToolAvailableCallback( const itk::EventObject & event ); 
  void ReferenceNotAvailableCallback( const itk::EventObject & event ); 
  void ReferenceAvailableCallback( const itk::EventObject & event ); 
  void NullActionCallback(const itk::EventObject & event ){};
  void OnITKProgressEvent(itk::Object *source, const itk::EventObject &);
  void OnSocketProgressEvent(itk::Object *source, const itk::EventObject &);
  

};

#endif
