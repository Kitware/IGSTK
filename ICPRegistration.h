#ifndef _IGSTK_ICPRegistration_H_
#define _IGSTK_ICPRegistration_H_

#include "itkCenteredAffineTransform.h"
#include "itkCommand.h"
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

class ICPRegistration  
{
public:
  
  typedef itk::PointSet< double, 3 > PointSetType;
  
  typedef PointSetType::PointType     PointType;

  typedef PointSetType::PointsContainer PointsContainerType;

  typedef itk::TranslationTransform< double, 3 > TTransformType;

  typedef TTransformType::ParametersType TParametersType;

  typedef itk::Euler3DTransform< double > E3DTransformType;

  typedef E3DTransformType::ParametersType E3DParametersType;

  typedef itk::CenteredAffineTransform <double, 3> CATransformType;

  typedef CATransformType::ParametersType CAParametersType;

  typedef itk::Rigid3DTransform <double> R3DTransformType;

  typedef R3DTransformType::ParametersType R3DParametersType;

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

  void SetFixedPointSet(PointSetType::Pointer pointset);

  void AddMovingPoint(double p[3]);

  void SetMovingPoint(int index, double p[3]);

  void SetMovingPointSet(PointSetType::Pointer pointset);

  void SimplifyMovingPointSet(double radius);

  void SimplifyFixedPointSet(double radius);

  void SimplifyPointSet(PointSetType::Pointer pointset, double radius);
  
  ICPRegistration();

	virtual ~ICPRegistration();

  void OptimizerProgress();

  void SetNumberOfIterations( int );
  
  void SetValueTolerance( double );
  
  void SetGradientTolerance( double );

  void SetEpsilonFunction( double );

  E3DTransformType::Pointer GetTransform()
  {
    return m_E3DTransform;
  }

public:

  PointSetType::Pointer m_FixedPointSet, m_MovingPointSet;

  ICPMetricType::Pointer m_ICPMetric;

  TTransformType::Pointer m_TTransform;

  E3DTransformType::Pointer m_E3DTransform;

  CATransformType::Pointer m_CATransform;

  R3DTransformType::Pointer m_R3DTransform;

  RSGDOptimizerType::Pointer m_Optimizer;

  LMOptimizerType::Pointer m_LMOptimizer;

  CGOptimizerType::Pointer m_CGOptimizer;

  itk::SimpleMemberCommand<ICPRegistration>* m_OptimizerProgress;

  PointSetToPointSetRegistrationType::Pointer m_PointSetToPointSetRegistration;	

};

}

#endif 

