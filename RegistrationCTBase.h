
#ifndef __ISIS_RegistrationCTBase_h__
#define __ISIS_RegistrationCTBase_h__

#include "itkImageRegistrationMethod.h"
#include "itkTranslationTransform.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkResampleImageFilter.h"
#include "itkImage.h"

namespace ISIS
{

class RegistrationCTBase
{

  itkStaticConstMacro( Dimension, unsigned int, 3 );

  typedef itk::RegularStepGradientDescentOptimizer        OptimizerType;
  typedef OptimizerType::Pointer                          OptimizerPointer;
  typedef itk::TranslationTransform< double, Dimension >  TransformType;
  typedef TransformType::Pointer                          TransformPointer;

public:
  RegistrationCTBase( void );

  virtual ~RegistrationCTBase( void );


protected:

  OptimizerPointer    m_Optimizer; 
  TransformPointer    m_Transform;

};


}  // end namespace ISIS

  

#endif



