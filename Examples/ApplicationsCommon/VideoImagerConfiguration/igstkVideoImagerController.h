/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoImagerController.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkVideoImagerController_h
#define __igstkVideoImagerController_h

#include "igstkSandboxConfigure.h"

#include "igstkVideoImagerConfiguration.h"

#include "igstkStateMachine.h"
#include "igstkMacros.h"
#include "igstkEvents.h"
#include "igstkObject.h"

#include "igstkVideoImager.h"
#include "igstkVideoImagerTool.h"

#if defined(WIN32) || defined(_WIN32)
//WebcamWin framegrabber specific headers
#include "igstkWebcamWinVideoImagerConfiguration.h"
#include "igstkWebcamWinVideoImagerTool.h"
#ifdef IGSTKSandbox_USE_OpenIGTLink
//Terason Ultrasound specific headers
#include "igstkTerasonVideoImagerConfiguration.h"
#include "igstkTerasonVideoImagerTool.h"
#include "igtlServerSocket.h"
#endif
#else
//ImagingSource framegrabber specific headers
#include "igstkImagingSourceVideoImagerConfiguration.h"
#include "igstkImagingSourceVideoImagerTool.h"
//CompressedDV framegrabber specific headers
#include "igstkCompressedDVVideoImagerConfiguration.h"
#include "igstkCompressedDVVideoImagerTool.h"
#endif


namespace igstk
{

/** \class VideoImagerController
 *
 *  \brief This class encapsulates the startup and shutdown procedures required
 *         for the VideoImager classes supported by IGSTK.
 *
 *  This class enables the user to intialize a VideoImager with minimal effort.
 *  Initialization is performed by passing the controller a VideoImagerConfiguration
 *  object. The controller identifies the specific VideoImager internally. The
 *  user can then request to intialize the VideoImager or to tell the VideoImager
 *  controller to shut down a VideoImager that is running.
 *
 *  NOTE: This class is usefull only when all tools are connected to the VideoImager
 *        in advance (no swapping tools in and out after initialization).
 */
class VideoImagerController : public Object
{

public:
  /** Macro with standard traits declarations (Self, SuperClass, State
   *  Machine etc.). */
  igstkStandardClassTraitsMacro( VideoImagerController, Object )

  /**
   * Initialize a VideoImager using the given configuration. The VideoImager and
   * communication are established and the VideoImager is in imaging mode.
   * NOTE: If an VideoImager was already initialized using this object it will be
   *       shut down (stop imaging, close communication, and if using serial
   *       communication that will be closed too).
   *
   * @param configuration A specific VideoImager configuration (Terason,
   *        ImagingSource, etc) which is identified at runtime.
   * This method generates two events, InitializationSuccessEvent if everything
   * went well and, InitializationFailureEvent if the VideoImager could not be
   * initialized (the specific error is described in the string contained in the
   * error object).
   */
  void RequestInitialize(const VideoImagerConfiguration *configuration);

  void RequestStart();

  void RequestStop();

  void RequestSetProgressCallback(itk::Command *progressCallback);

  /**
   * Stop the VideoImager and close its communication.
   * This method generates ShutdownSuccessEvent if everything
   * went well and, a ShutdownErrorEvent if there were problems during
   * shutdown.
   */
  void RequestShutdown();

  void RequestGetVideoImager();

  void RequestGetToolList();

  /** This event is generated if the initialization succeeds. */
  igstkEventMacro( InitializeSuccessEvent, IGSTKEvent );

  /** This event is generated if the start imaging procedure succeeds. */
  igstkEventMacro( StartSuccessEvent, IGSTKEvent );

  /** This event is generated if the stop imaging procedure succeeds. */
  igstkEventMacro( StopSuccessEvent, IGSTKEvent );

  /** This event is generated if the shutdown succeeds. */
  igstkEventMacro( ShutdownSuccessEvent, IGSTKEvent );

  /** This event is generated if communication open fails*/
  igstkLoadedEventMacro( OpenCommunicationFailureEvent,
                         IGSTKErrorEvent,
                         EventHelperType::StringType );

  /** This event is generated if the initialization fails*/
  igstkLoadedEventMacro( InitializeFailureEvent,
                         IGSTKErrorEvent,
                         EventHelperType::StringType );

  /** This event is generated if the start imaging procedure fails*/
  igstkLoadedEventMacro( StartFailureEvent,
                         IGSTKErrorEvent,
                         EventHelperType::StringType );

  /** This event is generated if the user requests the VideoImager and it is
   *  initialized. */
  igstkLoadedEventMacro( RequestVideoImagerEvent,
                         IGSTKEvent,
                         VideoImager::Pointer );

  /** This event is generated if the user requests the VideoImager tools and the
   *  VideoImager is initialized. */
  igstkLoadedEventMacro( RequestToolsEvent,
                         IGSTKEvent,
                         std::vector<VideoImagerTool::Pointer> );

  /** This event is generated if the stop imaging procedure encountered
   *  difficulties */
  igstkLoadedEventMacro( StopFailureEvent,
                         IGSTKErrorEvent,
                         EventHelperType::StringType );

  /** This event is generated if the shutdown encountered difficulties
   *   (e.g. VideoImager communication was already closed from outside). */
  igstkLoadedEventMacro( ShutdownFailureEvent,
                         IGSTKErrorEvent,
                         EventHelperType::StringType );

protected:

  VideoImagerController( void );
  ~VideoImagerController( void );

private:

  /** List of state machine states */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( AttemptingToInitialize );
  igstkDeclareStateMacro( AttemptingToStart );
  igstkDeclareStateMacro( AttemptingToStop );
  igstkDeclareStateMacro( AttemptingToShutdown );

#if defined(WIN32) || defined(_WIN32)
#ifdef IGSTKSandbox_USE_OpenIGTLink
  //Terason Ultrasound specific :begin
  igstkDeclareStateMacro( AttemptingToInitializeTerason );
  //Terason Ultrasound specific :end
#endif
  //WebcamWin framegrabber specific :begin
    igstkDeclareStateMacro( AttemptingToInitializeWebcamWin  );
  //WebcamWin framegrabber specific :end
#else
  //ImagingSource framegrabber specific :begin
  igstkDeclareStateMacro( AttemptingToInitializeImagingSource  );
  //ImagingSource framegrabber specific :end

  //CompressedDV framegrabber specific :begin
    igstkDeclareStateMacro( AttemptingToInitializeCompressedDV  );
  //CompressedDV framegrabber specific :end
#endif

  igstkDeclareStateMacro( Initialized );
  igstkDeclareStateMacro( Started );

  /** List of state machine inputs */
  igstkDeclareInputMacro( VideoImagerInitialize );
  igstkDeclareInputMacro( VideoImagerStart );
  igstkDeclareInputMacro( VideoImagerStop );
  igstkDeclareInputMacro( VideoImagerShutdown );

#if defined(WIN32) || defined(_WIN32)
#ifdef IGSTKSandbox_USE_OpenIGTLink
  //Terason Ultrasound specific :begin
  igstkDeclareInputMacro( TerasonInitialize );
  //Terason Ultrasound specific :end
#endif

 //WebcamWin framegrabber specific :begin
  igstkDeclareInputMacro( WebcamWinInitialize );
  //WebcamWin framegrabber specific :end
#else
  //ImagingSource framegrabber specific :begin
  igstkDeclareInputMacro( ImagingSourceInitialize );
  //ImagingSource framegrabber specific :end

  //CompressedDV framegrabber specific :begin
    igstkDeclareInputMacro( CompressedDVInitialize );
  //CompressedDV framegrabber specific :end
#endif


  igstkDeclareInputMacro( Failed  );
  igstkDeclareInputMacro( Succeeded  );
  igstkDeclareInputMacro( GetVideoImager  );
  igstkDeclareInputMacro( GetTools  );

  /**List of state machine actions*/
  void VideoImagerInitializeProcessing();
  void VideoImagerStartProcessing();
  void VideoImagerStopProcessing();
  void VideoImagerShutdownProcessing();

#if defined(WIN32) || defined(_WIN32)
#ifdef IGSTKSandbox_USE_OpenIGTLink
  //Terason Ultrasound specific :begin
  void TerasonInitializeProcessing();
  //Terason Ultrasound specific :end

  //WebcamWin framegrabber specific :begin
  void WebcamWinInitializeProcessing();
  //WebcamWin framegrabber specific :end
#endif
#else
  //ImagingSource framegrabber specific :begin
  void ImagingSourceInitializeProcessing();
  //ImagingSource framegrabber specific :end

  //CompressedDV framegrabber specific :begin
    void CompressedDVInitializeProcessing();
  //CompressedDV framegrabber specific :end
#endif


  void GetVideoImagerProcessing();
  void GetToolsProcessing();
  void ReportInvalidRequestProcessing();

  void ReportInitializationFailureProcessing();
  void ReportInitializationSuccessProcessing();

  void ReportStartFailureProcessing();
  void ReportStartSuccessProcessing();

  void ReportStopFailureProcessing();
  void ReportStopSuccessProcessing();

  void ReportShutdownFailureProcessing();
  void ReportShutdownSuccessProcessing();

  bool InitializeSerialCommunication();

#if defined(WIN32) || defined(_WIN32)
#ifdef IGSTKSandbox_USE_OpenIGTLink
  //Terason Ultrasound specific :begin
  bool InitializeSocketCommunication();
  TerasonVideoImagerTool::Pointer InitializeTerasonTool(
    const TerasonToolConfiguration *toolConfiguration );
  //Terason Ultrasound specific :end
#endif
  //WebcamWin framegrabber specific :begin
  WebcamWinVideoImagerTool::Pointer InitializeWebcamWinTool(
    const WebcamWinToolConfiguration *toolConfiguration );
  //WebcamWin framegrabber specific :end
#else
  //ImagingSource framegrabber specific :begin
  ImagingSourceVideoImagerTool::Pointer InitializeImagingSourceTool(
    const ImagingSourceToolConfiguration *toolConfiguration );
  //ImagingSource framegrabber specific :end

  //CompressedDV framegrabber specific :begin
  CompressedDVVideoImagerTool::Pointer InitializeCompressedDVTool(
    const CompressedDVToolConfiguration *toolConfiguration );
  //CompressedDV framegrabber specific :end
#endif

  class ErrorObserver : public itk::Command
  {
  public:
    typedef ErrorObserver                    Self;
    typedef ::itk::Command                   Superclass;
    typedef ::itk::SmartPointer<Self>        Pointer;
    typedef ::itk::SmartPointer<const Self>  ConstPointer;

    igstkNewMacro(Self)
    igstkTypeMacro(ErrorObserver, itk::Command)

    /** When an error occurs in an IGSTK component it will invoke this method
     *  with the appropriate error event object as a parameter.*/
    virtual void Execute(itk::Object *caller,
                         const itk::EventObject & event) throw (std::exception);

    /** When an error occurs in an IGSTK component it will invoke this method
     *  with the appropriate error event object as a parameter.*/
    virtual void Execute(const itk::Object *caller,
                         const itk::EventObject & event) throw (std::exception);

    /**Clear the current error.*/
    void ClearError()
    {
      this->m_ErrorOccured = false;
      this->m_ErrorMessage.clear();
    }

    /**If an error occurs in one of the observed IGSTK components this method
     * will return true.*/
    bool ErrorOccured() { return this->m_ErrorOccured; }
    /**Get the error message associated with the last IGSTK error.*/
    void GetErrorMessage(std::string &errorMessage)
      {
        errorMessage = this->m_ErrorMessage;
      }

  protected:

    /**Construct an error observer for all the possible errors that occur in
     * the observed IGSTK components.*/
    ErrorObserver();
    virtual ~ErrorObserver(){}
  private:
    bool                              m_ErrorOccured;
    std::string                       m_ErrorMessage;
    std::map<std::string,std::string> m_ErrorEvent2ErrorMessage;

    //purposely not implemented
    ErrorObserver(const Self&);
    void operator=(const Self&);
    };

  ErrorObserver::Pointer m_ErrorObserver;
  std::string m_ErrorMessage;

  VideoImagerConfiguration  *m_TmpVideoImagerConfiguration;
  VideoImagerConfiguration  *m_VideoImagerConfiguration;
  VideoImager::Pointer       m_VideoImager;
  std::vector<VideoImagerTool::Pointer> m_Tools;

#ifdef IGSTKSandbox_USE_OpenIGTLink
  //Terason Ultrasound specific :begin
  igtl::ServerSocket::Pointer m_SocketCommunication;
  SerialCommunication::Pointer m_SerialCommunication;
  //igtl::ClientSocket::Pointer m_SocketCommunication;
  //Terason Ultrasound specific :end
#endif

  itk::Command *m_ProgressCallback;
};

} // end of namespace
#endif
