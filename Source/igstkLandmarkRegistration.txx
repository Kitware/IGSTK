#ifndef __igstkLandmarkRegistration_txx
#define __igstkLandmarkRegistration_txx

#include "igstkLandmarkRegistration.h"


namespace igstk
{ 

/** Constructor */
template <unsigned int TDimension>
LandmarkRegistration<TDimension>::LandmarkRegistration():m_StateMachine( this ), m_Logger( NULL)
{
  // Set the state descriptors
  m_StateMachine.AddState(m_IdleState,"IdleState");
  m_StateMachine.AddState(m_ImageLandmark1AddedState,"ImageLandmark1AddedState");
  m_StateMachine.AddState(m_TrackerLandmark1AddedState,"TrackerLandmark1AddedState");
  m_StateMachine.AddState(m_ImageLandmark2AddedState,"ImageLandmark2AddedState");
  m_StateMachine.AddState(m_TrackerLandmark2AddedState,"TrackerLandmark2AddedState");
  m_StateMachine.AddState(m_ImageLandmark3AddedState,"ImageLandmark3AddedState");
  m_StateMachine.AddState(m_TrackerLandmark3AddedState,"TrackerLandmark3AddedState");
  m_StateMachine.AddState(m_ReadyToComputeTransformState,"ReadyToComputeTransformState");
  m_StateMachine.AddState(m_FailedTransformComputationState,"FailedTransformComputationState");
  m_StateMachine.AddState(m_SuccessfulTransformComputationState,"SuccessfulTransformComputationState");

    // Set the input descriptors 
  m_StateMachine.AddInput(m_ImageLandmarkInput,"ImageLandmarkInput");
  m_StateMachine.AddInput(m_TrackerLandmarkInput,"TrackerLandmarkInput");
  m_StateMachine.AddInput(m_ComputeTransformInput,"ComputeTransformInput");
  m_StateMachine.AddInput(m_ResetRegistrationInput,"ResetRegistrationInput");
  m_StateMachine.AddInput(m_GetTransformParameterInput,"GetTransformParameterInput");

  // Add transition  for landmark point adding
  m_StateMachine.AddTransition(m_IdleState,m_ImageLandmarkInput,m_ImageLandmark1AddedState,&LandmarkRegistration::AddImageLandmarkPoint);
  m_StateMachine.AddTransition(m_ImageLandmark1AddedState,m_TrackerLandmarkInput,m_TrackerLandmark1AddedState,&LandmarkRegistration::AddTrackerLandmarkPoint);
  m_StateMachine.AddTransition(m_TrackerLandmark1AddedState,m_ImageLandmarkInput,m_ImageLandmark2AddedState,&LandmarkRegistration::AddImageLandmarkPoint);
  m_StateMachine.AddTransition(m_ImageLandmark2AddedState,m_TrackerLandmarkInput,m_TrackerLandmark2AddedState,&LandmarkRegistration::AddTrackerLandmarkPoint);
  m_StateMachine.AddTransition(m_TrackerLandmark2AddedState,m_ImageLandmarkInput,m_ImageLandmark3AddedState,&LandmarkRegistration::AddImageLandmarkPoint);
  m_StateMachine.AddTransition(m_ImageLandmark3AddedState,m_TrackerLandmarkInput,m_TrackerLandmark3AddedState,&LandmarkRegistration::AddTrackerLandmarkPoint);
  m_StateMachine.AddTransition(m_TrackerLandmark3AddedState,m_ImageLandmarkInput,m_ImageLandmark3AddedState,&LandmarkRegistration::AddImageLandmarkPoint);

  m_StateMachine.AddTransition(m_TrackerLandmark3AddedState,m_ComputeTransformInput,m_ImageLandmark3AddedState,&LandmarkRegistration::ComputeTransform);
  m_StateMachine.AddTransition(m_ReadyToComputeTransformState,m_TransformComputationSuccessInput,m_SuccessfulTransformComputationState,&LandmarkRegistration::TransformComputationSuccessProcessing);
  m_StateMachine.AddTransition(m_ReadyToComputeTransformState,m_TransformComputationFailureInput,m_FailedTransformComputationState,&LandmarkRegistration::TransformComputationFailureProcessing);

  m_StateMachine.AddTransition(m_SuccessfulTransformComputationState,m_GetTransformParameterInput,m_SuccessfulTransformComputationState,&LandmarkRegistration::GetTransformParameters);

   // Select the initial state of the state machine
  m_StateMachine.SelectInitialState( m_IdleState );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();

  m_Transform = TransformType::New();
  m_TransformInitializer = TransformInitializerType::New();

} 

/** Destructor */
template <unsigned int TDimension>
LandmarkRegistration<TDimension>::~LandmarkRegistration()  
{

}

/* The "AddImageLandmark" method adds landmark points to the image
* landmark point container */
template <unsigned int TDimension>
void LandmarkRegistration<TDimension>::AddImageLandmarkPoint() 
{
  igstkLogMacro( DEBUG, "igstk::LandmarkRegistration::AddImageLandmarkPoint called...\n");
  m_ImageLandmarks.push_back(m_ImageLandmarkPoint);
}

  
/* The "AddTrackerLandmark" method adds landmark points to the
* tracker landmark point container */
template <unsigned int TDimension>
void  LandmarkRegistration<TDimension>::AddTrackerLandmarkPoint()
{
  igstkLogMacro( DEBUG, "igstk::LandmarkRegistration::AddTrackerLandmarkPoint called...\n");
  m_TrackerLandmarks.push_back(m_TrackerLandmarkPoint);
}

/* The "ResetRegsitration" method empties the landmark point
containers to start the process again */
template <unsigned int TDimension>
void LandmarkRegistration<TDimension>::ResetRegistration()
{
  igstkLogMacro( DEBUG, "igstk::LandmarkRegistration::ResetRegistration called...\n");
  //Empty the image and tracker landmark containers
  while( m_ImageLandmarks.size() > 0 )
    {
    m_ImageLandmarks.pop_back();
    }
  while( m_TrackerLandmarks.size() > 0 )
    {
    m_TrackerLandmarks.pop_back();
    }
  
  m_StateMachine.SelectInitialState( m_IdleState );
}

/* The "ComputeTransform" method calculates the rigid body
  transformation parameters */
template <unsigned int TDimension>
void LandmarkRegistration<TDimension>:: ComputeTransform()
{
  igstkLogMacro( DEBUG, "igstk::LandmarkRegistration::ComputeTransform called...\n");
  m_TransformInitializer->SetFixedLandmarks(m_TrackerLandmarks);
  m_TransformInitializer->SetMovingLandmarks(m_ImageLandmarks);
  m_TransformInitializer->SetTransform( m_Transform );
  m_TransformInitializer->InitializeTransform(); 
}


/* The "GetTransformParameters()" method returns transformation
* parameters */
template <unsigned int TDimension>
void LandmarkRegistration<TDimension>::GetTransformParameters()
{

  
}


/* The "TransformComputationSuccessProcessing" method processes
* succesful transform computation */
template <unsigned int TDimension>
void  LandmarkRegistration<TDimension>::TransformComputationSuccessProcessing()
{

  
}

  
/* The "TransformComputationFailureProcessing" method processes a
* failed transform computation */
template <unsigned int TDimension>
void  LandmarkRegistration<TDimension>::TransformComputationFailureProcessing()
{
  
}

template <unsigned int TDimension>
void LandmarkRegistration<TDimension>::RequestAddImageLandmarkPoint(LandmarkImagePointType pt)
{
  igstkLogMacro( DEBUG, "igstk::LandmarkRegistration::RequestAddImageLandmarkPoint called...\n");
  this->m_ImageLandmarkPoint = pt;
  this->m_StateMachine.PushInput( this->m_ImageLandmarkInput);
  this->m_StateMachine.ProcessInputs();
}

template <unsigned int TDimension>  
void LandmarkRegistration<TDimension>::RequestAddTrackerLandmarkPoint(LandmarkImagePointType pt)
{
  igstkLogMacro( DEBUG, "igstk::LandmarkRegistration::RequestAddTrackerLandmarkPoint called...\n");
  this->m_TrackerLandmarkPoint = pt;
  this->m_StateMachine.PushInput( this->m_TrackerLandmarkInput);
  this->m_StateMachine.ProcessInputs();
}

template <unsigned int TDimension>
void LandmarkRegistration<TDimension>::RequestResetRegistration()
{
  igstkLogMacro( DEBUG, "igstk::LandmarkRegistration::RequestResetRegistration called...\n");
  this->m_StateMachine.PushInput( this->m_ResetRegistrationInput);
  this->m_StateMachine.ProcessInputs();
}

template <unsigned int TDimension>
void LandmarkRegistration<TDimension>::RequestComputeTransform()
{
  igstkLogMacro( DEBUG, "igstk::LandmarkRegistration::RequestComputeTransform called...\n");
  this->m_StateMachine.PushInput( this->m_ComputeTransformInput);
  this->m_StateMachine.ProcessInputs();
}

template <unsigned int TDimension>  
void LandmarkRegistration<TDimension>::RequestGetTransformParameters()
{
  
}

/** Register the fixed and moving coordinates */
template <unsigned int TDimension>
void LandmarkRegistration<TDimension>::EvaluateTransform()  
{
  m_TransformInitializer->SetFixedLandmarks(m_TrackerLandmarks);
  m_TransformInitializer->SetMovingLandmarks(m_ImageLandmarks);
  m_TransformInitializer->SetTransform( m_Transform );
  m_TransformInitializer->InitializeTransform(); 
}

/** Print Self function */
template <unsigned int TDimension>
void LandmarkRegistration<TDimension>::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Tracker Landmarks: " << std::endl;
  PointsContainerConstIterator fitr = m_TrackerLandmarks.begin();
  while( fitr != m_TrackerLandmarks.end() )
    {
    os << indent << *fitr << std::endl;
    ++fitr;
    }
  os << indent << "ModalityImage Landmarks: " << std::endl;
  PointsContainerConstIterator mitr = m_ImageLandmarks.begin();
  while( mitr != m_ImageLandmarks.end() )
    {
    os << indent << *mitr << std::endl;
    ++mitr;
    }
}

} // end namespace igstk

#endif


