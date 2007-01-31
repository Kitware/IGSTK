/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmark3DRegistrationTest2.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
#pragma warning( disable : 4786 )
#endif
#include <iostream>
#include "igstkLandmark3DRegistration.h"
#include "igstkLandmark3DRegistrationErrorEstimator.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkObject.h"
#include "itkCommand.h"
#include "itkMacro.h"
#include "igstkEvents.h"
#include "igstkTransform.h"
 
class Landmark3DRegistrationGetTransformCallback: public itk::Command
{
public:
  typedef Landmark3DRegistrationGetTransformCallback    Self;
  typedef itk::SmartPointer<Self>                       Pointer;
  typedef itk::Command                                  Superclass;
  itkNewMacro(Self);

  typedef igstk::TransformModifiedEvent TransformModifiedEventType;

  void Execute( const itk::Object *caller, const itk::EventObject & event )
    {
    }

  void Execute( itk::Object *caller, const itk::EventObject & event )
    {
    std::cout<< " TransformEvent is thrown" << std::endl;
    const TransformModifiedEventType * transformEvent =
     dynamic_cast < const TransformModifiedEventType* > ( &event );
    m_Transform = transformEvent->Get();
    m_EventReceived = true;
    }

  bool GetEventReceived()
    {
    return m_EventReceived;
    }

  igstk::Transform GetTransform()
    {
    return m_Transform;
    } 

protected:
  Landmark3DRegistrationGetTransformCallback()  
    {
    m_EventReceived = true;
    }
 
private:
  bool             m_EventReceived;
  igstk::Transform m_Transform;
};


/** The objective of this program is to test if the LandmarkRegistration can
    be reused. First, registration transform is computed by populating the 
    landmark container with landmark points ( set 1 ), compute the transform, 
    and then reset the registration and recompute the the transform with a new
    set of landmark points ( set 2 ). Second, registration is computed with 
    the second set of the landmark points. The final transform parameters are
    then compared. 
*/


int igstkLandmark3DRegistrationTest2( int argv, char * argc[] )
{
  igstk::RealTimeClock::Initialize();

  typedef itk::Logger                   LoggerType;
  typedef itk::StdStreamLogOutput       LogOutputType;
   
  typedef igstk::Landmark3DRegistration
  Landmark3DRegistrationType;
  typedef igstk::Landmark3DRegistration::LandmarkPointContainerType
  LandmarkPointContainerType;
  typedef igstk::Landmark3DRegistration::LandmarkImagePointType
  LandmarkImagePointType;
  typedef igstk::Landmark3DRegistration::LandmarkTrackerPointType
  LandmarkTrackerPointType;
  typedef Landmark3DRegistrationType::TransformType::OutputVectorType
  OutputVectorType;

  typedef igstk::Transform                     TransformType;
  typedef itk::VersorRigid3DTransform<double>  VersorRigid3DTransformType;

  typedef itk::VersorRigid3DTransform<double>::ParametersType ParametersType;


  Landmark3DRegistrationType::Pointer landmarkRegister =
  Landmark3DRegistrationType::New();

  LandmarkImagePointType      imagePoint;
  LandmarkTrackerPointType    trackerPoint;

  TransformType      transform;

  Landmark3DRegistrationGetTransformCallback::Pointer lrtcb =
  Landmark3DRegistrationGetTransformCallback::New();
  landmarkRegister->AddObserver( igstk::TransformModifiedEvent(), lrtcb );

  typedef igstk::Landmark3DRegistrationErrorEstimator   ErrorEstimatorType;

  typedef ErrorEstimatorType::TargetPointType   TargetPointType;
  typedef ErrorEstimatorType::ErrorType         ErrorType;

  ErrorType                   landmarkRegistrationError;

  imagePoint[0] =  78.3163;
  imagePoint[1] =  15.3252;
  imagePoint[2] =  16;
  landmarkRegister->RequestAddImageLandmarkPoint(imagePoint);

  trackerPoint[0] =  199.274;
  trackerPoint[1] =  -26.6466;
  trackerPoint[2] =  -869.895;
  landmarkRegister->RequestAddTrackerLandmarkPoint(trackerPoint);

  imagePoint[0] =  12.5937;
  imagePoint[1] =   9.9381;
  imagePoint[2] =  16;
  landmarkRegister->RequestAddImageLandmarkPoint(imagePoint);

  trackerPoint[0] =  218.485;
  trackerPoint[1] =  -2.90349;
  trackerPoint[2] =  -882.859;
  landmarkRegister->RequestAddTrackerLandmarkPoint(trackerPoint);


  imagePoint[0] =  45.3473;
  imagePoint[1] =   7.78325;
  imagePoint[2] =  60;
  landmarkRegister->RequestAddImageLandmarkPoint(imagePoint);

  trackerPoint[0] =  304.046;
  trackerPoint[1] =  -34.6625;
  trackerPoint[2] =  -885.358;
  landmarkRegister->RequestAddTrackerLandmarkPoint(trackerPoint);

  landmarkRegister->RequestComputeTransform();

  // reuse the landmark registration
  landmarkRegister->RequestResetRegistration();

  imagePoint[0] =  77.2389;
  imagePoint[1] =  15.9716;
  imagePoint[2] =  16;
  landmarkRegister->RequestAddImageLandmarkPoint(imagePoint);
  trackerPoint[0] =  230.525;
  trackerPoint[1] =  -44.7263;
  trackerPoint[2] =  -894.183;
  landmarkRegister->RequestAddTrackerLandmarkPoint(trackerPoint);

  // Add 2nd landmark
  imagePoint[0] =  14.1021;
  imagePoint[1] =  10.3691;
  imagePoint[2] =  16;
  landmarkRegister->RequestAddImageLandmarkPoint(imagePoint);
  trackerPoint[0] =  211.976;
  trackerPoint[1] =  -41.8738;
  trackerPoint[2] =  -875.216;
  landmarkRegister->RequestAddTrackerLandmarkPoint(trackerPoint);

  // Add 3d landmark
  imagePoint[0] =  45.9937;
  imagePoint[1] =  6.27483;
  imagePoint[2] =  68;
  landmarkRegister->RequestAddImageLandmarkPoint(imagePoint);
  trackerPoint[0] =  303.781;
  trackerPoint[1] =  -35.4769;
  trackerPoint[2] =  -887.587;
  landmarkRegister->RequestAddTrackerLandmarkPoint(trackerPoint);

  landmarkRegister->RequestComputeTransform();
  landmarkRegister->RequestGetTransform();

  transform = lrtcb->GetTransform();
  std::cout << "Transform " << transform << std::cout;

  landmarkRegistrationError = landmarkRegister->ComputeRMSError();
  std::cout<<"RMS Error is "<<landmarkRegistrationError<<std::endl;


  // second landmark registration

  Landmark3DRegistrationType::Pointer landmarkRegister2 =
  Landmark3DRegistrationType::New();

  Landmark3DRegistrationGetTransformCallback::Pointer lrtcb2 =
  Landmark3DRegistrationGetTransformCallback::New();
  landmarkRegister2->AddObserver( igstk::TransformModifiedEvent(), lrtcb2 );


  imagePoint[0] =  77.2389;
  imagePoint[1] =  15.9716;
  imagePoint[2] =  16;
  landmarkRegister2->RequestAddImageLandmarkPoint(imagePoint);
  trackerPoint[0] =  230.525;
  trackerPoint[1] =  -44.7263;
  trackerPoint[2] =  -894.183;
  landmarkRegister2->RequestAddTrackerLandmarkPoint(trackerPoint);

  // Add 2nd landmark
  imagePoint[0] =  14.1021;
  imagePoint[1] =  10.3691;
  imagePoint[2] =  16;
  landmarkRegister2->RequestAddImageLandmarkPoint(imagePoint);
  trackerPoint[0] =  211.976;
  trackerPoint[1] =  -41.8738;
  trackerPoint[2] =  -875.216;
  landmarkRegister2->RequestAddTrackerLandmarkPoint(trackerPoint);

  // Add 3d landmark
  imagePoint[0] =  45.9937;
  imagePoint[1] =  6.27483;
  imagePoint[2] =  68;
  landmarkRegister2->RequestAddImageLandmarkPoint(imagePoint);
  trackerPoint[0] =  303.781;
  trackerPoint[1] =  -35.4769;
  trackerPoint[2] =  -887.587;
  landmarkRegister2->RequestAddTrackerLandmarkPoint(trackerPoint);

  landmarkRegister2->RequestComputeTransform();
  landmarkRegister2->RequestGetTransform();

  TransformType            transform2;
  transform2 = lrtcb2->GetTransform();
  std::cout << "Transform " << transform2 << std::cout;

  ErrorType                   landmarkRegistrationError2;

  landmarkRegistrationError2 = landmarkRegister2->ComputeRMSError();
  std::cout<<"RMS Error is "<<landmarkRegistrationError2<<std::endl;


  // Compare the two transforms ( transform1 and transform2 )

  typedef TransformType::VectorType                VectorType;
  typedef TransformType::VersorType                VersorType;

  VectorType translation;
  VectorType translation2;

  translation  = transform.GetTranslation();
  translation2 = transform2.GetTranslation();

  const double tolerance = 1e-8;

  for( unsigned int i=0; i<3; i++ )
    {
    if( fabs( translation2[i]  - translation[i] ) > tolerance )
      {
      std::cerr << "First  translation = " << translation << std::endl;
      std::cerr << "Second translation = " << translation2 << std::endl;
      return EXIT_FAILURE;
      }
    }

  VersorType           rotation;
  VersorType           rotation2;

  rotation  = transform.GetRotation();
  rotation2 = transform2.GetRotation();

  if( fabs( rotation2.GetX() - rotation.GetX() ) > tolerance ||
     fabs( rotation2.GetY() - rotation.GetY() ) > tolerance ||
     fabs( rotation2.GetZ() - rotation.GetZ() ) > tolerance ||
     fabs( rotation2.GetW() - rotation.GetW() ) > tolerance     )
    {
    std::cerr << "First  rotation = " << rotation << std::endl;
    std::cerr << "Second rotation = " << rotation2 << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
