/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmark3DRegistration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more DEBUGrmation.

=========================================================================*/
#ifndef __igstkLandmark3DRegistration_cxx
#define __igstkLandmark3DRegistration_cxx

#if defined(_MSC_VER)
#pragma warning( disable : 4786 )
#endif

#include "igstkLandmark3DRegistration.h"
#include "igstkTransform.h"
#include "igstkEvents.h"
#include "itkNumericTraits.h"

namespace igstk
{ 

/** Constructor */
Landmark3DRegistration::Landmark3DRegistration() :
  m_StateMachine( this )
{
  // Set the state descriptors
  igstkAddStateMacro( IdleState );
  igstkAddStateMacro( ImageLandmark1AddedState );
  igstkAddStateMacro( TrackerLandmark1AddedState );
  igstkAddStateMacro( ImageLandmark2AddedState );
  igstkAddStateMacro( TrackerLandmark2AddedState );
  igstkAddStateMacro( ImageLandmark3AddedState );
  igstkAddStateMacro( TrackerLandmark3AddedState );
  igstkAddStateMacro( AttemptingToComputeTransformState  );
  igstkAddStateMacro( TransformComputedState  );


  // Set the input descriptors 
  igstkAddInputMacro( ImageLandmarkInput );
  igstkAddInputMacro( TrackerLandmarkInput );
  igstkAddInputMacro( ComputeTransformInput );
  igstkAddInputMacro( GetTransformInput  );
  igstkAddInputMacro( ResetRegistrationInput );
  igstkAddInputMacro( TransformComputationSuccessInput  );
  igstkAddInputMacro( TransformComputationFailureInput  );

  // Add transition  for landmark point adding
  igstkAddTransitionMacro(IdleState,
                          ImageLandmarkInput,
                          ImageLandmark1AddedState,
                          AddImageLandmarkPoint);

  igstkAddTransitionMacro(ImageLandmark1AddedState,
                          TrackerLandmarkInput,
                          TrackerLandmark1AddedState,
                          AddTrackerLandmarkPoint);

  igstkAddTransitionMacro(TrackerLandmark1AddedState,
                          ImageLandmarkInput,
                          ImageLandmark2AddedState,
                          AddImageLandmarkPoint);

  igstkAddTransitionMacro(ImageLandmark2AddedState,
                          TrackerLandmarkInput,
                          TrackerLandmark2AddedState,
                          AddTrackerLandmarkPoint);

  igstkAddTransitionMacro(TrackerLandmark2AddedState,
                          ImageLandmarkInput,
                          ImageLandmark3AddedState,
                          AddImageLandmarkPoint);

  igstkAddTransitionMacro(ImageLandmark3AddedState,
                          TrackerLandmarkInput,
                          TrackerLandmark3AddedState,
                          AddTrackerLandmarkPoint);

  igstkAddTransitionMacro(TrackerLandmark3AddedState,
                          ImageLandmarkInput,
                          ImageLandmark3AddedState,
                          AddImageLandmarkPoint);

  igstkAddTransitionMacro( TrackerLandmark3AddedState,
                           ComputeTransformInput,
                           AttemptingToComputeTransformState,
                           ComputeTransform );

  igstkAddTransitionMacro( AttemptingToComputeTransformState,
                           TransformComputationSuccessInput,
                           TransformComputedState,
                           ReportSuccessInTransformComputation );

  igstkAddTransitionMacro( AttemptingToComputeTransformState,
                           TransformComputationFailureInput,
                           TrackerLandmark3AddedState,
                           ReportFailureInTransformComputation );

  igstkAddTransitionMacro( TransformComputedState,
                           GetTransformInput,
                           TransformComputedState,
                           GetTransform );

  // Add transitions for all invalid requests 
  igstkAddTransitionMacro( IdleState,
                           ComputeTransformInput,
                           IdleState,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageLandmark1AddedState,
                           ComputeTransformInput,
                           ImageLandmark1AddedState,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageLandmark2AddedState,
                           ComputeTransformInput,
                           ImageLandmark2AddedState,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( ImageLandmark3AddedState,
                           ComputeTransformInput,
                           ImageLandmark3AddedState,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( TrackerLandmark1AddedState,
                           ComputeTransformInput,
                           TrackerLandmark1AddedState,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( TrackerLandmark2AddedState,
                           ComputeTransformInput,
                           TrackerLandmark2AddedState,
                           ReportInvalidRequest);

  //Add transitions for registration reset state input 
  igstkAddTransitionMacro( TransformComputedState,
                           ResetRegistrationInput,
                           IdleState,
                           ResetRegistration);
  
  igstkAddTransitionMacro( ImageLandmark1AddedState,
                           ResetRegistrationInput,
                           IdleState,
                           ResetRegistration);
  
  igstkAddTransitionMacro( ImageLandmark2AddedState,
                           ResetRegistrationInput,
                           IdleState,
                           ResetRegistration);

  igstkAddTransitionMacro( ImageLandmark3AddedState,
                           ResetRegistrationInput,
                           IdleState,
                           ResetRegistration);
  
  igstkAddTransitionMacro( TrackerLandmark1AddedState,
                           ResetRegistrationInput,
                           IdleState,
                           ResetRegistration);

  igstkAddTransitionMacro( TrackerLandmark2AddedState,
                           ResetRegistrationInput,
                           IdleState,
                           ResetRegistration);
  
  igstkAddTransitionMacro( TrackerLandmark3AddedState,
                           ResetRegistrationInput,
                           IdleState,
                           ResetRegistration);

  igstkAddTransitionMacro( AttemptingToComputeTransformState,
                           ResetRegistrationInput,
                           IdleState,
                           ResetRegistration);

   // Select the initial state of the state machine
  m_StateMachine.SelectInitialState( m_IdleState );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();

  m_Transform = TransformType::New();
  m_TransformInitializer = TransformInitializerType::New();

} 

/** Destructor */
Landmark3DRegistration::~Landmark3DRegistration()  
{

}

/* The "AddImageLandmark" method adds landmark points to the image
* landmark point container */
void 
Landmark3DRegistration::AddImageLandmarkPoint() 
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                 "AddImageLandmarkPoint called...\n");
  m_ImageLandmarks.push_back(m_ImageLandmarkPoint);
}

  
/* The "AddTrackerLandmark" method adds landmark points to the
* tracker landmark point container */
void  
Landmark3DRegistration::AddTrackerLandmarkPoint()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                 "AddTrackerLandmarkPoint called...\n");
  m_TrackerLandmarks.push_back(m_TrackerLandmarkPoint);
}

/* The "ResetRegsitration" method empties the landmark point
containers to start the process again */
void 
Landmark3DRegistration::ResetRegistration()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                 "ResetRegistration called...\n");

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
void 
Landmark3DRegistration:: ComputeTransform()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                 "ComputeTransform called...\n");
  m_TransformInitializer->SetFixedLandmarks(m_TrackerLandmarks);
  m_TransformInitializer->SetMovingLandmarks(m_ImageLandmarks);
  m_TransformInitializer->SetTransform( m_Transform );
  bool failure = false;
  try 
    {
    m_TransformInitializer->InitializeTransform(); 
    }
  catch ( itk::ExceptionObject & excp )
    {
      igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                     "Transform computation exception" << excp.GetDescription());
      failure = true;
    }

  if( failure )
  {
    std::cout << "ComputationFailureInput getting pushed" << std::endl;
    this->m_StateMachine.PushInput( this->m_TransformComputationFailureInput );
  }
  else
  {
    std::cout << "ComputationSuccessInput getting pushed" << std::endl;
    this->m_StateMachine.PushInput( this->m_TransformComputationSuccessInput );
  }    
  
  this->m_StateMachine.ProcessInputs();
}

/* The "ComputeRMSError" method calculates and returns RMS Error of the transformation */
double 
Landmark3DRegistration:: ComputeRMSError()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                 "ComputeRMSError called..\n");

  //Check if the transformation parameters were evaluated correctly

  PointsContainerConstIterator
      mitr = m_TrackerLandmarks.begin();
  PointsContainerConstIterator
      fitr = m_ImageLandmarks.begin();

  TransformType::OutputVectorType   errortr;
  TransformType::OutputVectorType::RealValueType sum;

  sum = itk::NumericTraits< TransformType::OutputVectorType::RealValueType >::ZeroValue();
  
  int counter = itk::NumericTraits< int >::ZeroValue();
 
  while( mitr != m_TrackerLandmarks.end() ) 
    {
      errortr = *fitr - m_Transform->TransformPoint( *mitr );
      sum = sum + errortr.GetSquaredNorm();
      ++mitr;
      ++fitr;
      counter++;
    }

  double rms = sqrt( sum / counter );
  return rms;  
}
  

/* The "GetTransform()" method throws and event containing the transform */
void
Landmark3DRegistration::GetTransform()
{
  igstkLogMacro( DEBUG,
                  "igstk::Landmark3DRegistration::GetTransform called...\n" );

  igstk::Transform  transform;

  const igstk::Transform::ErrorType              error = 0.1;
  const igstk::Transform::TimePeriodType         timePeriod = 1000;;

  typedef TransformType::TranslationType        TranslationType;
  typedef TransformType::VersorType             VersorType;
  
  VersorType       versor      = m_Transform->GetVersor();
  TranslationType  translation = m_Transform->GetOffset();

  transform.SetTranslationAndRotation( translation, versor, error, timePeriod );

  TransformModifiedEvent event; 
  event.Set( transform );
  this->InvokeEvent( event );
}

/* The ReportInvalidRequest function reports invalid requests */
void  
Landmark3DRegistration::ReportInvalidRequest()
{
    igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                   "ReportInvalidRequest called...\n");
    this->InvokeEvent(InvalidRequestErrorEvent());
}

/* The ReportSuccessInTransformComputation function reports success in 
  transform calculation */
void  
Landmark3DRegistration::ReportSuccessInTransformComputation()
{
  igstkLogMacro( DEBUG,
                  "igstk::Landmark3DRegistration::"
                  "ReportSuccessInTransformComputation called...\n");
  this->InvokeEvent( TransformComputationSuccessEvent() );
}

/* The ReportFailureInTransformComputation function reports failure 
   in transform calculation */
void  
Landmark3DRegistration::ReportFailureInTransformComputation()
{
  igstkLogMacro( DEBUG,
                  "igstk::Landmark3DRegistration::"
                  "ReportFailureInTransformComputation called...\n");
  this->InvokeEvent( TransformComputationFailureEvent() );
}

void Landmark3DRegistration::RequestAddImageLandmarkPoint(
                                           const LandmarkImagePointType & pt)
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                 "RequestAddImageLandmarkPoint called...\n");
  this->m_ImageLandmarkPoint = pt;
  this->m_StateMachine.PushInput( this->m_ImageLandmarkInput);
  this->m_StateMachine.ProcessInputs();
}

void 
Landmark3DRegistration::RequestAddTrackerLandmarkPoint(
                                      const LandmarkImagePointType & pt)
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                 "RequestAddTrackerLandmarkPoint called...\n");
  this->m_TrackerLandmarkPoint = pt;
  this->m_StateMachine.PushInput( this->m_TrackerLandmarkInput );
  this->m_StateMachine.ProcessInputs();
}

void 
Landmark3DRegistration::RequestResetRegistration()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                 "RequestResetRegistration called...\n");
  this->m_StateMachine.PushInput( this->m_ResetRegistrationInput );
  this->m_StateMachine.ProcessInputs();
}

void 
Landmark3DRegistration::RequestComputeTransform()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                 "RequestComputeTransform called...\n");
  this->m_StateMachine.PushInput( this->m_ComputeTransformInput );
  this->m_StateMachine.ProcessInputs();
}

void 
Landmark3DRegistration::RequestGetTransform()
{
  igstkLogMacro( DEBUG,
             "igstk::Landmark3DRegistration::RequestGetTransform called...\n" );
  this->m_StateMachine.PushInput( this->m_GetTransformInput );
  this->m_StateMachine.ProcessInputs();
}

/** Print Self function */
void 
Landmark3DRegistration::PrintSelf( std::ostream& os,
                                             itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Tracker Landmarks: " << std::endl;
  PointsContainerConstIterator fitr = m_TrackerLandmarks.begin();
  while( fitr != m_TrackerLandmarks.end() )
    {
    os << indent << *fitr << std::endl;
    ++fitr;
    }
  os << indent << "Image Landmarks: " << std::endl;
  PointsContainerConstIterator mitr = m_ImageLandmarks.begin();
  while( mitr != m_ImageLandmarks.end() )
    {
    os << indent << *mitr << std::endl;
    ++mitr;
    }
}

} // end namespace igstk

#endif


