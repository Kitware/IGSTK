/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMR3DImageToUS3DImageRegistration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

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

namespace igstk
{


class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );

protected:
  CommandIterationUpdate() {};

public:

  typedef itk::RegularStepGradientDescentOptimizer     OptimizerType;
  typedef const OptimizerType                         *OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    Execute( (const itk::Object *)caller, event);
  }

  void Execute(const itk::Object * object, const itk::EventObject & event)
  {
    OptimizerPointer optimizer = 
                         dynamic_cast< OptimizerPointer >( object );

    if( ! itk::IterationEvent().CheckEvent( &event ) )
      {
      return;
      }

    std::cout << optimizer->GetCurrentIteration() << " = ";
    std::cout << optimizer->GetValue() << " : ";
    std::cout << optimizer->GetCurrentPosition() << std::endl;
  }
   
};

/** Constructor */
MR3DImageToUS3DImageRegistration::MR3DImageToUS3DImageRegistration() :
  m_StateMachine( this )
{
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

  // Add transition for RegistrationCalculated state
  igstkAddTransitionMacro( RegistrationCalculated, ResetRegistration, 
                           Idle, Reset );
  igstkAddTransitionMacro( RegistrationCalculated, ValidMovingMR3D, 
                           ImagesSet, SetMovingMR3D );
  igstkAddTransitionMacro( RegistrationCalculated, ValidFixedUS3D, 
                           ImagesSet, SetFixedUS3D );
  igstkAddTransitionMacro( RegistrationCalculated, CalculateRegistration, 
                           RegistrationCalculated, CalculateRegistration );
  igstkAddTransitionMacro( RegistrationCalculated, RequestRegistrationTransform, 
                           RegistrationCalculated, ReportRegistrationTransform );

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

  TransformModifiedEvent event;
  event.Set( this->m_RegistrationTransform );
  this->InvokeEvent( event );
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

  this->m_USFixedImage->RequestGetITKImage();

  if(!usImageObserver->GotITKUSImage())
    {
    return;
    }


  // Get the pointer to the ITK MR image
  ITKMRImageObserver::Pointer mrImageObserver = ITKMRImageObserver::New(); 
  this->m_MRMovingImage->AddObserver(
                  MRImageSpatialObject::ITKImageModifiedEvent(),mrImageObserver);

  this->m_MRMovingImage->RequestGetITKImage();

  if(!mrImageObserver->GotITKMRImage())
    {
    return;
    }
  
  // Fixed Image Type
  typedef USImageObject::ImageType            FixedImageType;
  //typedef MRImageSpatialObject::ImageType     FixedImageType;

  // Moving Image Type
  typedef MRImageSpatialObject::ImageType     MovingImageType;
  //typedef USImageObject::ImageType     MovingImageType;

  // Transform Type
  typedef itk::VersorRigid3DTransform< double > TransformType;

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
  TransformType::Pointer      transform     = TransformType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  FixedImageType::Pointer     fixedImage    = FixedImageType::New();  
  MovingImageType::Pointer    movingImage   = MovingImageType::New();  
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();
  
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );

  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetTransform(     transform     );
  registration->SetFixedImage( usImageObserver->GetITKUSImage() );
  registration->SetMovingImage( mrImageObserver->GetITKMRImage() );
  registration->SetInterpolator(  interpolator  );

  usImageObserver->GetITKUSImage()->Print(std::cout);
  mrImageObserver->GetITKMRImage()->Print(std::cout);

  typedef RegistrationType::ParametersType ParametersType;
  ParametersType initialParameters( transform->GetNumberOfParameters() );
  initialParameters.Fill(0);
  initialParameters[3] = 3;

  typedef OptimizerType::ScalesType ParameterScalesType;
  ParameterScalesType scales( transform->GetNumberOfParameters());
  scales.Fill(1);
  scales[0] = 10000;
  scales[1] = 10000;
  scales[2] = 10000;

  optimizer->SetScales(scales);
  registration->SetInitialTransformParameters( initialParameters );
  registration->StartRegistration();
  ParametersType params = registration->GetLastTransformParameters();

  // Reset the calibration transform (rotation and translation)
  VersorType quaternion;
  VectorType translation;

  quaternion.SetIdentity();
  translation[0] = params[3];
  translation[1] = params[4];
  translation[2] = params[5];
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
  this->ObserveMRImageTransformInput(this->m_USFixedImage);

  const_cast<USImageObject*>(this->m_USFixedImage)->RequestGetTransform();
  const_cast<MRImageSpatialObject*>(this->m_MRMovingImage)->RequestGetTransform();

  this->m_StateMachine.PushInput( this->m_CalculateRegistrationInput );
  this->m_StateMachine.ProcessInputs();
}

/** Method to invoke the SetMovingMR3D function */
void MR3DImageToUS3DImageRegistration::RequestSetMovingMR3D(MRImageSpatialObject* MRImage)
{
  igstkLogMacro( DEBUG, "igstk::RequestSetMovingMR3D\
                        ::RequestReset called...\n" );

  this->m_MRMovingImageToBeSet = MRImage;

  this->m_StateMachine.PushInput( this->m_ValidMovingMR3DInput );
  this->m_StateMachine.ProcessInputs();
}

/** Method to invoke the SetFixedUS3D function */
void MR3DImageToUS3DImageRegistration::RequestSetFixedUS3D(USImageObject* USImage)
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
