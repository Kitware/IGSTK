/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    LandmarkRegistration1.cxx
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

// BeginLatex
// This example illustrates how the landmark registration component can 
// be used.
// EndLatex

// BeginCodeSnippet
#include "igstkLandmark3DRegistration.h"
#include "igstkTransform.h"
#include "igstkEvents.h"

#include "itkNumericTraits.h"
#include "itkMatrix.h"
#include "itkSymmetricEigenAnalysis.h"

#include "vnl/vnl_math.h"
// EndCodeSnippet

namespace igstk
{ 

/** Constructor */
Landmark3DRegistration::Landmark3DRegistration() :
  m_StateMachine( this )
{
  // Set the state descriptors
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( ImageLandmark1Added );
  igstkAddStateMacro( TrackerLandmark1Added );
  igstkAddStateMacro( ImageLandmark2Added );
  igstkAddStateMacro( TrackerLandmark2Added );
  igstkAddStateMacro( ImageLandmark3Added );
  igstkAddStateMacro( TrackerLandmark3Added );
  igstkAddStateMacro( AttemptingToComputeTransform  );
  igstkAddStateMacro( TransformComputed  );


  // Set the input descriptors 
  igstkAddInputMacro( ImageLandmark );
  igstkAddInputMacro( TrackerLandmark );
  igstkAddInputMacro( ComputeTransform );
  igstkAddInputMacro( GetTransform  );
  igstkAddInputMacro( ResetRegistration );
  igstkAddInputMacro( TransformComputationSuccess  );
  igstkAddInputMacro( TransformComputationFailure  );

  // Add transition  for landmark point adding
  igstkAddTransitionMacro(Idle,
                          ImageLandmark,
                          ImageLandmark1Added,
                          AddImageLandmarkPoint);

  igstkAddTransitionMacro(ImageLandmark1Added,
                          TrackerLandmark,
                          TrackerLandmark1Added,
                          AddTrackerLandmarkPoint);

  igstkAddTransitionMacro(TrackerLandmark1Added,
                          ImageLandmark,
                          ImageLandmark2Added,
                          AddImageLandmarkPoint);

  igstkAddTransitionMacro(ImageLandmark2Added,
                          TrackerLandmark,
                          TrackerLandmark2Added,
                          AddTrackerLandmarkPoint);

  igstkAddTransitionMacro(TrackerLandmark2Added,
                          ImageLandmark,
                          ImageLandmark3Added,
                          AddImageLandmarkPoint);

  igstkAddTransitionMacro(ImageLandmark3Added,
                          TrackerLandmark,
                          TrackerLandmark3Added,
                          AddTrackerLandmarkPoint);

  igstkAddTransitionMacro(TrackerLandmark3Added,
                          ImageLandmark,
                          ImageLandmark3Added,
                          AddImageLandmarkPoint);

  igstkAddTransitionMacro( TrackerLandmark3Added,
                           ComputeTransform,
                           AttemptingToComputeTransform,
                           ComputeTransform );

  igstkAddTransitionMacro( AttemptingToComputeTransform,
                           TransformComputationSuccess,
                           TransformComputed,
                           ReportSuccessInTransformComputation );

  igstkAddTransitionMacro( AttemptingToComputeTransform,
                           TransformComputationFailure,
                           TrackerLandmark3Added,
                           ReportFailureInTransformComputation );

  igstkAddTransitionMacro( TransformComputed,
                           GetTransform,
                           TransformComputed,
                           GetTransform );

  // Add transitions for all invalid requests 
  igstkAddTransitionMacro( Idle,
                           ComputeTransform,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageLandmark1Added,
                           ComputeTransform,
                           ImageLandmark1Added,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageLandmark2Added,
                           ComputeTransform,
                           ImageLandmark2Added,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( ImageLandmark3Added,
                           ComputeTransform,
                           ImageLandmark3Added,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( TrackerLandmark1Added,
                           ComputeTransform,
                           TrackerLandmark1Added,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( TrackerLandmark2Added,
                           ComputeTransform,
                           TrackerLandmark2Added,
                           ReportInvalidRequest);

  //Add transitions for registration reset state input 
  igstkAddTransitionMacro( TransformComputed,
                           ResetRegistration,
                           Idle,
                           ResetRegistration);
  
  igstkAddTransitionMacro( ImageLandmark1Added,
                           ResetRegistration,
                           Idle,
                           ResetRegistration);
  
  igstkAddTransitionMacro( ImageLandmark2Added,
                           ResetRegistration,
                           Idle,
                           ResetRegistration);

  igstkAddTransitionMacro( ImageLandmark3Added,
                           ResetRegistration,
                           Idle,
                           ResetRegistration);
  
  igstkAddTransitionMacro( TrackerLandmark1Added,
                           ResetRegistration,
                           Idle,
                           ResetRegistration);

  igstkAddTransitionMacro( TrackerLandmark2Added,
                           ResetRegistration,
                           Idle,
                           ResetRegistration);
  
  igstkAddTransitionMacro( TrackerLandmark3Added,
                           ResetRegistration,
                           Idle,
                           ResetRegistration);

  igstkAddTransitionMacro( AttemptingToComputeTransform,
                           ResetRegistration,
                           Idle,
                           ResetRegistration);

   // Select the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

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
Landmark3DRegistration::AddImageLandmarkPointProcessing() 
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                 "AddImageLandmarkPointProcessing called...\n");
  m_ImageLandmarks.push_back(m_ImageLandmarkPoint);
}

  
/* The "AddTrackerLandmark" method adds landmark points to the
* tracker landmark point container */
void  
Landmark3DRegistration::AddTrackerLandmarkPointProcessing()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                 "AddTrackerLandmarkPointProcessing called...\n");
  m_TrackerLandmarks.push_back(m_TrackerLandmarkPoint);
}

/* The "ResetRegsitration" method empties the landmark point
containers to start the process again */
void 
Landmark3DRegistration::ResetRegistrationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                 "ResetRegistrationProcessing called...\n");

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

/* The "CheckCollinearity" method checks whether the landmark points 
 *  are collinear or not */
bool
Landmark3DRegistration::CheckCollinearity()
{
   typedef itk::Matrix<double,3,3>                 MatrixType;
   typedef itk::Vector<double,3>                   VectorType;

   MatrixType                                      covarianceMatrix;
   
   PointsContainerConstIterator                    pointItr;
   
   VectorType                                      landmarkVector;
   VectorType                                      landmarkCentered;
   VectorType                                      landmarkCentroid;
   
   landmarkVector.Fill(0.0);
   
   pointItr  = m_ImageLandmarks.begin();
   while( pointItr != m_ImageLandmarks.end() )
     {
     landmarkVector[0] += (*pointItr)[0] ;
     landmarkVector[1] += (*pointItr)[1] ;
     landmarkVector[2] += (*pointItr)[2] ;
     ++pointItr;
     }

  for(unsigned int ic=0; ic<3; ic++)
    {
    landmarkCentroid[ic]  = landmarkVector[ic]  / this->m_ImageLandmarks.size();
    } 
  
  pointItr  = m_ImageLandmarks.begin();
  while( pointItr != m_ImageLandmarks.end() )
     {
     for(unsigned int i=0; i<3; i++)
       {     
       landmarkCentered[i]  = (*pointItr)[i]  - landmarkCentroid[i];
       }

     for(unsigned int i=0; i<3; i++)
       {
       for(unsigned int j=0; j<3; j++)
         {
         covarianceMatrix[i][j] += landmarkCentered[i] * landmarkCentered[j];
         }
       }
     ++pointItr;
     }

    MatrixType                    eigenVectors;
    VectorType                    eigenValues;

   typedef itk::SymmetricEigenAnalysis< 
           MatrixType,  
           VectorType,
           MatrixType > SymmetricEigenAnalysisType;

   SymmetricEigenAnalysisType symmetricEigenSystem(3);
   
   symmetricEigenSystem.SetOrderEigenMagnitudes( true );
        
   symmetricEigenSystem.ComputeEigenValuesAndVectors
               ( covarianceMatrix, eigenValues, eigenVectors );

   double tolerance=0.001;

   double ratio;

   ratio = ( vnl_math_sqr ( eigenValues[0]) + vnl_math_sqr ( eigenValues[1]))/ 
           ( vnl_math_sqr ( eigenValues[2]));

   if ( ratio > tolerance )  
      return false; 
   else
     return true;
}

/* The "ComputeTransform" method calculates the rigid body
  transformation parameters */
void 
Landmark3DRegistration:: ComputeTransformProcessing()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                 "ComputeTransformProcessing called...\n");
  m_TransformInitializer->SetFixedLandmarks(m_TrackerLandmarks);
  m_TransformInitializer->SetMovingLandmarks(m_ImageLandmarks);
  m_TransformInitializer->SetTransform( m_Transform );
  
  bool failure = false;

  // check for collinearity
  failure = CheckCollinearity();

  if ( ! failure ) 
    {
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
    }

  if( failure )
    {
    igstkLogMacro( DEBUG, "ComputationFailureInput getting pushed" );
    igstkPushInputMacro( TransformComputationFailure );
    }
  else
    {
    igstkLogMacro( DEBUG, "ComputationSuccessInput getting pushed" );
    igstkPushInputMacro( TransformComputationSuccess );
    }    
  
  this->m_StateMachine.ProcessInputs();
}

/* The "ComputeRMSError" method calculates and returns RMS Error of the transformation */
double 
Landmark3DRegistration::ComputeRMSError()
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
Landmark3DRegistration::GetTransformProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::Landmark3DRegistration::GetTransformProcessing called...\n" );

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
Landmark3DRegistration::ReportInvalidRequestProcessing()
{
    igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                   "ReportInvalidRequestProcessing called...\n");
    this->InvokeEvent(InvalidRequestErrorEvent());
}

/* The ReportSuccessInTransformComputation function reports success in 
  transform calculation */
void  
Landmark3DRegistration::ReportSuccessInTransformComputationProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::Landmark3DRegistration::"
                  "ReportSuccessInTransformComputationProcessing called...\n");
  this->InvokeEvent( TransformComputationSuccessEvent() );
}

/* The ReportFailureInTransformComputationProcessing function reports failure 
   in transform calculation */
void  
Landmark3DRegistration::ReportFailureInTransformComputationProcessing()
{
  igstkLogMacro( DEBUG,
                  "igstk::Landmark3DRegistration::"
                  "ReportFailureInTransformComputationProcessing called...\n");
  this->InvokeEvent( TransformComputationFailureEvent() );
}

void Landmark3DRegistration::RequestAddImageLandmarkPoint(
                                           const LandmarkImagePointType & pt)
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                 "RequestAddImageLandmarkPoint called...\n");
  this->m_ImageLandmarkPoint = pt;
  igstkPushInputMacro( ImageLandmark );
  this->m_StateMachine.ProcessInputs();
}

void 
Landmark3DRegistration::RequestAddTrackerLandmarkPoint(
                                      const LandmarkImagePointType & pt)
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                 "RequestAddTrackerLandmarkPoint called...\n");
  this->m_TrackerLandmarkPoint = pt;
  igstkPushInputMacro( TrackerLandmark );
  this->m_StateMachine.ProcessInputs();
}

void 
Landmark3DRegistration::RequestResetRegistration()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                 "RequestResetRegistration called...\n");
  igstkPushInputMacro( ResetRegistration );
  this->m_StateMachine.ProcessInputs();
}

void 
Landmark3DRegistration::RequestComputeTransform()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                 "RequestComputeTransform called...\n");
  igstkPushInputMacro( ComputeTransform );
  this->m_StateMachine.ProcessInputs();
}

void 
Landmark3DRegistration::RequestGetTransform()
{
  igstkLogMacro( DEBUG,
             "igstk::Landmark3DRegistration::RequestGetTransform called...\n" );
  igstkPushInputMacro( GetTransform );
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


