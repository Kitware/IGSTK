#ifndef _IGSTK_FANTASTICREGISTRATION_H_
#define _IGSTK_FANTASTICREGISTRATION_H_

#include "itkCenteredAffineTransform.h"
#include "itkCumulativeGaussianOptimizer.h"
#include "itkEuler3DTransform.h"
#include "itkImage.h"
#include "itkImageRegistrationMethod.h"
#include "itkLevenbergMarquardtOptimizer.h"
#include "itkIterativeClosestPointMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkPointSet.h"
#include "itkPointSetToPointSetMetric.h"
#include "itkPointSetToPointSetRegistrationMethod.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkRigid3DTransform.h"
#include "itkTranslationTransform.h"

namespace IGSTK
{

class FantasticRegistration  
{
public:
  
  typedef itk::PointSet< double, 3 > PointSetType;
  
  typedef PointSetType::PointType     PointType;

  typedef itk::TranslationTransform< double, 3 > TTransformType;

  typedef itk::Euler3DTransform< double > E3DTransformType;

  typedef E3DTransformType::ParametersType E3DParametersType;

  typedef itk::CenteredAffineTransform <double, 3> CATransformType;

  typedef CATransformType::ParametersType CAParametersType;

  typedef itk::RegularStepGradientDescentOptimizer RSGDOptimizerType;

  typedef itk::LevenbergMarquardtOptimizer LMOptimizerType;

  typedef itk::CumulativeGaussianOptimizer CGOptimizerType;

  typedef itk::IterativeClosestPointMetric<PointSetType, PointSetType>
    ICPMetricType;
  
  typedef itk::PointSetToPointSetRegistrationMethod<PointSetType, PointSetType>
    PointSetToPointSetRegistrationType;  

public:
	void StartRegistration();
	void AddFixedPoint(double p[3]);

  void SetFixedPoint(int index, double p[3]);

  void AddMovingPoint(double p[3]);

  void SetMovingPoint(int index, double p[3]);

  PointSetType::Pointer m_FixedPointSet, m_MovingPointSet;

  ICPMetricType::Pointer m_ICPMetric;

  TTransformType::Pointer m_TTransform;

  E3DTransformType::Pointer m_E3DTransform;

  CATransformType::Pointer m_CATransform;

  RSGDOptimizerType::Pointer m_RSGDOptimizer;

  LMOptimizerType::Pointer m_LMOptimizer;

  CGOptimizerType::Pointer m_CGOptimizer;

  PointSetToPointSetRegistrationType::Pointer m_PointSetToPointSetRegistration;

	FantasticRegistration();

	virtual ~FantasticRegistration();

};

}

#endif 

