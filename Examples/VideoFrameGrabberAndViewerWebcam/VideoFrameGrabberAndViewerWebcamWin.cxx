/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    VideoFrameGrabberAndViewerWebcamWin.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "VideoFrameGrabberAndViewerWebcamWin.h"

#include "itksys/SystemTools.hxx"

#include "FL/fl_message.h"

#define VIEW_2D_REFRESH_RATE 25
#define IMAGER_DEFAULT_REFRESH_RATE 25

namespace igstk{

VideoFrameGrabberAndViewerWebcamWin::VideoFrameGrabberAndViewerWebcamWin()
{
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
  logFileName = "logVideoFrameGrabberAndViewerWebcamWin"
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

  // set background color to the view
  m_ViewerGroup->m_VideoView->SetRendererBackgroundColor(0,0,1);
  // set parallel projection to the camera
  m_ViewerGroup->m_VideoView->SetCameraParallelProjection(true);
  // set up view parameters
  m_ViewerGroup->m_VideoView->SetRefreshRate( VIEW_2D_REFRESH_RATE );
  m_ViewerGroup->m_VideoView->RequestStart();
  // Enable user interactions with the window via mouse and keyboard
  m_ViewerGroup->m_VideoWidget->RequestEnableInteractions();




     // setup the video frame spatial object
     m_VideoFrame = VideoFrameSpatialObjectType::New();
     m_VideoFrame->SetWidth(320);
     m_VideoFrame->SetHeight(240);
     m_VideoFrame->SetPixelSizeX(1);
     m_VideoFrame->SetPixelSizeY(1);
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

    m_ViewerGroup->m_VideoView->RequestAddObject( m_VideoFrameRepresentationForVideoView );
 //  this->View->RequestAddObject( m_VideoFrameRepresentationForVideoView );

    this->m_ErrorObserver = ErrorObserver::New();
   this->RequestInitialize();
}

VideoFrameGrabberAndViewerWebcamWin::~VideoFrameGrabberAndViewerWebcamWin()
{
}

void
VideoFrameGrabberAndViewerWebcamWin::RequestPrepareToQuit()
{
m_ViewerGroup->m_VideoView->RequestRemoveObject( m_VideoFrameRepresentationForVideoView );
m_ViewerGroup->m_VideoView->RequestResetCamera();

 // this->View->RequestRemoveObject( m_VideoFrameRepresentationForVideoView );
 // this->View->RequestResetCamera();


  this->RequestShutdown( );
}

void VideoFrameGrabberAndViewerWebcamWin::RequestInitialize()
{
  igstkLogMacro( DEBUG,
                 "igstkImagerController::RequestInitialize called...\n" );
    //create imager
  igstk::WebcamWinImager::Pointer imager = igstk::WebcamWinImager::New();
  this->m_Imager = imager;
  imager->RequestSetFrequency( IMAGER_DEFAULT_REFRESH_RATE );

  unsigned long observerID = imager->AddObserver( IGSTKErrorEvent(),
                                                   this->m_ErrorObserver );
  imager->RequestOpen();

  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    imager->RemoveObserver(observerID);
    igstkLogMacro( DEBUG,
                  "igstk::VideoFrameGrabberAndViewerWebcamWin::"
                  "ReportInitializationFailureProcessing called...\n");
  }
  else   //attach the tool
  {
    ImagerTool::Pointer imagerTool;
    WebcamWinImagerTool::Pointer imagerToolWebcam = WebcamWinImagerTool::New();
    unsigned int dims[3];
    dims[0] = 320;  dims[1] = 240;  dims[2] = 3;
    imagerToolWebcam->SetFrameDimensions(dims);
    imagerToolWebcam->SetPixelDepth(8);
    imagerToolWebcam->RequestSetImagerToolName("Camera");
    imagerToolWebcam->RequestConfigure();

    imagerTool = imagerToolWebcam;
    // this->m_Tools.push_back( imagerTool );
    imagerTool->RequestAttachToImager( imager );

    m_VideoFrame->SetImagerTool(imagerTool);
   m_ViewerGroup->m_VideoView->RequestResetCamera();
   // this->View->RequestResetCamera();

    m_Imager->RemoveObserver(observerID);

  }


    observerID = this->m_Imager->AddObserver( IGSTKErrorEvent(),
                                             this->m_ErrorObserver );

    m_Imager->RequestStartImaging();

    if( this->m_ErrorObserver->ErrorOccured() )
    {
      this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
      this->m_ErrorObserver->ClearError();
      m_Imager->RemoveObserver( observerID );
       igstkLogMacro( DEBUG,
                  "igstk::VideoFrameGrabberAndViewerWebcamWin::"
                  "ReportStartFailure called...\n");
    }
    else
    {
      m_Imager->RemoveObserver(observerID);
        igstkLogMacro( DEBUG,
                  "igstk::VideoFrameGrabberAndViewerWebcamWin::"
                  "ReportStartSuccess called...\n");
    }
}

void
VideoFrameGrabberAndViewerWebcamWin::RequestShutdown()
{
  igstkLogMacro( DEBUG,
                 "igstkImagerController::RequestShutdown called...\n" );

  unsigned long observerID;

  observerID = this->m_Imager->AddObserver( IGSTKErrorEvent(),
                                             this->m_ErrorObserver );
  //stop imaging
  this->m_Imager->RequestStopImaging();
  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    this->m_Imager->RemoveObserver(observerID);

      igstkLogMacro( DEBUG,
                  "igstk::VideoFrameGrabberAndViewerWebcamWin::"
                  "ReportStopFailure called...\n");

  }
  else
  {
    this->m_Imager->RemoveObserver(observerID);
     igstkLogMacro( DEBUG,
                  "igstk::VideoFrameGrabberAndViewerWebcamWin::"
                  "ReportStopSuccess called...\n");
  }

  observerID = this->m_Imager->AddObserver( IGSTKErrorEvent(),
                                             this->m_ErrorObserver );

  //close communication with imager
  this->m_Imager->RequestClose();
  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    this->m_Imager->RemoveObserver(observerID);

      igstkLogMacro( DEBUG,
                  "igstk::VideoFrameGrabberAndViewerWebcamWin::"
                  "ReportShutdownFailure called...\n");
  }
  else
  {
   this->m_Imager->RemoveObserver( observerID );
   this->m_Imager = NULL;
   this->m_Tools.clear();
    igstkLogMacro( DEBUG,
                  "igstk::VideoFrameGrabberAndViewerWebcamWin::"
                  "ReportShutdownSuccess called...\n");
  }
}


VideoFrameGrabberAndViewerWebcamWin::ErrorObserver::ErrorObserver() : m_ErrorOccured(false)
{
  //imager errors
  this->m_ErrorEvent2ErrorMessage.insert(
    std::pair<std::string,std::string>( ImagerOpenErrorEvent().GetEventName(),
                                        "Error opening imager communication." ) );
  this->m_ErrorEvent2ErrorMessage.insert(
    std::pair<std::string,std::string>( ImagerInitializeErrorEvent().GetEventName(),
                                        "Error initializing imager." ) );
  this->m_ErrorEvent2ErrorMessage.insert(
    std::pair<std::string,std::string>( ImagerStartImagingErrorEvent().GetEventName(),
                                        "Error starting imaging." ) );
  this->m_ErrorEvent2ErrorMessage.insert(
    std::pair<std::string,std::string>( ImagerStopImagingErrorEvent().GetEventName(),
                                        "Error stopping imaging." ) );
  this->m_ErrorEvent2ErrorMessage.insert(
    std::pair<std::string,std::string>( ImagerCloseErrorEvent().GetEventName(),
                                        "Error closing imager communication." ) );
}

void
VideoFrameGrabberAndViewerWebcamWin::ErrorObserver::Execute(itk::Object *caller,
                                            const itk::EventObject & event) throw (std::exception)
{
  std::map<std::string,std::string>::iterator it;
  std::string className = event.GetEventName();
  it = this->m_ErrorEvent2ErrorMessage.find(className);

  if( it != this->m_ErrorEvent2ErrorMessage.end() )
  {
    this->m_ErrorOccured = true;
    this->m_ErrorMessage = (*it).second;
  }

  //if the event we got wasn't in the error events map then we
  //silently ignore it
}

void
VideoFrameGrabberAndViewerWebcamWin::ErrorObserver::Execute(const itk::Object *caller,
                                            const itk::EventObject & event) throw (std::exception)
{
  const itk::Object * constCaller = caller;
  this->Execute(constCaller, event);
}

} //end of namespace
