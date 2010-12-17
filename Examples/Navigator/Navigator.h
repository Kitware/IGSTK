/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    Navigator.h
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

#include "NavigatorGUI.h"

#include "igstkConfigure.h"

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

#include "igstkTrackerController.h"
#include "igstkTrackerConfiguration.h"
#include "igstkCoordinateSystemTransformToResult.h"

#include "igstkTransformFileReader.h"
#include "igstkRigidTransformXMLFileReader.h"
#include "igstkTrackerConfigurationFileReader.h"
#include "igstkTrackerConfigurationXMLFileReaderBase.h"


/** \class Navigator
* 
* \brief Implementation class for NavigatorGUI.
*
* This class implements the main application.
*
*/
class Navigator : public NavigatorGUI
{
public:

  /** Typedefs */
  igstkStandardClassBasicTraitsMacro( Navigator, NavigatorGUI );
  
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
  typedef igstk::CylinderObject                      CylinderType;
  typedef igstk::CylinderObjectRepresentation        CylinderRepresentationType;

  /** typedef for an image voxel index and world coordinates */
  typedef ImageSpatialObjectType::IndexType           IndexType;
  typedef ImageSpatialObjectType::PointType           PointType;

  /** Cross hair spatial object and representation */
  typedef igstk::CrossHairSpatialObject            CrossHairType;
  typedef igstk::CrossHairObjectRepresentation     CrossHairRepresentationType;

  /** tool projection spatial object and representation */
  typedef igstk::ToolProjectionSpatialObject             ToolProjectionType;
  typedef igstk::ToolProjectionObjectRepresentation      
                                               ToolProjectionRepresentationType;

  /** reslicer plane spatial object */
  typedef igstk::ReslicerPlaneSpatialObject              ReslicerPlaneType;

  /** tool representation */
  typedef igstk::MeshObjectRepresentation                MeshRepresentationType;

  /** mesh reslice representation */
  typedef igstk::MeshResliceObjectRepresentation  MeshResliceRepresentationType;

  /** image reslice representation */
  typedef igstk::ImageResliceObjectRepresentation< ImageSpatialObjectType >
                                                        ImageRepresentationType;

  /** typedef for landmark registration types */
  typedef igstk::Landmark3DRegistration                        RegistrationType;

  typedef RegistrationType::LandmarkPointContainerType  
                                                     LandmarkPointContainerType;

  typedef std::map< unsigned int, bool >     AcceptedLandmarkPointContainerType;

  typedef itk::MemberCommand<Navigator>                     ProgressCommandType;

  /** Public request methods from the GUI */
  virtual void RequestLoadImage();
  virtual void RequestCancelImageLoad();
  virtual void RequestAcceptImageLoad();
  virtual void RequestLoadMesh();
  virtual void RequestLoadToolSpatialObject();
  virtual void RequestToggleSetImageFiducials();
  virtual void RequestStartSetTrackerFiducials();
  virtual void RequestEndSetTrackerFiducials();
  virtual void RequestAcceptTrackerFiducial();
  virtual void RequestPrepareToQuit();

  /** Public request methods from the GUI to help operation workflow */

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

  typedef igstk::TrackerConfigurationFileReader::
  ReadTrackerConfigurationFailSuccessObserver
                                ReadTrackerConfigurationFailSuccessObserverType;

  igstkObserverMacro( DICOMImageReaderInvalidDirectoryNameError, 
                      igstk::DICOMImageDirectoryIsNotDirectoryErrorEvent, 
                      std::string )

  igstkObserverMacro( DICOMImageReaderNonExistingDirectoryError, 
                      igstk::DICOMImageDirectoryDoesNotExistErrorEvent, 
                      igstk::EventHelperType::StringType )

  igstkObserverMacro( DICOMImageReaderEmptyDirectoryError, 
                      igstk::DICOMImageDirectoryEmptyErrorEvent, 
                      igstk::EventHelperType::StringType )

  igstkObserverMacro( DICOMImageDirectoryNameDoesNotHaveEnoughFilesError, 
                     igstk::DICOMImageDirectoryDoesNotHaveEnoughFilesErrorEvent,
                     igstk::EventHelperType::StringType )

  igstkObserverMacro( DICOMImageDirectoryDoesNotContainValidDICOMSeriesError, 
                      igstk::DICOMImageSeriesFileNamesGeneratingErrorEvent, 
                      igstk::EventHelperType::StringType )

  igstkObserverMacro( DICOMImageInvalidError, 
                      igstk::DICOMImageReadingErrorEvent, 
                      igstk::EventHelperType::StringType )

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

  Navigator();
  virtual ~Navigator();

private:

  Navigator(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented
  
   /** States for the State Machine */
  
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( LoadingImage );
  igstkDeclareStateMacro( ConfirmingImagePatientName );
  igstkDeclareStateMacro( ImageReady );
  igstkDeclareStateMacro( LoadingToolSpatialObject );
  igstkDeclareStateMacro( LoadingMesh );
  igstkDeclareStateMacro( SettingImageFiducials );
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
  
   /** Inputs to the state machine and it's designed transitions */

  igstkDeclareInputMacro( Success );
  igstkDeclareInputMacro( Failure );
  igstkDeclareInputMacro( LoadImage );
  igstkDeclareInputMacro( ConfirmImagePatientName );
  igstkDeclareInputMacro( LoadMesh );
  igstkDeclareInputMacro( LoadToolSpatialObject );
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

  /** DICOM image reader */
  ImageReaderType::Pointer                              m_ImageReader;
  
  /** DICOM image observers */
  ImageObserver::Pointer                                m_ImageObserver;

  LandmarkPointContainerType                            m_CandidateLandmarks;

  std::string                                           m_ImageDir;
  std::string                                           m_PlanFilename;
  std::string                                           m_PlanFilenamePath;
  
  igstk::FiducialsPlan                                * m_Plan;

  double                                                m_WindowLevel;
  double                                                m_WindowWidth;

  double                                              m_TrackerRMS;  
  bool                                                m_AxialViewInitialized;
  bool                                                m_SagittalViewInitialized;
  bool                                                m_CoronalViewInitialized;

  bool                                            m_ImagePlanesIn3DViewEnabled;
  bool                                            m_ModifyImageFiducialsEnabled;

  /* Command used for progress tracking */
  itk::SmartPointer<ProgressCommandType>                m_ProgressCommand;  
  
  /** image spatial object */
  ImageSpatialObjectType::Pointer                       m_ImageSpatialObject;

  /** tool projection spatial object */
  ToolProjectionType::Pointer                           m_ToolProjection;

  /** cross hair spatial object */
  CrossHairType::Pointer                                m_CrossHair;

  /** tool projection representations */
  ToolProjectionRepresentationType::Pointer             
                                            m_AxialToolProjectionRepresentation;
  ToolProjectionRepresentationType::Pointer  
                                         m_SagittalToolProjectionRepresentation;
  ToolProjectionRepresentationType::Pointer             
                                          m_CoronalToolProjectionRepresentation;

  /** cross hair representation */
  CrossHairRepresentationType::Pointer        m_AxialCrossHairRepresentation;
  CrossHairRepresentationType::Pointer        m_SagittalCrossHairRepresentation;
  CrossHairRepresentationType::Pointer        m_CoronalCrossHairRepresentation;
  CrossHairRepresentationType::Pointer        m_3DViewCrossHairRepresentation;

  /** a vector of mesh spatial objects */
  std::vector< MeshType::Pointer >                      m_MeshVector;

  /** tool spatial object and representation */
  MeshType::Pointer                                     m_ToolSpatialObject;
  MeshRepresentationType::Pointer                       m_ToolRepresentation;

  ReslicerPlaneType::Pointer                     m_AxialPlaneSpatialObject;
  ReslicerPlaneType::Pointer                     m_SagittalPlaneSpatialObject;
  ReslicerPlaneType::Pointer                     m_CoronalPlaneSpatialObject;

  ImageRepresentationType::Pointer               m_AxialPlaneRepresentation;
  ImageRepresentationType::Pointer               m_SagittalPlaneRepresentation;
  ImageRepresentationType::Pointer               m_CoronalPlaneRepresentation;

  ImageRepresentationType::Pointer               m_AxialPlaneRepresentation2;
  ImageRepresentationType::Pointer               m_SagittalPlaneRepresentation2;
  ImageRepresentationType::Pointer               m_CoronalPlaneRepresentation2;

  std::vector< MeshRepresentationType::Pointer >     m_MeshRepresentationVector;

  std::vector< MeshResliceRepresentationType::Pointer > 
                                         m_AxialMeshResliceRepresentationVector;
  std::vector< MeshResliceRepresentationType::Pointer > 
                                      m_SagittalMeshResliceRepresentationVector;
  std::vector< MeshResliceRepresentationType::Pointer > 
                                       m_CoronalMeshResliceRepresentationVector;
  
  /** Landmark registration points containers */
  LandmarkPointContainerType                       m_LandmarksContainer;
  AcceptedLandmarkPointContainerType               m_AcceptedLandmarksContainer;

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

    void SetParent(Navigator *p); 

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
     * If an error occurs in one of the observed IGSTK components this method 
     * will return true.
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
    Navigator *m_Parent;
    bool m_ErrorOccured;
    std::string m_ErrorMessage;

    //purposely not implemented
    TrackerControllerObserver(const Self&);
    void operator=(const Self&); 
  };

  igstk::TrackerController::Pointer                 m_TrackerController;
  TrackerControllerObserver::Pointer                m_TrackerControllerObserver;

  /** tracker configuration object */
  const igstk::TrackerConfiguration *                   m_TrackerConfiguration;

  /** typedef for the vector of tracker tools */
  typedef std::vector < igstk::TrackerTool::Pointer >    ToolVectorType;

  igstk::TrackerTool::Pointer                           m_TrackerTool;
  igstk::TrackerTool::Pointer                           m_ReferenceTool;
  
  /** Action methods to be invoked only by the state machine */
  void NoProcessing();
  void ReportInvalidRequestProcessing();
  void ReportSuccessImageLoadedProcessing();
  void ReportFailuredImageLoadedProcessing();
  void ReportSuccessToolSpatialObjectLoadedProcessing();
  void ReportFailuredToolSpatialObjectLoadedProcessing();
  void ReportSuccessMeshLoadedProcessing();
  void ReportFailuredMeshLoadedProcessing();
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
  void ReportSuccessStopTrackingProcessing();
  void ReportFailureStopTrackingProcessing();
  void ReportSuccessStartTrackingProcessing();
  void ReportFailureStartTrackingProcessing();
  void LoadImageProcessing();
  void ConfirmPatientNameProcessing();  
  void LoadMeshProcessing();
  void LoadToolSpatialObjectProcessing();
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

  /** Observer type for loaded event, 
   *  the callback can be set to a member function. */
  typedef itk::ReceptorMemberCommand < Self > LoadedObserverType;
  typedef itk::SimpleMemberCommand < Self > CancelObserverType;
  
  LoadedObserverType::Pointer               m_AxialViewPickerObserver;
  LoadedObserverType::Pointer               m_SagittalViewPickerObserver;
  LoadedObserverType::Pointer               m_CoronalViewPickerObserver;

  LoadedObserverType::Pointer               m_ManualReslicingObserver;
  LoadedObserverType::Pointer               m_KeyPressedObserver;
  LoadedObserverType::Pointer               m_MousePressedObserver;
  LoadedObserverType::Pointer               m_TrackerToolUpdateObserver;
  LoadedObserverType::Pointer               m_TrackerToolNotAvailableObserver;
  LoadedObserverType::Pointer               m_TrackerToolAvailableObserver;
  LoadedObserverType::Pointer               m_ReferenceNotAvailableObserver;
  LoadedObserverType::Pointer               m_ReferenceAvailableObserver;

  /** Ellipsoid spatial object, used to represent the fiducial points*/
  typedef igstk::EllipsoidObject                  EllipsoidType;
  typedef igstk::EllipsoidObjectRepresentation    EllipsoidRepresentationType;  

  /** Objects for path planning and fiducial selection */
  std::vector< EllipsoidType::Pointer >                m_FiducialPointVector;
  std::vector< EllipsoidRepresentationType::Pointer >  
                                            m_AxialFiducialRepresentationVector;
  std::vector< EllipsoidRepresentationType::Pointer >  
                                         m_SagittalFiducialRepresentationVector;
  std::vector< EllipsoidRepresentationType::Pointer >  
                                          m_CoronalFiducialRepresentationVector;
  std::vector< EllipsoidRepresentationType::Pointer >  
                                           m_3DViewFiducialRepresentationVector;

  /** Define a initial world coordinate system */
  typedef igstk::AxesObject                       AxesObjectType;
  AxesObjectType::Pointer                         m_WorldReference;

  igstk::Transform                                m_RegistrationTransform;
  igstk::Transform                                m_PickingTransform;

  /** Log file */
  std::ofstream                                   m_LogFile;  

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
  void ResliceImage( IndexType index );
  void HandleKeyPressed ( igstk::NavigatorQuadrantViews::KeyboardCommandType 
                                                                   keyCommand );
  void HandleMousePressed ( igstk::NavigatorQuadrantViews::MouseCommandType 
                                                                 mouseCommand );
  void EnableOrthogonalPlanes();
  void DisableOrthogonalPlanes();

  /** Callback functions for picking and reslicing image events. */  
  void AxialViewPickingCallback( const itk::EventObject & event );
  void SagittalViewPickingCallback( const itk::EventObject & event );
  void CoronalViewPickingCallback( const itk::EventObject & event );
  void ResliceImageCallback( const itk::EventObject & event );
  void HandleKeyPressedCallback( const itk::EventObject & event );
  void HandleMousePressedCallback( const itk::EventObject & event ); 
  void ToolNotAvailableCallback( const itk::EventObject & event ); 
  void ToolAvailableCallback( const itk::EventObject & event ); 
  void ReferenceNotAvailableCallback( const itk::EventObject & event ); 
  void ReferenceAvailableCallback( const itk::EventObject & event ); 
  void OffOrthogonalResliceCallback( const itk::EventObject & event ); 
  void OrthogonalResliceCallback( const itk::EventObject & event ); 
  void NullActionCallback(const itk::EventObject & itkNotUsed(event) ){};
  void OnITKProgressEvent(itk::Object *source, const itk::EventObject &);

};

#endif
