/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    Navigator.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "Navigator.h"

#include "FL/Fl_File_Chooser.H"
#include "FL/Fl_Input.H"
#include "igstkEvents.h"
#include "itksys/SystemTools.hxx"
#include "itksys/Directory.hxx"
#include "igstkTransformObserver.h"

#include "igstkMicronTrackerConfiguration.h"

#define TRACKER_DEFAULT_REFRESH_RATE 15
#define VIEW_2D_REFRESH_RATE 15
#define VIEW_3D_REFRESH_RATE 15

#define MAX_CANDIDATES 3

/** -----------------------------------------------------------------
*     Constructor
*  -----------------------------------------------------------------
*/
Navigator::Navigator() : 
  m_TrackerConfiguration( NULL ),
  m_StateMachine(this)
{
  std::srand( 5 );

  m_ImageDir = "C:/data";

  m_ResliceEnabled = false;
  m_ImagePlanesIn3DViewEnabled = true;

  m_AxialViewInitialized = false;
  m_SagittalViewInitialized = false;
  m_CoronalViewInitialized = false;
  
  m_ModifyImageFiducialsEnabled = false;

  m_CollectingCandidates = false;

  /** Create the controller for the tracker and assign observers to him*/
  m_TrackerController = igstk::TrackerController::New();

  m_TrackerControllerObserver = TrackerControllerObserver::New();
  m_TrackerControllerObserver->SetParent( this );

  m_TrackerController->AddObserver(igstk::TrackerController::InitializeErrorEvent(),
    m_TrackerControllerObserver );

  m_TrackerController->AddObserver(igstk::TrackerStartTrackingEvent(),
    m_TrackerControllerObserver );

  m_TrackerController->AddObserver(igstk::TrackerStartTrackingErrorEvent(),
    m_TrackerControllerObserver );

  m_TrackerController->AddObserver(igstk::TrackerStopTrackingEvent(),
    m_TrackerControllerObserver );

  m_TrackerController->AddObserver(igstk::TrackerStopTrackingErrorEvent(),
    m_TrackerControllerObserver );

  m_TrackerController->AddObserver(igstk::TrackerController::RequestToolEvent(),
    m_TrackerControllerObserver );

  m_TrackerController->AddObserver(igstk::TrackerController::RequestToolErrorEvent(),
    m_TrackerControllerObserver );
 
  m_TrackerController->AddObserver(igstk::TrackerController::RequestToolsEvent(),
    m_TrackerControllerObserver );  

  /** Setup logger, for all igstk components. */
  m_Logger   = LoggerType::New();
  this->GetLogger()->SetTimeStampFormat( itk::LoggerBase::HUMANREADABLE );
  this->GetLogger()->SetHumanReadableFormat("%Y %b %d, %H:%M:%S");
  this->GetLogger()->SetPriorityLevel( LoggerType::DEBUG);

  /** Direct the application log message to the std::cout */
  itk::StdStreamLogOutput::Pointer m_LogCoutOutput
                                           = itk::StdStreamLogOutput::New();
  m_LogCoutOutput->SetStream( std::cout );
  this->GetLogger()->AddLogOutput( m_LogCoutOutput );

  /** Direct the igstk components log message to the file. */
  itk::StdStreamLogOutput::Pointer m_LogFileOutput
                                           = itk::StdStreamLogOutput::New();

  std::string   logFileName;
  logFileName = "logNavigator"
  + itksys::SystemTools::GetCurrentDateTime( "_%Y_%m_%d_%H_%M_%S" ) + ".txt";
  m_LogFile.open( logFileName.c_str() );
  if( !m_LogFile.fail() )
  {
    m_LogFileOutput->SetStream( m_LogFile );
    this->GetLogger()->AddLogOutput( m_LogFileOutput );
    igstkLogMacro2( m_Logger, DEBUG, "Successfully opened Log file:" << logFileName << "\n" );
  }
  else
  {
    //Return if fail to open the log file
    igstkLogMacro2( m_Logger, DEBUG, "Problem opening Log file:"
                                                    << logFileName << "\n" );
    return;
  }

  // set logger to the controller
  m_TrackerController->SetLogger(this->GetLogger());

  /** Initialize all member variables  */
  
  m_ImageReader         = ImageReaderType::New();
  m_ImageReader->SetGlobalWarningDisplay(false);

  m_WorldReference        = igstk::AxesObject::New();

  m_Plan                = new igstk::FiducialsPlan;

  m_TrackerRMS = 0.0;

  m_NumberOfLoadedMeshes = 0;

  m_FiducialPointVector.resize(4);
  m_AxialFiducialRepresentationVector.resize(4);
  m_SagittalFiducialRepresentationVector.resize(4);
  m_CoronalFiducialRepresentationVector.resize(4);
  m_3DViewFiducialRepresentationVector.resize(4);

  for (int i=0; i<4; i++)
  {
    m_FiducialPointVector[i] = EllipsoidType::New();
    m_FiducialPointVector[i]->SetRadius( 6, 6, 6 );

    m_AxialFiducialRepresentationVector[i] = EllipsoidRepresentationType::New();
    m_AxialFiducialRepresentationVector[i]->RequestSetEllipsoidObject( m_FiducialPointVector[i] );
    m_AxialFiducialRepresentationVector[i]->SetColor( 1.0, 0.0, 0.0);
    m_AxialFiducialRepresentationVector[i]->SetOpacity( 0.6 );

    m_SagittalFiducialRepresentationVector[i] = EllipsoidRepresentationType::New();
    m_SagittalFiducialRepresentationVector[i]->RequestSetEllipsoidObject( m_FiducialPointVector[i] );
    m_SagittalFiducialRepresentationVector[i]->SetColor( 1.0, 0.0, 0.0);
    m_SagittalFiducialRepresentationVector[i]->SetOpacity( 0.6 );

    m_CoronalFiducialRepresentationVector[i] = EllipsoidRepresentationType::New();
    m_CoronalFiducialRepresentationVector[i]->RequestSetEllipsoidObject( m_FiducialPointVector[i] );
    m_CoronalFiducialRepresentationVector[i]->SetColor( 1.0, 0.0, 0.0);
    m_CoronalFiducialRepresentationVector[i]->SetOpacity( 0.6 );

    m_3DViewFiducialRepresentationVector[i] = EllipsoidRepresentationType::New();
    m_3DViewFiducialRepresentationVector[i]->RequestSetEllipsoidObject( m_FiducialPointVector[i] );
    m_3DViewFiducialRepresentationVector[i]->SetColor( 1.0, 0.0, 0.0);
    m_3DViewFiducialRepresentationVector[i]->SetOpacity( 0.6 );
  }  

  /** Creating observers and their callback functions */

  // Initialize the progress command
  m_ProgressCommand = ProgressCommandType::New();
  m_ProgressCommand->SetCallbackFunction( this, &Navigator::OnITKProgressEvent );

  /** These observe reslicing events from Quadrant class */
  m_ManualReslicingObserver = LoadedObserverType::New();
  m_ManualReslicingObserver->SetCallbackFunction( this,
    &Navigator::ResliceImageCallback );

  m_KeyPressedObserver = LoadedObserverType::New();
  m_KeyPressedObserver->SetCallbackFunction( this,
    &Navigator::HandleKeyPressedCallback );

  m_MousePressedObserver = LoadedObserverType::New();
  m_MousePressedObserver->SetCallbackFunction( this,
    &Navigator::HandleMousePressedCallback );

  /** 
   *  This observer listens to the TrackerToolTransformUpdateEvent from
   *  TrackerTool class, notice this event doesn't carry any payload, it
   *  only functions as a ticker here to trigger image representation class
   *  to do image reslicing according to the current tooltip location.
   *  Refer to:
   *  Navigator::ToolTrackingCallback()
   */
  //m_TrackerToolUpdateObserver = LoadedObserverType::New();
  //m_TrackerToolUpdateObserver->SetCallbackFunction( this,
  //                                               &Navigator::ToolTrackingCallback );

  m_TrackerToolNotAvailableObserver = LoadedObserverType::New();
  m_TrackerToolNotAvailableObserver->SetCallbackFunction( this,
                                                 &Navigator::ToolNotAvailableCallback );

  m_TrackerToolAvailableObserver = LoadedObserverType::New();
  m_TrackerToolAvailableObserver->SetCallbackFunction( this,
                                                 &Navigator::ToolAvailableCallback ); 

  m_ReferenceNotAvailableObserver = LoadedObserverType::New();
  m_ReferenceNotAvailableObserver->SetCallbackFunction( this,
                                                 &Navigator::ReferenceNotAvailableCallback );

  m_ReferenceAvailableObserver = LoadedObserverType::New();
  m_ReferenceAvailableObserver->SetCallbackFunction( this,
                                                 &Navigator::ReferenceAvailableCallback );  
  m_WindowWidth = 542;
  m_WindowLevel = 52;

  /** Machine States*/

  igstkAddStateMacro( Initial );
  igstkAddStateMacro( LoadingImage );
  igstkAddStateMacro( ConfirmingImagePatientName );
  igstkAddStateMacro( ImageReady );
  igstkAddStateMacro( LoadingSpatialObject ); 
  igstkAddStateMacro( LoadingTargetMesh ); 
  igstkAddStateMacro( SettingImageFiducials );
  igstkAddStateMacro( ConfiguringTracker );
  igstkAddStateMacro( TrackerConfigurationReady );
  igstkAddStateMacro( InitializingTracker );
  igstkAddStateMacro( TrackerInitializationReady );
  igstkAddStateMacro( SettingTrackerFiducials );
//  igstkAddStateMacro( PinpointingTrackerFiducial );
  igstkAddStateMacro( EndingSetTrackerFiducials );
  igstkAddStateMacro( TrackerFiducialsReady );
  igstkAddStateMacro( RegisteringTracker );  
  igstkAddStateMacro( AcceptingRegistration );
  igstkAddStateMacro( RegistrationReady );
  igstkAddStateMacro( StartingTracker ); 
  igstkAddStateMacro( StoppingTracker ); 
  igstkAddStateMacro( DisconnectingTracker ); 
  igstkAddStateMacro( Tracking );

  /** Machine Inputs*/
  
  igstkAddInputMacro( Success );
  igstkAddInputMacro( Failure );
  igstkAddInputMacro( LoadImage );
  igstkAddInputMacro( ConfirmImagePatientName );
  igstkAddInputMacro( LoadToolSpatialObject );
  igstkAddInputMacro( LoadTargetMesh );
  igstkAddInputMacro( StartSetImageFiducials );
  igstkAddInputMacro( SetPickingPosition );
  igstkAddInputMacro( EndSetImageFiducials );  
  igstkAddInputMacro( ConfigureTracker );
  igstkAddInputMacro( InitializeTracker );
  igstkAddInputMacro( StartSetTrackerFiducials );
  igstkAddInputMacro( AcceptTrackerFiducial );
  igstkAddInputMacro( EndSetTrackerFiducials );  
  igstkAddInputMacro( RegisterTracker );
  igstkAddInputMacro( StartTracking );
  igstkAddInputMacro( StopTracking );
  igstkAddInputMacro( DisconnectTracker );

  /** Initial State */

  igstkAddTransitionMacro( Initial, LoadImage,
                           LoadingImage, LoadImage );

    //complete table for state: Initial State

  igstkAddTransitionMacro( Initial, Success, 
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, Failure, 
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, ConfirmImagePatientName, 
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, LoadTargetMesh, 
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, LoadToolSpatialObject, 
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, StartSetImageFiducials, 
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, SetPickingPosition, 
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, EndSetImageFiducials, 
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, RegisterTracker, 
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, InitializeTracker, 
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, ConfigureTracker, 
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, StartSetTrackerFiducials, 
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, AcceptTrackerFiducial, 
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, EndSetTrackerFiducials, 
                           Initial, ReportInvalidRequest );  
  igstkAddTransitionMacro( Initial, StartTracking, 
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, StopTracking, 
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, DisconnectTracker, 
                           Initial, ReportInvalidRequest ); 

  /** LoadingImage State */

  igstkAddTransitionMacro( LoadingImage, Success,
                           ConfirmingImagePatientName, ConfirmPatientName );
  igstkAddTransitionMacro( LoadingImage, Failure, 
                           Initial, ReportFailuredImageLoaded );

 //complete table for state: LoadingImage State

  igstkAddTransitionMacro( LoadingImage, LoadImage, 
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, LoadToolSpatialObject, 
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, ConfirmImagePatientName, 
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, LoadTargetMesh, 
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, StartSetImageFiducials, 
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, SetPickingPosition, 
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, EndSetImageFiducials, 
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, RegisterTracker, 
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, ConfigureTracker, 
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, InitializeTracker, 
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, StartSetTrackerFiducials, 
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, AcceptTrackerFiducial, 
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, EndSetTrackerFiducials, 
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, StartTracking, 
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, StopTracking, 
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, DisconnectTracker, 
                           LoadingImage, ReportInvalidRequest );
  
   /** ConfirmingImagePatientName State */

  igstkAddTransitionMacro( ConfirmingImagePatientName, Success,
                           ImageReady, ReportSuccessImageLoaded );

  igstkAddTransitionMacro( ConfirmingImagePatientName, Failure, 
                           Initial, ReportFailuredImageLoaded );

  //complete table for state: ConfirmingImagePatientName

  igstkAddTransitionMacro( ConfirmingImagePatientName, LoadImage, 
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, LoadToolSpatialObject, 
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, ConfirmImagePatientName, 
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, LoadTargetMesh, 
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, StartSetImageFiducials,
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, SetPickingPosition,
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, EndSetImageFiducials,
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, RegisterTracker,
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, ConfigureTracker,
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, InitializeTracker,
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, StartSetTrackerFiducials, 
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, AcceptTrackerFiducial,
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, EndSetTrackerFiducials, 
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, StartTracking,
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, StopTracking,
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, DisconnectTracker, 
                           ConfirmingImagePatientName, ReportInvalidRequest );

  /** ImageReady State */
  
  igstkAddTransitionMacro( ImageReady, LoadToolSpatialObject,
                           LoadingSpatialObject, LoadToolSpatialObject );
  igstkAddTransitionMacro( ImageReady, LoadTargetMesh,
                           LoadingTargetMesh, LoadTargetMesh );
  igstkAddTransitionMacro( ImageReady, SetPickingPosition, 
                           ImageReady, SetImagePicking );
  igstkAddTransitionMacro( ImageReady, StartSetImageFiducials,
                           SettingImageFiducials, ReportSuccessStartSetImageFiducials );
  igstkAddTransitionMacro( ImageReady, ConfigureTracker, 
                           ConfiguringTracker, ConfigureTracker );  

  //complete table for state: ImageReady
  igstkAddTransitionMacro( ImageReady, Success, 
                           ImageReady, ReportInvalidRequest );
  igstkAddTransitionMacro( ImageReady, Failure, 
                           ImageReady, ReportInvalidRequest );
  igstkAddTransitionMacro( ImageReady, LoadImage, 
                           ImageReady, ReportInvalidRequest );
  igstkAddTransitionMacro( ImageReady, ConfirmImagePatientName, 
                           ImageReady, ReportInvalidRequest );
  igstkAddTransitionMacro( ImageReady, EndSetImageFiducials,
                           ImageReady, ReportInvalidRequest );
  igstkAddTransitionMacro( ImageReady, InitializeTracker, 
                           ImageReady, ReportInvalidRequest );
  igstkAddTransitionMacro( ImageReady, RegisterTracker,
                           ImageReady, ReportInvalidRequest );
  igstkAddTransitionMacro( ImageReady, StartSetTrackerFiducials, 
                           ImageReady, ReportInvalidRequest );
  igstkAddTransitionMacro( ImageReady, AcceptTrackerFiducial,
                           ImageReady, ReportInvalidRequest );
  igstkAddTransitionMacro( ImageReady, EndSetTrackerFiducials, 
                           ImageReady, ReportInvalidRequest );
  igstkAddTransitionMacro( ImageReady, StartTracking,
                           ImageReady, ReportInvalidRequest );
  igstkAddTransitionMacro( ImageReady, StopTracking,
                           ImageReady, ReportInvalidRequest );  
  igstkAddTransitionMacro( ImageReady, DisconnectTracker,
                           ImageReady, ReportInvalidRequest );
  
   
  /** LoadingSpatialObject State */

  igstkAddTransitionMacro( LoadingSpatialObject, Success,
                           ImageReady, ReportSuccessSpatialObjectLoaded );

  igstkAddTransitionMacro( LoadingSpatialObject, Failure,
                           ImageReady, ReportFailuredSpatialObjectLoaded );

  //complete table for state: LoadingTargetMesh

  igstkAddTransitionMacro( LoadingSpatialObject, LoadImage, 
                           LoadingSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingSpatialObject, LoadToolSpatialObject,
                           LoadingSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingSpatialObject, ConfirmImagePatientName, 
                           LoadingSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingSpatialObject, LoadTargetMesh,
                           LoadingSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingSpatialObject, StartSetImageFiducials, 
                           LoadingSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingSpatialObject, SetPickingPosition, 
                           LoadingSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingSpatialObject, EndSetImageFiducials, 
                           LoadingSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingSpatialObject, RegisterTracker, 
                           LoadingSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingSpatialObject, StartSetTrackerFiducials, 
                           LoadingSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingSpatialObject, AcceptTrackerFiducial, 
                           LoadingSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingSpatialObject, EndSetTrackerFiducials, 
                           LoadingSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingSpatialObject, ConfigureTracker, 
                           LoadingSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingSpatialObject, InitializeTracker, 
                           LoadingSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingSpatialObject, StartTracking, 
                           LoadingSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingSpatialObject, StopTracking, 
                           LoadingSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingSpatialObject, DisconnectTracker, 
                           LoadingSpatialObject, ReportInvalidRequest );
  
  /** LoadingTargetMesh State */

  igstkAddTransitionMacro( LoadingTargetMesh, Success,
                           ImageReady, ReportSuccessTargetMeshLoaded );

  igstkAddTransitionMacro( LoadingTargetMesh, Failure,
                           ImageReady, ReportFailuredTargetMeshLoaded );

  //complete table for state: LoadingTargetMesh

  igstkAddTransitionMacro( LoadingTargetMesh, LoadImage, 
                           LoadingTargetMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTargetMesh, LoadToolSpatialObject,
                           LoadingTargetMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTargetMesh, ConfirmImagePatientName, 
                           LoadingTargetMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTargetMesh, LoadTargetMesh, 
                           LoadingTargetMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTargetMesh, StartSetImageFiducials, 
                           LoadingTargetMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTargetMesh, SetPickingPosition, 
                           LoadingTargetMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTargetMesh, EndSetImageFiducials, 
                           LoadingTargetMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTargetMesh, RegisterTracker, 
                           LoadingTargetMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTargetMesh, StartSetTrackerFiducials, 
                           LoadingTargetMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTargetMesh, AcceptTrackerFiducial, 
                           LoadingTargetMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTargetMesh, EndSetTrackerFiducials, 
                           LoadingTargetMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTargetMesh, ConfigureTracker, 
                           LoadingTargetMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTargetMesh, InitializeTracker, 
                           LoadingTargetMesh, ReportInvalidRequest );

  igstkAddTransitionMacro( LoadingTargetMesh, StartTracking, 
                           LoadingTargetMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTargetMesh, StopTracking, 
                           LoadingTargetMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTargetMesh, DisconnectTracker, 
                           LoadingTargetMesh, ReportInvalidRequest );
  

  /** SettingImageFiducials State */  

  igstkAddTransitionMacro( SettingImageFiducials, SetPickingPosition, 
                           SettingImageFiducials, SetImageFiducial );

  igstkAddTransitionMacro( SettingImageFiducials, EndSetImageFiducials, 
                           ImageReady, ReportSuccessEndSetImageFiducials );

  //complete table for state: SettingImageFiducials        

  igstkAddTransitionMacro( SettingImageFiducials, Success,
                           SettingImageFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingImageFiducials, Failure,
                           SettingImageFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingImageFiducials, LoadImage, 
                           SettingImageFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingImageFiducials, ConfirmImagePatientName, 
                           SettingImageFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingImageFiducials, LoadToolSpatialObject, 
                           SettingImageFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingImageFiducials, LoadTargetMesh, 
                           SettingImageFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingImageFiducials, StartSetImageFiducials, 
                           SettingImageFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingImageFiducials, ConfigureTracker, 
                           SettingImageFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingImageFiducials, InitializeTracker, 
                           SettingImageFiducials, ReportInvalidRequest );
  
  igstkAddTransitionMacro( SettingImageFiducials, RegisterTracker, 
                           SettingImageFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingImageFiducials, StartSetTrackerFiducials, 
                           SettingImageFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingImageFiducials, AcceptTrackerFiducial, 
                           SettingImageFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingImageFiducials, EndSetTrackerFiducials, 
                           SettingImageFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingImageFiducials, StartTracking, 
                           SettingImageFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingImageFiducials, StopTracking, 
                           SettingImageFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingImageFiducials, DisconnectTracker, 
                           SettingImageFiducials, ReportInvalidRequest );
  
  /** ConfiguringTracker State */

  igstkAddTransitionMacro( ConfiguringTracker, Success, 
                           TrackerConfigurationReady, ReportSuccessTrackerConfiguration );

  igstkAddTransitionMacro( ConfiguringTracker, Failure, 
                           ImageReady, ReportFailureTrackerConfiguration );

  //complete table for state: ConfiguringTracker

  igstkAddTransitionMacro( ConfiguringTracker, LoadImage, 
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, LoadToolSpatialObject, 
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, ConfirmImagePatientName, 
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, LoadTargetMesh, 
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, StartSetImageFiducials, 
                           ConfiguringTracker, ReportInvalidRequest ); 
  igstkAddTransitionMacro( ConfiguringTracker, SetPickingPosition, 
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, EndSetImageFiducials, 
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, ConfigureTracker, 
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, InitializeTracker, 
                           ConfiguringTracker, ReportInvalidRequest );
  
  igstkAddTransitionMacro( ConfiguringTracker, RegisterTracker, 
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, StartSetTrackerFiducials, 
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, AcceptTrackerFiducial, 
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, EndSetTrackerFiducials, 
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, StartTracking, 
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, StopTracking, 
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, DisconnectTracker, 
                           ConfiguringTracker, ReportInvalidRequest );
  

  /** TrackerConfigurationReady State */

  igstkAddTransitionMacro( TrackerConfigurationReady, InitializeTracker, 
                           InitializingTracker, InitializeTracker );

  //complete table for state: TrackerConfigurationReady
  igstkAddTransitionMacro( TrackerConfigurationReady, Success, 
                           TrackerConfigurationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerConfigurationReady, Failure, 
                           TrackerConfigurationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerConfigurationReady, LoadImage, 
                           TrackerConfigurationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerConfigurationReady, LoadToolSpatialObject, 
                           TrackerConfigurationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerConfigurationReady, ConfirmImagePatientName, 
                           TrackerConfigurationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerConfigurationReady, LoadTargetMesh, 
                           TrackerConfigurationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerConfigurationReady, StartSetImageFiducials, 
                           TrackerConfigurationReady, ReportInvalidRequest ); 
  igstkAddTransitionMacro( TrackerConfigurationReady, SetPickingPosition, 
                           TrackerConfigurationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerConfigurationReady, EndSetImageFiducials, 
                           TrackerConfigurationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerConfigurationReady, ConfigureTracker, 
                           TrackerConfigurationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerConfigurationReady, StartSetTrackerFiducials, 
                           TrackerConfigurationReady, ReportInvalidRequest );

  igstkAddTransitionMacro( TrackerConfigurationReady, RegisterTracker, 
                           TrackerConfigurationReady, ReportInvalidRequest );  
  igstkAddTransitionMacro( TrackerConfigurationReady, AcceptTrackerFiducial, 
                           TrackerConfigurationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerConfigurationReady, EndSetTrackerFiducials, 
                           TrackerConfigurationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerConfigurationReady, StartTracking, 
                           TrackerConfigurationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerConfigurationReady, StopTracking, 
                           TrackerConfigurationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerConfigurationReady, DisconnectTracker, 
                           TrackerConfigurationReady, ReportInvalidRequest );
  

  /** InitializingTracker State */

  igstkAddTransitionMacro( InitializingTracker, Success, 
                           TrackerInitializationReady, ReportSuccessTrackerInitialization );

  igstkAddTransitionMacro( InitializingTracker, Failure, 
                           ImageReady, ReportFailureTrackerInitialization );

  //complete table for state: InitializingTracker

  igstkAddTransitionMacro( InitializingTracker, LoadImage, 
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, LoadToolSpatialObject, 
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, ConfirmImagePatientName, 
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, LoadTargetMesh, 
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, StartSetImageFiducials, 
                           InitializingTracker, ReportInvalidRequest ); 
  igstkAddTransitionMacro( InitializingTracker, SetPickingPosition, 
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, EndSetImageFiducials, 
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, ConfigureTracker, 
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, InitializeTracker, 
                           InitializingTracker, ReportInvalidRequest );
  
  igstkAddTransitionMacro( InitializingTracker, RegisterTracker, 
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, StartSetTrackerFiducials, 
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, AcceptTrackerFiducial, 
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, EndSetTrackerFiducials, 
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, StartTracking, 
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, StopTracking, 
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, DisconnectTracker, 
                           InitializingTracker, ReportInvalidRequest );
  

  /** TrackerInitializationReady State */

  igstkAddTransitionMacro( TrackerInitializationReady, StartSetTrackerFiducials, 
                           SettingTrackerFiducials, StartSetTrackerFiducials );

  igstkAddTransitionMacro( TrackerInitializationReady, SetPickingPosition, 
                           TrackerInitializationReady, SetImagePicking );

  //complete table for state: TrackerInitializationReady
  igstkAddTransitionMacro( TrackerInitializationReady, Success, 
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, Failure, 
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, LoadImage, 
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, LoadToolSpatialObject, 
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, ConfirmImagePatientName, 
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, LoadTargetMesh, 
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, StartSetImageFiducials, 
                           TrackerInitializationReady, ReportInvalidRequest ); 
  igstkAddTransitionMacro( TrackerInitializationReady, EndSetImageFiducials, 
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, ConfigureTracker, 
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, InitializeTracker, 
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, RegisterTracker, 
                           TrackerInitializationReady, ReportInvalidRequest );  
  igstkAddTransitionMacro( TrackerInitializationReady, AcceptTrackerFiducial, 
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, EndSetTrackerFiducials, 
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, StartTracking, 
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, DisconnectTracker, 
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, StopTracking, 
                           TrackerInitializationReady, ReportInvalidRequest );
  

  /** SettingTrackerFiducials State*/

  igstkAddTransitionMacro( SettingTrackerFiducials, AcceptTrackerFiducial,
                           SettingTrackerFiducials, AcceptTrackerFiducial );

  igstkAddTransitionMacro( SettingTrackerFiducials, EndSetTrackerFiducials,
                           EndingSetTrackerFiducials, EndSetTrackerFiducials );

  //complete table for state: SettingTrackerFiducials
  igstkAddTransitionMacro( SettingTrackerFiducials, Success, 
                           SettingTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingTrackerFiducials, Failure, 
                           SettingTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingTrackerFiducials, LoadImage, 
                           SettingTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingTrackerFiducials, LoadToolSpatialObject, 
                           SettingTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingTrackerFiducials, ConfirmImagePatientName, 
                           SettingTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingTrackerFiducials, LoadTargetMesh, 
                           SettingTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingTrackerFiducials, StartSetImageFiducials, 
                           SettingTrackerFiducials, ReportInvalidRequest ); 
  igstkAddTransitionMacro( SettingTrackerFiducials, SetPickingPosition, 
                           SettingTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingTrackerFiducials, EndSetImageFiducials, 
                           SettingTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingTrackerFiducials, ConfigureTracker, 
                           SettingTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingTrackerFiducials, InitializeTracker, 
                           SettingTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingTrackerFiducials, RegisterTracker, 
                           SettingTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingTrackerFiducials, StartSetTrackerFiducials, 
                           SettingTrackerFiducials, ReportInvalidRequest );

  igstkAddTransitionMacro( SettingTrackerFiducials, StartTracking, 
                           SettingTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingTrackerFiducials, StopTracking, 
                           SettingTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingTrackerFiducials, DisconnectTracker, 
                           SettingTrackerFiducials, ReportInvalidRequest );

  /** EndingSetTrackerFiducials State*/

  igstkAddTransitionMacro( EndingSetTrackerFiducials, Success, 
                           TrackerFiducialsReady, ReportSuccessEndSetTrackerFiducials );

  igstkAddTransitionMacro( EndingSetTrackerFiducials, Failure, 
                           SettingTrackerFiducials, ReportFailureEndSetTrackerFiducials );

  //complete table for state: EndingSetTrackerFiducials
  igstkAddTransitionMacro( EndingSetTrackerFiducials, LoadImage, 
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, LoadToolSpatialObject, 
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, ConfirmImagePatientName, 
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, LoadTargetMesh, 
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, StartSetImageFiducials, 
                           EndingSetTrackerFiducials, ReportInvalidRequest ); 
  igstkAddTransitionMacro( EndingSetTrackerFiducials, SetPickingPosition, 
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, EndSetImageFiducials, 
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, ConfigureTracker, 
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, InitializeTracker, 
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, AcceptTrackerFiducial,
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, EndSetTrackerFiducials,
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, RegisterTracker, 
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, StartSetTrackerFiducials, 
                           EndingSetTrackerFiducials, ReportInvalidRequest );


  igstkAddTransitionMacro( EndingSetTrackerFiducials, StartTracking, 
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, StopTracking, 
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, DisconnectTracker, 
                           EndingSetTrackerFiducials, ReportInvalidRequest );

  /** TrackerFiducialsReady State*/

  igstkAddTransitionMacro( TrackerFiducialsReady, RegisterTracker, 
                           RegisteringTracker, TrackerRegistration );

  //complete table for state: TrackerFiducialsReady

  igstkAddTransitionMacro( TrackerFiducialsReady, Success, 
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, Failure, 
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, LoadImage, 
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, LoadToolSpatialObject, 
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, ConfirmImagePatientName, 
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, LoadTargetMesh, 
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, StartSetImageFiducials, 
                           TrackerFiducialsReady, ReportInvalidRequest ); 
  igstkAddTransitionMacro( TrackerFiducialsReady, SetPickingPosition, 
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, EndSetImageFiducials, 
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, InitializeTracker, 
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, ConfigureTracker, 
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, StartSetTrackerFiducials, 
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, AcceptTrackerFiducial, 
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, EndSetTrackerFiducials, 
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, StartTracking, 
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, StopTracking, 
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, DisconnectTracker, 
                           TrackerFiducialsReady, ReportInvalidRequest );

  /** RegisteringTracker State*/

  igstkAddTransitionMacro( RegisteringTracker, Success, 
                           AcceptingRegistration, ReportSuccessTrackerRegistration );
  igstkAddTransitionMacro( RegisteringTracker, Failure, 
                           TrackerInitializationReady, ReportFailureTrackerRegistration );

  //complete table for state: RegisteringTracker

  igstkAddTransitionMacro( RegisteringTracker, LoadImage, 
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, LoadToolSpatialObject, 
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, ConfirmImagePatientName, 
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, LoadTargetMesh, 
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, StartSetImageFiducials, 
                           RegisteringTracker, ReportInvalidRequest ); 
  igstkAddTransitionMacro( RegisteringTracker, SetPickingPosition, 
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, EndSetImageFiducials, 
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, ConfigureTracker, 
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, InitializeTracker, 
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, StartSetTrackerFiducials, 
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, AcceptTrackerFiducial, 
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, EndSetTrackerFiducials, 
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, RegisterTracker,
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, StartTracking, 
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, StopTracking, 
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, DisconnectTracker, 
                           RegisteringTracker, ReportInvalidRequest );


  /** AcceptingRegistration State*/

  igstkAddTransitionMacro( AcceptingRegistration, Success, 
                           RegistrationReady, ReportSuccessAcceptingRegistration );
  igstkAddTransitionMacro( AcceptingRegistration, Failure, 
                           TrackerInitializationReady, ReportFailureAcceptingRegistration );

  //complete table for state: RegisteringTracker

  igstkAddTransitionMacro( AcceptingRegistration, LoadImage, 
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, LoadToolSpatialObject, 
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, ConfirmImagePatientName, 
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, LoadTargetMesh, 
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, StartSetImageFiducials, 
                           AcceptingRegistration, ReportInvalidRequest ); 
  igstkAddTransitionMacro( AcceptingRegistration, SetPickingPosition, 
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, EndSetImageFiducials, 
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, ConfigureTracker, 
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, InitializeTracker, 
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, StartSetTrackerFiducials, 
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, AcceptTrackerFiducial, 
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, EndSetTrackerFiducials, 
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, RegisterTracker, 
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, StartTracking, 
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, StopTracking, 
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, DisconnectTracker, 
                           AcceptingRegistration, ReportInvalidRequest );

  /** ReagistrationReady State*/

  igstkAddTransitionMacro( RegistrationReady, StartTracking, 
                           StartingTracker, StartTracking);

  igstkAddTransitionMacro( RegistrationReady, DisconnectTracker, 
                           DisconnectingTracker, DisconnectTracker );

  //complete table for state: StartingTracker

  igstkAddTransitionMacro( RegistrationReady, Success, 
                           RegistrationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( RegistrationReady, Failure, 
                           RegistrationReady, ReportInvalidRequest );

  igstkAddTransitionMacro( RegistrationReady, LoadImage, 
                           RegistrationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( RegistrationReady, LoadToolSpatialObject, 
                           RegistrationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( RegistrationReady, ConfirmImagePatientName, 
                           RegistrationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( RegistrationReady, LoadTargetMesh, 
                           RegistrationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( RegistrationReady, StartSetImageFiducials, 
                           RegistrationReady, ReportInvalidRequest ); 
  igstkAddTransitionMacro( RegistrationReady, SetPickingPosition, 
                           RegistrationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( RegistrationReady, StartSetTrackerFiducials, 
                           RegistrationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( RegistrationReady, EndSetImageFiducials, 
                           RegistrationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( RegistrationReady, ConfigureTracker, 
                           RegistrationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( RegistrationReady, InitializeTracker, 
                           RegistrationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( RegistrationReady, RegisterTracker, 
                           RegistrationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( RegistrationReady, AcceptTrackerFiducial, 
                           RegistrationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( RegistrationReady, EndSetTrackerFiducials, 
                           RegistrationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( RegistrationReady, StopTracking, 
                           RegistrationReady, ReportInvalidRequest );

  /** StartingTracker State*/

  igstkAddTransitionMacro( StartingTracker, Success, 
                           Tracking, ReportSuccessStartTracking);

  igstkAddTransitionMacro( StartingTracker, Failure, 
                           ImageReady, ReportFailureStartTracking );

  //complete table for state: StartingTracker

  igstkAddTransitionMacro( StartingTracker, LoadImage, 
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, LoadToolSpatialObject, 
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, ConfirmImagePatientName, 
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, LoadTargetMesh, 
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, StartSetImageFiducials, 
                           StartingTracker, ReportInvalidRequest ); 
  igstkAddTransitionMacro( StartingTracker, SetPickingPosition, 
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, StartSetTrackerFiducials, 
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, EndSetImageFiducials, 
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, ConfigureTracker, 
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, InitializeTracker, 
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, RegisterTracker, 
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, AcceptTrackerFiducial, 
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, EndSetTrackerFiducials, 
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, StartTracking, 
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, StopTracking, 
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, DisconnectTracker, 
                           StartingTracker, ReportInvalidRequest );

  /** Tracking State */

  igstkAddTransitionMacro( Tracking, StopTracking, 
                           StoppingTracker, StopTracking );

  igstkAddTransitionMacro( Tracking, StartSetTrackerFiducials, 
                           SettingTrackerFiducials, StartSetTrackerFiducials );

  //complete table for state: Tracking

  igstkAddTransitionMacro( Tracking, Success, 
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, Failure, 
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, LoadImage, 
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, LoadToolSpatialObject, 
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, ConfirmImagePatientName, 
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, LoadTargetMesh, 
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, StartSetImageFiducials, 
                           Tracking, ReportInvalidRequest ); 
  igstkAddTransitionMacro( Tracking, EndSetImageFiducials, 
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, InitializeTracker, 
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, ConfigureTracker, 
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, RegisterTracker, 
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, AcceptTrackerFiducial, 
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, EndSetTrackerFiducials, 
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, SetPickingPosition, 
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, StartTracking, 
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, DisconnectTracker, 
                           Tracking, ReportInvalidRequest );

   /** StoppingTracker State */

  igstkAddTransitionMacro( StoppingTracker, Success, 
                           RegistrationReady, ReportSuccessStopTracking);

  igstkAddTransitionMacro( StoppingTracker, Failure,
                           Tracking, ReportFailureStopTracking );

  //complete table for state: StoppingTracker

  igstkAddTransitionMacro( StoppingTracker, LoadImage, 
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, LoadToolSpatialObject, 
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, ConfirmImagePatientName, 
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, LoadTargetMesh, 
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, StartSetImageFiducials, 
                           StoppingTracker, ReportInvalidRequest ); 
  igstkAddTransitionMacro( StoppingTracker, SetPickingPosition, 
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, EndSetImageFiducials, 
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, InitializeTracker, 
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, ConfigureTracker, 
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, RegisterTracker, 
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, StartSetTrackerFiducials, 
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, AcceptTrackerFiducial, 
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, EndSetTrackerFiducials, 
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, StartTracking, 
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, StopTracking, 
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, DisconnectTracker, 
                           StoppingTracker, ReportInvalidRequest );

   /** DisconnectingTracker Tracker */

  igstkAddTransitionMacro( DisconnectingTracker, Success, 
                           ImageReady, ReportSuccessTrackerDisconnection);

  igstkAddTransitionMacro( DisconnectingTracker, Failure,
                           RegistrationReady, ReportFailureTrackerDisconnection );

  //complete table for state: DisconnectingTracker

  igstkAddTransitionMacro( DisconnectingTracker, LoadImage, 
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, LoadToolSpatialObject, 
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, ConfirmImagePatientName, 
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, LoadTargetMesh, 
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, StartSetImageFiducials, 
                           DisconnectingTracker, ReportInvalidRequest ); 
  igstkAddTransitionMacro( DisconnectingTracker, SetPickingPosition, 
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, EndSetImageFiducials, 
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, InitializeTracker, 
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, ConfigureTracker, 
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, RegisterTracker, 
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, StartSetTrackerFiducials, 
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, AcceptTrackerFiducial, 
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, EndSetTrackerFiducials, 
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, StartTracking, 
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, StopTracking, 
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, DisconnectTracker, 
                           DisconnectingTracker, ReportInvalidRequest );

  /** Set Initial State */

  igstkSetInitialStateMacro( Initial );

  m_StateMachine.SetReadyToRun();
/*
  std::ofstream ofile;
  ofile.open("NavigatorStateMachineDiagram.dot");
  const bool skipLoops = false;
  this->ExportStateMachineDescription( ofile, skipLoops );
  ofile.close();
*/
}

/** -----------------------------------------------------------------
*     Destructor
*  -----------------------------------------------------------------
*/
Navigator::~Navigator()
{  

}

void Navigator::ConfigureTrackerProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::ConfigureTrackerProcessing called...\n" )
  
  m_TrackerConfiguration = 
    new igstk::MicronTrackerConfiguration();

  if ( itksys::SystemTools::FileExists("C:/Program Files/Claron Technology/MicronTracker/CalibrationFiles", false) )
    m_TrackerConfiguration->SetCameraCalibrationFileDirectory( 
      "C:/Program Files/Claron Technology/MicronTracker/CalibrationFiles" );
  else if ( itksys::SystemTools::FileExists("D:/Archivos de programa/Claron Technology/MicronTracker/CalibrationFiles", false) )
    m_TrackerConfiguration->SetCameraCalibrationFileDirectory( 
      "D:/Archivos de programa/Claron Technology/MicronTracker/CalibrationFiles" );
  else
  {
    igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::RequestConfigureTracker Could not find MicronTracker/CalibrationFiles \n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
  }

  if ( itksys::SystemTools::FileExists("C:/Program Files/Claron Technology/MicronTracker/MTDemoCPP.ini", true) )
    m_TrackerConfiguration->SetInitializationFile( 
      "C:/Program Files/Claron Technology/MicronTracker/MTDemoCPP.ini" );
  else if ( itksys::SystemTools::FileExists("D:/Archivos de programa/Claron Technology/MicronTracker/MTDemoCPP.ini", true) )
    m_TrackerConfiguration->SetInitializationFile( 
      "D:/Archivos de programa/Claron Technology/MicronTracker/MTDemoCPP.ini" );
  else
  {
    igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::RequestConfigureTracker Could not find MicronTracker/MTDemoCPP.ini \n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
  }

 if ( itksys::SystemTools::FileExists("C:/Program Files/Claron Technology/MicronTracker/Markers", false) )
    m_TrackerConfiguration->SetTemplatesDirectory( 
      "C:/Program Files/Claron Technology/MicronTracker/Markers" ); 
 else if ( itksys::SystemTools::FileExists("D:/Archivos de programa/Claron Technology/MicronTracker/Markers", false) )
    m_TrackerConfiguration->SetTemplatesDirectory( 
      "D:/Archivos de programa/Claron Technology/MicronTracker/Markers" );
 else
 {
    igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::RequestConfigureTracker Could not find MicronTracker/Markers \n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
 }
  
  //set the tool parameters
  igstk::MicronToolConfiguration toolConfig;

  toolConfig.SetToolName( "sPtr" );
  toolConfig.SetMarkerName( "sPtr" ); //"sPtr" );
  igstk::Transform* calibrationTransform =  
    this->ReadTransformFile( "claron_sptr_Calibration.xml" );


  if ( calibrationTransform != NULL )
  {
    toolConfig.SetCalibrationTransform( *calibrationTransform ); 
  }
  else
  {
    igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::RequestConfigureTracker calibration .xml file was not loaded...\n" )
  }

  m_TrackerConfiguration->RequestAddTool( &toolConfig );

  igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::RequestConfigureTracker tool added...\n" )
  
  //set the tool parameters
  igstk::MicronToolConfiguration reftool;  
  reftool.SetToolName( "abdomen" );
  reftool.SetMarkerName( "abdomen" ); //reference
  m_TrackerConfiguration->RequestAddReference( &reftool );

  igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::RequestConfigureTracker reference tool added...\n" )

  m_TrackerConfiguration->RequestSetFrequency( TRACKER_DEFAULT_REFRESH_RATE );

  m_StateMachine.PushInput( m_SuccessInput );
  m_StateMachine.ProcessInputs();

}

void Navigator::RequestLoadImage()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::RequestLoadImage called...\n" )
  m_StateMachine.PushInput( m_LoadImageInput );
  m_StateMachine.ProcessInputs();
}

void Navigator::RequestLoadToolSpatialObject()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::RequestLoadToolSpatialObject called...\n" )
  m_StateMachine.PushInput( m_LoadToolSpatialObjectInput );
  m_StateMachine.ProcessInputs();
}

void Navigator::RequestLoadTargetMesh()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::RequestLoadTargetMesh called...\n" )
  m_StateMachine.PushInput( m_LoadTargetMeshInput );
  m_StateMachine.ProcessInputs();
}

void Navigator::RequestToggleSetImageFiducials()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::RequestToggleSetImageFiducials called...\n" )

  if ( !m_ModifyImageFiducialsEnabled )
    m_StateMachine.PushInput( m_StartSetImageFiducialsInput );
  else
    m_StateMachine.PushInput( m_EndSetImageFiducialsInput );

  m_StateMachine.ProcessInputs();
}

void Navigator::RequestAcceptRegistration()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::RequestAcceptRegistration called...\n" )

  m_StateMachine.PushInput( m_SuccessInput);
  m_StateMachine.ProcessInputs();
}

void Navigator::RequestRejectRegistration()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::RequestRejectRegistration called...\n" )

  m_StateMachine.PushInput( m_FailureInput);
  m_StateMachine.ProcessInputs();
}

void Navigator::RequestConfigureTracker()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::RequestConfigureTracker called...\n" )

  m_StateMachine.PushInput( m_ConfigureTrackerInput );
  m_StateMachine.ProcessInputs();
}

void Navigator::RequestInitializeTracker()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::RequestInitializeTracker called...\n" )

  m_StateMachine.PushInput( m_InitializeTrackerInput );
  m_StateMachine.ProcessInputs();
}

void Navigator::RequestStartSetTrackerFiducials()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::RequestStartSetTrackerFiducials called...\n" )

  m_StateMachine.PushInput( m_StartSetTrackerFiducialsInput );
  m_StateMachine.ProcessInputs();
}

void Navigator::RequestAcceptTrackerFiducial()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::RequestAcceptTrackerFiducial called...\n" ) 

  m_StateMachine.PushInput( m_AcceptTrackerFiducialInput );
  m_StateMachine.ProcessInputs();
}

void Navigator::RequestEndSetTrackerFiducials()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::RequestEndSetTrackerFiducials called...\n" )

  m_StateMachine.PushInput( m_EndSetTrackerFiducialsInput );
  m_StateMachine.ProcessInputs();
}

void Navigator::RequestTrackerRegistration()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::RequestTrackerRegistration called...\n" )

  m_StateMachine.PushInput( m_RegisterTrackerInput );
  m_StateMachine.ProcessInputs();
}

void Navigator::RequestStartTracking()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::RequestStartTracking called...\n" )

  m_StateMachine.PushInput( m_StartTrackingInput );
  m_StateMachine.ProcessInputs();  
}

void Navigator::RequestStopTracking()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::RequestStopTracking called...\n" )

  m_StateMachine.PushInput( m_StopTrackingInput );
  m_StateMachine.ProcessInputs();  
}


void Navigator::RequestToggleOrthogonalPlanes()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::RequestToggleOrthogonalPlanes called...\n" )
  
  m_ImagePlanesIn3DViewEnabled = !m_ImagePlanesIn3DViewEnabled;
  if (m_ImagePlanesIn3DViewEnabled)
  {
    this->EnableOrthogonalPlanes();
  }
  else
  {
    this->DisableOrthogonalPlanes();
  }
}

void Navigator::RequestDisconnectTracker()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::RequestDisconnectTracker called...\n" )

  m_StateMachine.PushInput( m_DisconnectTrackerInput );
  m_StateMachine.ProcessInputs();  
}

/** Method to be invoked when no operation is required */
void Navigator::NoProcessing()
{

}

/** Method to be invoked when an invalid operation was requested */
void 
Navigator::ReportInvalidRequestProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportInvalidRequestProcessing called...\n");
  this->InvokeEvent(InvalidRequestErrorEvent());
}


void 
Navigator::ReportSuccessImageLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportSuccessImageLoadedProcessing called...\n");

  m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 0, "" );
  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 0, "" );
  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 0, "" );

  m_ViewerGroup->RequestUpdateOverlays();

  for (int i=0; i<4; i++)
  {
    m_ViewerGroup->m_AxialView->RequestRemoveObject( m_AxialFiducialRepresentationVector[i] );
    m_ViewerGroup->m_SagittalView->RequestRemoveObject( m_SagittalFiducialRepresentationVector[i] );
    m_ViewerGroup->m_CoronalView->RequestRemoveObject( m_CoronalFiducialRepresentationVector[i] );
    m_ViewerGroup->m_3DView->RequestRemoveObject( m_3DViewFiducialRepresentationVector[i] );
  }

  m_ViewerGroup->redraw();
  Fl::check();
}

/** Method to be invoked on failured image loading */
void 
Navigator::ReportFailuredImageLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportFailuredImageLoadedProcessing called...\n");
}

/** Method to be invoked on successful spatial object loading */
void 
Navigator::ReportSuccessSpatialObjectLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportSuccessSpatialObjectLoadedProcessing called...\n");    

  this->RequestConfigureTracker();
}

/** Method to be invoked on failured spatial object loading */
void 
Navigator::ReportFailuredSpatialObjectLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportFailuredSpatialObjectLoadedProcessing called...\n");

}

/** Method to be invoked on successful target mesh loading */
void 
Navigator::ReportSuccessTargetMeshLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportSuccessTargetMeshLoadedProcessing called...\n");
}

/** Method to be invoked on failured target mesh loading */
void 
Navigator::ReportFailuredTargetMeshLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportFailuredTargetMeshLoadedProcessing called...\n");
}

/** Method to be invoked on successful start set image fiducials */
void 
Navigator::ReportSuccessStartSetImageFiducialsProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportSuccessStartSetImageFiducialsProcessing called...\n");
  m_ModifyButton->color(FL_YELLOW);

  m_ModifyImageFiducialsEnabled = true;
}

/** Method to be invoked on successful end set image fiducials */
void 
Navigator::ReportSuccessEndSetImageFiducialsProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportSuccessEndSetImageFiducialsProcessing called...\n");

  m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 2, " " );
  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 2, " " );
  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 2, " " );

  m_ViewerGroup->RequestUpdateOverlays();

  m_ModifyButton->color((Fl_Color)55);

  m_ModifyImageFiducialsEnabled = false;
}

/** Method to be invoked on successful start set tracker fiducials */
void 
Navigator::ReportSuccessStartSetTrackerFiducialsProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportSuccessStartSetTrackerFiducialsProcessing called...\n");

}


/** Method to be invoked on successful pinpointing */
void 
Navigator::ReportSuccessPinpointingTrackerFiducialProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportSuccessPinpointingTrackerFiducialProcessing called...\n");
}

/** Method to be invoked on failure pinpointing */
void 
Navigator::ReportFailurePinpointingTrackerFiducialProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportFailurePinpointingTrackerFiducialProcessing called...\n");
}

/** Method to be invoked on successful end set tracker fiducials */
void 
Navigator::ReportSuccessEndSetTrackerFiducialsProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportSuccessEndSetTrackerFiducialsProcessing called...\n");

  for (int i=0; i<4; i++)
  {
  m_ViewerGroup->m_AxialView->RequestRemoveObject( m_AxialFiducialRepresentationVector[i] );
  m_ViewerGroup->m_SagittalView->RequestRemoveObject( m_SagittalFiducialRepresentationVector[i] );
  m_ViewerGroup->m_CoronalView->RequestRemoveObject( m_CoronalFiducialRepresentationVector[i] );
  m_ViewerGroup->m_3DView->RequestRemoveObject( m_3DViewFiducialRepresentationVector[i] );
  }

  this->RequestTrackerRegistration();
}

void 
Navigator::AcceptTrackerFiducialProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "AcceptTrackerFiducialProcessing called...\n");

  //m_CollectingCandidates = true;

  typedef igstk::TransformObserver ObserverType;
  ObserverType::Pointer transformObserver = ObserverType::New();
  transformObserver->ObserveTransformEventsFrom( m_TrackerTool );
  transformObserver->Clear();
  m_TrackerTool->RequestComputeTransformTo( m_WorldReference );

  if ( transformObserver->GotTransform() )
  {
    int n = m_FiducialsPointList->value();
    int m = m_FiducialsPointList->size();
    m_LandmarksContainer[n] = 
          TransformToPoint( transformObserver->GetTransform() );
    m_AcceptedLandmarksContainer[n] = true;

    //todo: change color to fiducials to indicate that they were accepted

    if ( n < m )
    {
      m_FiducialsPointList->value(n+1);
      this->RequestChangeSelectedFiducial();
    }
    if ( n == m-2)
    {
      m_ModifyTrackerFiducialsBtn->label("Ready");     
      m_ModifyTrackerFiducialsBtn->color(FL_GREEN);
    }
  }
}

/** Method to be invoked on failure end set tracker fiducials */
void 
Navigator::ReportFailureEndSetTrackerFiducialsProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportFailureEndSetTrackerFiducialsProcessing called...\n");
}

/** Method to be invoked on successful tracker configuration */
void 
Navigator::ReportSuccessTrackerConfigurationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportSuccessTrackerConfigurationProcessing called...\n");

  this->RequestInitializeTracker();
}

/** Method to be invoked on failured tracker configuration */
void 
Navigator::ReportFailureTrackerConfigurationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportFailureTrackerConfigurationProcessing called...\n");
}

/** Method to be invoked on failured tracker initialization */
void 
Navigator::ReportFailureTrackerInitializationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportFailureTrackerConfigurationProcessing called...\n");
}


/** Method to be invoked on successful tracker initialization */
void 
Navigator::ReportSuccessTrackerInitializationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportSuccessTrackerInitializationProcessing called...\n");
}

/** Method to be invoked on successful registration acceptance */
void 
Navigator::ReportSuccessAcceptingRegistrationProcessing()
{
    igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportSuccessAcceptingRegistration called...\n");  

  // add the tool object to the image planes
  m_AxialPlaneSpatialObject->RequestSetToolSpatialObject( m_PointerSpatialObject );   
  m_SagittalPlaneSpatialObject->RequestSetToolSpatialObject( m_PointerSpatialObject );
  m_CoronalPlaneSpatialObject->RequestSetToolSpatialObject( m_PointerSpatialObject ); 

  m_CrossHair->RequestSetToolSpatialObject( m_PointerSpatialObject ); 
    
  // Set up tool projection for each view
  igstk::Transform identity;
  identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  m_ToolProjection = igstk::ToolProjectionObject::New();
  m_ToolProjection->SetSize(100);
  m_ToolProjection->RequestSetTransformAndParent( identity, m_WorldReference );

  m_AxialToolProjectionRepresentation = ToolProjectionRepresentationType::New();
  m_SagittalToolProjectionRepresentation = ToolProjectionRepresentationType::New();
  m_CoronalToolProjectionRepresentation = ToolProjectionRepresentationType::New();
  m_PerpendicularToolProjectionRepresentation = ToolProjectionRepresentationType::New();

  m_AxialToolProjectionRepresentation->RequestSetToolProjectionObject( m_ToolProjection );
  m_SagittalToolProjectionRepresentation->RequestSetToolProjectionObject( m_ToolProjection );
  m_CoronalToolProjectionRepresentation->RequestSetToolProjectionObject( m_ToolProjection );
  m_PerpendicularToolProjectionRepresentation->RequestSetToolProjectionObject( m_ToolProjection ); 

  m_AxialToolProjectionRepresentation->RequestSetReslicePlaneSpatialObject( m_AxialPlaneSpatialObject );
  m_SagittalToolProjectionRepresentation->RequestSetReslicePlaneSpatialObject( m_SagittalPlaneSpatialObject );
  m_CoronalToolProjectionRepresentation->RequestSetReslicePlaneSpatialObject( m_CoronalPlaneSpatialObject );

  m_AxialToolProjectionRepresentation->SetColor( 1,1,0 );
  m_SagittalToolProjectionRepresentation->SetColor( 1,1,0 );
  m_CoronalToolProjectionRepresentation->SetColor( 1,1,0 );
  m_PerpendicularToolProjectionRepresentation->SetColor( 1,1,0 );

  m_ViewerGroup->m_3DView->RequestAddObject( m_TrackerToolRepresentation );

  m_ViewerGroup->m_AxialView->RequestAddObject( m_AxialToolProjectionRepresentation );    
  m_ViewerGroup->m_SagittalView->RequestAddObject( m_SagittalToolProjectionRepresentation );
  m_ViewerGroup->m_CoronalView->RequestAddObject( m_CoronalToolProjectionRepresentation );

  m_ViewerGroup->m_3DView->RequestResetCamera();
}

/** Method to be invoked on failure registration acceptance */
void 
Navigator::ReportFailureAcceptingRegistrationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportFailureAcceptingRegistration called...\n");
}

/** Method to be invoked on successful tracker registration */
void 
Navigator::ReportSuccessTrackerRegistrationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportSuccessTrackerRegistrationProcessing called...\n");

   // ask the user to accept registration RMS error
   char buf[10];
   sprintf( buf, "%.2f", m_TrackerRMS);      

   m_RegistrationErrorLabel->value( buf );

   m_AcceptRegistrationWindow->show();

   Fl::check();       

}

/** Method to be invoked on failure tracker registration */
void 
Navigator::ReportFailureTrackerRegistrationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportFailureTrackerRegistrationProcessing called...\n");
}

/** Method to be invoked on failure tracker disconnection */
void 
Navigator::ReportFailureTrackerDisconnectionProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportFailureTrackerDisconnectionProcessing called...\n");
}

/** Method to be invoked on successful tracker disconnection */
void 
Navigator::ReportSuccessTrackerDisconnectionProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportSuccessTrackerDisconnectionProcessing called...\n");
  
  m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 1, "DISCONNECTED" );
  m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor(1, 1.0, 0.0, 0.0);

  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 1, "DISCONNECTED" );
  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor(1, 1.0, 0.0, 0.0);

  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 1, "DISCONNECTED" );
  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor(1, 1.0, 0.0, 0.0);

  m_ViewerGroup->RequestUpdateOverlays();

}

/** Method to be invoked on successful tracker start */
void 
Navigator::ReportSuccessStartTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportSuccessStartTrackingProcessing called...\n")

  char buf[50];
  sprintf( buf, "TRACKING (%.2f)", m_TrackerRMS);      
  //sprintf( buf, "TRACKING");      

  m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 1, buf );
  m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor(1, 0.0, 1.0, 0.0);

  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 1, buf );
  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor(1, 0.0, 1.0, 0.0);

  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 1, buf );
  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor(1, 0.0, 1.0, 0.0);

  m_ViewerGroup->RequestUpdateOverlays();
  
//  m_RunStopButton->label("Stop");
  //m_RunStopButton->deactivate();

  Fl::check();
}


/** Method to be invoked on failured tracker start */
void 
Navigator::ReportFailureStartTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportFailureStartTrackingProcessing called...\n")

  Fl::check();
}

/** Method to be invoked on failured tracker stop */
void 
Navigator::ReportFailureStopTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportFailureStopTrackingProcessing called...\n")
}

/** Method to be invoked on successful tracker stop */
void 
Navigator::ReportSuccessStopTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportSuccessStopTrackingProcessing called...\n")
 
  m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 1, "STOPPED" );
  m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor(1, 0.0, 1.0, 1.0);

  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 1, "STOPPED" );
  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor(1, 0.0, 1.0, 1.0);

  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 1, "STOPPED" );
  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor(1, 0.0, 1.0, 1.0); 

  m_ViewerGroup->RequestUpdateOverlays();
  
//  m_RunStopButton->label("Run");

  Fl::check();
}


void Navigator::LoadImageProcessing()
{
    igstkLogMacro2( m_Logger, DEBUG, 
                  "Navigator::LoadImageProcessing called...\n" )

    const char*  directoryName = 
      fl_dir_chooser("Indicate the DICOM directory ", m_ImageDir.c_str());

    if ( directoryName != NULL )
    {
        m_ImageDir = directoryName;

       //check if FLTK has added a slash
       if (itksys::SystemTools::StringEndsWith(m_ImageDir.c_str(),"/") )
       {
         m_ImageDir = m_ImageDir.substr (0,m_ImageDir.length()-1);
       }

       igstkLogMacro2( m_Logger, DEBUG, 
                          "Set ImageReader directory: " << directoryName << "\n" )

       m_ImageReader->RequestSetDirectory( directoryName );


       // Provide a progress observer to the image reader      
       m_ImageReader->RequestSetProgressCallback( m_ProgressCommand );

       m_ImageReader->RequestReadImage();

       m_ImageObserver = ImageObserver::New();
       m_ImageReader->AddObserver(ImageReaderType::ImageModifiedEvent(),
                                                        m_ImageObserver);

       m_ImageReader->RequestGetImage();

       if(!m_ImageObserver->GotImage())
       { 
         std::string errorMessage;
         errorMessage = "Could not open image";
         fl_alert( errorMessage.c_str() );
         fl_beep( FL_BEEP_ERROR );

         m_ImageObserver->RemoveAllObservers();
         m_ImageObserver = NULL;

         igstkLogMacro2( m_Logger, DEBUG, "Could not read image\n" )
         m_StateMachine.PushInput( m_FailureInput);
         m_StateMachine.ProcessInputs();

         return;
       }

       m_StateMachine.PushInput( m_SuccessInput);

       m_StateMachine.ProcessInputs();

       return;      
    }
    else
    {    
      igstkLogMacro2( m_Logger, DEBUG, "No directory was selected\n" )
      m_StateMachine.PushInput( m_FailureInput );
      m_StateMachine.ProcessInputs();
      return;
    }    
}

/** -----------------------------------------------------------------
* Confirm patient's name. This method asks for a confirmation to accept
* the loaded image.
*  -----------------------------------------------------------------
*/
void Navigator::ConfirmPatientNameProcessing()
{
   igstkLogMacro2( m_Logger, DEBUG, 
                  "Navigator::ConfirmImagePatientNameProcessing called...\n" )

   // ask the user to confirm patient's name

   if ( m_ImageObserver.IsNotNull() )
   {
       m_PatientNameLabel->value( m_ImageReader->GetPatientName().c_str() );
   }
   
   m_PatientNameWindow->show();

   Fl::check();       
}

/** -----------------------------------------------------------------
* Cancels image load in response to user indication.
*  -----------------------------------------------------------------
*/
void Navigator::RequestCancelImageLoad()
{
   igstkLogMacro2( m_Logger, DEBUG, 
                  "Navigator::RequestCancelImageLoad called...\n" )

   m_ImageReader->RemoveAllObservers();
   m_ImageObserver = NULL;

   fl_beep( FL_BEEP_ERROR );
   igstkLogMacro2( m_Logger, DEBUG, "Image load was canceled by the user\n" )
   m_StateMachine.PushInput( m_FailureInput);
   m_StateMachine.ProcessInputs();
}

/** -----------------------------------------------------------------
* Accepts image load in response to user indication.
*  -----------------------------------------------------------------
*/
void Navigator::RequestAcceptImageLoad()
{
  igstkLogMacro2( m_Logger, DEBUG, 
              "Navigator::RequestAcceptImageLoad called...\n" )

  m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText(3, "AXIAL VIEW");
  m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor(3, 1.0, 1.0, 1.0);
  m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontSize(3, 12);

  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText(3, "SAGITTAL VIEW");
  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor(3, 1.0, 1.0, 1.0);
  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontSize(3, 12);

  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText(3, "CORONAL VIEW");
  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor(3, 1.0, 1.0, 1.0);
  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontSize(3, 12);

  m_ViewerGroup->RequestUpdateOverlays();

  if ( m_ImageObserver.IsNotNull() )
  {
    m_ImageSpatialObject = m_ImageObserver->GetImage();
    m_ImageSpatialObject->SetLogger( this->GetLogger() );   
    this->ConnectImageRepresentation();
    this->ReadFiducials();    
    this->RequestChangeSelectedFiducial();
    m_StateMachine.PushInputBoolean( m_ImageReader->FileSuccessfullyRead(), 
                                     m_SuccessInput, m_FailureInput);
  }  
  m_StateMachine.ProcessInputs();
}

/** -----------------------------------------------------------------
* Load a spatial object for the principal tool. This method asks for a .msh file 
* with the ITK spatial object representation of the principal tool.
*  -----------------------------------------------------------------
*/
void Navigator::LoadToolSpatialObjectProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                "Navigator::LoadToolSpatialObjectProcessing called...\n" )

  const char* fileName = "tool.msh";

  if ( fileName != NULL )
  {
    igstkLogMacro2( m_Logger, DEBUG,
                        "Spatial Image filename: " << fileName << "\n" )

    MeshReaderType::Pointer reader = MeshReaderType::New();

    reader->RequestSetFileName( fileName );
    reader->RequestReadObject();

    MeshObjectObserver::Pointer observer = MeshObjectObserver::New();
    reader->AddObserver( igstk::MeshReader::MeshModifiedEvent(), observer);
    reader->RequestGetOutput();

    if(!observer->GotMeshObject())
    {
      igstkLogMacro2( m_Logger, DEBUG, "Could not read mesh spatial object\n" )
      m_StateMachine.PushInput( m_FailureInput );
      m_StateMachine.ProcessInputs();
      return;
    }

    m_PointerSpatialObject = observer->GetMeshObject();
      
    if (m_PointerSpatialObject.IsNull())
    {
      igstkLogMacro2( m_Logger, DEBUG, "Could not read mesh spatial object\n" ) 
      m_StateMachine.PushInput( m_FailureInput );
      m_StateMachine.ProcessInputs();
      return ;
    }

    igstk::Transform identity;
    identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

    m_PointerSpatialObject->RequestSetTransformAndParent( identity, m_WorldReference );

    m_TrackerToolRepresentation = MeshRepresentationType::New();
    m_TrackerToolRepresentation->RequestSetMeshObject( m_PointerSpatialObject );
    m_TrackerToolRepresentation->SetOpacity(1.0);
    m_TrackerToolRepresentation->SetColor(1,1,0);

    
    m_StateMachine.PushInput( m_SuccessInput );
    m_StateMachine.ProcessInputs();
  }
  else
  {
    igstkLogMacro2( m_Logger, DEBUG, 
      "No spatial object was selected.\n" )

    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
  }
}

/** -----------------------------------------------------------------
* Load target mesh. This method asks for a file with the target 
* segmentation: a mesh in the .msh format (see mesh SpatialObject in ITK)
* Any number of meshes can be loaded
*  -----------------------------------------------------------------
*/
void Navigator::LoadTargetMeshProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::LoadTargetMeshProcessing called...\n" )

  const char*  fileName = 
    fl_file_chooser("Select the target mesh file","*.msh", m_TargetDir.c_str());

  if ( fileName != NULL )
  {
     MeshReaderType::Pointer reader = MeshReaderType::New();
     reader->RequestSetFileName( fileName );

     reader->RequestReadObject();
   
     MeshObjectObserver::Pointer observer = MeshObjectObserver::New();

     reader->AddObserver( igstk::MeshReader::MeshModifiedEvent(), observer);

     reader->RequestGetOutput();

     if(!observer->GotMeshObject())
     {
         igstkLogMacro2( m_Logger, DEBUG, "Cannot read mesh\n" )
         m_StateMachine.PushInput( m_FailureInput);
         m_StateMachine.ProcessInputs();
         return;
     }

     m_TargetMeshObjectVector.push_back( observer->GetMeshObject() );
      
     if (m_TargetMeshObjectVector[m_NumberOfLoadedMeshes].IsNull())
     {
       igstkLogMacro2( m_Logger, DEBUG, "Cannot read mesh\n" )
       m_StateMachine.PushInput( m_FailureInput);
       m_StateMachine.ProcessInputs();
       return;
     }

     igstk::Transform identity;
     identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
     
     m_TargetMeshObjectVector[m_NumberOfLoadedMeshes]->RequestSetTransformAndParent( identity, m_WorldReference );
     
     MeshRepresentationType::Pointer rep = MeshRepresentationType::New();
     m_MeshRepresentationVector.push_back(rep);
     m_MeshRepresentationVector[m_NumberOfLoadedMeshes]->RequestSetMeshObject( 
       m_TargetMeshObjectVector[m_NumberOfLoadedMeshes] );

     MeshResliceRepresentationType::Pointer axialContour = MeshResliceRepresentationType::New();
     m_AxialMeshResliceRepresentationVector.push_back(axialContour);
     m_AxialMeshResliceRepresentationVector[m_NumberOfLoadedMeshes]->RequestSetMeshObject(
          m_TargetMeshObjectVector[m_NumberOfLoadedMeshes] );
     m_AxialMeshResliceRepresentationVector[m_NumberOfLoadedMeshes]->
        RequestSetReslicePlaneSpatialObject( m_AxialPlaneSpatialObject );

     MeshResliceRepresentationType::Pointer sagittalContour = MeshResliceRepresentationType::New();
     m_SagittalMeshResliceRepresentationVector.push_back(sagittalContour);
     m_SagittalMeshResliceRepresentationVector[m_NumberOfLoadedMeshes]->RequestSetMeshObject( 
          m_TargetMeshObjectVector[m_NumberOfLoadedMeshes] );
     m_SagittalMeshResliceRepresentationVector[m_NumberOfLoadedMeshes]->
        RequestSetReslicePlaneSpatialObject( m_SagittalPlaneSpatialObject );

     MeshResliceRepresentationType::Pointer coronalContour = MeshResliceRepresentationType::New();
     m_CoronalMeshResliceRepresentationVector.push_back(coronalContour);
     m_CoronalMeshResliceRepresentationVector[m_NumberOfLoadedMeshes]->RequestSetMeshObject( 
     m_TargetMeshObjectVector[m_NumberOfLoadedMeshes] );
     m_CoronalMeshResliceRepresentationVector[m_NumberOfLoadedMeshes]->
        RequestSetReslicePlaneSpatialObject( m_CoronalPlaneSpatialObject );

     m_MeshRepresentationVector[m_NumberOfLoadedMeshes]->SetOpacity(0.7);      

     m_AxialMeshResliceRepresentationVector[m_NumberOfLoadedMeshes]->SetOpacity(1.0);  
     m_SagittalMeshResliceRepresentationVector[m_NumberOfLoadedMeshes]->SetOpacity(1.0);  
     m_CoronalMeshResliceRepresentationVector[m_NumberOfLoadedMeshes]->SetOpacity(1.0);

     m_AxialMeshResliceRepresentationVector[m_NumberOfLoadedMeshes]->SetLineWidth(3.0);
     m_SagittalMeshResliceRepresentationVector[m_NumberOfLoadedMeshes]->SetLineWidth(3.0);
     m_CoronalMeshResliceRepresentationVector[m_NumberOfLoadedMeshes]->SetLineWidth(3.0);

     double r = ( ( ( double ) ( std::rand( ) ) ) / ( ( double ) ( RAND_MAX ) ) );
     double g = ( ( ( double ) ( std::rand( ) ) ) / ( ( double ) ( RAND_MAX ) ) );
     double b = ( ( ( double ) ( std::rand( ) ) ) / ( ( double ) ( RAND_MAX ) ) );

     m_MeshRepresentationVector[m_NumberOfLoadedMeshes]->SetColor(r, g, b);
     m_AxialMeshResliceRepresentationVector[m_NumberOfLoadedMeshes]->SetColor(r, g, b);
     m_SagittalMeshResliceRepresentationVector[m_NumberOfLoadedMeshes]->SetColor(r, g, b);
     m_CoronalMeshResliceRepresentationVector[m_NumberOfLoadedMeshes]->SetColor(r, g, b);

     m_ViewerGroup->m_3DView->RequestAddObject( m_AxialMeshResliceRepresentationVector[m_NumberOfLoadedMeshes] );
     m_ViewerGroup->m_3DView->RequestAddObject( m_SagittalMeshResliceRepresentationVector[m_NumberOfLoadedMeshes] );
     m_ViewerGroup->m_3DView->RequestAddObject( m_CoronalMeshResliceRepresentationVector[m_NumberOfLoadedMeshes] );     

     m_ViewerGroup->m_3DView->RequestAddObject( m_MeshRepresentationVector[m_NumberOfLoadedMeshes] );
     m_ViewerGroup->m_3DView->RequestResetCamera();

     m_NumberOfLoadedMeshes ++;

     m_StateMachine.PushInput( m_SuccessInput );
     m_StateMachine.ProcessInputs();
     return;
  }
  else
  {
     igstkLogMacro2( m_Logger, DEBUG, "No directory is selected\n" )
     m_StateMachine.PushInput( m_FailureInput );
     m_StateMachine.ProcessInputs();
     return;
  }
}

/** -----------------------------------------------------------------
* Reslices the views to the picked position
*---------------------------------------------------------------------
*/
void Navigator::SetImagePickingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                  "Navigator::SetImagePickingProcessing called...\n" )

  ImageSpatialObjectType::PointType point = TransformToPoint( m_TransformToBeChanged );

  if ( m_ImageSpatialObject->IsInside( point ) )
  {
    ImageSpatialObjectType::IndexType index;
    m_ImageSpatialObject->TransformPhysicalPointToIndex( point, index);
    m_AxialPlaneSpatialObject->RequestSetMousePosition( point );
    m_SagittalPlaneSpatialObject->RequestSetMousePosition( point );
    m_CoronalPlaneSpatialObject->RequestSetMousePosition( point );
    m_CrossHair->RequestSetMousePosition( point );
    this->ResliceImage( index );
  }
  else
  {
    igstkLogMacro( DEBUG,  "Picked point outside image...\n" )
  }
}

/** -----------------------------------------------------------------
* Sets fiducials on the image and reslices the the views to that position
*---------------------------------------------------------------------
*/
void Navigator::SetImageFiducialProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::SetImageFiducialProcessing called...\n" )

    ImageSpatialObjectType::PointType point = TransformToPoint( m_TransformToBeChanged );
  
    if( m_ImageSpatialObject->IsInside( point ) )
    {
      int choice = m_FiducialsPointList->value();

      m_FiducialPointVector[choice]->RequestSetTransformAndParent(m_TransformToBeChanged, m_WorldReference );

      m_Plan->m_FiducialPoints[choice] = point;

      char buf[50];
      sprintf( buf, "[%.2f, %.2f, %.2f]", point[0], point[1], point[2]);
      m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 2, buf );
      m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor(2, 0.0, 0.0, 1.0);

      m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 2, buf );
      m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor(2, 0.0, 0.0, 1.0);

      m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 2, buf );
      m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor(2, 0.0, 0.0, 1.0);     

      m_ViewerGroup->RequestUpdateOverlays();

      /** Write the updated plan to file */
      this->WriteFiducials();

      ImageSpatialObjectType::IndexType index;
      m_ImageSpatialObject->TransformPhysicalPointToIndex( point, index);

      m_AxialPlaneSpatialObject->RequestSetMousePosition( point );
      m_SagittalPlaneSpatialObject->RequestSetMousePosition( point );
      m_CoronalPlaneSpatialObject->RequestSetMousePosition( point );

      m_CrossHair->RequestSetMousePosition( point );

      this->ResliceImage( index );
    }
    else
    {
      igstkLogMacro( DEBUG,  "Picked point outside image...\n" )
    }
}

void Navigator::InitializeTrackerProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::InitializeTrackerProcessing called...\n" )

/*
  std::ofstream ofile;
  ofile.open("TrackerControllerStateMachineDiagram.dot");
  this->m_TrackerController->ExportStateMachineDescription( ofile, true );
*/

  if (!m_TrackerConfiguration)
  {
    std::string errorMessage;
    fl_alert( errorMessage.c_str() );
    fl_beep( FL_BEEP_ERROR );
    igstkLogMacro2( m_Logger, DEBUG, "Tracker Initialization error\n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
  }

  m_TrackerController->RequestInitialize( m_TrackerConfiguration );
               
  //check that initialization was successful
  if( m_TrackerControllerObserver->Error() )
  {
    std::string errorMessage;
    m_TrackerControllerObserver->GetErrorMessage( errorMessage ); 
    m_TrackerControllerObserver->ClearError();
    fl_alert( errorMessage.c_str() );
    fl_beep( FL_BEEP_ERROR );
    igstkLogMacro2( m_Logger, DEBUG, "Tracker Initialization error\n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
  }

//  m_TrackerController->RequestGetTracker();
  m_TrackerController->RequestGetNonReferenceToolList();
  m_TrackerController->RequestGetReferenceTool();
      
  igstk::Transform identity;
  identity.SetToIdentity(igstk::TimeStamp::GetLongestPossibleTime());
 
  // seba: recien comentado
  m_PointerSpatialObject->RequestDetachFromParent();
  m_PointerSpatialObject->RequestSetTransformAndParent( identity, m_TrackerTool);  

  /** Connect the scene graph with an identity transform first */
  if ( m_ReferenceTool.IsNotNull() )
  {
    m_ReferenceTool->RequestSetTransformAndParent(identity, m_WorldReference);
  }
  /*else
  {
    m_Tracker->RequestSetTransformAndParent(identity, m_WorldReference);
  }  */

  if (!m_TrackerConfiguration)
  {
    std::string errorMessage;
    fl_alert( errorMessage.c_str() );
    fl_beep( FL_BEEP_ERROR );
    igstkLogMacro2( m_Logger, DEBUG,
      "Tracker Starting error\n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
  }

  m_TrackerController->RequestStartTracking();
  
  //check that start was successful
  if( m_TrackerControllerObserver->Error() )
  {
    std::string errorMessage;
    m_TrackerControllerObserver->GetErrorMessage( errorMessage ); 
    m_TrackerControllerObserver->ClearError();
    fl_alert( errorMessage.c_str() );
    fl_beep( FL_BEEP_ERROR );
    igstkLogMacro2( m_Logger, DEBUG, 
      "Tracker start error\n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
  }

  // notify the GUI that the system is tracking
  m_TrackingSemaphore->color(FL_YELLOW);

//  m_TrackerTool->AddObserver(
//      igstk::TrackerToolTransformUpdateEvent(), m_TrackerToolUpdateObserver);

  m_TrackerTool->AddObserver(
      igstk::TrackerToolNotAvailableToBeTrackedEvent(), m_TrackerToolNotAvailableObserver);
 
  m_TrackerTool->AddObserver(
      igstk::TrackerToolMadeTransitionToTrackedStateEvent(), m_TrackerToolAvailableObserver);

  m_ReferenceTool->AddObserver(
      igstk::TrackerToolNotAvailableToBeTrackedEvent(), m_ReferenceNotAvailableObserver);
 
  m_ReferenceTool->AddObserver(
      igstk::TrackerToolMadeTransitionToTrackedStateEvent(), m_ReferenceAvailableObserver);
  
  m_StateMachine.PushInput( m_SuccessInput );
  m_StateMachine.ProcessInputs();
  return;
}

void Navigator::StartSetTrackerFiducialsProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::StartSetTrackerFiducialsProcessing called...\n" )

  m_ModifyTrackerFiducialsBtn->label("Registering...");
  m_ModifyTrackerFiducialsBtn->deactivate();

  m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 1, "REGISTERING" );
  m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor(1, 1.0, 0.0, 0.0);

  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 1, "REGISTERING" );
  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor(1, 1.0, 0.0, 0.0);

  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 1, "REGISTERING" );
  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor(1, 1.0, 0.0, 0.0);

  m_ViewerGroup->RequestUpdateOverlays();

  m_ModifyTrackerFiducialsBtn->color(FL_RED); 

  // first reset the reference tool
  igstk::Transform identity;
  identity.SetToIdentity(igstk::TimeStamp::GetLongestPossibleTime());
 
  if ( m_ReferenceTool.IsNotNull() )
  {
    m_ReferenceTool->RequestSetTransformAndParent(identity, m_WorldReference);
  }

  m_ResliceEnabled = false;

  m_FiducialsPointList->clear();
  m_LandmarksContainer.clear();
  m_AcceptedLandmarksContainer.clear();

  char buf[50];
  for ( int i=0; i<m_Plan->m_FiducialPoints.size(); i++)
  {
    sprintf( buf, "Fiducial #%d", i+1 );
    m_FiducialsPointList->add(buf);
    RegistrationType::LandmarkTrackerPointType p;
    m_LandmarksContainer.push_back(p);
    m_AcceptedLandmarksContainer.push_back(false);
  }

  m_FiducialsPointList->value(0);
  this->RequestChangeSelectedFiducial();

  Fl::check();
}

/*
void Navigator::PinpointTrackerFiducialProcessing()
{
  
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::PinpointingTrackerFiducialProcessing called...\n" )

  if (m_CollectingCandidates)
    return;

  unsigned int validTrials = m_CandidateLandmarks.size();

  igstkLogMacro2( m_Logger, DEBUG,
      "Navigator::PinpointingTrackerFiducialProcessing valid trials: " << validTrials << "\n" )

  if (!validTrials)
  {
    m_StateMachine.PushInput( m_FailureInput ); 
    m_StateMachine.ProcessInputs();
    return;
  }

  PointType pt;
  pt.Fill(0.0);

  for (int i=0; i<validTrials; i++)
  {
    pt[0] += m_CandidateLandmarks[i][0]/validTrials;
    pt[1] += m_CandidateLandmarks[i][1]/validTrials;
    pt[2] += m_CandidateLandmarks[i][2]/validTrials;
  }

  int n = m_FiducialsPointList->value();
  int m = m_FiducialsPointList->size();

  m_LandmarksContainer[n] = pt;
  m_AcceptedLandmarksContainer[n] = true;

  //todo: change color to fiducials to indicate that they were accepted
  if ( n < m )
  {
    m_FiducialsPointList->value(n+1);
    RequestChangeSelectedFiducial();       
  }

  if ( n == m-2)
  {
    m_ModifyTrackerFiducialsBtn->label("Ready");
    m_ModifyTrackerFiducialsBtn->color(FL_GREEN);
  }

  m_CandidateLandmarks.clear();

  m_StateMachine.PushInput( m_SuccessInput );
  m_StateMachine.ProcessInputs();
}
*/

/** -----------------------------------------------------------------
* Sets a new point to the registration procedure
*---------------------------------------------------------------------
*/
void Navigator::SetTrackerFiducialProcessing()
{ 
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::SetTrackerFiducialProcessing called...\n" )

  
}

void Navigator::EndSetTrackerFiducialsProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::EndSetTrackerFiducialsProcessing called...\n" )

  AcceptedLandmarkPointContainerType::const_iterator iter;
  iter = m_AcceptedLandmarksContainer.begin();

  unsigned int numberOfAcceptedLandmarks = 0;
  for (;iter != m_AcceptedLandmarksContainer.end(); iter++)
  {
    if (*iter)
      numberOfAcceptedLandmarks++;
  }

  igstkLogMacro2( m_Logger, DEBUG, 
                    "numberOfAcceptedLandmarks " << numberOfAcceptedLandmarks << "\n" )

  if (numberOfAcceptedLandmarks > 3)
  {
    m_ModifyTrackerFiducialsBtn->label("Register");
    m_ModifyTrackerFiducialsBtn->activate();
    m_ModifyTrackerFiducialsBtn->color((Fl_Color)55);
    m_StateMachine.PushInput( m_SuccessInput );
  }
  else
  {
    m_StateMachine.PushInput( m_FailureInput );
  }  
  
  m_StateMachine.ProcessInputs();
}

/** -----------------------------------------------------------------
* Registers the tracker to the working image
*---------------------------------------------------------------------
*/
void Navigator::TrackerRegistrationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::TrackerRegistrationProcessing called...\n" )
  
  RegistrationType::Pointer registration  = RegistrationType::New();
  registration->RequestResetRegistration();

  for( int i=0; i< m_LandmarksContainer.size(); i++)
  {
    // use only those fiducial points that were accepted by the user
    if ( m_AcceptedLandmarksContainer[i] ) 
    {
       registration->RequestAddImageLandmarkPoint( 
                        m_Plan->m_FiducialPoints[i] );
       registration->RequestAddTrackerLandmarkPoint( 
                        m_LandmarksContainer[i] );
    }
  }

  registration->RequestComputeTransform();

  igstk::TransformObserver::Pointer lrtcb = igstk::TransformObserver::New();
  lrtcb->ObserveTransformEventsFrom( registration );
  lrtcb->Clear();

  registration->RequestGetTransformFromTrackerToImage();

  if( lrtcb->GotTransform() )
  {
    RegistrationErrorObserver::Pointer lRmscb =  
                                            RegistrationErrorObserver::New();

    registration->AddObserver( igstk::DoubleTypeEvent(), lRmscb );
    registration->RequestGetRMSError();
    if( lRmscb->GotRegistrationError() )
    {
       igstkLogMacro2( m_Logger, DEBUG,  
         "Tracker to Image Registration Error " << lRmscb->GetRegistrationError() << "\n" )
       m_TrackerRMS = lRmscb->GetRegistrationError();

       char buf[50];
       sprintf( buf, "TRACKING (%.2f)", m_TrackerRMS);
       //sprintf( buf, "TRACKING");

       m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 1, buf );
       m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor(1, 0.0, 1.0, 0.0);

       m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 1, buf );
       m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor(1, 0.0, 1.0, 0.0);

       m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 1, buf );
       m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor(1, 0.0, 1.0, 0.0); 

       m_ViewerGroup->RequestUpdateOverlays();

       // todo: check what happends if we don't accept the registration
       if (m_TrackerRMS > MAX_RMS)
       {
         igstkLogMacro2( m_Logger, DEBUG, 
           "RMS error too big to operate\n" )
         m_StateMachine.PushInput( m_FailureInput );
         m_StateMachine.ProcessInputs();
         return;
       }
    }
    else
    {
     igstkLogMacro2( m_Logger, DEBUG, 
       "Tracker Registration Error: could not retrieve RMS error \n" )
     m_StateMachine.PushInput( m_FailureInput );
     m_StateMachine.ProcessInputs();
     return;
    }

    // set new transformation

    m_RegistrationTransform = lrtcb->GetTransform();
    
    if (m_ReferenceTool.IsNotNull())
    {
      m_ReferenceTool->RequestDetachFromParent();
      m_ReferenceTool->RequestSetTransformAndParent(m_RegistrationTransform, m_WorldReference);
    }

    m_ViewerGroup->m_3DView->RequestResetCamera();

    m_StateMachine.PushInput( m_SuccessInput );
    m_StateMachine.ProcessInputs();
    return;
  }
  else
  {
    igstkLogMacro2( m_Logger, DEBUG, "Tracker Registration Error \n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
  }
}
/** -----------------------------------------------------------------
* Starts tracking provided it is initialized and connected to the 
* communication port
*---------------------------------------------------------------------
*/
void Navigator::StartTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::StartTrackingProcessing called...\n" )

  // if (m_Tracking)
  // {
  //m_StateMachine.PushInput( m_SuccessInput );
  //m_StateMachine.ProcessInputs();
  //   return;
  // }

  //if (!m_TrackerConfiguration)
  //{
  //  std::string errorMessage;
  //  fl_alert( errorMessage.c_str() );
  //  fl_beep( FL_BEEP_ERROR );
  //  igstkLogMacro2( m_Logger, DEBUG,
  //    "Tracker Starting error\n" )
  //  m_StateMachine.PushInput( m_FailureInput );
  //  m_StateMachine.ProcessInputs();
  //  return;
  //}

  //m_TrackerController->RequestStart();
  //
  ////check that start was successful
  //if( m_TrackerControllerObserver->Error() )
  //{
  //  std::string errorMessage;
  //  m_TrackerControllerObserver->GetErrorMessage( errorMessage ); 
  //  m_TrackerControllerObserver->ClearError();
  //  fl_alert( errorMessage.c_str() );
  //  fl_beep( FL_BEEP_ERROR );
  //  igstkLogMacro2( m_Logger, DEBUG, 
  //    "Tracker start error\n" )
  //  m_StateMachine.PushInput( m_FailureInput );
  //  m_StateMachine.ProcessInputs();
  //  return;
  //}

  m_StateMachine.PushInput( m_SuccessInput );
  m_StateMachine.ProcessInputs();
  return;
}
/** -----------------------------------------------------------------
* Stops tracking but keeps the tracker connected to the 
* communication port
*---------------------------------------------------------------------
*/
void Navigator::StopTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::StopTrackingProcessing called...\n" )

/*
  //fix me
  m_TrackerController->RequestStop( );
               //check that stop was successful
  if( m_TrackerControllerObserver->Error() )
  {
    std::string errorMessage;
    m_TrackerControllerObserver->GetErrorMessage( errorMessage ); 
    m_TrackerControllerObserver->ClearError();
    fl_alert( errorMessage.c_str() );
    fl_beep( FL_BEEP_ERROR );
    igstkLogMacro2( m_Logger, DEBUG, 
      "Tracker stop error\n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
  }
*/
  m_StateMachine.PushInput( m_SuccessInput );
  m_StateMachine.ProcessInputs();
  return;

}
/** -----------------------------------------------------------------
* Disconnects the tracker and closes the communication port
*---------------------------------------------------------------------
*/
void Navigator::DisconnectTrackerProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::DisconnectTrackerProcessing called...\n" )
  // try to disconnect
  m_TrackerController->RequestShutdown( );
  //check if succeded
  if( m_TrackerControllerObserver->Error() )
  {
    std::string errorMessage;
    m_TrackerControllerObserver->GetErrorMessage( errorMessage ); 
    m_TrackerControllerObserver->ClearError();
    fl_alert( errorMessage.c_str() );
    fl_beep( FL_BEEP_ERROR );
    igstkLogMacro2( m_Logger, DEBUG, 
      "Tracker disconnect error\n" )

    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
  }

  m_StateMachine.PushInput( m_SuccessInput );
  m_StateMachine.ProcessInputs();
  return;
}

/** -----------------------------------------------------------------
* Set ImageSpatialObjects to 
* ImageRepresentations, sets image orientations, adds ImageSpatialObjects 
* to Views and connects the scene graph
*---------------------------------------------------------------------
*/
void Navigator::ConnectImageRepresentation()
{
  // observe picking event on the views
  m_ImagePickerObserver = LoadedObserverType::New();
  m_ImagePickerObserver->SetCallbackFunction( this, &Navigator::ImagePickingCallback );

  // create reslice plane spatial object for axial view
  m_AxialPlaneSpatialObject = ImageReslicePlaneSpatialObjectType::New();
  m_AxialPlaneSpatialObject->RequestSetReslicingMode( ImageReslicePlaneSpatialObjectType::Orthogonal );
  m_AxialPlaneSpatialObject->RequestSetOrientationType( ImageReslicePlaneSpatialObjectType::Axial );
  m_AxialPlaneSpatialObject->RequestSetImageSpatialObject( m_ImageSpatialObject );

  // create reslice plane spatial object for sagittal view
  m_SagittalPlaneSpatialObject = ImageReslicePlaneSpatialObjectType::New();
  m_SagittalPlaneSpatialObject->RequestSetReslicingMode( ImageReslicePlaneSpatialObjectType::Orthogonal );
  m_SagittalPlaneSpatialObject->RequestSetOrientationType( ImageReslicePlaneSpatialObjectType::Sagittal );
  m_SagittalPlaneSpatialObject->RequestSetImageSpatialObject( m_ImageSpatialObject );

  // create reslice plane spatial object for coronal view
  m_CoronalPlaneSpatialObject = ImageReslicePlaneSpatialObjectType::New();
  m_CoronalPlaneSpatialObject->RequestSetReslicingMode( ImageReslicePlaneSpatialObjectType::Orthogonal );
  m_CoronalPlaneSpatialObject->RequestSetOrientationType( ImageReslicePlaneSpatialObjectType::Coronal );
  m_CoronalPlaneSpatialObject->RequestSetImageSpatialObject( m_ImageSpatialObject );

  // create reslice plane representation for axial view
  m_AxialPlaneRepresentation1 = ImageRepresentationType::New();
  m_AxialPlaneRepresentation1->RequestSetImageSpatialObject( m_ImageSpatialObject );
  m_AxialPlaneRepresentation1->RequestSetReslicePlaneSpatialObject( m_AxialPlaneSpatialObject );

  // create reslice plane representation for sagittal view
  m_SagittalPlaneRepresentation1 = ImageRepresentationType::New();
  m_SagittalPlaneRepresentation1->RequestSetImageSpatialObject( m_ImageSpatialObject );
  m_SagittalPlaneRepresentation1->RequestSetReslicePlaneSpatialObject( m_SagittalPlaneSpatialObject );

  // create reslice plane representation for coronal view
  m_CoronalPlaneRepresentation1 = ImageRepresentationType::New();
  m_CoronalPlaneRepresentation1->RequestSetImageSpatialObject( m_ImageSpatialObject );
  m_CoronalPlaneRepresentation1->RequestSetReslicePlaneSpatialObject( m_CoronalPlaneSpatialObject );

  // create reslice plane representation for axial view in the 3D view
  m_AxialPlaneRepresentation2 = ImageRepresentationType::New();
  m_AxialPlaneRepresentation2->RequestSetImageSpatialObject( m_ImageSpatialObject );
  m_AxialPlaneRepresentation2->RequestSetReslicePlaneSpatialObject( m_AxialPlaneSpatialObject );

  // create reslice plane representation for sagittal view in the 3D view
  m_SagittalPlaneRepresentation2 = ImageRepresentationType::New();
  m_SagittalPlaneRepresentation2->RequestSetImageSpatialObject( m_ImageSpatialObject );
  m_SagittalPlaneRepresentation2->RequestSetReslicePlaneSpatialObject( m_SagittalPlaneSpatialObject );

  // create reslice plane representation for coronal view in the 3D view
  m_CoronalPlaneRepresentation2 = ImageRepresentationType::New();
  m_CoronalPlaneRepresentation2->RequestSetImageSpatialObject( m_ImageSpatialObject );
  m_CoronalPlaneRepresentation2->RequestSetReslicePlaneSpatialObject( m_CoronalPlaneSpatialObject );

  // set up camera observers
  m_AxialReslicePlaneCameraModifiedObserver = LoadedObserverType::New();
  m_AxialReslicePlaneCameraModifiedObserver->SetCallbackFunction(
    this, &Navigator::AxialReslicePlaneCameraModifiedCallback );

  m_SagittalReslicePlaneCameraModifiedObserver = LoadedObserverType::New();
  m_SagittalReslicePlaneCameraModifiedObserver->SetCallbackFunction(
    this, &Navigator::SagittalReslicePlaneCameraModifiedCallback );

  m_CoronalReslicePlaneCameraModifiedObserver = LoadedObserverType::New();
  m_CoronalReslicePlaneCameraModifiedObserver->SetCallbackFunction(
    this, &Navigator::CoronalReslicePlaneCameraModifiedCallback );

  m_AxialPlaneRepresentation1->AddObserver( 
    igstk::VTKCameraModifiedEvent(), m_AxialReslicePlaneCameraModifiedObserver );

  m_SagittalPlaneRepresentation1->AddObserver(
    igstk::VTKCameraModifiedEvent(), m_SagittalReslicePlaneCameraModifiedObserver );

  m_CoronalPlaneRepresentation1->AddObserver(
    igstk::VTKCameraModifiedEvent(), m_CoronalReslicePlaneCameraModifiedObserver );
  
   /** 
   *  Request information about the slice bounds. The answer will be
   *  received in the form of an event. This will be used to initialize
   *  the reslicing sliders and set initial slice position
   */

  ImageExtentObserver::Pointer extentObserver = ImageExtentObserver::New();
  
  unsigned int extentObserverID;

  extentObserverID = m_ImageSpatialObject->AddObserver( igstk::ImageExtentEvent(), extentObserver );

  m_ImageSpatialObject->RequestGetImageExtent();

  if( extentObserver->GotImageExtent() )
  {
    const igstk::EventHelperType::ImageExtentType& extent = extentObserver->GetImageExtent();

    const unsigned int zmin = extent.zmin;
    const unsigned int zmax = extent.zmax;
    const unsigned int zslice = static_cast< unsigned int > ( (zmin + zmax) / 2.0 );
    // todo: see if SetSliceNumber is working correctly and if we actually need it
    m_AxialPlaneSpatialObject->RequestSetSliceNumber( zslice );
    m_ViewerGroup->m_Sliders[0]->minimum( zmin );
    m_ViewerGroup->m_Sliders[0]->maximum( zmax );
    m_ViewerGroup->m_Sliders[0]->value( zslice );
    m_ViewerGroup->m_Sliders[0]->activate();

    const unsigned int ymin = extent.ymin;
    const unsigned int ymax = extent.ymax;
    const unsigned int yslice = static_cast< unsigned int > ( (ymin + ymax) / 2.0 );
    // todo: see if SetSliceNumber is working correctly and if we actually need it
    m_SagittalPlaneSpatialObject->RequestSetSliceNumber( yslice );
    m_ViewerGroup->m_Sliders[1]->minimum( ymin );
    m_ViewerGroup->m_Sliders[1]->maximum( ymax );
    m_ViewerGroup->m_Sliders[1]->value( yslice );
    m_ViewerGroup->m_Sliders[1]->activate();

    const unsigned int xmin = extent.xmin;
    const unsigned int xmax = extent.xmax;
    const unsigned int xslice = static_cast< unsigned int > ( (xmin + xmax) / 2.0 );
    // todo: see if SetSliceNumber is working correctly and if we actually need it
    m_CoronalPlaneSpatialObject->RequestSetSliceNumber( xslice );
    m_ViewerGroup->m_Sliders[2]->minimum( xmin );
    m_ViewerGroup->m_Sliders[2]->maximum( xmax );
    m_ViewerGroup->m_Sliders[2]->value( xslice );
    m_ViewerGroup->m_Sliders[2]->activate();
  }

  m_ImageSpatialObject->RemoveObserver( extentObserverID );

  // Set up cross hairs
  m_CrossHair = CrossHairType::New();
  m_CrossHair->RequestSetReferenceSpatialObject( m_ImageSpatialObject );

  // buid the cross hair representation and add the cross hair object
  m_CrossHairRepresentation = CrossHairRepresentationType::New();
  m_CrossHairRepresentation->SetColor(0,1,0);
  m_CrossHairRepresentation->RequestSetCrossHairObject( m_CrossHair );  

  // add the cross hair representation to the different views
  m_ViewerGroup->m_AxialView->RequestAddObject( m_CrossHairRepresentation->Copy() );
  m_ViewerGroup->m_SagittalView->RequestAddObject( m_CrossHairRepresentation->Copy() );
  m_ViewerGroup->m_CoronalView->RequestAddObject( m_CrossHairRepresentation->Copy() );
  m_ViewerGroup->m_3DView->RequestAddObject( m_CrossHairRepresentation->Copy() );

  // set background color to the views
  m_ViewerGroup->m_AxialView->SetRendererBackgroundColor(0,0,0);
  m_ViewerGroup->m_SagittalView->SetRendererBackgroundColor(0,0,0);
  m_ViewerGroup->m_CoronalView->SetRendererBackgroundColor(0,0,0);
  m_ViewerGroup->m_3DView->SetRendererBackgroundColor(1,1,1);
 
  /**
  *  Connect the scene graph
  *  Here we create a virtual world reference system (as the root) and
  *  attached all the objects as its children.
  *  This is for the convenience in the following implementation. You can
  *  use any spatial object, view, tracker, or tracker tool as a 
  *  reference system in IGSTK. And you can create your own class to
  *  use the coordinate system API by using this macro:
  *     igstkCoordinateSystemClassInterfaceMacro()
  *  Refer to:
  *      igstkCoordinateSystemInterfaceMacros.h
  *  Class:
  *      igstkCoordinateSystem
  *      igstkCoordinateSystemDelegator
  */
  igstk::Transform identity;
  identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  m_ViewerGroup->m_AxialView->RequestSetTransformAndParent(
      identity, m_WorldReference );

  m_ViewerGroup->m_SagittalView->RequestSetTransformAndParent(
      identity, m_WorldReference );

  m_ViewerGroup->m_CoronalView->RequestSetTransformAndParent(
      identity, m_WorldReference );

  m_ViewerGroup->m_3DView->RequestSetTransformAndParent(
      identity, m_ViewerGroup->m_AxialView );


  // set transform and parent to the image spatial object
  m_ImageSpatialObject->RequestSetTransformAndParent( identity, m_WorldReference );

  // set transform and parent to the image plane reslice spatial objects
  m_AxialPlaneSpatialObject->RequestSetTransformAndParent( identity, m_WorldReference );
  m_SagittalPlaneSpatialObject->RequestSetTransformAndParent( identity, m_WorldReference );
  m_CoronalPlaneSpatialObject->RequestSetTransformAndParent( identity, m_WorldReference );

  // set transform and parent to the cross hair object
  m_CrossHair->RequestSetTransformAndParent( identity, m_WorldReference );

  // set transform and parent to the fiducial points
  m_FiducialPointVector[0]->RequestSetTransformAndParent( identity, m_WorldReference );
  m_FiducialPointVector[1]->RequestSetTransformAndParent( identity, m_WorldReference );
  m_FiducialPointVector[2]->RequestSetTransformAndParent( identity, m_WorldReference );
  m_FiducialPointVector[3]->RequestSetTransformAndParent( identity, m_WorldReference );  

  // add reslice plane representations to the orthogonal views
  m_ViewerGroup->m_AxialView->RequestAddObject( m_AxialPlaneRepresentation1 );
  m_ViewerGroup->m_SagittalView->RequestAddObject( m_SagittalPlaneRepresentation1 );
  m_ViewerGroup->m_CoronalView->RequestAddObject( m_CoronalPlaneRepresentation1 );

  // add reslice plane representations to the 3D views
  m_ViewerGroup->m_3DView->RequestAddObject( m_AxialPlaneRepresentation2 );
  m_ViewerGroup->m_3DView->RequestAddObject( m_SagittalPlaneRepresentation2 );
  m_ViewerGroup->m_3DView->RequestAddObject( m_CoronalPlaneRepresentation2 );    
  m_ViewerGroup->m_3DView->RequestResetCamera();

  // set up view parameters
  m_ViewerGroup->m_AxialView->SetRefreshRate( VIEW_2D_REFRESH_RATE );
  m_ViewerGroup->m_AxialView->RequestStart();
  m_ViewerGroup->m_AxialWidget->RequestEnableInteractions();  

  m_ViewerGroup->m_SagittalView->SetRefreshRate( VIEW_2D_REFRESH_RATE );
  m_ViewerGroup->m_SagittalView->RequestStart();
  m_ViewerGroup->m_SagittalWidget->RequestEnableInteractions();

  m_ViewerGroup->m_CoronalView->SetRefreshRate( VIEW_2D_REFRESH_RATE );
  m_ViewerGroup->m_CoronalView->RequestStart();
  m_ViewerGroup->m_CoronalWidget->RequestEnableInteractions();

  m_ViewerGroup->m_3DView->SetRefreshRate( VIEW_3D_REFRESH_RATE );
  m_ViewerGroup->m_3DView->RequestStart();

  // reset the cameras in the different views
  m_ViewerGroup->m_AxialView->RequestResetCamera();
  m_ViewerGroup->m_SagittalView->RequestResetCamera();
  m_ViewerGroup->m_CoronalView->RequestResetCamera();
  m_ViewerGroup->m_3DView->RequestResetCamera();

  /** Adding observers for picking events in the 2D views */
  m_ViewerGroup->m_AxialView->AddObserver(
      igstk::CoordinateSystemTransformToEvent(), m_ImagePickerObserver );

  m_ViewerGroup->m_SagittalView->AddObserver(
      igstk::CoordinateSystemTransformToEvent(), m_ImagePickerObserver );

  m_ViewerGroup->m_CoronalView->AddObserver(
      igstk::CoordinateSystemTransformToEvent(), m_ImagePickerObserver );

  /** Adding observer for slider bar reslicing event */
  m_ManualReslicingObserverID = m_ViewerGroup->AddObserver( igstk::NavigatorQuadrantViews::ManualReslicingEvent(),
    m_ManualReslicingObserver );

  /** Adding observer for key pressed event */
  m_ViewerGroup->AddObserver( igstk::NavigatorQuadrantViews::KeyPressedEvent(),
    m_KeyPressedObserver );

  /** Adding observer for mouse pressed event */
  m_ViewerGroup->AddObserver( igstk::NavigatorQuadrantViews::MousePressedEvent(),
    m_MousePressedObserver );  

}

/** -----------------------------------------------------------------
*  Read fiducial points from file
*---------------------------------------------------------------------
*/
void Navigator::ReadFiducials()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::ReadFiducials called...\n" )

  igstk::FiducialsPlanIO * reader = new igstk::FiducialsPlanIO;

  m_PlanFilename = m_ImageDir + "/Fiducials.igstk";

  igstkLogMacro2( m_Logger, DEBUG, 
                    "Reading fiducials from " << m_PlanFilename << "\n")

  m_Plan = new igstk::FiducialsPlan;

  if (itksys::SystemTools::FileExists( m_PlanFilename.c_str()))
  {
    reader->SetFileName( m_PlanFilename );
    if ( reader->RequestRead( ) )
    {
      m_Plan = reader->GetFiducialsPlan();
    }
  }

  /** Populate the choice box */
  m_FiducialsPointList->clear();

  m_LandmarksContainer.clear();
  m_AcceptedLandmarksContainer.clear();
  
  char buf[50];
  for( int i = 0; i < m_Plan->m_FiducialPoints.size(); i++ )
  {
    sprintf( buf, "Fiducial #%d", i+1 );
    m_FiducialsPointList->add( buf );
    RegistrationType::LandmarkTrackerPointType p;
    m_LandmarksContainer.push_back(p);
    m_AcceptedLandmarksContainer.push_back(false);
  }

  m_FiducialsPointList->value(0);
  this->RequestChangeSelectedFiducial();
}

/** -----------------------------------------------------------------
*  Overwrite the file with the image fiducials
*---------------------------------------------------------------------
*/
void Navigator::WriteFiducials()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::WriteFiducials called...\n" )

  igstk::FiducialsPlanIO * writer = new igstk::FiducialsPlanIO;
  writer->SetFileName( m_PlanFilename );
  writer->SetFiducialsPlan( m_Plan );
  writer->RequestWrite();
}

/** -----------------------------------------------------------------
*  Switches the currently active view mode
*---------------------------------------------------------------------
*/
void Navigator::RequestChangeSelectedViewMode()
{
  
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::RequestChangeSelectedViewMode called...\n" )

  if ( m_ViewModeList->size() == 0)
  {
    return;
  }
    
  // check which view mode is selected  
  int choice = m_ViewModeList->value();
  
  switch (choice)
  {
    case 0:
      m_AxialPlaneSpatialObject->RequestSetReslicingMode( ImageReslicePlaneSpatialObjectType::Orthogonal );
      m_AxialPlaneSpatialObject->RequestSetOrientationType( ImageReslicePlaneSpatialObjectType::Axial );
      m_SagittalPlaneSpatialObject->RequestSetReslicingMode( ImageReslicePlaneSpatialObjectType::Orthogonal );
      m_SagittalPlaneSpatialObject->RequestSetOrientationType( ImageReslicePlaneSpatialObjectType::Sagittal );
      m_CoronalPlaneSpatialObject->RequestSetReslicingMode( ImageReslicePlaneSpatialObjectType::Orthogonal );
      m_CoronalPlaneSpatialObject->RequestSetOrientationType( ImageReslicePlaneSpatialObjectType::Coronal );
      break;

    case 1:
      m_AxialPlaneSpatialObject->RequestSetReslicingMode( ImageReslicePlaneSpatialObjectType::Orthogonal );
      m_AxialPlaneSpatialObject->RequestSetOrientationType( ImageReslicePlaneSpatialObjectType::Axial );
      m_SagittalPlaneSpatialObject->RequestSetReslicingMode( ImageReslicePlaneSpatialObjectType::OffOrthogonal );
      m_SagittalPlaneSpatialObject->RequestSetOrientationType( ImageReslicePlaneSpatialObjectType::OffAxial );
      m_CoronalPlaneSpatialObject->RequestSetReslicingMode( ImageReslicePlaneSpatialObjectType::OffOrthogonal );
      m_CoronalPlaneSpatialObject->RequestSetOrientationType( ImageReslicePlaneSpatialObjectType::OffSagittal );
      break;

    case 2:
      m_AxialPlaneSpatialObject->RequestSetReslicingMode( ImageReslicePlaneSpatialObjectType::Oblique );
      m_AxialPlaneSpatialObject->RequestSetOrientationType( ImageReslicePlaneSpatialObjectType::PlaneOrientationWithXAxesNormal );
      m_SagittalPlaneSpatialObject->RequestSetReslicingMode( ImageReslicePlaneSpatialObjectType::Oblique );
      m_SagittalPlaneSpatialObject->RequestSetOrientationType( ImageReslicePlaneSpatialObjectType::PlaneOrientationWithYAxesNormal );
      m_CoronalPlaneSpatialObject->RequestSetReslicingMode( ImageReslicePlaneSpatialObjectType::Oblique );
      m_CoronalPlaneSpatialObject->RequestSetOrientationType( ImageReslicePlaneSpatialObjectType::PlaneOrientationWithZAxesNormal );
      break;

  default:
    igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::RequestChangeSelectedViewMode invalid choice \n" )
    return;
  }

  m_AxialViewInitialized = false;
  m_SagittalViewInitialized = false;
  m_CoronalViewInitialized = false;

  m_ViewerGroup->redraw();
  Fl::check();

}

/** -----------------------------------------------------------------
*  Switches the currently active image fiducial
*---------------------------------------------------------------------
*/
void Navigator::RequestChangeSelectedFiducial()
{
  
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::RequestChangeSelectedFiducial called...\n" )

  if ( m_FiducialsPointList->size() == 0)
  {
    return;
  }
    
  // check which point is selected  
  int choice = m_FiducialsPointList->value();

  // get fiducial coordinates
  ImageSpatialObjectType::PointType point;
  point = m_Plan->m_FiducialPoints[ choice ];

  m_FiducialPointVector[choice]->RequestSetTransformAndParent(
      PointToTransform(point), m_WorldReference );

  for (int i=0; i<4; i++)
  {
    m_ViewerGroup->m_AxialView->RequestRemoveObject( m_AxialFiducialRepresentationVector[i] );
    m_ViewerGroup->m_SagittalView->RequestRemoveObject( m_SagittalFiducialRepresentationVector[i] );
    m_ViewerGroup->m_CoronalView->RequestRemoveObject( m_CoronalFiducialRepresentationVector[i] );
    m_ViewerGroup->m_3DView->RequestRemoveObject( m_3DViewFiducialRepresentationVector[i] );
  }

  m_ViewerGroup->m_AxialView->RequestAddObject( m_AxialFiducialRepresentationVector[choice] );
  m_ViewerGroup->m_SagittalView->RequestAddObject( m_SagittalFiducialRepresentationVector[choice] );
  m_ViewerGroup->m_CoronalView->RequestAddObject( m_CoronalFiducialRepresentationVector[choice] );
  m_ViewerGroup->m_3DView->RequestAddObject( m_3DViewFiducialRepresentationVector[choice] );

  char buf[50];

  sprintf( buf, "[%.2f, %.2f, %.2f]", point[0], point[1], point[2]);
 
  /** Display point position as annotation */    
  m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 2, buf );
  m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor(2, 0.0, 0.0, 1.0);

  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 2, buf );
  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor(2, 0.0, 0.0, 1.0);

  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 2, buf );
  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor(2, 0.0, 0.0, 1.0); 

  m_ViewerGroup->RequestUpdateOverlays();

  /** Reslice image to the selected point position */
  if( m_ImageSpatialObject->IsInside( point ) )
  {
    ImageSpatialObjectType::IndexType index;
    m_ImageSpatialObject->TransformPhysicalPointToIndex( point, index);
    m_AxialPlaneSpatialObject->RequestSetMousePosition( point );
    m_SagittalPlaneSpatialObject->RequestSetMousePosition( point );
    m_CoronalPlaneSpatialObject->RequestSetMousePosition( point );
    m_CrossHair->RequestSetMousePosition( point );
    this->ResliceImage( index );
  }
  else
  {
    igstkLogMacro2( m_Logger, DEBUG, 
      "This point is not defined in the image...\n" )
  }

  m_ViewerGroup->redraw();
  Fl::check();

}


/** -----------------------------------------------------------------
*  Callback function for observer listening to the slider bar
*  ReslicingEvent
*---------------------------------------------------------------------
*/
void Navigator::ResliceImageCallback( const itk::EventObject & event )
{

  if ( igstk::NavigatorQuadrantViews::ManualReslicingEvent().CheckEvent( &event ) )
  {
    igstk::NavigatorQuadrantViews::ManualReslicingEvent *resliceEvent =
    ( igstk::NavigatorQuadrantViews::ManualReslicingEvent *) & event;
    ImageSpatialObjectType::IndexType index = resliceEvent->Get();

    PointType point;
    m_ImageSpatialObject->TransformIndexToPhysicalPoint( index, point);
    m_AxialPlaneSpatialObject->RequestSetMousePosition( point );
    m_SagittalPlaneSpatialObject->RequestSetMousePosition( point );
    m_CoronalPlaneSpatialObject->RequestSetMousePosition( point );
    m_CrossHair->RequestSetMousePosition( point );
  }
}

/** -----------------------------------------------------------------
* Callback function for observer listening to a key-pressed event
*---------------------------------------------------------------------
*/
void Navigator::HandleKeyPressedCallback( const itk::EventObject & event )
{
  if ( igstk::NavigatorQuadrantViews::KeyPressedEvent().CheckEvent( &event ) )
  {
    igstk::NavigatorQuadrantViews::KeyPressedEvent *keyPressedEvent =
      ( igstk::NavigatorQuadrantViews::KeyPressedEvent *) & event;
    this->HandleKeyPressed( keyPressedEvent->Get() );
  }
}

/** -----------------------------------------------------------------
* Callback function for observer listening to a mouse-pressed event
*---------------------------------------------------------------------
*/
void Navigator::HandleMousePressedCallback( const itk::EventObject & event )
{
  if ( igstk::NavigatorQuadrantViews::MousePressedEvent().CheckEvent( &event ) )
  {
    igstk::NavigatorQuadrantViews::MousePressedEvent *mousePressedEvent =
      ( igstk::NavigatorQuadrantViews::MousePressedEvent *) & event;
    this->HandleMousePressed( mousePressedEvent->Get() );
  }
}

/** -----------------------------------------------------------------
*  Callback function for picking event.
*  Upon receiving a valid picking event, this method will reslice the 
*  image to that location and update the annotation with the new point 
*  position.
*---------------------------------------------------------------------
*/
void Navigator::ImagePickingCallback( const itk::EventObject & event)
{
  if ( igstk::CoordinateSystemTransformToEvent().CheckEvent( &event ) )
  {
    typedef igstk::CoordinateSystemTransformToEvent TransformEventType;
    const TransformEventType * tmevent =
    dynamic_cast< const TransformEventType *>( & event );

    igstk::CoordinateSystemTransformToResult transformCarrier = tmevent->Get();
    m_TransformToBeChanged = transformCarrier.GetTransform();

    m_StateMachine.PushInput( m_SetPickingPositionInput );
    m_StateMachine.ProcessInputs();
  }
}



void Navigator::HandleMousePressed ( 
  igstk::NavigatorQuadrantViews::MouseCommandType mouseCommand )
{ 

    m_WindowWidth += mouseCommand.dx * 2;
    if (m_WindowWidth < 1)
      m_WindowWidth = 1;

    m_WindowLevel += mouseCommand.dy * 2;

    m_AxialPlaneRepresentation1->SetWindowLevel( m_WindowWidth, m_WindowLevel );
    m_SagittalPlaneRepresentation1->SetWindowLevel( m_WindowWidth, m_WindowLevel );
    m_CoronalPlaneRepresentation1->SetWindowLevel( m_WindowWidth, m_WindowLevel );

    m_AxialPlaneRepresentation2->SetWindowLevel( m_WindowWidth, m_WindowLevel );
    m_SagittalPlaneRepresentation2->SetWindowLevel( m_WindowWidth, m_WindowLevel );
    m_CoronalPlaneRepresentation2->SetWindowLevel( m_WindowWidth, m_WindowLevel );

    Fl::check();
}

void Navigator::HandleKeyPressed ( 
  igstk::NavigatorQuadrantViews::KeyboardCommandType keyCommand )
{
  switch ( keyCommand.key ) 
  { 
    case 'p': // toggle orthogonal planes visualization
        this->RequestToggleOrthogonalPlanes();       
        break;

    case 'r': // reset 3D view
        m_ViewerGroup->m_3DView->RequestResetCamera();
        break;

    case 'h': // accepts a fiducial pointed out by the tracker while doing the registration        
          this->RequestAcceptTrackerFiducial();
        break;

    case 'g': // attempts to finish the registration
        this->RequestEndSetTrackerFiducials();
        break;

    default:  
         return;
  }
}

/** -----------------------------------------------------------------
*  Method for reslicing the image given an index number  
*---------------------------------------------------------------------
*/
void Navigator::ResliceImage ( IndexType index )
{   
  m_ViewerGroup->m_SuperiorLeftSlider->value( index[2] );
  m_ViewerGroup->m_SuperiorRightSlider->value( index[0] );
  m_ViewerGroup->m_InferiorLeftSlider->value( index[1] );
  m_ViewerGroup->redraw();
  Fl::check();
}

void Navigator::EnableOrthogonalPlanes()
{
  m_ViewerGroup->m_3DView->RequestAddObject( m_AxialPlaneRepresentation2 );
  m_ViewerGroup->m_3DView->RequestAddObject( m_SagittalPlaneRepresentation2 );
  m_ViewerGroup->m_3DView->RequestAddObject( m_CoronalPlaneRepresentation2 );
}

void Navigator::DisableOrthogonalPlanes()
{
  m_ViewerGroup->m_3DView->RequestRemoveObject( m_AxialPlaneRepresentation2 );
  m_ViewerGroup->m_3DView->RequestRemoveObject( m_SagittalPlaneRepresentation2 );
  m_ViewerGroup->m_3DView->RequestRemoveObject( m_CoronalPlaneRepresentation2 );
}

/** -----------------------------------------------------------------
*  Callback function for ReslicePlaneCameraUpdateEvent
*  This function sets the camera parameters to the Views
*  according to the data in the ReslicePlaneCameraUpdateEvent.
*---------------------------------------------------------------------
*/
void Navigator::AxialReslicePlaneCameraModifiedCallback(const itk::EventObject & event )
{
  if ( igstk::VTKCameraModifiedEvent().CheckEvent( &event ) )
  {
    const igstk::VTKCameraModifiedEvent *cameraModifiedEvent =
      dynamic_cast< const igstk::VTKCameraModifiedEvent * > (&event);

    if (cameraModifiedEvent)
    {
      vtkCamera * camera = cameraModifiedEvent->Get();
      double* viewup = camera->GetViewUp();
      m_ViewerGroup->m_AxialView->SetCameraViewUp(viewup[0],viewup[1],viewup[2]);
      if ( !m_AxialViewInitialized )
      {
        double* focalpoint = camera->GetFocalPoint();
        m_ViewerGroup->m_AxialView->SetCameraFocalPoint( focalpoint[0], focalpoint[1], focalpoint[2] );
        double* position = camera->GetPosition();
        m_ViewerGroup->m_AxialView->SetCameraPosition(position[0], position[1], position[2] );
        m_AxialViewInitialized = true;
      }
    }
  }
}

void Navigator::SagittalReslicePlaneCameraModifiedCallback(const itk::EventObject & event )
{
  if ( igstk::VTKCameraModifiedEvent().CheckEvent( &event ) )
  {
    const igstk::VTKCameraModifiedEvent *cameraModifiedEvent =
      dynamic_cast< const igstk::VTKCameraModifiedEvent * > (&event);

    if (cameraModifiedEvent)
    {
      vtkCamera * camera = cameraModifiedEvent->Get();
      double* viewup = camera->GetViewUp();
      m_ViewerGroup->m_SagittalView->SetCameraViewUp(viewup[0],viewup[1],viewup[2]);
      if ( !m_SagittalViewInitialized )
      {
        double* focalpoint = camera->GetFocalPoint();
        m_ViewerGroup->m_SagittalView->SetCameraFocalPoint( focalpoint[0], focalpoint[1], focalpoint[2] );
        double* position = camera->GetPosition();
        m_ViewerGroup->m_SagittalView->SetCameraPosition(position[0], position[1], position[2] );
        m_SagittalViewInitialized = true;
      }
    }
  }
}

void Navigator::CoronalReslicePlaneCameraModifiedCallback(const itk::EventObject & event )
{
  if ( igstk::VTKCameraModifiedEvent().CheckEvent( &event ) )
  {
    const igstk::VTKCameraModifiedEvent *cameraModifiedEvent =
      dynamic_cast< const igstk::VTKCameraModifiedEvent * > (&event);

    if (cameraModifiedEvent)
    {
      vtkCamera * camera = cameraModifiedEvent->Get();
      double* viewup = camera->GetViewUp();
      m_ViewerGroup->m_CoronalView->SetCameraViewUp(viewup[0],viewup[1],viewup[2]);
      if ( !m_CoronalViewInitialized )
      {
        double* focalpoint = camera->GetFocalPoint();
        m_ViewerGroup->m_CoronalView->SetCameraFocalPoint( focalpoint[0], focalpoint[1], focalpoint[2] );
        double* position = camera->GetPosition();
        m_ViewerGroup->m_CoronalView->SetCameraPosition(position[0], position[1], position[2] );
        m_CoronalViewInitialized = true;
      }
    }
  }
}

void Navigator::ToolAvailableCallback(const itk::EventObject & event )
{
  m_TrackingSemaphore->color(FL_GREEN);
  m_TrackingSemaphore->label("tracking");
  m_ControlGroup->redraw();  
  Fl::check();
}

void Navigator::ToolNotAvailableCallback(const itk::EventObject & event )
{
  m_TrackingSemaphore->color(FL_RED);
  m_TrackingSemaphore->label("not visible");
  m_ControlGroup->redraw();  
  Fl::check();
}

void Navigator::ReferenceAvailableCallback(const itk::EventObject & event )
{
  m_ReferenceSemaphore->color(FL_GREEN);
  m_ReferenceSemaphore->label("tracking");
  m_ControlGroup->redraw();  
  Fl::check();
}

void Navigator::ReferenceNotAvailableCallback(const itk::EventObject & event )
{
  m_ReferenceSemaphore->color(FL_RED);
  m_ReferenceSemaphore->label("not visible");
  m_ControlGroup->redraw();  
  Fl::check();
}

void Navigator::TrackerControllerObserver::SetParent( Navigator *p ) 
{
  m_Parent = p;
}

void Navigator::TrackerControllerObserver::Execute( const itk::Object *caller, 
                                                    const itk::EventObject & event )
{
  const itk::Object * constCaller = caller;
  this->Execute(constCaller, event);
}
  
void 
Navigator::TrackerControllerObserver::Execute( itk::Object *caller, 
                                                             const itk::EventObject & event )
{
  const igstk::TrackerController::InitializeErrorEvent *evt1a =
    dynamic_cast< const igstk::TrackerController::InitializeErrorEvent * > (&event);

  const igstk::TrackerStartTrackingErrorEvent *evt1b =
    dynamic_cast< const igstk::TrackerStartTrackingErrorEvent * > (&event);

    const igstk::TrackerStopTrackingErrorEvent *evt1c =
    dynamic_cast< const igstk::TrackerStopTrackingErrorEvent * > (&event);

 /* const igstk::TrackerController::RequestTrackerEvent *evt2 =
    dynamic_cast< const igstk::TrackerController::RequestTrackerEvent * > (&event);*/

   const igstk::TrackerController::RequestToolsEvent *evt3 = 
    dynamic_cast< const igstk::TrackerController::RequestToolsEvent * > (&event);

  const igstk::TrackerController::RequestToolEvent *evt4 = 
    dynamic_cast< const igstk::TrackerController::RequestToolEvent * > (&event);

  if( evt1a ) 
  {            
    m_ErrorOccured = true;
    m_ErrorMessage = evt1a->Get();
  }
  else if ( evt1b )
  {
    m_ErrorOccured = true;
    m_ErrorMessage = evt1b->Get();
  }
  else if ( evt1c )
  {
    m_ErrorOccured = true;
    m_ErrorMessage = evt1c->Get();
  }
  //else if ( evt2 )
  //{
  //  m_Parent->m_Tracker = evt2->Get();
  //}
  else if ( evt3 )
  {
    igstk::TrackerController::ToolContainerType toolContainer = evt3->Get();
    igstk::TrackerController::ToolContainerType::iterator iter = toolContainer.find("sPtr");

    if ( iter!=toolContainer.end() )
    {      
        m_Parent->m_TrackerTool = (*iter).second;
    }
  }
  else if ( evt4 )
  {
    igstk::TrackerController::ToolEntryType entry = evt4->Get();
      if ( entry.first == "reference" )
        m_Parent->m_ReferenceTool = entry.second;
  }
}

void 
Navigator
::OnITKProgressEvent(itk::Object *source, const itk::EventObject &)
{
  // Get the value of the progress
  float progress = reinterpret_cast<itk::ProcessObject *>(source)->GetProgress();

  // Update the progress bar and value
  m_OutProgressMeter->value(100 * progress);
  m_OutProgressCounter->value(100 * progress);

  // Show or hide progress bar if necessary
  if(progress < 1.0f && !m_WinProgress->visible())
    {
    m_WinProgress->show();
    }
  else if (progress == 1.0f && m_WinProgress->visible())
    {
    m_WinProgress->hide();
    }

  // Update the screen
  Fl::check();
}

void
Navigator
::RequestPrepareToQuit()
{
  this->RequestDisconnectTracker();
}


igstk::Transform *
Navigator::
ReadTransformFile( const std::string &calibrationFileName )
{
  igstk::PrecomputedTransformData::Pointer transformData;
                                  //our reader
  igstk::TransformFileReader::Pointer transformFileReader = 
    igstk::TransformFileReader::New();
                  //set our reader to read a rigid transformation from the 
                  //given file
  igstk::TransformXMLFileReaderBase::Pointer xmlFileReader = 
    igstk::RigidTransformXMLFileReader::New();
  transformFileReader->RequestSetReader( xmlFileReader );
  transformFileReader->RequestSetFileName( calibrationFileName );

            //observer for the read success and failure events
  ReadObserver::Pointer readObserver = ReadObserver::New();
            //observer for the get data event (initiated by the readObserver)
  TransformDataObserver::Pointer getDataObserver = TransformDataObserver::New();
                       
            //add our observers     
  transformFileReader->AddObserver( igstk::TransformFileReader::ReadFailureEvent(),
                                    readObserver );
  transformFileReader->AddObserver( igstk::TransformFileReader::ReadSuccessEvent(),
                                    readObserver );
  transformFileReader->AddObserver( igstk::TransformFileReader::TransformDataEvent(),
                                    getDataObserver );

                //request read. if it fails the readObserver will print the 
                //error, otherwise it will request to get the transform data                 
  transformFileReader->RequestRead();
               //if the read succeeded it invoked a request data, so check if
               //we got the data
  if( getDataObserver->GotTransformData() ) 
  {
    transformData = getDataObserver->GetTransformData();
                           //attach all observers to the transformation data
                           //object
    TransformationDescriptionObserver::Pointer descriptionObserver = 
      TransformationDescriptionObserver::New();
    transformData->AddObserver( igstk::StringEvent(), 
                                descriptionObserver );

    TransformationDateObserver::Pointer dateObserver = 
      TransformationDateObserver::New();
    transformData->AddObserver( igstk::PrecomputedTransformData::TransformDateTypeEvent(), 
                                dateObserver );
      
    TransformRequestObserver ::Pointer transformObserver = 
      TransformRequestObserver ::New();
    transformData->AddObserver( igstk::PrecomputedTransformData::TransformTypeEvent(), 
                                transformObserver );

    TransformErrorObserver::Pointer transformErrorObserver = 
      TransformErrorObserver::New();
    transformData->AddObserver( igstk::PrecomputedTransformData::TransformErrorTypeEvent(), 
                                transformErrorObserver );
                           //request all the info
    transformData->RequestTransformDescription();
    transformData->RequestComputationDateAndTime();
    transformData->RequestEstimationError();     
    if( descriptionObserver->GotTransformationDescription() &&
        dateObserver->GotTransformationDate() &&
        transformErrorObserver->GotTransformError() )
    {
//      char response;
      std::cout << descriptionObserver->GetTransformationDescription();
      std::cout << " ("<<dateObserver->GetTransformationDate() << ").\n";
      std::cout << "Estimation error: "<<transformErrorObserver->GetTransformError() << "\n";
      /*std::cout<<"Use this data [n,y]: ";
      std::cin>>response;*/
            //set the tool's calibration transform
      //if(response == 'y' || response=='Y') 
     // {
        transformData->RequestTransform();
        //if( transformObserver->GotTransform() )
       // {
        igstk::Transform* rigidTransform = dynamic_cast< igstk::Transform* >
          (transformObserver->GetTransformRequest());
        
        if (!rigidTransform)
          return NULL;
         
        igstk::Transform *result = new igstk::Transform();         

        result->SetTranslationAndRotation( rigidTransform->GetTranslation(),
                                             rigidTransform->GetRotation(),
                                             transformErrorObserver->GetTransformError(),
                                             igstk::TimeStamp::GetLongestPossibleTime() );

        return result;
       
    }
    else return NULL;
  }
  else return NULL;
}

