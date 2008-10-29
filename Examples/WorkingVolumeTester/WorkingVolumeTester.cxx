/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    WorkingVolumeTester.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "WorkingVolumeTester.h"

#include "FL/Fl_File_Chooser.H"
#include "FL/Fl_Input.H"

#include "itksys/SystemTools.hxx"
#include "itksys/Directory.hxx"
#include "igstkTransformObserver.h"

#include "igstkMicronTrackerConfiguration.h"
#include "igstkAuroraTrackerConfiguration.h"
#include "igstkPolarisTrackerConfiguration.h"

#include "igstkAuroraConfigurationXMLFileReader.h"
#include "igstkPolarisVicraConfigurationXMLFileReader.h"
#include "igstkPolarisSpectraConfigurationXMLFileReader.h"
#include "igstkPolarisHybridConfigurationXMLFileReader.h"
#include "igstkMicronConfigurationXMLFileReader.h"

#define VIEW_2D_REFRESH_RATE 15
#define VIEW_3D_REFRESH_RATE 15

/** -----------------------------------------------------------------
*     Constructor
*  -----------------------------------------------------------------
*/
WorkingVolumeTester::WorkingVolumeTester() : 
  m_StateMachine(this)
{

  std::srand( 5 );

  /** Create the controller for the tracker and assign observers to it*/
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
  logFileName = "logWorkingVolumeTester"
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

  /** Initialize member variables  */
  m_WorldReference        = AxesObjectType::New();
  
  /** Machine States*/

  igstkAddStateMacro( Initial );  
  igstkAddStateMacro( LoadingMesh ); 
  igstkAddStateMacro( ConfiguringTracker );
  igstkAddStateMacro( TrackerConfigurationReady );
  igstkAddStateMacro( InitializingTracker );
  igstkAddStateMacro( TrackerInitializationReady );
  igstkAddStateMacro( StartingTracker ); 
  igstkAddStateMacro( StoppingTracker ); 
  igstkAddStateMacro( DisconnectingTracker ); 

  igstkAddStateMacro( Tracking );

  /** Machine Inputs*/
  
  igstkAddInputMacro( Success );
  igstkAddInputMacro( Failure );

  igstkAddInputMacro( LoadTrackerMesh );
  igstkAddInputMacro( LoadWorkingVolumeMesh );
  igstkAddInputMacro( ConfigureTracker );
  igstkAddInputMacro( InitializeTracker );

  igstkAddInputMacro( StartTracking );
  igstkAddInputMacro( StopTracking );
  igstkAddInputMacro( DisconnectTracker );

  /** Initial State */
  
  igstkAddTransitionMacro( Initial, ConfigureTracker,
                           ConfiguringTracker, ConfigureTracker );

  //complete table for state: Initial
  igstkAddTransitionMacro( Initial, Success, 
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, Failure, 
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, LoadWorkingVolumeMesh,
                           Initial, LoadWorkingVolumeMesh );
  igstkAddTransitionMacro( Initial, LoadTrackerMesh,
                           Initial, LoadTrackerMesh );
  igstkAddTransitionMacro( Initial, InitializeTracker, 
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, StartTracking,
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, StopTracking,
                           Initial, ReportInvalidRequest );  
  igstkAddTransitionMacro( Initial, DisconnectTracker,
                           Initial, ReportInvalidRequest );

  /** ConfiguringTracker State */

  igstkAddTransitionMacro( ConfiguringTracker, Success, 
                           TrackerConfigurationReady, ReportSuccessTrackerConfiguration );

  igstkAddTransitionMacro( ConfiguringTracker, Failure, 
                           Initial, ReportFailureTrackerConfiguration );

  //complete table for state: ConfiguringTracker

  igstkAddTransitionMacro( ConfiguringTracker, LoadWorkingVolumeMesh, 
                           ConfiguringTracker, ReportInvalidRequest );

  igstkAddTransitionMacro( ConfiguringTracker, LoadTrackerMesh, 
                           ConfiguringTracker, ReportInvalidRequest );
  
  igstkAddTransitionMacro( ConfiguringTracker, ConfigureTracker, 
                           ConfiguringTracker, ReportInvalidRequest );

  igstkAddTransitionMacro( ConfiguringTracker, InitializeTracker, 
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
  
  igstkAddTransitionMacro( TrackerConfigurationReady, LoadWorkingVolumeMesh, 
                           TrackerConfigurationReady, ReportInvalidRequest );

  igstkAddTransitionMacro( TrackerConfigurationReady, LoadTrackerMesh, 
                           TrackerConfigurationReady, ReportInvalidRequest );

  igstkAddTransitionMacro( TrackerConfigurationReady, ConfigureTracker, 
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
                           TrackerConfigurationReady, ReportFailureTrackerInitialization );

  //complete table for state: InitializingTracker
  
  igstkAddTransitionMacro( InitializingTracker, LoadWorkingVolumeMesh, 
                           InitializingTracker, ReportInvalidRequest );
  
  igstkAddTransitionMacro( InitializingTracker, LoadTrackerMesh, 
                           InitializingTracker, ReportInvalidRequest );

  igstkAddTransitionMacro( InitializingTracker, ConfigureTracker, 
                           InitializingTracker, ReportInvalidRequest );

  igstkAddTransitionMacro( InitializingTracker, InitializeTracker, 
                           InitializingTracker, ReportInvalidRequest );
    
  igstkAddTransitionMacro( InitializingTracker, StartTracking, 
                           InitializingTracker, ReportInvalidRequest );

  igstkAddTransitionMacro( InitializingTracker, StopTracking, 
                           InitializingTracker, ReportInvalidRequest );

  igstkAddTransitionMacro( InitializingTracker, DisconnectTracker, 
                           InitializingTracker, ReportInvalidRequest );
  
  /** TrackerInitializationReady State */
  
  igstkAddTransitionMacro( TrackerInitializationReady, LoadWorkingVolumeMesh,
                           LoadingMesh, LoadWorkingVolumeMesh );

  igstkAddTransitionMacro( TrackerInitializationReady, LoadTrackerMesh,
                           LoadingMesh, LoadTrackerMesh );

  igstkAddTransitionMacro( TrackerInitializationReady, StartTracking, 
                           StartingTracker, StartTracking );

  //complete table for state: TrackerInitializationReady
  igstkAddTransitionMacro( TrackerInitializationReady, Success, 
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, Failure,
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, ConfigureTracker,
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, InitializeTracker, 
                           TrackerInitializationReady, ReportInvalidRequest );
  igstkAddTransitionMacro( TrackerInitializationReady, StopTracking,
                           TrackerInitializationReady, ReportInvalidRequest );  
  igstkAddTransitionMacro( TrackerInitializationReady, DisconnectTracker,
                           TrackerInitializationReady, ReportInvalidRequest );

  /** LoadingMesh State */

  igstkAddTransitionMacro( LoadingMesh, Success,
                           TrackerInitializationReady, ReportSuccessMeshLoaded );

  igstkAddTransitionMacro( LoadingMesh, Failure,
                           TrackerInitializationReady, ReportFailuredMeshLoaded );

  //complete table for state: LoadingMesh

  igstkAddTransitionMacro( LoadingMesh, LoadWorkingVolumeMesh, 
                           LoadingMesh, ReportInvalidRequest );

  igstkAddTransitionMacro( LoadingMesh, LoadTrackerMesh, 
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

  /** StartingTracker State*/

  igstkAddTransitionMacro( StartingTracker, Success, 
                           Tracking, ReportSuccessStartTracking);

  igstkAddTransitionMacro( StartingTracker, Failure, 
                           TrackerInitializationReady, ReportFailureStartTracking );

  //complete table for state: StartingTracker
      
  igstkAddTransitionMacro( StartingTracker, LoadWorkingVolumeMesh, 
                           StartingTracker, ReportInvalidRequest );

  igstkAddTransitionMacro( StartingTracker, LoadTrackerMesh, 
                           StartingTracker, ReportInvalidRequest );
  
  igstkAddTransitionMacro( StartingTracker, ConfigureTracker, 
                           StartingTracker, ReportInvalidRequest );

  igstkAddTransitionMacro( StartingTracker, InitializeTracker, 
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

  igstkAddTransitionMacro( Tracking, LoadWorkingVolumeMesh,
                           LoadingMesh, LoadWorkingVolumeMesh );

  igstkAddTransitionMacro( Tracking, LoadTrackerMesh,
                           LoadingMesh, LoadTrackerMesh );

  //complete table for state: Tracking

  igstkAddTransitionMacro( Tracking, Success, 
                           Tracking, ReportInvalidRequest );

  igstkAddTransitionMacro( Tracking, Failure, 
                           Tracking, ReportInvalidRequest );
  
  igstkAddTransitionMacro( Tracking, InitializeTracker, 
                           Tracking, ReportInvalidRequest );

  igstkAddTransitionMacro( Tracking, ConfigureTracker, 
                           Tracking, ReportInvalidRequest );
  
  igstkAddTransitionMacro( Tracking, StartTracking, 
                           Tracking, ReportInvalidRequest );

  igstkAddTransitionMacro( Tracking, DisconnectTracker, 
                           Tracking, ReportInvalidRequest );

   /** StoppingTracker State */

  igstkAddTransitionMacro( StoppingTracker, Failure,
                           Tracking, ReportFailureStopTracking );

  igstkAddTransitionMacro( StoppingTracker, Success,
                           Tracking, ReportSuccessStopTracking );

  //complete table for state: StoppingTracker
  
  igstkAddTransitionMacro( StoppingTracker, LoadWorkingVolumeMesh, 
                           StoppingTracker, ReportInvalidRequest );

  igstkAddTransitionMacro( StoppingTracker, LoadTrackerMesh, 
                           StoppingTracker, ReportInvalidRequest );
  
  igstkAddTransitionMacro( StoppingTracker, InitializeTracker, 
                           StoppingTracker, ReportInvalidRequest );

  igstkAddTransitionMacro( StoppingTracker, ConfigureTracker, 
                           StoppingTracker, ReportInvalidRequest );
  
  igstkAddTransitionMacro( StoppingTracker, StartTracking, 
                           StoppingTracker, ReportInvalidRequest );

  igstkAddTransitionMacro( StoppingTracker, StopTracking, 
                           StoppingTracker, ReportInvalidRequest );

  igstkAddTransitionMacro( StoppingTracker, DisconnectTracker, 
                           StoppingTracker, ReportInvalidRequest );

   /** DisconnectingTracker Tracker */

  igstkAddTransitionMacro( DisconnectingTracker, Success, 
                           TrackerInitializationReady, ReportSuccessTrackerDisconnection);

  igstkAddTransitionMacro( DisconnectingTracker, Failure, 
                           TrackerInitializationReady, ReportFailureTrackerDisconnection);

  //complete table for state: DisconnectingTracker
    
  igstkAddTransitionMacro( DisconnectingTracker, LoadWorkingVolumeMesh, 
                           DisconnectingTracker, ReportInvalidRequest );

  igstkAddTransitionMacro( DisconnectingTracker, LoadTrackerMesh, 
                           DisconnectingTracker, ReportInvalidRequest );
  
  igstkAddTransitionMacro( DisconnectingTracker, InitializeTracker, 
                           DisconnectingTracker, ReportInvalidRequest );

  igstkAddTransitionMacro( DisconnectingTracker, ConfigureTracker, 
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
  ofile.open("WorkingVolumeTesterStateMachineDiagram.dot");
  const bool skipLoops = false;
  this->ExportStateMachineDescription( ofile, skipLoops );
  ofile.close();
*/
}

/** -----------------------------------------------------------------
*     Destructor
*  -----------------------------------------------------------------
*/
WorkingVolumeTester::~WorkingVolumeTester()
{  

}

bool WorkingVolumeTester::ReadMicronConfiguration(igstk::TrackerConfigurationFileReader::Pointer reader)
{
  igstk::TrackerConfigurationXMLFileReaderBase::Pointer trackerCofigurationXMLReader;
  
  trackerCofigurationXMLReader = igstk::MicronConfigurationXMLFileReader::New();

  reader->RequestSetReader( trackerCofigurationXMLReader );  

   //need to observe if the request read succeeds or fails
   //there is a third option that the read is invalid, if the
   //file name or xml reader weren't set
  ReadTrackerConfigurationFailSuccessObserver::Pointer trackerReaderObserver = 
                                ReadTrackerConfigurationFailSuccessObserver::New();

  reader->AddObserver( igstk::TrackerConfigurationFileReader::ReadSuccessEvent(),
                                    trackerReaderObserver );
  reader->AddObserver( igstk::TrackerConfigurationFileReader::ReadFailureEvent(),
                                    trackerReaderObserver );
  reader->RequestRead();

  if( trackerReaderObserver->GotFailure() )
  {
      igstkLogMacro2( m_Logger, DEBUG, 
        "WorkingVolumeTester::ReadMicronConfiguration error: "\
        << trackerReaderObserver->GetFailureMessage() << "\n" )
      return false;
  }

  if( !trackerReaderObserver->GotSuccess() )
  {
     igstkLogMacro2( m_Logger, DEBUG, 
        "WorkingVolumeTester::ReadMicronConfiguration error: could not read MICRON tracker configuration file\n")
     return false;
  }

  //get the configuration data from the reader
  TrackerConfigurationObserver::Pointer trackerConfigurationObserver = 
    TrackerConfigurationObserver::New();

  reader->AddObserver( 
    igstk::TrackerConfigurationFileReader::TrackerConfigurationDataEvent(), trackerConfigurationObserver );

  reader->RequestGetData();
  
  if( !trackerConfigurationObserver->GotTrackerConfiguration() )
  {
     igstkLogMacro2( m_Logger, DEBUG, 
        "WorkingVolumeTester::ReadMicronConfiguration error: could not get MICRON tracker configuration\n")
     return false;
  }

  m_TrackerConfiguration = trackerConfigurationObserver->GetTrackerConfiguration();

  return true;
}

bool WorkingVolumeTester::ReadAuroraConfiguration(igstk::TrackerConfigurationFileReader::Pointer reader)
{

  igstk::TrackerConfigurationXMLFileReaderBase::Pointer 
                                                        trackerCofigurationXMLReader;

  trackerCofigurationXMLReader = igstk::AuroraConfigurationXMLFileReader::New();

  //setting the file name and reader always succeeds so I don't
             //observe the trackerConfigReader for their success events
 // trackerConfigReader->RequestSetFileName( TRACKER_CONFIGURATION_XML );
  reader->RequestSetReader( trackerCofigurationXMLReader );

   //need to observe if the request read succeeds or fails
   //there is a third option that the read is invalid, if the
   //file name or xml reader weren't set
  ReadTrackerConfigurationFailSuccessObserver::Pointer trackerReaderObserver = 
                                ReadTrackerConfigurationFailSuccessObserver::New();

  reader->AddObserver( igstk::TrackerConfigurationFileReader::ReadSuccessEvent(),
                                    trackerReaderObserver );
  reader->AddObserver( igstk::TrackerConfigurationFileReader::ReadFailureEvent(),
                                    trackerReaderObserver );
  reader->RequestRead();

  if( trackerReaderObserver->GotFailure() )
  {
      igstkLogMacro2( m_Logger, DEBUG, 
        "WorkingVolumeTester::ReadAuroraConfiguration error: "\
        << trackerReaderObserver->GetFailureMessage() << "\n" )
      return false;
  }

  if( !trackerReaderObserver->GotSuccess() )
  {
     igstkLogMacro2( m_Logger, DEBUG, 
        "WorkingVolumeTester::ReadAuroraConfiguration error: could not read AURORA tracker configuration file\n")
     return false;
  }

  //get the configuration data from the reader
  TrackerConfigurationObserver::Pointer trackerConfigurationObserver = 
    TrackerConfigurationObserver::New();

  reader->AddObserver( 
    igstk::TrackerConfigurationFileReader::TrackerConfigurationDataEvent(), trackerConfigurationObserver );

  reader->RequestGetData();
  
  if( !trackerConfigurationObserver->GotTrackerConfiguration() )
  {
     igstkLogMacro2( m_Logger, DEBUG, 
        "WorkingVolumeTester::ReadAuroraConfiguration error: could not get AURORA tracker configuration\n")
     return false;
  }

  m_TrackerConfiguration = trackerConfigurationObserver->GetTrackerConfiguration();

  return true;
}

void WorkingVolumeTester::ConfigureTrackerProcessing()
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

  igstk::TrackerConfigurationFileReader::Pointer trackerConfigReader = 
    igstk::TrackerConfigurationFileReader::New();
    //setting the file name and reader always succeeds so I don't
             //observe the trackerConfigReader for their success events
  trackerConfigReader->RequestSetFileName( fileName );

  if ( this->ReadAuroraConfiguration( trackerConfigReader ) )
  {
    m_StateMachine.PushInput( m_SuccessInput );
    m_StateMachine.ProcessInputs();
  }
  else if ( this->ReadMicronConfiguration( trackerConfigReader ) )
  {
    m_StateMachine.PushInput( m_SuccessInput );
    m_StateMachine.ProcessInputs();
  }
  else
  {
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
  } 
}

void WorkingVolumeTester::RequestLoadTrackerMesh()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "WorkingVolumeTester::RequestLoadTrackerMesh called...\n" )
  m_StateMachine.PushInput( m_LoadTrackerMeshInput );
  m_StateMachine.ProcessInputs();
}

void WorkingVolumeTester::RequestLoadWorkingVolumeMesh()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "WorkingVolumeTester::RequestLoadWorkingVolumeMesh called...\n" )
  m_StateMachine.PushInput( m_LoadWorkingVolumeMeshInput );
  m_StateMachine.ProcessInputs();
}

void WorkingVolumeTester::RequestConfigureTracker()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "WorkingVolumeTester::RequestConfigureTracker called...\n" )

  m_StateMachine.PushInput( m_ConfigureTrackerInput );
  m_StateMachine.ProcessInputs();
}

void WorkingVolumeTester::RequestInitializeTracker()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "WorkingVolumeTester::RequestInitializeTracker called...\n" )

  m_StateMachine.PushInput( m_InitializeTrackerInput );
  m_StateMachine.ProcessInputs();
}

void WorkingVolumeTester::RequestStartTracking()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "WorkingVolumeTester::RequestStartTracking called...\n" )

  m_StateMachine.PushInput( m_StartTrackingInput );
  m_StateMachine.ProcessInputs();  
}

void WorkingVolumeTester::RequestStopTracking()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "WorkingVolumeTester::RequestStopTracking called...\n" )

  m_StateMachine.PushInput( m_StopTrackingInput );
  m_StateMachine.ProcessInputs();  
}

void WorkingVolumeTester::RequestDisconnectTracker()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "WorkingVolumeTester::RequestDisconnectTracker called...\n" )

  m_StateMachine.PushInput( m_DisconnectTrackerInput );
  m_StateMachine.ProcessInputs();  
}

/** Method to be invoked when no operation is required */
void WorkingVolumeTester::NoProcessing()
{

}

/** Method to be invoked when an invalid operation was requested */
void 
WorkingVolumeTester::ReportInvalidRequestProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::WorkingVolumeTester::"
                 "ReportInvalidRequestProcessing called...\n");
  this->InvokeEvent(InvalidRequestErrorEvent());
}

/** Method to be invoked on successful Starting Application */
//void 
//WorkingVolumeTester::ReportSuccessStartingApplicationProcessing()
//{
//  igstkLogMacro2( m_Logger, DEBUG, "igstk::WorkingVolumeTester::"
//                 "ReportSuccessStartingApplicationProcessing called...\n");    
//
////  this->RequestConfigureTracker();
//}

/** Method to be invoked on failured Starting Application */
//void 
//WorkingVolumeTester::ReportFailuredStartingApplicationProcessing()
//{
//  igstkLogMacro2( m_Logger, DEBUG, "igstk::WorkingVolumeTester::"
//                 "ReportFailuredStartingApplicationProcessing called...\n");
//
//}

/** Method to be invoked on successful mesh loading */
void 
WorkingVolumeTester::ReportSuccessMeshLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::WorkingVolumeTester::"
                 "ReportSuccessMeshLoadedProcessing called...\n");
}

/** Method to be invoked on failured mesh loading */
void 
WorkingVolumeTester::ReportFailuredMeshLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::WorkingVolumeTester::"
                 "ReportFailuredMeshLoadedProcessing called...\n");
}


/** Method to be invoked on successful tracker configuration */
void 
WorkingVolumeTester::ReportSuccessTrackerConfigurationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::WorkingVolumeTester::"
                 "ReportSuccessTrackerConfigurationProcessing called...\n");

  this->RequestInitializeTracker();
}

/** Method to be invoked on failured tracker configuration */
void 
WorkingVolumeTester::ReportFailureTrackerConfigurationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::WorkingVolumeTester::"
                 "ReportFailureTrackerConfigurationProcessing called...\n");
}

/** Method to be invoked on failured tracker initialization */
void 
WorkingVolumeTester::ReportFailureTrackerInitializationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::WorkingVolumeTester::"
                 "ReportFailureTrackerConfigurationProcessing called...\n");
}


/** Method to be invoked on successful tracker initialization */
void 
WorkingVolumeTester::ReportSuccessTrackerInitializationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::WorkingVolumeTester::"
                 "ReportSuccessTrackerInitializationProcessing called...\n");

  this->RequestStartTracking();
}

/** Method to be invoked on failure tracker disconnection */
void 
WorkingVolumeTester::ReportFailureTrackerDisconnectionProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::WorkingVolumeTester::"
                 "ReportFailureTrackerDisconnectionProcessing called...\n");
}

/** Method to be invoked on successful tracker disconnection */
void 
WorkingVolumeTester::ReportSuccessTrackerDisconnectionProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::WorkingVolumeTester::"
                 "ReportSuccessTrackerDisconnectionProcessing called...\n");  
}

/** Method to be invoked on successful tracker start */
void 
WorkingVolumeTester::ReportSuccessStartTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::WorkingVolumeTester::"
                 "ReportSuccessStartTrackingProcessing called...\n")
  
//  m_RunStopButton->label("Stop");
  //m_RunStopButton->deactivate();

//  Fl::check();
}


/** Method to be invoked on failured tracker start */
void 
WorkingVolumeTester::ReportFailureStartTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::WorkingVolumeTester::"
                 "ReportFailureStartTrackingProcessing called...\n")

  Fl::check();
}

/** Method to be invoked on failured tracker stop */
void 
WorkingVolumeTester::ReportFailureStopTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::WorkingVolumeTester::"
                 "ReportFailureStopTrackingProcessing called...\n")
}

/** Method to be invoked on successful tracker stop */
void 
WorkingVolumeTester::ReportSuccessStopTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::WorkingVolumeTester::"
                 "ReportSuccessStopTrackingProcessing called...\n")
   
//  m_RunStopButton->label("Run");

  Fl::check();
}

/** -----------------------------------------------------------------
* Build scene graph with all detected tracking tools
*  -----------------------------------------------------------------
*/
void WorkingVolumeTester::BuildSceneGraph()
{
  
 
}

/** -----------------------------------------------------------------
* Load working volume mesh. This method asks for a .msh file
* -----------------------------------------------------------------
*/
void WorkingVolumeTester::LoadWorkingVolumeMeshProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "WorkingVolumeTester::LoadWorkingVolumeMeshProcessing called...\n" )

  const char*  fileName = 
    fl_file_chooser("Select the mesh file","*.msh", "");

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

     m_MeshSpatialObject = observer->GetMeshObject();   
     if ( m_MeshSpatialObject.IsNull() )
     {
       igstkLogMacro2( m_Logger, DEBUG, "Cannot read mesh\n" )
       m_StateMachine.PushInput( m_FailureInput);
       m_StateMachine.ProcessInputs();
       return;
     }

     igstk::Transform identity;
     identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
     
     // we want the mesh to be a child of the tracker
     m_MeshSpatialObject->RequestDetachFromParent();
     m_TrackerController->RequestAddChildSpatialObject( identity, m_MeshSpatialObject );
     
     MeshRepresentationType::Pointer m_MeshRepresentation = MeshRepresentationType::New();
     m_MeshRepresentation->RequestSetMeshObject( m_MeshSpatialObject );

     m_MeshRepresentation->SetOpacity(0.3);      
     m_MeshRepresentation->SetColor(0, 0, 1);
     
     m_ViewerGroup->m_AxialView->RequestAddObject( m_MeshRepresentation->Copy() );
     m_ViewerGroup->m_SagittalView->RequestAddObject( m_MeshRepresentation->Copy() );
     m_ViewerGroup->m_CoronalView->RequestAddObject( m_MeshRepresentation->Copy() );
     m_ViewerGroup->m_3DView->RequestAddObject( m_MeshRepresentation->Copy() );

     //// Set up cross hairs
     // m_CrossHair = CrossHairType::New();
     // m_CrossHair->RequestSetReferenceSpatialObject( m_MeshSpatialObject );      
     // m_CrossHair->RequestSetToolSpatialObject( m_TipSpatialObjectVector[1] );
     // m_CrossHair->RequestSetTransformAndParent( identity, m_WorldReference );

     // // set up the cross hair representation and add the cross hair object
     // m_CrossHairRepresentation = CrossHairRepresentationType::New();
     // m_CrossHairRepresentation->SetColor(1,0,0);
     // m_CrossHairRepresentation->RequestSetCrossHairObject( m_CrossHair );  

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

     
     m_ViewerGroup->m_AxialView->RequestResetCamera();
     m_ViewerGroup->m_SagittalView->RequestResetCamera();
     m_ViewerGroup->m_CoronalView->RequestResetCamera();
     m_ViewerGroup->m_3DView->RequestResetCamera();

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
* Load tracker mesh. This method asks for a .msh file
* -----------------------------------------------------------------
*/
void WorkingVolumeTester::LoadTrackerMeshProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "WorkingVolumeTester::LoadTrackerMeshProcessing called...\n" )

  const char*  fileName = 
    fl_file_chooser("Select the mesh file","*.msh", "");

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

     m_MeshSpatialObject = observer->GetMeshObject();   
     if ( m_MeshSpatialObject.IsNull() )
     {
       igstkLogMacro2( m_Logger, DEBUG, "Cannot read mesh\n" )
       m_StateMachine.PushInput( m_FailureInput);
       m_StateMachine.ProcessInputs();
       return;
     }

     igstk::Transform identity;
     identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );
     
     // we want the mesh to be a child of the tracker
     m_MeshSpatialObject->RequestDetachFromParent();
     m_TrackerController->RequestAddChildSpatialObject( identity, m_MeshSpatialObject );
     
     MeshRepresentationType::Pointer m_MeshRepresentation = MeshRepresentationType::New();
     m_MeshRepresentation->RequestSetMeshObject( m_MeshSpatialObject );

     m_MeshRepresentation->SetOpacity(0.3);      
     m_MeshRepresentation->SetColor(1,1,0);
     
     m_ViewerGroup->m_AxialView->RequestAddObject( m_MeshRepresentation->Copy() );
     m_ViewerGroup->m_SagittalView->RequestAddObject( m_MeshRepresentation->Copy() );
     m_ViewerGroup->m_CoronalView->RequestAddObject( m_MeshRepresentation->Copy() );
     m_ViewerGroup->m_3DView->RequestAddObject( m_MeshRepresentation->Copy() );
     
     m_ViewerGroup->m_AxialView->RequestResetCamera();
     m_ViewerGroup->m_SagittalView->RequestResetCamera();
     m_ViewerGroup->m_CoronalView->RequestResetCamera();
     m_ViewerGroup->m_3DView->RequestResetCamera();

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

void WorkingVolumeTester::InitializeTrackerProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "WorkingVolumeTester::InitializeTrackerProcessing called...\n" )

/*
  std::ofstream ofile;
  ofile.open("TrackerControllerStateMachineDiagram.dot");
  this->m_TrackerController->ExportStateMachineDescription( ofile, true );
*/

  if (!m_TrackerConfiguration)
  {
    igstkLogMacro2( m_Logger, DEBUG, "WorkingVolumeTester::InitializeTrackerProcessing\
                                     There is no tracker configuration\n" )
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

  m_TrackerController->RequestGetNonReferenceToolList();
 // m_TrackerController->RequestGetReferenceTool();
  
  m_StateMachine.PushInput( m_SuccessInput );
  m_StateMachine.ProcessInputs();
  return;
}

/** -----------------------------------------------------------------
* Starts tracking provided it is initialized and connected to the 
* communication port
*---------------------------------------------------------------------
*/
void WorkingVolumeTester::StartTrackingProcessing()
{
    igstkLogMacro2( m_Logger, DEBUG, 
                    "WorkingVolumeTester::StartTrackingProcessing called...\n" ) 

    igstk::Transform identity;
    identity.SetToIdentity(igstk::TimeStamp::GetLongestPossibleTime());

    m_TrackerController->RequestSetParentSpatialObject( identity, m_WorldReference );

    if (!m_TrackerConfiguration)
    {
      igstkLogMacro2( m_Logger, DEBUG, "WorkingVolumeTester::StartTrackingProcessing \
                                       There is not tracker configuration object\n" )
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
      igstkLogMacro2( m_Logger, DEBUG, "Tracker start error\n" )
      m_StateMachine.PushInput( m_FailureInput );
      m_StateMachine.ProcessInputs();
      return;
    }    

    // create a world reference system
    m_WorldReferenceRepresentation = AxesRepresentationType::New();
    m_WorldReferenceRepresentation->RequestSetAxesObject( m_WorldReference );
    m_WorldReference->SetSize(50,50,50);

    ToolVectorType::iterator iter = m_ToolVector.begin();

    int toolCounter = 0;        

    for ( ; iter != m_ToolVector.end(); iter++, toolCounter++ )
    {
      EllipsoidType::Pointer so = EllipsoidType::New();
      so->SetRadius(10,10,10);
      so->RequestDetachFromParent();
      so->RequestSetTransformAndParent( identity, (*iter));

      m_TipSpatialObjectVector.push_back( so );

      EllipsoidRepresentationType::Pointer rep = EllipsoidRepresentationType::New();
      rep->RequestSetEllipsoidObject( so );

      double r = ( ( ( double ) ( std::rand( ) ) ) / ( ( double ) ( RAND_MAX ) ) );
      double g = ( ( ( double ) ( std::rand( ) ) ) / ( ( double ) ( RAND_MAX ) ) );
      double b = ( ( ( double ) ( std::rand( ) ) ) / ( ( double ) ( RAND_MAX ) ) );

      rep->SetColor(r, g, b);

      m_TipRepresentationVector.push_back( rep );

      m_ViewerGroup->m_AxialView->RequestAddObject( rep->Copy() );
      m_ViewerGroup->m_SagittalView->RequestAddObject( rep->Copy() );
      m_ViewerGroup->m_CoronalView->RequestAddObject( rep->Copy() );
      m_ViewerGroup->m_3DView->RequestAddObject( rep->Copy() );
    }    

    // set views' background colors
    m_ViewerGroup->m_AxialView->SetRendererBackgroundColor(0,0,0);
    m_ViewerGroup->m_SagittalView->SetRendererBackgroundColor(0,0,0);
    m_ViewerGroup->m_CoronalView->SetRendererBackgroundColor(0,0,0);
    m_ViewerGroup->m_3DView->SetRendererBackgroundColor(1,1,1); 

    // build scene graph
    m_ViewerGroup->m_AxialView->RequestSetTransformAndParent(
      identity, m_WorldReference );

    m_ViewerGroup->m_SagittalView->RequestSetTransformAndParent(
      identity, m_WorldReference );

    m_ViewerGroup->m_CoronalView->RequestSetTransformAndParent(
      identity, m_WorldReference );

    m_ViewerGroup->m_3DView->RequestSetTransformAndParent(
      identity, m_ViewerGroup->m_AxialView );

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

    // add world reference to the views
    m_ViewerGroup->m_AxialView->RequestAddObject(m_WorldReferenceRepresentation->Copy());
    m_ViewerGroup->m_SagittalView->RequestAddObject(m_WorldReferenceRepresentation->Copy());
    m_ViewerGroup->m_CoronalView->RequestAddObject(m_WorldReferenceRepresentation->Copy());
    m_ViewerGroup->m_3DView->RequestAddObject(m_WorldReferenceRepresentation->Copy());  

    // add tool´s tip to the views
    //m_ViewerGroup->m_AxialView->RequestAddObject(m_TipRepresentation->Copy());
    //m_ViewerGroup->m_SagittalView->RequestAddObject(m_TipRepresentation->Copy());
    //m_ViewerGroup->m_CoronalView->RequestAddObject(m_TipRepresentation->Copy());
    //m_ViewerGroup->m_3DView->RequestAddObject(m_TipRepresentation->Copy());

    m_ViewerGroup->m_AxialView->RequestResetCamera();
    m_ViewerGroup->m_SagittalView->RequestResetCamera();
    m_ViewerGroup->m_CoronalView->RequestResetCamera();
    m_ViewerGroup->m_3DView->RequestResetCamera();

    m_StateMachine.PushInput( m_SuccessInput );
    m_StateMachine.ProcessInputs();

  return;
}
/** -----------------------------------------------------------------
* Stops tracking but keeps the tracker connected to the 
* communication port
*---------------------------------------------------------------------
*/
void WorkingVolumeTester::StopTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "WorkingVolumeTester::StopTrackingProcessing called...\n" )

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
void WorkingVolumeTester::DisconnectTrackerProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "WorkingVolumeTester::DisconnectTrackerProcessing called...\n" )
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


void WorkingVolumeTester::TrackerControllerObserver::SetParent( WorkingVolumeTester *p ) 
{
  m_Parent = p;
}

void WorkingVolumeTester::TrackerControllerObserver::Execute( const itk::Object *caller, 
                                                    const itk::EventObject & event )
{
  const itk::Object * constCaller = caller;
  this->Execute(constCaller, event);
}
  
void 
WorkingVolumeTester::TrackerControllerObserver::Execute( itk::Object *caller, 
                                                             const itk::EventObject & event )
{
  const igstk::TrackerController::InitializeErrorEvent *evt1a =
    dynamic_cast< const igstk::TrackerController::InitializeErrorEvent * > (&event);

  const igstk::TrackerStartTrackingErrorEvent *evt1b =
    dynamic_cast< const igstk::TrackerStartTrackingErrorEvent * > (&event);

  const igstk::TrackerStopTrackingErrorEvent *evt1c =
    dynamic_cast< const igstk::TrackerStopTrackingErrorEvent * > (&event);

  //const igstk::TrackerController::RequestTrackerEvent *evt2 =
  //  dynamic_cast< const igstk::TrackerController::RequestTrackerEvent * > (&event);

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
  /*else if ( evt2 )
  {
    m_Parent->m_Tracker = evt2->Get();
  }*/
  else if ( evt3 )
  {
    igstk::TrackerController::ToolContainerType toolContainer = evt3->Get();

    
    igstk::TrackerController::ToolContainerType::iterator iter = toolContainer.begin();

    for ( ; iter != toolContainer.end(); iter++ )
    {
      m_Parent->m_ToolVector.push_back( (*iter).second );
    }
      //toolContainer.find("needle");

    /*if ( iter!=toolContainer.end() )
    {      
        m_Parent->m_PointerTool = (*iter).second;
    }*/

  }
  else if ( evt4 )
  {
   /* igstk::TrackerController::ToolEntryType entry = evt4->Get();
      if ( entry.first == "reference" )
        m_Parent->m_ReferenceTool = entry.second;*/
  }
}


void
WorkingVolumeTester
::RequestPrepareToQuit()
{
  this->RequestDisconnectTracker();
}


igstk::Transform *
WorkingVolumeTester::
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
       /* }
        else
        {
          return NULL;
        }*/
     // }
    }
    else return NULL;
  }
  else return NULL;
}

