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

// BeginLatex
//
// Define refresh rates for View and VideoImager.
//
// EndLatex

// BeginCodeSnippet
#define VIEW_3D_REFRESH_RATE 25
#define VIDEOIMAGER_DEFAULT_REFRESH_RATE 25
// EndCodeSnippet

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

  /*
  // BeginLatex
  //
  // The GUI is implemented using FLTK. For this purpose
  // an instance of \doxygen{FLTKWidget} and \doxygen{View3D}
  // are created and connected via the method:
  // EndLatex
  // BeginCodeSnippet
  m_VideoWidget->RequestSetView( m_VideoView );
  // EndCodeSnippet
  // BeginLatex
  // This is done in VideoFrameGrabberAndViewerWebcamWinView.cxx.
  // Afterwards we set up and start \doxygen{View3D}.
  //
  // EndLatex
  */

  // BeginCodeSnippet
  // Set background color to the View
  m_ViewerGroup->m_VideoView->SetRendererBackgroundColor(0,0,1);
  // Set parallel projection to the camera
  m_ViewerGroup->m_VideoView->SetCameraParallelProjection(true);
  // Set refresh rate
  m_ViewerGroup->m_VideoView->SetRefreshRate( VIEW_3D_REFRESH_RATE );
  // Start the View
  m_ViewerGroup->m_VideoView->RequestStart();
  // Enable user interactions with the window
  m_ViewerGroup->m_VideoWidget->RequestEnableInteractions();
  // EndCodeSnippet


  // BeginLatex
  //
  // The geometrical description of the videoframe in the scene is
  // managed by the videoframe SpatialObject. For this purpose we need
  // \doxygen{VideoFrameSpatialObject}.
  // Since \doxygen{VideoFrameSpatialObject} is a template class we
  // define in the first parameter the pixeltype and in the second
  // parameter the channel number. Here we set "unsigned char" for pixeltype
  // and 3 channels for RGB.
  // We then declare and instantiate a \doxygen{VideoFrameSpatialObject}
  // as follows:
  //
  // EndLatex

  /** videoframe SpatialObject and Representation
  // BeginCodeSnippet
  typedef igstk::VideoFrameSpatialObject<unsigned char, 3 >
                                            VideoFrameSpatialObjectType;
  VideoFrameSpatialObjectType::Pointer      m_VideoFrame;
  // EndCodeSnippet
  */

  // BeginLatex
  //
  // We set device specific parameters in \doxygen{VideoFrameSpatialObject}.
  //
  // EndLatex

  // BeginCodeSnippet
  m_VideoFrame = VideoFrameSpatialObjectType::New();
  m_VideoFrame->SetWidth(320);
  m_VideoFrame->SetHeight(240);
  m_VideoFrame->SetPixelSizeX(1);
  m_VideoFrame->SetPixelSizeY(1);
  m_VideoFrame->SetNumberOfScalarComponents(3);
  m_VideoFrame->Initialize();
  // EndCodeSnippet

  igstk::Transform identity;
  identity.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

  // BeginLatex
  //
  // The following scene graph is implemented here:
  // \doxygen{View3D} - \doxygen{VideoFrameSpatialObject} - \doxygen{AxesObject}
  // with identity transform for each dependency.
  //
  // EndLatex

  // BeginCodeSnippet
  m_VideoFrame->RequestSetTransformAndParent( identity, m_WorldReference );
  m_ViewerGroup->m_VideoView->RequestDetachFromParent();
  m_ViewerGroup->m_VideoView->RequestSetTransformAndParent( identity,
                                                            m_VideoFrame );
  // EndCodeSnippet

  // BeginLatex
  //
  // The visual Representation of SpatialObjects in the visualization window is
  // created using SpatialObject Representation classes. The corresponding 
  // Representation class for \doxygen{VideoFrameSpatialObject} is
  // \doxygen{VideoFrameRepresentation}, which is also templated and needs the
  // \doxygen{VideoFrameSpatialObject} as parameter.
  // We declare and instantiate \doxygen{VideoFrameRepresentation}
  // as follows:
  //
  // EndLatex
  /*
  // BeginCodeSnippet
  typedef igstk::VideoFrameRepresentation<VideoFrameSpatialObjectType>
                                            VideoFrameRepresentationType;

  VideoFrameRepresentationType::Pointer m_VideoFrameRepresentationForVideoView;
  // EndCodeSnippet
  */

  // BeginCodeSnippet
  m_VideoFrameRepresentationForVideoView = 
                                     VideoFrameRepresentationType::New();
  // EndCodeSnippet

  // BeginLatex
  //
  // Add the videoframe Spatialobject to the videoframe Representation.
  //
  // EndLatex

  // BeginCodeSnippet
  m_VideoFrameRepresentationForVideoView->
                       RequestSetVideoFrameSpatialObject( m_VideoFrame );
  // EndCodeSnippet


  // BeginLatex
  //
  // Next, the Representation is added to the View as follows:
  //
  // EndLatex

  // BeginCodeSnippet
  m_ViewerGroup->m_VideoView->RequestAddObject( 
                                m_VideoFrameRepresentationForVideoView );
  // EndCodeSnippet

  // BeginLatex
  //
  // We create here an Observer for the \doxygen{VideoImager}. This Observer 
  // will catch all failure events generated by \doxygen{VideoImager}.
  // The ErrorObserver is implemented in 
  // \code{VideoFrameGrabberAndViewerWebcamWin.h}.
  //
  // EndLatex

  // BeginCodeSnippet
  this->m_ErrorObserver = ErrorObserver::New();
  // EndCodeSnippet

  this->RequestInitialize();
}

VideoFrameGrabberAndViewerWebcamWin::~VideoFrameGrabberAndViewerWebcamWin()
{
}

void VideoFrameGrabberAndViewerWebcamWin::RequestInitialize()
{
  igstkLogMacro( DEBUG,
         "VideoFrameGrabberAndViewerWebcamWin::RequestInitialize called...\n" );

  // BeginLatex
  //
  // The following code instantiates a new VideoImager object for conventional 
  // webcams. The \doxygen{WebcamWinVideoImager} derive from 
  // \doxygen{VideoImager} and implement device specific communication. 
  // For device specific implementations  see \doxygen{WebcamWinVideoImager.h}
  // and \doxygen{WebcamWinVideoImager.cxx}.
  //
  // EndLatex

  // BeginCodeSnippet
  igstk::WebcamWinVideoImager::Pointer videoImager = 
                                             igstk::WebcamWinVideoImager::New();
  // EndCodeSnippet

  this->m_VideoImager = videoImager;

  // BeginLatex
  //
  // Depending on the connected device, set the refresh rate here:
  //
  // EndLatex

  // BeginCodeSnippet
  videoImager->RequestSetFrequency( VIDEOIMAGER_DEFAULT_REFRESH_RATE );
  // EndCodeSnippet

  // BeginLatex
  //
  // Before any request calls to the VideoImager we add an observer to the
  // VideoImager class to catch possible error events.
  //
  // EndLatex

  // BeginCodeSnippet
  unsigned long observerID = m_VideoImager->AddObserver( IGSTKErrorEvent(),
                                                        this->m_ErrorObserver );
  // EndCodeSnippet

  // BeginLatex
  //
  // Now, we try to open the communication with the device and retrieve errors
  // that might occur.
  //
  // EndLatex

  // BeginCodeSnippet
  m_VideoImager->RequestOpen();

  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    m_VideoImager->RemoveObserver(observerID);
    cout << this->m_ErrorMessage << endl;
  }
  // EndCodeSnippet
  else
  {
    // BeginLatex
    //
    // Next we create an \doxygen{WebcamWinVideoImagerTool} and set frame 
    // dimensions, pixel depth, and an unique name for identification. Note 
    // these parameters must be the same as the parameters for the 
    // \doxygen{VideoFrameSpatialObject}. After setup, the VideoImager tool 
    // can be configured.
    //
    // EndLatex

    // BeginCodeSnippet
    VideoImagerTool::Pointer videoImagerTool;
    WebcamWinVideoImagerTool::Pointer videoImagerToolWebcam = 
                                         WebcamWinVideoImagerTool::New();
    unsigned int dims[3];
    dims[0] = 320;
    dims[1] = 240;
    dims[2] = 3;
    videoImagerToolWebcam->SetFrameDimensions(dims);
    videoImagerToolWebcam->SetPixelDepth(8);
    videoImagerToolWebcam->RequestSetVideoImagerToolName("Camera");
    videoImagerToolWebcam->RequestConfigure();
    // EndCodeSnippet

    // BeginLatex
    //
    // Here we connect the VideoImagerTool to the VideoImager:
    //
    // EndLatex

    videoImagerTool = videoImagerToolWebcam;

    // BeginCodeSnippet
    videoImagerTool->RequestAttachToVideoImager( m_VideoImager );
    // EndCodeSnippet

    // BeginLatex
    //
    // After that the VideoImager tool can set to the videoframe
    // Spatialobject as follows:
    //
    // EndLatex

    // BeginCodeSnippet
    m_VideoFrame->SetVideoImagerTool(videoImagerTool);
    // EndCodeSnippet

    m_ViewerGroup->m_VideoView->RequestResetCamera();
    m_VideoImager->RemoveObserver(observerID);
  }

  observerID = this->m_VideoImager->AddObserver( IGSTKErrorEvent(),
                                             this->m_ErrorObserver );

  // BeginLatex
  //
  // Here we request the system to start the VideoImager. In case of success 
  // the communication thread starts retrieving  frames continuously from
  // the device and the main application thread fills the ringbuffer
  // in the VideoImager tool according to the pulse generator frequency.
  //
  // EndLatex

  // BeginCodeSnippet
  m_VideoImager->RequestStartImaging();
  // EndCodeSnippet

  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    m_VideoImager->RemoveObserver( observerID );
    cout << this->m_ErrorMessage << endl;
  }
  else
  {
    m_VideoImager->RemoveObserver(observerID);
  }
}

void
VideoFrameGrabberAndViewerWebcamWin::RequestPrepareToQuit()
{
  // BeginLatex
  //
  // Finally, before exiting the application, the VideoImager is properly
  // closed and other clean up procedures are executed, as follows:
  //
  // EndLatex
  // BeginCodeSnippet
  m_ViewerGroup->m_VideoView->RequestRemoveObject( 
                                m_VideoFrameRepresentationForVideoView );
  m_ViewerGroup->m_VideoView->RequestResetCamera();
  // EndCodeSnippet

  this->RequestShutdown( );
}

void
VideoFrameGrabberAndViewerWebcamWin::RequestShutdown()
{
  igstkLogMacro( DEBUG,
           "VideoFrameGrabberAndViewerWebcamWin::RequestShutdown called...\n" );

  unsigned long observerID;

  observerID = this->m_VideoImager->AddObserver( IGSTKErrorEvent(),
                                                        this->m_ErrorObserver );

  // BeginCodeSnippet
  this->m_VideoImager->RequestStopImaging();
  // EndCodeSnippet

  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    this->m_VideoImager->RemoveObserver(observerID);
    cout << this->m_ErrorMessage << endl;
  }
  else
  {
    this->m_VideoImager->RemoveObserver(observerID);
  }

  observerID = this->m_VideoImager->AddObserver( IGSTKErrorEvent(),
                                                        this->m_ErrorObserver );

  // BeginCodeSnippet
  this->m_VideoImager->RequestClose();
  // EndCodeSnippet

  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    this->m_VideoImager->RemoveObserver(observerID);
    cout << this->m_ErrorMessage << endl;
  }
  else
  {
    this->m_VideoImager->RemoveObserver( observerID );
    this->m_VideoImager = NULL;
    this->m_Tools.clear();
  }
}


VideoFrameGrabberAndViewerWebcamWin
::ErrorObserver::ErrorObserver() : m_ErrorOccured(false)
{
  /** VideoImager errors */
  this->m_ErrorEvent2ErrorMessage.insert( std::pair<std::string,std::string>
                                   ( VideoImagerOpenErrorEvent().GetEventName(),
                                 "Error opening VideoImager communication." ) );
  
  this->m_ErrorEvent2ErrorMessage.insert( std::pair<std::string,std::string>
                             ( VideoImagerInitializeErrorEvent().GetEventName(),
                                          "Error initializing VideoImager." ) );
  
  this->m_ErrorEvent2ErrorMessage.insert( std::pair<std::string,std::string>
                           ( VideoImagerStartImagingErrorEvent().GetEventName(),
                                                  "Error starting imaging." ) );
  
  this->m_ErrorEvent2ErrorMessage.insert( std::pair<std::string,std::string>
                            ( VideoImagerStopImagingErrorEvent().GetEventName(),
                                                  "Error stopping imaging." ) );
  
  this->m_ErrorEvent2ErrorMessage.insert( std::pair<std::string,std::string>
                                  ( VideoImagerCloseErrorEvent().GetEventName(),
                                 "Error closing VideoImager communication." ) );
}

void
VideoFrameGrabberAndViewerWebcamWin
::ErrorObserver::Execute(itk::Object *caller,
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
}

void
VideoFrameGrabberAndViewerWebcamWin
::ErrorObserver::Execute(const itk::Object *caller,
                          const itk::EventObject & event) throw (std::exception)
{
  const itk::Object * constCaller = caller;
  this->Execute(constCaller, event);
}

} //end of namespace
