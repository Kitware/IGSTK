/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    VideoFrameGrabberAndViewer.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "VideoFrameGrabberAndViewer.h"

//TODO delete
#include "FL/Fl_File_Chooser.H"

#include "itksys/SystemTools.hxx"
#include "itksys/Directory.hxx"
#include "igstkTransformObserver.h"

#define VIEW_2D_REFRESH_RATE 25
#define IMAGER_DEFAULT_REFRESH_RATE 25

/** -----------------------------------------------------------------
*     Constructor
*  -----------------------------------------------------------------
*/
VideoFrameGrabberAndViewer::VideoFrameGrabberAndViewer() :
  m_StateMachine(this)
{
  m_VideoEnabled = false;
  m_ImagerInitialized = false;
  m_VideoRunning = false;

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
  logFileName = "logVideoFrameGrabberAndViewer"
  + itksys::SystemTools::GetCurrentDateTime( "_%Y_%m_%d_%H_%M_%S" ) + ".txt";
  m_LogFile.open( logFileName.c_str() );

  if( !m_LogFile.fail() )
  {
    m_LogFileOutput->SetStream( m_LogFile );
    this->GetLogger()->AddLogOutput( m_LogFileOutput );
    igstkLogMacro2( m_Logger, DEBUG,
      "Successfully opened Log file:" << logFileName << "\n" );
  }
  else
  {
    //Return if fail to open the log file
    igstkLogMacro2( m_Logger, DEBUG,
      "Problem opening Log file:" << logFileName << "\n" );
    return;
  }

  /** Instantiate the world reference */
  m_WorldReference        = AxesObjectType::New();

  // instatiate observer for key pressed event
  m_KeyPressedObserver = LoadedObserverType::New();
  m_KeyPressedObserver->SetCallbackFunction( this,
                       &VideoFrameGrabberAndViewer::HandleKeyPressedCallback );

  // add it to the Viewer group in our GUI
  m_ViewerGroup->AddObserver( igstk::VideoFrameGrabberAndViewerQuadrantViews::
                              KeyPressedEvent(), m_KeyPressedObserver );

  // instantiate observer for mouse pressed event
  m_MousePressedObserver = LoadedObserverType::New();
  m_MousePressedObserver->SetCallbackFunction( this, &VideoFrameGrabberAndViewer::HandleMousePressedCallback );

  // add it to the Viewer group in our GUI
  m_ViewerGroup->AddObserver( igstk::VideoFrameGrabberAndViewerQuadrantViews::MousePressedEvent(),
  m_MousePressedObserver );

  // set background color to the view
  m_ViewerGroup->m_VideoView->SetRendererBackgroundColor(0,0,1);
  // set parallel projection to the camera
  m_ViewerGroup->m_VideoView->SetCameraParallelProjection(true);
  // set up view parameters
  m_ViewerGroup->m_VideoView->SetRefreshRate( VIEW_2D_REFRESH_RATE );
  m_ViewerGroup->m_VideoView->RequestStart();
  /** Enable user interactions with the window via mouse and keyboard */
  m_ViewerGroup->m_VideoWidget->RequestEnableInteractions();

  /** Machine States*/

  igstkAddStateMacro( Initial );
  igstkAddStateMacro( InitializingImager );
  igstkAddStateMacro( Imaging );
  igstkAddStateMacro( StopImaging );
  /** Machine Inputs*/

  igstkAddInputMacro( Success );
  igstkAddInputMacro( Failure );
  igstkAddInputMacro( InitializeImager );
  igstkAddInputMacro( StartImaging );
  igstkAddInputMacro( StopImaging );
  igstkAddInputMacro( DisconnectImager );

  /** Initial State */

  igstkAddTransitionMacro( Initial, InitializeImager,
                           InitializingImager, InitializeImager );
  igstkAddTransitionMacro( Initial, Success,
                           Initial, ReportSuccessImagerDisconnection );
  igstkAddTransitionMacro( Initial, Failure,
                           Initial, ReportFailureImagerDisconnection );

  //complete table for state: Initial

  igstkAddTransitionMacro( Initial, StartImaging,
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, StopImaging,
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, DisconnectImager,
                           Initial, ReportInvalidRequest );

  /** InitializingImager State */

  igstkAddTransitionMacro( InitializingImager, Success,
                       InitializingImager, ReportSuccessImagerInitialization );

  igstkAddTransitionMacro( InitializingImager, Failure,
                           Initial, ReportFailureImagerInitialization );

  igstkAddTransitionMacro( InitializingImager, StartImaging,
                           Imaging, StartImaging);

  igstkAddTransitionMacro( InitializingImager, DisconnectImager,
                           Initial, DisconnectImager );

  //complete table for state: InitializingImager

  igstkAddTransitionMacro( InitializingImager, StopImaging,
                           InitializingImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingImager, InitializeImager,
                           InitializingImager, ReportInvalidRequest );

  /** Imaging State */

  igstkAddTransitionMacro( Imaging, StopImaging,
                           StopImaging, StopImaging );
  igstkAddTransitionMacro( Imaging, Success,
                   Imaging, ReportSuccessStartImaging );
  igstkAddTransitionMacro( Imaging, Failure,
                   InitializingImager, ReportFailureStartImaging );

   //complete table for state: InitializingImager

   igstkAddTransitionMacro( Imaging, StartImaging,
                            Imaging, ReportInvalidRequest);
   igstkAddTransitionMacro( Imaging, DisconnectImager,
                Imaging, ReportInvalidRequest );
   igstkAddTransitionMacro( Imaging, InitializeImager,
                Imaging, ReportInvalidRequest );

   /** StopImaging State */

   igstkAddTransitionMacro( StopImaging, Success,
                        InitializingImager, ReportSuccessStopImaging );
   igstkAddTransitionMacro( StopImaging, Failure,
                  InitializingImager, ReportFailureStopImaging );

   //complete table for state: StopImaging

   igstkAddTransitionMacro( StopImaging, StartImaging,
                        StopImaging, ReportInvalidRequest);
   igstkAddTransitionMacro( StopImaging, DisconnectImager,
                        StopImaging, ReportInvalidRequest );
   igstkAddTransitionMacro( StopImaging, InitializeImager,
                        StopImaging, ReportInvalidRequest );
   igstkAddTransitionMacro( StopImaging, StopImaging,
                          StopImaging, ReportInvalidRequest );


  /** Set Initial State */
  igstkSetInitialStateMacro( Initial );

  m_StateMachine.SetReadyToRun();
/*
  std::ofstream ofile;
  ofile.open("VideoFrameGrabberAndViewerStateMachineDiagram.dot");
  const bool skipLoops = false;
  this->ExportStateMachineDescription( ofile, skipLoops );
  ofile.close();
*/
}

/** -----------------------------------------------------------------
*     Destructor
*  -----------------------------------------------------------------
*/
VideoFrameGrabberAndViewer::~VideoFrameGrabberAndViewer()
{
  delete m_ImagerConfiguration;
}

/** -----------------------------------------------------------------
* Disconnects the imager
*---------------------------------------------------------------------*/
void
VideoFrameGrabberAndViewer::DisconnectImagerProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
           "VideoFrameGrabberAndViewer::DisconnectImagerProcessing called...\n" )

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
    igstkLogMacro2( m_Logger, DEBUG, "Imager disconnect error\n" )
    m_StateMachine.PushInput( m_FailureInput );
  }
  else
  {
    m_StateMachine.PushInput( m_SuccessInput );
  }

  m_StateMachine.ProcessInputs();
}

void
VideoFrameGrabberAndViewer::InitializeImagerProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
      "VideoFrameGrabberAndViewer::InitializeImagerProcessing called...\n" )

  m_ImagerConfiguration = new igstk::CompressedDVImagerConfiguration();

  //set the tool parameters
  igstk::CompressedDVToolConfiguration toolconfig;

  //set video input dimensions
  unsigned int dims[3];
  dims[0] = 640;
  dims[1] = 480;
  dims[2] = 3;

  toolconfig.SetFrameDimensions(dims);
  toolconfig.SetPixelDepth(8);

  std::string deviceName = "Camera";
  //  toolconfig.SetCalibrationFileName( ".igstk" );
  toolconfig.SetToolUniqueIdentifier( deviceName );

  m_ImagerConfiguration->RequestAddTool( &toolconfig );
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

  // initialize the imager controller with our imager configuration file
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

  m_CompressedDVImagerTool->SetInternalFrame(frame);

  m_VideoFrame->SetImagerTool(m_CompressedDVImagerTool);

  m_StateMachine.PushInput( m_SuccessInput );
  m_StateMachine.ProcessInputs();
}

void
VideoFrameGrabberAndViewer::RequestInitializeImager()
{
  igstkLogMacro2( m_Logger, DEBUG,
             "VideoFrameGrabberAndViewer::RequestInitializeImager called...\n" )

  m_StateMachine.PushInput( m_InitializeImagerInput );
  m_StateMachine.ProcessInputs();
}

void
VideoFrameGrabberAndViewer::RequestStartImaging()
{
  igstkLogMacro2( m_Logger, DEBUG,
                    "VideoFrameGrabberAndViewer::RequestStartImaging called...\n" )

  m_StateMachine.PushInput( m_StartImagingInput );
  m_StateMachine.ProcessInputs();
}

void
VideoFrameGrabberAndViewer::RequestDisconnectImager()
{
  igstkLogMacro2( m_Logger, DEBUG,
            "VideoFrameGrabberAndViewer::RequestDisconnectImager called...\n" )

  m_StateMachine.PushInput( m_DisconnectImagerInput );
  m_StateMachine.ProcessInputs();
}

void
VideoFrameGrabberAndViewer::StartImagingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
             "VideoFrameGrabberAndViewer::StartImagingProcessing called...\n" )

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

/** Method to be invoked on successful imager start */
void
VideoFrameGrabberAndViewer::ReportSuccessStartImagingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "VideoFrameGrabberAndViewer::"
                 "ReportSuccessStartImagingProcessing called...\n")

  m_ToggleRunVideoButton->label("Stop");
  m_Screenshot->activate();
  m_VideoRunning = true;
}

/** Method to be invoked on Failure imager start */
void
VideoFrameGrabberAndViewer::ReportFailureStartImagingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "VideoFrameGrabberAndViewer::"
                 "ReportFailureStartImagingProcessing called...\n")
}

void
VideoFrameGrabberAndViewer::RequestStopImaging()
{
  igstkLogMacro2( m_Logger, DEBUG,
     "VideoFrameGrabberAndViewer::RequestStopImaging called...\n" )

  m_StateMachine.PushInput( m_StopImagingInput );
  m_StateMachine.ProcessInputs();
}

/** Method to be invoked when an invalid operation was requested */
void
VideoFrameGrabberAndViewer::ReportInvalidRequestProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "VideoFrameGrabberAndViewer::"
                 "ReportInvalidRequestProcessing called...\n");
  this->InvokeEvent(InvalidRequestErrorEvent());
}

/** Method to be invoked on Failure imager initialization */
void
VideoFrameGrabberAndViewer::ReportFailureImagerInitializationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "VideoFrameGrabberAndViewer::"
                 "ReportFailureImagerInitializationProcessing called...\n");

  std::string errorMessage;
  errorMessage = "Could not initialize imager device";
  fl_alert( errorMessage.c_str() );
  fl_beep( FL_BEEP_ERROR );

  m_ImagerInitialized = false;
}


/** Method to be invoked on successful imager initialization */
void
VideoFrameGrabberAndViewer::ReportSuccessImagerInitializationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "VideoFrameGrabberAndViewer::"
                 "ReportSuccessImagerInitializationProcessing called...\n");
  m_ImagerInitialized = true;
}

/** Method to be invoked on Failure imager stop */
void
VideoFrameGrabberAndViewer::ReportFailureStopImagingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "VideoFrameGrabberAndViewer::"
                 "ReportFailureStopImagingProcessing called...\n")
}

/** Method to be invoked on successful imager stop */
void
VideoFrameGrabberAndViewer::ReportSuccessStopImagingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "VideoFrameGrabberAndViewer::"
                 "ReportSuccessStopImagingProcessing called...\n")

  m_ToggleRunVideoButton->label("Run");
  m_Screenshot->deactivate();
}

/** -----------------------------------------------------------------
* Stops imaging but keeps the imager connected to the
* communication port
*---------------------------------------------------------------------*/
void
VideoFrameGrabberAndViewer::StopImagingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "VideoFrameGrabberAndViewer::"
                  "StopImagingProcessing called...\n" )

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
    igstkLogMacro2( m_Logger, DEBUG, "Imager stop error\n" )
    m_StateMachine.PushInput( m_FailureInput );
  }
  else
  {
    m_StateMachine.PushInput( m_SuccessInput );
  }

  m_StateMachine.ProcessInputs();
}

/** Method to be invoked on failure imager disconnection */
void
VideoFrameGrabberAndViewer::ReportFailureImagerDisconnectionProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "VideoFrameGrabberAndViewer::"
                 "ReportFailureImagerDisconnectionProcessing called...\n");
}

/** Method to be invoked on successful imager disconnection */
void
VideoFrameGrabberAndViewer::ReportSuccessImagerDisconnectionProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "VideoFrameGrabberAndViewer::"
                 "ReportSuccessImagerDisconnectionProcessing called...\n");
}

void
VideoFrameGrabberAndViewer::ImagerControllerObserver::SetParent(
  VideoFrameGrabberAndViewer *p )
{
  m_Parent = p;
}

void
VideoFrameGrabberAndViewer::ImagerControllerObserver::Execute( const itk::Object *caller,
                                     const itk::EventObject & event )
{
  const itk::Object * constCaller = caller;
  this->Execute(constCaller, event);
}

void
VideoFrameGrabberAndViewer::ImagerControllerObserver::Execute( itk::Object *caller,
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
                    "VideoFrameGrabberAndViewer::ImagerControllerObserver found imager tool!\n" )

    m_Parent->m_CompressedDVImagerTool = tools[0];
  }
  else if ( evt4 )
  {
    m_ErrorOccured = true;
    m_ErrorMessage = evt4->Get();
  }
}

void
VideoFrameGrabberAndViewer::RequestToggleEnableVideo()
{
  igstkLogMacro2( m_Logger, DEBUG,
   "VideoFrameGrabberAndViewer::RequestToggleEnableVideo called...\n" )

  if (m_VideoEnabled)
  {
    this->m_ToggleEnableVideoButton->color((Fl_Color)55);
    this->DisableVideo();
  }
  else
  {
    this->m_ToggleEnableVideoButton->color(FL_GREEN);
    this->EnableVideo();
  }

  m_VideoEnabled = !m_VideoEnabled;
}

void VideoFrameGrabberAndViewer::EnableVideo()
{
  if (m_VideoFrame.IsNull())
  {
     // setup the video frame spatial object
     m_VideoFrame = VideoFrameSpatialObjectType::New();
     m_VideoFrame->SetWidth(720);
     m_VideoFrame->SetHeight(576);
   //TODO see all pixelsize settings
     m_VideoFrame->SetPixelSizeX(1); // in mm
     m_VideoFrame->SetPixelSizeY(1); // in mm
     m_VideoFrame->SetNumberOfScalarComponents(3);
     m_VideoFrame->Initialize();

     igstk::Transform identity;
     identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

     // set transformation between m_VideoFrame and m_ImagerToolSpatialObject according to
     // calibration for now identity
     m_VideoFrame->RequestSetTransformAndParent( identity, m_WorldReference );

     m_ViewerGroup->m_VideoView->RequestDetachFromParent();
     m_ViewerGroup->m_VideoView->RequestSetTransformAndParent( identity, m_VideoFrame );

     // create a video frame representation for the video view
     m_VideoFrameRepresentationForVideoView = VideoFrameRepresentationType::New();
     m_VideoFrameRepresentationForVideoView->RequestSetVideoFrameSpatialObject( m_VideoFrame );

     //TODO windowlevel etc
     m_WindowWidthVideo = 255;
     m_WindowLevelVideo = 128;
     m_VideoFrameRepresentationForVideoView->SetWindowLevel(m_WindowWidthVideo,m_WindowLevelVideo);
  }

  if (m_VideoFrame.IsNotNull())
  {
    m_ViewerGroup->m_VideoView->RequestAddObject( m_VideoFrameRepresentationForVideoView );
    m_ToggleRunVideoButton->activate();
  }

  m_ToggleEnableVideoButton->label("Disable");

  this->RequestInitializeImager();

  m_ViewerGroup->m_VideoView->RequestResetCamera();
}

void VideoFrameGrabberAndViewer::DisableVideo()
{
  m_ViewerGroup->m_VideoView->RequestRemoveObject( m_VideoFrameRepresentationForVideoView );

  m_ViewerGroup->m_VideoView->RequestResetCamera();

  m_ToggleEnableVideoButton->label("Enable");
  m_ToggleRunVideoButton->deactivate();

  this->RequestDisconnectImager();
}

void VideoFrameGrabberAndViewer::RequestToggleRunVideo()
{
  igstkLogMacro2( m_Logger, DEBUG,
      "VideoFrameGrabberAndViewer::RequestToggleRunVideo called...\n" )

  if (m_VideoRunning)
  {
  this->RequestStopImaging();
  }
  else
  {
  this->RequestStartImaging();
  }
}

void VideoFrameGrabberAndViewer::RequestScreenshot()
{
  m_VideoFrameRepresentationForVideoView->SaveScreenShot(
      "ScreenshotVideoFrame.png");
  fl_message( "Screenshot taken successfully!" );
}

void
VideoFrameGrabberAndViewer::RequestPrepareToQuit()
{
  void DisableVideo();
  void StopVideo();
}

/** -----------------------------------------------------------------
* Callback function for observer listening to a key-pressed event
*---------------------------------------------------------------------
*/
void
VideoFrameGrabberAndViewer::HandleKeyPressedCallback( const itk::EventObject & event )
{
  if ( igstk::VideoFrameGrabberAndViewerQuadrantViews::KeyPressedEvent().CheckEvent( &event ) )
  {
    igstk::VideoFrameGrabberAndViewerQuadrantViews::KeyPressedEvent *keyPressedEvent =
      ( igstk::VideoFrameGrabberAndViewerQuadrantViews::KeyPressedEvent *) & event;
    this->HandleKeyPressed( keyPressedEvent->Get() );
  }
}

void
VideoFrameGrabberAndViewer::HandleKeyPressed (
  igstk::VideoFrameGrabberAndViewerQuadrantViews::KeyboardCommandType keyCommand )
{
  switch ( keyCommand.key )
  {

    case 'r': // reset the cameras in the different views
        m_ViewerGroup->m_VideoView->RequestResetCamera();
        break;
    default:
         return;
  }
}

/** -----------------------------------------------------------------
* Callback function for observer listening to a mouse-pressed event
*--------------------------------------------------------------------*/
void
VideoFrameGrabberAndViewer::HandleMousePressedCallback( const itk::EventObject & event )
{
  if ( igstk::VideoFrameGrabberAndViewerQuadrantViews::MousePressedEvent().CheckEvent( &event ) )
  {
    igstk::VideoFrameGrabberAndViewerQuadrantViews::MousePressedEvent *mousePressedEvent =
      ( igstk::VideoFrameGrabberAndViewerQuadrantViews::MousePressedEvent *) & event;
    this->HandleMousePressed( mousePressedEvent->Get() );
  }
}

void
VideoFrameGrabberAndViewer::HandleMousePressed (
    igstk::VideoFrameGrabberAndViewerQuadrantViews::MouseCommandType mouseCommand )
{
    m_WindowWidthVideo += mouseCommand.dx;

    if (m_WindowWidthVideo < 1)
      m_WindowWidthVideo = 1;

    m_WindowLevelVideo += mouseCommand.dy;

    if ( m_VideoFrameRepresentationForVideoView.IsNotNull() )
      m_VideoFrameRepresentationForVideoView->SetWindowLevel(m_WindowWidthVideo,m_WindowLevelVideo);

}
