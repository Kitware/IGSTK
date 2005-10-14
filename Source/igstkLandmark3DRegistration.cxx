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

#include "igstkLandmark3DRegistration.h"
#include "igstkTransform.h"
#include "igstkTransformModifiedEvent.h"

namespace igstk
{ 

/** Constructor */
Landmark3DRegistration::Landmark3DRegistration() :
  m_StateMachine( this ), m_Logger( NULL)
{
  // Set the state descriptors
  m_StateMachine.AddState(m_IdleState,
                          "IdleState");
  m_StateMachine.AddState(m_ImageLandmark1AddedState,
                          "ImageLandmark1AddedState");
  m_StateMachine.AddState(m_TrackerLandmark1AddedState,
                          "TrackerLandmark1AddedState");
  m_StateMachine.AddState(m_ImageLandmark2AddedState,
                          "ImageLandmark2AddedState");
  m_StateMachine.AddState(m_TrackerLandmark2AddedState,
                          "TrackerLandmark2AddedState");
  m_StateMachine.AddState(m_ImageLandmark3AddedState,
                          "ImageLandmark3AddedState");
  m_StateMachine.AddState(m_TrackerLandmark3AddedState,
                          "TrackerLandmark3AddedState");
  m_StateMachine.AddState( m_AttemptingToComputeTransformState,
                           "AttemptingToComputeTransformState" );
  m_StateMachine.AddState( m_TransformComputedState,
                           "TransformComputedState" );



  // Set the input descriptors 
  m_StateMachine.AddInput(m_ImageLandmarkInput,
                          "ImageLandmarkInput");
  m_StateMachine.AddInput(m_TrackerLandmarkInput,
                          "TrackerLandmarkInput");
  m_StateMachine.AddInput(m_ComputeTransformInput,
                          "ComputeTransformInput");
  m_StateMachine.AddInput( m_GetTransformInput,
                          "GetTransformInput" );
  m_StateMachine.AddInput(m_ResetRegistrationInput,
                          "ResetRegistrationInput");
  m_StateMachine.AddInput( m_TransformComputationSuccessInput,
                          "TransformComputationSuccessInput" );
  m_StateMachine.AddInput( m_TransformComputationFailureInput,
                          "TransformComputationFailureInput" );

  // Add transition  for landmark point adding
  m_StateMachine.AddTransition(m_IdleState,
                               m_ImageLandmarkInput,
                               m_ImageLandmark1AddedState,
                               &Landmark3DRegistration::AddImageLandmarkPoint);

  m_StateMachine.AddTransition(m_ImageLandmark1AddedState,
                               m_TrackerLandmarkInput,
                               m_TrackerLandmark1AddedState,
                               &Landmark3DRegistration::AddTrackerLandmarkPoint);

  m_StateMachine.AddTransition(m_TrackerLandmark1AddedState,
                               m_ImageLandmarkInput,
                               m_ImageLandmark2AddedState,
                               &Landmark3DRegistration::AddImageLandmarkPoint);

  m_StateMachine.AddTransition(m_ImageLandmark2AddedState,
                               m_TrackerLandmarkInput,
                               m_TrackerLandmark2AddedState,
                               &Landmark3DRegistration::AddTrackerLandmarkPoint);

  m_StateMachine.AddTransition(m_TrackerLandmark2AddedState,
                               m_ImageLandmarkInput,
                               m_ImageLandmark3AddedState,
                               &Landmark3DRegistration::AddImageLandmarkPoint);

  m_StateMachine.AddTransition(m_ImageLandmark3AddedState,
                               m_TrackerLandmarkInput,
                               m_TrackerLandmark3AddedState,
                               &Landmark3DRegistration::AddTrackerLandmarkPoint);

  m_StateMachine.AddTransition(m_TrackerLandmark3AddedState,
                               m_ImageLandmarkInput,
                               m_ImageLandmark3AddedState,
                               &Landmark3DRegistration::AddImageLandmarkPoint);

  m_StateMachine.AddTransition( m_TrackerLandmark3AddedState,
                                m_ComputeTransformInput,
                                m_AttemptingToComputeTransformState,
                                &Landmark3DRegistration::ComputeTransform );

  m_StateMachine.AddTransition( m_AttemptingToComputeTransformState,
                                m_TransformComputationSuccessInput,
                                m_TransformComputedState,
                                &Landmark3DRegistration::ReportSuccessInTransformComputation );

  m_StateMachine.AddTransition( m_AttemptingToComputeTransformState,
                                m_TransformComputationFailureInput,
                                m_TrackerLandmark3AddedState,
                                &Landmark3DRegistration::ReportFailureInTransformComputation );

  m_StateMachine.AddTransition( m_TransformComputedState,
                                m_GetTransformInput,
                                m_TransformComputedState,
                                &Landmark3DRegistration::GetTransform );

  // Add transitions for all invalid requests 
  m_StateMachine.AddTransition(m_IdleState,
                               m_ComputeTransformInput,
                               m_IdleState,
                               &Landmark3DRegistration::ReportInvalidRequest);

  m_StateMachine.AddTransition(m_ImageLandmark1AddedState,
                               m_ComputeTransformInput,
                               m_ImageLandmark1AddedState,
                               &Landmark3DRegistration::ReportInvalidRequest);

  m_StateMachine.AddTransition(m_ImageLandmark2AddedState,
                               m_ComputeTransformInput,
                               m_ImageLandmark2AddedState,
                               &Landmark3DRegistration::ReportInvalidRequest);

  m_StateMachine.AddTransition(m_ImageLandmark3AddedState,
                               m_ComputeTransformInput,
                               m_ImageLandmark3AddedState,
                               &Landmark3DRegistration::ReportInvalidRequest);

  m_StateMachine.AddTransition(m_TrackerLandmark1AddedState,
                               m_ComputeTransformInput,
                               m_TrackerLandmark1AddedState,
                               &Landmark3DRegistration::ReportInvalidRequest);

  m_StateMachine.AddTransition(m_TrackerLandmark2AddedState,
                               m_ComputeTransformInput,
                               m_TrackerLandmark2AddedState,
                               &Landmark3DRegistration::ReportInvalidRequest);
 

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


/* The "GetTransform()" method throws and event containing the transform */
void
Landmark3DRegistration::GetTransform()
{
  igstkLogMacro( DEBUG,
                  "igstk::Landmark3DRegistration::GetTransform called...\n" );

  igstk::Transform  transform;
  igstk::Transform::ErrorType              error;
  igstk::Transform::TimePeriodType         timePeriod;

  error        = 0.1;
  timePeriod   = 1000;  

  typedef TransformType::TranslationType       TranslationType;
  typedef TransformType::CenterType            CenterType;
  typedef TransformType::VersorType            VersorType;
  typedef TransformType::AxisType              AxisType;
  typedef TransformType::ParametersType        ParametersType;
  
  ParametersType                                   parameters;
  TranslationType                                  translation;
  CenterType                                       center;
  VersorType                                       versor;
  AxisType                                         axis;
  
 
  parameters  = m_Transform->GetParameters();
  
  std::cout << "Calculated transform parameters: " << parameters << std::endl;

  axis[0] = parameters[0];
  axis[1] = parameters[1];
  axis[2] = parameters[2];

  translation[0] = parameters[3];
  translation[1] = parameters[4];
  translation[2] = parameters[5];

  versor.Set(axis);

  transform.SetTranslationAndRotation( translation,
                     versor, error,timePeriod );

  center   =  m_Transform->GetCenter();

  transform.SetCenter( center, error,timePeriod );

  TransformModifiedEvent event; 
  event.SetTransform( transform );
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


