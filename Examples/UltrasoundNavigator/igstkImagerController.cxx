/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImagerController.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkImagerController.h"

#include "igstkImagerConfiguration.h"

#include "itksys/SystemTools.hxx"
#include "itksys/Directory.hxx"

#include "igstkTerasonImager.h"

//#include "igstkCalibrationIO.h"

namespace igstk
{ 



ImagerController::ImagerController() : m_StateMachine( this )
{
            //create error observer
  this->m_ErrorObserver = ErrorObserver::New();

        //define the state machine's states 
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( AttemptingToInitialize );
  igstkAddStateMacro( AttemptingToInitializeTerason );

  igstkAddStateMacro( AttemptingToShutdown );
  igstkAddStateMacro( Initialized );

  igstkAddStateMacro( AttemptingToStart );
  igstkAddStateMacro( AttemptingToStop );

  igstkAddStateMacro( Started );  

                   //define the state machine's inputs
  igstkAddInputMacro( ImagerInitialize );
  igstkAddInputMacro( ImagerStart );
  igstkAddInputMacro( ImagerStop );
  igstkAddInputMacro( ImagerShutdown );
  igstkAddInputMacro( TerasonInitialize );
  igstkAddInputMacro( Failed  );
  igstkAddInputMacro( Succeeded  );
  igstkAddInputMacro( GetImager  );
  igstkAddInputMacro( GetTools  );

            //define the state machine's transitions

                         //transitions from Idle state
  igstkAddTransitionMacro(Idle,
                          ImagerInitialize,
                          AttemptingToInitialize,
                          ImagerInitialize);

  igstkAddTransitionMacro(Idle,
                          ImagerShutdown,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(Idle,
                          TerasonInitialize,
                          Idle,
                          ReportInvalidRequest);
    
  igstkAddTransitionMacro(Idle,
                          Failed,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Idle,
                          Succeeded,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(Idle,
                          GetImager,
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

  igstkAddTransitionMacro(AttemptingToInitialize,
                          TerasonInitialize,
                          AttemptingToInitializeTerason,
                          TerasonInitialize);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          ImagerInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          ImagerShutdown,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          Succeeded,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          GetImager,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitialize,
                          GetTools,
                          Idle,
                          ReportInvalidRequest);
  
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
                          ImagerInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializeTerason,
                          ImagerShutdown,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializeTerason,
                          TerasonInitialize,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializeTerason,
                          GetImager,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializeTerason,
                          GetTools,
                          Idle,
                          ReportInvalidRequest);
          
          //transitions from Initialized state
  igstkAddTransitionMacro(Initialized,
                          GetImager,
                          Initialized,
                          GetImager);

  igstkAddTransitionMacro(Initialized,
                          GetTools,
                          Initialized,
                          GetTools);

  igstkAddTransitionMacro(Initialized,
                          ImagerInitialize,
                          AttemptingToInitialize,
                          ImagerInitialize);

  igstkAddTransitionMacro(Initialized,
                          ImagerShutdown,
                          AttemptingToShutdown,
                          ImagerShutdown);

  igstkAddTransitionMacro(Initialized,
                          ImagerStart,
                          AttemptingToStart,
                          ImagerStart);
  
  igstkAddTransitionMacro(Initialized,
                          TerasonInitialize,
                          Initialized,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(Initialized,
                          ImagerStop,
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
                          GetImager,
                          AttemptingToStart,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStart,
                          GetTools,
                          AttemptingToStart,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStart,
                          ImagerInitialize,
                          AttemptingToStart,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStart,
                          ImagerShutdown,
                          AttemptingToStart,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStart,
                          ImagerStart,
                          AttemptingToStart,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToStart,
                          TerasonInitialize,
                          AttemptingToStart,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStart,
                          ImagerStop,
                          AttemptingToStart,
                          ReportInvalidRequest);

        //transitions from Started state

  igstkAddTransitionMacro(Started,
                          ImagerStop,
                          AttemptingToStop,
                          ImagerStop);
  
  igstkAddTransitionMacro(Started,
                          TerasonInitialize,
                          Started,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Started,
                          ImagerInitialize,
                          Started,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Started,
                          ImagerStart,
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
                          GetImager,
                          AttemptingToStop,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStop,
                          GetTools,
                          AttemptingToStop,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStop,
                          ImagerInitialize,
                          AttemptingToStop,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStop,
                          ImagerShutdown,
                          AttemptingToStop,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStop,
                          ImagerStart,
                          AttemptingToStop,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToStop,
                          TerasonInitialize,
                          AttemptingToStop,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStop,
                          ImagerStop,
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
                          ImagerInitialize,
                          AttemptingToShutdown,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToShutdown,
                          TerasonInitialize,
                          AttemptingToShutdown,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToShutdown,
                          ImagerShutdown,
                          AttemptingToShutdown,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToShutdown,
                          GetImager,
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


ImagerController::~ImagerController()  
{
     
}

void
ImagerController::RequestInitialize(
  const ImagerConfiguration *configuration)
{
  igstkLogMacro( DEBUG, 
                 "igstkImagerController::RequestInitialize called...\n" );
  this->m_TmpImagerConfiguration = 
    const_cast<ImagerConfiguration *>(configuration);
  igstkPushInputMacro( ImagerInitialize );
  this->m_StateMachine.ProcessInputs();
}

void
ImagerController::RequestStart()
{
  igstkLogMacro( DEBUG, 
                 "igstkImagerController::RequestStart called...\n" );
  igstkPushInputMacro( ImagerStart );
  this->m_StateMachine.ProcessInputs();
}

void
ImagerController::RequestStop()
{
  igstkLogMacro( DEBUG, 
                 "igstkImagerController::RequestStop called...\n" );
  igstkPushInputMacro( ImagerStop );
  this->m_StateMachine.ProcessInputs();
}

void
ImagerController::RequestShutdown()
{
  igstkLogMacro( DEBUG, 
                 "igstkImagerController::RequestShutdown called...\n" );
  igstkPushInputMacro( ImagerShutdown );
  this->m_StateMachine.ProcessInputs();
}

void 
ImagerController::RequestGetImager()
{
  igstkLogMacro( DEBUG, 
                 "igstkImagerController::RequestGetImager called...\n" );
  igstkPushInputMacro( GetImager );
  this->m_StateMachine.ProcessInputs();
}

void 
ImagerController::RequestGetToolList()
{
  igstkLogMacro( DEBUG, 
                 "igstkImagerController::RequestGetNonReferenceToolList called...\n" );
  igstkPushInputMacro( GetTools );
  this->m_StateMachine.ProcessInputs();
}

void 
ImagerController::ImagerInitializeProcessing()
{
  if( this->m_TmpImagerConfiguration == NULL )
  {
    this->m_ErrorMessage = "Null imager configuration received.";
    igstkPushInputMacro( Failed );
  }
  else 
  {
   
    if( dynamic_cast<TerasonImagerConfiguration *>
      ( this->m_TmpImagerConfiguration ) )
    {
      this->m_ImagerConfiguration = m_TmpImagerConfiguration;
      igstkPushInputMacro( TerasonInitialize );
    }
    else
    {
      this->m_ErrorMessage = "Unknown imager configuration type.";
      igstkPushInputMacro( Failed );
    }
  }
 this->m_StateMachine.ProcessInputs();
}

void 
ImagerController::ImagerStartProcessing()
{  
    StartFailureEvent evt;
    unsigned long observerID;
    observerID = this->m_Imager->AddObserver( IGSTKErrorEvent(), 
                                             this->m_ErrorObserver );

    m_Imager->RequestStartImaging();

    if( this->m_ErrorObserver->ErrorOccured() )
    {
      this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
      this->m_ErrorObserver->ClearError();
      m_Imager->RemoveObserver( observerID );
      //m_Imager->RequestClose();
      evt.Set( this->m_ErrorMessage );
      this->InvokeEvent( evt );
      igstkPushInputMacro( Failed );
    }
    else
    {
      m_Imager->RemoveObserver(observerID);
      igstkPushInputMacro( Succeeded );
    }

    this->m_StateMachine.ProcessInputs();
}

void 
ImagerController::ImagerStopProcessing()
{
  StopFailureEvent evt;
  unsigned long observerID;

  observerID = this->m_Imager->AddObserver( IGSTKErrorEvent(), 
                                             this->m_ErrorObserver );
                //stop tracking
  this->m_Imager->RequestStopImaging();
  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    this->m_Imager->RemoveObserver(observerID);
    evt.Set( this->m_ErrorMessage );
    this->InvokeEvent( evt );
    igstkPushInputMacro( Failed );
  }
  else
  {
    this->m_Imager->RemoveObserver(observerID);
    igstkPushInputMacro( Succeeded );
  }

  this->m_StateMachine.ProcessInputs();
}

void 
ImagerController::ImagerShutdownProcessing()
{
  ShutdownFailureEvent evt;
  unsigned long observerID;

  observerID = this->m_Imager->AddObserver( IGSTKErrorEvent(), 
                                             this->m_ErrorObserver );

                   //close communication with imager
  this->m_Imager->RequestClose();
  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    this->m_Imager->RemoveObserver(observerID);
    evt.Set( this->m_ErrorMessage );
    this->InvokeEvent( evt );
    igstkPushInputMacro( Failed );
  }
  else
  {
   this->m_Imager->RemoveObserver( observerID );

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
   this->m_Imager = NULL;   
   this->m_Tools.clear();
   this->m_SocketCommunication = NULL;
   igstkPushInputMacro( Succeeded );
  }

  this->m_StateMachine.ProcessInputs();
}

bool 
ImagerController::InitializeSocketCommunication()
{
  SocketCommunicatingImagerConfiguration *socketImagerConfiguration =
    dynamic_cast<SocketCommunicatingImagerConfiguration *>( this->m_ImagerConfiguration );
  
                 //create client socket communication
  this->m_SocketCommunication = igtl::ServerSocket::New();
  //this->m_SocketCommunication = igtl::ClientSocket::New();

 // std::cout << "socketImagerConfiguration->GetSocketPort() " << socketImagerConfiguration->GetSocketPort() << std::endl;
//  std::cout << "socketImagerConfiguration->GetHostName() " << socketImagerConfiguration->GetHostName() << std::endl;

  int err = this->m_SocketCommunication->CreateServer( socketImagerConfiguration->GetSocketPort() );

//  int err = this->m_SocketCommunication->ConnectToServer( 
//  socketImagerConfiguration->GetHostName().c_str(), socketImagerConfiguration->GetSocketPort() );

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

TerasonImagerTool::Pointer ImagerController::InitializeTerasonTool(
    const TerasonToolConfiguration *toolConfiguration )
{
  TerasonImagerTool::Pointer imagerTool = TerasonImagerTool::New();
 
  unsigned int dims[3];
  toolConfiguration->GetFrameDimensions(dims);
  imagerTool->SetFrameDimensions(dims);

  imagerTool->SetPixelDepth(toolConfiguration->GetPixelDepth());
  imagerTool->RequestSetImagerToolName( toolConfiguration->GetToolUniqueIdentifier() );
  imagerTool->RequestConfigure();

 // imagerTool->SetCalibrationTransform( identity );

  imagerTool->RequestConfigure();

  return imagerTool;
}

void ImagerController::TerasonInitializeProcessing()
{
  if( !InitializeSocketCommunication() )
  {
    igstkPushInputMacro( Failed );
  }
  else
  {
                      //create imager
      igstk::TerasonImager::Pointer imager = igstk::TerasonImager::New();
      this->m_Imager = imager; 
                     //don't need to observe this for errors because the 
                     //configuration class ensures that the frequency is valid
      imager->RequestSetFrequency( this->m_ImagerConfiguration->GetFrequency() );

      imager->SetCommunication( this->m_SocketCommunication );

      TerasonImagerConfiguration *imagerConfiguration =
        dynamic_cast<TerasonImagerConfiguration *>( this->m_ImagerConfiguration );
      
    //  imager->SetCameraCalibrationFilesDirectory( 
    //    imagerConfiguration->GetCameraCalibrationFileDirectory() );

    //  imager->SetInitializationFile(
    //    imagerConfiguration->GetInitializationFile() );
        
    //  imager->SetMarkerTemplatesDirectory( 
    //    imagerConfiguration->GetTemplatesDirectory() );
        
      unsigned long observerID = imager->AddObserver( IGSTKErrorEvent(),
                                                       this->m_ErrorObserver );
      imager->RequestOpen();
      
      if( this->m_ErrorObserver->ErrorOccured() )
      {
        this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
        this->m_ErrorObserver->ClearError();
        imager->RemoveObserver(observerID);    
        igstkPushInputMacro( Failed );
      }
      else   //attach the tools and start communication 
      {
        std::vector< ImagerToolConfiguration * > toolConfigurations = 
            this->m_ImagerConfiguration->GetImagerToolList();
                              //attach tools
        std::vector< ImagerToolConfiguration * >::const_iterator it;
        std::vector< ImagerToolConfiguration * >::const_iterator toolConfigEnd =
          toolConfigurations.end();
        ImagerTool::Pointer imagerTool;
        TerasonToolConfiguration * currentToolConfig;

        for(it = toolConfigurations.begin(); it!=toolConfigEnd; it++)
        {
          currentToolConfig = static_cast<TerasonToolConfiguration *>(*it);

          imagerTool = this->InitializeTerasonTool( currentToolConfig );
          this->m_Tools.push_back( imagerTool );
          imagerTool->RequestAttachToImager( imager );
        }   

        m_Imager->RemoveObserver(observerID);
        igstkPushInputMacro( Succeeded );
      }
  }
  this->m_StateMachine.ProcessInputs();

}

void 
ImagerController::GetImagerProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::ImagerController::"
                  "GetImagerProcessing called...\n");
  RequestImagerEvent evt;
  evt.Set( this->m_Imager );
  this->InvokeEvent( evt );
}


void 
ImagerController::GetToolsProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::ImagerController::"
                  "GetToolsProcessing called...\n");
  RequestToolsEvent evt;
  evt.Set( this->m_Tools );
  this->InvokeEvent( evt );
}


void 
ImagerController::ReportInitializationSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::ImagerController::"
                  "ReportInitializationSuccess called...\n");
  this->InvokeEvent( InitializeSuccessEvent() );
}
void
ImagerController::ReportInitializationFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::ImagerController::"
                  "ReportInitializationFailureProcessing called...\n");
  InitializeFailureEvent evt; 
  evt.Set( this->m_ErrorMessage );
  this->InvokeEvent( evt );

}
void 
ImagerController::ReportShutdownSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::ImagerController::"
                  "ReportShutdownSuccess called...\n");
  this->InvokeEvent( ShutdownSuccessEvent() );
}
void 
ImagerController::ReportShutdownFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::ImagerController::"
                  "ReportShutdownFailure called...\n");
  ShutdownFailureEvent evt; 
  evt.Set( this->m_ErrorMessage );
  this->InvokeEvent( evt );
}
void 
ImagerController::ReportStartSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::ImagerController::"
                  "ReportStartSuccess called...\n");
  this->InvokeEvent( StartSuccessEvent() );
}
void 
ImagerController::ReportStartFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::ImagerController::"
                  "ReportStartFailure called...\n");
  StartFailureEvent evt; 
  evt.Set( this->m_ErrorMessage );
  this->InvokeEvent( evt );
}
void 
ImagerController::ReportStopSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::ImagerController::"
                  "ReportStopSuccess called...\n");
  this->InvokeEvent( StopSuccessEvent() );
}
void 
ImagerController::ReportStopFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::ImagerController::"
                  "ReportStopFailure called...\n");
  StopFailureEvent evt; 
  evt.Set( this->m_ErrorMessage );
  this->InvokeEvent( evt );
}
void 
ImagerController::ReportInvalidRequestProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ImagerController::"
                 "ReportInvalidRequestProcessing called...\n");
  this->InvokeEvent(InvalidRequestErrorEvent());
}


ImagerController::ErrorObserver::ErrorObserver() : m_ErrorOccured(false)
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
ImagerController::ErrorObserver::Execute(itk::Object *caller, 
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
ImagerController::ErrorObserver::Execute(const itk::Object *caller, 
                                            const itk::EventObject & event) throw (std::exception)
{
  const itk::Object * constCaller = caller;
  this->Execute(constCaller, event);
}

}//end namespace igstk
