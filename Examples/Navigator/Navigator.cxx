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
#include "igstkAuroraTrackerConfiguration.h"
#include "igstkPolarisTrackerConfiguration.h"
#include "igstkAscensionTrackerConfiguration.h"

#include "igstkAuroraConfigurationXMLFileReader.h"
#include "igstkPolarisVicraConfigurationXMLFileReader.h"
#include "igstkPolarisSpectraConfigurationXMLFileReader.h"
#include "igstkPolarisHybridConfigurationXMLFileReader.h"
#include "igstkMicronConfigurationXMLFileReader.h"
#include "igstkAscensionConfigurationXMLFileReader.h"
#include "igstkAscension3DGConfigurationXMLFileReader.h"

#define VIEW_2D_REFRESH_RATE 10
#define VIEW_3D_REFRESH_RATE 10
// name of the tool that is going to drive the reslicing
#define DRIVING_TOOL_NAME "tool"
// name of the tool that is going to be used as dynamic reference
#define REFERENCE_NAME "reference" 

/** ---------------------------------------------------------------
*     Constructor
* -----------------------------------------------------------------
*/
Navigator::Navigator() : m_StateMachine(this)
{
  std::srand( 5 );

  m_TrackerConfiguration = NULL;

  m_ImageDir = "";

  m_ImagePlanesIn3DViewEnabled = true;
  m_AxialViewInitialized = false;
  m_SagittalViewInitialized = false;
  m_CoronalViewInitialized = false;
  m_ModifyImageFiducialsEnabled = false;  

  m_TrackerRMS = 0.0;
  m_WindowWidth = 542;
  m_WindowLevel = 52;

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


  /** Machine States*/

  igstkAddStateMacro( Initial );
  igstkAddStateMacro( LoadingImage );
  igstkAddStateMacro( ConfirmingImagePatientName );
  igstkAddStateMacro( ImageReady );
  igstkAddStateMacro( LoadingToolSpatialObject ); 
  igstkAddStateMacro( LoadingMesh ); 
  igstkAddStateMacro( SettingImageFiducials );
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

  /** Machine Inputs*/
  
  igstkAddInputMacro( Success );
  igstkAddInputMacro( Failure );
  igstkAddInputMacro( LoadImage );
  igstkAddInputMacro( ConfirmImagePatientName );
  igstkAddInputMacro( LoadMesh );
  igstkAddInputMacro( LoadToolSpatialObject );
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
  igstkAddTransitionMacro( Initial, LoadMesh, 
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
  igstkAddTransitionMacro( LoadingImage, ConfirmImagePatientName, 
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, LoadMesh, 
                           LoadingImage, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingImage, LoadToolSpatialObject, 
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
  igstkAddTransitionMacro( ConfirmingImagePatientName, ConfirmImagePatientName, 
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, LoadMesh, 
                           ConfirmingImagePatientName, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfirmingImagePatientName, LoadToolSpatialObject, 
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
  
  igstkAddTransitionMacro( ImageReady, LoadMesh,
                           LoadingMesh, LoadMesh );
  igstkAddTransitionMacro( ImageReady, LoadToolSpatialObject,
                           LoadingToolSpatialObject, LoadToolSpatialObject );
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
  
   
  /** LoadingToolSpatialObject State */

  igstkAddTransitionMacro( LoadingToolSpatialObject, Success,
                           ImageReady, ReportSuccessToolSpatialObjectLoaded );

  igstkAddTransitionMacro( LoadingToolSpatialObject, Failure,
                           ImageReady, ReportFailuredToolSpatialObjectLoaded );

  //complete table for state: LoadingMesh

  igstkAddTransitionMacro( LoadingToolSpatialObject, LoadImage, 
                           LoadingToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingToolSpatialObject, ConfirmImagePatientName, 
                           LoadingToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingToolSpatialObject, LoadMesh,
                           LoadingToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingToolSpatialObject, LoadToolSpatialObject,
                           LoadingToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingToolSpatialObject, StartSetImageFiducials, 
                           LoadingToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingToolSpatialObject, SetPickingPosition, 
                           LoadingToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingToolSpatialObject, EndSetImageFiducials, 
                           LoadingToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingToolSpatialObject, RegisterTracker, 
                           LoadingToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingToolSpatialObject, StartSetTrackerFiducials, 
                           LoadingToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingToolSpatialObject, AcceptTrackerFiducial, 
                           LoadingToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingToolSpatialObject, EndSetTrackerFiducials, 
                           LoadingToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingToolSpatialObject, ConfigureTracker, 
                           LoadingToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingToolSpatialObject, InitializeTracker, 
                           LoadingToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingToolSpatialObject, StartTracking, 
                           LoadingToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingToolSpatialObject, StopTracking, 
                           LoadingToolSpatialObject, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingToolSpatialObject, DisconnectTracker, 
                           LoadingToolSpatialObject, ReportInvalidRequest );
  
  /** LoadingMesh State */

  igstkAddTransitionMacro( LoadingMesh, Success,
                           ImageReady, ReportSuccessMeshLoaded );

  igstkAddTransitionMacro( LoadingMesh, Failure,
                           ImageReady, ReportFailuredMeshLoaded );

  //complete table for state: LoadingMesh

  igstkAddTransitionMacro( LoadingMesh, LoadImage, 
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, ConfirmImagePatientName, 
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, LoadMesh, 
                           LoadingMesh, ReportInvalidRequest );
  igstkAddTransitionMacro( LoadingMesh, LoadToolSpatialObject, 
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
  igstkAddTransitionMacro( SettingImageFiducials, LoadToolSpatialObject, 
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
  igstkAddTransitionMacro( ConfiguringTracker, ConfirmImagePatientName, 
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, LoadMesh, 
                           ConfiguringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( ConfiguringTracker, LoadToolSpatialObject, 
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
  igstkAddTransitionMacro( TrackerConfigurationReady, ConfirmImagePatientName, 
                           TrackerConfigurationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerConfigurationReady, LoadMesh, 
                           TrackerConfigurationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerConfigurationReady, LoadToolSpatialObject, 
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
  igstkAddTransitionMacro( InitializingTracker, ConfirmImagePatientName, 
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, LoadMesh, 
                           InitializingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingTracker, LoadToolSpatialObject, 
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
  igstkAddTransitionMacro( TrackerInitializationReady, ConfirmImagePatientName, 
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, LoadMesh, 
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, LoadToolSpatialObject, 
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
  igstkAddTransitionMacro( SettingTrackerFiducials, ConfirmImagePatientName, 
                           SettingTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingTrackerFiducials, LoadMesh, 
                           SettingTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( SettingTrackerFiducials, LoadToolSpatialObject, 
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
  igstkAddTransitionMacro( EndingSetTrackerFiducials, ConfirmImagePatientName, 
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, LoadMesh, 
                           EndingSetTrackerFiducials, ReportInvalidRequest );
  igstkAddTransitionMacro( EndingSetTrackerFiducials, LoadToolSpatialObject, 
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
  igstkAddTransitionMacro( TrackerFiducialsReady, ConfirmImagePatientName, 
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, LoadMesh, 
                           TrackerFiducialsReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerFiducialsReady, LoadToolSpatialObject, 
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
  igstkAddTransitionMacro( RegisteringTracker, ConfirmImagePatientName, 
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, LoadMesh, 
                           RegisteringTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( RegisteringTracker, LoadToolSpatialObject, 
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
  igstkAddTransitionMacro( AcceptingRegistration, ConfirmImagePatientName, 
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, LoadMesh, 
                           AcceptingRegistration, ReportInvalidRequest );
  igstkAddTransitionMacro( AcceptingRegistration, LoadToolSpatialObject, 
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
  igstkAddTransitionMacro( RegistrationReady, ConfirmImagePatientName, 
                           RegistrationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( RegistrationReady, LoadMesh, 
                           RegistrationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( RegistrationReady, LoadToolSpatialObject, 
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
  igstkAddTransitionMacro( StartingTracker, ConfirmImagePatientName, 
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, LoadMesh, 
                           StartingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StartingTracker, LoadToolSpatialObject, 
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
  igstkAddTransitionMacro( Tracking, ConfirmImagePatientName, 
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, LoadMesh, 
                           Tracking, ReportInvalidRequest );
  igstkAddTransitionMacro( Tracking, LoadToolSpatialObject, 
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
  igstkAddTransitionMacro( StoppingTracker, ConfirmImagePatientName, 
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, LoadMesh, 
                           StoppingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( StoppingTracker, LoadToolSpatialObject, 
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
  igstkAddTransitionMacro( DisconnectingTracker, ConfirmImagePatientName, 
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, LoadMesh, 
                           DisconnectingTracker, ReportInvalidRequest );
  igstkAddTransitionMacro( DisconnectingTracker, LoadToolSpatialObject, 
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
Navigator::~Navigator()
{  

}

void Navigator::ConfigureTrackerProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "BronchoscopyNavigator::ConfigureTrackerProcessing called...\n" )
 
  const char*  fileName = 
    fl_file_chooser("Select a tracker configuration file","*.xml", 
                                                     "auroraConfiguration.xml");

  if ( fileName == NULL )
  {
      igstkLogMacro2( m_Logger, DEBUG, "Navigator::ConfigureTrackerProcessing \
      none file was selected or operation canceled...\n" )
      m_StateMachine.PushInput( m_FailureInput );
      m_StateMachine.ProcessInputs();
      return;
  }

  const unsigned int NUM_TRACKER_TYPES = 7;
  igstk::TrackerConfigurationXMLFileReaderBase::Pointer
       trackerConfigurationXMLReaders[NUM_TRACKER_TYPES];
  trackerConfigurationXMLReaders[0] =
       igstk::PolarisVicraConfigurationXMLFileReader::New();
  trackerConfigurationXMLReaders[1] =
       igstk::PolarisSpectraConfigurationXMLFileReader::New();
  trackerConfigurationXMLReaders[2] =
       igstk::PolarisHybridConfigurationXMLFileReader::New();
  trackerConfigurationXMLReaders[3] =
       igstk::AuroraConfigurationXMLFileReader::New();
  trackerConfigurationXMLReaders[4] =
       igstk::MicronConfigurationXMLFileReader::New();
  trackerConfigurationXMLReaders[5] =
       igstk::AscensionConfigurationXMLFileReader::New();
  trackerConfigurationXMLReaders[6] = 
    igstk::Ascension3DGConfigurationXMLFileReader::New();

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

  TrackerConfigurationObserver::Pointer tco = 
                                            TrackerConfigurationObserver::New();

  for( unsigned int i=0; i<NUM_TRACKER_TYPES; i++ )
  {
   //setting the xml reader always succeeds so I don't
   //observe the success event
   trackerConfigReader->RequestSetReader( trackerConfigurationXMLReaders[i] );

   trackerConfigReader->RequestRead();

   if( rfso->GotUnexpectedTrackerType() )
   {
     rfso->Reset();
   }
   else if( rfso->GotFailure() && !rfso->GotUnexpectedTrackerType() )
   {
    //throw ExceptionWithMessage( rfso->GetFailureMessage() );
    std::string errorMessage = rfso->GetFailureMessage();
    fl_alert( "%s\n", errorMessage.c_str() );
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
       igstk::TrackerConfigurationFileReader::TrackerConfigurationDataEvent(), 
                                                                          tco );
     trackerConfigReader->RequestGetData();

     if( tco->GotTrackerConfiguration() )
     {
       m_TrackerConfiguration = tco->GetTrackerConfiguration();
       m_StateMachine.PushInput( m_SuccessInput );
     }
     else
     {
       igstkLogMacro2( m_Logger, DEBUG, 
                                 "Could not get tracker configuration error\n" )
       m_StateMachine.PushInput( m_FailureInput );
     }

     m_StateMachine.ProcessInputs();
     return; 
   }
   else
   {
    // just to complete all possibilities
       igstkLogMacro2( m_Logger, DEBUG, 
                                  "Very strange tracker configuration error\n" )
       m_StateMachine.PushInput( m_FailureInput );
       m_StateMachine.ProcessInputs();
       return;
   }
  } // for
}

void Navigator::RequestLoadImage()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::RequestLoadImage called...\n" )
  m_StateMachine.PushInput( m_LoadImageInput );
  m_StateMachine.ProcessInputs();
}

void Navigator::RequestLoadMesh()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::RequestLoadMesh called...\n" )
  m_StateMachine.PushInput( m_LoadMeshInput );
  m_StateMachine.ProcessInputs();
}

void Navigator::RequestLoadToolSpatialObject()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "Navigator::RequestLoadToolSpatialObject called...\n" )
  m_StateMachine.PushInput( m_LoadToolSpatialObjectInput );
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

//  m_ViewerGroup->RequestUpdateOverlays();

  for (unsigned int i=0; i<4; i++)
  {
    m_ViewerGroup->m_AxialView->RequestRemoveObject( 
                                       m_AxialFiducialRepresentationVector[i] );
    m_ViewerGroup->m_SagittalView->RequestRemoveObject( 
                                    m_SagittalFiducialRepresentationVector[i] );
    m_ViewerGroup->m_CoronalView->RequestRemoveObject( 
                                     m_CoronalFiducialRepresentationVector[i] );
    m_ViewerGroup->m_3DView->RequestRemoveObject( 
                                      m_3DViewFiducialRepresentationVector[i] );
  }

  this->DisableAll();

  m_LoadMeshButton->activate();
  m_FiducialsPointList->activate();
  m_ModifyFiducialsButton->activate();
  m_ConfigureTrackerButton->activate();

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
Navigator::ReportSuccessToolSpatialObjectLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportSuccessToolSpatialObjectLoadedProcessing called...\n");    

  this->RequestConfigureTracker();
}

/** Method to be invoked on failured spatial object loading */
void 
Navigator::ReportFailuredToolSpatialObjectLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportFailuredToolSpatialObjectLoadedProcessing called...\n");

}

/** Method to be invoked on successful target mesh loading */
void 
Navigator::ReportSuccessMeshLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportSuccessMeshLoadedProcessing called...\n");
}

/** Method to be invoked on failured target mesh loading */
void 
Navigator::ReportFailuredMeshLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportFailuredMeshLoadedProcessing called...\n");
}

/** Method to be invoked on successful start set image fiducials */
void 
Navigator::ReportSuccessStartSetImageFiducialsProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportSuccessStartSetImageFiducialsProcessing called...\n");
  m_ModifyFiducialsButton->color(FL_YELLOW);

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

//  m_ViewerGroup->RequestUpdateOverlays();

  m_ModifyFiducialsButton->color((Fl_Color)55);

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

  for (unsigned int i=0; i<4; i++)
  {
    m_ViewerGroup->m_AxialView->RequestRemoveObject( 
                                       m_AxialFiducialRepresentationVector[i] );
    m_ViewerGroup->m_SagittalView->RequestRemoveObject( 
                                    m_SagittalFiducialRepresentationVector[i] );
    m_ViewerGroup->m_CoronalView->RequestRemoveObject( 
                                     m_CoronalFiducialRepresentationVector[i] );
    m_ViewerGroup->m_3DView->RequestRemoveObject( 
                                      m_3DViewFiducialRepresentationVector[i] );
  }

  this->RequestTrackerRegistration();
}

void 
Navigator::AcceptTrackerFiducialProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "AcceptTrackerFiducialProcessing called...\n");

  typedef igstk::TransformObserver ObserverType;
  ObserverType::Pointer transformObserver = ObserverType::New();
  transformObserver->ObserveTransformEventsFrom( m_TrackerTool );
  transformObserver->Clear();
  m_TrackerTool->RequestComputeTransformTo( m_WorldReference );

  if ( !transformObserver->GotTransform() )
    return;
         //check that the transform is indeed valid
  igstk::Transform transform = transformObserver->GetTransform();
  if( !transform.IsValidNow() )
    return;

  int n = m_FiducialsPointList->value();
  int m = m_FiducialsPointList->size();
  m_LandmarksContainer[n] = TransformToPoint( transform );

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

  std::string errorMessage;
  errorMessage = "Could not configure tracker device";
  fl_alert( "%s\n", errorMessage.c_str() );
  fl_beep( FL_BEEP_ERROR );
}

/** Method to be invoked on failured tracker initialization */
void 
Navigator::ReportFailureTrackerInitializationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportFailureTrackerConfigurationProcessing called...\n");

  std::string errorMessage;
  errorMessage = "Could not initialize tracker device";
  fl_alert( "%s\n", errorMessage.c_str() );
  fl_beep( FL_BEEP_ERROR );
}


/** Method to be invoked on successful tracker initialization */
void 
Navigator::ReportSuccessTrackerInitializationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportSuccessTrackerInitializationProcessing called...\n");

  this->DisableAll();

  m_RegisterButton->activate();

  this->RequestStartSetTrackerFiducials();
}

/** Method to be invoked on successful registration acceptance */
void 
Navigator::ReportSuccessAcceptingRegistrationProcessing()
{
    igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportSuccessAcceptingRegistration called...\n");  

  // add the tool object to the image planes
  m_AxialPlaneSpatialObject->RequestSetToolSpatialObject( m_ToolSpatialObject );
  m_SagittalPlaneSpatialObject->RequestSetToolSpatialObject( 
                                                          m_ToolSpatialObject );
  m_CoronalPlaneSpatialObject->RequestSetToolSpatialObject( 
                                                          m_ToolSpatialObject );

  m_CrossHair->RequestSetToolSpatialObject( m_ToolSpatialObject ); 
       
  igstk::Transform identity;
  identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  // setup a tool projection spatial object
  m_ToolProjection = ToolProjectionType::New();
  m_ToolProjection->SetSize(150);
  m_ToolProjection->RequestSetTransformAndParent( identity, m_WorldReference );

  // setup axial tool projection
  m_AxialToolProjectionRepresentation = ToolProjectionRepresentationType::New();
  m_AxialToolProjectionRepresentation->RequestSetToolProjectionObject( 
                                                             m_ToolProjection );
  m_AxialToolProjectionRepresentation->RequestSetReslicePlaneSpatialObject( 
                                                    m_AxialPlaneSpatialObject );
  m_AxialToolProjectionRepresentation->SetColor( 1,1,0 );

  // setup sagittal tool projection
  m_SagittalToolProjectionRepresentation = 
                                        ToolProjectionRepresentationType::New();
  m_SagittalToolProjectionRepresentation->RequestSetToolProjectionObject( 
                                                             m_ToolProjection );
  m_SagittalToolProjectionRepresentation->RequestSetReslicePlaneSpatialObject( 
                                                 m_SagittalPlaneSpatialObject );
  m_SagittalToolProjectionRepresentation->SetColor( 1,1,0 );

  // setup coronal tool projection
  m_CoronalToolProjectionRepresentation = 
                                        ToolProjectionRepresentationType::New();
  m_CoronalToolProjectionRepresentation->RequestSetToolProjectionObject( 
                                                             m_ToolProjection );
  m_CoronalToolProjectionRepresentation->RequestSetReslicePlaneSpatialObject( 
                                                  m_CoronalPlaneSpatialObject );
  m_CoronalToolProjectionRepresentation->SetColor( 1,1,0 );

  // add tool representation to the 3D view
  m_ViewerGroup->m_3DView->RequestAddObject( m_ToolRepresentation );

  m_ViewerGroup->m_AxialView->RequestAddObject( 
                                          m_AxialToolProjectionRepresentation );
  m_ViewerGroup->m_SagittalView->RequestAddObject( 
                                       m_SagittalToolProjectionRepresentation );
  m_ViewerGroup->m_CoronalView->RequestAddObject( 
                                        m_CoronalToolProjectionRepresentation );
 
  // reset the cameras in the different views
  m_ViewerGroup->m_AxialView->RequestResetCamera();
  m_ViewerGroup->m_SagittalView->RequestResetCamera();
  m_ViewerGroup->m_CoronalView->RequestResetCamera();
  m_ViewerGroup->m_3DView->RequestResetCamera();

  this->DisableAll();

  m_ViewModeList->activate();
}

/** Method to be invoked on failure registration acceptance */
void 
Navigator::ReportFailureAcceptingRegistrationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
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
Navigator::ReportSuccessTrackerRegistrationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
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
  
  m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 1, 
                                                               "DISCONNECTED" );
  m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor(1, 1.0, 0.0, 0.0);

  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 1, 
                                                               "DISCONNECTED" );
  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor(1, 1.0, 0.0, 0);

  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 1, 
                                                               "DISCONNECTED" );
  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor(1, 1.0, 0.0, 0.0);

}

/** Method to be invoked on successful tracker start */
void 
Navigator::ReportSuccessStartTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportSuccessStartTrackingProcessing called...\n")

  char buf[50];
  sprintf( buf, "TRACKING (%.2f)", m_TrackerRMS);

  m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 1, buf );
  m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor( 1, 0.0, 1.0, 0.0 );

  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 1, buf );
  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor( 1, 0.0, 1.0, 0);

  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 1, buf );
  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor( 1, 0.0, 1.0, 0);

//  m_ViewerGroup->RequestUpdateOverlays();
  
  for (unsigned int i=0; i<4; i++)
  {
    m_AxialFiducialRepresentationVector[i]->SetColor( 1.0, 0.0, 0.0 );
    m_SagittalFiducialRepresentationVector[i]->SetColor( 1.0, 0.0, 0.0 );
    m_CoronalFiducialRepresentationVector[i]->SetColor( 1.0, 0.0, 0.0 );
  }

  Fl::check();
}


/** Method to be invoked on failured tracker start */
void 
Navigator::ReportFailureStartTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::Navigator::"
                 "ReportFailureStartTrackingProcessing called...\n")
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
 
  m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 1, "STOPPED");
  m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor(1, 0.0, 1.0, 1.0);

  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 1,
                                                                     "STOPPED");
  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor(1, 0.0, 1.0, 1);

  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 1, 
                                                                     "STOPPED");
  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor(1, 0.0, 1.0, 1.0); 

//  m_ViewerGroup->RequestUpdateOverlays();
  
//  m_RunStopButton->label("Run");

  Fl::check();
}


void Navigator::LoadImageProcessing()
{
    igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::LoadImageProcessing called...\n" )

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

   /** Setup image reader  */  
   m_ImageReader        = ImageReaderType::New();
   m_ImageReader->SetLogger( this->GetLogger() );
   m_ImageReader->SetGlobalWarningDisplay(true);
   m_ImageReader->DebugOn();

   /** Build itk progress command to assess image load progress */  
   m_ProgressCommand = ProgressCommandType::New();

   /** Set the callback to the itk progress command */
   m_ProgressCommand->SetCallbackFunction(this, &Navigator::OnITKProgressEvent);

   // Provide a progress observer to the image reader      
   m_ImageReader->RequestSetProgressCallback( m_ProgressCommand );

   //Add observer for invalid directory 
   DICOMImageReaderInvalidDirectoryNameErrorObserver::Pointer didcb = 
                    DICOMImageReaderInvalidDirectoryNameErrorObserver::New();
   m_ImageReader->AddObserver( 
          igstk::DICOMImageDirectoryIsNotDirectoryErrorEvent(), didcb );

   //Add observer for a non-existing directory 
   DICOMImageReaderNonExistingDirectoryErrorObserver::Pointer dndcb = 
                      DICOMImageReaderNonExistingDirectoryErrorObserver::New();
   m_ImageReader->AddObserver( 
          igstk::DICOMImageDirectoryDoesNotExistErrorEvent(), dndcb );
    
   //Add observer for a an empty directory name (null string) 
   DICOMImageReaderEmptyDirectoryErrorObserver::Pointer decb = 
                      DICOMImageReaderEmptyDirectoryErrorObserver::New();
   m_ImageReader->AddObserver( igstk::DICOMImageDirectoryEmptyErrorEvent(), 
                                                                         decb );

   //Add observer for a directory which does not have enough number of files 
   DICOMImageDirectoryNameDoesNotHaveEnoughFilesErrorObserver::Pointer ddhefcb =
            DICOMImageDirectoryNameDoesNotHaveEnoughFilesErrorObserver::New();
   m_ImageReader->AddObserver( 
      igstk::DICOMImageDirectoryDoesNotHaveEnoughFilesErrorEvent(), ddhefcb );

   //Add observer for a directory containing non-DICOM files 
   DICOMImageDirectoryDoesNotContainValidDICOMSeriesErrorObserver::Pointer 
   disgcb = 
          DICOMImageDirectoryDoesNotContainValidDICOMSeriesErrorObserver::New();
   m_ImageReader->AddObserver( 
      igstk::DICOMImageSeriesFileNamesGeneratingErrorEvent(), disgcb );
  
   //Add observer for reading invalid/corrupted dicom files 
   DICOMImageInvalidErrorObserver::Pointer dircb = 
                      DICOMImageInvalidErrorObserver::New();
   m_ImageReader->AddObserver( igstk::DICOMImageReadingErrorEvent(), dircb );

   // Set directory
   m_ImageReader->RequestSetDirectory( directoryName );

   if( didcb->GotDICOMImageReaderInvalidDirectoryNameError() )
    {
    igstkLogMacro2( m_Logger, DEBUG, 
                             "Got DICOMImageReaderInvalidDirectoryNameError\n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
    }

   if( dndcb->GotDICOMImageReaderNonExistingDirectoryError() )
    {
    igstkLogMacro2( m_Logger, DEBUG, 
                             "Got DICOMImageReaderNonExistingDirectoryError\n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
    }

   if( decb->GotDICOMImageReaderEmptyDirectoryError() )
    {
    igstkLogMacro2( m_Logger, DEBUG, 
                                   "Got DICOMImageReaderEmptyDirectoryError\n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
    }

   if( ddhefcb->GotDICOMImageDirectoryNameDoesNotHaveEnoughFilesError() )
    {
    igstkLogMacro2( m_Logger, DEBUG, 
              "Got DICOMImageDirectoryNameDoesNotHaveEnoughFilesError\n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
    }

   if( disgcb->GotDICOMImageDirectoryDoesNotContainValidDICOMSeriesError() )
    {
    igstkLogMacro2( m_Logger, DEBUG, 
                "Got DICOMImageDirectoryDoesNotContainValidDICOMSeriesError\n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
    }

   // Read Image
   m_ImageReader->RequestReadImage();

   m_ImageObserver = ImageObserver::New();
   m_ImageReader->AddObserver(ImageReaderType::ImageModifiedEvent(),
                                                    m_ImageObserver);

   m_ImageReader->RequestGetImage();

   if(!m_ImageObserver->GotImage())
   { 
     std::string errorMessage;
     errorMessage = "Could not open image";
     fl_alert( "%s\n", errorMessage.c_str() );
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
void Navigator::ConfirmPatientNameProcessing()
{
   igstkLogMacro2( m_Logger, DEBUG, 
                  "Navigator::ConfirmPatientNameProcessing called...\n" )

   // ask the user to confirm patient's name

   if ( m_ImageReader->FileSuccessfullyRead() )
   {
        m_PatientNameLabel->value( m_ImageReader->GetPatientName().c_str() );
   }
   else
   {
     fl_beep( FL_BEEP_ERROR );
     igstkLogMacro2( m_Logger, DEBUG, 
     "Navigator::ConfirmPatientNameProcessing \
     file was not successfully read!\n" )
     m_StateMachine.PushInput( m_FailureInput);
     m_StateMachine.ProcessInputs();
     return;
   }
   
   m_PatientNameWindow->show();
   this->CenterChildWindowInParentWindow( m_PatientNameWindow );
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

  m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText(3, 
                                                                  "AXIAL VIEW");
  m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor(3, 1.0, 1.0, 1.0);
  m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontSize(3, 12);

  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText(3, 
                                                               "SAGITTAL VIEW");
  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor(3, 1.0, 1.0, 1);
  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontSize(3, 12);

  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText(3, 
                                                                "CORONAL VIEW");
  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor(3, 1.0, 1.0, 1.0);
  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontSize(3, 12);

//  m_ViewerGroup->RequestUpdateOverlays();

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
* Load tool spatial object mesh. This method asks for a file with the 
* spatial object mesh in the .msh format (see mesh SpatialObject in ITK)
*  -----------------------------------------------------------------
*/
void Navigator::LoadToolSpatialObjectProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::LoadToolSpatialObjectProcessing called...\n" )

   const char*  fileName = 
    fl_file_chooser("Chose a tool spatial object mesh", "*.msh", "");

   if ( !fileName )
    {
     igstkLogMacro2( m_Logger, DEBUG, 
           "Navigator::LoadToolSpatialObjectProcessing No file was selected\n" )
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
       igstkLogMacro2( m_Logger, DEBUG, 
               "Navigator::LoadToolSpatialObjectProcessing cannot read mesh\n" )
       m_StateMachine.PushInput( m_FailureInput );
       m_StateMachine.ProcessInputs();
       return;
   }

   m_ToolSpatialObject = observer->GetMeshObject();

   /*
    // build a tool spatial object using a cylinder spatial object
    m_ToolSpatialObject = CylinderType::New();  
    m_ToolSpatialObject->SetRadius( 1.0 );
    m_ToolSpatialObject->SetHeight( 150 );
  */

    m_ToolRepresentation = MeshRepresentationType::New();
    m_ToolRepresentation->RequestSetMeshObject( m_ToolSpatialObject );
    m_ToolRepresentation->SetOpacity(1.0);
    m_ToolRepresentation->SetColor(0,0,1);

   m_StateMachine.PushInput( m_SuccessInput);
   m_StateMachine.ProcessInputs();
}

/** -----------------------------------------------------------------
* Load mesh. This method asks for a file with the a mesh (e.g representing a 
* segmentation). The file must be in the .msh format (see mesh SpatialObject in 
* ITK)
* Any number of meshes can be loaded. Both, spatial object and representation 
* will be kept in a vector.
* -----------------------------------------------------------------
*/
void Navigator::LoadMeshProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::LoadMeshProcessing called...\n" )

   const char*  fileName = 
    fl_file_chooser("Select the target mesh file","*.msh", m_ImageDir.c_str());

   if ( !fileName )
    {
     igstkLogMacro2( m_Logger, DEBUG, 
                   "Navigator::LoadMeshProcessing No directory was selected\n" )
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
       igstkLogMacro2( m_Logger, DEBUG, 
                     "Navigator::LoadMeshProcessing Could not read the mesh\n" )
       m_StateMachine.PushInput( m_FailureInput);
       m_StateMachine.ProcessInputs();
       return;
   }

   // get the mesh spatial object
   MeshType::Pointer meshSpatialObject = observer->GetMeshObject();
    
   if (meshSpatialObject.IsNull())
   {
     igstkLogMacro2( m_Logger, DEBUG, 
                 "Navigator::LoadMeshProcessing Could not retrieve the mesh\n" )
     m_StateMachine.PushInput( m_FailureInput);
     m_StateMachine.ProcessInputs();
     return;
   }

   // set transform and parent to the mesh spatial object
   igstk::Transform identity;
   identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
   meshSpatialObject->RequestSetTransformAndParent( identity, m_WorldReference);
   
   // set a random color
   double r = ( ( ( double ) ( std::rand( ) ) ) / ( ( double ) ( RAND_MAX ) ) );
   double g = ( ( ( double ) ( std::rand( ) ) ) / ( ( double ) ( RAND_MAX ) ) );
   double b = ( ( ( double ) ( std::rand( ) ) ) / ( ( double ) ( RAND_MAX ) ) );

   // setup a mesh representation
   MeshRepresentationType::Pointer meshRepresentation = 
                                                  MeshRepresentationType::New();
   meshRepresentation->RequestSetMeshObject( meshSpatialObject );
   meshRepresentation->SetOpacity(1);
   meshRepresentation->SetColor(r, g, b);
   m_MeshRepresentationVector.push_back( meshRepresentation );

   // build axial mesh reslice representation
   MeshResliceRepresentationType::Pointer axialContour = 
                                           MeshResliceRepresentationType::New();
   axialContour->SetOpacity(1);
   axialContour->SetLineWidth(3);
   axialContour->SetColor(r, g, b);     
   axialContour->RequestSetMeshObject( meshSpatialObject );
   axialContour->RequestSetReslicePlaneSpatialObject( m_AxialPlaneSpatialObject );   

   // build sagittal mesh reslice representation
   MeshResliceRepresentationType::Pointer sagittalContour = 
                                           MeshResliceRepresentationType::New();
   sagittalContour->SetOpacity(1);
   sagittalContour->SetLineWidth(3);
   sagittalContour->SetColor(r, g, b);
   sagittalContour->RequestSetMeshObject( meshSpatialObject );
   sagittalContour->RequestSetReslicePlaneSpatialObject( 
                                                 m_SagittalPlaneSpatialObject );
   
   // build coronal mesh reslice representation
   MeshResliceRepresentationType::Pointer coronalContour = 
                                           MeshResliceRepresentationType::New();
   coronalContour->SetOpacity(1);
   coronalContour->SetLineWidth(3);
   coronalContour->SetColor(r, g, b);
   coronalContour->RequestSetMeshObject( meshSpatialObject );
   coronalContour->RequestSetReslicePlaneSpatialObject( 
                                                  m_CoronalPlaneSpatialObject );
      
   // add repressentations to the views
   m_ViewerGroup->m_AxialView->RequestAddObject( axialContour );
   m_ViewerGroup->m_SagittalView->RequestAddObject( sagittalContour );
   m_ViewerGroup->m_CoronalView->RequestAddObject( coronalContour );     
   m_ViewerGroup->m_3DView->RequestAddObject( meshRepresentation );
   m_ViewerGroup->m_3DView->RequestResetCamera();

   // keep the mesh and contours in corresponding vectors
   m_MeshVector.push_back( meshSpatialObject );
   m_AxialMeshResliceRepresentationVector.push_back( axialContour );
   m_SagittalMeshResliceRepresentationVector.push_back( sagittalContour );
   m_CoronalMeshResliceRepresentationVector.push_back( coronalContour );  

   // reset the cameras in the different views
   m_ViewerGroup->m_AxialView->RequestResetCamera();
   m_ViewerGroup->m_SagittalView->RequestResetCamera();
   m_ViewerGroup->m_CoronalView->RequestResetCamera();
   m_ViewerGroup->m_3DView->RequestResetCamera();

   m_StateMachine.PushInput( m_SuccessInput );
   m_StateMachine.ProcessInputs();     
}

/** -----------------------------------------------------------------
* Reslices the views to the picked position
*---------------------------------------------------------------------
*/
void Navigator::SetImagePickingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                  "Navigator::SetImagePickingProcessing called...\n" )

  ImageSpatialObjectType::PointType point = TransformToPoint( 
                                                           m_PickingTransform );

  if ( m_ImageSpatialObject->IsInside( point ) )
  {
    ImageSpatialObjectType::IndexType index;
    m_ImageSpatialObject->TransformPhysicalPointToIndex( point, index);

    const double *data = point.GetVnlVector().data_block();

    m_AxialPlaneSpatialObject->RequestSetCursorPosition( data );
    m_SagittalPlaneSpatialObject->RequestSetCursorPosition( data );
    m_CoronalPlaneSpatialObject->RequestSetCursorPosition( data );
    m_CrossHair->RequestSetCursorPosition( data );
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

    ImageSpatialObjectType::PointType point = TransformToPoint( 
                                                           m_PickingTransform );
  
    if( m_ImageSpatialObject->IsInside( point ) )
    {
      int choice = m_FiducialsPointList->value();

      m_FiducialPointVector[choice]->RequestSetTransformAndParent(
                                         m_PickingTransform, m_WorldReference );

      m_Plan->m_FiducialPoints[choice] = point;

      char buf[50];
      sprintf( buf, "[%.2f, %.2f, %.2f]", point[0], point[1], point[2]);
      m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 2, buf );
      m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor(2, 0.0, 0.0, 1);

      m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 2,buf);
      m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor(2, 0, 0, 1);

      m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 2, buf);
      m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor(2, 0.0, 0, 1);

//      m_ViewerGroup->RequestUpdateOverlays();

      /** Write the updated plan to file */
      this->WriteFiducials();

      ImageSpatialObjectType::IndexType index;
      m_ImageSpatialObject->TransformPhysicalPointToIndex( point, index);

      const double *data = point.GetVnlVector().data_block();

      m_AxialPlaneSpatialObject->RequestSetCursorPosition( data );
      m_SagittalPlaneSpatialObject->RequestSetCursorPosition( data );
      m_CoronalPlaneSpatialObject->RequestSetCursorPosition( data );
      m_CrossHair->RequestSetCursorPosition( data );

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
    fl_alert( "%s\n",errorMessage.c_str() );
    fl_beep( FL_BEEP_ERROR );
    igstkLogMacro2( m_Logger, DEBUG, "Tracker Initialization error\n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
  }

  /** Create the controller for the tracker */
  m_TrackerController = igstk::TrackerController::New();

  /** Set the logger to the controller */
  m_TrackerController->SetLogger(this->GetLogger());

  /** Create an observer for the controller */
  m_TrackerControllerObserver = TrackerControllerObserver::New();
  m_TrackerControllerObserver->SetParent( this );

  // todo: replace these with igstk observers
  m_TrackerController->AddObserver(
                               igstk::TrackerController::InitializeErrorEvent(),
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

  m_TrackerController->AddObserver(
                              igstk::TrackerController::RequestToolErrorEvent(),
                                                  m_TrackerControllerObserver );
 
  m_TrackerController->AddObserver(
                                  igstk::TrackerController::RequestToolsEvent(),
                                                  m_TrackerControllerObserver );

  m_TrackerController->RequestInitialize( m_TrackerConfiguration );
               
  //check that initialization was successful
  if( m_TrackerControllerObserver->Error() )
  {
    std::string errorMessage;
    m_TrackerControllerObserver->GetErrorMessage( errorMessage ); 
    m_TrackerControllerObserver->ClearError();
    fl_alert( "%s\n",errorMessage.c_str() );
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

  if ( m_ToolSpatialObject.IsNull() )
  {
    igstkLogMacro2( m_Logger, DEBUG, "Tool spatial object not available\n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
  }

  m_ToolSpatialObject->RequestDetachFromParent();
  m_ToolSpatialObject->RequestSetTransformAndParent( identity, m_TrackerTool );  

  /** Connect the scene graph with an identity transform first */
  if ( m_ReferenceTool.IsNotNull() )
  { 
    m_ReferenceTool->RequestSetTransformAndParent(identity, m_WorldReference);
  }

  //if (!m_TrackerConfiguration)
  //{
  //  std::string errorMessage;
  //  fl_alert("%s\n", errorMessage.c_str() );
  //  fl_beep( FL_BEEP_ERROR );
  //  igstkLogMacro2( m_Logger, DEBUG,
  //    "Tracker Starting error\n" )
  //  m_StateMachine.PushInput( m_FailureInput );
  //  m_StateMachine.ProcessInputs();
  //  return;
  //}

  m_TrackerController->RequestStartTracking();
  
  //check that start was successful
  if( m_TrackerControllerObserver->Error() )
  {
    std::string errorMessage;
    m_TrackerControllerObserver->GetErrorMessage( errorMessage ); 
    m_TrackerControllerObserver->ClearError();
    fl_alert( "%s\n", errorMessage.c_str() );
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

void Navigator::StartSetTrackerFiducialsProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::StartSetTrackerFiducialsProcessing called...\n")

  this->DisableAll();

  m_FiducialsPointList->activate();

  m_RegisterButton->color(FL_RED); 
  m_RegisterButton->label("Registering...");

  m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 1, 
                                                                "REGISTERING" );
  m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor(1, 1.0, 0.0, 0.0);

  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 1, 
                                                                "REGISTERING" );
  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor(1, 1.0, 0.0, 0);

  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 1, 
                                                                "REGISTERING" );
  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor(1, 1.0, 0.0, 0.0);

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
    m_AcceptedLandmarksContainer.insert( std::pair<unsigned int,bool>(i,false));
  }

  m_FiducialsPointList->value(0);
  this->RequestChangeSelectedFiducial();

  Fl::check();
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
void Navigator::TrackerRegistrationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::TrackerRegistrationProcessing called...\n" )
  
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
         "Tracker to Image Registration Error " << 
                                       lRmscb->GetRegistrationError() << "\n" )
       m_TrackerRMS = lRmscb->GetRegistrationError();

       char buf[50];
       sprintf( buf, "TRACKING (%.2f)", m_TrackerRMS);
       //sprintf( buf, "TRACKING");

       m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 1, buf );
       m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor(1, 0.0, 1, 0);

       m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText(1,buf);
       m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor(1, 0, 1, 0);

       m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText(1,buf);
       m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor(1, 0, 1, 0);

//       m_ViewerGroup->RequestUpdateOverlays();    
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
      m_ReferenceTool->RequestSetTransformAndParent(m_RegistrationTransform, 
                                                              m_WorldReference);
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
  //  fl_alert("%s\n", errorMessage.c_str() );
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
  //  fl_alert("%s\n", errorMessage.c_str() );
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

  m_TrackerController->RequestStopTracking( );
               //check that stop was successful
  if( m_TrackerControllerObserver->Error() )
  {
    std::string errorMessage;
    m_TrackerControllerObserver->GetErrorMessage( errorMessage ); 
    m_TrackerControllerObserver->ClearError();
    fl_alert( "%s\n", errorMessage.c_str() );
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
    fl_alert( "%s\n",errorMessage.c_str() );
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
    m_AxialFiducialRepresentationVector[i]->RequestSetEllipsoidObject( 
                                                     m_FiducialPointVector[i] );
    m_AxialFiducialRepresentationVector[i]->SetColor( 1.0, 0.0, 0.0);
    m_AxialFiducialRepresentationVector[i]->SetOpacity( 0.6 );

    m_SagittalFiducialRepresentationVector[i] = 
                                             EllipsoidRepresentationType::New();
    m_SagittalFiducialRepresentationVector[i]->RequestSetEllipsoidObject( 
                                                     m_FiducialPointVector[i] );
    m_SagittalFiducialRepresentationVector[i]->SetColor( 1.0, 0.0, 0.0);
    m_SagittalFiducialRepresentationVector[i]->SetOpacity( 0.6 );

    m_CoronalFiducialRepresentationVector[i] = 
                                             EllipsoidRepresentationType::New();
    m_CoronalFiducialRepresentationVector[i]->RequestSetEllipsoidObject( 
                                                     m_FiducialPointVector[i] );
    m_CoronalFiducialRepresentationVector[i]->SetColor( 1.0, 0.0, 0.0);
    m_CoronalFiducialRepresentationVector[i]->SetOpacity( 0.6 );

    m_3DViewFiducialRepresentationVector[i] =EllipsoidRepresentationType::New();
    m_3DViewFiducialRepresentationVector[i]->RequestSetEllipsoidObject( 
                                                     m_FiducialPointVector[i] );
    m_3DViewFiducialRepresentationVector[i]->SetColor( 1.0, 0.0, 0.0);
    m_3DViewFiducialRepresentationVector[i]->SetOpacity( 0.6 );
  }  

  // create reslice plane spatial object for axial view
  m_AxialPlaneSpatialObject = ReslicerPlaneType::New();
  m_AxialPlaneSpatialObject->RequestSetReslicingMode( 
                                                ReslicerPlaneType::Orthogonal );
  m_AxialPlaneSpatialObject->RequestSetOrientationType( 
                                                     ReslicerPlaneType::Axial );
  m_AxialPlaneSpatialObject->RequestSetBoundingBoxProviderSpatialObject( 
                                                         m_ImageSpatialObject );

  // create reslice plane spatial object for sagittal view
  m_SagittalPlaneSpatialObject = ReslicerPlaneType::New();
  m_SagittalPlaneSpatialObject->RequestSetReslicingMode( 
                                                ReslicerPlaneType::Orthogonal );
  m_SagittalPlaneSpatialObject->RequestSetOrientationType( 
                                                  ReslicerPlaneType::Sagittal );
  m_SagittalPlaneSpatialObject->RequestSetBoundingBoxProviderSpatialObject( 
                                                         m_ImageSpatialObject );

  // create reslice plane spatial object for coronal view
  m_CoronalPlaneSpatialObject = ReslicerPlaneType::New();
  m_CoronalPlaneSpatialObject->RequestSetReslicingMode( 
                                                ReslicerPlaneType::Orthogonal );
  m_CoronalPlaneSpatialObject->RequestSetOrientationType( 
                                                   ReslicerPlaneType::Coronal );
  m_CoronalPlaneSpatialObject->RequestSetBoundingBoxProviderSpatialObject( 
                                                         m_ImageSpatialObject );

  // create reslice plane representation for axial view
  m_AxialPlaneRepresentation = ImageRepresentationType::New();
  m_AxialPlaneRepresentation->RequestSetImageSpatialObject( 
                                                         m_ImageSpatialObject );
  m_AxialPlaneRepresentation->RequestSetReslicePlaneSpatialObject( 
                                                    m_AxialPlaneSpatialObject );

  // create reslice plane representation for sagittal view
  m_SagittalPlaneRepresentation = ImageRepresentationType::New();
  m_SagittalPlaneRepresentation->RequestSetImageSpatialObject( 
                                                         m_ImageSpatialObject );
  m_SagittalPlaneRepresentation->RequestSetReslicePlaneSpatialObject( 
                                                 m_SagittalPlaneSpatialObject );

  // create reslice plane representation for coronal view
  m_CoronalPlaneRepresentation = ImageRepresentationType::New();
  m_CoronalPlaneRepresentation->RequestSetImageSpatialObject( 
                                                         m_ImageSpatialObject );
  m_CoronalPlaneRepresentation->RequestSetReslicePlaneSpatialObject( 
                                                  m_CoronalPlaneSpatialObject );

   /** 
   *  Request information about the slice bounds. The answer will be
   *  received in the form of an event. This will be used to initialize
   *  the reslicing sliders and set initial slice position
   */

  ImageExtentObserver::Pointer extentObserver = ImageExtentObserver::New();
  
  unsigned int extentObserverID;

  extentObserverID = m_ImageSpatialObject->AddObserver( 
                                    igstk::ImageExtentEvent(), extentObserver );

  m_ImageSpatialObject->RequestGetImageExtent();

  unsigned int xslice, yslice, zslice;
  if( extentObserver->GotImageExtent() )
  {
    const igstk::EventHelperType::ImageExtentType& extent = 
                                               extentObserver->GetImageExtent();

    const unsigned int zmin = extent.zmin;
    const unsigned int zmax = extent.zmax;
    zslice = static_cast< unsigned int > ( (zmin + zmax)/2 );
    m_ViewerGroup->m_Sliders[0]->minimum( zmin );
    m_ViewerGroup->m_Sliders[0]->maximum( zmax );
    m_ViewerGroup->m_Sliders[0]->value( zslice );
    m_ViewerGroup->m_Sliders[0]->activate();

    const unsigned int ymin = extent.ymin;
    const unsigned int ymax = extent.ymax;
    yslice = static_cast< unsigned int > ( (ymin + ymax)/2 );
    m_ViewerGroup->m_Sliders[1]->minimum( ymin );
    m_ViewerGroup->m_Sliders[1]->maximum( ymax );
    m_ViewerGroup->m_Sliders[1]->value( yslice );
    m_ViewerGroup->m_Sliders[1]->activate();

    const unsigned int xmin = extent.xmin;
    const unsigned int xmax = extent.xmax;
    xslice = static_cast< unsigned int > ( (xmin + xmax)/2 );
    m_ViewerGroup->m_Sliders[2]->minimum( xmin );
    m_ViewerGroup->m_Sliders[2]->maximum( xmax );
    m_ViewerGroup->m_Sliders[2]->value( xslice );
    m_ViewerGroup->m_Sliders[2]->activate();
  }

  m_ImageSpatialObject->RemoveObserver( extentObserverID );

  // Set up cross hairs
  m_CrossHair = CrossHairType::New();
  m_CrossHair->RequestSetBoundingBoxProviderSpatialObject(m_ImageSpatialObject);

  // Set initial crosshair possition
  // This initialization is important, otherwise, images will not load properly
  // under debug mode
  ImageSpatialObjectType::IndexType index;
  index[0] = xslice;
  index[1] = yslice;
  index[2] = zslice;

  PointType point;
  m_ImageSpatialObject->TransformIndexToPhysicalPoint( index, point);

  const double *data = NULL;
  data = point.GetVnlVector().data_block();
  m_CrossHair->RequestSetCursorPosition(data);

  // buid the cross hair representations
  m_AxialCrossHairRepresentation = CrossHairRepresentationType::New();
  m_AxialCrossHairRepresentation->SetColor(0,1,0);
  m_AxialCrossHairRepresentation->SetLineWidth(2);
  m_AxialCrossHairRepresentation->RequestSetCrossHairObject( m_CrossHair );

  m_SagittalCrossHairRepresentation = CrossHairRepresentationType::New();
  m_SagittalCrossHairRepresentation->SetColor(0,1,0);
  m_SagittalCrossHairRepresentation->SetLineWidth(2);
  m_SagittalCrossHairRepresentation->RequestSetCrossHairObject( m_CrossHair );

  m_CoronalCrossHairRepresentation = CrossHairRepresentationType::New();
  m_CoronalCrossHairRepresentation->SetColor(0,1,0);
  m_CoronalCrossHairRepresentation->SetLineWidth(2);
  m_CoronalCrossHairRepresentation->RequestSetCrossHairObject( m_CrossHair );

  m_3DViewCrossHairRepresentation = CrossHairRepresentationType::New();
  m_3DViewCrossHairRepresentation->SetColor(1,1,0);
  m_3DViewCrossHairRepresentation->SetLineWidth(2);
  m_3DViewCrossHairRepresentation->RequestSetCrossHairObject( m_CrossHair );

  // add the cross hair representation to the different views
  m_ViewerGroup->m_AxialView->RequestAddObject( m_AxialCrossHairRepresentation);
  m_ViewerGroup->m_SagittalView->RequestAddObject( 
                                            m_SagittalCrossHairRepresentation );
  m_ViewerGroup->m_CoronalView->RequestAddObject( 
                                             m_CoronalCrossHairRepresentation );
  m_ViewerGroup->m_3DView->RequestAddObject( m_3DViewCrossHairRepresentation );

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

  // our principal node in the scene graph: the world reference
  m_WorldReference  = igstk::AxesObject::New();

  // set transform and parent to the image spatial object
  m_ImageSpatialObject->RequestSetTransformAndParent(identity,m_WorldReference);

  // set transform and parent to the image plane reslice spatial objects
  m_AxialPlaneSpatialObject->RequestSetTransformAndParent( identity, 
                                                             m_WorldReference );
  m_SagittalPlaneSpatialObject->RequestSetTransformAndParent( identity, 
                                                             m_WorldReference );
  m_CoronalPlaneSpatialObject->RequestSetTransformAndParent( identity, 
                                                             m_WorldReference );

  m_ViewerGroup->m_AxialView->RequestSetTransformAndParent( identity, 
                                                    m_AxialPlaneSpatialObject );

  m_ViewerGroup->m_SagittalView->RequestSetTransformAndParent( identity, 
                                                 m_SagittalPlaneSpatialObject );

  m_ViewerGroup->m_CoronalView->RequestSetTransformAndParent( identity, 
                                                  m_CoronalPlaneSpatialObject );

  m_ViewerGroup->m_3DView->RequestSetTransformAndParent( identity, 
                                                             m_WorldReference );

  // set transform and parent to the cross hair object
  m_CrossHair->RequestSetTransformAndParent( identity, m_WorldReference );

  // set transform and parent to the fiducial points
  m_FiducialPointVector[0]->RequestSetTransformAndParent( identity, 
                                                             m_WorldReference );
  m_FiducialPointVector[1]->RequestSetTransformAndParent( identity, 
                                                             m_WorldReference );
  m_FiducialPointVector[2]->RequestSetTransformAndParent( identity, 
                                                             m_WorldReference );
  m_FiducialPointVector[3]->RequestSetTransformAndParent( identity, 
                                                             m_WorldReference );

  // add reslice plane representations to the orthogonal views
  m_ViewerGroup->m_AxialView->RequestAddObject( m_AxialPlaneRepresentation );
  m_ViewerGroup->m_SagittalView->RequestAddObject( 
                                                m_SagittalPlaneRepresentation );
  m_ViewerGroup->m_CoronalView->RequestAddObject( 
                                                 m_CoronalPlaneRepresentation );

  // add reslice plane representations to the 3D views
  m_AxialPlaneRepresentation2 = m_AxialPlaneRepresentation->Copy();
  m_ViewerGroup->m_3DView->RequestAddObject( m_AxialPlaneRepresentation2 );

  m_SagittalPlaneRepresentation2 = m_SagittalPlaneRepresentation->Copy();
  m_ViewerGroup->m_3DView->RequestAddObject( m_SagittalPlaneRepresentation2 );

  m_CoronalPlaneRepresentation2 = m_CoronalPlaneRepresentation->Copy();
  m_ViewerGroup->m_3DView->RequestAddObject( m_CoronalPlaneRepresentation2 );

  // set parallel projection in the 2D views
  m_ViewerGroup->m_AxialView->SetCameraParallelProjection(true);
  m_ViewerGroup->m_SagittalView->SetCameraParallelProjection(true);
  m_ViewerGroup->m_CoronalView->SetCameraParallelProjection(true);

  // reset the cameras in the different views
  m_ViewerGroup->m_AxialView->RequestResetCamera();
  m_ViewerGroup->m_SagittalView->RequestResetCamera();
  m_ViewerGroup->m_CoronalView->RequestResetCamera();
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
  //m_ViewerGroup->m_3DView->RequestAddOrientationBox();
  m_ViewerGroup->m_3DView->RequestStart();

  //m_ViewerGroup->m_AxialView->SetRectangleEnabled(true);
  //m_ViewerGroup->m_SagittalView->SetRectangleEnabled(true);
  //m_ViewerGroup->m_CoronalView->SetRectangleEnabled(true);
  //m_ViewerGroup->m_3DView->SetRectangleEnabled(true);

  /** Add observer for picking events in the Axial view */
  m_AxialViewPickerObserver = LoadedObserverType::New();
  m_AxialViewPickerObserver->SetCallbackFunction( this, 
                                         &Navigator::AxialViewPickingCallback );

  m_ViewerGroup->m_AxialView->AddObserver(
      igstk::CoordinateSystemTransformToEvent(), m_AxialViewPickerObserver );

  /** Add observer for picking events in the Axial view */
  m_SagittalViewPickerObserver = LoadedObserverType::New();
  m_SagittalViewPickerObserver->SetCallbackFunction( this, 
                                      &Navigator::SagittalViewPickingCallback );

  m_ViewerGroup->m_SagittalView->AddObserver(
      igstk::CoordinateSystemTransformToEvent(), m_SagittalViewPickerObserver );

  /** Add observer for picking events in the Axial view */
  m_CoronalViewPickerObserver = LoadedObserverType::New();
  m_CoronalViewPickerObserver->SetCallbackFunction( this, 
                                       &Navigator::CoronalViewPickingCallback );

  m_ViewerGroup->m_CoronalView->AddObserver(
      igstk::CoordinateSystemTransformToEvent(), m_CoronalViewPickerObserver );

  /** Add observer for slider bar reslicing event */
  m_ManualReslicingObserver = LoadedObserverType::New();
  m_ManualReslicingObserver->SetCallbackFunction( this,
    &Navigator::ResliceImageCallback );

  m_ViewerGroup->AddObserver( 
                          igstk::NavigatorQuadrantViews::ManualReslicingEvent(),
                                                    m_ManualReslicingObserver );

  /** Add observer for key pressed event */
  m_KeyPressedObserver = LoadedObserverType::New();
  m_KeyPressedObserver->SetCallbackFunction( this,
    &Navigator::HandleKeyPressedCallback );

  m_ViewerGroup->AddObserver( igstk::NavigatorQuadrantViews::KeyPressedEvent(),
                                                         m_KeyPressedObserver );

  /** Add observer for mouse pressed event */
  m_MousePressedObserver = LoadedObserverType::New();
  m_MousePressedObserver->SetCallbackFunction( this,
    &Navigator::HandleMousePressedCallback );

  m_ViewerGroup->AddObserver( 
                             igstk::NavigatorQuadrantViews::MousePressedEvent(),
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
  for( unsigned int i = 0; i < m_Plan->m_FiducialPoints.size(); i++ )
  {
    sprintf( buf, "Fiducial #%d", i+1 );
    m_FiducialsPointList->add( buf );
    RegistrationType::LandmarkTrackerPointType p;
    m_LandmarksContainer.push_back(p);
    m_AcceptedLandmarksContainer.insert( std::pair<unsigned int,bool>(i,false));
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
      m_AxialPlaneSpatialObject->RequestSetReslicingMode( 
                                                ReslicerPlaneType::Orthogonal );
      m_AxialPlaneSpatialObject->RequestSetOrientationType( 
                                                     ReslicerPlaneType::Axial );
      m_SagittalPlaneSpatialObject->RequestSetReslicingMode( 
                                                ReslicerPlaneType::Orthogonal );
      m_SagittalPlaneSpatialObject->RequestSetOrientationType( 
                                                  ReslicerPlaneType::Sagittal );
      m_CoronalPlaneSpatialObject->RequestSetReslicingMode( 
                                                ReslicerPlaneType::Orthogonal );
      m_CoronalPlaneSpatialObject->RequestSetOrientationType( 
                                                   ReslicerPlaneType::Coronal );
      m_ViewerGroup->m_AxialView->RequestAddObject( 
                                               m_AxialCrossHairRepresentation );
      m_ViewerGroup->m_SagittalView->RequestAddObject( 
                                            m_SagittalCrossHairRepresentation );
      m_ViewerGroup->m_CoronalView->RequestAddObject( 
                                             m_CoronalCrossHairRepresentation );
      m_ViewerGroup->m_CoronalView->RequestAddObject( 
                                              m_3DViewCrossHairRepresentation );
      break;

    case 1:
      m_AxialPlaneSpatialObject->RequestSetReslicingMode( 
                                             ReslicerPlaneType::OffOrthogonal );
      m_AxialPlaneSpatialObject->RequestSetOrientationType( 
                                                  ReslicerPlaneType::OffAxial );
      m_SagittalPlaneSpatialObject->RequestSetReslicingMode( 
                                             ReslicerPlaneType::OffOrthogonal );
      m_SagittalPlaneSpatialObject->RequestSetOrientationType( 
                                               ReslicerPlaneType::OffSagittal );
      m_CoronalPlaneSpatialObject->RequestSetReslicingMode( 
                                                ReslicerPlaneType::Orthogonal );
      m_CoronalPlaneSpatialObject->RequestSetOrientationType( 
                                                  ReslicerPlaneType::Sagittal );
      m_ViewerGroup->m_AxialView->RequestRemoveObject( 
                                               m_AxialCrossHairRepresentation );
      m_ViewerGroup->m_SagittalView->RequestRemoveObject( 
                                            m_SagittalCrossHairRepresentation );
      m_ViewerGroup->m_CoronalView->RequestRemoveObject( 
                                             m_CoronalCrossHairRepresentation );
      m_ViewerGroup->m_CoronalView->RequestRemoveObject( 
                                              m_3DViewCrossHairRepresentation );
      break;

    case 2:
      m_AxialPlaneSpatialObject->RequestSetReslicingMode( 
                                                   ReslicerPlaneType::Oblique );
      m_AxialPlaneSpatialObject->RequestSetOrientationType( 
                           ReslicerPlaneType::PlaneOrientationWithXAxesNormal );
      m_SagittalPlaneSpatialObject->RequestSetReslicingMode( 
                                                   ReslicerPlaneType::Oblique );
      m_SagittalPlaneSpatialObject->RequestSetOrientationType( 
                           ReslicerPlaneType::PlaneOrientationWithYAxesNormal );
      m_CoronalPlaneSpatialObject->RequestSetReslicingMode( 
                                                   ReslicerPlaneType::Oblique );
      m_CoronalPlaneSpatialObject->RequestSetOrientationType( 
                           ReslicerPlaneType::PlaneOrientationWithZAxesNormal );
      m_ViewerGroup->m_AxialView->RequestRemoveObject( 
                                               m_AxialCrossHairRepresentation );
      m_ViewerGroup->m_SagittalView->RequestRemoveObject( 
                                            m_SagittalCrossHairRepresentation );
      m_ViewerGroup->m_CoronalView->RequestRemoveObject( 
                                             m_CoronalCrossHairRepresentation );
      m_ViewerGroup->m_CoronalView->RequestRemoveObject( 
                                              m_3DViewCrossHairRepresentation );
      break;

  default:
    igstkLogMacro2( m_Logger, DEBUG, 
                 "Navigator::RequestChangeSelectedViewMode invalid choice \n" )
    return;
  }

  m_ViewerGroup->m_AxialView->RequestResetCamera();
  m_ViewerGroup->m_SagittalView->RequestResetCamera();
  m_ViewerGroup->m_CoronalView->RequestResetCamera();
  m_ViewerGroup->m_3DView->RequestResetCamera();

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

  for (unsigned int i=0; i<4; i++)
  {
    m_ViewerGroup->m_AxialView->RequestRemoveObject( 
                                       m_AxialFiducialRepresentationVector[i] );
    m_ViewerGroup->m_SagittalView->RequestRemoveObject( 
                                    m_SagittalFiducialRepresentationVector[i] );
    m_ViewerGroup->m_CoronalView->RequestRemoveObject( 
                                     m_CoronalFiducialRepresentationVector[i] );
    m_ViewerGroup->m_3DView->RequestRemoveObject( 
                                      m_3DViewFiducialRepresentationVector[i] );
  }

  m_ViewerGroup->m_AxialView->RequestAddObject( 
                                  m_AxialFiducialRepresentationVector[choice] );
  m_ViewerGroup->m_SagittalView->RequestAddObject( 
                               m_SagittalFiducialRepresentationVector[choice] );
  m_ViewerGroup->m_CoronalView->RequestAddObject( 
                                m_CoronalFiducialRepresentationVector[choice] );
  m_ViewerGroup->m_3DView->RequestAddObject( 
                                 m_3DViewFiducialRepresentationVector[choice] );

  char buf[50];

  sprintf( buf, "[%.2f, %.2f, %.2f]", point[0], point[1], point[2]);
 
  /** Display point position as annotation */    
  m_ViewerGroup->m_AxialViewAnnotation->RequestSetAnnotationText( 2, buf );
  m_ViewerGroup->m_AxialViewAnnotation->RequestSetFontColor(2, 0.0, 0.0, 1.0);

  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetAnnotationText( 2, buf );
  m_ViewerGroup->m_SagittalViewAnnotation->RequestSetFontColor(2, 0.0, 0.0, 1);

  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetAnnotationText( 2, buf );
  m_ViewerGroup->m_CoronalViewAnnotation->RequestSetFontColor(2, 0.0, 0.0, 1.0);

//  m_ViewerGroup->RequestUpdateOverlays();

  /** Reslice image to the selected point position */
  if( m_ImageSpatialObject->IsInside( point ) )
  {
    ImageSpatialObjectType::IndexType index;
    m_ImageSpatialObject->TransformPhysicalPointToIndex( point, index);

    const double *data = point.GetVnlVector().data_block();

    m_AxialPlaneSpatialObject->RequestSetCursorPosition( data );
    m_SagittalPlaneSpatialObject->RequestSetCursorPosition( data );
    m_CoronalPlaneSpatialObject->RequestSetCursorPosition( data );
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
void Navigator::ResliceImageCallback( const itk::EventObject & event )
{

  if ( igstk::NavigatorQuadrantViews::ManualReslicingEvent().CheckEvent(&event))
  {
    igstk::NavigatorQuadrantViews::ManualReslicingEvent *resliceEvent =
    ( igstk::NavigatorQuadrantViews::ManualReslicingEvent *) & event;
    ImageSpatialObjectType::IndexType index = resliceEvent->Get();

    PointType point;
    m_ImageSpatialObject->TransformIndexToPhysicalPoint( index, point );

    const double *data = point.GetVnlVector().data_block();

    m_AxialPlaneSpatialObject->RequestSetCursorPosition( data );
    m_SagittalPlaneSpatialObject->RequestSetCursorPosition( data );
    m_CoronalPlaneSpatialObject->RequestSetCursorPosition( data );
    m_CrossHair->RequestSetCursorPosition( data );
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
*  Callback function for picking event in the axial view.
*  Upon receiving a valid picking event, this method will reslice the 
*  image to that location.
*---------------------------------------------------------------------
*/
void Navigator::AxialViewPickingCallback( const itk::EventObject & event)
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

    unsigned int obsId = m_AxialPlaneSpatialObject->AddObserver( 
      igstk::CoordinateSystemTransformToEvent(), coordinateObserver );

    m_AxialPlaneSpatialObject->RequestComputeTransformTo( m_WorldReference );

    if( coordinateObserver->GotCoordinateSystemTransform() )
    {
      igstk::CoordinateSystemTransformToResult transformToResult = 
                             coordinateObserver->GetCoordinateSystemTransform();
      igstk::Transform viewToWorldReferenceTransform = 
                                               transformToResult.GetTransform();
      m_PickingTransform = igstk::Transform::TransformCompose( 
                            viewToWorldReferenceTransform, m_PickingTransform );
    }
    else
    {
      igstkLogMacro2( m_Logger, DEBUG, "Navigator::AxialViewPickingCallback \
                              could not get coordinate system transform...\n" )
      return;
    }

    m_AxialPlaneSpatialObject->RemoveObserver( obsId );

    m_StateMachine.PushInput( m_SetPickingPositionInput );
    m_StateMachine.ProcessInputs();
  }
}

/** -----------------------------------------------------------------
*  Callback function for picking event in the sagittal view.
*  Upon receiving a valid picking event, this method will reslice the 
*  image to that location.
*---------------------------------------------------------------------
*/
void Navigator::SagittalViewPickingCallback( const itk::EventObject & event)
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

    unsigned int obsId = m_SagittalPlaneSpatialObject->AddObserver( 
      igstk::CoordinateSystemTransformToEvent(), coordinateObserver );

    m_SagittalPlaneSpatialObject->RequestComputeTransformTo( m_WorldReference );

    if( coordinateObserver->GotCoordinateSystemTransform() )
    {
      igstk::CoordinateSystemTransformToResult transformToResult = 
                             coordinateObserver->GetCoordinateSystemTransform();
      igstk::Transform viewToWorldReferenceTransform = 
                                               transformToResult.GetTransform();
      m_PickingTransform = igstk::Transform::TransformCompose( 
                            viewToWorldReferenceTransform, m_PickingTransform );
    }
    else
    {
      igstkLogMacro2( m_Logger, DEBUG, "Navigator::SagittalViewPickingCallback \
                              could not get coordinate system transform...\n" )
      return;
    }

    m_SagittalPlaneSpatialObject->RemoveObserver( obsId );

    m_StateMachine.PushInput( m_SetPickingPositionInput );
    m_StateMachine.ProcessInputs();
  }
}

/** -----------------------------------------------------------------
*  Callback function for picking event in the coronal view.
*  Upon receiving a valid picking event, this method will reslice the 
*  image to that location.
*---------------------------------------------------------------------
*/
void Navigator::CoronalViewPickingCallback( const itk::EventObject & event)
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

    unsigned int obsId = m_CoronalPlaneSpatialObject->AddObserver( 
      igstk::CoordinateSystemTransformToEvent(), coordinateObserver );

    m_CoronalPlaneSpatialObject->RequestComputeTransformTo( m_WorldReference );

    if( coordinateObserver->GotCoordinateSystemTransform() )
    {
      igstk::CoordinateSystemTransformToResult transformToResult = 
                             coordinateObserver->GetCoordinateSystemTransform();
      igstk::Transform viewToWorldReferenceTransform = 
                                               transformToResult.GetTransform();
      m_PickingTransform = igstk::Transform::TransformCompose( 
                            viewToWorldReferenceTransform, m_PickingTransform );
    }
    else
    {
      igstkLogMacro2( m_Logger, DEBUG, "Navigator::CoronalViewPickingCallback could not get coordinate system transform...\n" )
      return;
    }

    m_CoronalPlaneSpatialObject->RemoveObserver( obsId );

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

    m_AxialPlaneRepresentation->SetWindowLevel( m_WindowWidth, m_WindowLevel );
    m_SagittalPlaneRepresentation->SetWindowLevel(m_WindowWidth, m_WindowLevel);
    m_CoronalPlaneRepresentation->SetWindowLevel( m_WindowWidth, m_WindowLevel);

    m_AxialPlaneRepresentation2->SetWindowLevel( m_WindowWidth, m_WindowLevel );
    m_SagittalPlaneRepresentation2->SetWindowLevel(m_WindowWidth,m_WindowLevel);
    m_CoronalPlaneRepresentation2->SetWindowLevel(m_WindowWidth, m_WindowLevel);
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
        m_ViewerGroup->m_AxialView->RequestResetCamera();
        m_ViewerGroup->m_SagittalView->RequestResetCamera();
        m_ViewerGroup->m_CoronalView->RequestResetCamera();
        m_ViewerGroup->m_3DView->RequestResetCamera();
        break;
    // accepts a fiducial pointed out by the tracker while doing registration
    case 'h': 
          this->RequestAcceptTrackerFiducial();
        break;
    // attempts to finish the registration
    case 'g': 
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
    igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::EnableOrthogonalPlanes called...\n" )

  m_ViewerGroup->m_3DView->RequestAddObject( m_AxialPlaneRepresentation );
  m_ViewerGroup->m_3DView->RequestAddObject( m_SagittalPlaneRepresentation );
  m_ViewerGroup->m_3DView->RequestAddObject( m_CoronalPlaneRepresentation );

}

void Navigator::DisableOrthogonalPlanes()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Navigator::DisableOrthogonalPlanes called...\n" )

  m_ViewerGroup->m_3DView->RequestRemoveObject( m_AxialPlaneRepresentation );
  m_ViewerGroup->m_3DView->RequestRemoveObject( m_SagittalPlaneRepresentation );
  m_ViewerGroup->m_3DView->RequestRemoveObject( m_CoronalPlaneRepresentation );

}

void Navigator::ToolAvailableCallback(const itk::EventObject & itkNotUsed(event) )
{
  m_TrackingSemaphore->color(FL_GREEN);
  m_TrackingSemaphore->label("tracking");
  m_ControlGroup->redraw();  
  Fl::check();
}

void Navigator::ToolNotAvailableCallback(const itk::EventObject & itkNotUsed(event) )
{
  m_TrackingSemaphore->color(FL_RED);
  m_TrackingSemaphore->label("not visible");
  m_ControlGroup->redraw();  
  Fl::check();
}

void Navigator::ReferenceAvailableCallback(const itk::EventObject & itkNotUsed(event) )
{
  m_ReferenceSemaphore->color(FL_GREEN);
  m_ReferenceSemaphore->label("tracking");
  m_ControlGroup->redraw();  
  Fl::check();
}

void Navigator::ReferenceNotAvailableCallback(const itk::EventObject & itkNotUsed(event) )
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
Navigator::TrackerControllerObserver::Execute( itk::Object * itkNotUsed(caller), 
                                                const itk::EventObject & event )
{
  const igstk::TrackerController::InitializeErrorEvent *evt1a =
  dynamic_cast<const igstk::TrackerController::InitializeErrorEvent * >(&event);

  const igstk::TrackerStartTrackingErrorEvent *evt1b =
    dynamic_cast< const igstk::TrackerStartTrackingErrorEvent * > (&event);

    const igstk::TrackerStopTrackingErrorEvent *evt1c =
    dynamic_cast< const igstk::TrackerStopTrackingErrorEvent * > (&event);

   const igstk::TrackerController::RequestToolsEvent *evt3 = 
    dynamic_cast< const igstk::TrackerController::RequestToolsEvent * >(&event);

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
    igstk::TrackerController::ToolContainerType::iterator iter = 
                                          toolContainer.find(DRIVING_TOOL_NAME);

    if ( iter!=toolContainer.end() )
    {      
        igstkLogMacro2( m_Parent->m_Logger, DEBUG, 
          "Navigator::TrackerControllerObserver found tool with name: " 
                                                      << (*iter).first << "\n" )

        m_Parent->m_TrackerTool = (*iter).second;

        m_Parent->m_TrackerToolNotAvailableObserver = LoadedObserverType::New();

        m_Parent->m_TrackerToolNotAvailableObserver->SetCallbackFunction( 
                               m_Parent, &Navigator::ToolNotAvailableCallback );

        m_Parent->m_TrackerTool->AddObserver(
                               igstk::TrackerToolNotAvailableToBeTrackedEvent(),
                                   m_Parent->m_TrackerToolNotAvailableObserver);
   
        m_Parent->m_TrackerToolAvailableObserver = LoadedObserverType::New();

        m_Parent->m_TrackerToolAvailableObserver->SetCallbackFunction( m_Parent,
                                            &Navigator::ToolAvailableCallback );

        m_Parent->m_TrackerTool->AddObserver(
         igstk::TrackerToolMadeTransitionToTrackedStateEvent(), 
                                      m_Parent->m_TrackerToolAvailableObserver);
    }
  }
  else if ( evt4 )
  {
    igstk::TrackerController::ToolEntryType entry = evt4->Get();
    if ( entry.first == REFERENCE_NAME )
    {
        igstkLogMacro2( m_Parent->m_Logger, DEBUG, 
        "Navigator::TrackerControllerObserver found reference tool with name:" 
                                                        << entry.first << "\n" )

        m_Parent->m_ReferenceTool = entry.second;

        m_Parent->m_ReferenceNotAvailableObserver = LoadedObserverType::New();
        m_Parent->m_ReferenceNotAvailableObserver->SetCallbackFunction(m_Parent,
                                    &Navigator::ReferenceNotAvailableCallback );
        m_Parent->m_ReferenceTool->AddObserver(
                               igstk::TrackerToolNotAvailableToBeTrackedEvent(),
                                     m_Parent->m_ReferenceNotAvailableObserver);

        m_Parent->m_ReferenceAvailableObserver = LoadedObserverType::New();
        m_Parent->m_ReferenceAvailableObserver->SetCallbackFunction( m_Parent,
                                       &Navigator::ReferenceAvailableCallback );

        m_Parent->m_ReferenceTool->AddObserver(
                          igstk::TrackerToolMadeTransitionToTrackedStateEvent(),
                                        m_Parent->m_ReferenceAvailableObserver);
    }
  }
}

void 
Navigator
::OnITKProgressEvent(itk::Object *source, const itk::EventObject &)
{
  // Get the value of the progress
  float progress =reinterpret_cast<itk::ProcessObject *>(source)->GetProgress();

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
Navigator
::RequestPrepareToQuit()
{
  this->RequestStopTracking();
  this->RequestDisconnectTracker();
}


