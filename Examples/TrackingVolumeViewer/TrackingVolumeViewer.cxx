/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    TrackingVolumeViewer.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "TrackingVolumeViewer.h"

#include "FL/Fl_File_Chooser.H"
#include "FL/Fl_Input.H"

#include "itksys/SystemTools.hxx"
#include "itksys/Directory.hxx"
#include "igstkTransformObserver.h"

#include "igstkMicronTrackerConfiguration.h"
#include "igstkAuroraTrackerConfiguration.h"
#include "igstkPolarisTrackerConfiguration.h"
#include "igstkAscensionTrackerConfiguration.h"

#include "igstkAuroraConfigurationXMLFileReader.h"
#include "igstkAscensionConfigurationXMLFileReader.h"
#include "igstkAscension3DGConfigurationXMLFileReader.h"
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
TrackingVolumeViewer::TrackingVolumeViewer() : 
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
  logFileName = "logTrackingVolumeViewer"
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
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, LoadTrackerMesh,
                           Initial, ReportInvalidRequest );
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

  igstkAddTransitionMacro( TrackerInitializationReady, DisconnectTracker, 
                           DisconnectingTracker, DisconnectTracker );

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

  igstkAddTransitionMacro( StoppingTracker, Success,
                           TrackerInitializationReady, ReportSuccessStopTracking );

  igstkAddTransitionMacro( StoppingTracker, Failure,
                           Tracking, ReportFailureStopTracking );

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

  std::ofstream ofile;
  ofile.open("TrackingVolumeViewerStateMachineDiagram.dot");
  const bool skipLoops = false;
  this->ExportStateMachineDescription( ofile, skipLoops );
  ofile.close();

}

/** -----------------------------------------------------------------
*     Destructor
*  -----------------------------------------------------------------
*/
TrackingVolumeViewer::~TrackingVolumeViewer()
{  

}

void TrackingVolumeViewer::ConfigureTrackerProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "TrackingVolumeViewer::ConfigureTrackerProcessing called...\n" )
 
  const char*  fileName = 
    fl_file_chooser("Select a tracker configuration file","*.xml", "auroraConfiguration.xml");

  if ( fileName == NULL )
  {
      igstkLogMacro2( m_Logger, DEBUG, 
             "TrackingVolumeViewer::ConfigureTrackerProcessing none file was selected or operation canceled...\n" )
      igstkPushInputMacro( Failure );
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

  TrackerConfigurationObserver::Pointer tco = TrackerConfigurationObserver::New();

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
     ReportError( rfso->GetFailureMessage() );
     igstkLogMacro2( m_Logger, DEBUG, "Tracker Configuration error\n" )
     igstkPushInputMacro( Failure );
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
       igstkPushInputMacro( Success );
     }
     else
     {
       igstkLogMacro2( m_Logger, DEBUG, "Could not get tracker configuration error\n" )
       igstkPushInputMacro( Failure );       
     }

     m_StateMachine.ProcessInputs();
     return; 
   }
   else
   {
    // just to complete all possibilities
       igstkLogMacro2( m_Logger, DEBUG, "Very strange tracker configuration error\n" )
       igstkPushInputMacro( Failure );
       m_StateMachine.ProcessInputs();
       return;
   }
  } // for
}

void TrackingVolumeViewer::RequestLoadTrackerMesh()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "TrackingVolumeViewer::RequestLoadTrackerMesh called...\n" )
  igstkPushInputMacro( LoadTrackerMesh );
  m_StateMachine.ProcessInputs();
}

void TrackingVolumeViewer::RequestLoadWorkingVolumeMesh()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "TrackingVolumeViewer::RequestLoadWorkingVolumeMesh called...\n" )
  igstkPushInputMacro( LoadWorkingVolumeMesh );
  m_StateMachine.ProcessInputs();
}

void TrackingVolumeViewer::RequestConfigureTracker()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "TrackingVolumeViewer::RequestConfigureTracker called...\n" )
  igstkPushInputMacro( ConfigureTracker );
  m_StateMachine.ProcessInputs();
}

void TrackingVolumeViewer::RequestInitializeTracker()
{
  igstkLogMacro2( m_Logger, DEBUG, 
             "TrackingVolumeViewer::RequestInitializeTracker called...\n" )
  igstkPushInputMacro( InitializeTracker );
  m_StateMachine.ProcessInputs();
}

void TrackingVolumeViewer::RequestStartTracking()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "TrackingVolumeViewer::RequestStartTracking called...\n" )
  igstkPushInputMacro( StartTracking );
  m_StateMachine.ProcessInputs();  
}

void TrackingVolumeViewer::RequestStopTracking()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "TrackingVolumeViewer::RequestStopTracking called...\n" )
  igstkPushInputMacro( StopTracking );
  m_StateMachine.ProcessInputs();  
}

void TrackingVolumeViewer::RequestDisconnectTracker()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "TrackingVolumeViewer::RequestDisconnectTracker called...\n" )
  igstkPushInputMacro( DisconnectTracker );
  m_StateMachine.ProcessInputs();  
}

/** Method to be invoked when no operation is required */
void TrackingVolumeViewer::NoProcessing()
{

}

/** Method to be invoked when an invalid operation was requested */
void 
TrackingVolumeViewer::ReportInvalidRequestProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::TrackingVolumeViewer::"
                 "ReportInvalidRequestProcessing called...\n");
  this->InvokeEvent(InvalidRequestErrorEvent());
}

/** Method to be invoked on successful mesh loading */
void 
TrackingVolumeViewer::ReportSuccessMeshLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::TrackingVolumeViewer::"
                 "ReportSuccessMeshLoadedProcessing called...\n");
}

/** Method to be invoked on failured mesh loading */
void 
TrackingVolumeViewer::ReportFailuredMeshLoadedProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::TrackingVolumeViewer::"
                 "ReportFailuredMeshLoadedProcessing called...\n");
  ReportError("Failed loading mesh file.");
}


/** Method to be invoked on successful tracker configuration */
void 
TrackingVolumeViewer::ReportSuccessTrackerConfigurationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::TrackingVolumeViewer::"
                 "ReportSuccessTrackerConfigurationProcessing called...\n");

  this->RequestInitializeTracker();
}

/** Method to be invoked on failured tracker configuration */
void 
TrackingVolumeViewer::ReportFailureTrackerConfigurationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::TrackingVolumeViewer::"
                 "ReportFailureTrackerConfigurationProcessing called...\n");
}

/** Method to be invoked on failured tracker initialization */
void 
TrackingVolumeViewer::ReportFailureTrackerInitializationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::TrackingVolumeViewer::"
                 "ReportFailureTrackerConfigurationProcessing called...\n");
}


/** Method to be invoked on successful tracker initialization */
void 
TrackingVolumeViewer::ReportSuccessTrackerInitializationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::TrackingVolumeViewer::"
                 "ReportSuccessTrackerInitializationProcessing called...\n");

  this->RequestStartTracking();
}

/** Method to be invoked on failure tracker disconnection */
void 
TrackingVolumeViewer::ReportFailureTrackerDisconnectionProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::TrackingVolumeViewer::"
                 "ReportFailureTrackerDisconnectionProcessing called...\n");
}

/** Method to be invoked on successful tracker disconnection */
void 
TrackingVolumeViewer::ReportSuccessTrackerDisconnectionProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::TrackingVolumeViewer::"
                 "ReportSuccessTrackerDisconnectionProcessing called...\n");  
}

/** Method to be invoked on successful tracker start */
void 
TrackingVolumeViewer::ReportSuccessStartTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::TrackingVolumeViewer::"
                 "ReportSuccessStartTrackingProcessing called...\n")
  this->m_LoadTrackerMeshButton->activate();
  this->m_LoadWorkVolumeMeshButton->activate();
}

/** Method to be invoked on failured tracker start */
void 
TrackingVolumeViewer::ReportFailureStartTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::TrackingVolumeViewer::"
                 "ReportFailureStartTrackingProcessing called...\n")
}

/** Method to be invoked on failured tracker stop */
void 
TrackingVolumeViewer::ReportFailureStopTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::TrackingVolumeViewer::"
                 "ReportFailureStopTrackingProcessing called...\n")
}

/** Method to be invoked on successful tracker stop */
void 
TrackingVolumeViewer::ReportSuccessStopTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "igstk::TrackingVolumeViewer::"
                 "ReportSuccessStopTrackingProcessing called...\n")
}

/** -----------------------------------------------------------------
* Load working volume mesh. This method asks for a .msh file
* -----------------------------------------------------------------
*/
void TrackingVolumeViewer::LoadWorkingVolumeMeshProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
           "TrackingVolumeViewer::LoadWorkingVolumeMeshProcessing called...\n" )

    const char*  fileName = fl_file_chooser("Select the mesh file","*.msh", "");

    if ( !fileName )
    {
       igstkLogMacro2( m_Logger, DEBUG, "No file was selected\n" )
       igstkPushInputMacro( Failure );
       m_StateMachine.ProcessInputs();
       return;
    }

    MeshReaderType::Pointer reader = MeshReaderType::New();
    reader->RequestSetFileName( fileName );
    reader->RequestReadObject();
    MeshObjectObserver::Pointer observer = MeshObjectObserver::New();
    reader->AddObserver( igstk::MeshReader::MeshModifiedEvent(), observer);
    reader->RequestGetOutput();

    if(!observer->GotMeshObject())
    {
       igstkLogMacro2( m_Logger, DEBUG, "Cannot read mesh\n" )
       igstkPushInputMacro( Failure );
       m_StateMachine.ProcessInputs();
       return;
    }

    m_MeshSpatialObject = observer->GetMeshObject();   
    if ( m_MeshSpatialObject.IsNull() )
    {
     igstkLogMacro2( m_Logger, DEBUG, "Cannot read mesh\n" )
     igstkPushInputMacro( Failure );
     m_StateMachine.ProcessInputs();
     return;
    }

    igstk::Transform identity;
    identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

    // we want the mesh spatial object to be a child of the tracker
    m_MeshSpatialObject->RequestDetachFromParent();
    m_TrackerController->RequestAddChildSpatialObject( identity, m_MeshSpatialObject );

    // build the mesh representation
    MeshRepresentationType::Pointer m_MeshRepresentation = MeshRepresentationType::New();
    m_MeshRepresentation->RequestSetMeshObject( m_MeshSpatialObject );
    m_MeshRepresentation->SetOpacity(0.3);      
    m_MeshRepresentation->SetColor(0, 0, 1);

    // add the mesh representation only to the 3D view
    m_ViewerGroup->m_3DView->RequestAddObject( m_MeshRepresentation );

    // create reslice plane spatial object for axial view
    m_AxialPlaneSpatialObject = ReslicerPlaneType::New();
    m_AxialPlaneSpatialObject->RequestSetReslicingMode( ReslicerPlaneType::Orthogonal );
    m_AxialPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Axial );
    m_AxialPlaneSpatialObject->RequestSetBoundingBoxProviderSpatialObject( m_MeshSpatialObject );
    m_AxialPlaneSpatialObject->RequestSetToolSpatialObject( m_TipSpatialObjectVector[0] );
    m_AxialPlaneSpatialObject->RequestSetTransformAndParent( identity, m_WorldReference );

    // create reslice plane spatial object for sagittal view
    m_SagittalPlaneSpatialObject = ReslicerPlaneType::New();
    m_SagittalPlaneSpatialObject->RequestSetReslicingMode( ReslicerPlaneType::Orthogonal );
    m_SagittalPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Sagittal );
    m_SagittalPlaneSpatialObject->RequestSetBoundingBoxProviderSpatialObject( m_MeshSpatialObject );
    m_SagittalPlaneSpatialObject->RequestSetToolSpatialObject( m_TipSpatialObjectVector[0] );
    m_SagittalPlaneSpatialObject->RequestSetTransformAndParent( identity, m_WorldReference );

    // create reslice plane spatial object for coronal view
    m_CoronalPlaneSpatialObject = ReslicerPlaneType::New();
    m_CoronalPlaneSpatialObject->RequestSetReslicingMode( ReslicerPlaneType::Orthogonal );
    m_CoronalPlaneSpatialObject->RequestSetOrientationType( ReslicerPlaneType::Coronal );
    m_CoronalPlaneSpatialObject->RequestSetBoundingBoxProviderSpatialObject( m_MeshSpatialObject );
    m_CoronalPlaneSpatialObject->RequestSetToolSpatialObject( m_TipSpatialObjectVector[0] );
    m_CoronalPlaneSpatialObject->RequestSetTransformAndParent( identity, m_WorldReference );

    // create a mesh reslice representation for axial view
    MeshResliceRepresentationType::Pointer m_AxialMeshResliceRepresentation = MeshResliceRepresentationType::New();
    m_AxialMeshResliceRepresentation->RequestSetMeshObject( m_MeshSpatialObject );
    m_AxialMeshResliceRepresentation->RequestSetReslicePlaneSpatialObject( m_AxialPlaneSpatialObject );
    m_AxialMeshResliceRepresentation->SetOpacity(1.0);
    m_AxialMeshResliceRepresentation->SetLineWidth(1.0);
    m_AxialMeshResliceRepresentation->SetColor(1, 1, 0);

    // create a mesh reslice representation for sagittal view
    MeshResliceRepresentationType::Pointer m_SagittalMeshResliceRepresentation = MeshResliceRepresentationType::New();
    m_SagittalMeshResliceRepresentation->RequestSetMeshObject( m_MeshSpatialObject );
    m_SagittalMeshResliceRepresentation->RequestSetReslicePlaneSpatialObject( m_SagittalPlaneSpatialObject );
    m_SagittalMeshResliceRepresentation->SetOpacity(1.0);
    m_SagittalMeshResliceRepresentation->SetLineWidth(1.0);
    m_SagittalMeshResliceRepresentation->SetColor(1, 1, 0);

    // create a mesh reslice representation for coronal view
    MeshResliceRepresentationType::Pointer m_CoronalMeshResliceRepresentation = MeshResliceRepresentationType::New();
    m_CoronalMeshResliceRepresentation->RequestSetMeshObject( m_MeshSpatialObject );
    m_CoronalMeshResliceRepresentation->RequestSetReslicePlaneSpatialObject( m_CoronalPlaneSpatialObject );
    m_CoronalMeshResliceRepresentation->SetOpacity(1.0);
    m_CoronalMeshResliceRepresentation->SetLineWidth(1.0);
    m_CoronalMeshResliceRepresentation->SetColor(1, 1, 0);

    // todo: see the camera issue within childs and parents for the case of
    // a View and a ReslicerPlaneSpatialObjet, respectively.
    
    m_ViewerGroup->m_AxialView->RequestDetachFromParent();
    m_ViewerGroup->m_AxialView->RequestSetTransformAndParent(
      identity, m_AxialPlaneSpatialObject );

    m_ViewerGroup->m_SagittalView->RequestDetachFromParent();
    m_ViewerGroup->m_SagittalView->RequestSetTransformAndParent(
      identity, m_SagittalPlaneSpatialObject );

    m_ViewerGroup->m_CoronalView->RequestDetachFromParent();
    m_ViewerGroup->m_CoronalView->RequestSetTransformAndParent(
      identity, m_CoronalPlaneSpatialObject );
    

    // add axial mesh reslice representation to the 2D and 3D views
    m_ViewerGroup->m_AxialView->RequestAddObject( m_AxialMeshResliceRepresentation );
    m_ViewerGroup->m_3DView->RequestAddObject( m_AxialMeshResliceRepresentation->Copy() );

    // add sagittal mesh reslice representation to the 2D and 3D views
    m_ViewerGroup->m_SagittalView->RequestAddObject( m_SagittalMeshResliceRepresentation );
    m_ViewerGroup->m_3DView->RequestAddObject( m_SagittalMeshResliceRepresentation->Copy() );

    // add coronal mesh reslice representation to the 2D and 3D views
    m_ViewerGroup->m_CoronalView->RequestAddObject( m_CoronalMeshResliceRepresentation );
    m_ViewerGroup->m_3DView->RequestAddObject( m_CoronalMeshResliceRepresentation->Copy() );

    // set background color to the views
    m_ViewerGroup->m_AxialView->SetRendererBackgroundColor(0,0,0);
    m_ViewerGroup->m_SagittalView->SetRendererBackgroundColor(0,0,0);
    m_ViewerGroup->m_CoronalView->SetRendererBackgroundColor(0,0,0);
    m_ViewerGroup->m_3DView->SetRendererBackgroundColor(1,1,1);

    // set parallel projection to the camera
    m_ViewerGroup->m_AxialView->SetCameraParallelProjection(true);
    m_ViewerGroup->m_SagittalView->SetCameraParallelProjection(true);
    m_ViewerGroup->m_CoronalView->SetCameraParallelProjection(true);

    // reset cameras in the different views
    m_ViewerGroup->m_AxialView->RequestResetCamera();
    m_ViewerGroup->m_SagittalView->RequestResetCamera();
    m_ViewerGroup->m_CoronalView->RequestResetCamera();
    m_ViewerGroup->m_3DView->RequestResetCamera();

    igstkPushInputMacro( Success );
    m_StateMachine.ProcessInputs();
}

/** -----------------------------------------------------------------
* Load tracker mesh. This method asks for a .msh file
* -----------------------------------------------------------------
*/
void TrackingVolumeViewer::LoadTrackerMeshProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "TrackingVolumeViewer::LoadTrackerMeshProcessing called...\n" )

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
         igstkPushInputMacro( Failure );
         m_StateMachine.ProcessInputs();
         return;
     }

     m_MeshSpatialObject = observer->GetMeshObject();   
     if ( m_MeshSpatialObject.IsNull() )
     {
       igstkLogMacro2( m_Logger, DEBUG, "Cannot read mesh\n" )
       igstkPushInputMacro( Failure );
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
     
     //m_ViewerGroup->m_AxialView->RequestAddObject( m_MeshRepresentation->Copy() );
     //m_ViewerGroup->m_SagittalView->RequestAddObject( m_MeshRepresentation->Copy() );
     //m_ViewerGroup->m_CoronalView->RequestAddObject( m_MeshRepresentation->Copy() );
     m_ViewerGroup->m_3DView->RequestAddObject( m_MeshRepresentation->Copy() );
     
     m_ViewerGroup->m_AxialView->RequestResetCamera();
     m_ViewerGroup->m_SagittalView->RequestResetCamera();
     m_ViewerGroup->m_CoronalView->RequestResetCamera();
     m_ViewerGroup->m_3DView->RequestResetCamera();

     igstkPushInputMacro( Success );
     m_StateMachine.ProcessInputs();
     return;
  }
  else
  {
     igstkLogMacro2( m_Logger, DEBUG, "No directory is selected\n" )
     igstkPushInputMacro( Failure );     
     m_StateMachine.ProcessInputs();
     return;
  }
}

void TrackingVolumeViewer::InitializeTrackerProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "TrackingVolumeViewer::InitializeTrackerProcessing called...\n" )

/*
  std::ofstream ofile;
  ofile.open("TrackerControllerStateMachineDiagram.dot");
  this->m_TrackerController->ExportStateMachineDescription( ofile, true );
*/

  if (!m_TrackerConfiguration)
  {
    igstkLogMacro2( m_Logger, DEBUG, "TrackingVolumeViewer::InitializeTrackerProcessing\
                                     There is no tracker configuration\n" )
    igstkPushInputMacro( Failure );
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
    ReportError( errorMessage );
    igstkLogMacro2( m_Logger, DEBUG, "Tracker Initialization error\n" )
    igstkPushInputMacro( Failure );
    m_StateMachine.ProcessInputs();
    return;
  }

  m_TrackerController->RequestGetNonReferenceToolList();
  igstkPushInputMacro( Success );
  m_StateMachine.ProcessInputs();
  return;
}

/** -----------------------------------------------------------------
* Starts tracking provided it is initialized and connected to the 
* communication port
*---------------------------------------------------------------------
*/
void TrackingVolumeViewer::StartTrackingProcessing()
{
    igstkLogMacro2( m_Logger, DEBUG, 
                    "TrackingVolumeViewer::StartTrackingProcessing called...\n" ) 

    igstk::Transform identity;
    identity.SetToIdentity(igstk::TimeStamp::GetLongestPossibleTime());

    m_TrackerController->RequestSetParentSpatialObject( identity, m_WorldReference );

    if (!m_TrackerConfiguration)
    {
      igstkLogMacro2( m_Logger, DEBUG, "TrackingVolumeViewer::StartTrackingProcessing \
                                       There is not tracker configuration object\n" )
      igstkPushInputMacro( Failure );
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
      ReportError( errorMessage );
      igstkLogMacro2( m_Logger, DEBUG, "Tracker start error\n" )
      igstkPushInputMacro( Failure );
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
      m_ViewerGroup->m_3DView->RequestAddObject( rep );
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
      identity, m_WorldReference );

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

    // reset cameras in all the views
    m_ViewerGroup->m_AxialView->RequestResetCamera();
    m_ViewerGroup->m_SagittalView->RequestResetCamera();
    m_ViewerGroup->m_CoronalView->RequestResetCamera();
    m_ViewerGroup->m_3DView->RequestResetCamera();

    igstkPushInputMacro( Success );
    m_StateMachine.ProcessInputs();

  return;
}
/** -----------------------------------------------------------------
* Stops tracking but keeps the tracker connected to the 
* communication port
*---------------------------------------------------------------------
*/
void TrackingVolumeViewer::StopTrackingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "TrackingVolumeViewer::StopTrackingProcessing called...\n" )

  m_TrackerController->RequestStopTracking( );
               //check that stop was successful
  if( m_TrackerControllerObserver->Error() )
  {
    std::string errorMessage;
    m_TrackerControllerObserver->GetErrorMessage( errorMessage ); 
    m_TrackerControllerObserver->ClearError();
    ReportError( errorMessage );
    igstkLogMacro2( m_Logger, DEBUG, 
      "Tracker stop error\n" )
    igstkPushInputMacro( Failure );
    m_StateMachine.ProcessInputs();
    return;
  }
  igstkPushInputMacro( Success );
  m_StateMachine.ProcessInputs();
}
/** -----------------------------------------------------------------
* Disconnects the tracker and closes the communication port
*---------------------------------------------------------------------
*/
void TrackingVolumeViewer::DisconnectTrackerProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "TrackingVolumeViewer::DisconnectTrackerProcessing called...\n" )
  // try to disconnect
  m_TrackerController->RequestShutdown( );
  //check if succeded
  if( m_TrackerControllerObserver->Error() )
  {
    std::string errorMessage;
    m_TrackerControllerObserver->GetErrorMessage( errorMessage ); 
    m_TrackerControllerObserver->ClearError();
    ReportError( errorMessage );
    igstkLogMacro2( m_Logger, DEBUG, 
      "Tracker disconnect error\n" )

    igstkPushInputMacro( Failure );
    m_StateMachine.ProcessInputs();
    return;
  }
  igstkPushInputMacro( Success );
  m_StateMachine.ProcessInputs();
}


void TrackingVolumeViewer::TrackerControllerObserver::SetParent( TrackingVolumeViewer *p ) 
{
  m_Parent = p;
}

void TrackingVolumeViewer::TrackerControllerObserver::Execute( const itk::Object *caller, 
                                                    const itk::EventObject & event )
{
  const itk::Object * constCaller = caller;
  this->Execute(constCaller, event);
}
  
void 
TrackingVolumeViewer::TrackerControllerObserver::Execute( itk::Object * itkNotUsed(caller), 
                                                             const itk::EventObject & event )
{
  const igstk::TrackerController::InitializeErrorEvent *evt1a =
    dynamic_cast< const igstk::TrackerController::InitializeErrorEvent * > (&event);

  const igstk::TrackerStartTrackingErrorEvent *evt1b =
    dynamic_cast< const igstk::TrackerStartTrackingErrorEvent * > (&event);

  const igstk::TrackerStopTrackingErrorEvent *evt1c =
    dynamic_cast< const igstk::TrackerStopTrackingErrorEvent * > (&event);

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

    
    igstk::TrackerController::ToolContainerType::iterator iter = toolContainer.begin();

    for ( ; iter != toolContainer.end(); iter++ )
    {
      m_Parent->m_ToolVector.push_back( (*iter).second );
    }
  }
  else if ( evt4 )
  {
   /* igstk::TrackerController::ToolEntryType entry = evt4->Get();
      if ( entry.first == "reference" )
        m_Parent->m_ReferenceTool = entry.second;*/
  }
}


void
TrackingVolumeViewer
::RequestPrepareToQuit()
{
  this->RequestStopTracking();
  this->RequestDisconnectTracker();
}


void 
TrackingVolumeViewer::ReportError( const std::string &message )
{
  ReportError( message.c_str() );
}


void
TrackingVolumeViewer::ReportError( const char *message )
{
  fl_alert("%s\n", message );
  fl_beep( FL_BEEP_ERROR );
}
