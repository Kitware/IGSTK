/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmark3DRegistrationTest.cxx
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
// Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include "igstkLandmark3DRegistration.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkObject.h"
#include "itkCommand.h"
#include "itkMacro.h"
#include "igstkEvents.h"
#include "igstkTransform.h"
#include "igstkTransformObserver.h"


class Landmark3DRegistrationErrorCallback : public itk::Command
{
public:
  typedef Landmark3DRegistrationErrorCallback Self;
  typedef itk::SmartPointer<Self>             Pointer;
  typedef itk::Command                        Superclass;
  itkNewMacro(Self);

  void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
    {
    }
  void Execute(itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
    {
    std::cerr<<"Error in transform computation"<<std::endl;
    }
protected:
  Landmark3DRegistrationErrorCallback() {};

private:
};

class Landmark3DRegistrationInvalidRequestCallback : public itk::Command
{
public:
  typedef Landmark3DRegistrationInvalidRequestCallback Self;
  typedef itk::SmartPointer<Self>                      Pointer;
  typedef itk::Command                                 Superclass;
  itkNewMacro(Self);
  void Execute(const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
    {
    }
  void Execute(itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event))
    {
    std::cerr<<"Invalid input request!!"<<std::endl;
    }
protected:
  Landmark3DRegistrationInvalidRequestCallback() {};

private:
};

class Landmark3DRegistrationGetRMSErrorCallback: public itk::Command
{
public:
  typedef Landmark3DRegistrationGetRMSErrorCallback  Self;
  typedef itk::SmartPointer<Self>                    Pointer;
  typedef itk::Command                               Superclass;
  itkNewMacro(Self);

  typedef igstk::DoubleTypeEvent DoubleTypeEventType;

  void Execute( const itk::Object * itkNotUsed(caller), const itk::EventObject & itkNotUsed(event) )
    {
    }
  void Execute( itk::Object * itkNotUsed(caller), const itk::EventObject & event )
    {
    std::cout<< " DoubleTypeEvent is thrown" << std::endl;
    const DoubleTypeEventType * errorEvent =
                  dynamic_cast < const DoubleTypeEventType* > ( &event );
    m_RMSError = errorEvent->Get();
    m_EventReceived = true;
    } 
  
  bool GetEventReceived()
    {
    return m_EventReceived;
    }
  
  igstk::Transform::ErrorType GetRMSError()
    {
    return m_RMSError;
    }  

protected:
  Landmark3DRegistrationGetRMSErrorCallback()   
    {
    m_EventReceived = true;
    }

private:
  bool m_EventReceived;
  igstk::Transform::ErrorType m_RMSError;
};


int igstkLandmark3DRegistrationTest( int , char * [] )
{
  igstk::RealTimeClock::Initialize();
  std::cout << "Testing igstk::Landmark3DRegistration" << std::endl;

  typedef igstk::Object::LoggerType     LoggerType;
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
  typedef igstk::Transform    TransformType;

  Landmark3DRegistrationType::Pointer landmarkRegister =
                                        Landmark3DRegistrationType::New();

  LandmarkPointContainerType  fpointcontainer;
  LandmarkPointContainerType  mpointcontainer;

  LandmarkImagePointType      fixedPoint;
  LandmarkTrackerPointType    movingPoint;

  // Add observer for invalid input request
  Landmark3DRegistrationInvalidRequestCallback::Pointer 
                     lrcb = Landmark3DRegistrationInvalidRequestCallback::New();

  typedef igstk::InvalidRequestErrorEvent  InvalidRequestEvent;
  landmarkRegister->AddObserver( InvalidRequestEvent(), lrcb );

  // Add observer for erro in computation 
  Landmark3DRegistrationErrorCallback::Pointer ecb = 
                                     Landmark3DRegistrationErrorCallback::New();

  typedef igstk::Landmark3DRegistration::TransformComputationFailureEvent 
                                                        ComputationFailureEvent;

  landmarkRegister->AddObserver( ComputationFailureEvent(), ecb );

  // logger object
  LoggerType::Pointer   logger = LoggerType::New();
  LogOutputType::Pointer logOutput = LogOutputType::New();
  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );
  landmarkRegister->SetLogger( logger );

  // Define the 3D rigid body transformation 
  typedef itk::VersorRigid3DTransform< double > Rigid3DTransformType;

  Rigid3DTransformType::Pointer rigid3DTransform = Rigid3DTransformType::New();

  Rigid3DTransformType::MatrixType mrotation;
  Rigid3DTransformType::VersorType vrotation;

  mrotation.SetIdentity();

  // define rotation angle
  const double angle = 40.0 * atan( 1.0f ) / 45.0;
  const double sinth = sin( angle );
  const double costh = cos( angle );

  // Rotation around the Z axis
  mrotation[0][0] =  costh;
  mrotation[0][1] =  sinth;
  mrotation[1][0] = -sinth;
  mrotation[1][1] =  costh;

  vrotation.Set(mrotation);
  rigid3DTransform->SetRotation( vrotation );

  // Apply translation
  Rigid3DTransformType::TranslationType translation;
  translation.Fill( 10.0f );
  rigid3DTransform->SetTranslation(translation);

  // Simulate invalid request
  landmarkRegister->RequestComputeTransform();

  // Set tolerance 
  landmarkRegister->RequestSetCollinearityTolerance( 0.01 );

  // Add 1st landmark
  fixedPoint[0] =  25.0;
  fixedPoint[1] =  1.0;
  fixedPoint[2] =  15.0;
  fpointcontainer.push_back(fixedPoint);
  landmarkRegister->RequestAddImageLandmarkPoint(fixedPoint);
  movingPoint = rigid3DTransform->TransformPoint(fixedPoint);
  mpointcontainer.push_back(movingPoint);
  landmarkRegister->RequestAddTrackerLandmarkPoint(movingPoint);

  // Add 2nd landmark
  fixedPoint[0] =  15.0;
  fixedPoint[1] =  21.0;
  fixedPoint[2] =  17.0;
  fpointcontainer.push_back(fixedPoint);
  landmarkRegister->RequestAddImageLandmarkPoint(fixedPoint);
  movingPoint = rigid3DTransform->TransformPoint(fixedPoint);
  mpointcontainer.push_back(movingPoint);
  landmarkRegister->RequestAddTrackerLandmarkPoint(movingPoint);

  // Add 3d landmark
  fixedPoint[0] =  14.0;
  fixedPoint[1] =  25.0;
  fixedPoint[2] =  11.0;
  fpointcontainer.push_back(fixedPoint);
  landmarkRegister->RequestAddImageLandmarkPoint(fixedPoint);
  movingPoint = rigid3DTransform->TransformPoint(fixedPoint);
  mpointcontainer.push_back(movingPoint);
  landmarkRegister->RequestAddTrackerLandmarkPoint(movingPoint);

  // Add 4th landmark
  fixedPoint[0] =  10.0;
  fixedPoint[1] =  11.0;
  fixedPoint[2] =  8.0;
  fpointcontainer.push_back(fixedPoint);
  landmarkRegister->RequestAddImageLandmarkPoint(fixedPoint);
  movingPoint = rigid3DTransform->TransformPoint(fixedPoint);
  mpointcontainer.push_back(movingPoint);
  landmarkRegister->RequestAddTrackerLandmarkPoint(movingPoint);

  OutputVectorType error;
  OutputVectorType::RealValueType tolerance = 0.00001;
  bool failed = false;
  landmarkRegister->Print(std::cout);
   
  // Calculate transform
  landmarkRegister->RequestComputeTransform();
 
  typedef itk::VersorRigid3DTransform<double>     VersorRigid3DTransformType;
  typedef itk::VersorRigid3DTransform<double>::ParametersType ParametersType;

  TransformType      transform;
  ParametersType     parameters(6);

  // Setup an obsever to get the transform parameters
  igstk::TransformObserver::Pointer lrtcb = igstk::TransformObserver::New();

  lrtcb->ObserveTransformEventsFrom( landmarkRegister );

  lrtcb->Clear();

  landmarkRegister->RequestGetTransformFromTrackerToImage();

  if( !lrtcb->GotTransform() )
    {
    std::cerr << "LandmarkRegsistration class failed to throw a modified "
              << "transform event" << std::endl;
    return EXIT_FAILURE;
    }
        
  transform = lrtcb->GetTransform();
  parameters[0] =    transform.GetRotation().GetX();
  parameters[1] =    transform.GetRotation().GetY();
  parameters[2] =    transform.GetRotation().GetZ();
  parameters[3] =    transform.GetTranslation()[0];
  parameters[4] =    transform.GetTranslation()[1];
  parameters[5] =    transform.GetTranslation()[2];
         
  VersorRigid3DTransformType::Pointer versorRigid3DTransform
                               = VersorRigid3DTransformType::New();
    
  versorRigid3DTransform->SetParameters(parameters);

  //Check if the transformation parameters were evaluated correctly
  Landmark3DRegistrationType::PointsContainerConstIterator
  mitr = mpointcontainer.begin();
  Landmark3DRegistrationType::PointsContainerConstIterator
  fitr = fpointcontainer.begin();

  OutputVectorType errortr;
  tolerance = 0.1;
  failed = false;

  while( mitr != mpointcontainer.end() )
    {
    std::cout << "  Tracker Landmark: " << *mitr << " image landmark " << *fitr
              << " Transformed trackerLandmark : " <<
    versorRigid3DTransform->TransformPoint( *mitr )  << std::endl;

    errortr = *fitr - versorRigid3DTransform->TransformPoint( *mitr );

    if( errortr.GetNorm() > tolerance )
      {
      failed = true;
      }
    ++mitr;
    ++fitr;
    }

  if( failed )
    {
    // Hang heads in shame
    std::cout << " Tracker landmarks transformed by the transform did not"
              << " match closely enough with the image image landmarks."
              << " The transform computed was: ";
    versorRigid3DTransform->Print(std::cout);
    std::cout << "  [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  else
    {
    std::cout << "  Landmark alignment using Rigid3D transform [PASSED]" 
              << std::endl;
    }

  //set up another observer to get transform from tracker to image
  igstk::TransformObserver::Pointer lrtcb2 = igstk::TransformObserver::New();

  lrtcb2->ObserveTransformEventsFrom( landmarkRegister );

  lrtcb2->Clear();

  landmarkRegister->RequestGetTransformFromImageToTracker();

  if( !lrtcb2->GotTransform() )
    {
    std::cerr << "LandmarkRegsistration class failed to throw a modified "
              << "transform event" << std::endl;
    return EXIT_FAILURE;
    }
        
  transform = lrtcb2->GetTransform();
  parameters[0] =    transform.GetRotation().GetX();
  parameters[1] =    transform.GetRotation().GetY();
  parameters[2] =    transform.GetRotation().GetZ();
  parameters[3] =    transform.GetTranslation()[0];
  parameters[4] =    transform.GetTranslation()[1];
  parameters[5] =    transform.GetTranslation()[2];
 
  VersorRigid3DTransformType::Pointer versorRigid3DTransform2
                               = VersorRigid3DTransformType::New();
    
  versorRigid3DTransform2->SetParameters(parameters);

  //Check if the transformation parameters were evaluated correctly
   Landmark3DRegistrationType::PointsContainerConstIterator 
                             mitr2 = mpointcontainer.begin();
   Landmark3DRegistrationType::PointsContainerConstIterator 
                          fitr2 = fpointcontainer.begin();

  tolerance = 0.1;
  failed = false;

  while( fitr2 != fpointcontainer.end() )
    {
    std::cout << "  Tracker Landmark: " << *mitr2 << " image landmark " << *fitr2
              << " Transformed image landmark : " <<
    versorRigid3DTransform2->TransformPoint( *fitr2 )  << std::endl;

    errortr = *mitr2 - versorRigid3DTransform2->TransformPoint( *fitr2 );

    if( errortr.GetNorm() > tolerance )
      {
      failed = true;
      }
    ++mitr2;
    ++fitr2;
    }

  if( failed )
    {
    // Hang heads in shame
    std::cout << " Tracker landmarks transformed by the transform did not"
              << " match closely enough with the image image landmarks."
              << " The transform computed was: ";
    versorRigid3DTransform2->Print(std::cout);
    std::cout << "  [FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  else
    {
    std::cout << "  Landmark alignment using Rigid3D transform [PASSED]" 
              << std::endl;
    }


  // Setup an obsever to get the RMS error 
  Landmark3DRegistrationGetRMSErrorCallback::Pointer lRmscb =
                            Landmark3DRegistrationGetRMSErrorCallback::New();

  landmarkRegister->AddObserver( igstk::DoubleTypeEvent(), lRmscb );
  landmarkRegister->RequestGetRMSError();

  if( !lRmscb->GetEventReceived() )
    {
    std::cerr << "LandmarkRegsistration class failed to throw a event "
              << "containing the RMS error " << std::endl;
    return EXIT_FAILURE;
    }
 
  std::cout << "RMS error= " << lRmscb->GetRMSError() << std::endl;

  // Test the Reset method
  landmarkRegister->RequestResetRegistration();

  // Test the transform computation with collinear points
    {
    std::cout << "Compute transform using collinear point" << std::endl;
    Rigid3DTransformType::Pointer   rigidTransform =
                                                    Rigid3DTransformType::New();
    rigidTransform->SetRotation( vrotation );
    rigidTransform->SetTranslation(translation);

    // Add 1st landmark
    fixedPoint[0] =  25.0;
    fixedPoint[1] =  11.0;
    fixedPoint[2] =  15.0;
    landmarkRegister->RequestAddImageLandmarkPoint(fixedPoint);
    movingPoint = rigidTransform->TransformPoint(fixedPoint);
    landmarkRegister->RequestAddTrackerLandmarkPoint(movingPoint);

    // Add 2nd landmark
    fixedPoint[0] =  15.0;
    fixedPoint[1] =  11.0;
    fixedPoint[2] =  15.0;
    landmarkRegister->RequestAddImageLandmarkPoint(fixedPoint);
    movingPoint = rigidTransform->TransformPoint(fixedPoint);
    landmarkRegister->RequestAddTrackerLandmarkPoint(movingPoint);

    // Add 3d landmark
    fixedPoint[0] =  14.0;
    fixedPoint[1] =  11.0;
    fixedPoint[2] =  15.0;
    landmarkRegister->RequestAddImageLandmarkPoint(fixedPoint);
    movingPoint = rigidTransform->TransformPoint(fixedPoint);
    landmarkRegister->RequestAddTrackerLandmarkPoint(movingPoint);
      
    landmarkRegister->Print(std::cout);
         
    Landmark3DRegistrationErrorCallback::Pointer ecb2 
                                   = Landmark3DRegistrationErrorCallback::New();

    landmarkRegister->AddObserver( ComputationFailureEvent(), ecb2 );
      
    // Calculate rigidTransform
    landmarkRegister->RequestComputeTransform();
    }
  return EXIT_SUCCESS;
}
