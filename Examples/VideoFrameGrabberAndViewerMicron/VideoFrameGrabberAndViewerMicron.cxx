/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    VideoFrameGrabberAndViewerMicron.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "VideoFrameGrabberAndViewerMicron.h"

#include "itksys/SystemTools.hxx"

#include "FL/fl_message.h"

// BeginLatex
//
// Define refresh rates for View and VideoImager.
//
// EndLatex

// BeginCodeSnippet
#define VIEW_2D_REFRESH_RATE 15
#define VideoImager_DEFAULT_REFRESH_RATE 15
// EndCodeSnippet

namespace igstk{

  VideoFrameGrabberAndViewerMicron::VideoFrameGrabberAndViewerMicron()
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
    logFileName = "logVideoFrameGrabberAndViewerMicron"
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
  // are created and connected via the method:
  // EndLatex
  // BeginCodeSnippet
  m_VideoWidget->RequestSetView( m_VideoView );
  // EndCodeSnippet
  // BeginLatex
  // This is done in VideoFrameGrabberAndViewerMicronView.cxx.
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
    m_ViewerGroup->m_VideoView->SetRefreshRate( VIEW_2D_REFRESH_RATE );
    // Start the View
    m_ViewerGroup->m_VideoView->RequestStart();
    // Enable user interactions with the window
    m_ViewerGroup->m_VideoWidget->RequestEnableInteractions();
    // EndCodeSnippet


    // BeginLatex
    //
    // The geometrical description of the videoframe in the scene is
    // managed by the videoframe Spatialobject. For this purpose we need
    // \doxygen{VideoFrameSpatialObject}.
    // Since \doxygen{VideoFrameSpatialObject} is a templated class we
    // define in the first parameter the pixeltype and in the second
    // parameter the channel number. Here we set "unsigned char" for pixeltype
    // and 3 channels for RGB.
    // We declare and instantiate then a \doxygen{VideoFrameSpatialObject}
    // as follows:
    //
    // EndLatex



  }

  VideoFrameGrabberAndViewerMicron::~VideoFrameGrabberAndViewerMicron()
  {
  }

  void VideoFrameGrabberAndViewerMicron::RequestInitialize(std::string InitializationFile,
      std::string CameraCalibrationFileDirectory,
      std::string markerTemplateDirectory)
  {
    igstkLogMacro( DEBUG,
        "VideoFrameGrabberAndViewerMicron::RequestInitialize called...\n" );

    // BeginLatex
    //
    // The following code instantiates a new VideoImager object for conventional webcams.
    // The \doxygen{MicronVideoImager} derive from \doxygen{VideoImager} and implement device
    // specific communication. See for device specific impelementations \doxygen{MicronVideoImager.h}
    // and \doxygen{MicronVideoImager.cxx}
    //
    // EndLatex

    // BeginCodeSnippet
    igstk::MicronVideoImager::Pointer VideoImager = igstk::MicronVideoImager::New();
    // EndCodeSnippet

    this->m_VideoImager = VideoImager;

    igstk::MicronTracker::Pointer tracker = igstk::MicronTracker::New();
    this->m_Tracker = tracker;

    m_Tracker->SetCameraCalibrationFilesDirectory(
        CameraCalibrationFileDirectory );

    m_Tracker->SetInitializationFile( InitializationFile );
    m_Tracker->SetMarkerTemplatesDirectory( markerTemplateDirectory );
    m_Tracker->RequestOpen();
    m_Tracker->RequestSetFrequency( VideoImager_DEFAULT_REFRESH_RATE );

    this->m_Tracker->RequestStartTracking();

    VideoImager->SetMicronTracker(this->m_Tracker);

    // BeginLatex
    //
    // According to connected device we set here the refresh rate
    //
    // EndLatex

    // BeginCodeSnippet
    VideoImager->RequestSetFrequency( VideoImager_DEFAULT_REFRESH_RATE );
    // EndCodeSnippet


    // EndCodeSnippet

    // BeginLatex
    //
    // Now, we try to open the communication with the device and retrieve for
    // possibly occured errors.
    //
    // EndLatex

    // BeginCodeSnippet
    m_VideoImager->RequestOpen();

    /** videoframe Spatialobject and Representation
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
    m_VideoFrame->SetWidth(1024);
    m_VideoFrame->SetHeight(768);
    m_VideoFrame->SetPixelSizeX(1);
    m_VideoFrame->SetPixelSizeY(1);
    m_VideoFrame->SetNumberOfScalarComponents(1);
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
    // The visual Representation of SpatialObjects in the visualization window is
    // created using SpatialObject Representation classes.
    // The corresponding Representation class for \doxygen{VideoFrameSpatialObject}
    // is \doxygen{VideoFrameRepresentation}, which is also templated and needs the
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
    m_VideoFrameRepresentationForVideoView = VideoFrameRepresentationType::New();
    m_VideoFrameRepresentationForVideoView->SetWindowLevel(250,125);
    // EndCodeSnippet

    // BeginLatex
    //
    // Add the videoframe Spatialobject to the videoframe Representation.
    //
    // EndLatex

    // BeginCodeSnippet
    m_VideoFrameRepresentationForVideoView->RequestSetVideoFrameSpatialObject( m_VideoFrame );
    // EndCodeSnippet


    // BeginLatex
    //
    // Next, the Representation is added to the View as follows:
    //
    // EndLatex

    // BeginCodeSnippet
    // EndCodeSnippet

    // EndCodeSnippet
    // BeginLatex
    //
    // Next we create an \doxygen{MicronVideoImagerTool} and set frame dimensions,
    // pixel depth and an unique name for identification. Consider these parameters
    // must be the same as the parameters for the \doxygen{VideoFrameSpatialObject}
    // After setup, the VideoImager tool can be configured.
    //
    // EndLatex

    // BeginCodeSnippet
    VideoImagerTool::Pointer VideoImagerTool;
    MicronVideoImagerTool::Pointer VideoImagerToolWebcam = MicronVideoImagerTool::New();
    unsigned int dims[3];
    dims[0] = 1024;
    dims[1] = 768;
    dims[2] = 1;
    VideoImagerToolWebcam->SetFrameDimensions(dims);
    VideoImagerToolWebcam->SetPixelDepth(8);
    VideoImagerToolWebcam->RequestSetVideoImagerToolName("Right");
    VideoImagerToolWebcam->RequestConfigure();
    // EndCodeSnippet

    // BeginLatex
    //
    // Here we connect the VideoImager with the VideoImager tool
    //
    // EndLatex

    VideoImagerTool = VideoImagerToolWebcam;

    // BeginCodeSnippet
    VideoImagerTool->RequestAttachToVideoImager( m_VideoImager );
    // EndCodeSnippet

    // BeginLatex
    //
    // After that the VideoImager tool can set to the videoframe
    // Spatialobject as follows:
    //
    // EndLatex

    // BeginCodeSnippet
    m_VideoFrame->SetVideoImagerTool(VideoImagerTool);
    // EndCodeSnippet


    m_ViewerGroup->m_VideoView->RequestAddObject( m_VideoFrameRepresentationForVideoView );
    m_ViewerGroup->m_VideoView->RequestResetCamera();

    // BeginLatex
    //
    // Here we request to start the VideoImager. In case of success the
    // communication thread starts retrieving continously frames from
    // the device and the main application thread fills according to
    // the pulse-generator-frequency the ringbuffer in the VideoImager tool.
    //
    // EndLatex

    // BeginCodeSnippet

    m_VideoImager->RequestStartImaging();
    // EndCodeSnippet

  }

  void
  VideoFrameGrabberAndViewerMicron::RequestPrepareToQuit()
  {
    // BeginLatex
    //
    // Finally, before exiting the application, the VideoImager is properly
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
  VideoFrameGrabberAndViewerMicron::RequestShutdown()
  {
    igstkLogMacro( DEBUG,
        "VideoFrameGrabberAndViewerMicron::RequestShutdown called...\n" );


    // BeginCodeSnippet
    this->m_VideoImager->RequestStopImaging();
    this->m_Tracker->RequestStopTracking();
    // EndCodeSnippet


    // BeginCodeSnippet
    this->m_VideoImager->RequestClose();
    // EndCodeSnippet

    this->m_VideoImager = NULL;
    this->m_Tools.clear();
  }


} //end of namespace
