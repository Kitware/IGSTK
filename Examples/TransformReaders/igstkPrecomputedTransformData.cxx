/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPrecomputedTransformData.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkPrecomputedTransformData.h"
#include "igstkTransform.h"
#include "igstkAffineTransform.h"
#include "igstkPerspectiveTransform.h"

namespace igstk
{ 

PrecomputedTransformData::PrecomputedTransformData() : 
m_StateMachine( this ), m_TmpTransform( NULL ), m_Transform( NULL ) 
{
  //define the state machine's states 
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( AttemptingToInitialize );
  igstkAddStateMacro( Initialized );

  //define the state machines inputs
  igstkAddInputMacro( Initialize );
  igstkAddInputMacro( Failed  );
  igstkAddInputMacro( Succeeded  );
  igstkAddInputMacro( GetTransform  );
  igstkAddInputMacro( GetEstimationError  );
  igstkAddInputMacro( GetDescription  );
  igstkAddInputMacro( GetDate  );

  //define the state machine's transitions
  igstkAddTransitionMacro( Idle,
                           Initialize,
                           AttemptingToInitialize,
                           Initialize );
  igstkAddTransitionMacro( Idle,
                           Failed,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           Succeeded,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           GetTransform,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           GetEstimationError,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           GetDescription,
                           Idle,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Idle,
                           GetDate,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToInitialize,
                           Initialize,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           Failed,
                           Idle,
                           ReportInitializationFailure );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           Succeeded,
                           Initialized,
                           ReportInitializationSuccess );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           GetTransform,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           GetEstimationError,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           GetDescription,
                           AttemptingToInitialize,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingToInitialize,
                           GetDate,
                           AttemptingToInitialize,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Initialized,
                           Initialize,
                           AttemptingToInitialize,
                           Initialize );
  igstkAddTransitionMacro( Initialized,
                           Failed,
                           Initialized,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Initialized,
                           Succeeded,
                           Initialized,
                           ReportInvalidRequest );
  igstkAddTransitionMacro( Initialized,
                           GetTransform,
                           Initialized,
                           GetTransform );
  igstkAddTransitionMacro( Initialized,
                           GetEstimationError,
                           Initialized,
                           GetEstimationError );
  igstkAddTransitionMacro( Initialized,
                           GetDescription,
                           Initialized,
                           GetDescription );
  igstkAddTransitionMacro( Initialized,
                           GetDate,
                           Initialized,
                           GetDate );

  //set the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

  // done setting the state machine, ready to run
  this->m_StateMachine.SetReadyToRun();  
}


PrecomputedTransformData::~PrecomputedTransformData()
{
  delete this->m_Transform;
}


void 
PrecomputedTransformData::RequestInitialize( 
                          TransformType *transform,
                          const DateType &date,
                          const std::string &description,
                          const ErrorType err )
{
  igstkLogMacro( DEBUG, "igstk::PrecomputedTransformData::"
                 "RequestInitialize called...\n" );
  this->m_TmpTransform = transform;
  this->m_TmpTransformationError = err;
  this->m_TmpTransformationDescription = description;
  this->m_TmpComputationDateAndTime = date;

  igstkPushInputMacro( Initialize );
  this->m_StateMachine.ProcessInputs();
}


void 
PrecomputedTransformData::RequestTransform() 
{
  igstkLogMacro( DEBUG, "igstk::PrecomputedTransformData::"
                 "RequestTransform called...\n" );
  igstkPushInputMacro( GetTransform );
  this->m_StateMachine.ProcessInputs();
}


void 
PrecomputedTransformData::RequestEstimationError() 
{
  igstkLogMacro( DEBUG, "igstk::PrecomputedTransformData::"
                 "RequestEstimationError called...\n" );
  igstkPushInputMacro( GetEstimationError );
  this->m_StateMachine.ProcessInputs();
}


void 
PrecomputedTransformData::RequestTransformDescription() 
{
  igstkLogMacro( DEBUG, "igstk::PrecomputedTransformData::"
                 "RequestTransformDescription called...\n" );
  igstkPushInputMacro( GetDescription );
  this->m_StateMachine.ProcessInputs();
}


void 
PrecomputedTransformData::RequestComputationDateAndTime() 
{
  igstkLogMacro( DEBUG, "igstk::PrecomputedTransformData::"
                 "RequestComputationDateAndTime called...\n" );
  igstkPushInputMacro( GetDate );
  this->m_StateMachine.ProcessInputs();
}


void  
PrecomputedTransformData::ReportInvalidRequestProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PrecomputedTransformData::"
                 "ReportInvalidRequestProcessing called...\n" );
  this->InvokeEvent( InvalidRequestErrorEvent() );
}


void  
PrecomputedTransformData::InitializeProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PrecomputedTransformData::"
                 "InitializeProcessing called...\n" );
  
  if( this->m_TmpTransform != NULL)
  {
      //create a copy of the transformation
   Transform *t;
   AffineTransform *at;
   PerspectiveTransform *pt;
   
   this->m_Transform = NULL;
   if( ( t = dynamic_cast<Transform *>(this->m_TmpTransform) ) != NULL )
     this->m_Transform = new Transform(*t);
   else if( ( at = dynamic_cast<AffineTransform *>(this->m_TmpTransform) ) != NULL )
     this->m_Transform = new AffineTransform(*at);
   else if( ( pt = dynamic_cast<PerspectiveTransform *>(this->m_TmpTransform) ) != NULL )
     this->m_Transform = new PerspectiveTransform(*pt);
   if( this->m_Transform != NULL )
     {
     this->m_TmpTransform = NULL;
     this->m_TransformationError = this->m_TmpTransformationError;
     this->m_TransformationDescription = this->m_TmpTransformationDescription;
     this->m_TmpTransformationDescription.clear();
     this->m_ComputationDateAndTime = this->m_TmpComputationDateAndTime;
     this->m_TmpComputationDateAndTime.clear();
     igstkPushInputMacro( Succeeded );
     }
   else
     {
     igstkPushInputMacro( Failed );
     }
  }
  else
    {
    igstkPushInputMacro( Failed );
    }
  this->m_StateMachine.ProcessInputs();
}


void 
PrecomputedTransformData::ReportInitializationFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PrecomputedTransformData::"
                  "ReportInitializationFailureProcessing called...\n");
  this->InvokeEvent( InitializationFailureEvent() );
}


void 
PrecomputedTransformData::ReportInitializationSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PrecomputedTransformData::"
                  "ReportInitializationSuccessProcessing called...\n");
  this->InvokeEvent( InitializationSuccessEvent() );
}


void  
PrecomputedTransformData::GetTransformProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PrecomputedTransformData::"
                 "GetTransformProcessing called...\n" );
  TransformTypeEvent evt;
  evt.Set( this->m_Transform );
  this->InvokeEvent( evt );
}
  

void  
PrecomputedTransformData::GetEstimationErrorProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PrecomputedTransformData::"
                 "GetEstimationErrorProcessing called...\n" );
  TransformErrorTypeEvent evt;
  evt.Set( this->m_TransformationError );
  this->InvokeEvent( evt );
}

  
void  
PrecomputedTransformData::GetDescriptionProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PrecomputedTransformData::"
                 "GetDescriptionProcessing called...\n" );
  StringEvent evt;
  evt.Set( this->m_TransformationDescription );
  this->InvokeEvent( evt );
}


void  
PrecomputedTransformData::GetDateProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PrecomputedTransformData::"
                 "GetDateProcessing called...\n" );
  TransformDateTypeEvent evt;
  evt.Set( this->m_ComputationDateAndTime );
  this->InvokeEvent( evt );
}


void  
PrecomputedTransformData::PrintSelf( std::ostream& os, 
                                     itk::Indent indent ) const
{
  os<< indent << this->m_TransformationDescription<<"("<<this->m_ComputationDateAndTime;
  os<<"):\n";
  os<< indent << this->m_Transform;
  os<< indent << "Error: "<<this->m_TransformationError<<"\n";
}

}
