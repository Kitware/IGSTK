
#include "FantasticRegistration.h"

namespace IGSTK
{

FantasticRegistration::FantasticRegistration()
{
  m_FixedPointSet = PointSetType::New();

  m_MovingPointSet = PointSetType::New();

  m_ICPMetric = ICPMetricType::New();

  m_TTransform = TTransformType::New();

  m_CATransform = CATransformType::New();

  m_E3DTransform = E3DTransformType::New();

  m_R3DTransform = R3DTransformType::New();

  m_LMOptimizer = LMOptimizerType::New();
  m_LMOptimizer->SetNumberOfIterations(500);
  m_LMOptimizer->SetUseCostFunctionGradient(false);
  

  m_CGOptimizer = CGOptimizerType::New();
//  m_CGOptimizer->SetNumberOfIterations(500);

  m_Optimizer = RSGDOptimizerType::New();

  m_PointSetToPointSetRegistration = PointSetToPointSetRegistrationType::New();

  m_PointSetToPointSetRegistration->SetMetric(m_ICPMetric);
  m_PointSetToPointSetRegistration->SetOptimizer( m_LMOptimizer);
  m_PointSetToPointSetRegistration->SetTransform(m_E3DTransform);

  LMOptimizerType::ScalesType scales( m_E3DTransform->GetNumberOfParameters() );
  scales.Fill( 0.001 );
  
  unsigned long   numberOfIterations =  1000;
  double          gradientTolerance  =  1e-4;   // convergence criterion
  double          valueTolerance     =  1e-4;   // convergence criterion
  double          epsilonFunction    =  1e-6;   // convergence criterion

  m_LMOptimizer->SetScales( scales );
  m_LMOptimizer->SetNumberOfIterations( numberOfIterations );
  m_LMOptimizer->SetValueTolerance( valueTolerance );
  m_LMOptimizer->SetGradientTolerance( gradientTolerance );
  m_LMOptimizer->SetEpsilonFunction( epsilonFunction );

  // Start from an Identity transform (in a normal case, the user 
  // can probably provide a better guess than the identity...
//  m_TTransform->SetIdentity();
  m_E3DTransform->SetIdentity();

  unsigned int num = m_E3DTransform->GetNumberOfParameters();
/*  CAParametersType initialParameters(num);

  for (unsigned int i = 0; i < num; i++)
  {
    initialParameters[i] = 0.0;
  }
*/
//  m_PointSetToPointSetRegistration->SetInitialTransformParameters(m_TTransform->GetParameters());
  m_PointSetToPointSetRegistration->SetInitialTransformParameters(m_E3DTransform->GetParameters());

  m_PointSetToPointSetRegistration->SetFixedPointSet(m_FixedPointSet);
  m_PointSetToPointSetRegistration->SetMovingPointSet(m_MovingPointSet);

//  m_OptimizerProgress = itk::SimpleMemberCommand<FantasticRegistration>::New();
//  m_OptimizerProgress->SetCallbackFunction(this, FantasticRegistration::OptimizerProgress);
//  m_CGOptimizer->AddObserver(itk::IterationEvent(), m_OptimizerProgress);

}

FantasticRegistration::~FantasticRegistration()
{

}

void FantasticRegistration::OptimizerProgress()
{
  //std::cout<<m_LMOptimizer->GetCurrentIteration()<<" = ";
  //std::cout<<m_LMOptimizer->GetValue()<<" : ";
  std::cout<<m_LMOptimizer->GetCurrentPosition()<<std::endl;
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

void FantasticRegistration::SetFixedPointSet(PointSetType::Pointer pointset)
{

  m_FixedPointSet->SetPoints(pointset->GetPoints());

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

void FantasticRegistration::SetMovingPointSet(PointSetType::Pointer pointset)
{

  m_MovingPointSet->SetPoints(pointset->GetPoints());

}

void FantasticRegistration::SimplifyMovingPointSet(double radius)
{
  unsigned int i, j, num = m_MovingPointSet->GetNumberOfPoints(), k;
  double d[3], dis, dd =  radius * radius;
  PointsContainerType::Pointer pc = PointsContainerType::New();
  PointSetType::PointType point1, point2;
  unsigned int it = 0;

  m_MovingPointSet->GetPoint(0, &point1);
  pc->InsertElement(it, point1);
  it++;
  k = 1;
  for (i = 1; i < num; i++)
  {
    m_MovingPointSet->GetPoint(i, &point2);
    dis = 0;
    for (j = 0; j < 3; j++)
    {
      d[j] = point1[j] - point2[j];
      dis += d[j] * d[j];
    }
    if (dis > dd)
    {
      pc->InsertElement(it, point2);
      it++;
      point1 = point2;
      k = 1;
    }
    else
    {
      for (j = 0; j < 3; j++)
      {
        point1[j] = (point1[j] * k + point2[j]) / (k + 1);
        k++;
      }
    }
  }

  m_MovingPointSet->SetPoints(pc);
}

void FantasticRegistration::SimplifyFixedPointSet(double radius)
{
  unsigned int i, j, num = m_FixedPointSet->GetNumberOfPoints(), k;
  double d[3], dis, dd =  radius * radius;
  PointsContainerType::Pointer pc = PointsContainerType::New();
  PointSetType::PointType point1, point2;
  unsigned int it = 0;

  m_FixedPointSet->GetPoint(0, &point1);
  pc->InsertElement(it, point1);
  it++;
  k = 1;
  for (i = 1; i < num; i++)
  {
    m_FixedPointSet->GetPoint(i, &point2);
    dis = 0;
    for (j = 0; j < 3; j++)
    {
      d[j] = point1[j] - point2[j];
      dis += d[j] * d[j];
    }
    if (dis > dd)
    {
      pc->InsertElement(it, point2);
      it++;
      point1 = point2;
      k = 1;
    }
    else
    {
      for (j = 0; j < 3; j++)
      {
        point1[j] = (point1[j] * k + point2[j]) / (k + 1);
        k++;
      }
    }
  }

  m_FixedPointSet->SetPoints(pc);
}

/*
void FantasticRegistration::SimplifyMovingPointSet(double radius)
{
  unsigned int i, j, num = m_MovingPointSet->GetNumberOfPoints();
  double d[3], dis, dd =  radius * radius;
  PointsContainerType::Pointer pc = PointsContainerType::New();
  PointSetType::PointType point1, point2;
  unsigned int it = 0;

  m_MovingPointSet->GetPoint(0, &point1);
  pc->InsertElement(it, point1);
  it++;
  for (i = 1; i < num; i++)
  {
    m_MovingPointSet->GetPoint(i, &point2);
    dis = 0;
    for (j = 0; j < 3; j++)
    {
      d[j] = point1[j] - point2[j];
      dis += d[j] * d[j];
    }
    if (dis > dd)
    {
      pc->InsertElement(it, point2);
      it++;
      point1 = point2;
    }
  }

  m_MovingPointSet->SetPoints(pc);
}
*/
void FantasticRegistration::StartRegistration()
{
  m_PointSetToPointSetRegistration->SetFixedPointSet(m_FixedPointSet);
  m_PointSetToPointSetRegistration->SetMovingPointSet(m_MovingPointSet);
  m_PointSetToPointSetRegistration->StartRegistration();
}

}


