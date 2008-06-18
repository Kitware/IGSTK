/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerController.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTrackerController.h"

#include "igstkTrackerConfiguration.h"

#include "itksys/SystemTools.hxx"
#include "itksys/Directory.hxx"

#include "igstkPolarisTracker.h"
#include "igstkAuroraTracker.h"

#ifdef IGSTKSandbox_USE_MicronTracker
#include "igstkMicronTracker.h"
#endif

#include "igstkFlockOfBirdsTrackerNew.h"

#include "igstkCalibrationIO.h"


namespace igstk
{ 



TrackerController::TrackerController() : m_StateMachine( this )
{
            //create error observer
  this->m_ErrorObserver = ErrorObserver::New();

        //define the state machine's states 
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( AttemptingToInitialize );
  igstkAddStateMacro( AttemptingToInitializePolarisVicra );
  igstkAddStateMacro( AttemptingToInitializePolarisHybrid );
  igstkAddStateMacro( AttemptingToInitializeAurora );
  igstkAddStateMacro( AttemptingToInitializeMicron );
  igstkAddStateMacro( AttemptingToInitializeAscension );  

  igstkAddStateMacro( AttemptingToShutdown );
  igstkAddStateMacro( Initialized );

  igstkAddStateMacro( AttemptingToStart );
  igstkAddStateMacro( AttemptingToStop );

  igstkAddStateMacro( Started );  

                   //define the state machine's inputs
  igstkAddInputMacro( TrackerInitialize );
  igstkAddInputMacro( TrackerStart );
  igstkAddInputMacro( TrackerStop );
  igstkAddInputMacro( TrackerShutdown );
  igstkAddInputMacro( PolarisVicraInitialize );
  igstkAddInputMacro( PolarisHybridInitialize );
  igstkAddInputMacro( AuroraInitialize );
  igstkAddInputMacro( MicronInitialize );
  igstkAddInputMacro( MicronStart );
  igstkAddInputMacro( MicronStop );
  igstkAddInputMacro( AscensionInitialize );
  igstkAddInputMacro( Failed  );
  igstkAddInputMacro( Succeeded  );
  igstkAddInputMacro( GetTracker  );
  igstkAddInputMacro( GetTools  );
  igstkAddInputMacro( GetReferenceTool  );

            //define the state machine's transitions

                         //transitions from Idle state
  igstkAddTransitionMacro(Idle,
                          TrackerInitialize,
                          AttemptingToInitialize,
                          TrackerInitialize);

  igstkAddTransitionMacro(Idle,
                          TrackerShutdown,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Idle,
                          PolarisVicraInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Idle,
                          PolarisHybridInitialize,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(Idle,
                          AuroraInitialize,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(Idle,
                          MicronInitialize,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(Idle,
                          AscensionInitialize,
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
                          GetTracker,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(Idle,
                          GetTools,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Idle,
                          GetReferenceTool,
                          Idle,
                          ReportInvalidRequest);

                  //transitions from AttemptingToInitialize state
  igstkAddTransitionMacro(AttemptingToInitialize,
                          Failed,
                          Idle,
                          ReportInitializationFailure);                          

  igstkAddTransitionMacro(AttemptingToInitialize,
                          PolarisVicraInitialize,
                          AttemptingToInitializePolarisVicra,
                          PolarisVicraInitialize);
  
  igstkAddTransitionMacro(AttemptingToInitialize,
                          PolarisHybridInitialize,
                          AttemptingToInitializePolarisHybrid,
                          PolarisHybridInitialize);
 
  igstkAddTransitionMacro(AttemptingToInitialize,
                          AuroraInitialize,
                          AttemptingToInitializeAurora,
                          AuroraInitialize);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          MicronInitialize,
                          AttemptingToInitializeMicron,
                          MicronInitialize);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          AscensionInitialize,
                          AttemptingToInitializeAscension,
                          AscensionInitialize);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          TrackerInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          TrackerShutdown,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          Succeeded,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          GetTracker,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitialize,
                          GetTools,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitialize,
                          GetReferenceTool,
                          Idle,
                          ReportInvalidRequest);

           //transitions from AttemptingToInitializePolarisVicra state
  igstkAddTransitionMacro(AttemptingToInitializePolarisVicra,
                          Failed,
                          Idle,
                          ReportInitializationFailure);
  
  igstkAddTransitionMacro(AttemptingToInitializePolarisVicra,
                          Succeeded,
                          Initialized,
                          ReportInitializationSuccess);

  igstkAddTransitionMacro(AttemptingToInitializePolarisVicra,
                          TrackerInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializePolarisVicra,
                          TrackerShutdown,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializePolarisVicra,
                          PolarisVicraInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializePolarisVicra,
                          PolarisHybridInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializePolarisVicra,
                          AuroraInitialize,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializePolarisVicra,
                          MicronInitialize,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializePolarisVicra,
                          AscensionInitialize,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializePolarisVicra,
                          GetTracker,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializePolarisVicra,
                          GetTools,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializePolarisVicra,
                          GetReferenceTool,
                          Idle,
                          ReportInvalidRequest);

           //transitions from AttemptingToInitializePolarisHybrid state
  igstkAddTransitionMacro(AttemptingToInitializePolarisHybrid,
                          Failed,
                          Idle,
                          ReportInitializationFailure);
  
  igstkAddTransitionMacro(AttemptingToInitializePolarisHybrid,
                          Succeeded,
                          Initialized,
                          ReportInitializationSuccess);

  igstkAddTransitionMacro(AttemptingToInitializePolarisHybrid,
                          TrackerInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializePolarisHybrid,
                          TrackerShutdown,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializePolarisHybrid,
                          PolarisVicraInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializePolarisHybrid,
                          PolarisHybridInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializePolarisHybrid,
                          AuroraInitialize,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializePolarisHybrid,
                          MicronInitialize,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializePolarisHybrid,
                          AscensionInitialize,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializePolarisHybrid,
                          GetTracker,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializePolarisHybrid,
                          GetTools,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializePolarisHybrid,
                          GetReferenceTool,
                          Idle,
                          ReportInvalidRequest);

          //transitions from AttemptingToInitializeAurora state
  igstkAddTransitionMacro(AttemptingToInitializeAurora,
                          Failed,
                          Idle,
                          ReportInitializationFailure);
  
  igstkAddTransitionMacro(AttemptingToInitializeAurora,
                          Succeeded,
                          Initialized,
                          ReportInitializationSuccess);

  igstkAddTransitionMacro(AttemptingToInitializeAurora,
                          TrackerInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializeAurora,
                          TrackerShutdown,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializeAurora,
                          PolarisVicraInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializeAurora,
                          PolarisHybridInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializeAurora,
                          AuroraInitialize,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializeAurora,
                          MicronInitialize,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializeAurora,
                          AscensionInitialize,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializeAurora,
                          GetTracker,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializeAurora,
                          GetTools,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializeAurora,
                          GetReferenceTool,
                          Idle,
                          ReportInvalidRequest);

          //transitions from AttemptingToInitializeMicron state
  igstkAddTransitionMacro(AttemptingToInitializeMicron,
                          Failed,
                          Idle,
                          ReportInitializationFailure);
  
  igstkAddTransitionMacro(AttemptingToInitializeMicron,
                          Succeeded,
                          Initialized,
                          ReportInitializationSuccess);

  igstkAddTransitionMacro(AttemptingToInitializeMicron,
                          TrackerInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializeMicron,
                          TrackerShutdown,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializeMicron,
                          PolarisVicraInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializeMicron,
                          PolarisHybridInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializeMicron,
                          AuroraInitialize,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializeMicron,
                          MicronInitialize,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializeMicron,
                          AscensionInitialize,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializeMicron,
                          GetTracker,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializeMicron,
                          GetTools,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializeMicron,
                          GetReferenceTool,
                          Idle,
                          ReportInvalidRequest);

          //transitions from AttemptingToInitializeAscension state
  igstkAddTransitionMacro(AttemptingToInitializeAscension,
                          Failed,
                          Idle,
                          ReportInitializationFailure);
  
  igstkAddTransitionMacro(AttemptingToInitializeAscension,
                          Succeeded,
                          Initialized,
                          ReportInitializationSuccess);

  igstkAddTransitionMacro(AttemptingToInitializeAscension,
                          TrackerInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializeAscension,
                          TrackerShutdown,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializeAscension,
                          PolarisVicraInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializeAscension,
                          PolarisHybridInitialize,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitializeAscension,
                          AuroraInitialize,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializeAscension,
                          MicronInitialize,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializeAscension,
                          AscensionInitialize,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializeAscension,
                          GetTracker,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializeAscension,
                          GetTools,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToInitializeAscension,
                          GetReferenceTool,
                          Idle,
                          ReportInvalidRequest);
          
          //transitions from Initialized state
  igstkAddTransitionMacro(Initialized,
                          GetTracker,
                          Initialized,
                          GetTracker);

  igstkAddTransitionMacro(Initialized,
                          GetTools,
                          Initialized,
                          GetTools);

  igstkAddTransitionMacro(Initialized,
                          GetReferenceTool,
                          Initialized,
                          GetReferenceTool);

  igstkAddTransitionMacro(Initialized,
                          TrackerInitialize,
                          AttemptingToInitialize,
                          TrackerInitialize);

  igstkAddTransitionMacro(Initialized,
                          TrackerShutdown,
                          AttemptingToShutdown,
                          TrackerShutdown);

  igstkAddTransitionMacro(Initialized,
                          TrackerStart,
                          AttemptingToStart,
                          TrackerStart);

  igstkAddTransitionMacro(Initialized,
                          PolarisVicraInitialize,
                          Initialized,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Initialized,
                          PolarisHybridInitialize,
                          Initialized,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(Initialized,
                          AuroraInitialize,
                          Initialized,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(Initialized,
                          MicronInitialize,
                          Initialized,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(Initialized,
                          AscensionInitialize,
                          Initialized,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Initialized,
                          TrackerStop,
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
                          GetTracker,
                          AttemptingToStart,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStart,
                          GetTools,
                          AttemptingToStart,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStart,
                          GetReferenceTool,
                          AttemptingToStart,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStart,
                          TrackerInitialize,
                          AttemptingToStart,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStart,
                          TrackerShutdown,
                          AttemptingToStart,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStart,
                          TrackerStart,
                          AttemptingToStart,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStart,
                          PolarisVicraInitialize,
                          AttemptingToStart,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStart,
                          PolarisHybridInitialize,
                          AttemptingToStart,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToStart,
                          AuroraInitialize,
                          AttemptingToStart,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToStart,
                          MicronInitialize,
                          AttemptingToStart,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToStart,
                          AscensionInitialize,
                          AttemptingToStart,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStart,
                          TrackerStop,
                          AttemptingToStart,
                          ReportInvalidRequest);

        //transitions from Started state

  igstkAddTransitionMacro(Started,
                          TrackerStop,
                          AttemptingToStop,
                          TrackerStop);

  igstkAddTransitionMacro(Started,
                          PolarisVicraInitialize,
                          Started,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Started,
                          PolarisHybridInitialize,
                          Started,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(Started,
                          AuroraInitialize,
                          Started,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(Started,
                          MicronInitialize,
                          Started,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(Started,
                          AscensionInitialize,
                          Started,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Started,
                          TrackerInitialize,
                          Started,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Started,
                          TrackerStart,
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
                          GetTracker,
                          AttemptingToStop,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStop,
                          GetTools,
                          AttemptingToStop,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStop,
                          GetReferenceTool,
                          AttemptingToStop,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStop,
                          TrackerInitialize,
                          AttemptingToStop,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStop,
                          TrackerShutdown,
                          AttemptingToStop,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStop,
                          TrackerStart,
                          AttemptingToStop,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStop,
                          PolarisVicraInitialize,
                          AttemptingToStop,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStop,
                          PolarisHybridInitialize,
                          AttemptingToStop,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToStop,
                          AuroraInitialize,
                          AttemptingToStop,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToStop,
                          MicronInitialize,
                          AttemptingToStop,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToStop,
                          AscensionInitialize,
                          AttemptingToStop,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToStop,
                          TrackerStop,
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
                          TrackerInitialize,
                          AttemptingToShutdown,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToShutdown,
                          PolarisVicraInitialize,
                          AttemptingToShutdown,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToShutdown,
                          PolarisHybridInitialize,
                          AttemptingToShutdown,
                          ReportInvalidRequest);
 
  igstkAddTransitionMacro(AttemptingToShutdown,
                          AuroraInitialize,
                          AttemptingToShutdown,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToShutdown,
                          MicronInitialize,
                          AttemptingToShutdown,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToShutdown,
                          AscensionInitialize,
                          AttemptingToShutdown,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToShutdown,
                          TrackerShutdown,
                          AttemptingToShutdown,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToShutdown,
                          GetTracker,
                          AttemptingToShutdown,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToShutdown,
                          GetTools,
                          AttemptingToShutdown,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(AttemptingToShutdown,
                          GetReferenceTool,
                          AttemptingToShutdown,
                          ReportInvalidRequest);

              //set the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

         // done setting the state machine, ready to run
  this->m_StateMachine.SetReadyToRun();
} 


TrackerController::~TrackerController()  
{
     
}

void
TrackerController::RequestInitialize(
  const TrackerConfiguration *configuration)
{
  igstkLogMacro( DEBUG, 
                 "igstkTrackerController::RequestInitialize called...\n" );
  this->m_TmpTrackerConfiguration = 
    const_cast<TrackerConfiguration *>(configuration);
  igstkPushInputMacro( TrackerInitialize );
  this->m_StateMachine.ProcessInputs();
}

void
TrackerController::RequestStart(
  const TrackerConfiguration *configuration)
{
  igstkLogMacro( DEBUG, 
                 "igstkTrackerController::RequestStart called...\n" );
  this->m_TmpTrackerConfiguration = 
    const_cast<TrackerConfiguration *>(configuration);
  igstkPushInputMacro( TrackerStart );
  this->m_StateMachine.ProcessInputs();
}

void
TrackerController::RequestStop(
  const TrackerConfiguration *configuration)
{
  igstkLogMacro( DEBUG, 
                 "igstkTrackerController::RequestStop called...\n" );
    this->m_TmpTrackerConfiguration = 
    const_cast<TrackerConfiguration *>(configuration);
    igstkPushInputMacro( TrackerStop );
    this->m_StateMachine.ProcessInputs();
}

void
TrackerController::RequestShutdown()
{
  igstkLogMacro( DEBUG, 
                 "igstkTrackerController::RequestShutdown called...\n" );
  igstkPushInputMacro( TrackerShutdown );
  this->m_StateMachine.ProcessInputs();
}

void 
TrackerController::RequestGetTracker()
{
  igstkLogMacro( DEBUG, 
                 "igstkTrackerController::RequestGetTracker called...\n" );
  igstkPushInputMacro( GetTracker );
  this->m_StateMachine.ProcessInputs();
}


void 
TrackerController::RequestGetNonReferenceToolList()
{
  igstkLogMacro( DEBUG, 
                 "igstkTrackerController::RequestGetNonReferenceToolList called...\n" );
  igstkPushInputMacro( GetTools );
  this->m_StateMachine.ProcessInputs();
}
void 
TrackerController::RequestGetReferenceTool()
{
  igstkLogMacro( DEBUG, 
                 "igstkTrackerController::RequestGetReferenceTool called...\n" );
  igstkPushInputMacro( GetReferenceTool );
  this->m_StateMachine.ProcessInputs();
}


void 
TrackerController::TrackerInitializeProcessing()
{
  if( this->m_TmpTrackerConfiguration == NULL )
  {
    this->m_ErrorMessage = "Null tracker configuration received.";
    igstkPushInputMacro( Failed );
  }
  else 
  {
              //clear the previous data, call ShutdownProcessing if there is
              //something active
    /*if( this->m_Tracker.IsNotNull() )
    {
      TrackerShutdownProcessing();
    }*/

    if( dynamic_cast<PolarisVicraTrackerConfiguration *>
      ( this->m_TmpTrackerConfiguration ) )
    {
      this->m_TrackerConfiguration = m_TmpTrackerConfiguration;
      igstkPushInputMacro( PolarisVicraInitialize );
    }
    else if( dynamic_cast<PolarisHybridTrackerConfiguration *>
      ( this->m_TmpTrackerConfiguration ) )
    {
      this->m_TrackerConfiguration = m_TmpTrackerConfiguration;
      igstkPushInputMacro( PolarisHybridInitialize );
    }
    else if( dynamic_cast<AuroraTrackerConfiguration *>
      ( this->m_TmpTrackerConfiguration ) )
    {
      this->m_TrackerConfiguration = m_TmpTrackerConfiguration;
      igstkPushInputMacro( AuroraInitialize );
    }
    #ifdef IGSTKSandbox_USE_MicronTracker
    else if( dynamic_cast<MicronTrackerConfiguration *>
      ( this->m_TmpTrackerConfiguration ) )
    {
      this->m_TrackerConfiguration = m_TmpTrackerConfiguration;
      igstkPushInputMacro( MicronInitialize );
    }
    #endif
    else if( dynamic_cast<AscensionTrackerConfiguration *>
    ( this->m_TmpTrackerConfiguration ) )
    {
      this->m_TrackerConfiguration = m_TmpTrackerConfiguration;
      igstkPushInputMacro( AscensionInitialize );
    }
    else
    {
      this->m_ErrorMessage = "Unknown tracker configuration type.";
      igstkPushInputMacro( Failed );
    }
  }
 this->m_StateMachine.ProcessInputs();
}

void 
TrackerController::TrackerStartProcessing()
{  
    StartFailureEvent evt;
    unsigned long observerID;
    observerID = this->m_Tracker->AddObserver( IGSTKErrorEvent(), 
                                             this->m_ErrorObserver );

    m_Tracker->RequestStartTracking();

    if( this->m_ErrorObserver->ErrorOccured() )
    {
      this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
      this->m_ErrorObserver->ClearError();
      m_Tracker->RemoveObserver( observerID );
      //m_Tracker->RequestClose();
      evt.Set( this->m_ErrorMessage );
      this->InvokeEvent( evt );
      igstkPushInputMacro( Failed );
    }
    else
    {
      m_Tracker->RemoveObserver(observerID);
      igstkPushInputMacro( Succeeded );
    }

    this->m_StateMachine.ProcessInputs();
}

void 
TrackerController::TrackerStopProcessing()
{
  StopFailureEvent evt;
  unsigned long observerID;

  observerID = this->m_Tracker->AddObserver( IGSTKErrorEvent(), 
                                             this->m_ErrorObserver );
                //stop tracking
  this->m_Tracker->RequestStopTracking();
  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    this->m_Tracker->RemoveObserver(observerID);
    evt.Set( this->m_ErrorMessage );
    this->InvokeEvent( evt );
    igstkPushInputMacro( Failed );
  }
  else
  {
    this->m_Tracker->RemoveObserver(observerID);
    igstkPushInputMacro( Succeeded );
  }

  this->m_StateMachine.ProcessInputs();
}

void 
TrackerController::TrackerShutdownProcessing()
{
  ShutdownFailureEvent evt;
  unsigned long observerID;

  observerID = this->m_Tracker->AddObserver( IGSTKErrorEvent(), 
                                             this->m_ErrorObserver );
                //stop tracking
  /*this->m_Tracker->RequestStopTracking();
  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    this->m_Tracker->RemoveObserver(observerID);
    evt.Set( this->m_ErrorMessage );
    this->InvokeEvent( evt );
  }*/
                   //close communication with tracker
  this->m_Tracker->RequestClose();
  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    this->m_Tracker->RemoveObserver(observerID);
    evt.Set( this->m_ErrorMessage );
    this->InvokeEvent( evt );
    igstkPushInputMacro( Failed );
  }
  else
  {
   this->m_Tracker->RemoveObserver( observerID );

                   //if serial communication, close COM port
   if( this->m_SerialCommunication.IsNotNull() )
   {
     observerID = this->m_SerialCommunication->AddObserver( ClosePortErrorEvent(),
                                                           this->m_ErrorObserver );
     this->m_SerialCommunication->CloseCommunication();
     this->m_SerialCommunication->RemoveObserver(observerID);
     if( this->m_ErrorObserver->ErrorOccured() )
     {
       this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
       this->m_ErrorObserver->ClearError();
       evt.Set( this->m_ErrorMessage );
       this->InvokeEvent( evt );
       igstkPushInputMacro( Failed );
     }
   }
   this->m_Tracker = NULL;
   this->m_ReferenceTool = NULL;
   this->m_Tools.clear();
   this->m_SerialCommunication = NULL;
   igstkPushInputMacro( Succeeded );
  }

  this->m_StateMachine.ProcessInputs();
}


bool 
TrackerController::InitializeSerialCommunication()
{
  SerialCommunicatingTrackerConfiguration *serialTrackerConfiguration =
    dynamic_cast<SerialCommunicatingTrackerConfiguration *>( this->m_TrackerConfiguration );
  
                 //create serial communication
  this->m_SerialCommunication = igstk::SerialCommunication::New();

              //observe errors generated by the serial communication
  unsigned long observerID = 
    this->m_SerialCommunication->AddObserver( OpenPortErrorEvent(),
                                              this->m_ErrorObserver );

  this->m_SerialCommunication->SetPortNumber( serialTrackerConfiguration->GetCOMPort() );
  this->m_SerialCommunication->SetParity( serialTrackerConfiguration->GetParity() );
  this->m_SerialCommunication->SetBaudRate( serialTrackerConfiguration->GetBaudRate() );
  this->m_SerialCommunication->SetDataBits( serialTrackerConfiguration->GetDataBits() );
  this->m_SerialCommunication->SetStopBits( serialTrackerConfiguration->GetStopBits() );
  this->m_SerialCommunication->SetHardwareHandshake( serialTrackerConfiguration->GetHandshake() );

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

  this->m_SerialCommunication->Print(std::cout);

  return true;
}


PolarisTrackerTool::Pointer
TrackerController::InitializePolarisWirelessTool(
  const PolarisWirelessToolConfiguration *toolConfiguration )
{
  PolarisTrackerTool::Pointer trackerTool = PolarisTrackerTool::New();
    
  trackerTool->RequestSelectWirelessTrackerTool();
  trackerTool->RequestSetSROMFileName( toolConfiguration->GetSROMFile() );
  trackerTool->SetCalibrationTransform( 
    toolConfiguration->GetCalibrationTransform() );
  trackerTool->RequestConfigure();
  return trackerTool;
}


PolarisTrackerTool::Pointer
TrackerController::InitializePolarisWiredTool(
  const PolarisWiredToolConfiguration *toolConfiguration )
{
  PolarisTrackerTool::Pointer trackerTool = PolarisTrackerTool::New();
    
  trackerTool->RequestSelectWiredTrackerTool();
  trackerTool->RequestSetPortNumber( toolConfiguration->GetPortNumber() );
  std::string sromFileName = toolConfiguration->GetSROMFile();
  if( !sromFileName.empty() )
  {
    trackerTool->RequestSetSROMFileName( sromFileName );
  }
  trackerTool->SetCalibrationTransform( 
    toolConfiguration->GetCalibrationTransform() );
  trackerTool->RequestConfigure();
  return trackerTool;
}


void 
TrackerController::PolarisVicraInitializeProcessing()
{
  if( !InitializeSerialCommunication() )
  {
    igstkPushInputMacro( Failed );
  }
  else
  {
                                  //create tracker
    igstk::PolarisTracker::Pointer tracker = igstk::PolarisTracker::New();
    this->m_Tracker = tracker; 
                 //don't need to observe this for errors because the 
                 //configuration class ensures that the frequency is valid
    tracker->RequestSetFrequency( this->m_TrackerConfiguration->GetFrequency() );


                //observe all possible errors generated by the tracker
               //(TrackerOpenErrorEvent, TrackerInitializeErrorEvent,
               //TrackerStartTrackingErrorEvent)
    unsigned long observerID = tracker->AddObserver( IGSTKErrorEvent(),
                                                     this->m_ErrorObserver );

    tracker->SetCommunication( this->m_SerialCommunication );

    tracker->RequestOpen();
    if( this->m_ErrorObserver->ErrorOccured() )
    {
      this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
      this->m_ErrorObserver->ClearError();
      tracker->RemoveObserver(observerID);
      this->m_SerialCommunication->CloseCommunication();
      igstkPushInputMacro( Failed );
    }
    else   //attach the tools and start communication 
    {
      std::vector< TrackerToolConfiguration * > toolConfigurations = 
        this->m_TrackerConfiguration->GetTrackerToolList();
                   //attach tools
      std::vector< TrackerToolConfiguration * >::const_iterator it;
      std::vector< TrackerToolConfiguration * >::const_iterator toolConfigEnd =
        toolConfigurations.end();      
      TrackerTool::Pointer currentTool;
      PolarisWirelessToolConfiguration * currentToolConfig;

      for(it = toolConfigurations.begin(); it!=toolConfigEnd; it++)
      {
        currentToolConfig = static_cast<PolarisWirelessToolConfiguration *>(*it);
        currentTool = InitializePolarisWirelessTool( currentToolConfig );
        this->m_Tools.push_back( currentTool );
        currentTool->RequestAttachToTracker( tracker );
      }
                      //add the reference if we have one
      TrackerToolConfiguration* referenceToolConfiguration = 
        this->m_TrackerConfiguration->GetReferenceTool();
      if( referenceToolConfiguration )
      {
        currentToolConfig = 
          static_cast<PolarisWirelessToolConfiguration *>( referenceToolConfiguration );

        currentTool = InitializePolarisWirelessTool( currentToolConfig );
        this->m_ReferenceTool = currentTool;
        currentTool->RequestAttachToTracker( tracker );
        tracker->RequestSetReferenceTool( currentTool );
      }
      tracker->RequestStartTracking();
      if( this->m_ErrorObserver->ErrorOccured() )
      {
        this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
        this->m_ErrorObserver->ClearError();
        tracker->RemoveObserver( observerID );
        tracker->RequestClose();
        this->m_SerialCommunication->CloseCommunication();
        igstkPushInputMacro( Failed );
      }
      else
      {
        tracker->RemoveObserver(observerID);
        igstkPushInputMacro( Succeeded );
      }
    }
  }
  this->m_StateMachine.ProcessInputs();
}


void 
TrackerController::PolarisHybridInitializeProcessing()
{
  if( !InitializeSerialCommunication() )
  {
    igstkPushInputMacro( Failed );
  }
  else
  {
                                  //create tracker
    igstk::PolarisTracker::Pointer tracker = igstk::PolarisTracker::New();
    this->m_Tracker = tracker; 
                 //don't need to observe this for errors because the 
                 //configuration class ensures that the frequency is valid
    tracker->RequestSetFrequency( this->m_TrackerConfiguration->GetFrequency() );


                //observe all possible errors generated by the tracker
               //(TrackerOpenErrorEvent, TrackerInitializeErrorEvent,
               //TrackerStartTrackingErrorEvent)
    unsigned long observerID = tracker->AddObserver( IGSTKErrorEvent(),
                                                     this->m_ErrorObserver );

    tracker->SetCommunication( this->m_SerialCommunication );

    tracker->RequestOpen();
    if( this->m_ErrorObserver->ErrorOccured() )
    {
      this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
      this->m_ErrorObserver->ClearError();
      tracker->RemoveObserver(observerID);
      this->m_SerialCommunication->CloseCommunication();
      igstkPushInputMacro( Failed );
    }
    else   //attach the tools and start communication 
    {
      std::vector< TrackerToolConfiguration * > toolConfigurations = 
        this->m_TrackerConfiguration->GetTrackerToolList();
                   //attach tools
      std::vector< TrackerToolConfiguration * >::const_iterator it;
      std::vector< TrackerToolConfiguration * >::const_iterator toolConfigEnd =
        toolConfigurations.end();
      TrackerTool::Pointer trackerTool;
      PolarisWirelessToolConfiguration * wirelessToolConfig;
      PolarisWiredToolConfiguration * wiredToolConfig;

      for(it = toolConfigurations.begin(); it!=toolConfigEnd; it++)
      {
        if( (wirelessToolConfig = 
             dynamic_cast<PolarisWirelessToolConfiguration *>(*it)))
        {
          trackerTool = InitializePolarisWirelessTool( wirelessToolConfig );
        }
        else 
        {
          wiredToolConfig = dynamic_cast<PolarisWiredToolConfiguration *>(*it);
          trackerTool = InitializePolarisWiredTool( wiredToolConfig );
        }
        this->m_Tools.push_back( trackerTool );
        trackerTool->RequestAttachToTracker( tracker );
      }
                      //add the reference if we have one
      TrackerToolConfiguration* referenceToolConfiguration = 
        this->m_TrackerConfiguration->GetReferenceTool();
      if( referenceToolConfiguration )
      {
        if ( ( wirelessToolConfig = 
          dynamic_cast<PolarisWirelessToolConfiguration *>( referenceToolConfiguration ) ) )
        {
          trackerTool = InitializePolarisWirelessTool( wirelessToolConfig );
        }
        else
        {
          wiredToolConfig = 
              dynamic_cast<PolarisWiredToolConfiguration *>( referenceToolConfiguration );
          trackerTool = InitializePolarisWiredTool( wiredToolConfig );
        }
        this->m_ReferenceTool = trackerTool;
        trackerTool->RequestAttachToTracker( tracker );
        tracker->RequestSetReferenceTool( trackerTool );
      }
      tracker->RequestStartTracking();
      if( this->m_ErrorObserver->ErrorOccured() )
      {
        this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
        this->m_ErrorObserver->ClearError();
        tracker->RemoveObserver( observerID );
        tracker->RequestClose();
        this->m_SerialCommunication->CloseCommunication();
        igstkPushInputMacro( Failed );
      }
      else
      {
        tracker->RemoveObserver(observerID);
        igstkPushInputMacro( Succeeded );
      }
    }
  }
  this->m_StateMachine.ProcessInputs();
}


AuroraTrackerTool::Pointer 
TrackerController::InitializeAuroraTool(
    const AuroraToolConfiguration *toolConfiguration )
{
  AuroraTrackerTool::Pointer trackerTool = AuroraTrackerTool::New();
  bool is5DOF = toolConfiguration->GetIs5DOF();

  if( is5DOF )
    trackerTool->RequestSelect5DOFTrackerTool();
  else
    trackerTool->RequestSelect6DOFTrackerTool();

  trackerTool->RequestSetPortNumber( toolConfiguration->GetPortNumber() );

  if( is5DOF )
    trackerTool->RequestSetChannelNumber( toolConfiguration->GetChannelNumber() );

  std::string sromFileName = toolConfiguration->GetSROMFile();
  if( !sromFileName.empty() )
  {
    trackerTool->RequestSetSROMFileName( sromFileName );
  }
  trackerTool->SetCalibrationTransform( 
    toolConfiguration->GetCalibrationTransform() );

  trackerTool->RequestConfigure();
  return trackerTool;
}

void 
TrackerController::AuroraInitializeProcessing()
{
  if( !InitializeSerialCommunication() )
  {
    igstkPushInputMacro( Failed );
  }
  else
  {
                                  //create tracker
    igstk::AuroraTracker::Pointer tracker = igstk::AuroraTracker::New();
    this->m_Tracker = tracker; 
                 //don't need to observe this for errors because the 
                 //configuration class ensures that the frequency is valid
    tracker->RequestSetFrequency( this->m_TrackerConfiguration->GetFrequency() );


                //observe all possible errors generated by the tracker
               //(TrackerOpenErrorEvent, TrackerInitializeErrorEvent,
               //TrackerStartTrackingErrorEvent)
    unsigned long observerID = tracker->AddObserver( IGSTKErrorEvent(),
                                                     this->m_ErrorObserver );

    tracker->SetCommunication( this->m_SerialCommunication );

    tracker->RequestOpen();
    if( this->m_ErrorObserver->ErrorOccured() )
    {
      this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
      this->m_ErrorObserver->ClearError();
      tracker->RemoveObserver(observerID);
      this->m_SerialCommunication->CloseCommunication();
      igstkPushInputMacro( Failed );
    }
    else   //attach the tools and start communication 
    {
      std::vector< TrackerToolConfiguration * > toolConfigurations = 
        this->m_TrackerConfiguration->GetTrackerToolList();
                   //attach tools
      std::vector< TrackerToolConfiguration * >::const_iterator it;
      std::vector< TrackerToolConfiguration * >::const_iterator toolConfigEnd =
        toolConfigurations.end();
      TrackerTool::Pointer trackerTool;
      AuroraToolConfiguration * currentToolConfig;

      for(it = toolConfigurations.begin(); it!=toolConfigEnd; it++)
      {
        currentToolConfig = static_cast<AuroraToolConfiguration *>(*it);

        trackerTool = InitializeAuroraTool( currentToolConfig );
        this->m_Tools.push_back( trackerTool );
        trackerTool->RequestAttachToTracker( tracker );
      }
                      //add the reference if we have one
      TrackerToolConfiguration* referenceToolConfiguration = 
        this->m_TrackerConfiguration->GetReferenceTool();
      if( referenceToolConfiguration )
      {
        currentToolConfig = 
          static_cast<AuroraToolConfiguration *>( referenceToolConfiguration );

        trackerTool = InitializeAuroraTool( currentToolConfig );   
        this->m_ReferenceTool = trackerTool;
        trackerTool->RequestAttachToTracker( tracker );
        tracker->RequestSetReferenceTool( trackerTool );
      }
      tracker->RequestStartTracking();
      if( this->m_ErrorObserver->ErrorOccured() )
      {
        this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
        this->m_ErrorObserver->ClearError();
        tracker->RemoveObserver( observerID );
        tracker->RequestClose();
        this->m_SerialCommunication->CloseCommunication();
        igstkPushInputMacro( Failed );
      }
      else
      {
        tracker->RemoveObserver(observerID);
        igstkPushInputMacro( Succeeded );
      }
    }
  }
  this->m_StateMachine.ProcessInputs();
}


#ifdef IGSTKSandbox_USE_MicronTracker
MicronTrackerTool::Pointer TrackerController::InitializeMicronTool(
    const MicronToolConfiguration *toolConfiguration )
{
  MicronTrackerTool::Pointer trackerTool = MicronTrackerTool::New();
 
  trackerTool->RequestSetMarkerName( toolConfiguration->GetMarkerName() );

  igstk::CalibrationIO * reader = new igstk::CalibrationIO;

  std::string fileName = toolConfiguration->GetCalibrationFileName();

  if (itksys::SystemTools::FileExists( fileName.c_str() ) )
  {
    reader->SetFileName( fileName );
    if ( reader->RequestRead( ) )
    {
      trackerTool->SetCalibrationTransform( reader->GetCalibrationTransform() );
      trackerTool->RequestConfigure();
      return trackerTool;
    }
  }

  // if we could not find the attached calibration file
  // we set a default identity transformation
  trackerTool->SetCalibrationTransform( 
             toolConfiguration->GetCalibrationTransform() );

  trackerTool->RequestConfigure();
  return trackerTool;
}
#endif


void TrackerController::MicronInitializeProcessing()
{
  #ifdef IGSTKSandbox_USE_MicronTracker
                  //create tracker
  igstk::MicronTracker::Pointer tracker = igstk::MicronTracker::New();
  this->m_Tracker = tracker; 
                 //don't need to observe this for errors because the 
                 //configuration class ensures that the frequency is valid
  tracker->RequestSetFrequency( this->m_TrackerConfiguration->GetFrequency() );

  MicronTrackerConfiguration *trackerConfiguration =
    dynamic_cast<MicronTrackerConfiguration *>( this->m_TrackerConfiguration );
  
  tracker->SetCameraCalibrationFilesDirectory( 
    trackerConfiguration->GetCameraCalibrationFileDirectory() );

  tracker->SetInitializationFile(
    trackerConfiguration->GetInitializationFile() );
    
  tracker->SetMarkerTemplatesDirectory( 
    trackerConfiguration->GetTemplatesDirectory() );
    
  unsigned long observerID = tracker->AddObserver( IGSTKErrorEvent(),
                                                   this->m_ErrorObserver );
  tracker->RequestOpen();
  
  if( this->m_ErrorObserver->ErrorOccured() )
  {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    tracker->RemoveObserver(observerID);    
    igstkPushInputMacro( Failed );
  }
  else   //attach the tools and start communication 
  {
    std::vector< TrackerToolConfiguration * > toolConfigurations = 
        this->m_TrackerConfiguration->GetTrackerToolList();
                          //attach tools
    std::vector< TrackerToolConfiguration * >::const_iterator it;
    std::vector< TrackerToolConfiguration * >::const_iterator toolConfigEnd =
      toolConfigurations.end();
    TrackerTool::Pointer trackerTool;
    MicronToolConfiguration * currentToolConfig;

    for(it = toolConfigurations.begin(); it!=toolConfigEnd; it++)
    {
      currentToolConfig = static_cast<MicronToolConfiguration *>(*it);

      trackerTool = InitializeMicronTool( currentToolConfig );
      this->m_Tools.push_back( trackerTool );
      trackerTool->RequestAttachToTracker( tracker );
    }
    //add the reference if we have one
    TrackerToolConfiguration* referenceToolConfiguration = 
      this->m_TrackerConfiguration->GetReferenceTool();
    if( referenceToolConfiguration )
    {
      currentToolConfig = 
        static_cast<MicronToolConfiguration *>( referenceToolConfiguration );

      trackerTool = InitializeMicronTool( currentToolConfig );   
      this->m_ReferenceTool = trackerTool;
      trackerTool->RequestAttachToTracker( tracker );
      tracker->RequestSetReferenceTool( trackerTool );
    }
 
    //tracker->RequestStartTracking();
    //if( this->m_ErrorObserver->ErrorOccured() )
    //{
    //  this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    //  this->m_ErrorObserver->ClearError();
    //  m_Tracker->RemoveObserver( observerID );
    //  m_Tracker->RequestClose();
    //  igstkPushInputMacro( Failed );
    //}
  
    //else
    //{
    //  tracker->RemoveObserver(observerID);
    //  igstkPushInputMacro( Succeeded );
    //}

    m_Tracker->RemoveObserver(observerID);
    igstkPushInputMacro( Succeeded );
  }
  this->m_StateMachine.ProcessInputs();
#else
  igstkPushInputMacro( Failed );
  this->m_StateMachine.ProcessInputs();
#endif
}


FlockOfBirdsTrackerTool::Pointer 
TrackerController::InitializeAscensionTool(
    const AscensionToolConfiguration *toolConfiguration )
{
  FlockOfBirdsTrackerTool::Pointer trackerTool = FlockOfBirdsTrackerTool::New();

  trackerTool->RequestSetBirdName("bird0");
/*
  igstk::CalibrationIO * reader = new igstk::CalibrationIO;

  std::string fileName = toolConfiguration->GetCalibrationFileName();

  if (itksys::SystemTools::FileExists( fileName.c_str() ) )
  {
    reader->SetFileName( fileName );
    if ( reader->RequestRead( ) )
    {
      trackerTool->SetCalibrationTransform( reader->GetCalibrationTransform() );
      trackerTool->RequestConfigure();
      return trackerTool;
    }
  }
*/
   //SET SOME IDENTIFIER
  trackerTool->SetCalibrationTransform( 
    toolConfiguration->GetCalibrationTransform() );


  trackerTool->RequestConfigure();
  return trackerTool;
}

void 
TrackerController::AscensionInitializeProcessing()
{
  if( !InitializeSerialCommunication() )
  {
    igstkPushInputMacro( Failed );
  }
  else
  {
    this->m_SerialCommunication->Print(std::cout, 0);

                                  //create tracker
    igstk::FlockOfBirdsTracker::Pointer tracker = igstk::FlockOfBirdsTracker::New();
    this->m_Tracker = tracker; 
                 //don't need to observe this for errors because the 
                 //configuration class ensures that the frequency is valid
    tracker->RequestSetFrequency( this->m_TrackerConfiguration->GetFrequency() );


                //observe all possible errors generated by the tracker
               //(TrackerOpenErrorEvent, TrackerInitializeErrorEvent,
               //TrackerStartTrackingErrorEvent)
    unsigned long observerID = tracker->AddObserver( IGSTKErrorEvent(),
                                                     this->m_ErrorObserver );

    tracker->SetCommunication( this->m_SerialCommunication );

    tracker->Print(std::cout);

    tracker->RequestOpen();
    if( this->m_ErrorObserver->ErrorOccured() )
    {
      this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
      this->m_ErrorObserver->ClearError();
      tracker->RemoveObserver(observerID);
      this->m_SerialCommunication->CloseCommunication();
      igstkPushInputMacro( Failed );
    }
    else   //attach the tools and start communication 
    {
      std::vector< TrackerToolConfiguration * > toolConfigurations = 
        this->m_TrackerConfiguration->GetTrackerToolList();
                   //attach tools
      std::vector< TrackerToolConfiguration * >::const_iterator it;
      std::vector< TrackerToolConfiguration * >::const_iterator toolConfigEnd =
        toolConfigurations.end();
      TrackerTool::Pointer trackerTool;
      AscensionToolConfiguration * currentToolConfig;

      for(it = toolConfigurations.begin(); it!=toolConfigEnd; it++)
      {
        currentToolConfig = static_cast<AscensionToolConfiguration *>(*it);

        trackerTool = InitializeAscensionTool( currentToolConfig );
        this->m_Tools.push_back( trackerTool );
        trackerTool->RequestAttachToTracker( tracker );
      }
                      //add the reference if we have one
      TrackerToolConfiguration* referenceToolConfiguration = 
        this->m_TrackerConfiguration->GetReferenceTool();
      if( referenceToolConfiguration )
      {
        currentToolConfig = 
          static_cast<AscensionToolConfiguration *>( referenceToolConfiguration );

        trackerTool = InitializeAscensionTool( currentToolConfig );   
        this->m_ReferenceTool = trackerTool;
        trackerTool->RequestAttachToTracker( tracker );
        tracker->RequestSetReferenceTool( trackerTool );
      }
      /*
      tracker->RequestStartTracking();
      if( this->m_ErrorObserver->ErrorOccured() )
      {
        this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
        this->m_ErrorObserver->ClearError();
        tracker->RemoveObserver( observerID );
        tracker->RequestClose();
        this->m_SerialCommunication->CloseCommunication();
        igstkPushInputMacro( Failed );
      }
      else
      {
        tracker->RemoveObserver(observerID);
        igstkPushInputMacro( Succeeded );
      }
      */
      m_Tracker->RemoveObserver(observerID);
      igstkPushInputMacro( Succeeded );
    }
  }
  this->m_StateMachine.ProcessInputs();

}

void 
TrackerController::GetTrackerProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "GetTrackerProcessing called...\n");
  RequestTrackerEvent evt;
  evt.Set( this->m_Tracker );
  this->InvokeEvent( evt );
}


void 
TrackerController::GetToolsProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "GetToolsProcessing called...\n");
  RequestToolsEvent evt;
  evt.Set( this->m_Tools );
  this->InvokeEvent( evt );
}


void 
TrackerController::GetReferenceToolProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "GetReferenceToolProcessing called...\n");
  RequestReferenceToolEvent evt;
  evt.Set( this->m_ReferenceTool );
  this->InvokeEvent( evt );
}


void 
TrackerController::ReportInitializationSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "ReportInitializationSuccess called...\n");
  this->InvokeEvent( InitializeSuccessEvent() );
}
void
TrackerController::ReportInitializationFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "ReportInitializationFailureProcessing called...\n");
  InitializeFailureEvent evt; 
  evt.Set( this->m_ErrorMessage );
  this->InvokeEvent( evt );

}
void 
TrackerController::ReportShutdownSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "ReportShutdownSuccess called...\n");
  this->InvokeEvent( ShutdownSuccessEvent() );
}
void 
TrackerController::ReportShutdownFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "ReportShutdownFailure called...\n");
  ShutdownFailureEvent evt; 
  evt.Set( this->m_ErrorMessage );
  this->InvokeEvent( evt );
}
void 
TrackerController::ReportStartSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "ReportStartSuccess called...\n");
  this->InvokeEvent( StartSuccessEvent() );
}
void 
TrackerController::ReportStartFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "ReportStartFailure called...\n");
  StartFailureEvent evt; 
  evt.Set( this->m_ErrorMessage );
  this->InvokeEvent( evt );
}
void 
TrackerController::ReportStopSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "ReportStopSuccess called...\n");
  this->InvokeEvent( StopSuccessEvent() );
}
void 
TrackerController::ReportStopFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "ReportStopFailure called...\n");
  StopFailureEvent evt; 
  evt.Set( this->m_ErrorMessage );
  this->InvokeEvent( evt );
}
void 
TrackerController::ReportInvalidRequestProcessing()
{
  igstkLogMacro( DEBUG, "igstk::TrackerController::"
                 "ReportInvalidRequestProcessing called...\n");
  this->InvokeEvent(InvalidRequestErrorEvent());
}


TrackerController::ErrorObserver::ErrorObserver() : m_ErrorOccured(false)
{                              //serial communication errors
  this->m_ErrorEvent2ErrorMessage.insert(
    std::pair<std::string,std::string>( OpenPortErrorEvent().GetEventName(),
                                       "Error opening com port." ) );
  this->m_ErrorEvent2ErrorMessage.insert(
    std::pair<std::string,std::string>( ClosePortErrorEvent().GetEventName(),
                                        "Error closing com port." ) );
                              //tracker errors
  this->m_ErrorEvent2ErrorMessage.insert(
    std::pair<std::string,std::string>( TrackerOpenErrorEvent().GetEventName(),
                                        "Error opening tracker communication." ) );
  this->m_ErrorEvent2ErrorMessage.insert(
    std::pair<std::string,std::string>( TrackerInitializeErrorEvent().GetEventName(),
                                        "Error initializing tracker." ) );
  this->m_ErrorEvent2ErrorMessage.insert(
    std::pair<std::string,std::string>( TrackerStartTrackingErrorEvent().GetEventName(),
                                        "Error starting tracking." ) );
  this->m_ErrorEvent2ErrorMessage.insert(
    std::pair<std::string,std::string>( TrackerStopTrackingErrorEvent().GetEventName(),
                                        "Error stopping tracking." ) );
  this->m_ErrorEvent2ErrorMessage.insert(
    std::pair<std::string,std::string>( TrackerCloseErrorEvent().GetEventName(),
                                        "Error closing tracker communication." ) );
}

void 
TrackerController::ErrorObserver::Execute(itk::Object *caller, 
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
TrackerController::ErrorObserver::Execute(const itk::Object *caller, 
                                            const itk::EventObject & event) throw (std::exception)
{
  const itk::Object * constCaller = caller;
  this->Execute(constCaller, event);
}

}//end namespace igstk
