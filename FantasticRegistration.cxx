
#include "FantasticRegistration.h"

namespace IGSTK
{

FantasticRegistration::FantasticRegistration()
{
  m_FixedPointSet = PointSetType::New();

  m_MovingPointSet = PointSetType::New();

  m_ICPMetric = ICPMetricType::New();

  m_CATransform = CATransformType::New();

  m_E3DTransform = E3DTransformType::New();

  m_LMOptimizer = LMOptimizerType::New();
  m_LMOptimizer->SetNumberOfIterations(500);

  m_CGOptimizer = CGOptimizerType::New();
//  m_CGOptimizer->SetNumberOfIterations(500);

  m_PointSetToPointSetRegistration = PointSetToPointSetRegistrationType::New();

  m_PointSetToPointSetRegistration->SetMetric(m_ICPMetric);
  m_PointSetToPointSetRegistration->SetOptimizer(m_LMOptimizer);
  m_PointSetToPointSetRegistration->SetTransform(m_CATransform);

  unsigned int num = m_CATransform->GetNumberOfParameters();
  CAParametersType initialParameters(num);

  for (unsigned int i = 0; i < num; i++)
  {
    initialParameters[i] = 0.0;
  }

  m_PointSetToPointSetRegistration->SetInitialTransformParameters(initialParameters);

  m_PointSetToPointSetRegistration->SetFixedPointSet(m_FixedPointSet);
  m_PointSetToPointSetRegistration->SetMovingPointSet(m_MovingPointSet);

}

FantasticRegistration::~FantasticRegistration()
{

}

void FantasticRegistration::AddFixedPoint(double p[])
{

  this->SetFixedPoint( m_FixedPointSet->GetNumberOfPoints(), p);

}

void FantasticRegistration::SetFixedPoint(int index, double p[])
{
  PointType point;
  
  point[0] = p[0];
  point[1] = p[1];
  point[2] = p[2];

  m_FixedPointSet->SetPoint( index, point );

}

void FantasticRegistration::AddMovingPoint(double p[])
{

  this->SetMovingPoint( m_MovingPointSet->GetNumberOfPoints(), p);

}

void FantasticRegistration::SetMovingPoint(int index, double p[])
{
  PointType point;
  
  point[0] = p[0];
  point[1] = p[1];
  point[2] = p[2];

  m_MovingPointSet->SetPoint( index, point );

}

void FantasticRegistration::StartRegistration()
{
  m_PointSetToPointSetRegistration->StartRegistration();
}

}


