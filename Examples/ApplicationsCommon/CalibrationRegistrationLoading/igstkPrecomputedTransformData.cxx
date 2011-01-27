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

namespace igstk
{ 

PrecomputedTransformData::PrecomputedTransformData() : 
m_StateMachine( this )
{
  //define the state machine's states 
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( Initialized );

  //define the state machines inputs
  igstkAddInputMacro( Initialize );
  igstkAddInputMacro( GetTransform  );
  igstkAddInputMacro( GetEstimationError  );
  igstkAddInputMacro( GetDescription  );
  igstkAddInputMacro( GetDate  );

  //define the state machine's transitions
  igstkAddTransitionMacro( Idle,
                           Initialize,
                           Initialized,
                           Initialize );
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

  igstkAddTransitionMacro( Initialized,
                           Initialize,
                           Initialized,
                           Initialize );
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
  this->m_Transform = this->m_TmpTransform;
  this->m_TmpTransform = NULL;
  this->m_TransformationError = this->m_TmpTransformationError;
  this->m_TransformationDescription = this->m_TmpTransformationDescription;
  this->m_TmpTransformationDescription.clear();
  this->m_ComputationDateAndTime = this->m_TmpComputationDateAndTime;
  this->m_TmpComputationDateAndTime.clear();
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
                                     itk::Indent itkNotUsed(indent) ) const
{
  os<<this->m_TransformationDescription<<"("<<this->m_ComputationDateAndTime;
  os<<"):\n";
  os<<this->m_Transform;
  os<<"Error: "<<this->m_TransformationError<<"\n";
}

}
