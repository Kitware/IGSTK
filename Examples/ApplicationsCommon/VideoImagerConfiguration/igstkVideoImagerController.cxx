/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoImagerController.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkVideoImagerController.h"

#include "igstkVideoImagerConfiguration.h"

#include "itksys/SystemTools.hxx"
#include "itksys/Directory.hxx"

#if defined(WIN32) || defined(_WIN32)
#ifdef IGSTKSandbox_USE_OpenIGTLink
//Terason Ultrasound specific header
#include "igstkTerasonVideoImager.h"
#endif
//WebcamWin specific header
#include "igstkWebcamWinVideoImager.h"
#else
//ImagingSource framegrabber specific header
#include "igstkImagingSourceVideoImager.h"
//CompressedDV specific header
#include "igstkCompressedDVVideoImager.h"
//#include "igstkCalibrationIO.h"
#endif


namespace igstk
{

VideoImagerController::VideoImagerController() : m_StateMachine( this )
{
  //create error observer
  this->m_ErrorObserver = ErrorObserver::New();

  m_ProgressCallback = NULL;

  //define the state machine's states
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( AttemptingToInitialize );

#if defined(WIN32) || defined(_WIN32)
#ifdef IGSTKSandbox_USE_OpenIGTLink
  //Terason Ultrasound specific :begin
  igstkAddStateMacro( AttemptingToInitializeTerason );
  //Terason Ultrasound specific :end
#endif
  //WebcamWin framegrabber specific :begin
    igstkAddStateMacro( AttemptingToInitializeWebcamWin );
  //WebcamWin framegrabber specific :end
#else
  //ImagingSource framegrabber specific :begin
  igstkAddStateMacro( AttemptingToInitializeImagingSource );
  //ImagingSource framegrabber specific :end
  //CompressedDV framegrabber specific :begin
    igstkAddStateMacro( AttemptingToInitializeCompressedDV );
    //CompressedDV framegrabber specific :end
#endif



  igstkAddStateMacro( AttemptingToShutdown );
  igstkAddStateMacro( Initialized );
  igstkAddStateMacro( AttemptingToStart );
  igstkAddStateMacro( AttemptingToStop );
  igstkAddStateMacro( Started );

  //define the state machine's inputs
  igstkAddInputMacro( VideoImagerInitialize );
  igstkAddInputMacro( VideoImagerStart );
  igstkAddInputMacro( VideoImagerStop );
  igstkAddInputMacro( VideoImagerShutdown );

#if defined(WIN32) || defined(_WIN32)
#ifdef IGSTKSandbox_USE_OpenIGTLink
  //Terason Ultrasound specific :begin
  igstkAddInputMacro( TerasonInitialize );
  //Terason Ultrasound specific :end
#endif
 //WebcamWin framegrabber specific :begin
    igstkAddInputMacro( WebcamWinInitialize );
  //WebcamWin framegrabber specific :end
#else
  //ImagingSource framegrabber specific :begin
  igstkAddInputMacro( ImagingSourceInitialize );
  //ImagingSource framegrabber specific :end
  //CompressedDV framegrabber specific :begin
    igstkAddInputMacro( CompressedDVInitialize );
  //CompressedDV framegrabber specific :end
#endif

  igstkAddInputMacro( Failed  );
  igstkAddInputMacro( Succeeded  );
  igstkAddInputMacro( GetVideoImager  );
  igstkAddInputMacro( GetTools  );

  //define the state machine's transitions

  //transitions from Idle state
  igstkAddTransitionMacro(Idle,
                          VideoImagerInitialize,
                          AttemptingToInitialize,
                          VideoImagerInitialize);

  igstkAddTransitionMacro(Idle,
                          VideoImagerShutdown,
                          Idle,
                          ReportInvalidRequest);
#if defined(WIN32) || defined(_WIN32)
#ifdef IGSTKSandbox_USE_OpenIGTLink
  igstkAddTransitionMacro(Idle,
                          TerasonInitialize,
                          Idle,
                          ReportInvalidRequest);
#endif
igstkAddTransitionMacro(Idle,
                        WebcamWinInitialize,
                            Idle,
                            ReportInvalidRequest);
#else
  igstkAddTransitionMacro(Idle,
                          ImagingSourceInitialize,
                          Idle,
                          ReportInvalidRequest);
  igstkAddTransitionMacro(Idle,
                      CompressedDVInitialize,
                          Idle,
                          ReportInvalidRequest);
#endif


  igstkAddTransitionMacro(Idle,
                          Failed,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Idle,
                          Succeeded,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Idle,
                          GetVideoImager,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Idle,
                          GetTools,
                          Idle,
                          ReportInvalidRequest);

  //transitions from AttemptingToInitialize state
  igstkAddTransitionMacro(AttemptingToInitialize,
                          Failed,
                          Idle,
                          ReportInitializationFailure);
#if defined(WIN32) || defined(_WIN32)
#ifdef IGSTKSandbox_USE_OpenIGTLink
  igstkAddTransitionMacro(AttemptingToInitialize,
                          TerasonInitialize,
                          AttemptingToInitializeTerason,
                          TerasonInitialize);
#endif
igstkAddTransitionMacro(AttemptingToInitialize,
                            WebcamWinInitialize,
                              AttemptingToInitializeWebcamWin,
                              WebcamWinInitialize);
#else
  igstkAddTransitionMacro(AttemptingToInitialize,
                          ImagingSourceInitialize,
                          AttemptingToInitializeImagingSource,
                          ImagingSourceInitialize);
  igstkAddTransitionMacro(AttemptingToInitialize,
                          CompressedDVInitialize,
                            AttemptingToInitializeTerason,
                            CompressedDVInitialize);
#endif

  igstkAddTransitionMacro(AttemptingToInitialize,
                          VideoImagerInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          VideoImagerShutdown,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          Succeeded,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          GetVideoImager,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          GetTools,
                          Idle,
                          ReportInvalidRequest);
#if defined(WIN32) || defined(_WIN32)
  //transitions from AttemptingToInitializeTerason state
  igstkAddTransitionMacro(AttemptingToInitializeTerason,
                          Failed,
                          Idle,
                          ReportInitializationFailure);

  igstkAddTransitionMacro(AttemptingToInitializeTerason,
                          Succeeded,
                          Initialized,
                          ReportInitializationSuccess);

  igstkAddTransitionMacro(AttemptingToInitializeTerason,
                          VideoImagerInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializeTerason,
                          VideoImagerShutdown,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializeTerason,
                          TerasonInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializeTerason,
                          GetVideoImager,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializeTerason,
                          GetTools,
                          Idle,
                          ReportInvalidRequest);
   //transitions from AttemptingToInitializeWebcamWin state
        igstkAddTransitionMacro(AttemptingToInitializeWebcamWin,
                                Failed,
                                Idle,
                                ReportInitializationFailure);

        igstkAddTransitionMacro(AttemptingToInitializeWebcamWin,
                                Succeeded,
                                Initialized,
                                ReportInitializationSuccess);

        igstkAddTransitionMacro(AttemptingToInitializeWebcamWin,
                                VideoImagerInitialize,
                                Idle,
                                ReportInvalidRequest);

        igstkAddTransitionMacro(AttemptingToInitializeWebcamWin,
                                VideoImagerShutdown,
                                Idle,
                                ReportInvalidRequest);

        igstkAddTransitionMacro(AttemptingToInitializeWebcamWin,
                                TerasonInitialize,
                                Idle,
                                ReportInvalidRequest);

        igstkAddTransitionMacro(AttemptingToInitializeWebcamWin,
                                GetVideoImager,
                                Idle,
                                ReportInvalidRequest);

        igstkAddTransitionMacro(AttemptingToInitializeWebcamWin,
                                GetTools,
                                Idle,
                                ReportInvalidRequest);
#else
  //transitions from AttemptingToInitializeImagingSource state
  igstkAddTransitionMacro(AttemptingToInitializeImagingSource,
                          Failed,
                          Idle,
                          ReportInitializationFailure);

  igstkAddTransitionMacro(AttemptingToInitializeImagingSource,
                          Succeeded,
                          Initialized,
                          ReportInitializationSuccess);

  igstkAddTransitionMacro(AttemptingToInitializeImagingSource,
                          VideoImagerInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializeImagingSource,
                          VideoImagerShutdown,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializeImagingSource,
                          TerasonInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializeImagingSource,
                          GetVideoImager,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializeImagingSource,
                          GetTools,
                          Idle,
                          ReportInvalidRequest);
  //transitions from AttemptingToInitializeCompressedDV state
    igstkAddTransitionMacro(AttemptingToInitializeCompressedDV,
                            Failed,
                            Idle,
                            ReportInitializationFailure);

    igstkAddTransitionMacro(AttemptingToInitializeCompressedDV,
                            Succeeded,
                            Initialized,
                            ReportInitializationSuccess);

    igstkAddTransitionMacro(AttemptingToInitializeCompressedDV,
                            VideoImagerInitialize,
                            Idle,
                            ReportInvalidRequest);

    igstkAddTransitionMacro(AttemptingToInitializeCompressedDV,
                            VideoImagerShutdown,
                            Idle,
                            ReportInvalidRequest);

    igstkAddTransitionMacro(AttemptingToInitializeCompressedDV,
                            TerasonInitialize,
                            Idle,
                            ReportInvalidRequest);

    igstkAddTransitionMacro(AttemptingToInitializeCompressedDV,
                            GetVideoImager,
                            Idle,
                            ReportInvalidRequest);

    igstkAddTransitionMacro(AttemptingToInitializeCompressedDV,
                            GetTools,
                            Idle,
                            ReportInvalidRequest);
#endif


  //transitions from Initialized state
  igstkAddTransitionMacro(Initialized,
                          GetVideoImager,
                          Initialized,
                          GetVideoImager);

  igstkAddTransitionMacro(Initialized,
                          GetTools,
                          Initialized,
                          GetTools);

  igstkAddTransitionMacro(Initialized,
                          VideoImagerInitialize,
                          AttemptingToInitialize,
                          VideoImagerInitialize);

  igstkAddTransitionMacro(Initialized,
                          VideoImagerShutdown,
                          AttemptingToShutdown,
                          VideoImagerShutdown);

  igstkAddTransitionMacro(Initialized,
                          VideoImagerStart,
                          AttemptingToStart,
                          VideoImagerStart);
#if defined(WIN32) || defined(_WIN32)
  igstkAddTransitionMacro(Initialized,
                          TerasonInitialize,
                          Initialized,
                          ReportInvalidRequest);
igstkAddTransitionMacro(Initialized,
                              WebcamWinInitialize,
                              Initialized,
                              ReportInvalidRequest);
#else
  igstkAddTransitionMacro(Initialized,
                          ImagingSourceInitialize,
                          Initialized,
                          ReportInvalidRequest);
  igstkAddTransitionMacro(Initialized,
                            CompressedDVInitialize,
                            Initialized,
                            ReportInvalidRequest);
#endif

  igstkAddTransitionMacro(Initialized,
                          VideoImagerStop,
                          Initialized,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Initialized,
                          Failed,
                          Initialized,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Initialized,
                          Succeeded,
                          Initialized,
                          ReportInvalidRequest);

  //transitions from AttemtingToStart state
  igstkAddTransitionMacro(AttemptingToStart,
                          Succeeded,
                          Started,
                          ReportStartSuccess);

  igstkAddTransitionMacro(AttemptingToStart,
                          Failed,
                          Initialized,
                          ReportStartFailure);

  igstkAddTransitionMacro(AttemptingToStart,
                          GetVideoImager,
                          AttemptingToStart,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStart,
                          GetTools,
                          AttemptingToStart,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStart,
                          VideoImagerInitialize,
                          AttemptingToStart,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStart,
                          VideoImagerShutdown,
                          AttemptingToStart,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStart,
                          VideoImagerStart,
                          AttemptingToStart,
                          ReportInvalidRequest);
#if defined(WIN32) || defined(_WIN32)
  igstkAddTransitionMacro(AttemptingToStart,
                          TerasonInitialize,
                          AttemptingToStart,
                          ReportInvalidRequest);
igstkAddTransitionMacro(AttemptingToStart,
                              WebcamWinInitialize,
                              AttemptingToStart,
                              ReportInvalidRequest);
#else
  igstkAddTransitionMacro(AttemptingToStart,
                          ImagingSourceInitialize,
                          AttemptingToStart,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStart,
                            CompressedDVInitialize,
                            AttemptingToStart,
                            ReportInvalidRequest);
#endif

  igstkAddTransitionMacro(AttemptingToStart,
                          VideoImagerStop,
                          AttemptingToStart,
                          ReportInvalidRequest);

  //transitions from Started state
  igstkAddTransitionMacro(Started,
                          VideoImagerStop,
                          AttemptingToStop,
                          VideoImagerStop);
#if defined(WIN32) || defined(_WIN32)
  igstkAddTransitionMacro(Started,
                          TerasonInitialize,
                          Started,
                          ReportInvalidRequest);
igstkAddTransitionMacro(Started,
                             WebcamWinInitialize,
                             Started,
                             ReportInvalidRequest);
#else
  igstkAddTransitionMacro(Started,
                          ImagingSourceInitialize,
                          Started,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Started,
                            CompressedDVInitialize,
                            Started,
                            ReportInvalidRequest);
#endif
  igstkAddTransitionMacro(Started,
                          VideoImagerInitialize,
                          Started,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Started,
                          VideoImagerStart,
                          Started,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Started,
                          Succeeded,
                          Started,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Started,
                          Failed,
                          Started,
                          ReportInvalidRequest);

  //transitions from AttemtingtoStop state
  igstkAddTransitionMacro(AttemptingToStop,
                          Succeeded,
                          Initialized,
                          ReportStopSuccess);

  igstkAddTransitionMacro(AttemptingToStop,
                          Failed,
                          Started,
                          ReportStopFailure);

  igstkAddTransitionMacro(AttemptingToStop,
                          GetVideoImager,
                          AttemptingToStop,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStop,
                          GetTools,
                          AttemptingToStop,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStop,
                          VideoImagerInitialize,
                          AttemptingToStop,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStop,
                          VideoImagerShutdown,
                          AttemptingToStop,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStop,
                          VideoImagerStart,
                          AttemptingToStop,
                          ReportInvalidRequest);
#if defined(WIN32) || defined(_WIN32)
  igstkAddTransitionMacro(AttemptingToStop,
                          TerasonInitialize,
                          AttemptingToStop,
                          ReportInvalidRequest);
igstkAddTransitionMacro(AttemptingToStop,
                              WebcamWinInitialize,
                              AttemptingToStop,
                              ReportInvalidRequest);
#else
  igstkAddTransitionMacro(AttemptingToStop,
                          ImagingSourceInitialize,
                          AttemptingToStop,
                          ReportInvalidRequest);
  igstkAddTransitionMacro(AttemptingToStop,
                            CompressedDVInitialize,
                            AttemptingToStop,
                            ReportInvalidRequest);
#endif
  igstkAddTransitionMacro(AttemptingToStop,
                          VideoImagerStop,
                          AttemptingToStop,
                          ReportInvalidRequest);

  //transitions from AttemptingToShutdown state
  igstkAddTransitionMacro(AttemptingToShutdown,
                          Succeeded,
                          Idle,
                          ReportShutdownSuccess);

  igstkAddTransitionMacro(AttemptingToShutdown,
                          Failed,
                          Initialized,
                          ReportShutdownFailure);

  igstkAddTransitionMacro(AttemptingToShutdown,
                          VideoImagerInitialize,
                          AttemptingToShutdown,
                          ReportInvalidRequest);
#if defined(WIN32) || defined(_WIN32)
  igstkAddTransitionMacro(AttemptingToShutdown,
                          TerasonInitialize,
                          AttemptingToShutdown,
                          ReportInvalidRequest);
igstkAddTransitionMacro(AttemptingToShutdown,
                             WebcamWinInitialize,
                             AttemptingToShutdown,
                             ReportInvalidRequest);
#else
  igstkAddTransitionMacro(AttemptingToShutdown,
                          ImagingSourceInitialize,
                          AttemptingToShutdown,
                          ReportInvalidRequest);
  igstkAddTransitionMacro(AttemptingToShutdown,
                           CompressedDVInitialize,
                           AttemptingToShutdown,
                           ReportInvalidRequest);
#endif

  igstkAddTransitionMacro(AttemptingToShutdown,
                          VideoImagerShutdown,
                          AttemptingToShutdown,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToShutdown,
                          GetVideoImager,
                          AttemptingToShutdown,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToShutdown,
                          GetTools,
                          AttemptingToShutdown,
                          ReportInvalidRequest);

  //set the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

  // done setting the state machine, ready to run
  this->m_StateMachine.SetReadyToRun();
}


VideoImagerController::~VideoImagerController()
{

}

void
VideoImagerController::RequestSetProgressCallback(itk::Command *progressCallback)
{
  m_ProgressCallback = progressCallback;
}

void
VideoImagerController::RequestInitialize(
  const VideoImagerConfiguration *configuration)
{
  igstkLogMacro( DEBUG,
                 "igstkVideoImagerController::RequestInitialize called...\n" );

  this->m_TmpVideoImagerConfiguration =
    const_cast<VideoImagerConfiguration *>(configuration);
  igstkPushInputMacro( VideoImagerInitialize );
  this->m_StateMachine.ProcessInputs();
}

void
VideoImagerController::RequestStart()
{
  igstkLogMacro( DEBUG,
                 "igstkVideoImagerController::RequestStart called...\n" );
  igstkPushInputMacro( VideoImagerStart );
  this->m_StateMachine.ProcessInputs();
}

void
VideoImagerController::RequestStop()
{
  igstkLogMacro( DEBUG,
                 "igstkVideoImagerController::RequestStop called...\n" );
  igstkPushInputMacro( VideoImagerStop );
  this->m_StateMachine.ProcessInputs();
}

void
VideoImagerController::RequestShutdown()
{
  igstkLogMacro( DEBUG,
                 "igstkVideoImagerController::RequestShutdown called...\n" );
  igstkPushInputMacro( VideoImagerShutdown );
  this->m_StateMachine.ProcessInputs();
}

void
VideoImagerController::RequestGetVideoImager()
{
  igstkLogMacro( DEBUG,
                 "igstkVideoImagerController::RequestGetVideoImager called...\n" );
  igstkPushInputMacro( GetVideoImager );
  this->m_StateMachine.ProcessInputs();
}

void
VideoImagerController::RequestGetToolList()
{
  igstkLogMacro( DEBUG,
                 "igstkVideoImagerController::RequestGetToolList called...\n" );
  igstkPushInputMacro( GetTools );
  this->m_StateMachine.ProcessInputs();
}

void
VideoImagerController::VideoImagerInitializeProcessing()
{
  if( this->m_TmpVideoImagerConfiguration == NULL )
  {
    this->m_ErrorMessage = "Null VideoImager configuration received.";
    igstkPushInputMacro( Failed );
  }
  else
  {
#if defined(WIN32) || defined(_WIN32)
#ifdef IGSTKSandbox_USE_OpenIGTLink
    //Terason Ultrasound specific :begin
    if( dynamic_cast<TerasonVideoImagerConfiguration *>
      ( this->m_TmpVideoImagerConfiguration ) )
    {
      this->m_VideoImagerConfiguration = m_TmpVideoImagerConfiguration;
      igstkPushInputMacro( TerasonInitialize );
    }
    //Terason Ultrasound specific :end
#endif
   //WebcamWin framegrabber specific :begin
            if( dynamic_cast<WebcamWinVideoImagerConfiguration *>
              ( this->m_TmpVideoImagerConfiguration ) )
            {
              this->m_VideoImagerConfiguration = m_TmpVideoImagerConfiguration;
              igstkPushInputMacro( WebcamWinInitialize );
            }
    //WebcamWin framegrabber specific :end
#else
    //ImagingSource framegrabber specific :begin
    else if( dynamic_cast<ImagingSourceVideoImagerConfiguration *>
      ( this->m_TmpVideoImagerConfiguration ) )
    {
      this->m_VideoImagerConfiguration = m_TmpVideoImagerConfiguration;
      igstkPushInputMacro( ImagingSourceInitialize );
    }
    //ImagingSource framegrabber specific :end
    //CompressedDV framegrabber specific :begin
        else if( dynamic_cast<CompressedDVVideoImagerConfiguration *>
          ( this->m_TmpVideoImagerConfiguration ) )
        {
          this->m_VideoImagerConfiguration = m_TmpVideoImagerConfiguration;
          igstkPushInputMacro( CompressedDVInitialize );
        }
        //CompressedDV framegrabber specific :end
#endif
    else
    {
      this->m_ErrorMessage = "Unknown VideoImager configuration type.";
      igstkPushInputMacro( Failed );
    }
  }
 this->m_StateMachine.ProcessInputs();
}

void
VideoImagerController::VideoImagerStartProcessing()
{
    StartFailureEvent evt;
    unsigned long observerID;
    observerID = this->m_VideoImager->AddObserver( IGSTKErrorEvent(),
                                             this->m_ErrorObserver );

    m_VideoImager->RequestStartImaging();

    if( this->m_ErrorObserver->ErrorOccured() )
    {
      this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
      this->m_ErrorObserver->ClearError();
      m_VideoImager->RemoveObserver( observerID );
      //m_VideoImager->RequestClose();
      evt.Set( this->m_ErrorMessage );
      this->InvokeEvent( evt );
      igstkPushInputMacro( Failed );
    }
    else
    {
      m_VideoImager->RemoveObserver(observerID);
      igstkPushInputMacro( Succeeded );
    }

    this->m_StateMachine.ProcessInputs();
}

void
VideoImagerController::VideoImagerStopProcessing()
{
  StopFailureEvent evt;
  unsigned long observerID;

  observerID = this->m_VideoImager->AddObserver( IGSTKErrorEvent(),
                                             this->m_ErrorObserver );
  //stop imaging
  this->m_VideoImager->RequestStopImaging();
  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    this->m_VideoImager->RemoveObserver(observerID);
    evt.Set( this->m_ErrorMessage );
    this->InvokeEvent( evt );
    igstkPushInputMacro( Failed );
  }
  else
  {
    this->m_VideoImager->RemoveObserver(observerID);
    igstkPushInputMacro( Succeeded );
  }

  this->m_StateMachine.ProcessInputs();
}

void
VideoImagerController::VideoImagerShutdownProcessing()
{
  ShutdownFailureEvent evt;
  unsigned long observerID;

  observerID = this->m_VideoImager->AddObserver( IGSTKErrorEvent(),
                                             this->m_ErrorObserver );

  //close communication with VideoImager
  this->m_VideoImager->RequestClose();
  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    this->m_VideoImager->RemoveObserver(observerID);
    evt.Set( this->m_ErrorMessage );
    this->InvokeEvent( evt );
    igstkPushInputMacro( Failed );
  }
  else
  {
   this->m_VideoImager->RemoveObserver( observerID );

   //if serial communication, close COM port
   if( this->m_SocketCommunication.IsNotNull() )
   {
      try
      {
        this->m_SocketCommunication->CloseSocket();
      }
      catch (...)
      {
           this->m_ErrorMessage = "Could not close socket";
           evt.Set( this->m_ErrorMessage );
           this->InvokeEvent( evt );
           igstkPushInputMacro( Failed );
      }
   }
   this->m_VideoImager = NULL;
   this->m_Tools.clear();
   this->m_SocketCommunication = NULL;
   igstkPushInputMacro( Succeeded );
  }

  this->m_StateMachine.ProcessInputs();
}

bool
VideoImagerController::InitializeSerialCommunication()
{
  SerialCommunicatingVideoImagerConfiguration *serialVideoImagerConfiguration =
    dynamic_cast<SerialCommunicatingVideoImagerConfiguration *>( this->m_VideoImagerConfiguration );

  //create serial communication
  this->m_SerialCommunication = igstk::SerialCommunication::New();

  //observe errors generated by the serial communication
  unsigned long observerID =
    this->m_SerialCommunication->AddObserver( OpenPortErrorEvent(),
                                              this->m_ErrorObserver );

  this->m_SerialCommunication->SetPortNumber( serialVideoImagerConfiguration->GetCOMPort() );
  this->m_SerialCommunication->SetParity( serialVideoImagerConfiguration->GetParity() );
  this->m_SerialCommunication->SetBaudRate( serialVideoImagerConfiguration->GetBaudRate() );
  this->m_SerialCommunication->SetDataBits( serialVideoImagerConfiguration->GetDataBits() );
  this->m_SerialCommunication->SetStopBits( serialVideoImagerConfiguration->GetStopBits() );
  this->m_SerialCommunication->SetHardwareHandshake( serialVideoImagerConfiguration->GetHandshake() );

  this->m_SerialCommunication->OpenCommunication();
  //remove the observer, if an error occured we have already
  //been notified
  this->m_SerialCommunication->RemoveObserver(observerID);

  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    return false;
  }
  return true;
}

#if defined(WIN32) || defined(_WIN32)
#ifdef IGSTKSandbox_USE_OpenIGTLink

//Terason Ultrasound specific :begin
bool
VideoImagerController::InitializeSocketCommunication()
{
  SocketCommunicatingVideoImagerConfiguration *socketVideoImagerConfiguration =
    dynamic_cast<SocketCommunicatingVideoImagerConfiguration *>( this->m_VideoImagerConfiguration );

  //create client socket communication
  this->m_SocketCommunication = igtl::ServerSocket::New();
  //this->m_SocketCommunication = igtl::ClientSocket::New();

  // std::cout << "socketVideoImagerConfiguration->GetSocketPort() " << socketVideoImagerConfiguration->GetSocketPort() << std::endl;
  // std::cout << "socketVideoImagerConfiguration->GetHostName() " << socketVideoImagerConfiguration->GetHostName() << std::endl;

  int err = this->m_SocketCommunication->CreateServer( socketVideoImagerConfiguration->GetSocketPort() );

  //  int err = this->m_SocketCommunication->ConnectToServer(
  //  socketVideoImagerConfiguration->GetHostName().c_str(), socketVideoImagerConfiguration->GetSocketPort() );

  if (err != 0)
  {
    OpenCommunicationFailureEvent evt;
    this->m_ErrorMessage = "Could not create server";
    evt.Set( this->m_ErrorMessage );
    this->InvokeEvent( evt );
    return false;
  }

  return true;
}


TerasonVideoImagerTool::Pointer VideoImagerController::InitializeTerasonTool(
    const TerasonToolConfiguration *toolConfiguration )
{
  TerasonVideoImagerTool::Pointer VideoImagerTool = TerasonVideoImagerTool::New();

  unsigned int dims[3];
  toolConfiguration->GetFrameDimensions(dims);
  VideoImagerTool->SetFrameDimensions(dims);

  VideoImagerTool->SetPixelDepth(toolConfiguration->GetPixelDepth());
  VideoImagerTool->RequestSetVideoImagerToolName( toolConfiguration->GetToolUniqueIdentifier() );
  std::cout << toolConfiguration->GetToolUniqueIdentifier() << std::endl;
  VideoImagerTool->RequestConfigure();

  // VideoImagerTool->SetCalibrationTransform( identity );

  VideoImagerTool->RequestConfigure();

  return VideoImagerTool;
}
//Terason Ultrasound specific :end
#endif

//WebcamWin framegrabber specific :begin
WebcamWinVideoImagerTool::Pointer VideoImagerController::InitializeWebcamWinTool(
    const WebcamWinToolConfiguration *toolConfiguration )
{
  WebcamWinVideoImagerTool::Pointer VideoImagerTool = WebcamWinVideoImagerTool::New();

  unsigned int dims[3];
  toolConfiguration->GetFrameDimensions(dims);
  VideoImagerTool->SetFrameDimensions(dims);

  VideoImagerTool->SetPixelDepth(toolConfiguration->GetPixelDepth());
  VideoImagerTool->RequestSetVideoImagerToolName(
                            toolConfiguration->GetToolUniqueIdentifier() );
  VideoImagerTool->RequestConfigure();

  // VideoImagerTool->SetCalibrationTransform( identity );

  return VideoImagerTool;
}
//WebcamWin framegrabber specific :end

#else

//ImagingSource framegrabber specific :begin
ImagingSourceVideoImagerTool::Pointer VideoImagerController::InitializeImagingSourceTool(
    const ImagingSourceToolConfiguration *toolConfiguration )
{
  ImagingSourceVideoImagerTool::Pointer VideoImagerTool = ImagingSourceVideoImagerTool::New();

  unsigned int dims[3];
  toolConfiguration->GetFrameDimensions(dims);
  VideoImagerTool->SetFrameDimensions(dims);

  VideoImagerTool->SetPixelDepth(toolConfiguration->GetPixelDepth());
  VideoImagerTool->RequestSetVideoImagerToolName(
                            toolConfiguration->GetToolUniqueIdentifier() );
  VideoImagerTool->RequestConfigure();

  // VideoImagerTool->SetCalibrationTransform( identity );

  return VideoImagerTool;
}
//ImagingSource framegrabber specific :end

//CompressedDV framegrabber specific :begin
CompressedDVVideoImagerTool::Pointer VideoImagerController::InitializeCompressedDVTool(
    const CompressedDVToolConfiguration *toolConfiguration )
{
  CompressedDVVideoImagerTool::Pointer VideoImagerTool = CompressedDVVideoImagerTool::New();

  unsigned int dims[3];
  toolConfiguration->GetFrameDimensions(dims);
  VideoImagerTool->SetFrameDimensions(dims);

  VideoImagerTool->SetPixelDepth(toolConfiguration->GetPixelDepth());
  VideoImagerTool->RequestSetVideoImagerToolName(
                            toolConfiguration->GetToolUniqueIdentifier() );
  VideoImagerTool->RequestConfigure();

  // VideoImagerTool->SetCalibrationTransform( identity );

  return VideoImagerTool;
}
//CompressedDV framegrabber specific :end

#endif

#if defined(WIN32) || defined(_WIN32)
#ifdef IGSTKSandbox_USE_OpenIGTLink

//Terason Ultrasound specific :begin
void VideoImagerController::TerasonInitializeProcessing()
{
  if( !InitializeSocketCommunication() )
  {
    igstkPushInputMacro( Failed );
  }
  else
  {
      //create VideoImager
      igstk::TerasonVideoImager::Pointer VideoImager = igstk::TerasonVideoImager::New();
      this->m_VideoImager = VideoImager;

      if (this->m_ProgressCallback != NULL)
        VideoImager->AddObserver(igstk::DoubleTypeEvent(), this->m_ProgressCallback);

      //don't need to observe this for errors because the
      //configuration class ensures that the frequency is valid
      VideoImager->RequestSetFrequency( this->m_VideoImagerConfiguration->GetFrequency() );

      VideoImager->SetCommunication( this->m_SocketCommunication );

      TerasonVideoImagerConfiguration *VideoImagerConfiguration =
        dynamic_cast<TerasonVideoImagerConfiguration *>( this->m_VideoImagerConfiguration );

      unsigned long observerID = VideoImager->AddObserver( IGSTKErrorEvent(),
                                                       this->m_ErrorObserver );
      VideoImager->RequestOpen();

      if( this->m_ErrorObserver->ErrorOccured() )
      {
        this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
        this->m_ErrorObserver->ClearError();
        VideoImager->RemoveObserver(observerID);
        igstkPushInputMacro( Failed );
      }
      else   //attach the tools and start communication
      {
        std::vector< VideoImagerToolConfiguration * > toolConfigurations =
            this->m_VideoImagerConfiguration->GetVideoImagerToolList();
        //attach tools
        std::vector< VideoImagerToolConfiguration * >::const_iterator it;
        std::vector< VideoImagerToolConfiguration * >::const_iterator toolConfigEnd =
          toolConfigurations.end();
        VideoImagerTool::Pointer VideoImagerTool;
        TerasonToolConfiguration * currentToolConfig;

        for( it = toolConfigurations.begin(); it!=toolConfigEnd; it++ )
        {
          currentToolConfig = static_cast<TerasonToolConfiguration *>(*it);

          VideoImagerTool = this->InitializeTerasonTool( currentToolConfig );
          this->m_Tools.push_back( VideoImagerTool );
          VideoImagerTool->RequestAttachToVideoImager( VideoImager );
        }

        m_VideoImager->RemoveObserver(observerID);
        igstkPushInputMacro( Succeeded );
      }
  }
  this->m_StateMachine.ProcessInputs();
}
//Terason Ultrasound specific :end
#endif

//WebcamWin framegrabber specific :begin
void VideoImagerController::WebcamWinInitializeProcessing()
{
  //create VideoImager
  igstk::WebcamWinVideoImager::Pointer VideoImager = igstk::WebcamWinVideoImager::New();
  this->m_VideoImager = VideoImager;
  //don't need to observe this for errors because the
  //configuration class ensures that the frequency is valid
  VideoImager->RequestSetFrequency( this->m_VideoImagerConfiguration->GetFrequency() );

  WebcamWinVideoImagerConfiguration *VideoImagerConfiguration =
  dynamic_cast<WebcamWinVideoImagerConfiguration *>( this->m_VideoImagerConfiguration );

  unsigned long observerID = VideoImager->AddObserver( IGSTKErrorEvent(),
                                                   this->m_ErrorObserver );
  VideoImager->RequestOpen();
//TODO modify
  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    VideoImager->RemoveObserver(observerID);
    igstkPushInputMacro( Failed );
  }
  else   //attach the tools and start communication
  {
    std::vector< VideoImagerToolConfiguration * > toolConfigurations =
        this->m_VideoImagerConfiguration->GetVideoImagerToolList();
    //attach tools
    std::vector< VideoImagerToolConfiguration * >::const_iterator it;
    std::vector< VideoImagerToolConfiguration * >::const_iterator toolConfigEnd =
          toolConfigurations.end();
    VideoImagerTool::Pointer VideoImagerTool;
    TerasonToolConfiguration * currentToolConfig;

    for( it = toolConfigurations.begin(); it!=toolConfigEnd; it++ )
    {
      currentToolConfig = static_cast<TerasonToolConfiguration *>(*it);

      VideoImagerTool = this->InitializeTerasonTool( currentToolConfig );
      this->m_Tools.push_back( VideoImagerTool );
      VideoImagerTool->RequestAttachToVideoImager( VideoImager );
    }

    m_VideoImager->RemoveObserver(observerID);
    igstkPushInputMacro( Succeeded );
  }
  this->m_StateMachine.ProcessInputs();
}
//WebcamWin framegrabber specific :end

#else

//ImagingSource framegrabber specific :begin
void VideoImagerController::ImagingSourceInitializeProcessing()
{
  //create VideoImager
  igstk::ImagingSourceVideoImager::Pointer VideoImager = igstk::ImagingSourceVideoImager::New();
  this->m_VideoImager = VideoImager;
  //don't need to observe this for errors because the
  //configuration class ensures that the frequency is valid
  VideoImager->RequestSetFrequency( this->m_VideoImagerConfiguration->GetFrequency() );

  ImagingSourceVideoImagerConfiguration *VideoImagerConfiguration =
  dynamic_cast<ImagingSourceVideoImagerConfiguration *>( this->m_VideoImagerConfiguration );

  unsigned long observerID = VideoImager->AddObserver( IGSTKErrorEvent(),
                                                   this->m_ErrorObserver );
  VideoImager->RequestOpen();

  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    VideoImager->RemoveObserver(observerID);
    igstkPushInputMacro( Failed );
  }
  else   //attach the tools and start communication
  {
    std::vector< VideoImagerToolConfiguration * > toolConfigurations =
        this->m_VideoImagerConfiguration->GetVideoImagerToolList();
    //attach tools
    std::vector< VideoImagerToolConfiguration * >::const_iterator it;
    std::vector< VideoImagerToolConfiguration * >::const_iterator toolConfigEnd =
          toolConfigurations.end();
    VideoImagerTool::Pointer VideoImagerTool;
    TerasonToolConfiguration * currentToolConfig;

    for( it = toolConfigurations.begin(); it!=toolConfigEnd; it++ )
    {
      currentToolConfig = static_cast<TerasonToolConfiguration *>(*it);

      VideoImagerTool = this->InitializeTerasonTool( currentToolConfig );
      this->m_Tools.push_back( VideoImagerTool );
      VideoImagerTool->RequestAttachToVideoImager( VideoImager );
    }

    m_VideoImager->RemoveObserver(observerID);
    igstkPushInputMacro( Succeeded );
  }
  this->m_StateMachine.ProcessInputs();
}
//ImagingSource framegrabber specific :end

//CompressedDV framegrabber specific :begin
void VideoImagerController::CompressedDVInitializeProcessing()
{
  //create VideoImager
  igstk::CompressedDVVideoImager::Pointer VideoImager = igstk::CompressedDVVideoImager::New();
  this->m_VideoImager = VideoImager;
  //don't need to observe this for errors because the
  //configuration class ensures that the frequency is valid
  VideoImager->RequestSetFrequency( this->m_VideoImagerConfiguration->GetFrequency() );

  CompressedDVVideoImagerConfiguration *VideoImagerConfiguration =
  dynamic_cast<CompressedDVVideoImagerConfiguration *>( this->m_VideoImagerConfiguration );

  unsigned long observerID = VideoImager->AddObserver( IGSTKErrorEvent(),
                                                   this->m_ErrorObserver );
  VideoImager->RequestOpen();
//TODO modify
  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    VideoImager->RemoveObserver(observerID);
    igstkPushInputMacro( Failed );
  }
  else   //attach the tools and start communication
  {
    std::vector< VideoImagerToolConfiguration * > toolConfigurations =
        this->m_VideoImagerConfiguration->GetVideoImagerToolList();
    //attach tools
    std::vector< VideoImagerToolConfiguration * >::const_iterator it;
    std::vector< VideoImagerToolConfiguration * >::const_iterator toolConfigEnd =
          toolConfigurations.end();
    VideoImagerTool::Pointer VideoImagerTool;
    TerasonToolConfiguration * currentToolConfig;

    for( it = toolConfigurations.begin(); it!=toolConfigEnd; it++ )
    {
      currentToolConfig = static_cast<TerasonToolConfiguration *>(*it);

      VideoImagerTool = this->InitializeTerasonTool( currentToolConfig );
      this->m_Tools.push_back( VideoImagerTool );
      VideoImagerTool->RequestAttachToVideoImager( VideoImager );
    }

    m_VideoImager->RemoveObserver(observerID);
    igstkPushInputMacro( Succeeded );
  }
  this->m_StateMachine.ProcessInputs();
}
//CompressedDV framegrabber specific :end

#endif

void
VideoImagerController::GetVideoImagerProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::VideoImagerController::"
                  "GetVideoImagerProcessing called...\n");
  RequestVideoImagerEvent evt;
  evt.Set( this->m_VideoImager );
  this->InvokeEvent( evt );
}


void
VideoImagerController::GetToolsProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::VideoImagerController::"
                  "GetToolsProcessing called...\n");
  RequestToolsEvent evt;
  evt.Set( this->m_Tools );
  this->InvokeEvent( evt );
}


void
VideoImagerController::ReportInitializationSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::VideoImagerController::"
                  "ReportInitializationSuccess called...\n");
  this->InvokeEvent( InitializeSuccessEvent() );
}
void
VideoImagerController::ReportInitializationFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::VideoImagerController::"
                  "ReportInitializationFailureProcessing called...\n");
  InitializeFailureEvent evt;
  evt.Set( this->m_ErrorMessage );
  this->InvokeEvent( evt );

}
void
VideoImagerController::ReportShutdownSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::VideoImagerController::"
                  "ReportShutdownSuccess called...\n");
  this->InvokeEvent( ShutdownSuccessEvent() );
}
void
VideoImagerController::ReportShutdownFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::VideoImagerController::"
                  "ReportShutdownFailure called...\n");
  ShutdownFailureEvent evt;
  evt.Set( this->m_ErrorMessage );
  this->InvokeEvent( evt );
}
void
VideoImagerController::ReportStartSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::VideoImagerController::"
                  "ReportStartSuccess called...\n");
  this->InvokeEvent( StartSuccessEvent() );
}
void
VideoImagerController::ReportStartFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::VideoImagerController::"
                  "ReportStartFailure called...\n");
  StartFailureEvent evt;
  evt.Set( this->m_ErrorMessage );
  this->InvokeEvent( evt );
}
void
VideoImagerController::ReportStopSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::VideoImagerController::"
                  "ReportStopSuccess called...\n");
  this->InvokeEvent( StopSuccessEvent() );
}
void
VideoImagerController::ReportStopFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::VideoImagerController::"
                  "ReportStopFailure called...\n");
  StopFailureEvent evt;
  evt.Set( this->m_ErrorMessage );
  this->InvokeEvent( evt );
}
void
VideoImagerController::ReportInvalidRequestProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VideoImagerController::"
                 "ReportInvalidRequestProcessing called...\n");
  this->InvokeEvent(InvalidRequestErrorEvent());
}


VideoImagerController::ErrorObserver::ErrorObserver() : m_ErrorOccured(false)
{
  //VideoImager errors
  this->m_ErrorEvent2ErrorMessage.insert(
    std::pair<std::string,std::string>( VideoImagerOpenErrorEvent().GetEventName(),
                                        "Error opening VideoImager communication." ) );
  this->m_ErrorEvent2ErrorMessage.insert(
    std::pair<std::string,std::string>( VideoImagerInitializeErrorEvent().GetEventName(),
                                        "Error initializing VideoImager." ) );
  this->m_ErrorEvent2ErrorMessage.insert(
    std::pair<std::string,std::string>( VideoImagerStartImagingErrorEvent().GetEventName(),
                                        "Error starting imaging." ) );
  this->m_ErrorEvent2ErrorMessage.insert(
    std::pair<std::string,std::string>( VideoImagerStopImagingErrorEvent().GetEventName(),
                                        "Error stopping imaging." ) );
  this->m_ErrorEvent2ErrorMessage.insert(
    std::pair<std::string,std::string>( VideoImagerCloseErrorEvent().GetEventName(),
                                        "Error closing VideoImager communication." ) );
}

void
VideoImagerController::ErrorObserver::Execute(itk::Object *caller,
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
VideoImagerController::ErrorObserver::Execute(const itk::Object *caller,
                                            const itk::EventObject & event) throw (std::exception)
{
  const itk::Object * constCaller = caller;
  this->Execute(constCaller, event);
}

}//end namespace igstk
