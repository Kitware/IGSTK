/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    Tracking.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "Tracking.h"

#include "igstkEvents.h"
#include "itksys/SystemTools.hxx"
#include "itksys/Directory.hxx"
#include "igstkTransformObserver.h"


/** -----------------------------------------------------------------
*     Constructor
*  -----------------------------------------------------------------
*/
Tracking::Tracking() : 
  m_TrackerConfiguration( NULL )
{

  this->m_TrackerController = igstk::TrackerController::New();

  this->m_TrackerControllerObserver = TrackerControllerObserver::New();
  this->m_TrackerControllerObserver->SetParent( this );
  this->m_TrackerController->AddObserver(igstk::TrackerController::RequestTrackerEvent(), this->m_TrackerControllerObserver );

  /*
  this->m_TrackerController->AddObserver(igstk::TrackerController::RequestReferenceToolEvent(),
    this->m_TrackerControllerObserver );

  */

  this->m_TrackerController->AddObserver(igstk::TrackerController::RequestToolsEvent(), this->m_TrackerControllerObserver );


  this->m_Socket = igtl::ClientSocket::New(); 
  this->m_TransformMessage = igtl::TransformMessage::New();
  this->m_TransformMessage->SetDeviceName("Tracker");

  
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
  logFileName = "logTracking"
  + itksys::SystemTools::GetCurrentDateTime( "_%Y_%m_%d_%H_%M_%S" ) + ".txt";
  m_LogFile.open( logFileName.c_str() );
  if( !m_LogFile.fail() )
  {
    m_LogFileOutput->SetStream( m_LogFile );
    this->GetLogger()->AddLogOutput( m_LogFileOutput );
    igstkLogMacro( DEBUG, "Successfully opened Log file:" << logFileName << "\n" );
  }
  else
  {
    //Return if fail to open the log file
    igstkLogMacro( DEBUG, "Problem opening Log file:"
                                                    << logFileName << "\n" );
    return;
  }

  // set logger to the controller
  this->m_TrackerController->SetLogger(this->GetLogger());

  /** Initialize all member variables  */
  
  m_WorldReference        = igstk::AxesObject::New();

  /** 
   *  This observer listens to the TrackerToolTransformUpdateEvent from
   *  TrackerTool class, notice this event doesn't carry any payload, it
   *  only functions as a ticker here to trigger image representation class
   *  to do image reslicing according to the current tooltip location.
   *  Refer to:
   *  Tracking::TrackingCallback()
   */
  m_TrackerToolUpdateObserver    = LoadedObserverType::New();
  m_TrackerToolUpdateObserver->SetCallbackFunction( this, &Tracking::TrackingCallback );

}



/** -----------------------------------------------------------------
*     Destructor
*  -----------------------------------------------------------------
*/
Tracking::~Tracking()
{
    if ( m_TrackerConfiguration )
    {
        m_TrackerConfiguration->Delete();
    }

    this->m_Socket->CloseSocket();
    this->m_Socket->Delete();
    this->m_TransformMessage->Delete();
}



void Tracking::SetTrackerConfiguration(igstk::TrackerConfiguration *configuration)
{
  /*if ( m_TrackerConfiguration )
  {
    m_TrackerConfiguration->Delete();
  }*/
  m_TrackerConfiguration = configuration;
          //hide the GUI we used to get the configuration
 // this->m_TrackerConfigurationGUI[this->m_TrackingSystemChoice->value()]->Hide();
}



void Tracking::InitializeTracking()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Tracking::InitializeTracking called...\n" )

  std::ofstream ofile;
  ofile.open("TrackerControllerStateMachineDiagram.dot");
  this->m_TrackerController->ExportStateMachineDescription( ofile, true );

  if (!this->m_TrackerConfiguration)
  {
    std::string errorMessage;
    //fl_alert("%s\n", errorMessage.c_str() );
    //fl_beep( FL_BEEP_ERROR );
    igstkLogMacro( DEBUG, "Tracker Initialization error\n" )
    return;
  }

  this->m_TrackerController->RequestInitialize( this->m_TrackerConfiguration );
               //check that initialization was successful
  if( this->m_TrackerControllerObserver->Error() )
  {
    std::string errorMessage;
    this->m_TrackerControllerObserver->GetErrorMessage( errorMessage ); 
    this->m_TrackerControllerObserver->ClearError();
    //fl_alert("%s\n", errorMessage.c_str() );
    //fl_beep( FL_BEEP_ERROR );
    igstkLogMacro( DEBUG, "Tracker Initialization error\n" )
    return;
  }

  this->m_TrackerController->RequestGetTracker();
  this->m_TrackerController->RequestGetNonReferenceToolList();
  this->m_TrackerController->RequestGetReferenceTool();
      
  igstk::Transform transform;
  transform.SetToIdentity(igstk::TimeStamp::GetLongestPossibleTime());


  char * hostname = const_cast< char * >( this->m_HostName.c_str() );

  std::cout << "Trying to connect to host = " << hostname << std::endl;
  std::cout << "In port = " << this->m_Port << std::endl;

  int r = this->m_Socket->ConnectToServer(hostname, this->m_Port);
  if (r != 0)
  {
      std::cerr << "Cannot connect to the server." << std::endl;
      exit(0);
  }

 
//  m_ObjectTool->RequestDetachFromParent();
//  m_ObjectTool->RequestSetTransformAndParent( transform, m_WorkingTool);

  /** Connect the scene graph with an identity transform first */
  /*
  if ( this->m_ReferenceTool.IsNotNull() )
  {
    m_ReferenceTool->RequestSetTransformAndParent(transform, m_WorldReference);
  }
  else
  {
    m_Tracker->RequestSetTransformAndParent(transform, m_WorldReference);
  }  
  */

//  m_WorkingTool->AddObserver(
//      igstk::TrackerToolTransformUpdateEvent(), m_TrackerToolUpdateObserver);


}



/** -----------------------------------------------------------------
* Starts tracking provided it is initialized and connected to the 
* communication port
*---------------------------------------------------------------------
*/
void Tracking::StartTracking()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Tracking::StartTracking called...\n" )

  if (!this->m_TrackerConfiguration)
  {
    std::string errorMessage;
    //fl_alert("%s\n", errorMessage.c_str() );
    //fl_beep( FL_BEEP_ERROR );
    igstkLogMacro( DEBUG, "Tracker Starting error\n" )
    return;
  }

  this->m_TrackerController->RequestStart( this->m_TrackerConfiguration );
               //check that start was successful
  if( this->m_TrackerControllerObserver->Error() )
  {
    std::string errorMessage;
    this->m_TrackerControllerObserver->GetErrorMessage( errorMessage ); 
    this->m_TrackerControllerObserver->ClearError();
    //fl_alert("%s\n", errorMessage.c_str() );
    //fl_beep( FL_BEEP_ERROR );
    igstkLogMacro( DEBUG, "Tracker start error\n" )
    return;
  }


}



/** -----------------------------------------------------------------
* Stops tracking but keeps the tracker connected to the 
* communication port
*---------------------------------------------------------------------
*/
void Tracking::StopTracking()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Tracking::StopTracking called...\n" )

  this->m_TrackerController->RequestStop( this->m_TrackerConfiguration );
               //check that stop was successful
  if( this->m_TrackerControllerObserver->Error() )
  {
    std::string errorMessage;
    this->m_TrackerControllerObserver->GetErrorMessage( errorMessage ); 
    this->m_TrackerControllerObserver->ClearError();
    //fl_alert("%s\n", errorMessage.c_str() );
    //fl_beep( FL_BEEP_ERROR );
    igstkLogMacro( DEBUG, "Tracker stop error\n" )
    return;
  }

  //TrackingBtn->label("Stop");
  //TrackingBtn->value(1);
  //ControlGroup->redraw();
}
/** -----------------------------------------------------------------
* Disconnects the tracker and closes the communication port
*---------------------------------------------------------------------
*/
void Tracking::DisconnectTracker()
{
  igstkLogMacro2( m_Logger, DEBUG, 
                    "Tracking::DisconnectTracker called...\n" )
  // try to disconnect
  this->m_TrackerController->RequestShutdown( );
  //check if succeded
  if( this->m_TrackerControllerObserver->Error() )
  {
    std::string errorMessage;
    this->m_TrackerControllerObserver->GetErrorMessage( errorMessage ); 
    this->m_TrackerControllerObserver->ClearError();
    //fl_alert("%s\n", errorMessage.c_str() );
    //fl_beep( FL_BEEP_ERROR );
    igstkLogMacro( DEBUG, "Tracker disconnect error\n" )

    return;
  }
}


void Tracking::SetPort(int port)
{
  this->m_Port = port;
}



void Tracking::SetFramesPerSecond(double fps)
{
  this->m_FramesPerSecond = fps;
}



void Tracking::SetHostName(const char * hostname)
{
  this->m_HostName = hostname;
}



/** -----------------------------------------------------------------
*  Callback function for TrackerToolTransformUpdateEvent
*  This function computes the transform of the tooltip location in
*  the image space (WorldReference is a essential image coordinate system
*  since image is attached to WorldReference using an identity transform),
*  and reslice image to that tip location
*---------------------------------------------------------------------
*/
void Tracking::TrackingCallback(const itk::EventObject & event )
{

  if ( igstk::TrackerToolTransformUpdateEvent().CheckEvent( &event ) )
  {
    typedef igstk::TransformObserver ObserverType;
    ObserverType::Pointer transformObserver = ObserverType::New();
    transformObserver->ObserveTransformEventsFrom( m_WorkingTool );
    transformObserver->Clear();
    
    m_WorkingTool->RequestComputeTransformTo( m_WorldReference );
   
    if ( transformObserver->GotTransform() )
    {
        igstk::Transform transform = transformObserver->GetTransform();

//      if ( transform.IsValidNow() )
        {
        vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
        transform.ExportTransform( *matrix );

        igtl::Matrix4x4 m;

        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                m[i][j] = (float) matrix->GetElement(i, j);

        this->m_TransformMessage->SetMatrix(m);
        this->m_TransformMessage->Pack();

        int interval = (int) (1000.0 / this->m_FramesPerSecond);
        igtl::Sleep(interval); // wait
        this->m_Socket->Send(this->m_TransformMessage->GetPackPointer(), this->m_TransformMessage->GetPackSize());


//      matrix->Print(cout);
        matrix->Delete();
        }
    }
  }
}



void Tracking::TrackerControllerObserver::SetParent(Tracking *p) 
{
  this->m_Parent = p;
}



void Tracking::TrackerControllerObserver::Execute( const itk::Object *caller, const itk::EventObject & event )
{
  const itk::Object * constCaller = caller;
  this->Execute(constCaller, event);
}


  
void Tracking::TrackerControllerObserver::Execute( itk::Object *caller, const itk::EventObject & event )
{
  const igstk::TrackerController::InitializeFailureEvent *evt1a =
    dynamic_cast< const igstk::TrackerController::InitializeFailureEvent * > (&event);

  const igstk::TrackerController::StartFailureEvent *evt1b =
    dynamic_cast< const igstk::TrackerController::StartFailureEvent * > (&event);

    const igstk::TrackerController::StopFailureEvent *evt1c =
    dynamic_cast< const igstk::TrackerController::StopFailureEvent * > (&event);

  const igstk::TrackerController::RequestTrackerEvent *evt2 =
    dynamic_cast< const igstk::TrackerController::RequestTrackerEvent * > (&event);

  const igstk::TrackerController::RequestToolsEvent *evt3 = 
    dynamic_cast< const igstk::TrackerController::RequestToolsEvent * > (&event);

  /*
  const igstk::TrackerController::RequestReferenceToolEvent *evt4 = 
    dynamic_cast< const igstk::TrackerController::RequestReferenceToolEvent * > (&event);
    */

  if( evt1a ) 
  {            
    this->m_ErrorOccured = true;
    this->m_ErrorMessage = evt1a->Get();
  }
  else if ( evt1b )
  {
    this->m_ErrorOccured = true;
    this->m_ErrorMessage = evt1b->Get();
  }
  else if ( evt1c )
  {
    this->m_ErrorOccured = true;
    this->m_ErrorMessage = evt1c->Get();
  }
  else if ( evt2 )
  {
    this->m_Parent->m_Tracker = evt2->Get();
    this->m_Parent->m_Tracker->RequestSetTransformAndParent(this->m_Parent->m_CurrentTransform, 
                                                            this->m_Parent->m_WorldReference);
  }
  else if ( evt3 )
  {
    const std::vector<igstk::TrackerTool::Pointer> &tools = evt3->Get();
    this->m_Parent->m_WorkingTool = tools[0];
    this->m_Parent->m_WorkingTool->AddObserver(igstk::TrackerToolTransformUpdateEvent(), 
                                               this->m_Parent->m_TrackerToolUpdateObserver);
  }


//  else if ( evt4 )
//  {
//    this->m_Parent->m_ReferenceTool = evt4->Get();
//  }


/*
  this->m_TrackerController->AddObserver(igstk::TrackerController::InitializationFailureEvent(),
    this->m_TrackerControllerObserver );

  this->m_TrackerController->AddObserver(igstk::TrackerController::RequestTrackerEvent(),
    this->m_TrackerControllerObserver );

  this->m_TrackerController->AddObserver(igstk::TrackerController::RequestToolsEvent(),
    this->m_TrackerControllerObserver );


  if( dynamic_cast< const igstk::PivotCalibrationNew::InitializationSuccessEvent * > (&event) ) 
  {            //activate "Calibrate" button 
    this->m_parent->m_calibrateButton->activate();               
  }
  else if( dynamic_cast< const igstk::PivotCalibrationNew::InitializationFailureEvent * > (&event) ) 
  {
    fl_alert("%s\n","Failed to initialize pivot calibration.\n Check that tracker is in tracking state.");
    fl_beep(FL_BEEP_ERROR);;
  }
  */
}
