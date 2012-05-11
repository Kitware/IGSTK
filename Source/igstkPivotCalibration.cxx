/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPivotCalibration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkPivotCalibration.h"
#include "igstkTransformObserver.h"

/*
#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#pragma warning( disable : 4786 )
#endif
*/

namespace igstk
{ 

PivotCalibration::PivotCalibration() : m_StateMachine( this )
{
              //instantiate the class that performs the pivot calibration 
              //computation
  this->m_PivotCalibrationAlgorithm = PivotCalibrationAlgorithm::New();
              //observer for error's which use specific error messages 
  this->m_ErrorObserver = ErrorObserver::New();
  this->m_PivotCalibrationAlgorithm->AddObserver( 
                           PivotCalibrationAlgorithm::CalibrationFailureEvent(),
                                                  this->m_ErrorObserver );

                    //create the observers for all the requests that are 
                    //forwarded to the PivotCalibrationAlgorithm
  this->m_GetCalibrationTransformObserver = CalibrationTransformObserver::New();
  this->m_PivotCalibrationAlgorithm->AddObserver( 
                                             CoordinateSystemTransformToEvent(),
                                      this->m_GetCalibrationTransformObserver );
  this->m_GetPivotPointObserver = PivotPointObserver::New(); 
  this->m_PivotCalibrationAlgorithm->AddObserver( PointEvent() , 
                                                this->m_GetPivotPointObserver );
  this->m_GetCalibrationRMSEObserver = CalibrationRMSEObserver::New(); 
  this->m_PivotCalibrationAlgorithm->AddObserver( DoubleTypeEvent() , 
                                           this->m_GetCalibrationRMSEObserver );

        //setup the transformation acquired observer using class method
  this->m_TransformObserver = TransformToTrackerObserver::New();

        //define the state machine's states 
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( AttemptingToInitialize );
  igstkAddStateMacro( Initialized );
  igstkAddStateMacro( AttemptingToComputeCalibration );
  igstkAddStateMacro( CalibrationComputed );

                   //define the state machine's inputs
  igstkAddInputMacro( Initialize );
  igstkAddInputMacro( Failed  );
  igstkAddInputMacro( Succeeded  );
  igstkAddInputMacro( ComputeCalibration );
  igstkAddInputMacro( GetTransform );
  igstkAddInputMacro( GetPivotPoint  );
  igstkAddInputMacro( GetRMSE  );

            //define the state machine's transitions
                         //transitions from Idle state
  igstkAddTransitionMacro(Idle,
                          Initialize,
                          AttemptingToInitialize,
                          Initialize);

  igstkAddTransitionMacro(Idle,
                          Failed,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(Idle,
                          Succeeded,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Idle,
                          ComputeCalibration,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Idle,
                          GetTransform,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Idle,
                          GetPivotPoint,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Idle,
                          GetRMSE,
                          Idle,
                          ReportInvalidRequest);
  
                         //transitions from AttemptingToInitialize state
  igstkAddTransitionMacro(AttemptingToInitialize,
                          Initialize,
                          AttemptingToInitialize,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          Failed,
                          Idle,
                          ReportInitializationFailure);
  
  igstkAddTransitionMacro(AttemptingToInitialize,
                          Succeeded,
                          Initialized,
                          ReportInitializationSuccess);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          ComputeCalibration,
                          AttemptingToInitialize,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          GetTransform,
                          AttemptingToInitialize,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          GetPivotPoint,
                          AttemptingToInitialize,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToInitialize,
                          GetRMSE,
                          AttemptingToInitialize,
                          ReportInvalidRequest);
  
                         //transitions from Initialized state
  igstkAddTransitionMacro(Initialized,
                          Initialize,
                          AttemptingToInitialize,
                          Initialize);

  igstkAddTransitionMacro(Initialized,
                          Failed,
                          Initialized,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(Initialized,
                          Succeeded,
                          Initialized,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Initialized,
                          ComputeCalibration,
                          AttemptingToComputeCalibration,
                          ComputeCalibration);

  igstkAddTransitionMacro(Initialized,
                          GetTransform,
                          Initialized,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Initialized,
                          GetPivotPoint,
                          Initialized,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(Initialized,
                          GetRMSE,
                          Initialized,
                          ReportInvalidRequest);
  
                     //transitions from AttemptingToComputeCalibration state
  igstkAddTransitionMacro(AttemptingToComputeCalibration,
                          Initialize,
                          AttemptingToComputeCalibration,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToComputeCalibration,
                          Failed,
                          Initialized,
                          ReportCalibrationComputationFailure);
  
  igstkAddTransitionMacro(AttemptingToComputeCalibration,
                          Succeeded,
                          CalibrationComputed,
                          ReportCalibrationComputationSuccess);

  igstkAddTransitionMacro(AttemptingToComputeCalibration,
                          ComputeCalibration,
                          AttemptingToComputeCalibration,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToComputeCalibration,
                          GetTransform,
                          AttemptingToComputeCalibration,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToComputeCalibration,
                          GetPivotPoint,
                          AttemptingToComputeCalibration,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToComputeCalibration,
                          GetRMSE,
                          AttemptingToComputeCalibration,
                          ReportInvalidRequest);
  
                         //transitions from CalibrationComputed state
  igstkAddTransitionMacro(CalibrationComputed,
                          Initialize,
                          AttemptingToInitialize,
                          Initialize);

  igstkAddTransitionMacro(CalibrationComputed,
                          Failed,
                          CalibrationComputed,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(CalibrationComputed,
                          Succeeded,
                          CalibrationComputed,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(CalibrationComputed,
                          ComputeCalibration,
                          AttemptingToComputeCalibration,
                          ComputeCalibration);

  igstkAddTransitionMacro(CalibrationComputed,
                          GetTransform,
                          CalibrationComputed,
                          GetTransform);

  igstkAddTransitionMacro(CalibrationComputed,
                          GetPivotPoint,
                          CalibrationComputed,
                          GetPivotPoint);

  igstkAddTransitionMacro(CalibrationComputed,
                          GetRMSE,
                          CalibrationComputed,
                          GetRMSE);
  
             //set the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

         // done setting the state machine, ready to run
  this->m_StateMachine.SetReadyToRun();
} 


PivotCalibration::~PivotCalibration()  
{

}

void 
PivotCalibration::RequestInitialize( unsigned int n, 
                                    igstk::TrackerTool::Pointer trackerTool)
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::"
                 "RequestInitialize called...\n");
  this->m_TmpRequiredNumberOfTransformations = n;
  this->m_TmpTrackerTool = trackerTool;
  igstkPushInputMacro( Initialize );
  this->m_StateMachine.ProcessInputs();
}
  
void 
PivotCalibration::RequestComputeCalibration()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::"
                 "RequestComputeCalibration called...\n");
  igstkPushInputMacro( ComputeCalibration );
  this->m_StateMachine.ProcessInputs();
}

void 
PivotCalibration::RequestCalibrationTransform()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::"
                 "RequestCalibrationTransform called...\n");
  igstkPushInputMacro( GetTransform );
  this->m_StateMachine.ProcessInputs();
}

void 
PivotCalibration::RequestPivotPoint()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::"
                 "RequestPivotPoint called...\n");
  igstkPushInputMacro( GetPivotPoint );
  this->m_StateMachine.ProcessInputs();  
}

void 
PivotCalibration::RequestCalibrationRMSE()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::"
                 "RequestCalibrationTransformRMSE called...\n");
  igstkPushInputMacro( GetRMSE );
  this->m_StateMachine.ProcessInputs();  
}


void 
PivotCalibration::ReportInvalidRequestProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::"
                 "ReportInvalidRequestProcessing called...\n");
  this->InvokeEvent(InvalidRequestErrorEvent());
}

void 
PivotCalibration::InitializeProcessing()
{
  if( this->m_TmpTrackerTool.IsNull() )
    {
    igstkPushInputMacro( Failed );
    }
  else 
    {
    this->m_TrackerTool = this->m_TmpTrackerTool;
    this->m_RequiredNumberOfTransformations = 
                                     this->m_TmpRequiredNumberOfTransformations;
    this->m_Transforms.clear();
    this->m_PivotCalibrationAlgorithm->RequestResetCalibration();
    igstkPushInputMacro( Succeeded );
    }
}

void 
PivotCalibration::ReportInitializationFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibration::"
                  "ReportInitializationFailureProcessing called...\n");
  this->InvokeEvent( InitializationFailureEvent() );
}
  
void 
PivotCalibration::ReportInitializationSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibration::"
                  "ReportInitializationSuccessProcessing called...\n");
  this->InvokeEvent( InitializationSuccessEvent() );
}

void 
PivotCalibration::ComputeCalibrationProcessing()
{
  this->m_Transforms.clear();
  this->m_ReasonForCalibrationFailure.clear();
  this->InvokeEvent( DataAcquisitionStartEvent() );

  this->m_TransformAcquiredObserver = TransformAcquiredCommand::New();

  this->m_TrackerTool->AddObserver( igstk::TrackerToolTransformUpdateEvent(),
                                            this->m_TransformAcquiredObserver );

  this->m_TransformAcquiredObserver->SetCallbackFunction( this, 
                             &PivotCalibration::AcquireTransformsAndCalibrate );

  this->m_TransformToTrackerObserverID = 
      this->m_TrackerTool->AddObserver( CoordinateSystemTransformToEvent() ,
                                                   this->m_TransformObserver );
}


void 
PivotCalibration::AcquireTransformsAndCalibrate(
                                              itk::Object * itkNotUsed(caller),
                                   const itk::EventObject & itkNotUsed(event) )
{  
  //got all the transformations we need for calibration
  if( this->m_Transforms.size() == this->m_RequiredNumberOfTransformations )
    {
    // Instead of removing the observer, we set the callback function to empty
    // because that the tracker is running on a separate thread, when the 
    // tracker update event evoke the observer callback, it will crash the 
    // application if the observer is being removed by another thread.Thus it is
    // safer to  set the observer callback to an empty function
    this->m_TransformAcquiredObserver->SetCallbackFunction(this,
                                             & PivotCalibration::EmptyCallBack);
    this->m_TrackerTool->RemoveObserver( this->m_TransformToTrackerObserverID );

    this->InvokeEvent( DataAcquisitionEndEvent() );
    //actually perform the calibration
    this->m_PivotCalibrationAlgorithm->RequestResetCalibration();
    this->m_PivotCalibrationAlgorithm->RequestAddTransforms(this->m_Transforms);
    this->m_PivotCalibrationAlgorithm->RequestComputeCalibration();
    //check if the calibration computation failed
    if( this->m_ErrorObserver->ErrorOccured() ) 
      {
      this->m_ErrorObserver->GetErrorMessage( 
                                          this->m_ReasonForCalibrationFailure );
      this->m_ErrorObserver->ClearError();
      igstkPushInputMacro( Failed );
      }
    else
      {
      igstkPushInputMacro( Succeeded );
      }
    this->m_StateMachine.ProcessInputs();
    }
  else  //transform was updated, we need to retrieve it
    {
    this->m_TrackerTool->RequestGetTransformToParent();
    if( this->m_TransformObserver->GotTransformToTracker() )
      {
      this->m_Transforms.push_back( 
        (this->m_TransformObserver->GetTransformToTracker()).GetTransform() );
      DataAcquisitionEvent evt;
      evt.Set( (double)this->m_Transforms.size()/
                (double)(this->m_RequiredNumberOfTransformations) );
      this->InvokeEvent( evt );
      }
    }
}

void 
PivotCalibration::ReportCalibrationComputationSuccessProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibration::"
                  "ReportCalibrationComputationSuccessProcessing called...\n");
  this->InvokeEvent( CalibrationSuccessEvent() );
}

void 
PivotCalibration::ReportCalibrationComputationFailureProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibration::"
                  "ReportCalibrationComputationFailureProcessing called...\n");
  CalibrationFailureEvent evt; 
  evt.Set( this->m_ReasonForCalibrationFailure );
  this->InvokeEvent( evt );
}

void 
PivotCalibration::GetTransformProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibration::"
                  "GetTransformProcessing called...\n");
  this->m_PivotCalibrationAlgorithm->RequestCalibrationTransform();
  if( this->m_GetCalibrationTransformObserver->GotCalibrationTransform() ) 
    {
    CoordinateSystemTransformToEvent  event;
    event.Set(
      this->m_GetCalibrationTransformObserver->GetCalibrationTransform() );
    this->InvokeEvent(  event );
    }  
}

void 
PivotCalibration::GetPivotPointProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibration::"
                  "GetPivotPointProcessing called...\n");
  //the events generated by the 
  this->m_PivotCalibrationAlgorithm->RequestPivotPoint(); 
  if( this->m_GetPivotPointObserver->GotPivotPoint() ) 
    {
    PointEvent evt;
    evt.Set( this->m_GetPivotPointObserver->GetPivotPoint() );
    this->InvokeEvent( evt );
    }
}

void 
PivotCalibration::GetRMSEProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibration::"
                  "GetTransformRMSEProcessing called...\n");
  //the events generated by the 
  this->m_PivotCalibrationAlgorithm->RequestCalibrationRMSE();
  if( this->m_GetCalibrationRMSEObserver->GotCalibrationRMSE() ) 
    {
    DoubleTypeEvent evt;
    evt.Set( this->m_GetCalibrationRMSEObserver->GetCalibrationRMSE() );
    this->InvokeEvent( evt );
    }
}

void 
PivotCalibration::PrintSelf( std::ostream& os,
                                itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Tool : " << std::endl;
  os << indent << this->m_TrackerTool << std::endl;
  os << indent << "Required number of transformations: " << std::endl;
  os << indent << m_RequiredNumberOfTransformations << std::endl;
}

PivotCalibration::ErrorObserver::ErrorObserver() : m_ErrorOccured(false)
{
                           //calibration errors
  this->m_ErrorEvent2ErrorMessage.insert( std::pair<std::string,std::string>( 
   (igstk::PivotCalibrationAlgorithm::CalibrationFailureEvent()).GetEventName(),
                         "Pivot Calibration Algorithm: computation failed." ) );
}

void 
PivotCalibration::ErrorObserver::Execute(const itk::Object * itkNotUsed(caller),
                          const itk::EventObject & event) throw (std::exception)
{
  std::map<std::string,std::string>::iterator it;
  std::string className = event.GetEventName();
  it = this->m_ErrorEvent2ErrorMessage.find(className);

  this->m_ErrorOccured = true;
  this->m_ErrorMessage = (*it).second;
}

void 
PivotCalibration::ErrorObserver::Execute(itk::Object *caller, 
                          const itk::EventObject & event) throw (std::exception)
{
  const itk::Object * constCaller = caller;
  this->Execute(constCaller, event);
}

}//end namespace igstk
