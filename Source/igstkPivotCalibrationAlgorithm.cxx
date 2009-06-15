/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPivotCalibrationAlgorithm.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkPivotCalibrationAlgorithm.h"

#include "vnl/algo/vnl_svd.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include "igstkCoordinateSystemInterfaceMacros.h"


namespace igstk
{ 

const double PivotCalibrationAlgorithm::DEFAULT_SINGULAR_VALUE_THRESHOLD = 1e-1;

PivotCalibrationAlgorithm::PivotCalibrationAlgorithm() : 
  m_StateMachine( this ), 
  m_SingularValueThreshold( DEFAULT_SINGULAR_VALUE_THRESHOLD )
{
  //define the state machine's states 
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( AttemptingToComputeCalibration  );
  igstkAddStateMacro( CalibrationComputed );


  //define the state machines inputs
  igstkAddInputMacro( AddTransform );
  igstkAddInputMacro( SetSingularValueThreshold );
  igstkAddInputMacro( ComputeCalibration );
  igstkAddInputMacro( GetTransform  );
  igstkAddInputMacro( GetPivotPoint  );
  igstkAddInputMacro( GetRMSE  );
  igstkAddInputMacro( ResetCalibration );
  igstkAddInputMacro( CalibrationComputationSuccess  );
  igstkAddInputMacro( CalibrationComputationFailure  );

  //define the state machine's transitions
  igstkAddTransitionMacro(Idle,
                          AddTransform,
                          Idle,
                          AddTransform);

  igstkAddTransitionMacro(Idle,
                          SetSingularValueThreshold,
                          Idle,
                          SetSingularValueThreshold);

  igstkAddTransitionMacro(Idle,
                          ComputeCalibration,
                          AttemptingToComputeCalibration,
                          ComputeCalibration);

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
  
  igstkAddTransitionMacro(Idle,
                          ResetCalibration,
                          Idle,
                          ResetCalibration);
  
  igstkAddTransitionMacro(Idle,
                          CalibrationComputationSuccess,
                          Idle,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(Idle,
                          CalibrationComputationFailure,
                          Idle,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToComputeCalibration,
                          AddTransform,
                          AttemptingToComputeCalibration,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToComputeCalibration,
                          SetSingularValueThreshold,
                          AttemptingToComputeCalibration,
                          ReportInvalidRequest);

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

  igstkAddTransitionMacro(AttemptingToComputeCalibration,
                          ResetCalibration,
                          AttemptingToComputeCalibration,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(AttemptingToComputeCalibration,
                          CalibrationComputationSuccess,
                          CalibrationComputed,
                          ReportSuccessInCalibrationComputation);

  igstkAddTransitionMacro(AttemptingToComputeCalibration,
                          CalibrationComputationFailure,
                          Idle,
                          ReportFailureInCalibrationComputation);

  igstkAddTransitionMacro(CalibrationComputed,
                          AddTransform,
                          CalibrationComputed,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(CalibrationComputed,
                          SetSingularValueThreshold,
                          CalibrationComputed,
                          SetSingularValueThreshold);  

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
  
  igstkAddTransitionMacro(CalibrationComputed,
                          ResetCalibration,
                          Idle,
                          ResetCalibration);
  
  igstkAddTransitionMacro(CalibrationComputed,
                          CalibrationComputationSuccess,
                          CalibrationComputed,
                          ReportInvalidRequest);
  
  igstkAddTransitionMacro(CalibrationComputed,
                          CalibrationComputationFailure,
                          CalibrationComputed,
                          ReportInvalidRequest);

  //set the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

  // done setting the state machine, ready to run
  this->m_StateMachine.SetReadyToRun();
  
  //internal variables are not initialized as they are not accessible till
  //they are assigned valid values later on
} 


PivotCalibrationAlgorithm::~PivotCalibrationAlgorithm()  
{

}

void 
PivotCalibrationAlgorithm::RequestAddTransform( const TransformType & t )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationAlgorithm::"
                 "RequestAddTransform called...\n");
  this->m_TmpTransforms.push_back(t);
  igstkPushInputMacro( AddTransform );
  this->m_StateMachine.ProcessInputs();
}

void 
PivotCalibrationAlgorithm::RequestAddTransforms( 
  std::vector< TransformType > & t )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationAlgorithm::"
                 "RequestAddTransforms called...\n");
  
  std::vector< TransformType >::const_iterator it = t.begin();
  std::vector< TransformType >::const_iterator transformsEnd = t.end(); 
  while(it!=transformsEnd) 
    {
    this->m_TmpTransforms.push_back((*it));
    it++;
    }
  igstkPushInputMacro( AddTransform );
  this->m_StateMachine.ProcessInputs();
}

void 
PivotCalibrationAlgorithm::RequestResetCalibration()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationAlgorithm::"
                 "RequestResetCalibration called...\n");
  igstkPushInputMacro( ResetCalibration );
  this->m_StateMachine.ProcessInputs();
}

void 
PivotCalibrationAlgorithm::RequestComputeCalibration()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationAlgorithm::"
                 "RequestComputeCalibration called...\n");
  igstkPushInputMacro( ComputeCalibration );
  this->m_StateMachine.ProcessInputs();
}

void 
PivotCalibrationAlgorithm::RequestCalibrationTransform()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationAlgorithm::"
                 "RequestGetCalibrationTransform called...\n");  
  igstkPushInputMacro( GetTransform );
  this->m_StateMachine.ProcessInputs();
}

void 
PivotCalibrationAlgorithm::RequestPivotPoint()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationAlgorithm::"
                 "RequestGetPivotPoint called...\n");  
  igstkPushInputMacro( GetPivotPoint );
  this->m_StateMachine.ProcessInputs();
}

void 
PivotCalibrationAlgorithm::RequestCalibrationRMSE()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationAlgorithm::"
                 "RequestCalibrationRMSE called...\n");  

  igstkPushInputMacro( GetRMSE );
  this->m_StateMachine.ProcessInputs();
}

void 
PivotCalibrationAlgorithm::RequestSetSingularValueThreshold( double threshold )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationAlgorithm::"
                 "RequestSetSingularValueThreshold called...\n");
  this->m_TmpSingularValueThreshold = threshold;
  igstkPushInputMacro( SetSingularValueThreshold );
  this->m_StateMachine.ProcessInputs();
}

void  
PivotCalibrationAlgorithm::ReportInvalidRequestProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationAlgorithm::"
                 "ReportInvalidRequestProcessing called...\n");
  this->InvokeEvent(InvalidRequestErrorEvent());
}

void 
PivotCalibrationAlgorithm::AddTransformProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationAlgorithm::"
                 "AddTransformProcessing called...\n");
  this->m_Transforms.insert(this->m_Transforms.begin(),
                            this->m_TmpTransforms.begin(),
                            this->m_TmpTransforms.end());
  this->m_TmpTransforms.clear();
}

void 
PivotCalibrationAlgorithm::SetSingularValueThresholdProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationAlgorithm::"
                 "SetSingularValueThresholdProcessing called...\n");
  this->m_SingularValueThreshold = this->m_TmpSingularValueThreshold;
}


void  
PivotCalibrationAlgorithm::ResetCalibrationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationAlgorithm::"
                 "ResetCalibrationProcessing called...\n");
  this->m_TmpTransforms.clear();
  this->m_Transforms.clear();
}

void 
PivotCalibrationAlgorithm::ComputeCalibrationProcessing()
{
  if(this->m_Transforms.empty()) 
    {
    igstkPushInputMacro( CalibrationComputationFailure );
    return;
    }

  unsigned int rows = 3*this->m_Transforms.size();
  unsigned int columns = 6;

  vnl_matrix< double > A( rows, columns ), minusI( 3, 3, 0 ), R( 3, 3 );
  vnl_vector< double > b(rows), x(columns), t(3);
  minusI( 0, 0 ) = -1;
  minusI( 1, 1 ) = -1;
  minusI( 2, 2 ) = -1;

  //do the computation and set the internal variables
  TransformContainerType::const_iterator it, 
    transformsEnd = this->m_Transforms.end();
  unsigned int currentRow;
  for( currentRow = 0, it = this->m_Transforms.begin(); 
       it != transformsEnd; 
       it++, currentRow += 3 ) 
    {
    t = (*it).GetTranslation().GetVnlVector();
    t *= -1;
    b.update( t, currentRow );
    R = (*it).GetRotation().GetMatrix().GetVnlMatrix();
    A.update( R, currentRow, 0 );
    A.update( minusI, currentRow, 3 );
    }
  vnl_svd<double> svdA(A);

  svdA.zero_out_absolute( this->m_SingularValueThreshold );

  //there is a solution only if rank(A)=6 (columns are linearly 
  //independent) 
  if( svdA.rank() < 6 ) 
    {
    igstkPushInputMacro( CalibrationComputationFailure );
    }
  else
    {
    x = svdA.solve( b );

    //set the RMSE
    this->m_RMSE = ( A * x - b ).rms();

    //set the transformation
    this->m_Transform.SetToIdentity( itk::NumericTraits<double>::max() );
    igstk::Transform::VectorType translation;
    translation[0] = x[0];
    translation[1] = x[1];
    translation[2] = x[2];
    //error value associated with transformation is the RMSE 
    //of the equation system, and validity time is set to 
    //maximal possible 
    this->m_Transform.SetTranslation( translation, 
                                      this->m_RMSE, 
                                      itk::NumericTraits<double>::max() );

    //set the pivot point
    this->m_PivotPoint[0] = x[3];
    this->m_PivotPoint[1] = x[4];
    this->m_PivotPoint[2] = x[5];

    igstkPushInputMacro( CalibrationComputationSuccess );
    }
}

void  
PivotCalibrationAlgorithm::ReportSuccessInCalibrationComputationProcessing()
{
  igstkLogMacro( DEBUG,
                 "igstk::PivotCalibrationAlgorithm::"
                 "ReportSuccessInCalibrationComputationProcessing called...\n");
  this->InvokeEvent( CalibrationSuccessEvent() );
}

void  
PivotCalibrationAlgorithm::ReportFailureInCalibrationComputationProcessing()
{
  igstkLogMacro( DEBUG,
                 "igstk::PivotCalibrationAlgorithm::"
                 "ReportFailureInCalibrationComputationProcessing called...\n");
  this->InvokeEvent( CalibrationFailureEvent() );
}

void  
PivotCalibrationAlgorithm::GetTransformProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibrationAlgorithm::"
                  "GetTransformProcessing called...\n");

  CoordinateSystemTransformToResult result;
  CoordinateSystemTransformToEvent event; 
  // FIXME: This event should also include the source and destination 
  // coordinate systems.
  result.Initialize( this->m_Transform, NULL, NULL );

  event.Set( result );
  this->InvokeEvent( event );
}

void  
PivotCalibrationAlgorithm::GetPivotPointProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibrationAlgorithm::"
                  "GetPivotPointProcessing called...\n");
  PointEvent event;
  event.Set( this->m_PivotPoint );
  this->InvokeEvent( event );
}

void  
PivotCalibrationAlgorithm::GetRMSEProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::PivotCalibrationAlgorithm::"
                  "GetRMSEProcessing called...\n");
  DoubleTypeEvent event;
  event.Set( this->m_RMSE );
  this->InvokeEvent( event );
}

void 
PivotCalibrationAlgorithm::PrintSelf( std::ostream& os,
                                             itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Transforms for pivot calibration: " << std::endl;
  TransformContainerType::const_iterator it, 
    endTransforms = this->m_Transforms.end();
  for( it=this->m_Transforms.begin(); it != endTransforms; it++)   
    os << indent << *it << std::endl;
}

} //end namespace igstk
