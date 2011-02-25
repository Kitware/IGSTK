namespace igstk
{ 
template<class DataType>
USCalibrationAlgorithm<DataType>::USCalibrationAlgorithm() : 
  m_StateMachine( this )  
{
        //define the state machine's states 
  igstkAddStateMacro( Idle )
  igstkAddStateMacro( AttemptingToComputeCalibration )
  igstkAddStateMacro( CalibrationComputed )


                   //define the state machines inputs
  igstkAddInputMacro( AddDataElement )  
  igstkAddInputMacro( ComputeCalibration )
  igstkAddInputMacro( GetTransform  )
  igstkAddInputMacro( GetCalibrationError  )
  igstkAddInputMacro( ResetCalibration )
  igstkAddInputMacro( CalibrationComputationSuccess )
  igstkAddInputMacro( CalibrationComputationFailure )

            //define the state machine's transitions
  igstkAddTransitionMacro( Idle,
                           AddDataElement,
                           Idle,
                           AddData )

  igstkAddTransitionMacro( Idle,
                           ComputeCalibration,
                           AttemptingToComputeCalibration,
                           ComputeCalibration )

  igstkAddTransitionMacro( Idle,
                           GetTransform,
                           Idle,
                           ReportInvalidRequest )
  
  igstkAddTransitionMacro( Idle,
                           GetCalibrationError,
                           Idle,
                           ReportInvalidRequest )
  
  igstkAddTransitionMacro( Idle,
                           ResetCalibration,
                           Idle,
                           ResetCalibration )
  
  igstkAddTransitionMacro( Idle,
                           CalibrationComputationSuccess,
                           Idle,
                           ReportInvalidRequest )
  
  igstkAddTransitionMacro( Idle,
                           CalibrationComputationFailure,
                           Idle,
                           ReportInvalidRequest )

  igstkAddTransitionMacro( AttemptingToComputeCalibration,
                           AddDataElement,
                           AttemptingToComputeCalibration,
                           ReportInvalidRequest )

  igstkAddTransitionMacro( AttemptingToComputeCalibration,
                           ComputeCalibration,
                           AttemptingToComputeCalibration,
                           ReportInvalidRequest )

  igstkAddTransitionMacro( AttemptingToComputeCalibration,
                           GetTransform,
                           AttemptingToComputeCalibration,
                           ReportInvalidRequest )
  
  igstkAddTransitionMacro( AttemptingToComputeCalibration,
                           GetCalibrationError,
                           AttemptingToComputeCalibration,
                           ReportInvalidRequest )

  igstkAddTransitionMacro( AttemptingToComputeCalibration,
                           ResetCalibration,
                           AttemptingToComputeCalibration,
                           ReportInvalidRequest )

  igstkAddTransitionMacro( AttemptingToComputeCalibration,
                           CalibrationComputationSuccess,
                           CalibrationComputed,
                           ReportSuccessInCalibrationComputation )

  igstkAddTransitionMacro( AttemptingToComputeCalibration,
                           CalibrationComputationFailure,
                           Idle,
                           ReportFailureInCalibrationComputation )

  igstkAddTransitionMacro( CalibrationComputed,
                           AddDataElement,
                           CalibrationComputed,
                           ReportInvalidRequest )
  
  igstkAddTransitionMacro( CalibrationComputed,
                           ComputeCalibration,
                           AttemptingToComputeCalibration,
                           ComputeCalibration )

  igstkAddTransitionMacro( CalibrationComputed,
                           GetTransform,
                           CalibrationComputed,
                           GetTransform )
    
  igstkAddTransitionMacro( CalibrationComputed,
                           GetCalibrationError,
                           CalibrationComputed,
                           GetCalibrationError )
  
  igstkAddTransitionMacro( CalibrationComputed,
                           ResetCalibration,
                           Idle,
                           ResetCalibration )
  
  igstkAddTransitionMacro( CalibrationComputed,
                           CalibrationComputationSuccess,
                           CalibrationComputed,
                           ReportInvalidRequest )
  
  igstkAddTransitionMacro( CalibrationComputed,
                           CalibrationComputationFailure,
                           CalibrationComputed,
                           ReportInvalidRequest )

         //set the initial state of the state machine
  igstkSetInitialStateMacro( Idle )

         // done setting the state machine, ready to run
  this->m_StateMachine.SetReadyToRun();
  
       //internal variables are not initialized as they are not accessible till
       //they are assigned valid values later on
} 


template<class DataType>
USCalibrationAlgorithm<DataType>::~USCalibrationAlgorithm()  
{

}


template<class DataType>
void 
USCalibrationAlgorithm<DataType>::RequestAddDataElement( const DataType & d )
{
  igstkLogMacro( DEBUG, "igstk::USCalibrationAlgorithm::"
                 "RequestAddDataElement called...\n" );
  this->m_TmpData.push_back( d );
  igstkPushInputMacro( AddDataElement )
  this->m_StateMachine.ProcessInputs();
}


template<class DataType>
void 
USCalibrationAlgorithm<DataType>::RequestAddDataElements( 
  std::vector< DataType > & d )
{
  igstkLogMacro( DEBUG, "igstk::USCalibrationAlgorithm::"
                 "RequestAddDataElements called...\n");
  this->m_TmpData.insert( this->m_TmpData.begin(), d.begin(), d.end() );
  igstkPushInputMacro( AddDataElement )
  this->m_StateMachine.ProcessInputs();
}


template<class DataType>
void 
USCalibrationAlgorithm<DataType>::RequestResetCalibration()
{
  igstkLogMacro( DEBUG, "igstk::USCalibrationAlgorithm::"
                 "RequestResetCalibration called...\n" );
  igstkPushInputMacro( ResetCalibration )
  this->m_StateMachine.ProcessInputs();
}


template<class DataType>
void 
USCalibrationAlgorithm<DataType>::RequestComputeCalibration()
{
  igstkLogMacro( DEBUG, "igstk::USCalibrationAlgorithm::"
                 "RequestComputeCalibration called...\n" );
  igstkPushInputMacro( ComputeCalibration )
  this->m_StateMachine.ProcessInputs();
}


template<class DataType>
void 
USCalibrationAlgorithm<DataType>::RequestCalibrationTransform()
{
  igstkLogMacro( DEBUG, "igstk::USCalibrationAlgorithm::"
                 "RequestCalibrationTransform called...\n" );  
  igstkPushInputMacro( GetTransform )
  this->m_StateMachine.ProcessInputs();
}


template<class DataType>
void 
USCalibrationAlgorithm<DataType>::RequestCalibrationError()
{
  igstkLogMacro( DEBUG, "igstk::USCalibrationAlgorithm::"
                 "RequestCalibrationError called...\n" );  

  igstkPushInputMacro( GetCalibrationError )
  this->m_StateMachine.ProcessInputs();
}


template<class DataType>
void  
USCalibrationAlgorithm<DataType>::ReportInvalidRequestProcessing()
{
  igstkLogMacro( DEBUG, "igstk::USCalibrationAlgorithm::"
                 "ReportInvalidRequestProcessing called...\n" );
  this->InvokeEvent(InvalidRequestErrorEvent());
}


template<class DataType>
void 
USCalibrationAlgorithm<DataType>::AddDataProcessing()
{
  igstkLogMacro( DEBUG, "igstk::USCalibrationAlgorithm::"
                 "AddDataProcessing called...\n" );
  this->m_Data.insert( this->m_Data.begin(),
                       this->m_TmpData.begin(),
                       this->m_TmpData.end() );
  this->m_TmpData.clear();
}


template<class DataType>
void 
USCalibrationAlgorithm<DataType>::ComputeCalibrationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::USCalibrationAlgorithm::"
                 "ComputeCalibrationProcessing called...\n" );
  
  if( InternalComputeCalibrationProcessing() )
    igstkPushInputMacro( CalibrationComputationSuccess )
  else
    igstkPushInputMacro( CalibrationComputationFailure )
  
  this->m_StateMachine.ProcessInputs();
}


template<class DataType>
void  
USCalibrationAlgorithm<DataType>::ResetCalibrationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::USCalibrationAlgorithm::"
                 "ResetCalibrationProcessing called...\n" );
  this->m_TmpData.clear();
  this->m_Data.clear();
}


template<class DataType>
void  
USCalibrationAlgorithm<DataType>::ReportSuccessInCalibrationComputationProcessing()
{
  igstkLogMacro( DEBUG,
                 "igstk::USCalibrationAlgorithm::"
                 "ReportSuccessInCalibrationComputationProcessing called...\n" );
  this->InvokeEvent( CalibrationSuccessEvent() );
}


template<class DataType>
void  
USCalibrationAlgorithm<DataType>::ReportFailureInCalibrationComputationProcessing()
{
  igstkLogMacro( DEBUG,
                 "igstk::USCalibrationAlgorithm::"
                 "ReportFailureInCalibrationComputationProcessing called...\n" );
  this->InvokeEvent( CalibrationFailureEvent() );
}


template<class DataType>
void  
USCalibrationAlgorithm<DataType>::GetTransformProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::USCalibrationAlgorithm::"
                  "GetTransformProcessing called...\n" );

  TransformResultEvent result;
  result.Set( this->m_TransformData );
  this->InvokeEvent( result );  
}


template<class DataType>
void  
USCalibrationAlgorithm<DataType>::GetCalibrationErrorProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::USCalibrationAlgorithm::"
                  "GetCalibrationErrorProcessing called...\n" );
  DoubleTypeEvent event;
  event.Set( this->m_EstimationError );
  this->InvokeEvent( event );
}

} //end namespace igstk
