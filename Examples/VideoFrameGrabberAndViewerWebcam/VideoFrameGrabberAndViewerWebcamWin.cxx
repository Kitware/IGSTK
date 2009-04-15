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

  /*
  // BeginLatex
  // 
  // The GUI is impelented using FLTK. For this purpose
  // an instance of \doxygen{FLTKWidget} and \doxygen{View3D}  
  // are generated and connected via the method
  // EndLatex
  // BeginCodeSnippet
  m_VideoWidget->RequestSetView( m_VideoView );
  // EndCodeSnippet
  // BeginLatex
  // This is done in VideoFrameGrabberAndViewerWebcamWinView.cxx.
  // 
  // We then set up and start \doxygen{View3D} here.
  //
  // EndLatex
  */

  // BeginCodeSnippet
  /** set up view parameters */
  // set background color to the view
  m_ViewerGroup->m_VideoView->SetRendererBackgroundColor(0,0,1);
  // set parallel projection to the camera
  m_ViewerGroup->m_VideoView->SetCameraParallelProjection(true);
  // set refresh rate
  m_ViewerGroup->m_VideoView->SetRefreshRate( VIEW_2D_REFRESH_RATE );
  // start the view
  m_ViewerGroup->m_VideoView->RequestStart();
  // enable user interactions with the window 
  m_ViewerGroup->m_VideoWidget->RequestEnableInteractions();
  // EndCodeSnippet


  // BeginLatex
  // 
  // The geometrical description of the video-frame in the scene is
  // managed by SpatialObjects. For this purpose we need 
  // \doxygen{VideoFrameSpatialObject}. 
  // Since \doxygen{VideoFrameSpatialObject} is a templated class we 
  // define in the first parameter the pixeltype and in the second
  // parameter the channel number. Here "unsigned char" for pixeltype
  // and 3 channels for RGB. 
  // We declare and instantiate then a \doxygen{VideoFrameSpatialObject} 
  // as follows:
  //
  // EndLatex

  /** video frame spatial object and representation
  // BeginCodeSnippet
  typedef igstk::VideoFrameSpatialObject<unsigned char, 3 >
                                            VideoFrameSpatialObjectType;
  VideoFrameSpatialObjectType::Pointer      m_VideoFrame;
  // EndCodeSnippet
  */

  // BeginLatex
  // 
  // We set device specific parameters in \doxygen{VideoFrameSpatialObject} 
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
  // Following scene graph is implemented here:
  // \doxygen{View3D} - \doxygen{VideoFrameSpatialObject} - \doxygen{AxesObject}
  // with identity transform for each dependency.
  //
  // EndLatex

  // BeginCodeSnippet
  m_VideoFrame->RequestSetTransformAndParent( identity, m_WorldReference );
  m_ViewerGroup->m_VideoView->RequestDetachFromParent();
  m_ViewerGroup->m_VideoView->RequestSetTransformAndParent( identity, m_VideoFrame );
  // EndCodeSnippet
  
  // BeginLatex
  //
  // The visual representation of SpatialObjects in the visualization window is
  // created using SpatialObject Representation classes. 
  // The corresponding Representation class for \doxygen{VideoFrameSpatialObject}
  // is \doxygen{VideoFrameRepresentation}, which is also templated and needs the 
  // \doxygen{VideoFrameSpatialObject} as parameter.
  // We declare and instantiate then a \doxygen{VideoFrameRepresentation} 
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
  m_VideoFrameRepresentationForVideoView = VideoFrameRepresentationType::New();
  // EndCodeSnippet
  
  // BeginLatex
  // 
  // Add the videoframe spatial object to the videoframe representation.
  //
  // EndLatex

  // BeginCodeSnippet
  m_VideoFrameRepresentationForVideoView->RequestSetVideoFrameSpatialObject( m_VideoFrame );
  // EndCodeSnippet
  

  // BeginLatex
  // 
  // Next, the representation is added to the view as follows:
  // 
  // EndLatex

  // BeginCodeSnippet
  m_ViewerGroup->m_VideoView->RequestAddObject( m_VideoFrameRepresentationForVideoView );
  // EndCodeSnippet

  // BeginLatex
  // 
  // We create here an Observer for the \doxygen{Imager}. This Observer will catch all
  // failure events generated by \doxygen{Imager}.
  // The ErrorObserver is implemented in VideoFrameGrabberAndViewerWebcamWin.h.
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
  // The following code instantiates a new imager object for conventional webcams.
  // The \doxygen{WebcamWinImager} derive from \doxygen{Imager} and implement device
  // specific communication. See for device specific impelementation \doxygen{WebcamWinImager.h}
  // and \doxygen{WebcamWinImager.cxx}
  // 
  // EndLatex
  
  // BeginCodeSnippet
  igstk::WebcamWinImager::Pointer imager = igstk::WebcamWinImager::New();
  // EndCodeSnippet

  this->m_Imager = imager;

  // BeginLatex
  //
  // According to connected device we set here the refresh rate
  // 
  // EndLatex
  
  // BeginCodeSnippet
  imager->RequestSetFrequency( IMAGER_DEFAULT_REFRESH_RATE );
  // EndCodeSnippet

  // BeginLatex
  //
  // Befor request calls to the imager we add an observer to the
  // imager class in order to catch possible error events.
  // 
  // EndLatex
  
  // BeginCodeSnippet
  unsigned long observerID = m_Imager->AddObserver( IGSTKErrorEvent(),
                                                   this->m_ErrorObserver );
  // EndCodeSnippet

  // BeginLatex
  //
  // Now, we try to open the communication with the device and retrieve for
  // occured errors.
  // 
  // EndLatex
  
  // BeginCodeSnippet
  m_Imager->RequestOpen();

  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    m_Imager->RemoveObserver(observerID);
    cout << this->m_ErrorMessage << endl;
  }
  // EndCodeSnippet
  else  
  {
    // BeginLatex
    //
    // Next we create an \doxygen{WebcamWinImagerTool} and set frame dimensions,
    // pixel depth and an unique name for identification. Consider these parameters
    // must be the same as the parameters for the \doxygen{VideoFrameSpatialObject}
    // After set up, the imager tool can be configured.
    // 
    // EndLatex
    
    // BeginCodeSnippet
    ImagerTool::Pointer imagerTool;
    WebcamWinImagerTool::Pointer imagerToolWebcam = WebcamWinImagerTool::New();
    unsigned int dims[3];
    dims[0] = 320;  
    dims[1] = 240;  
    dims[2] = 3;
    imagerToolWebcam->SetFrameDimensions(dims);
    imagerToolWebcam->SetPixelDepth(8);
    imagerToolWebcam->RequestSetImagerToolName("Camera");
    imagerToolWebcam->RequestConfigure();
    // EndCodeSnippet

    // BeginLatex
    //
    // Here we connect the imager with the imager tool
    // 
    // EndLatex

    imagerTool = imagerToolWebcam;

    // BeginCodeSnippet
    imagerTool->RequestAttachToImager( m_Imager );
    // EndCodeSnippet

    // BeginLatex
    //
    // After that the imager tool can set to the videoframe
    // spatial object as follows:
    // 
    // EndLatex

    // BeginCodeSnippet
    m_VideoFrame->SetImagerTool(imagerTool);
    // EndCodeSnippet

    m_ViewerGroup->m_VideoView->RequestResetCamera();
    m_Imager->RemoveObserver(observerID);
  }

  observerID = this->m_Imager->AddObserver( IGSTKErrorEvent(),
                                             this->m_ErrorObserver );

  // BeginLatex
  //
  // Here we request to start the imager. In case of success the 
  // communication thread starts retrieving continouly frames from
  // the device and the main application thread fills according to 
  // pulse generator frequency the ringbuffer in the imager tool. 
  // 
  // EndLatex

  // BeginCodeSnippet
  m_Imager->RequestStartImaging();
  // EndCodeSnippet

  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    m_Imager->RemoveObserver( observerID );
    cout << this->m_ErrorMessage << endl;
  }
  else
  {
    m_Imager->RemoveObserver(observerID);
  }
}

void
VideoFrameGrabberAndViewerWebcamWin::RequestPrepareToQuit()
{
  // BeginLatex
  // 
  // Finally, before exiting the application, the imager is properly 
  // closed and other clean up procedures are executed, as follows:
  //
  // EndLatex
  // BeginCodeSnippet
  m_ViewerGroup->m_VideoView->RequestRemoveObject( m_VideoFrameRepresentationForVideoView );
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

  observerID = this->m_Imager->AddObserver( IGSTKErrorEvent(),
                                             this->m_ErrorObserver );
  
  // BeginCodeSnippet
  this->m_Imager->RequestStopImaging();
  // EndCodeSnippet

  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    this->m_Imager->RemoveObserver(observerID);
    cout << this->m_ErrorMessage << endl;
  }
  else
  {
    this->m_Imager->RemoveObserver(observerID);
  }

  observerID = this->m_Imager->AddObserver( IGSTKErrorEvent(),
                                             this->m_ErrorObserver );

  // BeginCodeSnippet
  this->m_Imager->RequestClose();
  // EndCodeSnippet

  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    this->m_Imager->RemoveObserver(observerID);
    cout << this->m_ErrorMessage << endl;
  }
  else
  {
    this->m_Imager->RemoveObserver( observerID );
    this->m_Imager = NULL;
    this->m_Tools.clear();
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
}

void
VideoFrameGrabberAndViewerWebcamWin::ErrorObserver::Execute(const itk::Object *caller,
                                      const itk::EventObject & event) throw (std::exception)
{
  const itk::Object * constCaller = caller;
  this->Execute(constCaller, event);
}

} //end of namespace
