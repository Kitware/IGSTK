/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    VideoFrameGrabberAndViewerMicron.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __VideoFrameGrabberAndViewerMicron_h
#define __VideoFrameGrabberAndViewerMicron_h

#include "VideoFrameGrabberAndViewerMicronGUI.h"

#include "igstkSandboxConfigure.h"

#include "igstkMacros.h"
#include "igstkObject.h"
#include "igstkEvents.h"

#include "igstkAxesObject.h"
#include "igstkAxesObjectRepresentation.h"

#include "igstkVideoFrameSpatialObject.h"
#include "igstkVideoFrameRepresentation.h"

#include "igstkMicronVideoImager.h"
#include "igstkMicronVideoImagerTool.h"

#include "igstkMicronTracker.h"

#include "itkStdStreamLogOutput.h"

namespace igstk{
  /** \class VideoFrameGrabberAndViewerMicron
  *
  * \brief Implementation class for VideoFrameGrabberAndViewerMicronGUI.
  *
  * This class implements the main application.
  *
  */
  class VideoFrameGrabberAndViewerMicron : public VideoFrameGrabberAndViewerMicronGUI
  {
    public:

      /** Typedefs */
      igstkStandardClassBasicTraitsMacro( VideoFrameGrabberAndViewerMicron,
          VideoFrameGrabberAndViewerMicronGUI );

      igstkLoggerMacro();

      /** typedef for axes spatial objects */
      typedef igstk::AxesObject                           AxesObjectType;
      typedef igstk::AxesObjectRepresentation             AxesRepresentationType;

      /** video frame spatial object and representation*/
      typedef igstk::VideoFrameSpatialObject<unsigned char, 1 >
      VideoFrameSpatialObjectType;
      VideoFrameSpatialObjectType::Pointer      m_VideoFrame;

      typedef igstk::VideoFrameRepresentation<VideoFrameSpatialObjectType>
      VideoFrameRepresentationType;

      VideoFrameRepresentationType::Pointer m_VideoFrameRepresentationForVideoView;


      /** VideoImager object and VideoImager tool*/
      igstk::VideoImagerTool::Pointer                m_MicronVideoImagerTool;

      /** Public request methods from the GUI */
      virtual void RequestPrepareToQuit();
      void RequestInitialize(std::string InitializationFile,
          std::string CameraCalibrationFileDirectory,
          std::string markerTemplateDirectory);
      void RequestShutdown();

      VideoFrameGrabberAndViewerMicron();
      virtual ~VideoFrameGrabberAndViewerMicron();

    private:

      VideoFrameGrabberAndViewerMicron(const Self&); // purposely not implemented
      void operator=(const Self&); // purposely not implemented


      /** Define a initial world coordinate system */
      AxesObjectType::Pointer                   m_WorldReference;
      AxesRepresentationType::Pointer           m_WorldReferenceRepresentation;

      /** Log file */
      std::ofstream                             m_LogFile;


      VideoImager::Pointer       m_VideoImager;
      MicronTracker::Pointer           m_Tracker;
      std::vector<VideoImagerTool::Pointer> m_Tools;

  };

}//end of igstk
#endif
