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
#define VideoVideoImager_DEFAULT_REFRESH_RATE 25

/** -----------------------------------------------------------------
*     Constructor
*  -----------------------------------------------------------------
*/
VideoFrameGrabberAndViewer::VideoFrameGrabberAndViewer() :
  m_StateMachine(this)
{
  m_VideoEnabled = false;
  m_VideoVideoImagerInitialized = false;
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
  igstkAddStateMacro( InitializingVideoVideoImager );
  igstkAddStateMacro( Imaging );
  igstkAddStateMacro( StopImaging );
  /** Machine Inputs*/

  igstkAddInputMacro( Success );
  igstkAddInputMacro( Failure );
  igstkAddInputMacro( InitializeVideoVideoImager );
  igstkAddInputMacro( StartImaging );
  igstkAddInputMacro( StopImaging );
  igstkAddInputMacro( DisconnectVideoVideoImager );

  /** Initial State */

  igstkAddTransitionMacro( Initial, InitializeVideoImager,
                           InitializingVideoImager, InitializeVideoImager );
  igstkAddTransitionMacro( Initial, Success,
                           Initial, ReportSuccessVideoImagerDisconnection );
  igstkAddTransitionMacro( Initial, Failure,
                           Initial, ReportFailureVideoImagerDisconnection );

  //complete table for state: Initial

  igstkAddTransitionMacro( Initial, StartImaging,
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, StopImaging,
                           Initial, ReportInvalidRequest );
  igstkAddTransitionMacro( Initial, DisconnectVideoImager,
                           Initial, ReportInvalidRequest );

  /** InitializingVideoImager State */

  igstkAddTransitionMacro( InitializingVideoImager, Success,
                       InitializingVideoImager, ReportSuccessVideoImagerInitialization );

  igstkAddTransitionMacro( InitializingVideoImager, Failure,
                           Initial, ReportFailureVideoImagerInitialization );

  igstkAddTransitionMacro( InitializingVideoImager, StartImaging,
                           Imaging, StartImaging);

  igstkAddTransitionMacro( InitializingVideoImager, DisconnectVideoImager,
                           Initial, DisconnectVideoImager );

  //complete table for state: InitializingVideoImager

  igstkAddTransitionMacro( InitializingVideoImager, StopImaging,
                           InitializingVideoImager, ReportInvalidRequest );
  igstkAddTransitionMacro( InitializingVideoImager, InitializeVideoImager,
                           InitializingVideoImager, ReportInvalidRequest );

  /** Imaging State */

  igstkAddTransitionMacro( Imaging, StopImaging,
                           StopImaging, StopImaging );
  igstkAddTransitionMacro( Imaging, Success,
                   Imaging, ReportSuccessStartImaging );
  igstkAddTransitionMacro( Imaging, Failure,
                   InitializingVideoImager, ReportFailureStartImaging );

   //complete table for state: InitializingVideoImager

   igstkAddTransitionMacro( Imaging, StartImaging,
                            Imaging, ReportInvalidRequest);
   igstkAddTransitionMacro( Imaging, DisconnectVideoImager,
                Imaging, ReportInvalidRequest );
   igstkAddTransitionMacro( Imaging, InitializeVideoImager,
                Imaging, ReportInvalidRequest );

   /** StopImaging State */

   igstkAddTransitionMacro( StopImaging, Success,
                        InitializingVideoImager, ReportSuccessStopImaging );
   igstkAddTransitionMacro( StopImaging, Failure,
                  InitializingVideoImager, ReportFailureStopImaging );

   //complete table for state: StopImaging

   igstkAddTransitionMacro( StopImaging, StartImaging,
                        StopImaging, ReportInvalidRequest);
   igstkAddTransitionMacro( StopImaging, DisconnectVideoImager,
                        StopImaging, ReportInvalidRequest );
   igstkAddTransitionMacro( StopImaging, InitializeVideoImager,
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
  delete m_VideoImagerConfiguration;
}

/** -----------------------------------------------------------------
* Disconnects the VideoImager
*---------------------------------------------------------------------*/
void
VideoFrameGrabberAndViewer::DisconnectVideoImagerProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
           "VideoFrameGrabberAndViewer::DisconnectVideoImagerProcessing called...\n" )

  // try to disconnect
  m_VideoImagerController->RequestShutdown( );

  //check if succeded
  if( m_VideoImagerControllerObserver->Error() )
  {
    std::string errorMessage;
    m_VideoImagerControllerObserver->GetErrorMessage( errorMessage );
    m_VideoImagerControllerObserver->ClearError();
    fl_alert("%s\n", errorMessage.c_str() );
    fl_beep( FL_BEEP_ERROR );
    igstkLogMacro2( m_Logger, DEBUG, "VideoImager disconnect error\n" )
    m_StateMachine.PushInput( m_FailureInput );
  }
  else
  {
    m_StateMachine.PushInput( m_SuccessInput );
  }

  m_StateMachine.ProcessInputs();
}

void
VideoFrameGrabberAndViewer::InitializeVideoImagerProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
      "VideoFrameGrabberAndViewer::InitializeVideoImagerProcessing called...\n" )

  m_VideoImagerConfiguration = new igstk::CompressedDVVideoImagerConfiguration();

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

  m_VideoImagerConfiguration->RequestAddTool( &toolconfig );
  m_VideoImagerConfiguration->RequestSetFrequency( VideoImager_DEFAULT_REFRESH_RATE );

  if (!m_VideoImagerConfiguration)
  {
    std::string errorMessage;
    fl_alert("%s\n", errorMessage.c_str() );
    fl_beep( FL_BEEP_ERROR );
    igstkLogMacro2( m_Logger, DEBUG, "VideoImager Initialization error\n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
  }

  /** Create the controller for the VideoImager and assign observers to it*/
  m_VideoImagerController = igstk::VideoImagerController::New();

  m_VideoImagerControllerObserver = VideoImagerControllerObserver::New();
  m_VideoImagerControllerObserver->SetParent( this );

  m_VideoImagerController->AddObserver(igstk::VideoImagerController::InitializeFailureEvent(),
    m_VideoImagerControllerObserver );

  m_VideoImagerController->AddObserver(igstk::VideoImagerController::StartFailureEvent(),
    m_VideoImagerControllerObserver );

  m_VideoImagerController->AddObserver(igstk::VideoImagerController::StopFailureEvent(),
    m_VideoImagerControllerObserver );

  m_VideoImagerController->AddObserver(igstk::VideoImagerController::ShutdownFailureEvent(),
    m_VideoImagerControllerObserver );

  m_VideoImagerController->AddObserver(igstk::VideoImagerController::RequestVideoImagerEvent(),
    m_VideoImagerControllerObserver );

  m_VideoImagerController->AddObserver(igstk::VideoImagerController::RequestToolsEvent(),
    m_VideoImagerControllerObserver );

  // initialize the VideoImager controller with our VideoImager configuration file
  m_VideoImagerController->RequestInitialize( m_VideoImagerConfiguration );

  //check that initialization was successful
  if( m_VideoImagerControllerObserver->Error() )
  {
    std::string errorMessage;
    m_VideoImagerControllerObserver->GetErrorMessage( errorMessage );
    m_VideoImagerControllerObserver->ClearError();
    fl_alert("%s\n", errorMessage.c_str() );
    fl_beep( FL_BEEP_ERROR );
    igstkLogMacro2( m_Logger, DEBUG, "VideoImager Initialization error\n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
  }

  m_VideoImagerController->RequestGetVideoImager();
  m_VideoImagerController->RequestGetToolList();

  igstk::Frame frame;

  m_CompressedDVVideoImagerTool->SetInternalFrame(frame);

  m_VideoFrame->SetVideoImagerTool(m_CompressedDVVideoImagerTool);

  m_StateMachine.PushInput( m_SuccessInput );
  m_StateMachine.ProcessInputs();
}

void
VideoFrameGrabberAndViewer::RequestInitializeVideoImager()
{
  igstkLogMacro2( m_Logger, DEBUG,
             "VideoFrameGrabberAndViewer::RequestInitializeVideoImager called...\n" )

  m_StateMachine.PushInput( m_InitializeVideoImagerInput );
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
VideoFrameGrabberAndViewer::RequestDisconnectVideoImager()
{
  igstkLogMacro2( m_Logger, DEBUG,
            "VideoFrameGrabberAndViewer::RequestDisconnectVideoImager called...\n" )

  m_StateMachine.PushInput( m_DisconnectVideoImagerInput );
  m_StateMachine.ProcessInputs();
}

void
VideoFrameGrabberAndViewer::StartImagingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG,
             "VideoFrameGrabberAndViewer::StartImagingProcessing called...\n" )

  m_VideoImagerController->RequestStart();

  //check if succeded
  if( m_VideoImagerControllerObserver->Error() )
  {
    std::string errorMessage;
    m_VideoImagerControllerObserver->GetErrorMessage( errorMessage );
    m_VideoImagerControllerObserver->ClearError();
    fl_alert("%s\n", errorMessage.c_str() );
    fl_beep( FL_BEEP_ERROR );
    igstkLogMacro2( m_Logger, DEBUG, "VideoImager start error\n" )
    m_StateMachine.PushInput( m_FailureInput );
    m_StateMachine.ProcessInputs();
    return;
  }

  m_StateMachine.PushInput( m_SuccessInput );
  m_StateMachine.ProcessInputs();
}

/** Method to be invoked on successful VideoImager start */
void
VideoFrameGrabberAndViewer::ReportSuccessStartImagingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "VideoFrameGrabberAndViewer::"
                 "ReportSuccessStartImagingProcessing called...\n")

  m_ToggleRunVideoButton->label("Stop");
  m_Screenshot->activate();
  m_VideoRunning = true;
}

/** Method to be invoked on Failure VideoImager start */
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

/** Method to be invoked on Failure VideoImager initialization */
void
VideoFrameGrabberAndViewer::ReportFailureVideoImagerInitializationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "VideoFrameGrabberAndViewer::"
                 "ReportFailureVideoImagerInitializationProcessing called...\n");

  std::string errorMessage;
  errorMessage = "Could not initialize VideoImager device";
  fl_alert("%s\n", errorMessage.c_str() );
  fl_beep( FL_BEEP_ERROR );

  m_VideoImagerInitialized = false;
}


/** Method to be invoked on successful VideoImager initialization */
void
VideoFrameGrabberAndViewer::ReportSuccessVideoImagerInitializationProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "VideoFrameGrabberAndViewer::"
                 "ReportSuccessVideoImagerInitializationProcessing called...\n");
  m_VideoImagerInitialized = true;
}

/** Method to be invoked on Failure VideoImager stop */
void
VideoFrameGrabberAndViewer::ReportFailureStopImagingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "VideoFrameGrabberAndViewer::"
                 "ReportFailureStopImagingProcessing called...\n")
}

/** Method to be invoked on successful VideoImager stop */
void
VideoFrameGrabberAndViewer::ReportSuccessStopImagingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "VideoFrameGrabberAndViewer::"
                 "ReportSuccessStopImagingProcessing called...\n")

  m_ToggleRunVideoButton->label("Run");
  m_Screenshot->deactivate();
}

/** -----------------------------------------------------------------
* Stops imaging but keeps the VideoImager connected to the
* communication port
*---------------------------------------------------------------------*/
void
VideoFrameGrabberAndViewer::StopImagingProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "VideoFrameGrabberAndViewer::"
                  "StopImagingProcessing called...\n" )

  // try to stop
  m_VideoImagerController->RequestStop( );

  //check if succeded
  if( m_VideoImagerControllerObserver->Error() )
  {
    std::string errorMessage;
    m_VideoImagerControllerObserver->GetErrorMessage( errorMessage );
    m_VideoImagerControllerObserver->ClearError();
    fl_alert("%s\n", errorMessage.c_str() );
    fl_beep( FL_BEEP_ERROR );
    igstkLogMacro2( m_Logger, DEBUG, "VideoImager stop error\n" )
    m_StateMachine.PushInput( m_FailureInput );
  }
  else
  {
    m_StateMachine.PushInput( m_SuccessInput );
  }

  m_StateMachine.ProcessInputs();
}

/** Method to be invoked on failure VideoImager disconnection */
void
VideoFrameGrabberAndViewer::ReportFailureVideoImagerDisconnectionProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "VideoFrameGrabberAndViewer::"
                 "ReportFailureVideoImagerDisconnectionProcessing called...\n");
}

/** Method to be invoked on successful VideoImager disconnection */
void
VideoFrameGrabberAndViewer::ReportSuccessVideoImagerDisconnectionProcessing()
{
  igstkLogMacro2( m_Logger, DEBUG, "VideoFrameGrabberAndViewer::"
                 "ReportSuccessVideoImagerDisconnectionProcessing called...\n");
}

void
VideoFrameGrabberAndViewer::VideoImagerControllerObserver::SetParent(
  VideoFrameGrabberAndViewer *p )
{
  m_Parent = p;
}

void
VideoFrameGrabberAndViewer::VideoImagerControllerObserver::Execute( const itk::Object *caller,
                                     const itk::EventObject & event )
{
  const itk::Object * constCaller = caller;
  this->Execute(constCaller, event);
}

void
VideoFrameGrabberAndViewer::VideoImagerControllerObserver::Execute( itk::Object *caller,
                                     const itk::EventObject & event )
{
  const igstk::VideoImagerController::InitializeFailureEvent *evt1a =
    dynamic_cast< const igstk::VideoImagerController::InitializeFailureEvent * > (&event);

  const igstk::VideoImagerController::StartFailureEvent *evt1b =
    dynamic_cast< const igstk::VideoImagerController::StartFailureEvent * > (&event);

    const igstk::VideoImagerController::StopFailureEvent *evt1c =
    dynamic_cast< const igstk::VideoImagerController::StopFailureEvent * > (&event);

  const igstk::VideoImagerController::RequestVideoImagerEvent *evt2 =
    dynamic_cast< const igstk::VideoImagerController::RequestVideoImagerEvent * > (&event);

  const igstk::VideoImagerController::RequestToolsEvent *evt3 =
    dynamic_cast< const igstk::VideoImagerController::RequestToolsEvent * > (&event);

  const igstk::VideoImagerController::OpenCommunicationFailureEvent *evt4 =
    dynamic_cast< const igstk::VideoImagerController::OpenCommunicationFailureEvent * > (&event);

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
    m_Parent->m_VideoImager = evt2->Get();
  }
  else if ( evt3 )
  {
    const std::vector<igstk::VideoImagerTool::Pointer> &tools = evt3->Get();

    igstkLogMacro2( m_Parent->m_Logger, DEBUG,
                    "VideoFrameGrabberAndViewer::VideoImagerControllerObserver found VideoImager tool!\n" )

    m_Parent->m_CompressedDVVideoImagerTool = tools[0];
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

     // set transformation between m_VideoFrame and m_VideoImagerToolSpatialObject according to
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

  this->RequestInitializeVideoImager();

  m_ViewerGroup->m_VideoView->RequestResetCamera();
}

void VideoFrameGrabberAndViewer::DisableVideo()
{
  m_ViewerGroup->m_VideoView->RequestRemoveObject( m_VideoFrameRepresentationForVideoView );

  m_ViewerGroup->m_VideoView->RequestResetCamera();

  m_ToggleEnableVideoButton->label("Enable");
  m_ToggleRunVideoButton->deactivate();

  this->RequestDisconnectVideoImager();
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
