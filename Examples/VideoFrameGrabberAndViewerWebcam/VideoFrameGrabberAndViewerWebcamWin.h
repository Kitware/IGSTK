/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    VideoFrameGrabberAndViewerWebcamWin.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __VideoFrameGrabberAndViewerWebcamWin_h
#define __VideoFrameGrabberAndViewerWebcamWin_h

#include "VideoFrameGrabberAndViewerWebcamWinGUI.h"

#include "igstkSandboxConfigure.h"

#include "igstkStateMachine.h"
#include "igstkMacros.h"
#include "igstkObject.h"
#include "igstkEvents.h"

#include "igstkImagerController.h"
#include "igstkImagerConfiguration.h"

#include "igstkAxesObject.h"
#include "igstkAxesObjectRepresentation.h"

#include "igstkVideoFrameSpatialObject.h"
#include "igstkVideoFrameRepresentation.h"
#include "igstkFrame.h"

#include "itkStdStreamLogOutput.h"


/** \class VideoFrameGrabberAndViewerWebcamWin
*
* \brief Implementation class for VideoFrameGrabberAndViewerWebcamWinGUI.
*
* This class implements the main application.
*
*/
class VideoFrameGrabberAndViewerWebcamWin : public VideoFrameGrabberAndViewerWebcamWinGUI
{
public:

  /** Typedefs */
  igstkStandardClassBasicTraitsMacro( VideoFrameGrabberAndViewerWebcamWin,
                                      VideoFrameGrabberAndViewerWebcamWinGUI );

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

  /** imager object and imager tool*/
  igstk::Imager::Pointer                    m_Imager;
  igstk::ImagerTool::Pointer                m_WebcamWinImagerTool;

  /** Observer type for loaded event,
   *  the callback can be set to a member function. */
  typedef itk::ReceptorMemberCommand < Self > LoadedObserverType;

  /** observer for key pressed event */
  LoadedObserverType::Pointer               m_KeyPressedObserver;

  /** observer for mouse button pressed event */
  LoadedObserverType::Pointer               m_MousePressedObserver;

  /** observers for the imager tool */
  LoadedObserverType::Pointer               m_ImagerToolUpdateObserver;
  LoadedObserverType::Pointer               m_ImagerToolNotAvailableObserver;
  LoadedObserverType::Pointer               m_ImagerToolAvailableObserver;

  bool m_VideoEnabled;
  bool m_ImagerInitialized;
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

  virtual void RequestInitializeImager();
  virtual void RequestStartImaging();
  virtual void ReportSuccessStartImagingProcessing();
  virtual void ReportFailureStartImagingProcessing();
  virtual void RequestStopImaging();
  virtual void RequestDisconnectImager();

  igstkEventMacro( InvalidRequestErrorEvent, igstk::IGSTKErrorEvent );

  VideoFrameGrabberAndViewerWebcamWin();
  virtual ~VideoFrameGrabberAndViewerWebcamWin();

private:

  VideoFrameGrabberAndViewerWebcamWin(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented

  /** States for the State Machine */
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( InitializingImager );
  igstkDeclareStateMacro( Imaging );
  igstkDeclareStateMacro( StopImaging );

  /** Inputs to the state machine and it's designed transitions */
  igstkDeclareInputMacro( Success );
  igstkDeclareInputMacro( Failure );
  igstkDeclareInputMacro( InitializeImager );
  igstkDeclareInputMacro( StartImaging );
  igstkDeclareInputMacro( StopImaging );
  igstkDeclareInputMacro( DisconnectImager );


  /** Define a initial world coordinate system */
  AxesObjectType::Pointer                   m_WorldReference;
  AxesRepresentationType::Pointer           m_WorldReferenceRepresentation;

  /** Class that observes the events generated by the imager controller. */
  class ImagerControllerObserver : public itk::Command
  {
  public:
    typedef ImagerControllerObserver             Self;
    typedef ::itk::Command                       Superclass;
    typedef ::itk::SmartPointer<Self>            Pointer;
    typedef ::itk::SmartPointer<const Self>      ConstPointer;

    igstkNewMacro(Self)
    igstkTypeMacro(ImagerControllerObserver, itk::Command)

    void SetParent(VideoFrameGrabberAndViewerWebcamWin *p);

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
    ImagerControllerObserver() : m_Parent(NULL), m_ErrorOccured(false) {}
    virtual ~ImagerControllerObserver(){}

  private:
    VideoFrameGrabberAndViewerWebcamWin *m_Parent;
    bool m_ErrorOccured;
    std::string m_ErrorMessage;

    //purposely not implemented
    ImagerControllerObserver(const Self&);
    void operator=(const Self&);
  };

  /** imager controller object */
  igstk::ImagerController::Pointer          m_ImagerController;

  /** imager controller observer object */
  ImagerControllerObserver::Pointer         m_ImagerControllerObserver;

  /** imager controller configuration */
  igstk::WebcamWinImagerConfiguration * m_ImagerConfiguration;

  /** Action methods to be invoked only by the state machine */
  void ReportInvalidRequestProcessing();
  void ReportSuccessImagerInitializationProcessing();
  void ReportFailureImagerInitializationProcessing();
  void ReportSuccessStopImagingProcessing();
  void ReportFailureStopImagingProcessing();
  void ReportSuccessImagerDisconnectionProcessing();
  void ReportFailureImagerDisconnectionProcessing();
  void StopImagingProcessing();
  void StartImagingProcessing();
  void DisconnectImagerProcessing();
  void InitializeImagerProcessing();

  /** Log file */
  std::ofstream                             m_LogFile;

  /** Callback functions for picking and reslicing image events. */
  void HandleKeyPressed(igstk::VideoFrameGrabberAndViewerWebcamWinQuadrantViews::
                        KeyboardCommandType keyCommand);
  void HandleKeyPressedCallback( const itk::EventObject & event );
  void HandleMousePressedCallback( const itk::EventObject & event );
  void NullActionCallback(const itk::EventObject & event ){};
  void HandleMousePressed(igstk::VideoFrameGrabberAndViewerWebcamWinQuadrantViews::
                          MouseCommandType mouseCommand);

};

#endif
