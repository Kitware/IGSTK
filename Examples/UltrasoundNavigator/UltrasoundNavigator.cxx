/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    UltrasoundNavigator.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "UltrasoundNavigator.h"

#include "FL/Fl_File_Chooser.H"
#include "FL/Fl_Input.H"
#include "igstkEvents.h"
#include "itksys/SystemTools.hxx"
#include "itksys/Directory.hxx"
#include "igstkTransformObserver.h"

#include "igstkAuroraTrackerConfiguration.h"
#include "igstkAscensionTrackerConfiguration.h"
#include "igstkMicronTrackerConfiguration.h"
#include "igstkPolarisTrackerConfiguration.h"

#include "igstkAuroraConfigurationXMLFileReader.h"
#include "igstkAscensionConfigurationXMLFileReader.h"
#include "igstkMicronConfigurationXMLFileReader.h"
#include "igstkPolarisVicraConfigurationXMLFileReader.h"
#include "igstkPolarisSpectraConfigurationXMLFileReader.h"
#include "igstkPolarisHybridConfigurationXMLFileReader.h"

#define VIEW_2D_REFRESH_RATE 10
#define VIEW_3D_REFRESH_RATE 10
#define IMAGER_DEFAULT_REFRESH_RATE 15
// name of the tool that is going to drive the reslicing
#define TRACKER_TOOL_NAME "sPtr" //sPtr // bayonet // hybrid_pointer
#define IMAGER_TOOL_NAME "sProbe" //sPtr // bayonet // hybrid_pointer
// name of the tool that is going to be used as dynamic reference
#define REFERENCE_NAME "reference"

/** ---------------------------------------------------------------
*     Constructor
* -----------------------------------------------------------------
*/
UltrasoundNavigator::UltrasoundNavigator() :
  m_TrackerConfiguration( NULL ),
  m_ImagerConfiguration( NULL ),
  m_StateMachine(this)
{
  std::srand( 5 );

  /** member variables initialization */
  m_ImageDir = "";
  m_ImagePlanesIn3DViewEnabled = false;
  m_VideoEnabled = false;
  m_VideoRunning = false;
  m_ImagerInitialized = false;
  m_VideoConnected = false;
  m_ModifyImageFiducialsEnabled = false;
  m_TrackerRMS = 0.0;
  m_WindowWidth = 542;
  m_WindowLevel = 52;

  m_WindowWidthVideo = 286;
  m_WindowLevelVideo = 108;

  /** Setup logger, for all igstk components */
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

  if( m_LogFile.fail() )
  {
    //Return if fail to open the log file
    igstkLogMacro2( m_Logger, DEBUG,
      "Problem opening Log file:" << logFileName << "\n" );
    return;
  }

  m_LogFileOutput->SetStream( m_LogFile );
  this->GetLogger()->AddLogOutput( m_LogFileOutput );
  igstkLogMacro2( m_Logger, DEBUG,
    "Successfully opened Log file:" << logFileName << "\n" );

  /** Machine States */

  igstkAddStateMacro( Initial );
  igstkAddStateMacro( LoadingImage );
  igstkAddStateMacro( ConfirmingImagePatientName );
  igstkAddStateMacro( ImageReady );
  igstkAddStateMacro( LoadingTrackerToolSpatialObject );
  igstkAddStateMacro( LoadingImagerToolSpatialObject );
  igstkAddStateMacro( LoadingMesh );
  igstkAddStateMacro( SettingImageFiducials );
  igstkAddStateMacro( TrackerToolSpatialObjectReady );
  igstkAddStateMacro( ConfiguringTracker );
  igstkAddStateMacro( TrackerConfigurationReady );
  igstkAddStateMacro( InitializingTracker );
  igstkAddStateMacro( TrackerInitializationReady );
  igstkAddStateMacro( SettingTrackerFiducials );
  igstkAddStateMacro( EndingSetTrackerFiducials );
  igstkAddStateMacro( TrackerFiducialsReady );
  igstkAddStateMacro( RegisteringTracker );
  igstkAddStateMacro( AcceptingRegistration );
  igstkAddStateMacro( RegistrationReady );
  igstkAddStateMacro( StartingTracker );
  igstkAddStateMacro( StoppingTracker );
  igstkAddStateMacro( DisconnectingTracker );
  igstkAddStateMacro( Tracking );
  igstkAddStateMacro( TrackingAndImaging );
  igstkAddStateMacro( InitializingImager );
  igstkAddStateMacro( StartingImager );
  igstkAddStateMacro( StoppingImager );
  igstkAddStateMacro( DisconnectingImager );

  /** Machine Inputs*/

  igstkAddInputMacro( Success );
  igstkAddInputMacro( Failure );
  igstkAddInputMacro( LoadImage );
  igstkAddInputMacro( ConfirmImagePatientName );
  igstkAddInputMacro( LoadMesh );
  igstkAddInputMacro( LoadTrackerToolSpatialObject );
  igstkAddInputMacro( LoadImagerToolSpatialObject );
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

  igstkAddInputMacro( InitializeImager );
  igstkAddInputMacro( StartImaging );
  igstkAddInputMacro( StopImaging );
  igstkAddInputMacro( DisconnectImager );

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
  igstkAddTransitionMacro( Initial, LoadMesh,
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, LoadTrackerToolSpatialObject,
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, LoadImagerToolSpatialObject,
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
  igstkAddTransitionMacro( Initial, InitializeImager,
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, StartImaging,
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, StopImaging,
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, DisconnectImager,
                           Initial, ReportInvalidRequest );

  /** LoadingImage State */

  igstkAddTransitionMacro( LoadingImage, Success,
                           ConfirmingImagePatientName, ConfirmPatientName );
  igstkAddTransitionMacro( LoadingImage, Failure,
                           Initial, ReportFailureImageLoaded );

 //complete table for state: LoadingImage State

  igstkAddTransitionMacro( LoadingImage, LoadImage,
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, ConfirmImagePatientName,
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, LoadMesh,
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, LoadTrackerToolSpatialObject,
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, LoadImagerToolSpatialObject,
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
  igstkAddTransitionMacro( LoadingImage, StartImaging,
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, StopImaging,
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, DisconnectImager,
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, InitializeImager,
                           LoadingImage, ReportInvalidRequest );

   /** ConfirmingImagePatientName State */

  igstkAddTransitionMacro( ConfirmingImagePatientName, Success,
                           ImageReady, ReportSuccessImageLoaded );

  igstkAddTransitionMacro( ConfirmingImagePatientName, Failure,
                           Initial, ReportFailureImageLoaded );

  //complete table for state: ConfirmingImagePatientName

  igstkAddTransitionMacro( ConfirmingImagePatientName, LoadImage,
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, ConfirmImagePatientName,
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, LoadMesh,
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, LoadTrackerToolSpatialObject,
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, LoadImagerToolSpatialObject,
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
  igstkAddTransitionMacro( ConfirmingImagePatientName, StartImaging,
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, StopImaging,
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, DisconnectImager,
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, InitializeImager,
                           ConfirmingImagePatientName, ReportInvalidRequest );

  /** ImageReady State */

  igstkAddTransitionMacro( ImageReady, LoadMesh,
                           LoadingMesh, LoadMesh );
  igstkAddTransitionMacro( ImageReady, LoadTrackerToolSpatialObject,
                           LoadingTrackerToolSpatialObject, LoadTrackerToolSpatialObject );
  igstkAddTransitionMacro( ImageReady, SetPickingPosition,
                           ImageReady, SetImagePicking );
  igstkAddTransitionMacro( ImageReady, StartSetImageFiducials,
                           SettingImageFiducials, ReportSuccessStartSetImageFiducials );

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
  igstkAddTransitionMacro( ImageReady, LoadImagerToolSpatialObject,
                           ImageReady, ReportInvalidRequest );
  igstkAddTransitionMacro( ImageReady, ConfigureTracker,
                           ImageReady, ReportInvalidRequest );
  igstkAddTransitionMacro( ImageReady, StartImaging,
                           ImageReady, ReportInvalidRequest );
  igstkAddTransitionMacro( ImageReady, StopImaging,
                           ImageReady, ReportInvalidRequest );
  igstkAddTransitionMacro( ImageReady, DisconnectImager,
                           ImageReady, ReportInvalidRequest );
  igstkAddTransitionMacro( ImageReady, InitializeImager,
                           ImageReady, ReportInvalidRequest );

  /** LoadingMesh State */

  igstkAddTransitionMacro( LoadingMesh, Success,
                           ImageReady, ReportSuccessMeshLoaded );

  igstkAddTransitionMacro( LoadingMesh, Failure,
                           ImageReady, ReportFailureMeshLoaded );

  //complete table for state: LoadingMesh

  igstkAddTransitionMacro( LoadingMesh, LoadImage,
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, ConfirmImagePatientName,
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, LoadMesh,
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, LoadTrackerToolSpatialObject,
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, LoadImagerToolSpatialObject,
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, StartSetImageFiducials,
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, SetPickingPosition,
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, EndSetImageFiducials,
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, RegisterTracker,
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, StartSetTrackerFiducials,
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, AcceptTrackerFiducial,
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, EndSetTrackerFiducials,
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, ConfigureTracker,
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, InitializeTracker,
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, StartTracking,
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, StopTracking,
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, DisconnectTracker,
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, StartImaging,
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, StopImaging,
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, DisconnectImager,
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, InitializeImager,
                           LoadingMesh, ReportInvalidRequest );

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
  igstkAddTransitionMacro( SettingImageFiducials, LoadMesh,
                           SettingImageFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingImageFiducials, LoadTrackerToolSpatialObject,
                           SettingImageFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingImageFiducials, LoadImagerToolSpatialObject,
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
  igstkAddTransitionMacro( SettingImageFiducials, StartImaging,
                           SettingImageFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingImageFiducials, StopImaging,
                           SettingImageFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingImageFiducials, DisconnectImager,
                           SettingImageFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingImageFiducials, InitializeImager,
                           SettingImageFiducials, ReportInvalidRequest );

   /** LoadingTrackerToolSpatialObject State */

  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, Success,
                           TrackerToolSpatialObjectReady, ReportSuccessTrackerToolSpatialObjectLoaded );

  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, Failure,
                           ImageReady, ReportFailureTrackerToolSpatialObjectLoaded );

  //complete table for state: LoadingTrackerToolSpatialObject

  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, LoadImage,
                           LoadingTrackerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, ConfirmImagePatientName,
                           LoadingTrackerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, LoadMesh,
                           LoadingTrackerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, LoadTrackerToolSpatialObject,
                           LoadingTrackerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, LoadImagerToolSpatialObject,
                           LoadingTrackerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, StartSetImageFiducials,
                           LoadingTrackerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, SetPickingPosition,
                           LoadingTrackerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, EndSetImageFiducials,
                           LoadingTrackerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, RegisterTracker,
                           LoadingTrackerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, StartSetTrackerFiducials,
                           LoadingTrackerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, AcceptTrackerFiducial,
                           LoadingTrackerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, EndSetTrackerFiducials,
                           LoadingTrackerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, ConfigureTracker,
                           LoadingTrackerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, InitializeTracker,
                           LoadingTrackerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, StartTracking,
                           LoadingTrackerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, StopTracking,
                           LoadingTrackerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, DisconnectTracker,
                           LoadingTrackerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, StartImaging,
                           LoadingTrackerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, StopImaging,
                           LoadingTrackerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, DisconnectImager,
                           LoadingTrackerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingTrackerToolSpatialObject, InitializeImager,
                           LoadingTrackerToolSpatialObject, ReportInvalidRequest );

  /** TrackerToolSpatialObjectReady State */

  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, ConfigureTracker,
                           ConfiguringTracker, ConfigureTracker );

  //complete table for state: ImageReady
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, Success,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, Failure,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, LoadImage,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, ConfirmImagePatientName,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, EndSetImageFiducials,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, LoadMesh,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, InitializeTracker,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, RegisterTracker,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, StartSetTrackerFiducials,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, AcceptTrackerFiducial,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, EndSetTrackerFiducials,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, StartTracking,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, StopTracking,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, DisconnectTracker,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, LoadTrackerToolSpatialObject,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, StartSetImageFiducials,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, SetPickingPosition,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, LoadImagerToolSpatialObject,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, StartImaging,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, StopImaging,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, DisconnectImager,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerToolSpatialObjectReady, InitializeImager,
                           TrackerToolSpatialObjectReady, ReportInvalidRequest );

  /** ConfiguringTracker State */

  igstkAddTransitionMacro( ConfiguringTracker, Success,
                           TrackerConfigurationReady, ReportSuccessTrackerConfiguration );

  igstkAddTransitionMacro( ConfiguringTracker, Failure,
                           TrackerToolSpatialObjectReady, ReportFailureTrackerConfiguration );

  //complete table for state: ConfiguringTracker

  igstkAddTransitionMacro( ConfiguringTracker, LoadImage,
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, ConfirmImagePatientName,
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, LoadMesh,
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, LoadTrackerToolSpatialObject,
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, LoadImagerToolSpatialObject,
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
  igstkAddTransitionMacro( ConfiguringTracker, StartImaging,
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, StopImaging,
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, DisconnectImager,
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, InitializeImager,
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
  igstkAddTransitionMacro( TrackerConfigurationReady, ConfirmImagePatientName,
                           TrackerConfigurationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerConfigurationReady, LoadMesh,
                           TrackerConfigurationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerConfigurationReady, LoadTrackerToolSpatialObject,
                           TrackerConfigurationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerConfigurationReady, LoadImagerToolSpatialObject,
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
  igstkAddTransitionMacro( TrackerConfigurationReady, StartImaging,
                           TrackerConfigurationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerConfigurationReady, StopImaging,
                           TrackerConfigurationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerConfigurationReady, DisconnectImager,
                           TrackerConfigurationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerConfigurationReady, InitializeImager,
                           TrackerConfigurationReady, ReportInvalidRequest );

  /** InitializingTracker State */

  igstkAddTransitionMacro( InitializingTracker, Success,
                           TrackerInitializationReady, ReportSuccessTrackerInitialization );

  igstkAddTransitionMacro( InitializingTracker, Failure,
                           ImageReady, ReportFailureTrackerInitialization );

  //complete table for state: InitializingTracker

  igstkAddTransitionMacro( InitializingTracker, LoadImage,
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, ConfirmImagePatientName,
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, LoadMesh,
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, LoadTrackerToolSpatialObject,
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, LoadImagerToolSpatialObject,
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
  igstkAddTransitionMacro( InitializingTracker, StartImaging,
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, StopImaging,
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, DisconnectImager,
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, InitializeImager,
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
  igstkAddTransitionMacro( TrackerInitializationReady, ConfirmImagePatientName,
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, LoadMesh,
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, LoadTrackerToolSpatialObject,
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, LoadImagerToolSpatialObject,
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
  igstkAddTransitionMacro( TrackerInitializationReady, StartImaging,
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, StopImaging,
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, DisconnectImager,
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, InitializeImager,
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
  igstkAddTransitionMacro( SettingTrackerFiducials, ConfirmImagePatientName,
                           SettingTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingTrackerFiducials, LoadMesh,
                           SettingTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingTrackerFiducials, LoadTrackerToolSpatialObject,
                           SettingTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingTrackerFiducials, LoadImagerToolSpatialObject,
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
  igstkAddTransitionMacro( SettingTrackerFiducials, StartImaging,
                           SettingTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingTrackerFiducials, StopImaging,
                           SettingTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingTrackerFiducials, DisconnectImager,
                           SettingTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingTrackerFiducials, InitializeImager,
                           SettingTrackerFiducials, ReportInvalidRequest );

  /** EndingSetTrackerFiducials State*/

  igstkAddTransitionMacro( EndingSetTrackerFiducials, Success,
                           TrackerFiducialsReady, ReportSuccessEndSetTrackerFiducials );

  igstkAddTransitionMacro( EndingSetTrackerFiducials, Failure,
                           SettingTrackerFiducials, ReportFailureEndSetTrackerFiducials );

  //complete table for state: EndingSetTrackerFiducials
  igstkAddTransitionMacro( EndingSetTrackerFiducials, LoadImage,
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, ConfirmImagePatientName,
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, LoadMesh,
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, LoadTrackerToolSpatialObject,
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, LoadImagerToolSpatialObject,
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
  igstkAddTransitionMacro( EndingSetTrackerFiducials, StartImaging,
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, StopImaging,
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, DisconnectImager,
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, InitializeImager,
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
  igstkAddTransitionMacro( TrackerFiducialsReady, ConfirmImagePatientName,
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, LoadMesh,
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, LoadTrackerToolSpatialObject,
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, LoadImagerToolSpatialObject,
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
  igstkAddTransitionMacro( TrackerFiducialsReady, StartImaging,
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, StopImaging,
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, DisconnectImager,
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, InitializeImager,
                           TrackerFiducialsReady, ReportInvalidRequest );

  /** RegisteringTracker State*/

  igstkAddTransitionMacro( RegisteringTracker, Success,
                           AcceptingRegistration, ReportSuccessTrackerRegistration );
  igstkAddTransitionMacro( RegisteringTracker, Failure,
                           TrackerInitializationReady, ReportFailureTrackerRegistration );

  //complete table for state: RegisteringTracker

  igstkAddTransitionMacro( RegisteringTracker, LoadImage,
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, ConfirmImagePatientName,
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, LoadMesh,
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, LoadTrackerToolSpatialObject,
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, LoadImagerToolSpatialObject,
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
  igstkAddTransitionMacro( RegisteringTracker, StartImaging,
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, StopImaging,
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, DisconnectImager,
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, InitializeImager,
                           RegisteringTracker, ReportInvalidRequest );

  /** AcceptingRegistration State*/

  igstkAddTransitionMacro( AcceptingRegistration, Success,
                           RegistrationReady, ReportSuccessAcceptingRegistration );
  igstkAddTransitionMacro( AcceptingRegistration, Failure,
                           TrackerInitializationReady, ReportFailureAcceptingRegistration );

  //complete table for state: RegisteringTracker

  igstkAddTransitionMacro( AcceptingRegistration, LoadImage,
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, ConfirmImagePatientName,
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, LoadMesh,
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, LoadTrackerToolSpatialObject,
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, LoadImagerToolSpatialObject,
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
  igstkAddTransitionMacro( AcceptingRegistration, StartImaging,
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, StopImaging,
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, DisconnectImager,
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, InitializeImager,
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
  igstkAddTransitionMacro( RegistrationReady, ConfirmImagePatientName,
                           RegistrationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( RegistrationReady, LoadMesh,
                           RegistrationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( RegistrationReady, LoadTrackerToolSpatialObject,
                           RegistrationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( RegistrationReady, LoadImagerToolSpatialObject,
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
  igstkAddTransitionMacro( RegistrationReady, StartImaging,
                           RegistrationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( RegistrationReady, StopImaging,
                           RegistrationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( RegistrationReady, DisconnectImager,
                           RegistrationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( RegistrationReady, InitializeImager,
                           RegistrationReady, ReportInvalidRequest );

  /** StartingTracker State*/

  igstkAddTransitionMacro( StartingTracker, Success,
                           Tracking, ReportSuccessStartTracking);

  igstkAddTransitionMacro( StartingTracker, Failure,
                           ImageReady, ReportFailureStartTracking );

  //complete table for state: StartingTracker

  igstkAddTransitionMacro( StartingTracker, LoadImage,
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, ConfirmImagePatientName,
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, LoadMesh,
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, LoadTrackerToolSpatialObject,
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, LoadImagerToolSpatialObject,
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
  igstkAddTransitionMacro( StartingTracker, StartImaging,
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, StopImaging,
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, DisconnectImager,
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, InitializeImager,
                           StartingTracker, ReportInvalidRequest );

  /** Tracking State */

  igstkAddTransitionMacro( Tracking, StopTracking,
                           StoppingTracker, StopTracking );

  igstkAddTransitionMacro( Tracking, InitializeImager,
                           InitializingImager, InitializeImager );

  igstkAddTransitionMacro( Tracking, StartImaging,
                           StartingImager, StartImaging );

  igstkAddTransitionMacro( Tracking, StartSetTrackerFiducials,
                           SettingTrackerFiducials, StartSetTrackerFiducials );

  igstkAddTransitionMacro( Tracking, LoadImagerToolSpatialObject,
                           LoadingImagerToolSpatialObject, LoadImagerToolSpatialObject );

  //complete table for state: Tracking

  igstkAddTransitionMacro( Tracking, Success,
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, Failure,
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, LoadImage,
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, ConfirmImagePatientName,
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, LoadMesh,
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, LoadTrackerToolSpatialObject,
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
  igstkAddTransitionMacro( Tracking, StopImaging,
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, DisconnectImager,
                           Tracking, ReportInvalidRequest );

  /** StoppingTracker State */

  igstkAddTransitionMacro( StoppingTracker, Success,
                           RegistrationReady, ReportSuccessStopTracking);

  igstkAddTransitionMacro( StoppingTracker, Failure,
                           Tracking, ReportFailureStopTracking );

  //complete table for state: StoppingTracker

  igstkAddTransitionMacro( StoppingTracker, LoadImage,
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, ConfirmImagePatientName,
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, LoadMesh,
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, LoadTrackerToolSpatialObject,
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
  igstkAddTransitionMacro( StoppingTracker, LoadImagerToolSpatialObject,
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, StartImaging,
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, StopImaging,
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, DisconnectImager,
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, InitializeImager,
                           StoppingTracker, ReportInvalidRequest );

  /** DisconnectingTracker Tracker */

  igstkAddTransitionMacro( DisconnectingTracker, Success,
                           ImageReady, ReportSuccessTrackerDisconnection);

  igstkAddTransitionMacro( DisconnectingTracker, Failure,
                           RegistrationReady, ReportFailureTrackerDisconnection );

  //complete table for state: DisconnectingTracker

  igstkAddTransitionMacro( DisconnectingTracker, LoadImage,
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, ConfirmImagePatientName,
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, LoadMesh,
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, LoadTrackerToolSpatialObject,
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
  igstkAddTransitionMacro( DisconnectingTracker, LoadImagerToolSpatialObject,
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, StartImaging,
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, StopImaging,
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, DisconnectImager,
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, InitializeImager,
                           DisconnectingTracker, ReportInvalidRequest );

  /** LoadingImagerToolSpatialObject State */

  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, Success,
                           Tracking, ReportSuccessImagerToolSpatialObjectLoaded );

  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, Failure,
                           Tracking, ReportFailureImagerToolSpatialObjectLoaded );

  //complete table for state: LoadingImagerToolSpatialObject

  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, LoadImage,
                           LoadingImagerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, ConfirmImagePatientName,
                           LoadingImagerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, LoadMesh,
                           LoadingImagerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, LoadTrackerToolSpatialObject,
                           LoadingImagerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, LoadImagerToolSpatialObject,
                           LoadingImagerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, StartSetImageFiducials,
                           LoadingImagerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, SetPickingPosition,
                           LoadingImagerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, EndSetImageFiducials,
                           LoadingImagerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, RegisterTracker,
                           LoadingImagerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, StartSetTrackerFiducials,
                           LoadingImagerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, AcceptTrackerFiducial,
                           LoadingImagerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, EndSetTrackerFiducials,
                           LoadingImagerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, ConfigureTracker,
                           LoadingImagerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, InitializeTracker,
                           LoadingImagerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, StartTracking,
                           LoadingImagerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, StopTracking,
                           LoadingImagerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, DisconnectTracker,
                           LoadingImagerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, StartImaging,
                           LoadingImagerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, StopImaging,
                           LoadingImagerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, DisconnectImager,
                           LoadingImagerToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImagerToolSpatialObject, InitializeImager,
                           LoadingImagerToolSpatialObject, ReportInvalidRequest );

  /** InitializingImager State */

  igstkAddTransitionMacro( InitializingImager, Success,
                           Tracking, ReportSuccessImagerInitialization );

  igstkAddTransitionMacro( InitializingImager, Failure,
                           Tracking, ReportFailureImagerInitialization );

  //complete table for state: InitializingImager

  igstkAddTransitionMacro( InitializingImager, LoadImage,
                           InitializingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingImager, ConfirmImagePatientName,
                           InitializingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingImager, LoadMesh,
                           InitializingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingImager, LoadTrackerToolSpatialObject,
                           InitializingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingImager, LoadImagerToolSpatialObject,
                           InitializingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingImager, StartSetImageFiducials,
                           InitializingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingImager, SetPickingPosition,
                           InitializingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingImager, EndSetImageFiducials,
                           InitializingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingImager, ConfigureTracker,
                           InitializingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingImager, InitializeTracker,
                           InitializingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingImager, RegisterTracker,
                           InitializingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingImager, StartSetTrackerFiducials,
                           InitializingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingImager, AcceptTrackerFiducial,
                           InitializingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingImager, EndSetTrackerFiducials,
                           InitializingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingImager, StartTracking,
                           InitializingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingImager, StopTracking,
                           InitializingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingImager, DisconnectTracker,
                           InitializingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingImager, StartImaging,
                           InitializingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingImager, StopImaging,
                           InitializingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingImager, DisconnectImager,
                           InitializingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingImager, InitializeImager,
                           InitializingImager, ReportInvalidRequest );

  /** StartingImager State*/

  igstkAddTransitionMacro( StartingImager, Success,
                           TrackingAndImaging, ReportSuccessStartImaging );

  igstkAddTransitionMacro( StartingImager, Failure,
                           Tracking, ReportFailureStartImaging );

  //complete table for state: StartingImager

  igstkAddTransitionMacro( StartingImager, LoadImage,
                           StartingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingImager, ConfirmImagePatientName,
                           StartingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingImager, LoadMesh,
                           StartingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingImager, LoadTrackerToolSpatialObject,
                           StartingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingImager, LoadImagerToolSpatialObject,
                           StartingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingImager, StartSetImageFiducials,
                           StartingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingImager, SetPickingPosition,
                           StartingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingImager, StartSetTrackerFiducials,
                           StartingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingImager, EndSetImageFiducials,
                           StartingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingImager, ConfigureTracker,
                           StartingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingImager, InitializeTracker,
                           StartingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingImager, RegisterTracker,
                           StartingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingImager, AcceptTrackerFiducial,
                           StartingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingImager, EndSetTrackerFiducials,
                           StartingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingImager, StartTracking,
                           StartingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingImager, StopTracking,
                           StartingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingImager, DisconnectTracker,
                           StartingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingImager, StartImaging,
                           StartingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingImager, StopImaging,
                           StartingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingImager, DisconnectImager,
                           StartingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingImager, InitializeImager,
                           StartingImager, ReportInvalidRequest );

  /** TrackingAndImagingAndImaging State */

  igstkAddTransitionMacro( TrackingAndImaging, StopImaging,
                           StoppingImager, StopImaging );

  //complete table for state: TrackingAndImaging

  igstkAddTransitionMacro( TrackingAndImaging, Success,
                           TrackingAndImaging, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackingAndImaging, Failure,
                           TrackingAndImaging, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackingAndImaging, LoadImage,
                           TrackingAndImaging, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackingAndImaging, ConfirmImagePatientName,
                           TrackingAndImaging, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackingAndImaging, LoadMesh,
                           TrackingAndImaging, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackingAndImaging, LoadTrackerToolSpatialObject,
                           TrackingAndImaging, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackingAndImaging, StartSetImageFiducials,
                           TrackingAndImaging, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackingAndImaging, EndSetImageFiducials,
                           TrackingAndImaging, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackingAndImaging, StartSetTrackerFiducials,
                           TrackingAndImaging, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackingAndImaging, LoadImagerToolSpatialObject,
                           TrackingAndImaging, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackingAndImaging, InitializeTracker,
                           TrackingAndImaging, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackingAndImaging, ConfigureTracker,
                           TrackingAndImaging, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackingAndImaging, RegisterTracker,
                           TrackingAndImaging, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackingAndImaging, AcceptTrackerFiducial,
                           TrackingAndImaging, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackingAndImaging, EndSetTrackerFiducials,
                           TrackingAndImaging, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackingAndImaging, SetPickingPosition,
                           TrackingAndImaging, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackingAndImaging, StartTracking,
                           TrackingAndImaging, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackingAndImaging, StopTracking,
                           TrackingAndImaging, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackingAndImaging, DisconnectTracker,
                           TrackingAndImaging, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackingAndImaging, StartImaging,
                           TrackingAndImaging, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackingAndImaging, InitializeImager,
                           TrackingAndImaging, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackingAndImaging, DisconnectImager,
                           TrackingAndImaging, ReportInvalidRequest );

  /** StoppingImager State */

  igstkAddTransitionMacro( StoppingImager, Success,
                           Tracking, ReportSuccessStopImaging);

  igstkAddTransitionMacro( StoppingImager, Failure,
                           TrackingAndImaging, ReportFailureStopImaging );

  //complete table for state: StoppingImager

  igstkAddTransitionMacro( StoppingImager, LoadImage,
                           StoppingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingImager, ConfirmImagePatientName,
                           StoppingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingImager, LoadMesh,
                           StoppingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingImager, LoadTrackerToolSpatialObject,
                           StoppingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingImager, StartSetImageFiducials,
                           StoppingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingImager, SetPickingPosition,
                           StoppingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingImager, EndSetImageFiducials,
                           StoppingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingImager, InitializeTracker,
                           StoppingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingImager, ConfigureTracker,
                           StoppingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingImager, RegisterTracker,
                           StoppingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingImager, StartSetTrackerFiducials,
                           StoppingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingImager, AcceptTrackerFiducial,
                           StoppingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingImager, EndSetTrackerFiducials,
                           StoppingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingImager, StartTracking,
                           StoppingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingImager, StopTracking,
                           StoppingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingImager, DisconnectTracker,
                           StoppingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingImager, LoadImagerToolSpatialObject,
                           StoppingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingImager, StartImaging,
                           StoppingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingImager, StopImaging,
                           StoppingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingImager, DisconnectImager,
                           StoppingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingImager, InitializeImager,
                           StoppingImager, ReportInvalidRequest );

  /** DisconnectingImager */

  igstkAddTransitionMacro( DisconnectingImager, Success,
                           Tracking, ReportSuccessImagerDisconnection);

  igstkAddTransitionMacro( DisconnectingImager, Failure,
                           Tracking, ReportFailureTrackerDisconnection );

  //complete table for state: DisconnectingImager

  igstkAddTransitionMacro( DisconnectingImager, LoadImage,
                           DisconnectingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingImager, ConfirmImagePatientName,
                           DisconnectingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingImager, LoadMesh,
                           DisconnectingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingImager, LoadTrackerToolSpatialObject,
                           DisconnectingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingImager, StartSetImageFiducials,
                           DisconnectingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingImager, SetPickingPosition,
                           DisconnectingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingImager, EndSetImageFiducials,
                           DisconnectingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingImager, InitializeTracker,
                           DisconnectingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingImager, ConfigureTracker,
                           DisconnectingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingImager, RegisterTracker,
                           DisconnectingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingImager, StartSetTrackerFiducials,
                           DisconnectingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingImager, AcceptTrackerFiducial,
                           DisconnectingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingImager, EndSetTrackerFiducials,
                           DisconnectingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingImager, StartTracking,
                           DisconnectingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingImager, StopTracking,
                           DisconnectingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingImager, DisconnectTracker,
                           DisconnectingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingImager, LoadImagerToolSpatialObject,
                           DisconnectingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingImager, StartImaging,
                           DisconnectingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingImager, StopImaging,
                           DisconnectingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingImager, DisconnectImager,
                           DisconnectingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingImager, InitializeImager,
                           DisconnectingImager, ReportInvalidRequest );
  /** Set Initial State */

  igstkSetInitialStateMacro( Initial );

  m_StateMachine.SetReadyToRun();

  this->DisableAll();

  this->m_LoadImageButton->activate();

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
UltrasoundNavigator::~UltrasoundNavigator()
{

}

void UltrasoundNavigator::ConfigureTrackerProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
             "WorkingVolumeTester::ConfigureTrackerProcessing called...\n" )

  const char*  fileName =
    fl_file_chooser("Select a tracker configuration file","*.xml", "auroraConfiguration.xml");

  if ( fileName == NULL )
  {
      igstkLogMacro2( m_Logger, DEBUG,
             "WorkingVolumeTester::ConfigureTrackerProcessing none file was selected or operation canceled...\n" )
      m_StateMachine.PushInput( m_FailureInput );
      m_StateMachine.ProcessInputs();
      return;
  }

  const unsigned int NUM_TRACKER_TYPES = 6;
  igstk::TrackerConfigurationXMLFileReaderBase::Pointer
       trackerCofigurationXMLReaders[NUM_TRACKER_TYPES];
  trackerCofigurationXMLReaders[0] =
       igstk::PolarisVicraConfigurationXMLFileReader::New();
  trackerCofigurationXMLReaders[1] =
       igstk::PolarisSpectraConfigurationXMLFileReader::New();
  trackerCofigurationXMLReaders[2] =
       igstk::PolarisHybridConfigurationXMLFileReader::New();
  trackerCofigurationXMLReaders[3] =
       igstk::AuroraConfigurationXMLFileReader::New();
  trackerCofigurationXMLReaders[4] =
       igstk::MicronConfigurationXMLFileReader::New();
  trackerCofigurationXMLReaders[5] =
       igstk::AscensionConfigurationXMLFileReader::New();

  igstk::TrackerConfigurationFileReader::Pointer trackerConfigReader =
        igstk::TrackerConfigurationFileReader::New();

  //need to observe if the request read succeeds or fails
  //there is a third option that the read is invalid, if the
  //file name or xml reader weren't set
  igstk::TrackerConfigurationFileReader::ReadFailSuccessObserver::Pointer
        rfso = igstk::TrackerConfigurationFileReader::ReadFailSuccessObserver::New();

  trackerConfigReader->AddObserver(
           igstk::TrackerConfigurationFileReader::ReadSuccessEvent(), rfso );

  trackerConfigReader->AddObserver(
           igstk::TrackerConfigurationFileReader::ReadFailureEvent(), rfso );

  trackerConfigReader->AddObserver(
           igstk::TrackerConfigurationFileReader::UnexpectedTrackerTypeEvent(), rfso );

  //setting the file name and reader always succeeds so I don't
  //observe for success event
  trackerConfigReader->RequestSetFileName( fileName );

  TrackerConfigurationObserver::Pointer tco = TrackerConfigurationObserver::New();

  for( unsigned int i=0; i<NUM_TRACKER_TYPES; i++ )
  {
   //setting the xml reader always succeeds so I don't
   //observe the success event
   trackerConfigReader->RequestSetReader( trackerCofigurationXMLReaders[i] );

   trackerConfigReader->RequestRead();

   if( rfso->GotUnexpectedTrackerType() )
   {
     rfso->Reset();
   }
   else if( rfso->GotFailure() && !rfso->GotUnexpectedTrackerType() )
   {
    //throw ExceptionWithMessage( rfso->GetFailureMessage() );
    std::string errorMessage = rfso->GetFailureMessage();
    fl_alert( errorMessage.c_str() );
    fl_beep( FL_BEEP_ERROR );
    igstkLogMacro2( m_Logger, DEBUG, "Tracker Configuration error\n" )

     m_StateMachine.PushInput( m_FailureInput );
     m_StateMachine.ProcessInputs();
     return;
   }
   else if( rfso->GotSuccess() )
   {
     //get the configuration data from the reader
     trackerConfigReader->AddObserver(
       igstk::TrackerConfigurationFileReader::TrackerConfigurationDataEvent(), tco );
     trackerConfigReader->RequestGetData();

     if( tco->GotTrackerConfiguration() )
     {
       m_TrackerConfiguration = tco->GetTrackerConfiguration();
       m_StateMachine.PushInput( m_SuccessInput );
     }
     else
     {
       igstkLogMacro2( m_Logger, DEBUG, "Could not get tracker configuration error\n" )
       m_StateMachine.PushInput( m_FailureInput );
     }

     m_StateMachine.ProcessInputs();
     return;
   }
   else
   {
    // just to complete all possibilities
       igstkLogMacro2( m_Logger, DEBUG, "Very strange tracker configuration error\n" )
       m_StateMachine.PushInput( m_FailureInput );
       m_StateMachine.ProcessInputs();
       return;
   }
  } // for
}

void UltrasoundNavigator::RequestLoadImage()
{
  igstkLogMacro2( m_Logger, DEBUG,
             "UltrasoundNavigator::RequestLoadImage called...\n" )
  m_StateMachine.PushInput( m_LoadImageInput );
  m_StateMachine.ProcessInputs();
}

void UltrasoundNavigator::RequestLoadMesh()
{
  igstkLogMacro2( m_Logger, DEBUG,
             "UltrasoundNavigator::RequestLoadMesh called...\n" )
  m_StateMachine.PushInput( m_LoadMeshInput );
  m_StateMachine.ProcessInputs();
}

void UltrasoundNavigator::RequestLoadTrackerToolSpatialObject()
{
  igstkLogMacro2( m_Logger, DEBUG,
             "UltrasoundNavigator::RequestLoadTrackerToolSpatialObject called...\n" )
  m_StateMachine.PushInput( m_LoadTrackerToolSpatialObjectInput );
  m_StateMachine.ProcessInputs();
}

void UltrasoundNavigator::RequestLoadImagerToolSpatialObject()
{
  igstkLogMacro2( m_Logger, DEBUG,
             "UltrasoundNavigator::RequestLoadImagerToolSpatialObject called...\n" )
  m_StateMachine.PushInput( m_LoadImagerToolSpatialObjectInput );
  m_StateMachine.ProcessInputs();
}

void UltrasoundNavigator::RequestToggleEnableVideo()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::RequestToggleEnableVideo called...\n" )

  if (m_VideoEnabled)
  {
    this->DisableVideo();
  }
  else
  {
    this->EnableVideo();
  }

  m_VideoEnabled = !m_VideoEnabled;
}

void UltrasoundNavigator::RequestToggleRunVideo()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::RequestToggleRunVideo called...\n" )

  if (m_VideoRunning)
  {
    this->StopVideo();
  }
  else
  {
    this->StartVideo();
  }

  m_VideoRunning = !m_VideoRunning;
}

void UltrasoundNavigator::EnableVideo()
{
  // we put here stuff that we want to do once
  // fixme: set a proper state machine action
  if (m_VideoFrame.IsNull())
  {
    this->RequestLoadImagerToolSpatialObject();
  }

  if (m_VideoFrame.IsNotNull())
  {
    m_ViewerGroup->m_VideoView->RequestAddObject( m_VideoFrameRepresentationForVideoView );
    m_ViewerGroup->m_3DView->RequestAddObject( m_VideoFrameRepresentationFor3DView );
    m_ViewerGroup->m_CTView1->RequestAddObject( m_CTView1ImageRepresentation );
    m_ViewerGroup->m_3DView->RequestAddObject( m_CTView1ImageRepresentation2 );

    m_ViewerGroup->m_VideoView->RequestAddObject( m_ImagerToolRepresentationForVideoView );
    m_ViewerGroup->m_3DView->RequestAddObject( m_ImagerToolRepresentationFor3DView );

    m_ToggleRunVideoButton->activate();
  }

  m_ToggleEnableVideoButton->label("Disable");
  m_ViewerGroup->m_VideoView->RequestResetCamera();
  m_ViewerGroup->m_3DView->RequestResetCamera();
}

void UltrasoundNavigator::DisableVideo()
{
  m_ViewerGroup->m_VideoView->RequestRemoveObject( m_VideoFrameRepresentationForVideoView );
  m_ViewerGroup->m_3DView->RequestRemoveObject( m_VideoFrameRepresentationFor3DView );
  m_ViewerGroup->m_CTView1->RequestRemoveObject( m_CTView1ImageRepresentation );
  m_ViewerGroup->m_3DView->RequestRemoveObject( m_CTView1ImageRepresentation2 );

  m_ViewerGroup->m_VideoView->RequestRemoveObject( m_ImagerToolRepresentationForVideoView );
  m_ViewerGroup->m_3DView->RequestRemoveObject( m_ImagerToolRepresentationFor3DView );

  m_ViewerGroup->m_VideoView->RequestResetCamera();
  m_ViewerGroup->m_3DView->RequestResetCamera();

  m_ToggleEnableVideoButton->label("Enable");
  m_ToggleRunVideoButton->deactivate();
}

void UltrasoundNavigator::StopVideo()
{
  this->RequestStopImaging();
}

void UltrasoundNavigator::StartVideo()
{
  // if we are not initialize
  if (!m_ImagerInitialized)
  {
    this->RequestInitializeImager();
  }

  this->RequestStartImaging();
}

void UltrasoundNavigator::RequestToggleSetImageFiducials()
{
  igstkLogMacro2( m_Logger, DEBUG,
             "UltrasoundNavigator::RequestToggleSetImageFiducials called...\n" )

  if ( !m_ModifyImageFiducialsEnabled )
    m_StateMachine.PushInput( m_StartSetImageFiducialsInput );
  else
    m_StateMachine.PushInput( m_EndSetImageFiducialsInput );

  m_StateMachine.ProcessInputs();
}

void UltrasoundNavigator::RequestAcceptRegistration()
{
  igstkLogMacro2( m_Logger, DEBUG,
             "UltrasoundNavigator::RequestAcceptRegistration called...\n" )

  m_StateMachine.PushInput( m_SuccessInput);
  m_StateMachine.ProcessInputs();
}

void UltrasoundNavigator::RequestRejectRegistration()
{
  igstkLogMacro2( m_Logger, DEBUG,
             "UltrasoundNavigator::RequestRejectRegistration called...\n" )

  m_StateMachine.PushInput( m_FailureInput);
  m_StateMachine.ProcessInputs();
}

void UltrasoundNavigator::RequestConfigureTracker()
{
  igstkLogMacro2( m_Logger, DEBUG,
             "UltrasoundNavigator::RequestConfigureTracker called...\n" )

  m_StateMachine.PushInput( m_ConfigureTrackerInput );
  m_StateMachine.ProcessInputs();
}

void UltrasoundNavigator::RequestInitializeTracker()
{
  igstkLogMacro2( m_Logger, DEBUG,
             "UltrasoundNavigator::RequestInitializeTracker called...\n" )

  m_StateMachine.PushInput( m_InitializeTrackerInput );
  m_StateMachine.ProcessInputs();
}

void UltrasoundNavigator::RequestInitializeImager()
{
  igstkLogMacro2( m_Logger, DEBUG,
             "UltrasoundNavigator::RequestInitializeImager called...\n" )

  m_StateMachine.PushInput( m_InitializeImagerInput );
  m_StateMachine.ProcessInputs();
}

void UltrasoundNavigator::RequestStartSetTrackerFiducials()
{
  igstkLogMacro2( m_Logger, DEBUG,
             "UltrasoundNavigator::RequestStartSetTrackerFiducials called...\n" )

  m_StateMachine.PushInput( m_StartSetTrackerFiducialsInput );
  m_StateMachine.ProcessInputs();
}

void UltrasoundNavigator::RequestAcceptTrackerFiducial()
{
  igstkLogMacro2( m_Logger, DEBUG,
             "UltrasoundNavigator::RequestAcceptTrackerFiducial called...\n" )

  m_StateMachine.PushInput( m_AcceptTrackerFiducialInput );
  m_StateMachine.ProcessInputs();
}

void UltrasoundNavigator::RequestEndSetTrackerFiducials()
{
  igstkLogMacro2( m_Logger, DEBUG,
             "UltrasoundNavigator::RequestEndSetTrackerFiducials called...\n" )

  m_StateMachine.PushInput( m_EndSetTrackerFiducialsInput );
  m_StateMachine.ProcessInputs();
}

void UltrasoundNavigator::RequestTrackerRegistration()
{
  igstkLogMacro2( m_Logger, DEBUG,
             "UltrasoundNavigator::RequestTrackerRegistration called...\n" )

  m_StateMachine.PushInput( m_RegisterTrackerInput );
  m_StateMachine.ProcessInputs();
}

void UltrasoundNavigator::RequestStartTracking()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::RequestStartTracking called...\n" )

  m_StateMachine.PushInput( m_StartTrackingInput );
  m_StateMachine.ProcessInputs();
}

void UltrasoundNavigator::RequestStartImaging()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::RequestStartImaging called...\n" )

  m_StateMachine.PushInput( m_StartImagingInput );
  m_StateMachine.ProcessInputs();
}

void UltrasoundNavigator::RequestStopTracking()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::RequestStopTracking called...\n" )

  m_StateMachine.PushInput( m_StopTrackingInput );
  m_StateMachine.ProcessInputs();
}

void UltrasoundNavigator::RequestStopImaging()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::RequestStopImaging called...\n" )

  m_StateMachine.PushInput( m_StopImagingInput );
  m_StateMachine.ProcessInputs();
}

void UltrasoundNavigator::RequestToggleOrthogonalPlanes()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::RequestToggleOrthogonalPlanes called...\n" )

  if (m_ImagePlanesIn3DViewEnabled)
  {
    this->AddImagePlanesTo3DView();
  }
  else
  {
    this->RemoveImagePlanesTo3DView();
  }

  m_ImagePlanesIn3DViewEnabled = !m_ImagePlanesIn3DViewEnabled;
}

void UltrasoundNavigator::RequestDisconnectTracker()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::RequestDisconnectTracker called...\n" )

  m_StateMachine.PushInput( m_DisconnectTrackerInput );
  m_StateMachine.ProcessInputs();
}

void UltrasoundNavigator::RequestDisconnectImager()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::RequestDisconnectImager called...\n" )

  m_StateMachine.PushInput( m_DisconnectImagerInput );
  m_StateMachine.ProcessInputs();
}

/** Method to be invoked when an invalid operation was requested */
void
UltrasoundNavigator::ReportInvalidRequestProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportInvalidRequestProcessing called...\n");
  this->InvokeEvent(InvalidRequestErrorEvent());
}


void
UltrasoundNavigator::ReportSuccessImageLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportSuccessImageLoadedProcessing called...\n");

  //m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 0, "" );
  //m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 0, "" );
  //m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 0, "" );

  for (unsigned int i=0; i<4; i++)
  {
    m_ViewerGroup->m_CTView2->RequestRemoveObject( m_AxialFiducialRepresentationVector[i] );
    m_ViewerGroup->m_3DView->RequestRemoveObject( m_3DViewFiducialRepresentationVector[i] );
  }

  this->DisableAll();

  m_LoadMeshButton->activate();
  m_FiducialsPointList->activate();
  m_ModifyFiducialsButton->activate();
  m_ConfigureTrackerButton->activate();

  m_ViewerGroup->redraw();
  Fl::check();
}

/** Method to be invoked on Failure image loading */
void
UltrasoundNavigator::ReportFailureImageLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportFailureImageLoadedProcessing called...\n");
}

/** Method to be invoked on successful tracker tool spatial object loading */
void
UltrasoundNavigator::ReportSuccessTrackerToolSpatialObjectLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportSuccessTrackerToolSpatialObjectLoadedProcessing called...\n");

  this->RequestConfigureTracker();
}

/** Method to be invoked on Failure tracker tool spatial object loading */
void
UltrasoundNavigator::ReportFailureTrackerToolSpatialObjectLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportFailureTrackerToolSpatialObjectLoadedProcessing called...\n");

}

/** Method to be invoked on successful imager tool spatial object loading */
void
UltrasoundNavigator::ReportSuccessImagerToolSpatialObjectLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportSuccessImagerToolSpatialObjectLoadedProcessing called...\n");

//  this->RequestConfigureImager();
}

/** Method to be invoked on Failure imager tool spatial object loading */
void
UltrasoundNavigator::ReportFailureImagerToolSpatialObjectLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportFailureImagerToolSpatialObjectLoadedProcessing called...\n");

}

/** Method to be invoked on successful target mesh loading */
void
UltrasoundNavigator::ReportSuccessMeshLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportSuccessMeshLoadedProcessing called...\n");
}

/** Method to be invoked on Failure target mesh loading */
void
UltrasoundNavigator::ReportFailureMeshLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportFailureMeshLoadedProcessing called...\n");
}

/** Method to be invoked on successful start set image fiducials */
void
UltrasoundNavigator::ReportSuccessStartSetImageFiducialsProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportSuccessStartSetImageFiducialsProcessing called...\n");
  m_ModifyFiducialsButton->color(FL_YELLOW);

  m_ModifyImageFiducialsEnabled = true;
}

/** Method to be invoked on successful end set image fiducials */
void
UltrasoundNavigator::ReportSuccessEndSetImageFiducialsProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportSuccessEndSetImageFiducialsProcessing called...\n");

  //m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 2, " " );
  //m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 2, " " );
  //m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 2, " " );

  m_ModifyFiducialsButton->color((Fl_Color)55);

  m_ModifyImageFiducialsEnabled = false;
}

/** Method to be invoked on successful start set tracker fiducials */
void
UltrasoundNavigator::ReportSuccessStartSetTrackerFiducialsProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportSuccessStartSetTrackerFiducialsProcessing called...\n");

}


/** Method to be invoked on successful pinpointing */
void
UltrasoundNavigator::ReportSuccessPinpointingTrackerFiducialProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportSuccessPinpointingTrackerFiducialProcessing called...\n");
}

/** Method to be invoked on failure pinpointing */
void
UltrasoundNavigator::ReportFailurePinpointingTrackerFiducialProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportFailurePinpointingTrackerFiducialProcessing called...\n");
}

/** Method to be invoked on successful end set tracker fiducials */
void
UltrasoundNavigator::ReportSuccessEndSetTrackerFiducialsProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportSuccessEndSetTrackerFiducialsProcessing called...\n");

  for (unsigned int i=0; i<4; i++)
  {
    m_ViewerGroup->m_CTView2->RequestRemoveObject( m_AxialFiducialRepresentationVector[i] );
    m_ViewerGroup->m_3DView->RequestRemoveObject( m_3DViewFiducialRepresentationVector[i] );
  }

  this->RequestTrackerRegistration();
}

void
UltrasoundNavigator::AcceptTrackerFiducialProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "AcceptTrackerFiducialProcessing called...\n");

  typedef igstk::TransformObserver ObserverType;
  ObserverType::Pointer transformObserver = ObserverType::New();
  transformObserver->ObserveTransformEventsFrom( m_TrackerTool );
  transformObserver->Clear();
  m_TrackerTool->RequestComputeTransformTo( m_WorldReference );

  if ( !transformObserver->GotTransform() )
    return;

  int n = m_FiducialsPointList->value();
  int m = m_FiducialsPointList->size();
  m_LandmarksContainer[n] =
        TransformToPoint( transformObserver->GetTransform() );
  m_AcceptedLandmarksContainer[n] = true;

  m_AxialFiducialRepresentationVector[n]->SetColor( 0.0, 1.0, 0.0 );
  m_SagittalFiducialRepresentationVector[n]->SetColor( 0.0, 1.0, 0.0 );
  m_CoronalFiducialRepresentationVector[n]->SetColor( 0.0, 1.0, 0.0 );

  if ( n < m )
  {
    m_FiducialsPointList->value(n+1);
  }
  else
  {
    m_FiducialsPointList->value(0);
  }

  this->RequestChangeSelectedFiducial();

  fl_beep( FL_BEEP_MESSAGE );

  AcceptedLandmarkPointContainerType::const_iterator iter;
  iter = m_AcceptedLandmarksContainer.begin();

  unsigned int numberOfAcceptedLandmarks = 0;
  for (;iter != m_AcceptedLandmarksContainer.end(); iter++)
  {
    if (iter->second)
      numberOfAcceptedLandmarks++;
  }

  if ( numberOfAcceptedLandmarks >= 3 )
  {
    m_RegisterButton->label("Ready");
    m_RegisterButton->color(FL_GREEN);
  }
}

/** Method to be invoked on failure end set tracker fiducials */
void
UltrasoundNavigator::ReportFailureEndSetTrackerFiducialsProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportFailureEndSetTrackerFiducialsProcessing called...\n");
}

/** Method to be invoked on successful tracker configuration */
void
UltrasoundNavigator::ReportSuccessTrackerConfigurationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportSuccessTrackerConfigurationProcessing called...\n");

  this->RequestInitializeTracker();
}

/** Method to be invoked on Failure tracker configuration */
void
UltrasoundNavigator::ReportFailureTrackerConfigurationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportFailureTrackerConfigurationProcessing called...\n");

  std::string errorMessage;
  errorMessage = "Could not configure tracker device";
  fl_alert( errorMessage.c_str() );
  fl_beep( FL_BEEP_ERROR );
}

/** Method to be invoked on Failure tracker initialization */
void
UltrasoundNavigator::ReportFailureTrackerInitializationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportFailureTrackerConfigurationProcessing called...\n");

  std::string errorMessage;
  errorMessage = "Could not initialize tracker device";
  fl_alert( errorMessage.c_str() );
  fl_beep( FL_BEEP_ERROR );
}


/** Method to be invoked on successful tracker initialization */
void
UltrasoundNavigator::ReportSuccessTrackerInitializationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportSuccessTrackerInitializationProcessing called...\n");

  this->DisableAll();

  m_RegisterButton->activate();

  this->RequestStartSetTrackerFiducials();
}

/** Method to be invoked on Failure imager initialization */
void
UltrasoundNavigator::ReportFailureImagerInitializationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportFailureImagerInitializationProcessing called...\n");

  std::string errorMessage;
  errorMessage = "Could not initialize imager device";
  fl_alert( errorMessage.c_str() );
  fl_beep( FL_BEEP_ERROR );
}


/** Method to be invoked on successful imager initialization */
void
UltrasoundNavigator::ReportSuccessImagerInitializationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportSuccessImagerInitializationProcessing called...\n");
}

/** Method to be invoked on successful registration acceptance */
void
UltrasoundNavigator::ReportSuccessAcceptingRegistrationProcessing()
{
    igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportSuccessAcceptingRegistration called...\n");

  // add the tool spatial object to the image planes
  // we want to add it now, because otherwise it would've appeared in any place of my scene
  // (i.e. it was not registered to the image)

  m_CTView2PlaneSpatialObject->RequestSetToolSpatialObject( m_TrackerToolSpatialObject );

  // add the tool spatial object to the cross hair spatial object.
  // (the tool spatial object drives the cross hair)
  m_CrossHair->RequestSetToolSpatialObject( m_TrackerToolSpatialObject );

  // build the scene view

  // first of all, let's have an identity transform that lasts for ever
  igstk::Transform identity;
  identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  // setup a tool projection spatial object
  m_ToolProjection = ToolProjectionType::New();
  // todo: get the length from the tracker tool spatial object
  m_ToolProjection->SetSize(150);
  m_ToolProjection->RequestSetTransformAndParent( identity, m_WorldReference );

  // setup tool projection representation for CTView2
  m_CTView2ToolProjectionRepresentation = ToolProjectionRepresentationType::New();
  m_CTView2ToolProjectionRepresentation->RequestSetToolProjectionObject( m_ToolProjection );
  m_CTView2ToolProjectionRepresentation->RequestSetReslicePlaneSpatialObject( m_CTView2PlaneSpatialObject );
  m_CTView2ToolProjectionRepresentation->SetColor( 1,1,0 );
  m_CTView2ToolProjectionRepresentation->SetLineWidth( 2 );

  // add tool representation to the 3D view
  m_ViewerGroup->m_3DView->RequestAddObject( m_TrackerToolRepresentation );

  // add tool projection to the 2D views
  m_ViewerGroup->m_CTView2->RequestAddObject( m_CTView2ToolProjectionRepresentation );

  // reset camera in all views
  m_ViewerGroup->m_CTView1->RequestResetCamera();
  m_ViewerGroup->m_CTView2->RequestResetCamera();
  m_ViewerGroup->m_3DView->RequestResetCamera();

  this->DisableAll();

  m_ViewModeList->activate();
  m_VideoGroup->activate();

  this->RequestStartTracking();
}

/** Method to be invoked on failure registration acceptance */
void
UltrasoundNavigator::ReportFailureAcceptingRegistrationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportFailureAcceptingRegistration called...\n");

  for (unsigned int i=0; i<4; i++)
  {
    m_AxialFiducialRepresentationVector[i]->SetColor( 1.0, 0.0, 0.0 );
    m_SagittalFiducialRepresentationVector[i]->SetColor( 1.0, 0.0, 0.0 );
    m_CoronalFiducialRepresentationVector[i]->SetColor( 1.0, 0.0, 0.0 );
  }

  //try again
  this->RequestStartSetTrackerFiducials();
}

/** Method to be invoked on successful tracker registration */
void
UltrasoundNavigator::ReportSuccessTrackerRegistrationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportSuccessTrackerRegistrationProcessing called...\n");

   // ask the user to accept registration RMS error
   char buf[10];
   sprintf( buf, "%.2f", m_TrackerRMS);

   m_RegistrationErrorLabel->value( buf );

   m_AcceptRegistrationWindow->show();
   this->CenterChildWindowInParentWindow( m_AcceptRegistrationWindow );

   Fl::check();

}

/** Method to be invoked on failure tracker registration */
void
UltrasoundNavigator::ReportFailureTrackerRegistrationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportFailureTrackerRegistrationProcessing called...\n");
}

/** Method to be invoked on failure tracker disconnection */
void
UltrasoundNavigator::ReportFailureTrackerDisconnectionProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportFailureTrackerDisconnectionProcessing called...\n");
}

/** Method to be invoked on successful tracker disconnection */
void
UltrasoundNavigator::ReportSuccessTrackerDisconnectionProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportSuccessTrackerDisconnectionProcessing called...\n");

  //m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 1, "DISCONNECTED" );
  //m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor(1, 1.0, 0.0, 0.0);

  //m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 1, "DISCONNECTED" );
  //m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor(1, 1.0, 0.0, 0.0);

  //m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 1, "DISCONNECTED" );
  //m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor(1, 1.0, 0.0, 0.0);

}

/** Method to be invoked on failure imager disconnection */
void
UltrasoundNavigator::ReportFailureImagerDisconnectionProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportFailureImagerDisconnectionProcessing called...\n");
}

/** Method to be invoked on successful imager disconnection */
void
UltrasoundNavigator::ReportSuccessImagerDisconnectionProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportSuccessImagerDisconnectionProcessing called...\n");
}

/** Method to be invoked on successful tracker start */
void
UltrasoundNavigator::ReportSuccessStartTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportSuccessStartTrackingProcessing called...\n")

  char buf[50];
  sprintf( buf, "TRACKING (%.2f)", m_TrackerRMS);

  //m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 1, buf );
  //m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor( 1, 0.0, 1.0, 0.0 );

  //m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 1, buf );
  //m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor( 1, 0.0, 1.0, 0.0 );

  //m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 1, buf );
  //m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor( 1, 0.0, 1.0, 0.0 );

  for (unsigned int i=0; i<4; i++)
  {
    m_AxialFiducialRepresentationVector[i]->SetColor( 1.0, 0.0, 0.0 );
    m_SagittalFiducialRepresentationVector[i]->SetColor( 1.0, 0.0, 0.0 );
    m_CoronalFiducialRepresentationVector[i]->SetColor( 1.0, 0.0, 0.0 );
  }

  Fl::check();
}

/** Method to be invoked on successful imager start */
void
UltrasoundNavigator::ReportSuccessStartImagingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportSuccessStartImagingProcessing called...\n")

  m_ToggleRunVideoButton->label("Stop");
}

/** Method to be invoked on Failure imager start */
void
UltrasoundNavigator::ReportFailureStartImagingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportFailureStartImagingProcessing called...\n")
}

/** Method to be invoked on Failure tracker start */
void
UltrasoundNavigator::ReportFailureStartTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportFailureStartTrackingProcessing called...\n")
}

/** Method to be invoked on Failure tracker stop */
void
UltrasoundNavigator::ReportFailureStopTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportFailureStopTrackingProcessing called...\n")
}

/** Method to be invoked on Failure imager stop */
void
UltrasoundNavigator::ReportFailureStopImagingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportFailureStopImagingProcessing called...\n")
}

/** Method to be invoked on successful tracker stop */
void
UltrasoundNavigator::ReportSuccessStopTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportSuccessStopTrackingProcessing called...\n")

  //m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 1, "STOPPED" );
  //m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor(1, 0.0, 1.0, 1.0);

  //m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 1, "STOPPED" );
  //m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor(1, 0.0, 1.0, 1.0);

  //m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 1, "STOPPED" );
  //m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor(1, 0.0, 1.0, 1.0);

//  Fl::check();
}

/** Method to be invoked on successful imager stop */
void
UltrasoundNavigator::ReportSuccessStopImagingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::"
                 "ReportSuccessStopImagingProcessing called...\n")

  m_ToggleRunVideoButton->label("Run");
}

void UltrasoundNavigator::LoadImageProcessing()
{
    igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::LoadImageProcessing called...\n" )

    const char*  directoryName =
        fl_dir_chooser("Set DICOM directory ", m_ImageDir.c_str());

   if ( !directoryName )
   {
      igstkLogMacro2( m_Logger, DEBUG, "No directory was selected\n" )
      m_StateMachine.PushInput( m_FailureInput );
      m_StateMachine.ProcessInputs();
      return;
   }

   m_ImageDir = directoryName;

   //check if FLTK has added a slash
   if (itksys::SystemTools::StringEndsWith(m_ImageDir.c_str(),"/") )
   {
     m_ImageDir = m_ImageDir.substr (0,m_ImageDir.length()-1);
   }

   igstkLogMacro2( m_Logger, DEBUG,
                      "Set ImageReader directory: " << directoryName << "\n" )

   /** Setup image reader */
   m_ImageReader         = ImageReaderType::New();
   m_ImageReader->SetGlobalWarningDisplay(false);
   // Initialize the progress command
   m_DICOMProgressCommand = ProgressCommandType::New();
   m_DICOMProgressCommand->SetCallbackFunction( this, &UltrasoundNavigator::OnITKProgressEvent );

   m_ImageReader->RequestSetDirectory( directoryName );

   // Provide a progress observer to the image reader
   m_ImageReader->RequestSetProgressCallback( m_DICOMProgressCommand );

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
}

/** -----------------------------------------------------------------
* Confirm patient's name. This method asks for a confirmation to accept
* the loaded image.
*  -----------------------------------------------------------------
*/
void UltrasoundNavigator::ConfirmPatientNameProcessing()
{
   igstkLogMacro2( m_Logger, DEBUG,
                  "UltrasoundNavigator::ConfirmImagePatientNameProcessing called...\n" )

   // ask the user to confirm patient's name

   if ( m_ImageObserver.IsNotNull() )
   {
     if ( m_ImageReader->FileSuccessfullyRead() )
     {
        m_PatientNameLabel->value( m_ImageReader->GetPatientName().c_str() );
     }
     else
     {
     fl_beep( FL_BEEP_ERROR );
     igstkLogMacro2( m_Logger, DEBUG,
     "UltrasoundNavigator::ConfirmImagePatientNameProcessing \
     file was not successfully read!\n" )
     m_StateMachine.PushInput( m_FailureInput);
     m_StateMachine.ProcessInputs();
     return;
     }
   }

   m_PatientNameWindow->show();
   this->CenterChildWindowInParentWindow( m_PatientNameWindow );
}

/** -----------------------------------------------------------------
* Cancels image load in response to user indication.
*  -----------------------------------------------------------------
*/
void UltrasoundNavigator::RequestCancelImageLoad()
{
   igstkLogMacro2( m_Logger, DEBUG,
                  "UltrasoundNavigator::RequestCancelImageLoad called...\n" )

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
void UltrasoundNavigator::RequestAcceptImageLoad()
{
  igstkLogMacro2( m_Logger, DEBUG,
              "UltrasoundNavigator::RequestAcceptImageLoad called...\n" )

  if ( m_ImageObserver.IsNotNull() )
  {
    m_ImageSpatialObject = m_ImageObserver->GetImage();
    this->ConnectImageRepresentation();
    this->ReadFiducials();
    this->RequestChangeSelectedFiducial();
    m_StateMachine.PushInputBoolean( m_ImageReader->FileSuccessfullyRead(),
                                     m_SuccessInput, m_FailureInput);
  }
  m_StateMachine.ProcessInputs();
}


/** -----------------------------------------------------------------
* Load tool spatial object mesh for the tracker. This method asks for a file with the
* spatial object mesh in the .msh format (see mesh SpatialObject in ITK)
*  -----------------------------------------------------------------
*/
void UltrasoundNavigator::LoadTrackerToolSpatialObjectProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::LoadTrackerToolSpatialObjectProcessing called...\n" )

   const char*  fileName =
    fl_file_chooser("Chose a tool spatial object mesh for the tracker", "*.msh", "");

   if ( !fileName )
    {
     igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::LoadTrackerToolSpatialObjectProcessing No file was selected\n" )
     m_StateMachine.PushInput( m_FailureInput );
     m_StateMachine.ProcessInputs();
     return;
    }

   MeshReaderType::Pointer reader = MeshReaderType::New();
   reader->RequestSetFileName( fileName );

   reader->RequestReadObject();

   MeshObjectObserver::Pointer observer = MeshObjectObserver::New();

   reader->AddObserver( igstk::MeshReader::MeshModifiedEvent(), observer);

   reader->RequestGetOutput();

   if( !observer->GotMeshObject() )
   {
       igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::LoadTrackerToolSpatialObjectProcessing cannot read mesh\n" )
       m_StateMachine.PushInput( m_FailureInput );
       m_StateMachine.ProcessInputs();
       return;
   }

   m_TrackerToolSpatialObject = observer->GetMeshObject();

   m_TrackerToolRepresentation = MeshRepresentationType::New();
   m_TrackerToolRepresentation->RequestSetMeshObject( m_TrackerToolSpatialObject );
   m_TrackerToolRepresentation->SetOpacity(1.0);
   m_TrackerToolRepresentation->SetColor(0,0,1);

   m_StateMachine.PushInput( m_SuccessInput);
   m_StateMachine.ProcessInputs();
}

/** -----------------------------------------------------------------
* Load tool spatial object mesh for the imager. This method asks for a file with the
* spatial object mesh in the .msh format (see mesh SpatialObject in ITK)
*  -----------------------------------------------------------------
*/
void UltrasoundNavigator::LoadImagerToolSpatialObjectProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::LoadImagerToolSpatialObjectProcessing called...\n" )

   const char*  fileName =
    fl_file_chooser("Chose a tool spatial object mesh for the imager", "*.msh", "");

   if ( !fileName )
    {
     igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::LoadImagerToolSpatialObjectProcessing No file was selected\n" )
     m_StateMachine.PushInput( m_FailureInput );
     m_StateMachine.ProcessInputs();
     return;
    }

   MeshReaderType::Pointer reader = MeshReaderType::New();
   reader->RequestSetFileName( fileName );

   reader->RequestReadObject();

   MeshObjectObserver::Pointer observer = MeshObjectObserver::New();

   reader->AddObserver( igstk::MeshReader::MeshModifiedEvent(), observer);

   reader->RequestGetOutput();

   if( !observer->GotMeshObject() )
   {
       igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::LoadImagerToolSpatialObjectProcessing cannot read mesh\n" )
       m_StateMachine.PushInput( m_FailureInput );
       m_StateMachine.ProcessInputs();
       return;
   }

   m_ImagerToolSpatialObject = observer->GetMeshObject();

   if (m_ImagerToolSpatialObject.IsNull() )
   {
       igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::LoadImagerToolSpatialObjectProcessing\
                                        could not get spatial object\n" )
       m_StateMachine.PushInput( m_FailureInput );
       m_StateMachine.ProcessInputs();
       return;
   }

   igstk::Transform identity;
   identity.SetToIdentity(igstk::TimeStamp::GetLongestPossibleTime());

   // set imager tool spatial object as child of imager tool
   m_ImagerToolSpatialObject->RequestDetachFromParent();
   m_ImagerToolSpatialObject->RequestSetTransformAndParent( identity, m_ImagerTool );

   // create reslice plane spatial object for CTView1
   m_CTView1PlaneSpatialObject = ReslicerPlaneType::New();
   // todo: change to Oblique
   m_CTView1PlaneSpatialObject->RequestSetReslicingMode( ReslicerPlaneType::Orthogonal );
   // todo: change to PlaneOrientationWithXAxesNormal
   m_CTView1PlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Axial );
   m_CTView1PlaneSpatialObject->RequestSetBoundingBoxProviderSpatialObject( m_ImageSpatialObject );

   // create reslice plane representation for axial view
   m_CTView1ImageRepresentation = ImageRepresentationType::New();
   m_CTView1ImageRepresentation->RequestSetImageSpatialObject( m_ImageSpatialObject );
   m_CTView1ImageRepresentation->RequestSetReslicePlaneSpatialObject( m_CTView1PlaneSpatialObject );

   //
   m_CTView1PlaneSpatialObject->RequestSetTransformAndParent( identity, m_WorldReference );

   // add reslice plane representation to the 2D view
   m_ViewerGroup->m_CTView1->RequestAddObject( m_CTView1ImageRepresentation );

   // add reslice plane representations to the 3D view
   m_CTView1ImageRepresentation2 = m_CTView1ImageRepresentation->Copy();
   m_ViewerGroup->m_3DView->RequestAddObject( m_CTView1ImageRepresentation2 );

   // make the CTView1 child of CTView1 reslicer plane spatial object
   m_ViewerGroup->m_CTView1->RequestSetTransformAndParent(
      identity, m_CTView1PlaneSpatialObject );

   // make the VideoView also child of CTView1 reslicer plane spatial object
   // todo: see if it's better to set the video view as child of video frame spatial object
   m_ViewerGroup->m_VideoView->RequestSetTransformAndParent(
      identity, m_ImagerToolSpatialObject );

   // set the imager tool spatial object as the driver of the CTView1 plane
   m_CTView1PlaneSpatialObject->RequestSetToolSpatialObject( m_ImagerToolSpatialObject );

   // setup the imager tool representation for the video view
   m_ImagerToolRepresentationForVideoView = MeshRepresentationType::New();
   m_ImagerToolRepresentationForVideoView->RequestSetMeshObject( m_ImagerToolSpatialObject );
   m_ImagerToolRepresentationForVideoView->SetOpacity(1.0);
   m_ImagerToolRepresentationForVideoView->SetColor(1,1,0);

   // setup the imager tool representation for the 3d view
   m_ImagerToolRepresentationFor3DView = m_ImagerToolRepresentationForVideoView->Copy();

   // setup the video frame spatial object
   m_VideoFrame = VideoFrameSpatialObjectType::New();
   m_VideoFrame->SetWidth(640);
   m_VideoFrame->SetHeight(480);
   m_VideoFrame->SetPixelSizeX(0.4); // in mm
   m_VideoFrame->SetPixelSizeY(0.4); // in mm
   m_VideoFrame->SetNumberOfScalarComponents(1);
   m_VideoFrame->Initialize();

   // set transformation between m_VideoFrame and m_ImagerToolSpatialObject according to
   // ultrasound calibration
   m_VideoFrame->RequestSetTransformAndParent( identity, m_ImagerToolSpatialObject );

   // create a video frame representation for the video view
   m_VideoFrameRepresentationForVideoView = VideoFrameRepresentationType::New();
   m_VideoFrameRepresentationForVideoView->RequestSetVideoFrameSpatialObject( m_VideoFrame );
   m_VideoFrameRepresentationForVideoView->SetWindowLevel(m_WindowWidthVideo,m_WindowLevelVideo);
   m_VideoFrameRepresentationForVideoView->SetWindowLevel(m_WindowWidthVideo,m_WindowLevelVideo);

   // create a video frame representation for the 3D view
   m_VideoFrameRepresentationFor3DView = m_VideoFrameRepresentationForVideoView->Copy();

   // setup CTView1
   m_ViewerGroup->m_CTView1->SetRefreshRate( VIEW_2D_REFRESH_RATE );
   m_ViewerGroup->m_CTWidget1->RequestEnableInteractions();
   m_ViewerGroup->m_CTView1->RequestStart();

   // setup Video
   m_ViewerGroup->m_VideoView->SetRefreshRate( VIEW_2D_REFRESH_RATE );
   m_ViewerGroup->m_VideoWidget->RequestEnableInteractions();
   m_ViewerGroup->m_VideoView->RequestStart();

   //add picking observer to CTView1

   m_CTView1PickerObserver = LoadedObserverType::New();
   m_CTView1PickerObserver->SetCallbackFunction( this, &UltrasoundNavigator::CTView1PickingCallback );
   m_ViewerGroup->m_CTView1->AddObserver(
      igstk::CoordinateSystemTransformToEvent(), m_CTView1PickerObserver );

   //finally, reset cameras
   m_ViewerGroup->m_CTView1->RequestResetCamera();
   m_ViewerGroup->m_VideoView->RequestResetCamera();
   m_ViewerGroup->m_3DView->RequestResetCamera();

   m_StateMachine.PushInput( m_SuccessInput);
   m_StateMachine.ProcessInputs();
}

/** -----------------------------------------------------------------
* Load mesh. This method asks for a file with the a mesh (e.g representing a
* segmentation). The file must be in the .msh format (see mesh SpatialObject in ITK)
* Any number of meshes can be loaded. Both, spatial object and representation
* will be kept in a vector.
* -----------------------------------------------------------------
*/
void UltrasoundNavigator::LoadMeshProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::LoadMeshProcessing called...\n" )

   const char*  fileName =
    fl_file_chooser("Select the target mesh file","*.msh", m_ImageDir.c_str());

   if ( !fileName )
    {
     igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::LoadMeshProcessing No directory was selected\n" )
     m_StateMachine.PushInput( m_FailureInput );
     m_StateMachine.ProcessInputs();
     return;
    }

   // setup a mesh reader
   MeshReaderType::Pointer reader = MeshReaderType::New();
   reader->RequestSetFileName( fileName );

   reader->RequestReadObject();

   MeshObjectObserver::Pointer observer = MeshObjectObserver::New();

   reader->AddObserver( igstk::MeshReader::MeshModifiedEvent(), observer);

   reader->RequestGetOutput();

   if( !observer->GotMeshObject() )
   {
       igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::LoadMeshProcessing Could not read the mesh\n" )
       m_StateMachine.PushInput( m_FailureInput);
       m_StateMachine.ProcessInputs();
       return;
   }

   // get the mesh spatial object
   MeshType::Pointer meshSpatialObject = observer->GetMeshObject();

   if (meshSpatialObject.IsNull())
   {
     igstkLogMacro2( m_Logger, DEBUG, "UltrasoundNavigator::LoadMeshProcessing Could not retrieve the mesh\n" )
     m_StateMachine.PushInput( m_FailureInput);
     m_StateMachine.ProcessInputs();
     return;
   }

   // set transform and parent to the mesh spatial object
   igstk::Transform identity;
   identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
   meshSpatialObject->RequestSetTransformAndParent( identity, m_WorldReference );

   // set a random color
   double r = ( ( ( double ) ( std::rand( ) ) ) / ( ( double ) ( RAND_MAX ) ) );
   double g = ( ( ( double ) ( std::rand( ) ) ) / ( ( double ) ( RAND_MAX ) ) );
   double b = ( ( ( double ) ( std::rand( ) ) ) / ( ( double ) ( RAND_MAX ) ) );

   // setup a mesh representation
   MeshRepresentationType::Pointer meshRepresentation = MeshRepresentationType::New();
   meshRepresentation->RequestSetMeshObject( meshSpatialObject );
   meshRepresentation->SetOpacity(0.7);
   meshRepresentation->SetColor(r, g, b);

   // build mesh reslice representation for CTView1
   MeshResliceRepresentationType::Pointer view1ContourRepresentation = MeshResliceRepresentationType::New();
   view1ContourRepresentation->SetOpacity(1);
   view1ContourRepresentation->SetLineWidth(3);
   view1ContourRepresentation->SetColor(r, g, b);
   view1ContourRepresentation->RequestSetMeshObject( meshSpatialObject );
   view1ContourRepresentation->RequestSetReslicePlaneSpatialObject( m_CTView1PlaneSpatialObject );

   // build mesh reslice representation for CTView2
   MeshResliceRepresentationType::Pointer view2ContourRepresentation = MeshResliceRepresentationType::New();
   view2ContourRepresentation->SetOpacity(1);
   view2ContourRepresentation->SetLineWidth(3);
   view2ContourRepresentation->SetColor(r, g, b);
   view2ContourRepresentation->RequestSetMeshObject( meshSpatialObject );
   view2ContourRepresentation->RequestSetReslicePlaneSpatialObject( m_CTView2PlaneSpatialObject );

   // keep the mesh and contours in corresponding vectors
   m_MeshRepresentationVector.push_back( meshRepresentation );

   // we have two mesh contour containers
   m_CTView1MeshResliceRepresentationVector.push_back( view1ContourRepresentation );
   m_CTView2MeshResliceRepresentationVector.push_back( view2ContourRepresentation );
   // we need to do a copy here because the same representation cannot
   // be assigned to two different views
   m_VideoViewMeshResliceRepresentationVector.push_back( view2ContourRepresentation->Copy() );

   // add mesh and contour representations to the corresponding views
   unsigned int currentMeshIndex = m_CTView1MeshResliceRepresentationVector.size()-1;
   m_ViewerGroup->m_CTView1->RequestAddObject( m_CTView1MeshResliceRepresentationVector[currentMeshIndex] );
   m_ViewerGroup->m_CTView2->RequestAddObject( m_CTView2MeshResliceRepresentationVector[currentMeshIndex] );
   m_ViewerGroup->m_VideoView->RequestAddObject( m_VideoViewMeshResliceRepresentationVector[currentMeshIndex] );
   m_ViewerGroup->m_3DView->RequestAddObject( m_MeshRepresentationVector[currentMeshIndex] );

   // finally reset the 3D view
   m_ViewerGroup->m_3DView->RequestResetCamera();

   m_StateMachine.PushInput( m_SuccessInput );
   m_StateMachine.ProcessInputs();
}

/** -----------------------------------------------------------------
* Reslices the views to the picked position
*---------------------------------------------------------------------
*/
void UltrasoundNavigator::SetImagePickingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
                  "UltrasoundNavigator::SetImagePickingProcessing called...\n" )

  ImageSpatialObjectType::PointType point = TransformToPoint( m_PickingTransform );

  if ( m_ImageSpatialObject->IsInside( point ) )
  {
    const double *data = point.GetVnlVector().data_block();
    m_CTView2PlaneSpatialObject->RequestSetCursorPosition( data );

    m_CrossHair->RequestSetCursorPosition( data );

    ImageSpatialObjectType::IndexType index;
    m_ImageSpatialObject->TransformPhysicalPointToIndex( point, index);

    m_xslice = index[0];
    m_yslice = index[1];

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
void UltrasoundNavigator::SetImageFiducialProcessing()
{
    igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::SetImageFiducialProcessing called...\n" )

    ImageSpatialObjectType::PointType point = TransformToPoint( m_PickingTransform );

    if( m_ImageSpatialObject->IsInside( point ) )
    {
      int choice = m_FiducialsPointList->value();

      m_FiducialPointVector[choice]->RequestSetTransformAndParent(
                                              m_PickingTransform, m_WorldReference );

      m_Plan->m_FiducialPoints[choice] = point;

      // Write the updated plan to file
      this->WriteFiducials();

      ImageSpatialObjectType::IndexType index;
      m_ImageSpatialObject->TransformPhysicalPointToIndex( point, index);

      const double *data = point.GetVnlVector().data_block();

      m_CTView2PlaneSpatialObject->RequestSetCursorPosition( data );
      m_CrossHair->RequestSetCursorPosition( data );

      this->ResliceImage( index );
    }
    else
    {
      igstkLogMacro( DEBUG,  "Picked point outside image...\n" )
    }
}

void UltrasoundNavigator::InitializeTrackerProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::InitializeTrackerProcessing called...\n" )

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

  /** Create the controller for the tracker and assign observers to him*/
  m_TrackerController = igstk::TrackerController::New();

  // set logger to the controller
  m_TrackerController->SetLogger(this->GetLogger());

  m_TrackerControllerObserver = TrackerControllerObserver::New();
  m_TrackerControllerObserver->SetParent( this );

  // todo: replace this with igstk observers
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

  m_TrackerController->RequestGetNonReferenceToolList();
  m_TrackerController->RequestGetReferenceTool();

  igstk::Transform identity;
  identity.SetToIdentity(igstk::TimeStamp::GetLongestPossibleTime());

  if ( m_TrackerToolSpatialObject.IsNull() )
  {
    igstkLogMacro2( m_Logger, DEBUG, "Tool spatial object not available\n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
  }

  m_TrackerToolSpatialObject->RequestDetachFromParent();
  m_TrackerToolSpatialObject->RequestSetTransformAndParent( identity, m_TrackerTool );

  /** Connect the scene graph with an identity transform first */
  if ( m_ReferenceTool.IsNotNull() )
  {
    m_ReferenceTool->RequestSetTransformAndParent(identity, m_WorldReference);
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

  m_StateMachine.PushInput( m_SuccessInput );
  m_StateMachine.ProcessInputs();

}

void UltrasoundNavigator::InitializeImagerProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::InitializeImagerProcessing called...\n" )

  m_ImagerConfiguration = new igstk::TerasonImagerConfiguration();

  //set the tool parameters
  igstk::TerasonToolConfiguration toolconfig;

  unsigned int dims[3];
  dims[0] = 640;
  dims[1] = 480;
  dims[2] = 1;

  toolconfig.SetFrameDimensions(dims);
  toolconfig.SetPixelDepth(8);
  std::string deviceName = "Terason2000";
//  toolconfig.SetCalibrationFileName( "Terason2000_Calibration.igstk" );
  toolconfig.SetToolUniqueIdentifier( deviceName );

  m_ImagerConfiguration->RequestAddTool( &toolconfig );
  m_ImagerConfiguration->SetSocketPort( 18944 );

  m_ImagerConfiguration->RequestSetFrequency( IMAGER_DEFAULT_REFRESH_RATE );

  if (!m_ImagerConfiguration)
  {
    std::string errorMessage;
    fl_alert( errorMessage.c_str() );
    fl_beep( FL_BEEP_ERROR );
    igstkLogMacro2( m_Logger, DEBUG, "Imager Initialization error\n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
  }

  /** Create the controller for the imager and assign observers to it*/
  m_ImagerController = igstk::ImagerController::New();

  m_ImagerControllerObserver = ImagerControllerObserver::New();
  m_ImagerControllerObserver->SetParent( this );

  m_ImagerController->AddObserver(igstk::ImagerController::InitializeFailureEvent(),
    m_ImagerControllerObserver );

  m_ImagerController->AddObserver(igstk::ImagerController::StartFailureEvent(),
    m_ImagerControllerObserver );

  m_ImagerController->AddObserver(igstk::ImagerController::StopFailureEvent(),
    m_ImagerControllerObserver );

  m_ImagerController->AddObserver(igstk::ImagerController::ShutdownFailureEvent(),
    m_ImagerControllerObserver );

  m_ImagerController->AddObserver(igstk::ImagerController::RequestImagerEvent(),
    m_ImagerControllerObserver );

  m_ImagerController->AddObserver(igstk::ImagerController::RequestToolsEvent(),
    m_ImagerControllerObserver );

  // Initialize the progress command
  m_SocketProgressCommand = ProgressCommandType::New();
  m_SocketProgressCommand->SetCallbackFunction( this, &UltrasoundNavigator::OnSocketProgressEvent );

  // Provide a progress observer to the imager controller
  m_ImagerController->RequestSetProgressCallback( m_SocketProgressCommand );

  // initialize the imager controller with our image configuration file
  m_ImagerController->RequestInitialize( m_ImagerConfiguration );

  //check that initialization was successful
  if( m_ImagerControllerObserver->Error() )
  {
    std::string errorMessage;
    m_ImagerControllerObserver->GetErrorMessage( errorMessage );
    m_ImagerControllerObserver->ClearError();
    fl_alert( errorMessage.c_str() );
    fl_beep( FL_BEEP_ERROR );
    igstkLogMacro2( m_Logger, DEBUG, "Imager Initialization error\n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
  }

  m_ImagerController->RequestGetImager();
  m_ImagerController->RequestGetToolList();

  igstk::Frame frame;
  frame.SetImagePtr( m_VideoFrame->GetImagePtr(),
                                  igstk::TimeStamp::GetLongestPossibleTime() );

  m_TerasonImagerTool->SetInternalFrame(frame);

  m_StateMachine.PushInput( m_SuccessInput );
  m_StateMachine.ProcessInputs();

}

void UltrasoundNavigator::StartSetTrackerFiducialsProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::StartSetTrackerFiducialsProcessing called...\n" )

  this->DisableAll();

  m_FiducialsPointList->activate();

  m_RegisterButton->color(FL_RED);
  m_RegisterButton->label("Registering...");

  //m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 1, "REGISTERING" );
  //m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor(1, 1.0, 0.0, 0.0);

  //m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 1, "REGISTERING" );
  //m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor(1, 1.0, 0.0, 0.0);

  //m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 1, "REGISTERING" );
  //m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor(1, 1.0, 0.0, 0.0);

//  m_ViewerGroup->RequestUpdateOverlays();

  // first reset the reference tool
  igstk::Transform identity;
  identity.SetToIdentity(igstk::TimeStamp::GetLongestPossibleTime());

  if ( m_ReferenceTool.IsNotNull() )
  {
    m_ReferenceTool->RequestDetachFromParent();
    m_ReferenceTool->RequestSetTransformAndParent(identity, m_WorldReference);
  }

  m_FiducialsPointList->clear();
  m_LandmarksContainer.clear();
  m_AcceptedLandmarksContainer.clear();

  char buf[50];
  for ( unsigned int i=0; i<m_Plan->m_FiducialPoints.size(); i++)
  {
    sprintf( buf, "Fiducial #%d", i+1 );
    m_FiducialsPointList->add(buf);
    RegistrationType::LandmarkTrackerPointType p;
    m_LandmarksContainer.push_back(p);
    m_AcceptedLandmarksContainer.insert( std::pair<unsigned int,bool>(i,false) );
  }

  m_FiducialsPointList->value(0);
  this->RequestChangeSelectedFiducial();

  Fl::check();
}


void UltrasoundNavigator::EndSetTrackerFiducialsProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::EndSetTrackerFiducialsProcessing called...\n" )

  AcceptedLandmarkPointContainerType::const_iterator iter;
  iter = m_AcceptedLandmarksContainer.begin();

  unsigned int numberOfAcceptedLandmarks = 0;
  for (;iter != m_AcceptedLandmarksContainer.end(); iter++)
  {
    if (iter->second)
      numberOfAcceptedLandmarks++;
  }

  igstkLogMacro2( m_Logger, DEBUG,
                    "numberOfAcceptedLandmarks " << numberOfAcceptedLandmarks << "\n" )

  if (numberOfAcceptedLandmarks >= 3)
  {
    m_RegisterButton->label("Register");
    m_RegisterButton->activate();
    m_RegisterButton->color((Fl_Color)55);
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
void UltrasoundNavigator::TrackerRegistrationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::TrackerRegistrationProcessing called...\n" )

  RegistrationType::Pointer registration  = RegistrationType::New();
  registration->RequestResetRegistration();

  for( unsigned int i=0; i< m_LandmarksContainer.size(); i++)
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

       //m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 1, buf );
       //m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor(1, 0.0, 1.0, 0.0);

       //m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 1, buf );
       //m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor(1, 0.0, 1.0, 0.0);

       //m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 1, buf );
       //m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor(1, 0.0, 1.0, 0.0);

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

    if ( m_ReferenceTool.IsNotNull() )
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
void UltrasoundNavigator::StartTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::StartTrackingProcessing called...\n" )

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
void UltrasoundNavigator::StopTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::StopTrackingProcessing called...\n" )

  m_TrackerController->RequestStopTracking( );
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

  m_StateMachine.PushInput( m_SuccessInput );
  m_StateMachine.ProcessInputs();
  return;

}
/** -----------------------------------------------------------------
* Disconnects the tracker and closes the communication port
*---------------------------------------------------------------------
*/
void UltrasoundNavigator::DisconnectTrackerProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::DisconnectTrackerProcessing called...\n" )
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
void UltrasoundNavigator::ConnectImageRepresentation()
{
  // instantiate a plan object
  m_Plan = new igstk::FiducialsPlan;

  // set up fiducual representations
  m_FiducialPointVector.resize(4);
  m_AxialFiducialRepresentationVector.resize(4);
  m_SagittalFiducialRepresentationVector.resize(4);
  m_CoronalFiducialRepresentationVector.resize(4);
  m_3DViewFiducialRepresentationVector.resize(4);

  for (unsigned int i=0; i<4; i++)
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

  // create reslice plane spatial object for CTView2
  m_CTView2PlaneSpatialObject = ReslicerPlaneType::New();
  m_CTView2PlaneSpatialObject->RequestSetReslicingMode( ReslicerPlaneType::Orthogonal );
  m_CTView2PlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Axial );
  m_CTView2PlaneSpatialObject->RequestSetBoundingBoxProviderSpatialObject( m_ImageSpatialObject );

  // create reslice plane representation for sagittal view
  m_CTView2ImageRepresentation = ImageRepresentationType::New();
  m_CTView2ImageRepresentation->RequestSetImageSpatialObject( m_ImageSpatialObject );
  m_CTView2ImageRepresentation->RequestSetReslicePlaneSpatialObject( m_CTView2PlaneSpatialObject );

  // Request information about the slice bounds

  ImageExtentObserver::Pointer extentObserver = ImageExtentObserver::New();

  unsigned int extentObserverID;

  extentObserverID = m_ImageSpatialObject->AddObserver( igstk::ImageExtentEvent(), extentObserver );

  m_ImageSpatialObject->RequestGetImageExtent();

  if( extentObserver->GotImageExtent() )
  {
    const igstk::EventHelperType::ImageExtentType& extent = extentObserver->GetImageExtent();

    const unsigned int zmin = extent.zmin;
    const unsigned int zmax = extent.zmax;
    m_zslice = static_cast< unsigned int > ( (zmin + zmax) / 3.0 );
    m_ViewerGroup->m_SuperiorRightSlider->minimum( zmin );
    m_ViewerGroup->m_SuperiorRightSlider->maximum( zmax );
    m_ViewerGroup->m_SuperiorRightSlider->value( m_zslice );
    m_ViewerGroup->m_SuperiorRightSlider->activate();

    const unsigned int ymin = extent.ymin;
    const unsigned int ymax = extent.ymax;
    m_yslice = static_cast< unsigned int > ( (ymin + ymax) / 3.0 );

    const unsigned int xmin = extent.xmin;
    const unsigned int xmax = extent.xmax;
    m_xslice = static_cast< unsigned int > ( (xmin + xmax) / 3.0 );
  }

  m_ImageSpatialObject->RemoveObserver( extentObserverID );

  // Set up cross hairs
  m_CrossHair = CrossHairType::New();
  m_CrossHair->RequestSetBoundingBoxProviderSpatialObject( m_ImageSpatialObject );

  // buid the cross hair representation and add the cross hair object
  m_CrossHairRepresentationForCTView2 = CrossHairRepresentationType::New();
  m_CrossHairRepresentationForCTView2->SetColor(0,1,0);
  m_CrossHairRepresentationForCTView2->RequestSetCrossHairObject( m_CrossHair );

  m_CrossHairRepresentationFor3DView = m_CrossHairRepresentationForCTView2->Copy();

  // add the cross hair representation to the different views
  m_ViewerGroup->m_CTView2->RequestAddObject( m_CrossHairRepresentationForCTView2 );
  m_ViewerGroup->m_3DView->RequestAddObject( m_CrossHairRepresentationFor3DView );

  // set background color to the views
  m_ViewerGroup->m_CTView1->SetRendererBackgroundColor(0,0,0);
  m_ViewerGroup->m_CTView2->SetRendererBackgroundColor(0,0,0);
  m_ViewerGroup->m_VideoView->SetRendererBackgroundColor(1,1,1);
  m_ViewerGroup->m_3DView->SetRendererBackgroundColor(1,1,1);

  //  Connect the scene graph
  igstk::Transform identity;
  identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  // our principal node in the scene graph: the world reference
  m_WorldReference  = igstk::AxesObject::New();

  // set transform and parent to the image spatial object
  m_ImageSpatialObject->RequestSetTransformAndParent( identity, m_WorldReference );

  // set transform and parent to the image plane reslice spatial objects

  m_CTView2PlaneSpatialObject->RequestSetTransformAndParent( identity, m_WorldReference );

  // important: we set the 2D views as child of coresponding reslicing planes
  // so that they can "follow" the plane and always face their normals (the camera position
  // in the views is not changed though)

  m_ViewerGroup->m_CTView2->RequestSetTransformAndParent(
      identity, m_CTView2PlaneSpatialObject );

  m_ViewerGroup->m_3DView->RequestSetTransformAndParent(
      identity, m_WorldReference );

  // set transform and parent to the cross hair object
  m_CrossHair->RequestSetTransformAndParent( identity, m_WorldReference );

  // set transform and parent to the fiducial points
  m_FiducialPointVector[0]->RequestSetTransformAndParent( identity, m_WorldReference );
  m_FiducialPointVector[1]->RequestSetTransformAndParent( identity, m_WorldReference );
  m_FiducialPointVector[2]->RequestSetTransformAndParent( identity, m_WorldReference );
  m_FiducialPointVector[3]->RequestSetTransformAndParent( identity, m_WorldReference );

  // add reslice plane representations to the orthogonal views
  m_ViewerGroup->m_CTView2->RequestAddObject( m_CTView2ImageRepresentation );

  m_CTView2ImageRepresentation2 = m_CTView2ImageRepresentation->Copy();
  m_ViewerGroup->m_3DView->RequestAddObject( m_CTView2ImageRepresentation2 );

  // set parallel projection in the 2D views
  m_ViewerGroup->m_CTView1->SetCameraParallelProjection(true);
  m_ViewerGroup->m_CTView2->SetCameraParallelProjection(true);
  m_ViewerGroup->m_VideoView->SetCameraParallelProjection(true);

  // reset the cameras in the different views
  m_ViewerGroup->m_CTView1->RequestResetCamera();
  m_ViewerGroup->m_CTView2->RequestResetCamera();
  m_ViewerGroup->m_VideoView->RequestResetCamera();
  m_ViewerGroup->m_3DView->RequestResetCamera();

  // set up view parameters

  m_ViewerGroup->m_CTView2->SetRefreshRate( VIEW_2D_REFRESH_RATE );
  m_ViewerGroup->m_CTWidget2->RequestEnableInteractions();
  m_ViewerGroup->m_CTView2->RequestStart();

  m_ViewerGroup->m_3DView->SetRefreshRate( VIEW_3D_REFRESH_RATE );
  //m_ViewerGroup->m_3DView->RequestAddOrientationBox();
  m_ViewerGroup->m_3DView->RequestStart();

  //m_ViewerGroup->m_AxialView->SetRectangleEnabled(true);
  //m_ViewerGroup->m_SagittalView->SetRectangleEnabled(true);
  //m_ViewerGroup->m_CoronalView->SetRectangleEnabled(true);
  //m_ViewerGroup->m_3DView->SetRectangleEnabled(true);

  // setup picking event observe on CTView1
  m_CTView2PickerObserver = LoadedObserverType::New();
  m_CTView2PickerObserver->SetCallbackFunction( this, &UltrasoundNavigator::CTView2PickingCallback );
  m_ViewerGroup->m_CTView2->AddObserver(
      igstk::CoordinateSystemTransformToEvent(), m_CTView2PickerObserver );

  //  Adding observer for slider bar reslicing event
  m_ManualReslicingObserver = LoadedObserverType::New();
  m_ManualReslicingObserver->SetCallbackFunction( this, &UltrasoundNavigator::ResliceImageCallback );

  m_ViewerGroup->AddObserver( igstk::UltrasoundNavigatorQuadrantViews::ManualReslicingEvent(),
    m_ManualReslicingObserver );

  // Adding observer for key pressed event
  m_KeyPressedObserver = LoadedObserverType::New();
  m_KeyPressedObserver->SetCallbackFunction( this, &UltrasoundNavigator::HandleKeyPressedCallback );

  m_ViewerGroup->AddObserver( igstk::UltrasoundNavigatorQuadrantViews::KeyPressedEvent(),
    m_KeyPressedObserver );

  //Adding observer for mouse pressed event
  m_MousePressedObserver = LoadedObserverType::New();
  m_MousePressedObserver->SetCallbackFunction( this, &UltrasoundNavigator::HandleMousePressedCallback );

  m_ViewerGroup->AddObserver( igstk::UltrasoundNavigatorQuadrantViews::MousePressedEvent(),
    m_MousePressedObserver );

}

/** -----------------------------------------------------------------
*  Read fiducial points from file
*---------------------------------------------------------------------
*/
void UltrasoundNavigator::ReadFiducials()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::ReadFiducials called...\n" )

  igstk::FiducialsPlanIO * reader = new igstk::FiducialsPlanIO;

  m_PlanFilename = m_ImageDir + "/Fiducials.igstk";

  igstkLogMacro2( m_Logger, DEBUG,
                    "Reading fiducials from " << m_PlanFilename << "\n")

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
  for( unsigned int i = 0; i < m_Plan->m_FiducialPoints.size(); i++ )
  {
    sprintf( buf, "Fiducial #%d", i+1 );
    m_FiducialsPointList->add( buf );
    RegistrationType::LandmarkTrackerPointType p;
    m_LandmarksContainer.push_back(p);
    m_AcceptedLandmarksContainer.insert( std::pair<unsigned int,bool>(i,false) );
  }

  m_FiducialsPointList->value(0);
  this->RequestChangeSelectedFiducial();

  delete reader;
}

/** -----------------------------------------------------------------
*  Overwrite the file with the image fiducials
*---------------------------------------------------------------------
*/
void UltrasoundNavigator::WriteFiducials()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::WriteFiducials called...\n" )

  igstk::FiducialsPlanIO * writer = new igstk::FiducialsPlanIO;
  writer->SetFileName( m_PlanFilename );
  writer->SetFiducialsPlan( m_Plan );
  writer->RequestWrite();
  delete writer;
}

/** -----------------------------------------------------------------
*  Switches the currently active view mode
*---------------------------------------------------------------------
*/
void UltrasoundNavigator::RequestChangeSelectedViewMode()
{

  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::RequestChangeSelectedViewMode called...\n" )

  if ( m_ViewModeList->size() == 0)
  {
    return;
  }

  // check which view mode is selected
  int choice = m_ViewModeList->value();

  switch (choice)
  {
    case 0:
      m_CTView1PlaneSpatialObject->RequestSetReslicingMode( ReslicerPlaneType::Oblique );
      m_CTView1PlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::PlaneOrientationWithYAxesNormal );

      m_CTView2PlaneSpatialObject->RequestSetReslicingMode( ReslicerPlaneType::Orthogonal );
      m_CTView2PlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Axial );
      break;

    case 1:
      m_CTView1PlaneSpatialObject->RequestSetReslicingMode( ReslicerPlaneType::Oblique );
      m_CTView1PlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::PlaneOrientationWithYAxesNormal );

      m_CTView2PlaneSpatialObject->RequestSetReslicingMode( ReslicerPlaneType::Orthogonal );
      m_CTView2PlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Sagittal );
      break;

    case 2:
      m_CTView1PlaneSpatialObject->RequestSetReslicingMode( ReslicerPlaneType::Oblique );
      m_CTView1PlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::PlaneOrientationWithYAxesNormal );

      m_CTView2PlaneSpatialObject->RequestSetReslicingMode( ReslicerPlaneType::Orthogonal );
      m_CTView2PlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Coronal );
      break;

  default:
    igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::RequestChangeSelectedViewMode invalid choice \n" )
    return;
  }

  m_ViewerGroup->redraw();
  Fl::check();
}

/** -----------------------------------------------------------------
*  Switches the currently active image fiducial
*---------------------------------------------------------------------
*/
void UltrasoundNavigator::RequestChangeSelectedFiducial()
{

  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::RequestChangeSelectedFiducial called...\n" )

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

  for (unsigned int i=0; i<4; i++)
  {
    m_ViewerGroup->m_CTView2->RequestRemoveObject( m_AxialFiducialRepresentationVector[i] );
    m_ViewerGroup->m_3DView->RequestRemoveObject( m_3DViewFiducialRepresentationVector[i] );
  }

  m_ViewerGroup->m_CTView2->RequestAddObject( m_AxialFiducialRepresentationVector[choice] );
  m_ViewerGroup->m_3DView->RequestAddObject( m_3DViewFiducialRepresentationVector[choice] );

  char buf[50];
  sprintf( buf, "[%.2f, %.2f, %.2f]", point[0], point[1], point[2]);

  /** Display point position as annotation */
  //m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 2, buf );
  //m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor(2, 0.0, 0.0, 1.0);

  //m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 2, buf );
  //m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor(2, 0.0, 0.0, 1.0);

  //m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 2, buf );
  //m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor(2, 0.0, 0.0, 1.0);

  /** Reslice image to the selected point position */
  if( m_ImageSpatialObject->IsInside( point ) )
  {
    ImageSpatialObjectType::IndexType index;
    m_ImageSpatialObject->TransformPhysicalPointToIndex( point, index);
    const double *data = point.GetVnlVector().data_block();

    m_CTView2PlaneSpatialObject->RequestSetCursorPosition( data );

    m_CrossHair->RequestSetCursorPosition( data );
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
void UltrasoundNavigator::ResliceImageCallback( const itk::EventObject & event )
{

  if ( igstk::UltrasoundNavigatorQuadrantViews::ManualReslicingEvent().CheckEvent( &event ) )
  {
    igstk::UltrasoundNavigatorQuadrantViews::ManualReslicingEvent *resliceEvent =
    ( igstk::UltrasoundNavigatorQuadrantViews::ManualReslicingEvent *) & event;
    ImageSpatialObjectType::IndexType index = resliceEvent->Get();

    index[0] = m_xslice;
    index[1] = m_yslice;

    PointType point;
    m_ImageSpatialObject->TransformIndexToPhysicalPoint( index, point );
    const double *data = point.GetVnlVector().data_block();

    m_CTView2PlaneSpatialObject->RequestSetCursorPosition( data );

    m_CrossHair->RequestSetCursorPosition( data );
  }
}

/** -----------------------------------------------------------------
* Callback function for observer listening to a key-pressed event
*---------------------------------------------------------------------
*/
void UltrasoundNavigator::HandleKeyPressedCallback( const itk::EventObject & event )
{
  if ( igstk::UltrasoundNavigatorQuadrantViews::KeyPressedEvent().CheckEvent( &event ) )
  {
    igstk::UltrasoundNavigatorQuadrantViews::KeyPressedEvent *keyPressedEvent =
      ( igstk::UltrasoundNavigatorQuadrantViews::KeyPressedEvent *) & event;
    this->HandleKeyPressed( keyPressedEvent->Get() );
  }
}

/** -----------------------------------------------------------------
* Callback function for observer listening to a mouse-pressed event
*---------------------------------------------------------------------
*/
void UltrasoundNavigator::HandleMousePressedCallback( const itk::EventObject & event )
{
  if ( igstk::UltrasoundNavigatorQuadrantViews::MousePressedEvent().CheckEvent( &event ) )
  {
    igstk::UltrasoundNavigatorQuadrantViews::MousePressedEvent *mousePressedEvent =
      ( igstk::UltrasoundNavigatorQuadrantViews::MousePressedEvent *) & event;
    this->HandleMousePressed( mousePressedEvent->Get() );
  }
}

/** -----------------------------------------------------------------
*  Callback function for picking event in the CTView1 view.
*  Upon receiving a valid picking event, this method will reslice the
*  image to that location.
*---------------------------------------------------------------------
*/
void UltrasoundNavigator::CTView1PickingCallback( const itk::EventObject & event)
{
  if ( igstk::CoordinateSystemTransformToEvent().CheckEvent( &event ) )
  {
    typedef igstk::CoordinateSystemTransformToEvent TransformEventType;
    const TransformEventType * tmevent =
    dynamic_cast< const TransformEventType *>( & event );

    // get the transform from the view to its parent (reslicer plane)
    igstk::CoordinateSystemTransformToResult transformCarrier = tmevent->Get();
    m_PickingTransform = transformCarrier.GetTransform();

    // get the transform from the reslicer plane to its parent (world reference)
    CoordinateSystemTransformObserver::Pointer coordinateObserver =
      CoordinateSystemTransformObserver::New();

    unsigned int obsId = m_CTView1PlaneSpatialObject->AddObserver(
      igstk::CoordinateSystemTransformToEvent(), coordinateObserver );

    m_CTView1PlaneSpatialObject->RequestComputeTransformTo( m_ImageSpatialObject );

    if( coordinateObserver->GotCoordinateSystemTransform() )
    {
      igstk::CoordinateSystemTransformToResult transformToResult = coordinateObserver->GetCoordinateSystemTransform();
      igstk::Transform viewToWorldReferenceTransform = transformToResult.GetTransform();
      m_PickingTransform = igstk::Transform::TransformCompose( viewToWorldReferenceTransform, m_PickingTransform );
    }
    else
    {
      igstkLogMacro2( m_Logger, DEBUG,
                    "Navigator::CTView1PickingCallback could not get coordinate system transform...\n" )
      return;
    }

    m_CTView1PlaneSpatialObject->RemoveObserver( obsId );

    m_StateMachine.PushInput( m_SetPickingPositionInput );
    m_StateMachine.ProcessInputs();
  }
}

/** -----------------------------------------------------------------
*  Callback function for picking event in the CTView2 view.
*  Upon receiving a valid picking event, this method will reslice the
*  image to that location.
*---------------------------------------------------------------------
*/
void UltrasoundNavigator::CTView2PickingCallback( const itk::EventObject & event)
{
  if ( igstk::CoordinateSystemTransformToEvent().CheckEvent( &event ) )
  {
    typedef igstk::CoordinateSystemTransformToEvent TransformEventType;
    const TransformEventType * tmevent =
    dynamic_cast< const TransformEventType *>( & event );

    // get the transform from the view to its parent (reslicer plane)
    igstk::CoordinateSystemTransformToResult transformCarrier = tmevent->Get();
    m_PickingTransform = transformCarrier.GetTransform();

    // get the transform from the reslicer plane to its parent (world reference)
    CoordinateSystemTransformObserver::Pointer coordinateObserver =
      CoordinateSystemTransformObserver::New();

    unsigned int obsId = m_CTView2PlaneSpatialObject->AddObserver(
      igstk::CoordinateSystemTransformToEvent(), coordinateObserver );

    m_CTView2PlaneSpatialObject->RequestComputeTransformTo( m_ImageSpatialObject );

    if( coordinateObserver->GotCoordinateSystemTransform() )
    {
      igstk::CoordinateSystemTransformToResult transformToResult = coordinateObserver->GetCoordinateSystemTransform();
      igstk::Transform viewToWorldReferenceTransform = transformToResult.GetTransform();
      m_PickingTransform = igstk::Transform::TransformCompose( viewToWorldReferenceTransform, m_PickingTransform );
    }
    else
    {
      igstkLogMacro2( m_Logger, DEBUG,
                    "Navigator::CTView2PickingCallback could not get coordinate system transform...\n" )
      return;
    }

    m_CTView2PlaneSpatialObject->RemoveObserver( obsId );

    m_StateMachine.PushInput( m_SetPickingPositionInput );
    m_StateMachine.ProcessInputs();
  }
}

/** -----------------------------------------------------------------
*  Callback function for picking event in the Video view.
*  Upon receiving a valid picking event, this method will reslice the
*  image to that location.
*---------------------------------------------------------------------
*/
void UltrasoundNavigator::VideoViewPickingCallback( const itk::EventObject & event)
{
  if ( igstk::CoordinateSystemTransformToEvent().CheckEvent( &event ) )
  {
    typedef igstk::CoordinateSystemTransformToEvent TransformEventType;
    const TransformEventType * tmevent =
    dynamic_cast< const TransformEventType *>( & event );

    // get the transform from the view to its parent (reslicer plane)
    igstk::CoordinateSystemTransformToResult transformCarrier = tmevent->Get();
    m_PickingTransform = transformCarrier.GetTransform();

    // get the transform from the reslicer plane to its parent (world reference)
    CoordinateSystemTransformObserver::Pointer coordinateObserver =
      CoordinateSystemTransformObserver::New();

    // important: we don't have a reslicer plane for the video
    unsigned int obsId = m_CTView1PlaneSpatialObject->AddObserver(
      igstk::CoordinateSystemTransformToEvent(), coordinateObserver );

    m_CTView1PlaneSpatialObject->RequestComputeTransformTo( m_ImageSpatialObject );

    if( coordinateObserver->GotCoordinateSystemTransform() )
    {
      igstk::CoordinateSystemTransformToResult transformToResult = coordinateObserver->GetCoordinateSystemTransform();
      igstk::Transform viewToWorldReferenceTransform = transformToResult.GetTransform();
      m_PickingTransform = igstk::Transform::TransformCompose( viewToWorldReferenceTransform, m_PickingTransform );
    }
    else
    {
      igstkLogMacro2( m_Logger, DEBUG,
                    "Navigator::VideoViewPickingCallback could not get coordinate system transform...\n" )
      return;
    }

    m_CTView1PlaneSpatialObject->RemoveObserver( obsId );

    m_StateMachine.PushInput( m_SetPickingPositionInput );
    m_StateMachine.ProcessInputs();
  }
}

void UltrasoundNavigator::HandleMousePressed (
  igstk::UltrasoundNavigatorQuadrantViews::MouseCommandType mouseCommand )
{

   if ( mouseCommand.quadrant != 2 )
   {
    m_WindowWidth += mouseCommand.dx * 2;
    if (m_WindowWidth < 1)
      m_WindowWidth = 1;

    m_WindowLevel += mouseCommand.dy * 2;

    if ( m_CTView1ImageRepresentation.IsNotNull() )
      m_CTView1ImageRepresentation->SetWindowLevel( m_WindowWidth, m_WindowLevel );

    if ( m_CTView2ImageRepresentation.IsNotNull() )
      m_CTView2ImageRepresentation->SetWindowLevel( m_WindowWidth, m_WindowLevel );

    if ( m_CTView1ImageRepresentation2.IsNotNull() )
      m_CTView1ImageRepresentation2->SetWindowLevel( m_WindowWidth, m_WindowLevel );

    if ( m_CTView2ImageRepresentation2.IsNotNull() )
      m_CTView2ImageRepresentation2->SetWindowLevel( m_WindowWidth, m_WindowLevel );
   }
   else
   {
      m_WindowWidthVideo += mouseCommand.dx;
      if (m_WindowWidthVideo < 1)
        m_WindowWidthVideo = 1;

      m_WindowLevelVideo += mouseCommand.dy;

      if ( m_VideoFrameRepresentationForVideoView.IsNotNull() )
        m_VideoFrameRepresentationForVideoView->SetWindowLevel(m_WindowWidthVideo,m_WindowLevelVideo);

      if ( m_VideoFrameRepresentationFor3DView.IsNotNull() )
        m_VideoFrameRepresentationFor3DView->SetWindowLevel(m_WindowWidthVideo,m_WindowLevelVideo);
   }
}

void UltrasoundNavigator::HandleKeyPressed (
  igstk::UltrasoundNavigatorQuadrantViews::KeyboardCommandType keyCommand )
{
  switch ( keyCommand.key )
  {
    case 'p': // toggle orthogonal planes visualization
        this->RequestToggleOrthogonalPlanes();
        break;

    case 'r': // reset 3D view
        // reset the cameras in the different views
        m_ViewerGroup->m_CTView1->RequestResetCamera();
        m_ViewerGroup->m_CTView2->RequestResetCamera();
        m_ViewerGroup->m_VideoView->RequestResetCamera();
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
void UltrasoundNavigator::ResliceImage ( IndexType index )
{
//  m_ViewerGroup->m_SuperiorLeftSlider->value( index[2] );
//  m_ViewerGroup->m_SuperiorRightSlider->value( index[0] );
//  m_ViewerGroup->m_InferiorLeftSlider->value( index[1] );

   // todo: fix it so that when the user sets the uppper right view to e.g.
  // sagittal, the slider updates the correct index
  m_ViewerGroup->m_SuperiorRightSlider->value( index[2] );

  m_ViewerGroup->redraw();
  Fl::check();
}

void UltrasoundNavigator::AddImagePlanesTo3DView()
{
    igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::AddImagePlanesTo3DView called...\n" )

  m_ViewerGroup->m_3DView->RequestAddObject( m_CTView1ImageRepresentation2 );
  m_ViewerGroup->m_3DView->RequestAddObject( m_CTView2ImageRepresentation2 );

}

void UltrasoundNavigator::RemoveImagePlanesTo3DView()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::RemoveImagePlanesTo3DView called...\n" )

  m_ViewerGroup->m_3DView->RequestRemoveObject( m_CTView1ImageRepresentation2 );
  m_ViewerGroup->m_3DView->RequestRemoveObject( m_CTView1ImageRepresentation2 );

}

void UltrasoundNavigator::TrackerToolAvailableCallback(const itk::EventObject & event )
{
  m_TrackingSemaphore->color(FL_GREEN);
  m_TrackingSemaphore->label("tracking");
  m_ControlGroup->redraw();
  Fl::check();
}

void UltrasoundNavigator::TrackerToolNotAvailableCallback(const itk::EventObject & event )
{
  m_TrackingSemaphore->color(FL_RED);
  m_TrackingSemaphore->label("not visible");
  m_ControlGroup->redraw();
  Fl::check();
}

void UltrasoundNavigator::ImagerToolAvailableCallback(const itk::EventObject & event )
{
  m_ImagerSemaphore->color(FL_GREEN);
  m_ImagerSemaphore->label("tracking");
  m_ControlGroup->redraw();
  Fl::check();
}

void UltrasoundNavigator::ImagerToolNotAvailableCallback(const itk::EventObject & event )
{
  m_ImagerSemaphore->color(FL_RED);
  m_ImagerSemaphore->label("not visible");
  m_ControlGroup->redraw();
  Fl::check();
}

void UltrasoundNavigator::ReferenceAvailableCallback(const itk::EventObject & event )
{
  m_ReferenceSemaphore->color(FL_GREEN);
  m_ReferenceSemaphore->label("tracking");
  m_ControlGroup->redraw();
  Fl::check();
}

void UltrasoundNavigator::ReferenceNotAvailableCallback(const itk::EventObject & event )
{
  m_ReferenceSemaphore->color(FL_RED);
  m_ReferenceSemaphore->label("not visible");
  m_ControlGroup->redraw();
  Fl::check();
}

void UltrasoundNavigator::TrackerControllerObserver::SetParent( UltrasoundNavigator *p )
{
  m_Parent = p;
}

void UltrasoundNavigator::TrackerControllerObserver::Execute( const itk::Object *caller,
                                                    const itk::EventObject & event )
{
  const itk::Object * constCaller = caller;
  this->Execute(constCaller, event);
}

void
UltrasoundNavigator::TrackerControllerObserver::Execute( itk::Object *caller,
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
  else if ( evt3 )
  {
    igstk::TrackerController::ToolContainerType toolContainer = evt3->Get();
    igstk::TrackerController::ToolContainerType::iterator trackerIter = toolContainer.find(TRACKER_TOOL_NAME);

    if ( trackerIter!=toolContainer.end() )
    {
        m_Parent->m_TrackerTool = (*trackerIter).second;

        // observe tracker tool not available events
        m_Parent->m_TrackerToolNotAvailableObserver = LoadedObserverType::New();
        m_Parent->m_TrackerToolNotAvailableObserver->SetCallbackFunction( m_Parent,
                                                 &UltrasoundNavigator::TrackerToolNotAvailableCallback );

        m_Parent->m_TrackerTool->AddObserver(
         igstk::TrackerToolNotAvailableToBeTrackedEvent(), m_Parent->m_TrackerToolNotAvailableObserver);

        // observe tracker tool available events
        m_Parent->m_TrackerToolAvailableObserver = LoadedObserverType::New();
        m_Parent->m_TrackerToolAvailableObserver->SetCallbackFunction( m_Parent,
                                                 &UltrasoundNavigator::TrackerToolAvailableCallback );

        m_Parent->m_TrackerTool->AddObserver(
         igstk::TrackerToolMadeTransitionToTrackedStateEvent(), m_Parent->m_TrackerToolAvailableObserver);
    }

    igstk::TrackerController::ToolContainerType::iterator imagerIter = toolContainer.find(IMAGER_TOOL_NAME);

    if ( imagerIter!=toolContainer.end() )
    {
        m_Parent->m_ImagerTool = (*imagerIter).second;

        // observer imager tool not available events
        m_Parent->m_ImagerToolNotAvailableObserver = LoadedObserverType::New();
        m_Parent->m_ImagerToolNotAvailableObserver->SetCallbackFunction( m_Parent,
                                                 &UltrasoundNavigator::ImagerToolNotAvailableCallback );
        m_Parent->m_ImagerTool->AddObserver(
         igstk::TrackerToolNotAvailableToBeTrackedEvent(), m_Parent->m_ImagerToolNotAvailableObserver);

        // observer imager tool available events
        m_Parent->m_ImagerToolAvailableObserver = LoadedObserverType::New();
        m_Parent->m_ImagerToolAvailableObserver->SetCallbackFunction( m_Parent,
                                                 &UltrasoundNavigator::ImagerToolAvailableCallback );

        m_Parent->m_ImagerTool->AddObserver(
         igstk::TrackerToolMadeTransitionToTrackedStateEvent(), m_Parent->m_ImagerToolAvailableObserver);
    }
  }
  else if ( evt4 )
  {
    igstk::TrackerController::ToolEntryType entry = evt4->Get();
    if ( entry.first == REFERENCE_NAME )
    {
        m_Parent->m_ReferenceTool = entry.second;

        m_Parent->m_ReferenceNotAvailableObserver = LoadedObserverType::New();
        m_Parent->m_ReferenceNotAvailableObserver->SetCallbackFunction( m_Parent,
                                                 &UltrasoundNavigator::ReferenceNotAvailableCallback );

        m_Parent->m_ReferenceTool->AddObserver(
            igstk::TrackerToolNotAvailableToBeTrackedEvent(), m_Parent->m_ReferenceNotAvailableObserver);

        m_Parent->m_ReferenceAvailableObserver = LoadedObserverType::New();
        m_Parent->m_ReferenceAvailableObserver->SetCallbackFunction( m_Parent,
                                                 &UltrasoundNavigator::ReferenceAvailableCallback );

        m_Parent->m_ReferenceTool->AddObserver(
            igstk::TrackerToolMadeTransitionToTrackedStateEvent(), m_Parent->m_ReferenceAvailableObserver);
    }
  }
}

void
UltrasoundNavigator
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
    this->CenterChildWindowInParentWindow( m_WinProgress );
    }
  else if (progress == 1.0f && m_WinProgress->visible())
    {
    m_WinProgress->hide();
    }

  // Update the screen
  Fl::check();
}

void
UltrasoundNavigator
::OnSocketProgressEvent(itk::Object *source, const itk::EventObject & event)
{
  const igstk::DoubleTypeEvent *evt =
    dynamic_cast< const igstk::DoubleTypeEvent * > (&event);

  if ( evt )
  {
    igstk::EventHelperType::DoubleType progress = evt->Get();

    // Get the value of the progress
    //float progress = reinterpret_cast<igstk::DoubleTypeEvent *>(source)->GetProgress();

    // Update the progress bar and value
    m_OutProgressMeter->value(100 * progress);
    m_OutProgressCounter->value(100 * progress);

    // Show or hide progress bar if necessary
    if(progress < 1.0f && !m_WinProgress->visible())
      {
      m_WinProgress->show();
      this->CenterChildWindowInParentWindow( m_WinProgress );
      }
    else if (progress == 1.0f && m_WinProgress->visible())
      {
      m_WinProgress->hide();
      }

    // Update the screen
    Fl::check();
  }
}

void
UltrasoundNavigator
::RequestPrepareToQuit()
{
  this->RequestStopTracking();
  this->RequestDisconnectTracker();
}

/** -----------------------------------------------------------------
* Starts imaging provided it is initialized and connected to the
* communication port
*---------------------------------------------------------------------
*/
void UltrasoundNavigator::StartImagingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::StartImagingProcessing called...\n" )

  m_ImagerController->RequestStart();

  //check if succeded
  if( m_ImagerControllerObserver->Error() )
  {
    std::string errorMessage;
    m_ImagerControllerObserver->GetErrorMessage( errorMessage );
    m_ImagerControllerObserver->ClearError();
    fl_alert( errorMessage.c_str() );
    fl_beep( FL_BEEP_ERROR );
    igstkLogMacro2( m_Logger, DEBUG, "Imager start error\n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
  }

  m_StateMachine.PushInput( m_SuccessInput );
  m_StateMachine.ProcessInputs();
}

/** -----------------------------------------------------------------
* Stops imaging but keeps the imager connected to the
* communication port
*---------------------------------------------------------------------
*/
void UltrasoundNavigator::StopImagingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::StopImagingProcessing called...\n" )

  // try to stop
  m_ImagerController->RequestStop( );

  //check if succeded
  if( m_ImagerControllerObserver->Error() )
  {
    std::string errorMessage;
    m_ImagerControllerObserver->GetErrorMessage( errorMessage );
    m_ImagerControllerObserver->ClearError();
    fl_alert( errorMessage.c_str() );
    fl_beep( FL_BEEP_ERROR );
    igstkLogMacro2( m_Logger, DEBUG,
      "Imager stop error\n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
  }

  m_StateMachine.PushInput( m_SuccessInput );
  m_StateMachine.ProcessInputs();
  return;

}
/** -----------------------------------------------------------------
* Disconnects the imager and closes the socket
*---------------------------------------------------------------------
*/
void UltrasoundNavigator::DisconnectImagerProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "UltrasoundNavigator::DisconnectImagerProcessing called...\n" )

  // try to disconnect
  m_ImagerController->RequestShutdown( );

  //check if succeded
  if( m_ImagerControllerObserver->Error() )
  {
    std::string errorMessage;
    m_ImagerControllerObserver->GetErrorMessage( errorMessage );
    m_ImagerControllerObserver->ClearError();
    fl_alert( errorMessage.c_str() );
    fl_beep( FL_BEEP_ERROR );
    igstkLogMacro2( m_Logger, DEBUG,
      "Imager disconnect error\n" )

    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
  }

  m_StateMachine.PushInput( m_SuccessInput );
  m_StateMachine.ProcessInputs();

}

void
UltrasoundNavigator::ImagerControllerObserver::SetParent(
  UltrasoundNavigator *p )
{
  m_Parent = p;
}

void
UltrasoundNavigator::ImagerControllerObserver::Execute( const itk::Object *caller,
                                                             const itk::EventObject & event )
{
  const itk::Object * constCaller = caller;
  this->Execute(constCaller, event);
}

void
UltrasoundNavigator::ImagerControllerObserver::Execute( itk::Object *caller,
                                                             const itk::EventObject & event )
{
  const igstk::ImagerController::InitializeFailureEvent *evt1a =
    dynamic_cast< const igstk::ImagerController::InitializeFailureEvent * > (&event);

  const igstk::ImagerController::StartFailureEvent *evt1b =
    dynamic_cast< const igstk::ImagerController::StartFailureEvent * > (&event);

    const igstk::ImagerController::StopFailureEvent *evt1c =
    dynamic_cast< const igstk::ImagerController::StopFailureEvent * > (&event);

  const igstk::ImagerController::RequestImagerEvent *evt2 =
    dynamic_cast< const igstk::ImagerController::RequestImagerEvent * > (&event);

  const igstk::ImagerController::RequestToolsEvent *evt3 =
    dynamic_cast< const igstk::ImagerController::RequestToolsEvent * > (&event);

  const igstk::ImagerController::OpenCommunicationFailureEvent *evt4 =
    dynamic_cast< const igstk::ImagerController::OpenCommunicationFailureEvent * > (&event);

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
  else if ( evt2 )
  {
    m_Parent->m_Imager = evt2->Get();
  }
  else if ( evt3 )
  {
    const std::vector<igstk::ImagerTool::Pointer> &tools = evt3->Get();

    igstkLogMacro2( m_Parent->m_Logger, DEBUG,
                    "UltrasoundNavigator::ImagerControllerObserver found imager tool!\n" )

    m_Parent->m_TerasonImagerTool = tools[0];
  }
  else if ( evt4 )
  {
    m_ErrorOccured = true;
    m_ErrorMessage = evt4->Get();
  }
}

