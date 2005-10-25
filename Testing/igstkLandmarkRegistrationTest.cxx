/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmarkRegistrationTest.cxx
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
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include "igstkLandmarkRegistration.h"
#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkObject.h"
#include "itkCommand.h"
#include "itkMacro.h"


class LandmarkRegistrationErrorCallback : public itk::Command
{
public:
  typedef LandmarkRegistrationErrorCallback Self;
  typedef itk::SmartPointer<Self>      Pointer;
  typedef itk::Command                 Superclass;
  itkNewMacro(Self);
  void Execute(const itk::Object *caller, const itk::EventObject & event)
  {

  }
  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    std::cerr<<"Error in transform computation"<<std::endl;
  }
protected:
  LandmarkRegistrationErrorCallback()   { };

private:
};

class LandmarkRegistrationInvalidRequestCallback : public itk::Command
{
public:
  typedef LandmarkRegistrationInvalidRequestCallback Self;
  typedef itk::SmartPointer<Self>      Pointer;
  typedef itk::Command                 Superclass;
  itkNewMacro(Self);
  void Execute(const itk::Object *caller, const itk::EventObject & event)
  {

  }
  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    std::cerr<<"Invalid input request!!"<<std::endl;
  }
protected:
  LandmarkRegistrationInvalidRequestCallback()   { };

private:
};

class LandmarkRegistrationGetTransformCallback: public itk::Command
{
  public:
    typedef LandmarkRegistrationGetTransformCallback    Self;
    typedef itk::SmartPointer<Self>                     Pointer;
    typedef itk::Command                                Superclass;
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
      m_Transform = transformEvent->GetTransform();
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
    LandmarkRegistrationGetTransformCallback()   
    {
      m_EventReceived = true;
    };
  private:
      bool m_EventReceived;
      igstk::Transform m_Transform;
};

int igstkLandmarkRegistrationTest( int argv, char * argc[] )
{
    std::cout << "Testing igstk::LandmarkRegistration" << std::endl;

    typedef itk::Logger                   LoggerType;
    typedef itk::StdStreamLogOutput       LogOutputType;
    
    typedef igstk::LandmarkRegistration<3>     
                              LandmarkRegistrationType;
    typedef igstk::LandmarkRegistration<3>::LandmarkPointContainerType
                              LandmarkPointContainerType;
    typedef igstk::LandmarkRegistration<3>::LandmarkImagePointType 
                              LandmarkImagePointType;
    typedef igstk::LandmarkRegistration<3>::LandmarkTrackerPointType
                              LandmarkTrackerPointType;
    typedef LandmarkRegistrationType::TransformType::OutputVectorType 
                              OutputVectorType;
    typedef igstk::Transform  TransformType;

    LandmarkRegistrationType::Pointer landmarkRegister = 
                                        LandmarkRegistrationType::New();    

    LandmarkPointContainerType  fpointcontainer;
    LandmarkPointContainerType  mpointcontainer;

    LandmarkImagePointType      fixedPoint;
    LandmarkTrackerPointType    movingPoint;

   //Add observer for invalid input request
    LandmarkRegistrationInvalidRequestCallback::Pointer 
                              lrcb = LandmarkRegistrationInvalidRequestCallback::New();

    typedef igstk::LandmarkRegistration<3>::InvalidRequestErrorEvent  InvalidRequestEvent;
    landmarkRegister->AddObserver( InvalidRequestEvent(), lrcb );

   //Add observer for erro in computation 
    LandmarkRegistrationErrorCallback::Pointer ecb = LandmarkRegistrationErrorCallback::New();

    typedef igstk::LandmarkRegistration<3>::TransformComputationFailureEvent ComputationFailureEvent;

    landmarkRegister->AddObserver( ComputationFailureEvent(), ecb );

// logger object
    LoggerType::Pointer   logger = LoggerType::New();
    LogOutputType::Pointer logOutput = LogOutputType::New();
    logOutput->SetStream( std::cout );
    logger->AddLogOutput( logOutput );
    logger->SetPriorityLevel( itk::Logger::DEBUG );
    landmarkRegister->SetLogger( logger );


    // Define the 3D rigid body transformation 
    typedef itk::Rigid3DTransform< double > Rigid3DTransformType;

    Rigid3DTransformType::Pointer   rigid3DTransform = Rigid3DTransformType::New();

    Rigid3DTransformType::MatrixType mrotation;

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

    rigid3DTransform->SetRotationMatrix( mrotation );

    // Apply translation

    Rigid3DTransformType::TranslationType translation;
    translation.Fill( 10.0f );
    rigid3DTransform->SetTranslation(translation);


    // Simulate invalid request
    landmarkRegister->RequestComputeTransform();

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
    landmarkRegister->RequestAddTrackerLandmarkPoint(movingPoint);

    OutputVectorType error;
    OutputVectorType::RealValueType tolerance = 0.00001;
    bool failed = false;
    landmarkRegister->Print(std::cout);
    
     // Calculate transform
    landmarkRegister->RequestComputeTransform();
 
    typedef itk::VersorRigid3DTransform<double>                
                                        VersorRigid3DTransformType;
    typedef itk::VersorRigid3DTransform<double>::ParametersType 
                                        ParametersType;

    TransformType      transform;
    ParametersType     parameters(6);

     // Setup an obsever to get the transform parameters
    LandmarkRegistrationGetTransformCallback::Pointer lrtcb =
                            LandmarkRegistrationGetTransformCallback::New();

    landmarkRegister->AddObserver( igstk::TransformModifiedEvent(), lrtcb );
    landmarkRegister->RequestGetTransform();

    if( !lrtcb->GetEventReceived() )
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
    LandmarkRegistrationType::PointsContainerConstIterator
      mitr = mpointcontainer.begin();
    LandmarkRegistrationType::PointsContainerConstIterator
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
      std::cout << "  Tracker landmarks transformed by the transform did not match closely "
        << " enough with the image image landmarks.  The transform computed was: ";
      versorRigid3DTransform->Print(std::cout);
      std::cout << "  [FAILED]" << std::endl;
      return EXIT_FAILURE;
      }
    else
      {
      std::cout << "  Landmark alignment using Rigid3D transform [PASSED]" << std::endl;
      }


    // Exercise the Reset method
    landmarkRegister->RequestResetRegistration();


    return EXIT_SUCCESS;
}


