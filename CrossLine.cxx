
#include "CrossLine.h"

#include "vtkProperty.h"
#include "vtkProperty2D.h"

namespace IGSTK
{

CrossLine::CrossLine()
{
  m_HLine = vtkLineSource::New();

  m_HLineMapper = vtkPolyDataMapper2D::New();
  m_HLineMapper->SetInput(m_HLine->GetOutput());

  m_HLineActor = vtkActor2D::New();
  m_HLineActor->SetMapper(m_HLineMapper);
  m_HLineActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
  m_HLineActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
  m_HLineActor->GetPositionCoordinate()->SetValue(0.5, 0.5);

  m_VLine = vtkLineSource::New();

  m_VLineMapper = vtkPolyDataMapper2D::New();
  m_VLineMapper->SetInput(m_VLine->GetOutput());

  m_VLineActor = vtkActor2D::New();
  m_VLineActor->SetMapper(m_VLineMapper);
  m_VLineActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
  m_VLineActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
  m_VLineActor->GetPositionCoordinate()->SetValue(0.5, 0.5);
 
}

CrossLine::~CrossLine()
{
  if (m_HLine)
  {
    m_HLine->Delete();
  }

  if (m_HLineMapper)
  {
    m_HLineMapper->Delete();
  }

  if (m_HLineActor)
  {
    m_HLineActor->Delete();
  }

  if (m_HLine)
  {
    m_HLine->Delete();
  }

  if (m_HLineMapper)
  {
    m_HLineMapper->Delete();
  }

  if (m_HLineActor)
  {
    m_HLineActor->Delete();
  }  
}

void 
CrossLine
::SetCenter(double x, double y)
{
  m_HLine->SetPoint1(-10000, y, 0);
  m_HLine->SetPoint2(10000, y, 0);

  m_VLine->SetPoint1(x, -10000, 0);
  m_VLine->SetPoint2(x, 10000, 0);
}

void CrossLine::AddActorsToRenderer(vtkRenderer *renderer)
{
  renderer->AddActor2D(m_HLineActor);
  renderer->AddActor2D(m_VLineActor);
}

void CrossLine::SetColor(float r, float g, float b)
{
  m_HLineActor->GetProperty()->SetColor(r, g, b);
  m_VLineActor->GetProperty()->SetColor(r, g, b);
}
}




