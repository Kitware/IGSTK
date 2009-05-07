/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    VideoFrameGrabberAndViewer.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __VideoFrameGrabberAndViewer_h
#define __VideoFrameGrabberAndViewer_h

#include "VideoFrameGrabberAndViewerGUI.h"

#include "igstkSandboxConfigure.h"

#include "igstkStateMachine.h"
#include "igstkMacros.h"
#include "igstkObject.h"
#include "igstkEvents.h"

#include "igstkVideoImagerController.h"
#include "igstkVideoImagerConfiguration.h"

#include "igstkAxesObject.h"
#include "igstkAxesObjectRepresentation.h"

#include "igstkVideoFrameSpatialObject.h"
#include "igstkVideoFrameRepresentation.h"
#include "igstkFrame.h"

#include "itkStdStreamLogOutput.h"


/** \class VideoFrameGrabberAndViewer
*
* \brief Implementation class for VideoFrameGrabberAndViewerGUI.
*
* This class implements the main application.
*
*/
class VideoFrameGrabberAndViewer : public VideoFrameGrabberAndViewerGUI
{
public:

  /** Typedefs */
  igstkStandardClassBasicTraitsMacro( VideoFrameGrabberAndViewer,
                                      VideoFrameGrabberAndViewerGUI );

  igstkLoggerMacro();

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

  /** typedef for axes spatial objects */
  typedef igstk::AxesObject                           AxesObjectType;
  typedef igstk::AxesObjectRepresentation             AxesRepresentationType;

  /** video frame spatial object and representation*/
  typedef igstk::VideoFrameSpatialObject<unsigned char, 3 >
                                            VideoFrameSpatialObjectType;
  VideoFrameSpatialObjectType::Pointer      m_VideoFrame;

  typedef igstk::VideoFrameRepresentation<VideoFrameSpatialObjectType>
                            VideoFrameRepresentationType;

  VideoFrameRepresentationType::Pointer m_VideoFrameRepresentationForVideoView;

  double                                    m_WindowLevelVideo;
  double                                    m_WindowWidthVideo;

  /** VideoImager object and VideoImager tool*/
  igstk::VideoImager::Pointer                    m_VideoImager;
  igstk::VideoImagerTool::Pointer                m_CompressedDVVideoImagerTool;

  /** Observer type for loaded event,
   *  the callback can be set to a member function. */
  typedef itk::ReceptorMemberCommand < Self > LoadedObserverType;

  /** observer for key pressed event */
  LoadedObserverType::Pointer               m_KeyPressedObserver;

  /** observer for mouse button pressed event */
  LoadedObserverType::Pointer               m_MousePressedObserver;

  /** observers for the VideoImager tool */
  LoadedObserverType::Pointer               m_VideoImagerToolUpdateObserver;
  LoadedObserverType::Pointer               m_VideoImagerToolNotAvailableObserver;
  LoadedObserverType::Pointer               m_VideoImagerToolAvailableObserver;

  bool m_VideoEnabled;
  bool m_VideoImagerInitialized;
  bool m_VideoRunning;

  /** Public request methods from the GUI */
  virtual void RequestPrepareToQuit();
  virtual void RequestToggleEnableVideo();
  virtual void RequestToggleRunVideo();
  virtual void RequestScreenshot();

  void EnableVideo();
  void DisableVideo();

  void StartVideo();
  void StopVideo();

  virtual void RequestInitializeVideoImager();
  virtual void RequestStartImaging();
  virtual void ReportSuccessStartImagingProcessing();
  virtual void ReportFailureStartImagingProcessing();
  virtual void RequestStopImaging();
  virtual void RequestDisconnectVideoImager();

  igstkEventMacro( InvalidRequestErrorEvent, igstk::IGSTKErrorEvent );

  VideoFrameGrabberAndViewer();
  virtual ~VideoFrameGrabberAndViewer();

private:

  VideoFrameGrabberAndViewer(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented

  /** States for the State Machine */
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( InitializingVideoImager );
  igstkDeclareStateMacro( Imaging );
  igstkDeclareStateMacro( StopImaging );

  /** Inputs to the state machine and it's designed transitions */
  igstkDeclareInputMacro( Success );
  igstkDeclareInputMacro( Failure );
  igstkDeclareInputMacro( InitializeVideoImager );
  igstkDeclareInputMacro( StartImaging );
  igstkDeclareInputMacro( StopImaging );
  igstkDeclareInputMacro( DisconnectVideoImager );


  /** Define a initial world coordinate system */
  AxesObjectType::Pointer                   m_WorldReference;
  AxesRepresentationType::Pointer           m_WorldReferenceRepresentation;

  /** Class that observes the events generated by the VideoImager controller. */
  class VideoImagerControllerObserver : public itk::Command
  {
  public:
    typedef VideoImagerControllerObserver             Self;
    typedef ::itk::Command                       Superclass;
    typedef ::itk::SmartPointer<Self>            Pointer;
    typedef ::itk::SmartPointer<const Self>      ConstPointer;

    igstkNewMacro(Self)
    igstkTypeMacro(VideoImagerControllerObserver, itk::Command)

    void SetParent(VideoFrameGrabberAndViewer *p);

      /** Invoke this method when an initialization event occurs. */
    void Execute(itk::Object *caller, const itk::EventObject & event);
    void Execute(const itk::Object *caller, const itk::EventObject & event);

    /**
     * Clear the current error.
     */
    void ClearError()
    {
      this->m_ErrorOccured = false;
      this->m_ErrorMessage.clear();
    }
    /**
     * If an error occurs in one of the observed IGSTK components this method
     * will return true.
     */
    bool Error()
    {
      return this->m_ErrorOccured;
    }

    void GetErrorMessage(std::string &errorMessage)
    {
      errorMessage = this->m_ErrorMessage;
    }

  protected:
    VideoImagerControllerObserver() : m_Parent(NULL), m_ErrorOccured(false) {}
    virtual ~VideoImagerControllerObserver(){}

  private:
    VideoFrameGrabberAndViewer *m_Parent;
    bool m_ErrorOccured;
    std::string m_ErrorMessage;

    //purposely not implemented
    VideoImagerControllerObserver(const Self&);
    void operator=(const Self&);
  };

  /** VideoImager controller object */
  igstk::VideoImagerController::Pointer          m_VideoImagerController;

  /** VideoImager controller observer object */
  VideoImagerControllerObserver::Pointer         m_VideoImagerControllerObserver;

  /** VideoImager controller configuration */
  igstk::CompressedDVVideoImagerConfiguration * m_VideoImagerConfiguration;

  /** Action methods to be invoked only by the state machine */
  void ReportInvalidRequestProcessing();
  void ReportSuccessVideoImagerInitializationProcessing();
  void ReportFailureVideoImagerInitializationProcessing();
  void ReportSuccessStopImagingProcessing();
  void ReportFailureStopImagingProcessing();
  void ReportSuccessVideoImagerDisconnectionProcessing();
  void ReportFailureVideoImagerDisconnectionProcessing();
  void StopImagingProcessing();
  void StartImagingProcessing();
  void DisconnectVideoImagerProcessing();
  void InitializeVideoImagerProcessing();

  /** Log file */
  std::ofstream                             m_LogFile;

  /** Callback functions for picking and reslicing image events. */
  void HandleKeyPressed(igstk::VideoFrameGrabberAndViewerQuadrantViews::
                        KeyboardCommandType keyCommand);
  void HandleKeyPressedCallback( const itk::EventObject & event );
  void HandleMousePressedCallback( const itk::EventObject & event );
  void NullActionCallback(const itk::EventObject & event ){};
  void HandleMousePressed(igstk::VideoFrameGrabberAndViewerQuadrantViews::
                          MouseCommandType mouseCommand);

};

#endif
