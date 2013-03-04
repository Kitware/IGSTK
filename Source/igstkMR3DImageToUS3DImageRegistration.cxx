/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMR3DImageToUS3DImageRegistration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _igstkMR3DImageToUS3DImageRegistration_cxx
#define _igstkMR3DImageToUS3DImageRegistration_cxx


#include "igstkMR3DImageToUS3DImageRegistration.h"
#include "itkVersorRigid3DTransform.h"

#include "itkImageRegistrationMethod.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkRegularStepGradientDescentOptimizer.h"

#include "itkImageFileWriter.h"

namespace igstk
{

namespace MR3DImageToUS3DImageRegistrationHelper
{

class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>   Pointer;
  itkNewMacro( Self );

protected:
  CommandIterationUpdate() {};

public:

  typedef itk::RegularStepGradientDescentOptimizer   OptimizerType;
  typedef const OptimizerType*                       OptimizerPointer;
  typedef ::igstk::Object::LoggerType                LoggerType; 

  void SetLogger( LoggerType * logger )
    {
    m_Logger = logger;
    }
  LoggerType * GetLogger()
    {
    return m_Logger;
    }
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
    OptimizerPointer optimizer = dynamic_cast< OptimizerPointer >( object );

    if( ! itk::IterationEvent().CheckEvent( &event ) )
      {
      return;
      }

    igstkLogMacro( DEBUG, "igstk::MR3DImageToUS3DImageRegistration "
            << optimizer->GetCurrentIteration() << " = "
            << optimizer->GetValue() << " : "
            << optimizer->GetCurrentPosition() << "\n");
    }
private:
  LoggerType::Pointer    m_Logger; 
};

}

/** Constructor */
MR3DImageToUS3DImageRegistration::MR3DImageToUS3DImageRegistration() :
  m_StateMachine( this )
{
  m_InitialTransform.SetToIdentity(10000);

  // Set the state descriptors
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( MRImageSet );
  igstkAddStateMacro( USImageSet );
  igstkAddStateMacro( ImagesSet );
  igstkAddStateMacro( RegistrationCalculated );

  // Set the input descriptors 
  igstkAddInputMacro( ResetRegistration );
  igstkAddInputMacro( ValidMovingMR3D );
  igstkAddInputMacro( ValidFixedUS3D );
  igstkAddInputMacro( ValidRegistration);
  igstkAddInputMacro( ResetRegistration );
  igstkAddInputMacro( CalculateRegistration );
  igstkAddInputMacro( RequestRegistrationTransform );
  igstkAddInputMacro( MRImageTransform );
  igstkAddInputMacro( USImageTransform  );

  // Add transition  for idle state
  igstkAddTransitionMacro( Idle, ResetRegistration, Idle, Reset );
  igstkAddTransitionMacro( Idle, ValidMovingMR3D, MRImageSet, SetMovingMR3D );
  igstkAddTransitionMacro( Idle, ValidFixedUS3D, USImageSet, SetFixedUS3D );
  igstkAddTransitionMacro( Idle, CalculateRegistration, Idle, No );
  igstkAddTransitionMacro( Idle, CalculateRegistration, Idle, No );

  // Add transition for MRImageSet state
  igstkAddTransitionMacro( MRImageSet, ResetRegistration, Idle, Reset );
  igstkAddTransitionMacro( MRImageSet, ValidMovingMR3D, 
                           MRImageSet, SetMovingMR3D );
  igstkAddTransitionMacro( MRImageSet, ValidFixedUS3D, 
                           ImagesSet, SetFixedUS3D );
  igstkAddTransitionMacro( MRImageSet, CalculateRegistration, 
                           MRImageSet, No );
  igstkAddTransitionMacro( MRImageSet, RequestRegistrationTransform, 
                           MRImageSet, No );

  // Add transition for USImageSet state
  igstkAddTransitionMacro( USImageSet, ResetRegistration, Idle, Reset );
  igstkAddTransitionMacro( USImageSet, ValidMovingMR3D, 
                           ImagesSet, SetMovingMR3D );
  igstkAddTransitionMacro( USImageSet, ValidFixedUS3D, 
                           USImageSet, SetFixedUS3D );
  igstkAddTransitionMacro( USImageSet, CalculateRegistration, 
                           USImageSet, No );
  igstkAddTransitionMacro( USImageSet, RequestRegistrationTransform, 
                           USImageSet, No );

  // Add transition for USImageSet state
  igstkAddTransitionMacro( ImagesSet, ResetRegistration, Idle, Reset );
  igstkAddTransitionMacro( ImagesSet, ValidMovingMR3D, 
                           ImagesSet, SetMovingMR3D );
  igstkAddTransitionMacro( ImagesSet, ValidFixedUS3D,
                           ImagesSet, SetFixedUS3D );
  igstkAddTransitionMacro( ImagesSet, CalculateRegistration, 
                           ImagesSet, CalculateRegistration );
  igstkAddTransitionMacro( ImagesSet, ValidRegistration, 
                           RegistrationCalculated, No );
  igstkAddTransitionMacro( ImagesSet, RequestRegistrationTransform, 
                           ImagesSet, No );
  igstkAddTransitionMacro( ImagesSet, MRImageTransform,
                           ImagesSet, No );
  igstkAddTransitionMacro( ImagesSet, USImageTransform,
                           ImagesSet, No );

  // Add transition for RegistrationCalculated state
  igstkAddTransitionMacro( RegistrationCalculated, ResetRegistration, 
                           Idle, Reset );
  igstkAddTransitionMacro( RegistrationCalculated, ValidMovingMR3D, 
                           ImagesSet, SetMovingMR3D );
  igstkAddTransitionMacro( RegistrationCalculated, ValidFixedUS3D, 
                           ImagesSet, SetFixedUS3D );
  igstkAddTransitionMacro( RegistrationCalculated, CalculateRegistration, 
                           RegistrationCalculated, CalculateRegistration );
  igstkAddTransitionMacro( RegistrationCalculated, 
                           RequestRegistrationTransform, 
                           RegistrationCalculated, 
                           ReportRegistrationTransform );

  // Select the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

  // Finish the programming and get ready to run
  this->m_StateMachine.SetReadyToRun();

  // ResetProcessing the initial state and variables
  this->ResetProcessing();

}

/** Destructor */
MR3DImageToUS3DImageRegistration::~MR3DImageToUS3DImageRegistration()
{
}

/** Print Self function */
void MR3DImageToUS3DImageRegistration
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


void MR3DImageToUS3DImageRegistration
::RequestGetRegistrationTransform() 
{
  igstkLogMacro( DEBUG, "igstk::MR3DImageToUS3DImageRegistration\
                      ::RequestGetRegistrationTransform called...\n" );

  m_StateMachine.PushInput( m_RequestRegistrationTransformInput );
  m_StateMachine.ProcessInputs();
}

void MR3DImageToUS3DImageRegistration
::ReportRegistrationTransformProcessing() 
{
  igstkLogMacro( DEBUG, "igstk::MR3DImageToUS3DImageRegistration\
                      ::ReportRegistrationTransformProcessing called...\n" );

  CoordinateSystemTransformToResult transformCarrier;

  transformCarrier.Initialize( this->m_RegistrationTransform,
   this->m_MRMovingImage,
   this->m_USFixedImage 
   );

  CoordinateSystemTransformToEvent transformEvent;

  transformEvent.Set( transformCarrier );

  this->InvokeEvent( transformEvent );
}


/** Method to NoProcessing */
void MR3DImageToUS3DImageRegistration::NoProcessing()
{
  igstkLogMacro( DEBUG, "igstk::MR3DImageToUS3DImageRegistration\
                        ::NoProcessing called...\n" );

}

/** Method to reset the calibration */
void MR3DImageToUS3DImageRegistration::ResetProcessing()
{
  igstkLogMacro( DEBUG, "igstk::MR3DImageToUS3DImageRegistration\
                        ::ResetProcessing called...\n" );

  VersorType quaternion;
  VectorType translation;

  // Reset the calibration transform (rotation and translation)
  quaternion.SetIdentity();
  translation.Fill( 0.0);
  this->m_RegistrationTransform.SetTranslationAndRotation( translation, 
                                                          quaternion, 
                                                          0.1, 1000);
  // Reset the RMS calibration error
  this->m_RootMeanSquareError = 0.0;

  // Reset the validation indicator
  this->m_ValidMR3DImageToUS3DImageRegistration = false;

  this->m_USFixedImageToBeSet = 0;
  this->m_USFixedImage = 0;
  this->m_MRMovingImageToBeSet = 0;
  this->m_MRMovingImage = 0;
}


/** Method to invoke the ResetProcessing function */
void MR3DImageToUS3DImageRegistration::RequestReset()
{
  igstkLogMacro( DEBUG, "igstk::MR3DImageToUS3DImageRegistration\
                        ::RequestReset called...\n" );

  this->m_StateMachine.PushInput( this->m_ResetRegistrationInput );
  this->m_StateMachine.ProcessInputs();
}
 
 
/** Compute the registration transform */
void MR3DImageToUS3DImageRegistration::CalculateRegistrationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::MR3DImageToUS3DImageRegistration\
                        ::CalculateRegistrationProcessing called...\n" );


  // Get the pointer to the ITK US image
  ITKUSImageObserver::Pointer usImageObserver = ITKUSImageObserver::New();
  this->m_USFixedImage->AddObserver(
                  USImageObject::ITKImageModifiedEvent(),usImageObserver);

  usImageObserver->Reset();
  this->m_USFixedImage->RequestGetITKImage();

  if( !usImageObserver->GotITKUSImage() )
    {
    igstkLogMacro( CRITICAL, "igstk::MR3DImageToUS3DImageRegistration\
                               No US Image!\n" );
    return;
    }


  // Get the pointer to the ITK MR image
  ITKMRImageObserver::Pointer mrImageObserver = ITKMRImageObserver::New(); 
  this->m_MRMovingImage->AddObserver(
             MRImageSpatialObject::ITKImageModifiedEvent(),mrImageObserver);

  mrImageObserver->Reset();
  this->m_MRMovingImage->RequestGetITKImage();

  if(!mrImageObserver->GotITKMRImage())
    {
    igstkLogMacro( CRITICAL, "igstk::MR3DImageToUS3DImageRegistration\
                               No US Image!\n" );
    return;
    }
  
  // Fixed Image Type
  typedef USImageObject::ImageType            FixedImageType;
  
  // Moving Image Type
  typedef MRImageSpatialObject::ImageType     MovingImageType;
 
  // Transform Type
  typedef itk::VersorRigid3DTransform< double > VersorRigidTransformType;

  // Optimizer Type
  typedef itk::RegularStepGradientDescentOptimizer       OptimizerType;

  // Metric Type
  typedef itk::MeanSquaresImageToImageMetric< 
                                    FixedImageType, 
                                    MovingImageType >    MetricType;

  // Interpolation technique
  typedef itk:: LinearInterpolateImageFunction< 
                                    MovingImageType,
                                    double          >    InterpolatorType;

  // Registration Method
  typedef itk::ImageRegistrationMethod< 
                                    FixedImageType, 
                                    MovingImageType >    RegistrationType;


  MetricType::Pointer         metric        = MetricType::New();

  VersorRigidTransformType::Pointer    transform   = 
                                VersorRigidTransformType::New();

  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  FixedImageType::Pointer     fixedImage    = FixedImageType::New();  
  MovingImageType::Pointer    movingImage   = MovingImageType::New();  
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();
  
  typedef MR3DImageToUS3DImageRegistrationHelper::CommandIterationUpdate 
                                                                 ObserverType;
    
  ObserverType::Pointer observer = ObserverType::New();

  observer->SetLogger( this->GetLogger() );

  optimizer->AddObserver( itk::IterationEvent(), observer );
 
  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetTransform(     transform     );
  registration->SetFixedImage( usImageObserver->GetITKUSImage() );
  registration->SetMovingImage( mrImageObserver->GetITKMRImage() );
  registration->SetInterpolator(  interpolator  );

  // Here we should get the transforms of the images and use it to initialize
  // the registration
  USImageTransformObserver::Pointer usTransformObserver 
                                    = USImageTransformObserver::New();
  m_USFixedImage->AddObserver( CoordinateSystemTransformToEvent(),
                                           usTransformObserver );

  m_USFixedImage->RequestGetImageTransform();

  Transform usTransform;
  if( usTransformObserver->GotUSImageTransform() )
    {
    const CoordinateSystemTransformToResult transformCarrier =
      usTransformObserver->GetUSImageTransform();
    usTransform = transformCarrier.GetTransform();
    }
  else
    {
    igstkLogMacro( CRITICAL, "igstk::MR3DImageToUS3DImageRegistration"
        << " Observer did not receive expected Transform\n" );
    }

  typedef RegistrationType::ParametersType ParametersType;
  ParametersType initialParameters( transform->GetNumberOfParameters() );
  initialParameters.Fill(0);
  initialParameters[0] = m_InitialTransform.GetRotation().GetX();
  initialParameters[1] = m_InitialTransform.GetRotation().GetY();
  initialParameters[2] = m_InitialTransform.GetRotation().GetZ();
  initialParameters[3] = usTransform.GetTranslation()[0]
                                      +m_InitialTransform.GetTranslation()[0];
  initialParameters[4] = usTransform.GetTranslation()[1]
                                      +m_InitialTransform.GetTranslation()[1];
  initialParameters[5] = usTransform.GetTranslation()[2]
                                      +m_InitialTransform.GetTranslation()[2];

  typedef OptimizerType::ScalesType ParameterScalesType;
  ParameterScalesType scales( transform->GetNumberOfParameters());
  scales.Fill(1000);
  scales[0] = 1000000000;
  scales[1] = 1000000000;
  scales[2] = 1000000000;

  optimizer->SetScales(scales);
  registration->SetInitialTransformParameters( initialParameters );
  try
    {
	    registration->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    igstkLogMacro( CRITICAL, "igstk::MR3DImageToUS3DImageRegistration" 
                             << excp.GetDescription() << "\n" );
    return;
    }
  ParametersType params = registration->GetLastTransformParameters();

  // Reset the calibration transform (rotation and translation)
  VersorType quaternion;
  VersorType initQuaternion;
  VectorType translation;
  VectorType initTranslation;

  VersorType::VectorType axis;

  ParametersType finalparams = params;
  
  // We have the following transforms:
  // 1) Transform found by optimization of the metric (we'll call it "C")
  // 2) The initial transform (we'll call it "A")
  // We want to find the transform "B" that, when combined with the
  // initial transform "A", will produce the optimized transform "C",
  // i.e.  B * A = C
  // so    B = C * A^(-1)
  // therefore we need to find the inverse of "A", and transform it by "C"
  

  // Create the quaternion for the optimized transform "C"
  double norm = finalparams[0]*finalparams[0];
  axis[0] = finalparams[0];
  norm += finalparams[1]*finalparams[1];
  axis[1] = finalparams[1];
  norm += finalparams[2]*finalparams[2];
  axis[2] = finalparams[2];
  if( norm > 0)
    {
    norm = vcl_sqrt(norm);
    }

  double epsilon = 1e-10;
  if(norm >= 1.0-epsilon)
    {
    axis = axis / (norm+epsilon*norm);
    }
  quaternion.Set(axis);

  // Compute the quaternion for the initial transform "A"
  norm = initialParameters[0]*initialParameters[0];
  axis[0] = initialParameters[0];
  norm += initialParameters[1]*initialParameters[1];
  axis[1] = initialParameters[1];
  norm += initialParameters[2]*initialParameters[2];
  axis[2] = initialParameters[2];
  if( norm > 0)
    {
    norm = vcl_sqrt(norm);
    }

  if(norm >= 1.0-epsilon)
    {
    axis = axis / (norm+epsilon*norm);
    }
  initQuaternion.Set(axis);

  // Get the translation for the optimized transform "C"
  translation[0] = finalparams[3];
  translation[1] = finalparams[4];
  translation[2] = finalparams[5];

  // Get the translation for the initial transform "A"
  initTranslation[0] = initialParameters[3];
  initTranslation[1] = initialParameters[4];
  initTranslation[2] = initialParameters[5];
  
  // Compute B = C * A^(-1)
  quaternion /= initQuaternion;
  translation -= initTranslation;

  // Create the transform
  this->m_RegistrationTransform.SetTranslationAndRotation( translation, 
                                                          quaternion, 
                                                          0.1, 1000);

  this->m_StateMachine.PushInput( this->m_ValidRegistrationInput );
  this->m_StateMachine.ProcessInputs();
}

/** Method to invoke the calculation */
void MR3DImageToUS3DImageRegistration::RequestCalculateRegistration()
{
  igstkLogMacro( DEBUG, "igstk::MR3DImageToUS3DImageRegistration\
                        ::RequestCalculateRegistration called...\n" );

  // Try to get the transform from the two images
  this->ObserveUSImageTransformInput(this->m_USFixedImage);
  this->ObserveMRImageTransformInput(this->m_MRMovingImage);

  const_cast<USImageObject*>(this->m_USFixedImage)->RequestGetImageTransform();
  const_cast<MRImageSpatialObject*>(
                            this->m_MRMovingImage)->RequestGetImageTransform();

  this->m_StateMachine.PushInput( this->m_CalculateRegistrationInput );
  this->m_StateMachine.ProcessInputs();
}

/** Method to invoke the SetMovingMR3D function */
void MR3DImageToUS3DImageRegistration
::RequestSetMovingMR3D(MRImageSpatialObject* MRImage)
{
  igstkLogMacro( DEBUG, "igstk::RequestSetMovingMR3D\
                        ::RequestReset called...\n" );

  this->m_MRMovingImageToBeSet = MRImage;

  this->m_StateMachine.PushInput( this->m_ValidMovingMR3DInput );
  this->m_StateMachine.ProcessInputs();
}

/** Method to invoke the SetFixedUS3D function */
void MR3DImageToUS3DImageRegistration
::RequestSetFixedUS3D(USImageObject* USImage)
{
  igstkLogMacro( DEBUG, "igstk::RequestSetFixedUS3D\
                        ::RequestReset called...\n" );

  this->m_USFixedImageToBeSet = USImage;

  this->m_StateMachine.PushInput( this->m_ValidFixedUS3DInput );
  this->m_StateMachine.ProcessInputs();
}

/** Set the 3D fixed US image */
void MR3DImageToUS3DImageRegistration::SetFixedUS3DProcessing()
{
  igstkLogMacro( DEBUG, "igstk::SetFixedUS3DProcessing\
                        ::RequestReset called...\n" );

  this->m_USFixedImage = this->m_USFixedImageToBeSet;
}

/** Set the 3D moving MR image */
void MR3DImageToUS3DImageRegistration::SetMovingMR3DProcessing()
{
  igstkLogMacro( DEBUG, "igstk::SetMovingMR3DProcessing\
                        ::RequestReset called...\n" );

  this->m_MRMovingImage = this->m_MRMovingImageToBeSet;
}


} // end namespace igstk

#endif
