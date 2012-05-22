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
#include "igstkAscensionTracker.h"

#ifdef IGSTK_USE_MicronTracker
#include "igstkMicronTracker.h"
#endif

#ifdef IGSTK_USE_Ascension3DGTracker
#include "igstkAscension3DGTracker.h"
#endif

#ifdef IGSTK_USE_NDICertusTracker
#include "igstkNDICertusTracker.h"
#endif

namespace igstk
{ 


TrackerController::TrackerController() : m_StateMachine( this )
{
  //create error observer
  this->m_ErrorObserver = ErrorObserver::New();

  //create observer for the tracker update status events
  this->m_TrackerUpdateStatusObserver = 
    TrackerUpdateObserver::New();
  this->m_TrackerUpdateStatusObserver->SetParent( this );

  //define the state machine's states 
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( AttemptingToInitialize );
  igstkAddStateMacro( AttemptingToInitializePolarisVicra );
  igstkAddStateMacro( AttemptingToInitializePolarisHybrid );
  igstkAddStateMacro( AttemptingToInitializeAurora );
  igstkAddStateMacro( AttemptingToInitializeCertus );
  igstkAddStateMacro( AttemptingToInitializeMicron );
  igstkAddStateMacro( AttemptingToInitializeAscension3DG );
  igstkAddStateMacro( AttemptingToInitializeAscension );  
  igstkAddStateMacro( Initialized );
  igstkAddStateMacro( AttemptingToStartTracking );
  igstkAddStateMacro( Tracking );
  igstkAddStateMacro( AttemptingToCloseCommunication );
  igstkAddStateMacro( AttemptingToStopTracking );

  //define the state machine's inputs
  igstkAddInputMacro( TrackerInitialize );
  igstkAddInputMacro( PolarisVicraInitialize );
  igstkAddInputMacro( PolarisHybridInitialize );
  igstkAddInputMacro( AuroraInitialize );
  igstkAddInputMacro( AscensionInitialize );
  igstkAddInputMacro( MicronInitialize );
  igstkAddInputMacro( CertusInitialize );
  igstkAddInputMacro( Ascension3DGInitialize );
  igstkAddInputMacro( StartTracking );
  igstkAddInputMacro( StopTracking );
  igstkAddInputMacro( Failed  );
  igstkAddInputMacro( Succeeded  );
  igstkAddInputMacro( CloseCommunication );
  igstkAddInputMacro( GetTools  );
  igstkAddInputMacro( GetTool  );
  igstkAddInputMacro( GetReferenceTool  );
  igstkAddInputMacro( SetParentSpatialObject  );
  igstkAddInputMacro( SetChildSpatialObject  );

  //define the state machine's transitions

  //transitions from Idle state
  igstkAddTransitionMacro( Idle,
                           TrackerInitialize,
                           AttemptingToInitialize,
                           TrackerInitialize );

  igstkAddTransitionMacro( Idle,
                           PolarisVicraInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Idle,
                           PolarisHybridInitialize,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( Idle,
                           AuroraInitialize,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( Idle,
                           CertusInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Idle,
                           AscensionInitialize,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( Idle,
                           MicronInitialize,
                           Idle,
                           ReportInvalidRequest );
 
  igstkAddTransitionMacro( Idle,
                           Ascension3DGInitialize,
                           Idle,
                           ReportInvalidRequest );
    
  igstkAddTransitionMacro( Idle,
                           StartTracking,
                           Idle,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( Idle,
                           StopTracking,
                           Idle,
                           ReportStopTrackingSuccess );

  igstkAddTransitionMacro( Idle,
                           Failed,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Idle,
                           Succeeded,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( Idle,
                           CloseCommunication,
                           Idle,
                           ReportCloseCommunicationSuccess );
  
  igstkAddTransitionMacro( Idle,
                           GetTools,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Idle,
                           GetTool,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Idle,
                           GetReferenceTool,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Idle,
                           SetParentSpatialObject,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Idle,
                           SetChildSpatialObject,
                           Idle,
                           ReportInvalidRequest );

  //transitions from AttemptingToInitialize state
  igstkAddTransitionMacro( AttemptingToInitialize,
                           Failed,
                           Idle,
                           ReportInitializationFailure );

  igstkAddTransitionMacro( AttemptingToInitialize,
                           PolarisVicraInitialize,
                           AttemptingToInitializePolarisVicra,
                           PolarisVicraInitialize );
  
  igstkAddTransitionMacro( AttemptingToInitialize,
                           PolarisHybridInitialize,
                           AttemptingToInitializePolarisHybrid,
                           PolarisHybridInitialize );
 
  igstkAddTransitionMacro( AttemptingToInitialize,
                           AuroraInitialize,
                           AttemptingToInitializeAurora,
                           AuroraInitialize );

  igstkAddTransitionMacro( AttemptingToInitialize,
                           AscensionInitialize,
                           AttemptingToInitializeAscension,
                           AscensionInitialize );

  igstkAddTransitionMacro( AttemptingToInitialize,
                           MicronInitialize,
                           AttemptingToInitializeMicron,
                           MicronInitialize );

  igstkAddTransitionMacro( AttemptingToInitialize,
                           CertusInitialize,
                           AttemptingToInitializeCertus,
                           CertusInitialize );

  igstkAddTransitionMacro( AttemptingToInitialize,
                           Ascension3DGInitialize,
                           AttemptingToInitializeAscension3DG,
                           Ascension3DGInitialize );

  igstkAddTransitionMacro( AttemptingToInitialize,
                           TrackerInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitialize,
                           StartTracking,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitialize,
                           StopTracking,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitialize,
                           Succeeded,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitialize,
                           CloseCommunication,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitialize,
                           GetTools,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitialize,
                           GetTool,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitialize,
                           GetReferenceTool,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitialize,
                           SetParentSpatialObject,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitialize,
                           SetChildSpatialObject,
                           Idle,
                           ReportInvalidRequest );

  //transitions from AttemptingToInitializePolarisVicra state
  igstkAddTransitionMacro( AttemptingToInitializePolarisVicra,
                           Failed,
                           Idle,
                           ReportInitializationFailure );
  
  igstkAddTransitionMacro( AttemptingToInitializePolarisVicra,
                           Succeeded,
                           Initialized,
                           ReportInitializationSuccess );

  igstkAddTransitionMacro( AttemptingToInitializePolarisVicra,
                           TrackerInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializePolarisVicra,
                           PolarisVicraInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializePolarisVicra,
                           PolarisHybridInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializePolarisVicra,
                           AuroraInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializePolarisVicra,
                           AscensionInitialize,
                           Idle,
                           ReportInvalidRequest );

   igstkAddTransitionMacro( AttemptingToInitializePolarisVicra,
                           CertusInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializePolarisVicra,
                           MicronInitialize,
                           Idle,
                           ReportInvalidRequest );
 
  igstkAddTransitionMacro( AttemptingToInitializePolarisVicra,
                           Ascension3DGInitialize,
                           Idle,
                           ReportInvalidRequest );
    
  igstkAddTransitionMacro( AttemptingToInitializePolarisVicra,
                           StartTracking,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitializePolarisVicra,
                           StopTracking,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializePolarisVicra,
                           CloseCommunication,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializePolarisVicra,
                           GetTools,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitializePolarisVicra,
                           GetTool,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializePolarisVicra,
                           GetReferenceTool,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializePolarisVicra,
                           SetParentSpatialObject,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializePolarisVicra,
                           SetChildSpatialObject,
                           Idle,
                           ReportInvalidRequest );

  //transitions from AttemptingToInitializePolarisHybrid state
  igstkAddTransitionMacro( AttemptingToInitializePolarisHybrid,
                           Failed,
                           Idle,
                           ReportInitializationFailure );
  
  igstkAddTransitionMacro( AttemptingToInitializePolarisHybrid,
                           Succeeded,
                           Initialized,
                           ReportInitializationSuccess );

  igstkAddTransitionMacro( AttemptingToInitializePolarisHybrid,
                           TrackerInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializePolarisHybrid,
                           PolarisVicraInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializePolarisHybrid,
                           PolarisHybridInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializePolarisHybrid,
                           AuroraInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializePolarisHybrid,
                           CertusInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializePolarisHybrid,
                           AscensionInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializePolarisHybrid,
                           MicronInitialize,
                           Idle,
                           ReportInvalidRequest );
 
  igstkAddTransitionMacro( AttemptingToInitializePolarisHybrid,
                           Ascension3DGInitialize,
                           Idle,
                           ReportInvalidRequest );
    
  igstkAddTransitionMacro( AttemptingToInitializePolarisHybrid,
                           StartTracking,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitializePolarisHybrid,
                           StopTracking,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializePolarisHybrid,
                           CloseCommunication,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializePolarisHybrid,
                           GetTools,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitializePolarisHybrid,
                           GetTool,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializePolarisHybrid,
                           GetReferenceTool,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializePolarisHybrid,
                           SetParentSpatialObject,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializePolarisHybrid,
                           SetChildSpatialObject,
                           Idle,
                           ReportInvalidRequest );

  //transitions from AttemptingToInitializeAurora state
  igstkAddTransitionMacro( AttemptingToInitializeAurora,
                           Failed,
                           Idle,
                           ReportInitializationFailure );
  
  igstkAddTransitionMacro( AttemptingToInitializeAurora,
                           Succeeded,
                           Initialized,
                           ReportInitializationSuccess );

  igstkAddTransitionMacro( AttemptingToInitializeAurora,
                           TrackerInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAurora,
                           PolarisVicraInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAurora,
                           PolarisHybridInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAurora,
                           AuroraInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAurora,
                           CertusInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAurora,
                           AscensionInitialize,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitializeAurora,
                           MicronInitialize,
                           Idle,
                           ReportInvalidRequest );
 
  igstkAddTransitionMacro( AttemptingToInitializeAurora,
                           Ascension3DGInitialize,
                           Idle,
                           ReportInvalidRequest );
                           
    
  igstkAddTransitionMacro( AttemptingToInitializeAurora,
                           StartTracking,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitializeAurora,
                           StopTracking,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAurora,
                           CloseCommunication,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAurora,
                           GetTools,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitializeAurora,
                           GetTool,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAurora,
                           GetReferenceTool,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAurora,
                           SetParentSpatialObject,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAurora,
                           SetChildSpatialObject,
                           Idle,
                           ReportInvalidRequest );

    //transitions from AttemptingToInitializeAscension state
  igstkAddTransitionMacro( AttemptingToInitializeAscension,
                           Failed,
                           Idle,
                           ReportInitializationFailure );
  
  igstkAddTransitionMacro( AttemptingToInitializeAscension,
                           Succeeded,
                           Initialized,
                           ReportInitializationSuccess );

  igstkAddTransitionMacro( AttemptingToInitializeAscension,
                           TrackerInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAscension,
                           PolarisVicraInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAscension,
                           PolarisHybridInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAscension,
                           AscensionInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAscension,
                           AuroraInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAscension,
                           CertusInitialize,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitializeAscension,
                           MicronInitialize,
                           Idle,
                           ReportInvalidRequest );
    
  igstkAddTransitionMacro( AttemptingToInitializeAscension,
                           Ascension3DGInitialize,
                           Idle,
                           ReportInvalidRequest );
 
  igstkAddTransitionMacro( AttemptingToInitializeAscension,
                           StartTracking,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitializeAscension,
                           StopTracking,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAscension,
                           CloseCommunication,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAscension,
                           GetTools,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitializeAscension,
                           GetTool,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAscension,
                           GetReferenceTool,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAscension,
                           SetParentSpatialObject,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAscension,
                           SetChildSpatialObject,
                           Idle,
                           ReportInvalidRequest );

  //transitions from AttemptingToInitializeMicron state
  igstkAddTransitionMacro( AttemptingToInitializeMicron,
                           Failed,
                           Idle,
                           ReportInitializationFailure );
  
  igstkAddTransitionMacro( AttemptingToInitializeMicron,
                           Succeeded,
                           Initialized,
                           ReportInitializationSuccess );

  igstkAddTransitionMacro( AttemptingToInitializeMicron,
                           TrackerInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeMicron,
                           PolarisVicraInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeMicron,
                           PolarisHybridInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeMicron,
                           AuroraInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeMicron,
                           CertusInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeMicron,
                           AscensionInitialize,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitializeMicron,
                           MicronInitialize,
                           Idle,
                           ReportInvalidRequest );
    
  igstkAddTransitionMacro( AttemptingToInitializeMicron,
                           StartTracking,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitializeMicron,
                           StopTracking,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeMicron,
                           CloseCommunication,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeMicron,
                           GetTools,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitializeMicron,
                           GetTool,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeMicron,
                           GetReferenceTool,
                           Idle,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToInitializeMicron,
                           SetParentSpatialObject,
                           Idle,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToInitializeMicron,
                           SetChildSpatialObject,
                           Idle,
                           ReportInvalidRequest);

//transitions from AttemptingToInitializeAscension3DG state
  igstkAddTransitionMacro( AttemptingToInitializeAscension3DG,
                           Failed,
                           Idle,
                           ReportInitializationFailure );
  
  igstkAddTransitionMacro( AttemptingToInitializeAscension3DG,
                           Succeeded,
                           Initialized,
                           ReportInitializationSuccess );

  igstkAddTransitionMacro( AttemptingToInitializeAscension3DG,
                           TrackerInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAscension3DG,
                           PolarisVicraInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAscension3DG,
                           PolarisHybridInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAscension3DG,
                           AuroraInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAscension3DG,
                           CertusInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAscension3DG,
                           AscensionInitialize,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitializeAscension3DG,
                           MicronInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAscension3DG,
                           Ascension3DGInitialize,
                           Idle,
                           ReportInvalidRequest );
                           
  igstkAddTransitionMacro( AttemptingToInitializeAscension3DG,
                           StartTracking,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitializeAscension3DG,
                           StopTracking,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAscension3DG,
                           CloseCommunication,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAscension3DG,
                           GetTools,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitializeAscension3DG,
                           GetTool,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeAscension3DG,
                           GetReferenceTool,
                           Idle,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToInitializeAscension3DG,
                           SetParentSpatialObject,
                           Idle,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToInitializeAscension3DG,
                           SetChildSpatialObject,
                           Idle,
                           ReportInvalidRequest);

  //transitions from AttemptingToInitializeCertus state
  igstkAddTransitionMacro( AttemptingToInitializeCertus,
                           Failed,
                           Idle,
                           ReportInitializationFailure );
  
  igstkAddTransitionMacro( AttemptingToInitializeCertus,
                           Succeeded,
                           Initialized,
                           ReportInitializationSuccess );

  igstkAddTransitionMacro( AttemptingToInitializeCertus,
                           TrackerInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeCertus,
                           PolarisVicraInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeCertus,
                           PolarisHybridInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeCertus,
                           AuroraInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeCertus,
                           Ascension3DGInitialize,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitializeCertus,
                           AscensionInitialize,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitializeCertus,
                           MicronInitialize,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeCertus,
                           CertusInitialize,
                           Idle,
                           ReportInvalidRequest );
    
  igstkAddTransitionMacro( AttemptingToInitializeCertus,
                           StartTracking,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitializeCertus,
                           StopTracking,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeCertus,
                           CloseCommunication,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeCertus,
                           GetTools,
                           Idle,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToInitializeCertus,
                           GetTool,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitializeCertus,
                           GetReferenceTool,
                           Idle,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToInitializeCertus,
                           SetParentSpatialObject,
                           Idle,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToInitializeCertus,
                           SetChildSpatialObject,
                           Idle,
                           ReportInvalidRequest);

  
  //transitions from Initialized state
  igstkAddTransitionMacro( Initialized,
                           GetTools,
                           Initialized,
                           GetTools );

  igstkAddTransitionMacro( Initialized,
                           GetTool,
                           Initialized,
                           GetTool );

  igstkAddTransitionMacro( Initialized,
                           GetReferenceTool,
                           Initialized,
                           GetReferenceTool );

  igstkAddTransitionMacro( Initialized,
                           SetParentSpatialObject,
                           Initialized,
                           SetParentSpatialObject );

  igstkAddTransitionMacro( Initialized,
                           SetChildSpatialObject,
                           Initialized,
                           SetChildSpatialObject );

  igstkAddTransitionMacro( Initialized,
                           StartTracking,
                           AttemptingToStartTracking,
                           StartTracking);

  igstkAddTransitionMacro( Initialized,
                           StopTracking,
                           Initialized,
                           ReportStopTrackingSuccess);

  igstkAddTransitionMacro( Initialized,
                           TrackerInitialize,
                           AttemptingToInitialize,
                           TrackerInitialize );

  igstkAddTransitionMacro( Initialized,
                           CloseCommunication,
                           AttemptingToCloseCommunication,
                           CloseCommunication );

  igstkAddTransitionMacro( Initialized,
                           PolarisVicraInitialize,
                           Initialized,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Initialized,
                           PolarisHybridInitialize,
                           Initialized,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( Initialized,
                           AuroraInitialize,
                           Initialized,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Initialized,
                           CertusInitialize,
                           Initialized,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Initialized,
                           AscensionInitialize,
                           Initialized,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( Initialized,
                           MicronInitialize,
                           Initialized,
                           ReportInvalidRequest );

 igstkAddTransitionMacro(  Initialized,
                           Ascension3DGInitialize,
                           Initialized,
                           ReportInvalidRequest );
                           
  igstkAddTransitionMacro( Initialized,
                           Failed,
                           Initialized,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Initialized,
                           Succeeded,
                           Initialized,
                           ReportInvalidRequest );

  //transitions from AttemptingToStartTracking state
  igstkAddTransitionMacro( AttemptingToStartTracking,
                           Succeeded,
                           Tracking,
                           ReportStartTrackingSuccess );

  igstkAddTransitionMacro( AttemptingToStartTracking,
                           Failed,
                           Initialized,
                           ReportStartTrackingFailure );

  igstkAddTransitionMacro( AttemptingToStartTracking,
                           TrackerInitialize,
                           AttemptingToStartTracking,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToStartTracking,
                           PolarisVicraInitialize,
                           AttemptingToStartTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStartTracking,
                           PolarisHybridInitialize,
                           AttemptingToStartTracking,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToStartTracking,
                           AuroraInitialize,
                           AttemptingToStartTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStartTracking,
                           CertusInitialize,
                           AttemptingToStartTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStartTracking,
                           AscensionInitialize,
                           AttemptingToStartTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStartTracking,
                           MicronInitialize,
                           AttemptingToStartTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStartTracking,
                           Ascension3DGInitialize,
                           AttemptingToStartTracking,
                           ReportInvalidRequest );
 
  igstkAddTransitionMacro( AttemptingToStartTracking,
                           StartTracking,
                           AttemptingToStartTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStartTracking,
                           StopTracking,
                           AttemptingToStartTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStartTracking,
                           CloseCommunication,
                           AttemptingToStartTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStartTracking,
                           GetTools,
                           AttemptingToStartTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStartTracking,
                           GetTool,
                           AttemptingToStartTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStartTracking,
                           GetReferenceTool,
                           AttemptingToStartTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStartTracking,
                           SetParentSpatialObject,
                           AttemptingToStartTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStartTracking,
                           SetChildSpatialObject,
                           AttemptingToStartTracking,
                           ReportInvalidRequest );

  //transitions from Tracking state
  igstkAddTransitionMacro( Tracking,
                           StopTracking,
                           AttemptingToStopTracking,
                           StopTracking );

  igstkAddTransitionMacro( Tracking,
                           TrackerInitialize,
                           Tracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Tracking,
                           PolarisVicraInitialize,
                           Tracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Tracking,
                           PolarisHybridInitialize,
                           Tracking,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( Tracking,
                           AuroraInitialize,
                           Tracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Tracking,
                           CertusInitialize,
                           Tracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Tracking,
                           AscensionInitialize,
                           Tracking,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( Tracking,
                           MicronInitialize,
                           Tracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Tracking,
                           Ascension3DGInitialize,
                           Tracking,
                           ReportInvalidRequest );
                           
  igstkAddTransitionMacro( Tracking,
                           StartTracking,
                           Tracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Tracking,
                           Failed,
                           Tracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Tracking,
                           Succeeded,
                           Tracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Tracking,
                           CloseCommunication,
                           Tracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Tracking,
                           GetTools,
                           Tracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Tracking,
                           GetTool,
                           Tracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Tracking,
                           GetReferenceTool,
                           Tracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Tracking,
                           SetParentSpatialObject,
                           Tracking,
                           SetParentSpatialObject );

  igstkAddTransitionMacro( Tracking,
                           SetChildSpatialObject,
                           Tracking,
                           SetChildSpatialObject );

  //transitions from AttemtingtoStop state
  igstkAddTransitionMacro( AttemptingToStopTracking,
                           Succeeded,
                           Initialized,
                           ReportStopTrackingSuccess );

  igstkAddTransitionMacro( AttemptingToStopTracking,
                           Failed,
                           Tracking,
                           ReportStopTrackingFailure );

  igstkAddTransitionMacro( AttemptingToStopTracking,
                           TrackerInitialize,
                           AttemptingToStopTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStopTracking,
                           PolarisVicraInitialize,
                           AttemptingToStopTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStopTracking,
                           PolarisHybridInitialize,
                           AttemptingToStopTracking,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToStopTracking,
                           AuroraInitialize,
                           AttemptingToStopTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStopTracking,
                           CertusInitialize,
                           AttemptingToStopTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStopTracking,
                           AscensionInitialize,
                           AttemptingToStopTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStopTracking,
                           MicronInitialize,
                           AttemptingToStopTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStopTracking,
                           Ascension3DGInitialize,
                           AttemptingToStopTracking,
                           ReportInvalidRequest );
                           
  igstkAddTransitionMacro( AttemptingToStopTracking,
                           StartTracking,
                           AttemptingToStopTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStopTracking,
                           StopTracking,
                           AttemptingToStopTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStopTracking,
                           CloseCommunication,
                           AttemptingToStopTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStopTracking,
                           GetTools,
                           AttemptingToStopTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStopTracking,
                           GetTool,
                           AttemptingToStopTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStopTracking,
                           GetReferenceTool,
                           AttemptingToStopTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStopTracking,
                           SetParentSpatialObject,
                           AttemptingToStopTracking,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToStopTracking,
                           SetChildSpatialObject,
                           AttemptingToStopTracking,
                           ReportInvalidRequest );

  //transitions from AttemptingToCloseCommunication state

  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           Succeeded,
                           Idle,
                           ReportCloseCommunicationSuccess );

  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           Failed,
                           Initialized,
                           ReportCloseCommunicationFailure );

  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           TrackerInitialize,
                           AttemptingToCloseCommunication,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           PolarisVicraInitialize,
                           AttemptingToCloseCommunication,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           PolarisHybridInitialize,
                           AttemptingToCloseCommunication,
                           ReportInvalidRequest );
 
  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           AuroraInitialize,
                           AttemptingToCloseCommunication,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           CertusInitialize,
                           AttemptingToCloseCommunication,
                           ReportInvalidRequest );

   igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           AscensionInitialize,
                           AttemptingToCloseCommunication,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           MicronInitialize,
                           AttemptingToCloseCommunication,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           Ascension3DGInitialize,
                           AttemptingToCloseCommunication,
                           ReportInvalidRequest );
                           
  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           StartTracking,
                           AttemptingToCloseCommunication,
                           ReportInvalidRequest );
  
  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           StopTracking,
                           AttemptingToCloseCommunication,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           CloseCommunication,
                           AttemptingToCloseCommunication,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           GetTools,
                           AttemptingToCloseCommunication,
                           ReportInvalidRequest);
  
  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           GetTool,
                           AttemptingToCloseCommunication,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           GetReferenceTool,
                           AttemptingToCloseCommunication,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           SetParentSpatialObject,
                           AttemptingToCloseCommunication,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToCloseCommunication,
                           SetChildSpatialObject,
                           AttemptingToCloseCommunication,
                           ReportInvalidRequest );

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
TrackerController::RequestStartTracking( )
{
  igstkLogMacro( DEBUG, 
                 "igstkTrackerController::RequestStartTracking called...\n" );
  igstkPushInputMacro( StartTracking );
  this->m_StateMachine.ProcessInputs();
}


void
TrackerController::RequestStopTracking()
{
  igstkLogMacro( DEBUG, 
                 "igstkTrackerController::RequestStopTracking called...\n" );
  igstkPushInputMacro( StopTracking );
  this->m_StateMachine.ProcessInputs();
}


void
TrackerController::RequestShutdown()
{
  igstkLogMacro( DEBUG, 
                 "igstkTrackerController::RequestShutdown called...\n" );
  igstkPushInputMacro( StopTracking );
  igstkPushInputMacro( CloseCommunication );
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
TrackerController::RequestGetTool( const std::string &toolName )
{
  igstkLogMacro( DEBUG, 
                 "igstkTrackerController::RequestGetTool called...\n" );
  this->m_RequestedToolName = toolName;
  igstkPushInputMacro( GetTool );
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
TrackerController::RequestSetParentSpatialObject( igstk::Transform transform, 
                                igstk::SpatialObject * spatialObject)
{
  igstkLogMacro( DEBUG, 
    "igstkTrackerController::RequestSetParentSpatialObject called...\n" );

  m_TmpTransform = transform;
  m_TmpSpatialObject = spatialObject;

  igstkPushInputMacro( SetParentSpatialObject );
  this->m_StateMachine.ProcessInputs();
}


void 
TrackerController::RequestAddChildSpatialObject( igstk::Transform transform, 
                                igstk::SpatialObject * spatialObject)
{
  igstkLogMacro( DEBUG, 
    "igstkTrackerController::RequestAddChildSpatialObject called...\n" );

  m_TmpTransform = transform;
  m_TmpSpatialObject = spatialObject;

  igstkPushInputMacro( SetChildSpatialObject );
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
    else if( dynamic_cast<AscensionTrackerConfiguration *>
      ( this->m_TmpTrackerConfiguration ) )
      {
      this->m_TrackerConfiguration = m_TmpTrackerConfiguration;
      igstkPushInputMacro( AscensionInitialize );
      }
   #ifdef IGSTK_USE_MicronTracker
    else if( dynamic_cast<MicronTrackerConfiguration *>
      ( this->m_TmpTrackerConfiguration ) )
      {
      this->m_TrackerConfiguration = m_TmpTrackerConfiguration;
      igstkPushInputMacro( MicronInitialize );
      }
   #endif
   #ifdef IGSTK_USE_Ascension3DGTracker
    else if( dynamic_cast<Ascension3DGTrackerConfiguration *>
      ( this->m_TmpTrackerConfiguration ) )
      {
      this->m_TrackerConfiguration = m_TmpTrackerConfiguration;
      igstkPushInputMacro( Ascension3DGInitialize );
      }
   #endif
   
    #ifdef IGSTK_USE_NDICertusTracker
	else if( dynamic_cast<CertusTrackerConfiguration *>
      ( this->m_TmpTrackerConfiguration ) )
      {
      this->m_TrackerConfiguration = m_TmpTrackerConfiguration;
      igstkPushInputMacro( CertusInitialize );
      }
    #endif

    else
      {
      this->m_ErrorMessage = 
    "Unrecognized tracker type (Possibly IGSTK_USE_MicronTracker or IGSTK_USE_NDICertusTracker flag are off).";
      igstkPushInputMacro( Failed );
      }
    }
 this->m_StateMachine.ProcessInputs();
}


void 
TrackerController::StartTrackingProcessing()
{  
  unsigned long observerID;
              
  observerID = this->m_Tracker->AddObserver( IGSTKErrorEvent(), 
                                             this->m_ErrorObserver );
  m_Tracker->RequestStartTracking();
  this->m_Tracker->RemoveObserver(observerID);

  if( this->m_ErrorObserver->ErrorOccured() )
    {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    igstkPushInputMacro( Failed );
    }
  else
    {
    igstkPushInputMacro( Succeeded );
    }
  this->m_StateMachine.ProcessInputs();
}


void 
TrackerController::StopTrackingProcessing()
{
  unsigned long observerID;

  observerID = this->m_Tracker->AddObserver( IGSTKErrorEvent(), 
                                             this->m_ErrorObserver );
  this->m_Tracker->RequestStopTracking();
  this->m_Tracker->RemoveObserver(observerID);

  if( this->m_ErrorObserver->ErrorOccured() )
    {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    igstkPushInputMacro( Failed );
    }
  else
    {
    igstkPushInputMacro( Succeeded );
    }
  this->m_StateMachine.ProcessInputs();
}


void 
TrackerController::CloseCommunicationProcessing()
{
  CloseCommunicationErrorEvent evt;
  unsigned long observerID;

  observerID = this->m_Tracker->AddObserver( IGSTKErrorEvent(), 
                                             this->m_ErrorObserver );  
                   //close communication with tracker
  this->m_Tracker->RequestClose();
  this->m_Tracker->RemoveObserver( observerID );

  if( this->m_ErrorObserver->ErrorOccured() )
    {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    igstkPushInputMacro( Failed );
    }
  else
    {
    //if serial communication, close COM port
    if( this->m_SerialCommunication.IsNotNull() )
      {
      observerID = 
        this->m_SerialCommunication->AddObserver( ClosePortErrorEvent(),
                                                  this->m_ErrorObserver );
      this->m_SerialCommunication->CloseCommunication();
      this->m_SerialCommunication->RemoveObserver(observerID);
      if( this->m_ErrorObserver->ErrorOccured() )
        {
        this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
        this->m_ErrorObserver->ClearError();
        igstkPushInputMacro( Failed );
        }
      else
        {
        igstkPushInputMacro( Succeeded );
        }
      }
    else //not serial communication
      {
      igstkPushInputMacro( Succeeded );
      }
    }
  this->m_StateMachine.ProcessInputs();
}


bool 
TrackerController::InitializeSerialCommunication()
{
  SerialCommunicatingTrackerConfiguration *serialTrackerConfiguration =
    dynamic_cast<SerialCommunicatingTrackerConfiguration *>
    ( this->m_TrackerConfiguration );
  
                 //create serial communication
  this->m_SerialCommunication = igstk::SerialCommunication::New();

              //observe errors generated by the serial communication
  unsigned long observerID = 
    this->m_SerialCommunication->AddObserver( OpenPortErrorEvent(),
                                              this->m_ErrorObserver );

  this->m_SerialCommunication->SetPortNumber
    (serialTrackerConfiguration->GetCOMPort() );
  this->m_SerialCommunication->SetParity
    ( serialTrackerConfiguration->GetParity() );
  this->m_SerialCommunication->SetBaudRate
    ( serialTrackerConfiguration->GetBaudRate() );
  this->m_SerialCommunication->SetDataBits
    ( serialTrackerConfiguration->GetDataBits() );
  this->m_SerialCommunication->SetStopBits
    ( serialTrackerConfiguration->GetStopBits() );
  this->m_SerialCommunication->SetHardwareHandshake
    ( serialTrackerConfiguration->GetHandshake() );

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
    //(TrackerOpenErrorEvent, TrackerInitializeErrorEvent)
    unsigned long observerID = tracker->AddObserver( IGSTKErrorEvent(),
                                                     this->m_ErrorObserver );
    tracker->SetCommunication( this->m_SerialCommunication );
    tracker->RequestOpen();
    tracker->RemoveObserver(observerID);

    if( this->m_ErrorObserver->ErrorOccured() )
      {
      this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
      this->m_ErrorObserver->ClearError();
      this->m_SerialCommunication->CloseCommunication();
      igstkPushInputMacro( Failed );
      }
    else   //attach the tools 
      {
      ToolAttachErrorObserver::Pointer attachErrorObserver = 
        ToolAttachErrorObserver::New();
      std::map<std::string, TrackerToolConfiguration *> toolConfigurations = 
        this->m_TrackerConfiguration->m_TrackerToolList;
                   //attach tools
      std::map<std::string, TrackerToolConfiguration *>::const_iterator it;
      std::map<std::string, TrackerToolConfiguration *>::const_iterator 
        toolConfigEnd = toolConfigurations.end();
      TrackerTool::Pointer currentTool;
      PolarisWirelessToolConfiguration * currentToolConfig;

      for( it = toolConfigurations.begin(); it != toolConfigEnd; it++ )
        {
        currentToolConfig = 
          static_cast<PolarisWirelessToolConfiguration *>( it->second );
        currentTool = InitializePolarisWirelessTool( currentToolConfig );
        this->m_Tools.insert(
          std::pair<std::string, TrackerTool::Pointer>( it->first, 
                                                        currentTool ) );
        unsigned long observerIDTool = currentTool->AddObserver(
          TrackerToolAttachmentToTrackerErrorEvent(),
          attachErrorObserver );
        currentTool->RequestAttachToTracker( tracker );
        currentTool->RemoveObserver( observerIDTool );
        if( attachErrorObserver->GotToolAttachError() )
          {
          igstkPushInputMacro( Failed );
          this->m_StateMachine.ProcessInputs();
          return;
          }
        }
                      //add the reference if we have one
      TrackerToolConfiguration* referenceToolConfiguration = 
        this->m_TrackerConfiguration->m_ReferenceTool;
      if( referenceToolConfiguration )
        {
        currentToolConfig = 
          static_cast<PolarisWirelessToolConfiguration *>
            ( referenceToolConfiguration );

        currentTool = InitializePolarisWirelessTool( currentToolConfig );
        this->m_ReferenceTool = currentTool;
        unsigned long observerIDRef = currentTool->AddObserver(
          TrackerToolAttachmentToTrackerErrorEvent(),
          attachErrorObserver );
        currentTool->RequestAttachToTracker( tracker );
        currentTool->RemoveObserver( observerIDRef );
        if( attachErrorObserver->GotToolAttachError() )
          {
          igstkPushInputMacro( Failed );
          this->m_StateMachine.ProcessInputs();
          return;
          }
        tracker->RequestSetReferenceTool( currentTool );
        }
      igstkPushInputMacro( Succeeded );
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
    //(TrackerOpenErrorEvent, TrackerInitializeErrorEvent)
    unsigned long observerID = tracker->AddObserver( IGSTKErrorEvent(),
                                                     this->m_ErrorObserver );
    tracker->SetCommunication( this->m_SerialCommunication );
    tracker->RequestOpen();
    tracker->RemoveObserver(observerID);

    if( this->m_ErrorObserver->ErrorOccured() )
      {
      this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
      this->m_ErrorObserver->ClearError();
      this->m_SerialCommunication->CloseCommunication();
      igstkPushInputMacro( Failed );
      }
    else   //attach the tools 
      {
      ToolAttachErrorObserver::Pointer attachErrorObserver = 
        ToolAttachErrorObserver::New();

      std::map<std::string, TrackerToolConfiguration *> toolConfigurations = 
        this->m_TrackerConfiguration->m_TrackerToolList;
      //attach tools
      std::map<std::string, TrackerToolConfiguration *>::const_iterator it;
      std::map<std::string, TrackerToolConfiguration *>::const_iterator 
        toolConfigEnd = toolConfigurations.end();
      TrackerTool::Pointer trackerTool;
      PolarisWirelessToolConfiguration * wirelessToolConfig;
      PolarisWiredToolConfiguration * wiredToolConfig;

      for( it = toolConfigurations.begin(); it != toolConfigEnd; it++ )
        {
        if( ( wirelessToolConfig = 
             dynamic_cast<PolarisWirelessToolConfiguration *>( it->second ) ) )
          {
          trackerTool = InitializePolarisWirelessTool( wirelessToolConfig );
          }
        else 
          {
          wiredToolConfig = 
            dynamic_cast<PolarisWiredToolConfiguration *>( it->second );
          trackerTool = InitializePolarisWiredTool( wiredToolConfig );
          }
        this->m_Tools.insert(
          std::pair<std::string, TrackerTool::Pointer>( it->first, 
                                                        trackerTool ) );

        unsigned long observerIDTool = trackerTool->AddObserver(
          TrackerToolAttachmentToTrackerErrorEvent(),
          attachErrorObserver );
        trackerTool->RequestAttachToTracker( tracker );
        trackerTool->RemoveObserver( observerIDTool );
        if( attachErrorObserver->GotToolAttachError() )
          {
          igstkPushInputMacro( Failed );
          this->m_StateMachine.ProcessInputs();
          return;
          }
        }
                      //add the reference if we have one
      TrackerToolConfiguration* referenceToolConfiguration = 
        this->m_TrackerConfiguration->m_ReferenceTool;
      if( referenceToolConfiguration )
        {
        if ( ( wirelessToolConfig = 
          dynamic_cast<PolarisWirelessToolConfiguration *>( 
            referenceToolConfiguration ) ) )
          {
          trackerTool = InitializePolarisWirelessTool( wirelessToolConfig );
          }
        else
          {
          wiredToolConfig = 
              dynamic_cast<PolarisWiredToolConfiguration *>( 
                referenceToolConfiguration );
          trackerTool = InitializePolarisWiredTool( wiredToolConfig );
          }
        this->m_ReferenceTool = trackerTool;
        unsigned long observerIDRef = trackerTool->AddObserver(
          TrackerToolAttachmentToTrackerErrorEvent(),
          attachErrorObserver );
        trackerTool->RequestAttachToTracker( tracker );
        trackerTool->RemoveObserver( observerIDRef );
        if( attachErrorObserver->GotToolAttachError() )
          {
          igstkPushInputMacro( Failed );
          this->m_StateMachine.ProcessInputs();
          return;
          }
        tracker->RequestSetReferenceTool( trackerTool );
        }
      igstkPushInputMacro( Succeeded );
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
    {
    trackerTool->RequestSelect5DOFTrackerTool();
    }
  else
    {
    trackerTool->RequestSelect6DOFTrackerTool();
    }

  // aurora internally handles [0-3] , so here we substract

  trackerTool->RequestSetPortNumber( toolConfiguration->GetPortNumber() - 1 );

  if( is5DOF )
    {
    trackerTool->RequestSetChannelNumber( 
      toolConfiguration->GetChannelNumber() );
    }

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

AscensionTrackerTool::Pointer 
TrackerController::InitializeAscensionTool(
    const AscensionToolConfiguration *toolConfiguration )
{
  AscensionTrackerTool::Pointer trackerTool = AscensionTrackerTool::New();

  trackerTool->RequestSetPortNumber( toolConfiguration->GetPortNumber() );

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
    //create tracker
    igstk::AscensionTracker::Pointer tracker = igstk::AscensionTracker::New();
    this->m_Tracker = tracker; 
    //don't need to observe this for errors because the 
    //configuration class ensures that the frequency is valid
    tracker->RequestSetFrequency( this->m_TrackerConfiguration->GetFrequency() );

    //observe all possible errors generated by the tracker
    unsigned long observerID = tracker->AddObserver( IGSTKErrorEvent(),
                                                     this->m_ErrorObserver );
    tracker->SetCommunication( this->m_SerialCommunication );
    tracker->RequestOpen();
    tracker->RemoveObserver(observerID);

    if( this->m_ErrorObserver->ErrorOccured() )
      {
      this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
      this->m_ErrorObserver->ClearError();
      this->m_SerialCommunication->CloseCommunication();
      igstkPushInputMacro( Failed );
      }
    else   //attach the tools 
      {
      ToolAttachErrorObserver::Pointer attachErrorObserver = 
        ToolAttachErrorObserver::New();

      std::map<std::string, TrackerToolConfiguration *> toolConfigurations = 
        this->m_TrackerConfiguration->m_TrackerToolList;
                   //attach tools
      std::map<std::string, TrackerToolConfiguration *>::const_iterator it;
      std::map<std::string, TrackerToolConfiguration *>::const_iterator 
        toolConfigEnd = toolConfigurations.end();
      TrackerTool::Pointer currentTool;
      AscensionToolConfiguration * currentToolConfig;

      for( it = toolConfigurations.begin(); it != toolConfigEnd; it++ )
        {
        currentToolConfig = 
          static_cast<AscensionToolConfiguration *>( it->second );
          currentTool = InitializeAscensionTool( currentToolConfig );
        this->m_Tools.insert(
          std::pair<std::string, TrackerTool::Pointer>( 
            it->first, currentTool ) );
        unsigned long observerIDTool = currentTool->AddObserver(
          TrackerToolAttachmentToTrackerErrorEvent(),
          attachErrorObserver );
        currentTool->RequestAttachToTracker( tracker );
        currentTool->RemoveObserver( observerIDTool );
        if( attachErrorObserver->GotToolAttachError() )
          {
          igstkPushInputMacro( Failed );
          this->m_StateMachine.ProcessInputs();
          return;
          }
        }
                      //add the reference if we have one
      TrackerToolConfiguration* referenceToolConfiguration = 
        this->m_TrackerConfiguration->m_ReferenceTool;
      if( referenceToolConfiguration )
        {
        currentToolConfig = 
          static_cast<AscensionToolConfiguration *>( 
            referenceToolConfiguration );
        currentTool = InitializeAscensionTool( currentToolConfig );
        this->m_ReferenceTool = currentTool;
        unsigned long observerIDRef = currentTool->AddObserver(
          TrackerToolAttachmentToTrackerErrorEvent(),
          attachErrorObserver );
        currentTool->RequestAttachToTracker( tracker );
        currentTool->RemoveObserver( observerIDRef );
        if( attachErrorObserver->GotToolAttachError() )
          {
          igstkPushInputMacro( Failed );
          this->m_StateMachine.ProcessInputs();
          return;
          }
        tracker->RequestSetReferenceTool( currentTool );
        }
      igstkPushInputMacro( Succeeded );
      }
    }  
  this->m_StateMachine.ProcessInputs();
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
    //(TrackerOpenErrorEvent, TrackerInitializeErrorEvent)
    unsigned long observerID = tracker->AddObserver( IGSTKErrorEvent(),
                                                     this->m_ErrorObserver );
    tracker->SetCommunication( this->m_SerialCommunication );
    tracker->RequestOpen();
    tracker->RemoveObserver( observerID );

    if( this->m_ErrorObserver->ErrorOccured() )
      {
      this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
      this->m_ErrorObserver->ClearError();
      this->m_SerialCommunication->CloseCommunication();
      igstkPushInputMacro( Failed );
      }
    else   //attach the tools 
      {
      ToolAttachErrorObserver::Pointer attachErrorObserver = 
        ToolAttachErrorObserver::New();

      std::map<std::string, TrackerToolConfiguration *> toolConfigurations = 
        this->m_TrackerConfiguration->m_TrackerToolList;
                   //attach tools
      std::map<std::string, TrackerToolConfiguration *>::const_iterator it;
      std::map<std::string, TrackerToolConfiguration *>::const_iterator 
        toolConfigEnd = toolConfigurations.end();
      TrackerTool::Pointer currentTool;
      AuroraToolConfiguration * currentToolConfig;

      for( it = toolConfigurations.begin(); it != toolConfigEnd; it++ )
        {
        currentToolConfig = 
          static_cast<AuroraToolConfiguration *>( it->second );
        currentTool = InitializeAuroraTool( currentToolConfig );
        this->m_Tools.insert(
          std::pair<std::string, TrackerTool::Pointer>( it->first, 
                                                        currentTool ) );
        
        unsigned long observerIDTool = currentTool->AddObserver(
          TrackerToolAttachmentToTrackerErrorEvent(),
          attachErrorObserver );
        currentTool->RequestAttachToTracker( tracker );
        currentTool->RemoveObserver( observerIDTool );
        if( attachErrorObserver->GotToolAttachError() )
          {
          igstkPushInputMacro( Failed );
          this->m_StateMachine.ProcessInputs();
          return;
          }
        }
                      //add the reference if we have one
      TrackerToolConfiguration* referenceToolConfiguration = 
        this->m_TrackerConfiguration->m_ReferenceTool;
      if( referenceToolConfiguration )
        {
        currentToolConfig = 
          static_cast<AuroraToolConfiguration *>( referenceToolConfiguration );

        currentTool = InitializeAuroraTool( currentToolConfig );
        this->m_ReferenceTool = currentTool;

        currentTool->AddObserver( 
          TrackerToolAttachmentToTrackerErrorEvent(),
          attachErrorObserver );
        currentTool->RequestAttachToTracker( tracker );
        if( attachErrorObserver->GotToolAttachError() )
          {
          igstkPushInputMacro( Failed );
          this->m_StateMachine.ProcessInputs();
          return;
          }
        tracker->RequestSetReferenceTool( currentTool );
        }
      igstkPushInputMacro( Succeeded );
      }
    }  
  this->m_StateMachine.ProcessInputs();
}


#ifdef IGSTK_USE_MicronTracker

MicronTrackerTool::Pointer TrackerController::InitializeMicronTool(
    const MicronToolConfiguration *toolConfiguration )
{
  MicronTrackerTool::Pointer trackerTool = MicronTrackerTool::New();
 
  trackerTool->RequestSetMarkerName( toolConfiguration->GetMarkerName() );
 
  trackerTool->SetCalibrationTransform( 
    toolConfiguration->GetCalibrationTransform() );
  trackerTool->RequestConfigure();
  return trackerTool;
}

#endif

#ifdef IGSTK_USE_Ascension3DGTracker

Ascension3DGTrackerTool::Pointer TrackerController::InitializeAscension3DGTool(
    const Ascension3DGToolConfiguration *toolConfiguration )
{
  Ascension3DGTrackerTool::Pointer trackerTool = Ascension3DGTrackerTool::New();
 
  trackerTool->SetCalibrationTransform( 
    toolConfiguration->GetCalibrationTransform() );
  trackerTool->RequestSetPortNumber(toolConfiguration->GetPortNumber());
  trackerTool->RequestConfigure();
  return trackerTool;
}

#endif



void TrackerController::MicronInitializeProcessing()
{
#ifdef IGSTK_USE_MicronTracker
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
  tracker->RemoveObserver(observerID);
  
  if( this->m_ErrorObserver->ErrorOccured() )
    {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    igstkPushInputMacro( Failed );
    }
  else   //attach the tools and start communication 
    {
    ToolAttachErrorObserver::Pointer attachErrorObserver = 
      ToolAttachErrorObserver::New();
    std::map<std::string, TrackerToolConfiguration *> toolConfigurations = 
        this->m_TrackerConfiguration->m_TrackerToolList;
                          //attach tools
    std::map<std::string, TrackerToolConfiguration *>::const_iterator it;
    std::map<std::string, TrackerToolConfiguration *>::const_iterator 
      toolConfigEnd = toolConfigurations.end();
    TrackerTool::Pointer trackerTool;
    MicronToolConfiguration * currentToolConfig;

    for(it = toolConfigurations.begin(); it != toolConfigEnd; it++)
      {
      currentToolConfig = static_cast<MicronToolConfiguration *>(it->second);

      trackerTool = InitializeMicronTool( currentToolConfig );
      this->m_Tools.insert(
          std::pair<std::string, TrackerTool::Pointer>( it->first, 
                                                        trackerTool ) );

      unsigned long observerID = trackerTool->AddObserver( 
        TrackerToolAttachmentToTrackerErrorEvent(),
        attachErrorObserver );
      trackerTool->RequestAttachToTracker( tracker );
      trackerTool->RemoveObserver( observerID ); 
      if( attachErrorObserver->GotToolAttachError() )
        {
        igstkPushInputMacro( Failed );
        this->m_StateMachine.ProcessInputs();
        return;
        }
      }
                      //add the reference if we have one
    TrackerToolConfiguration* referenceToolConfiguration = 
      this->m_TrackerConfiguration->m_ReferenceTool;
    if( referenceToolConfiguration )
      {
      currentToolConfig = 
        static_cast<MicronToolConfiguration *>( referenceToolConfiguration );

      trackerTool = InitializeMicronTool( currentToolConfig );   
      this->m_ReferenceTool = trackerTool;

      trackerTool->AddObserver( 
        TrackerToolAttachmentToTrackerErrorEvent(),
        attachErrorObserver );      
      trackerTool->RequestAttachToTracker( tracker );
      if( attachErrorObserver->GotToolAttachError() )
        {
        igstkPushInputMacro( Failed );
        this->m_StateMachine.ProcessInputs();
        return;
        }
      tracker->RequestSetReferenceTool( trackerTool );
      }
    igstkPushInputMacro( Succeeded );
    }
  this->m_StateMachine.ProcessInputs();
#else
  igstkPushInputMacro( Failed );
  this->m_StateMachine.ProcessInputs();
#endif
}

void TrackerController::Ascension3DGInitializeProcessing()
{
#ifdef IGSTK_USE_Ascension3DGTracker
                  //create tracker
  igstk::Ascension3DGTracker::Pointer tracker = igstk::Ascension3DGTracker::New();
  this->m_Tracker = tracker; 
                 //don't need to observe this for errors because the 
                 //configuration class ensures that the frequency is valid
  tracker->RequestSetFrequency( this->m_TrackerConfiguration->GetFrequency() );

  Ascension3DGTrackerConfiguration *trackerConfiguration =
    dynamic_cast<Ascension3DGTrackerConfiguration *>( this->m_TrackerConfiguration );

  unsigned long observerID = tracker->AddObserver( IGSTKErrorEvent(),
                                                   this->m_ErrorObserver );
  tracker->RequestOpen();
  tracker->RemoveObserver(observerID);
  
  if( this->m_ErrorObserver->ErrorOccured() )
    {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    igstkPushInputMacro( Failed );
    }
  else   //attach the tools and start communication 
    {
    ToolAttachErrorObserver::Pointer attachErrorObserver = 
      ToolAttachErrorObserver::New();
    std::map<std::string, TrackerToolConfiguration *> toolConfigurations = 
        this->m_TrackerConfiguration->m_TrackerToolList;
                          //attach tools
    std::map<std::string, TrackerToolConfiguration *>::const_iterator it;
    std::map<std::string, TrackerToolConfiguration *>::const_iterator 
      toolConfigEnd = toolConfigurations.end();
    TrackerTool::Pointer trackerTool;
    Ascension3DGToolConfiguration * currentToolConfig;

    for(it = toolConfigurations.begin(); it != toolConfigEnd; it++)
      {
      currentToolConfig = static_cast<Ascension3DGToolConfiguration *>(it->second);

      trackerTool = InitializeAscension3DGTool( currentToolConfig );
      this->m_Tools.insert(
          std::pair<std::string, TrackerTool::Pointer>( it->first, 
                                                        trackerTool ) );

      unsigned long observerID = trackerTool->AddObserver( 
        TrackerToolAttachmentToTrackerErrorEvent(),
        attachErrorObserver );
      trackerTool->RequestAttachToTracker( tracker );
      trackerTool->RemoveObserver( observerID ); 
      if( attachErrorObserver->GotToolAttachError() )
        {
        igstkPushInputMacro( Failed );
        this->m_StateMachine.ProcessInputs();
        return;
        }
      }
                      //add the reference if we have one
    TrackerToolConfiguration* referenceToolConfiguration = 
      this->m_TrackerConfiguration->m_ReferenceTool;
    if( referenceToolConfiguration )
      {
      currentToolConfig = 
        static_cast<Ascension3DGToolConfiguration *>( referenceToolConfiguration );

      trackerTool = InitializeAscension3DGTool( currentToolConfig );   
      this->m_ReferenceTool = trackerTool;

      trackerTool->AddObserver( 
        TrackerToolAttachmentToTrackerErrorEvent(),
        attachErrorObserver );      
      trackerTool->RequestAttachToTracker( tracker );
      if( attachErrorObserver->GotToolAttachError() )
        {
        igstkPushInputMacro( Failed );
        this->m_StateMachine.ProcessInputs();
        return;
        }
      tracker->RequestSetReferenceTool( trackerTool );
      }
    igstkPushInputMacro( Succeeded );
    }
  this->m_StateMachine.ProcessInputs();
#else
  igstkPushInputMacro( Failed );
  this->m_StateMachine.ProcessInputs();
#endif
}

#ifdef IGSTK_USE_NDICertusTracker

NDICertusTrackerTool::Pointer
TrackerController::InitializeCertusTool(
  const CertusToolConfiguration *toolConfiguration )
{
  NDICertusTrackerTool::Pointer trackerTool = NDICertusTrackerTool::New();
  trackerTool->RequestSetRigidBodyName(toolConfiguration->GetRigidBodyName());
  trackerTool->RequestConfigure();
  return trackerTool;
}
#endif

void TrackerController::CertusInitializeProcessing()
{

#ifdef IGSTK_USE_NDICertusTracker

	//create tracker
  igstk::NDICertusTracker::Pointer tracker = igstk::NDICertusTracker::New();
  this->m_Tracker = tracker; 
                 //don't need to observe this for errors because the 
                 //configuration class ensures that the frequency is valid
  tracker->RequestSetFrequency( this->m_TrackerConfiguration->GetFrequency() );

  CertusTrackerConfiguration *trackerConfiguration =
    dynamic_cast<CertusTrackerConfiguration *>( this->m_TrackerConfiguration );
  
  
  tracker->SetIniFileName(trackerConfiguration->GetSetupFile());

  //tracker->SetCfgFileName(trackerConfiguration->GetRigidCfgFile());

  tracker->rigidBodyStatus.lnRigidBodies = atoi(trackerConfiguration->GetNumberOfRigidBodies().c_str());

  std::map<std::string, TrackerToolConfiguration *> toolConfigurations = 
        this->m_TrackerConfiguration->m_TrackerToolList;
  
  int i=0;
  std::map<std::string, TrackerToolConfiguration *>::const_iterator it;
  std::map<std::string, TrackerToolConfiguration *>::const_iterator   toolConfigEnd = toolConfigurations.end();
  CertusToolConfiguration * currentToolConfig;
  for(it = toolConfigurations.begin(); it != toolConfigEnd; it++)
      {
      currentToolConfig = static_cast<CertusToolConfiguration *>(it->second);
	  tracker->rigidBodyDescrArray[i].lnStartMarker = atoi(currentToolConfig->GetStartMarker().c_str());
	  tracker->rigidBodyDescrArray[i].lnNumberOfMarkers = atoi(currentToolConfig->GetNumberOfMarkers().c_str());
	  strcpy(tracker->rigidBodyDescrArray[i].szName, currentToolConfig->GetRigidBodyName().c_str());
	  i++;
  }

  unsigned long observerID = tracker->AddObserver( IGSTKErrorEvent(),
                                                   this->m_ErrorObserver );

  tracker->RequestOpen( );
  tracker->RemoveObserver(observerID);
  
  if( this->m_ErrorObserver->ErrorOccured() )
    {
    this->m_ErrorObserver->GetErrorMessage( this->m_ErrorMessage );
    this->m_ErrorObserver->ClearError();
    igstkPushInputMacro( Failed );
    }
  else   //attach the tools and start communication 
    {
    ToolAttachErrorObserver::Pointer attachErrorObserver = 
      ToolAttachErrorObserver::New();
    std::map<std::string, TrackerToolConfiguration *> toolConfigurations = 
        this->m_TrackerConfiguration->m_TrackerToolList;
                          //attach tools
    std::map<std::string, TrackerToolConfiguration *>::const_iterator it;
    std::map<std::string, TrackerToolConfiguration *>::const_iterator 
      toolConfigEnd = toolConfigurations.end();
    TrackerTool::Pointer trackerTool;
    CertusToolConfiguration * currentToolConfig;

    for(it = toolConfigurations.begin(); it != toolConfigEnd; it++)
      {
      currentToolConfig = static_cast<CertusToolConfiguration *>(it->second);

      trackerTool = InitializeCertusTool( currentToolConfig );
      this->m_Tools.insert(
          std::pair<std::string, TrackerTool::Pointer>( it->first, 
                                                        trackerTool ) );

      unsigned long observerID = trackerTool->AddObserver( 
        TrackerToolAttachmentToTrackerErrorEvent(),
        attachErrorObserver );
      trackerTool->RequestAttachToTracker( tracker );
      trackerTool->RemoveObserver( observerID ); 
      if( attachErrorObserver->GotToolAttachError() )
        {
        this->m_ErrorMessage = "Failed to connect tool to tracker.";
        igstkPushInputMacro( Failed );
        this->m_StateMachine.ProcessInputs();
        return;
        }
      }
                      //add the reference if we have one
    TrackerToolConfiguration* referenceToolConfiguration = 
      this->m_TrackerConfiguration->m_ReferenceTool;
    if( referenceToolConfiguration )
      {
      currentToolConfig = 
        static_cast<CertusToolConfiguration *>( referenceToolConfiguration );

      trackerTool = InitializeCertusTool( currentToolConfig );   
      this->m_ReferenceTool = trackerTool;

      trackerTool->AddObserver( 
        TrackerToolAttachmentToTrackerErrorEvent(),
        attachErrorObserver );      
      trackerTool->RequestAttachToTracker( tracker );

      if( attachErrorObserver->GotToolAttachError() )
        {
        this->m_ErrorMessage = "Failed to connect tool to tracker.";
        igstkPushInputMacro( Failed );
        this->m_StateMachine.ProcessInputs();
        return;
        }
        tracker->RequestSetReferenceTool( trackerTool );
      }
    igstkPushInputMacro( Succeeded );
    }
  this->m_StateMachine.ProcessInputs();
#else
  igstkPushInputMacro( Failed );
  this->m_StateMachine.ProcessInputs();
#endif
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
TrackerController::GetToolProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "GetToolProcessing called...\n");  

  RequestToolEvent sevt;
  RequestToolErrorEvent fevt;
   
  std::map<std::string, TrackerTool::Pointer>::iterator it =
    this->m_Tools.find( this->m_RequestedToolName );
 
  this->m_RequestedToolName.clear();

  if( it == this->m_Tools.end() ) 
    {  
    this->InvokeEvent( fevt );
    }
  else
    {
    sevt.Set( *it );
    this->InvokeEvent( sevt );
    }
}


void 
TrackerController::GetReferenceToolProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "GetReferenceToolProcessing called...\n");
  RequestToolEvent evt;
  RequestToolErrorEvent fevt;
  //we don't have a reference
  if( this->m_ReferenceTool.IsNull() )
    {
    this->InvokeEvent( fevt );
    return;
    }

  evt.Set( std::pair<std::string, igstk::TrackerTool::Pointer>( 
           "reference",
           this->m_ReferenceTool ) );
  this->InvokeEvent( evt );
}

void 
TrackerController::SetParentSpatialObjectProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "SetParentSpatialObjectProcessing called...\n");
 
  this->m_Tracker->RequestDetachFromParent();
  this->m_Tracker->RequestSetTransformAndParent(m_TmpTransform, 
                                                m_TmpSpatialObject);
}

void 
TrackerController::SetChildSpatialObjectProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "SetParentSpatialObjectProcessing called...\n");
 
  m_TmpSpatialObject->RequestSetTransformAndParent(m_TmpTransform, 
                                                   this->m_Tracker);
}

void 
TrackerController::ReportInitializationSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "ReportInitializationSuccess called...\n");
  this->m_Tracker->AddObserver( igstk::TrackerUpdateStatusEvent(),
                                this->m_TrackerUpdateStatusObserver );
  this->m_Tracker->AddObserver( igstk::TrackerUpdateStatusErrorEvent(),
                                this->m_TrackerUpdateStatusObserver );  
  this->InvokeEvent( InitializeEvent() );
}


void
TrackerController::ReportInitializationFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "ReportInitializationFailureProcessing called...\n");
  InitializeErrorEvent evt; 
  evt.Set( this->m_ErrorMessage );
  this->InvokeEvent( evt );
}


void 
TrackerController::ReportStartTrackingSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "ReportStartTrackingSuccessProcessing called...\n");
  this->InvokeEvent( TrackerStartTrackingEvent() );
}


void 
TrackerController::ReportStartTrackingFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "ReportStartTrackingFailureProcessing called...\n");
  TrackerStartTrackingErrorEvent evt; 
  evt.Set( this->m_ErrorMessage );
  this->InvokeEvent( evt );
}


void 
TrackerController::ReportStopTrackingSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "ReportStopTrackingSuccessProcessing called...\n");
  this->InvokeEvent( TrackerStopTrackingEvent() );
}


void 
TrackerController::ReportStopTrackingFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "ReportStopTrackingFailureProcessing called...\n");
  TrackerStopTrackingErrorEvent evt; 
  evt.Set( this->m_ErrorMessage );
  this->InvokeEvent( evt );
}


void 
TrackerController::ReportCloseCommunicationFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "ReportCloseCommunicationFailureProcessing called...\n");
  CloseCommunicationErrorEvent evt; 
  evt.Set( this->m_ErrorMessage );
  this->InvokeEvent( evt );
}


void 
TrackerController::ReportCloseCommunicationSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::TrackerController::"
                  "ReportCloseCommunicationSuccessProcessing called...\n");
  this->InvokeEvent( CloseCommunicationEvent() );
}


void 
TrackerController::ReportInvalidRequestProcessing()
{
  igstkLogMacro( DEBUG, "igstk::TrackerController::"
                 "ReportInvalidRequestProcessing called...\n");
  this->InvokeEvent(InvalidRequestErrorEvent());
}


TrackerController::ErrorObserver::ErrorObserver() : m_ErrorOccured( false )
{ //serial communication errors
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
    std::pair<std::string,std::string>( 
      TrackerInitializeErrorEvent().GetEventName(),
      "Error initializing tracker." ) );
  this->m_ErrorEvent2ErrorMessage.insert(
    std::pair<std::string,std::string>( 
      TrackerStartTrackingErrorEvent().GetEventName(),
      "Error starting tracking." ) );
  this->m_ErrorEvent2ErrorMessage.insert(
    std::pair<std::string,std::string>( 
      TrackerStopTrackingErrorEvent().GetEventName(),
      "Error stopping tracking." ) );
  this->m_ErrorEvent2ErrorMessage.insert(
    std::pair<std::string,std::string>( 
      TrackerCloseErrorEvent().GetEventName(),
      "Error closing tracker communication." ) );
}

void 
TrackerController::ErrorObserver::Execute( 
  const itk::Object * itkNotUsed(caller), 
  const itk::EventObject & event ) throw (std::exception)
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
TrackerController::ErrorObserver::Execute( 
  itk::Object *caller, 
  const itk::EventObject & event ) throw (std::exception)
{
  const itk::Object * constCaller = caller;
  this->Execute(constCaller, event);
}


void
TrackerController::TrackerUpdateObserver::SetParent( TrackerController *parent )
{
  this->m_parent = parent;
}


void
TrackerController::TrackerUpdateObserver::Execute( 
  const itk::Object * itkNotUsed(caller), 
  const itk::EventObject & event ) throw ( std::exception )
{
  if( igstk::TrackerUpdateStatusEvent().CheckEvent( &event ) ||
      igstk::TrackerUpdateStatusErrorEvent().CheckEvent( &event ) )
    {
    this->m_parent->InvokeEvent( event );
    }
}


void
TrackerController::TrackerUpdateObserver::Execute( 
  itk::Object *caller, 
  const itk::EventObject & event ) throw ( std::exception )
{
  const itk::Object * constCaller = caller;
  this->Execute(constCaller, event);
}

}//end namespace igstk
