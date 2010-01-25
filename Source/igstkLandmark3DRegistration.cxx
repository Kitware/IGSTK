/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmark3DRegistration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
#pragma warning( disable : 4786 )
#endif

#include "igstkLandmark3DRegistration.h"
#include "igstkEvents.h"
#include "igstkCoordinateSystemTransformToResult.h" 

#include "itkNumericTraits.h"
#include "itkMatrix.h"
#include "itkSymmetricEigenAnalysis.h"

#include "vnl/vnl_math.h"

namespace igstk
{ 

/** Constructor */
Landmark3DRegistration::Landmark3DRegistration() : m_StateMachine( this )
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
  igstkAddInputMacro( GetTransformFromTrackerToImage  );
  igstkAddInputMacro( GetTransformFromImageToTracker  );
  igstkAddInputMacro( GetRMSError  );
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

  igstkAddTransitionMacro(ImageLandmark1Added,
                          ImageLandmark,
                          ImageLandmark1Added,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(TrackerLandmark1Added,
                          ImageLandmark,
                          ImageLandmark2Added,
                          AddImageLandmarkPoint);

  igstkAddTransitionMacro(TrackerLandmark1Added,
                          TrackerLandmark,
                          TrackerLandmark1Added,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(ImageLandmark2Added,
                          TrackerLandmark,
                          TrackerLandmark2Added,
                          AddTrackerLandmarkPoint);

  igstkAddTransitionMacro(ImageLandmark2Added,
                          ImageLandmark,
                          ImageLandmark2Added,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(TrackerLandmark2Added,
                          ImageLandmark,
                          ImageLandmark3Added,
                          AddImageLandmarkPoint);

  igstkAddTransitionMacro(TrackerLandmark2Added,
                          TrackerLandmark,
                          TrackerLandmark2Added,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(ImageLandmark3Added,
                          TrackerLandmark,
                          TrackerLandmark3Added,
                          AddTrackerLandmarkPoint);

  igstkAddTransitionMacro(ImageLandmark3Added,
                          ImageLandmark,
                          ImageLandmark3Added,
                          ReportInvalidRequest);

  igstkAddTransitionMacro(TrackerLandmark3Added,
                          ImageLandmark,
                          ImageLandmark3Added,
                          AddImageLandmarkPoint);

  igstkAddTransitionMacro(TrackerLandmark3Added,
                          TrackerLandmark,
                          TrackerLandmark3Added,
                          ReportInvalidRequest);

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
                           GetTransformFromTrackerToImage,
                           TransformComputed,
                           GetTransformFromTrackerToImage );

  igstkAddTransitionMacro( TransformComputed,
                           GetTransformFromImageToTracker,
                           TransformComputed,
                           GetTransformFromImageToTracker );


  igstkAddTransitionMacro( TransformComputed,
                           GetRMSError,
                           TransformComputed,
                           GetRMSError );

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

  igstkAddTransitionMacro( Idle,
                           GetTransformFromTrackerToImage,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( Idle,
                           GetTransformFromImageToTracker,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageLandmark1Added,
                           GetTransformFromTrackerToImage,
                           ImageLandmark1Added,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageLandmark1Added,
                           GetTransformFromImageToTracker,
                           ImageLandmark1Added,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageLandmark2Added,
                           GetTransformFromTrackerToImage,
                           ImageLandmark2Added,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( ImageLandmark2Added,
                           GetTransformFromImageToTracker,
                           ImageLandmark2Added,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( ImageLandmark3Added,
                           GetTransformFromTrackerToImage,
                           ImageLandmark3Added,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( ImageLandmark3Added,
                           GetTransformFromImageToTracker,
                           ImageLandmark3Added,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( TrackerLandmark1Added,
                           GetTransformFromTrackerToImage,
                           TrackerLandmark1Added,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( TrackerLandmark1Added,
                           GetTransformFromImageToTracker,
                           TrackerLandmark1Added,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( TrackerLandmark2Added,
                           GetTransformFromTrackerToImage,
                           TrackerLandmark2Added,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( TrackerLandmark2Added,
                           GetTransformFromImageToTracker,
                           TrackerLandmark2Added,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( TrackerLandmark3Added,
                           GetTransformFromTrackerToImage,
                           TrackerLandmark3Added,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( TrackerLandmark3Added,
                           GetTransformFromImageToTracker,
                           TrackerLandmark3Added,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( Idle,
                           GetRMSError,
                           Idle,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageLandmark1Added,
                           GetRMSError,
                           ImageLandmark1Added,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ImageLandmark2Added,
                           GetRMSError,
                           ImageLandmark2Added,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( ImageLandmark3Added,
                           GetRMSError,
                           ImageLandmark3Added,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( TrackerLandmark1Added,
                           GetRMSError,
                           TrackerLandmark1Added,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( TrackerLandmark2Added,
                           GetRMSError,
                           TrackerLandmark2Added,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( TrackerLandmark3Added,
                           GetRMSError,
                           TrackerLandmark3Added,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( TransformComputed,
                           ImageLandmark,
                           TransformComputed,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( TransformComputed,
                           TrackerLandmark,
                           TransformComputed,
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

  // Initialize the RMS error
  m_RMSError = 0.0;

  // Initialize collinearity tolerance
  m_CollinearityTolerance = 0.0001;


  // Initialize the coordinate systems of the Tracker and the Image
  // This should later be replaced with the actual coordinate systems
  // of the Tracker and Image points that are received.
  m_TrackerCoordinateSystem = CoordinateSystem::New();
  m_ImageCoordinateSystem   = CoordinateSystem::New();

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

  //Reset the transform
  TransformType::ParametersType parameters ( 
                               m_Transform->GetNumberOfParameters() );

  parameters[0] = 0.0;
  parameters[1] = 0.0;
  parameters[2] = 1.0;
  parameters[3] = 0.0;
  parameters[4] = 0.0;
  parameters[5] = 0.0;

  m_Transform->SetParameters( parameters );
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
    landmarkVector[0] += (*pointItr)[0];
    landmarkVector[1] += (*pointItr)[1];
    landmarkVector[2] += (*pointItr)[2];
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

  double ratio;

  ratio = ( vnl_math_sqr ( eigenValues[0]) + vnl_math_sqr ( eigenValues[1]))/ 
          ( vnl_math_sqr ( eigenValues[2]));

  if ( ratio > m_CollinearityTolerance )  
    {
    return false;
    }
  else
    {
    return true;
    }
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
    //Compute RMS error it the transform computation is successful
    this->ComputeRMSError();
    igstkLogMacro( DEBUG, "ComputationSuccessInput getting pushed" );
    igstkPushInputMacro( TransformComputationSuccess );
    }
  
  this->m_StateMachine.ProcessInputs();
}

/** The "ComputeRMSError" method computes the RMS error of the registration */
void
Landmark3DRegistration::ComputeRMSError()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                 "ComputeRMSError called..\n");

  //Check if the transformation parameters were evaluated correctly

  PointsContainerConstIterator      mitr = m_TrackerLandmarks.begin();
  PointsContainerConstIterator      fitr = m_ImageLandmarks.begin();

  TransformType::OutputVectorType   errortr;
  TransformType::OutputVectorType::RealValueType sum;

  sum = itk::NumericTraits< TransformType::OutputVectorType::RealValueType >
                                                                ::ZeroValue();
  
  int counter = itk::NumericTraits< int >::ZeroValue();
 
  while( mitr != m_TrackerLandmarks.end() ) 
    {
    errortr = *fitr - m_Transform->TransformPoint( *mitr );
    sum = sum + errortr.GetSquaredNorm();
    ++mitr;
    ++fitr;
    counter++;
    }

  m_RMSError = sqrt( sum / counter );
}
  

/** The "GetTransformFromTrackerToImage()" method throws and event 
 * containing the transform from tracker to image. */
void
Landmark3DRegistration::GetTransformFromTrackerToImageProcessing()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::\
                      GetTransformFromTrackerToImageProcessing called...\n" );

  igstk::Transform  transform;

  const igstk::Transform::ErrorType              error = m_RMSError; 

  const igstk::Transform::TimePeriodType         timePeriod = 
                                             itk::NumericTraits<double>::max();

  typedef TransformType::TranslationType        TranslationType;
  typedef TransformType::VersorType             VersorType;
  
  VersorType       versor      = m_Transform->GetVersor();
  TranslationType  translation = m_Transform->GetOffset();

  transform.SetTranslationAndRotation( translation, versor, error, timePeriod );

  CoordinateSystemTransformToResult transformCarrier;
  transformCarrier.Initialize( transform,
    m_TrackerCoordinateSystem,
    m_ImageCoordinateSystem );

  CoordinateSystemTransformToEvent transformEvent; 
  transformEvent.Set( transformCarrier );

  this->InvokeEvent( transformEvent );
}

/** The "GetTransformFromImageToTrackerProcessing()" method throws and event
 * containing the transform from image to tracker */
void
Landmark3DRegistration::GetTransformFromImageToTrackerProcessing()
{
  igstkLogMacro( DEBUG, 
    "igstk::Landmark3DRegistration::"
    "GetTransformFromImageToTrackerProcessing called...\n" );

  igstk::Transform  transform;

  const igstk::Transform::ErrorType              error = m_RMSError; 

  const igstk::Transform::TimePeriodType         timePeriod = 
                                             itk::NumericTraits<double>::max();

  typedef TransformType::TranslationType        TranslationType;
  typedef TransformType::VersorType             VersorType;
  
  VersorType       versor      = m_Transform->GetVersor();
  TranslationType  translation = m_Transform->GetOffset();

  transform.SetTranslationAndRotation( translation, versor, error, timePeriod );

  CoordinateSystemTransformToResult transformCarrier;
  transformCarrier.Initialize( transform.GetInverse(),
    m_ImageCoordinateSystem,
    m_TrackerCoordinateSystem );

  CoordinateSystemTransformToEvent transformEvent; 
  transformEvent.Set( transformCarrier );

  this->InvokeEvent( transformEvent );
}


/** The "GetRMSError()" method throws and event containing the RMS error */
void
Landmark3DRegistration::GetRMSErrorProcessing()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::\
                         GetRMSErrorProcessing called...\n" );

  DoubleTypeEvent event; 
  EventHelperType::DoubleType error;
  error = m_RMSError;
  event.Set( error );
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

void Landmark3DRegistration::RequestSetCollinearityTolerance(
                                            const double & tolerance)
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistration::"
                 "RequestSetCollinearityTolerance called...\n");
  this->m_CollinearityTolerance = tolerance;
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
Landmark3DRegistration::RequestGetTransformFromTrackerToImage()
{
  igstkLogMacro( DEBUG,
    "igstk::Landmark3DRegistration::RequestGetTransformFromTrackerToImage"
    " called...\n" );
  igstkPushInputMacro( GetTransformFromTrackerToImage );
  this->m_StateMachine.ProcessInputs();
}

void 
Landmark3DRegistration::RequestGetTransformFromImageToTracker()
{
  igstkLogMacro( DEBUG,
    "igstk::Landmark3DRegistration::RequestGetTransformFromImageToTracker"
    " called...\n" );
  igstkPushInputMacro( GetTransformFromImageToTracker );
  this->m_StateMachine.ProcessInputs();
}

void 
Landmark3DRegistration::RequestGetRMSError()
{
  igstkLogMacro( DEBUG,
             "igstk::Landmark3DRegistration::RequestGetRMSError called...\n" );
  igstkPushInputMacro( GetRMSError );
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
