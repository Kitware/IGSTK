
#include "IGMTTargetViewer.h"

#include "math.h"

#include "vtkInteractorStyleImage.h"
#include "vtkProperty.h"
#include "vtkTextProperty.h"

namespace IGSTK
{

IGMTTargetViewer::IGMTTargetViewer()
{
  m_Renderer = vtkRenderer::New();
	m_Renderer->SetBackground(0, 0, 0);
  
  m_Camera = vtkCamera::New();
  m_Renderer->SetActiveCamera(m_Camera);

	m_RenderWindow = vtkRenderWindow::New();
	m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetDesiredUpdateRate( 25 );

  m_EntryPlane = vtkPlaneSource::New();
  m_EntryPlane->SetXResolution(1);
  m_EntryPlane->SetYResolution(1);

  m_Transform = vtkTransform::New();

  m_TransformFilter = vtkTransformPolyDataFilter::New();
  m_TransformFilter->SetInput(m_EntryPlane->GetOutput());
  m_TransformFilter->SetTransform(m_Transform);

  m_PlaneMapper = vtkPolyDataMapper::New();
  m_PlaneMapper->SetInput(m_TransformFilter->GetOutput());

  m_PlaneActor = vtkActor::New();
  m_PlaneActor->SetMapper(m_PlaneMapper);
  m_PlaneActor->GetProperty()->SetOpacity(0.0);

  m_DepthLUT = vtkLookupTable::New();

  BuildDefaultLUT1(m_DepthLUT);

/*  m_DepthLUT->SetHueRange(0.0, 1.0);
  m_DepthLUT->SetSaturationRange(0.0, 1.0);
  m_DepthLUT->SetValueRange(0.0, 1.0);
  m_DepthLUT->SetAlphaRange(0.0, 1.0);
  m_DepthLUT->SetNumberOfColors(256);
  m_DepthLUT->Build();
*/
  m_AccLUT = vtkLookupTable::New();

  BuildDefaultLUT2(m_AccLUT);

/*  m_AccLUT->SetHueRange(0.0, 1.0);
  m_AccLUT->SetSaturationRange(0.0, 1.0);
  m_AccLUT->SetValueRange(0.0, 1.0);
  m_AccLUT->SetAlphaRange(0.0, 1.0);
  m_AccLUT->SetNumberOfColors(256);
  m_AccLUT->Build();
*/
  m_DepthBarActor = vtkScalarBarActor::New();
  m_DepthBarActor->SetTitle("");
  m_DepthBarActor->SetLookupTable(m_DepthLUT);
  m_DepthBarActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  m_DepthBarActor->GetPositionCoordinate()->SetValue(0.1, 0.01);
  m_DepthBarActor->SetOrientationToHorizontal();
  m_DepthBarActor->SetWidth(0.8);
  m_DepthBarActor->SetHeight(0.13);
  m_DepthBarActor->SetNumberOfLabels(0);

  m_AccBarActor = vtkScalarBarActor::New();
  m_AccBarActor->SetTitle("");
//  m_AccBarActor->GetTitleTextProperty()->SetFontSize(12);
  m_AccBarActor->SetLookupTable(m_AccLUT);
  m_AccBarActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  m_AccBarActor->GetPositionCoordinate()->SetValue(0.94, 0.13);
  m_AccBarActor->SetOrientationToVertical();
  m_AccBarActor->SetWidth(0.07);
  m_AccBarActor->SetHeight(0.9);
  m_AccBarActor->SetNumberOfLabels(0);
  
  m_Renderer->AddActor(m_PlaneActor);
  m_Renderer->AddActor(m_Probe.GetVTKActorPointer());
  m_Renderer->AddActor(m_EntryMarker.GetVTKActorPointer());
  m_Renderer->AddActor(m_TargetMarker.GetVTKActorPointer());
  m_Renderer->AddActor(m_DepthBarActor);
  m_Renderer->AddActor(m_AccBarActor);
  m_TargetCrossLine.AddActorsToRenderer(m_Renderer);
  m_EntryCrossLine.AddActorsToRenderer(m_Renderer);
  m_ProbeTipCrossLine.AddActorsToRenderer(m_Renderer);
  m_ProbeHipCrossLine.AddActorsToRenderer(m_Renderer);
 
  m_EntryMarker.SetColor(0, 1, 0);
  m_EntryMarker.SetRadius(2.0);
  m_EntryMarker.SetOpacity(0.2);
  
  m_TargetMarker.SetColor(1, 0, 0);
  m_TargetMarker.SetRadius(2.0);
  m_TargetMarker.SetOpacity(0.2);

  m_Probe.SetColor(0.5294, 0.8078, 0.9804);
  m_Probe.SetRadius( 1.5f );
  m_Probe.SetLength( 70.0f);

  m_Probe.VisibilityOff();
  m_Probe.TipBubbleVisibilityOff();
  m_EntryMarker.VisibilityOff();
  m_TargetMarker.VisibilityOff();

  m_TargetCrossLine.SetColor(1.0, 0.0, 0.0);
  m_EntryCrossLine.SetColor(0.0, 1.0, 0.0);
  m_ProbeTipCrossLine.SetColor(0.5294, 0.8078, 0.9804);
  m_ProbeHipCrossLine.SetColor(1.000, 0.032, 0.798);

  m_Spacing[0] = m_Spacing[1] = m_Spacing[2] = 1.0;
}

IGMTTargetViewer::~IGMTTargetViewer()
{
  if (m_Renderer)
	{
		m_Renderer->Delete();
	}

	if (m_RenderWindow)
	{
		m_RenderWindow->Delete();
	}

  if (m_EntryPlane)
  {
    m_EntryPlane->Delete();
  }

  if (m_PlaneMapper)
  {
    m_PlaneMapper->Delete();
  }

  if (m_PlaneActor)
  {
    m_PlaneActor->Delete();
  }

  if (m_Transform)
  {
    m_Transform->Delete();
  } 
}

void IGMTTargetViewer::Render()
{

  float      allBounds[6];

  m_Renderer->ComputeVisiblePropBounds( allBounds );

  m_Renderer->ResetCameraClippingRange(allBounds);

  m_RenderWindow->Render(); 
}

void IGMTTargetViewer::SetInteractor(vtkRenderWindowInteractor *interactor)
{
  m_Interactor = interactor;

  vtkInteractorStyleImage * interactorStyle = vtkInteractorStyleImage::New();
  interactor->SetInteractorStyle( interactorStyle );
  interactorStyle->Delete();

//  m_Interactor->SetRenderWindow(m_RenderWindow);
	m_RenderWindow->SetInteractor(m_Interactor);
}

void IGMTTargetViewer::SetEntryPoint(double *entry)
{
//  memcpy(m_EntryPoint, entry, 3 * sizeof(double));
  for (unsigned int i = 0; i < 3; i++)
  {
    m_EntryPoint[i] = entry[i];
  }
}

void IGMTTargetViewer::SetTargetPoint(double *target)
{
//  memcpy(m_TargetPoint, target, 3 * sizeof(double));
//  memcpy(m_UpdatedTargetPoint, target, 3 * sizeof(double));

  for (unsigned int i = 0; i < 3; i++)
  {
    m_TargetPoint[i] = m_UpdatedTargetPoint[i] = target[i];
  }
}

void IGMTTargetViewer::SetEntryTargetPoint(double *entry, double *target)
{
  double dis = 0;

  this->SetEntryPoint(entry);
  this->SetTargetPoint(target);

  for (unsigned int i = 0; i < 3; i++)
  {
    m_Normal[i] = m_EntryPoint[i] - m_TargetPoint[i];
    dis += m_Normal[i] * m_Normal[i];
  }  

  if (dis >= 0.000001)
  {
    for (i = 0; i < 3; i++)
    {
      m_Normal[i] /= dis;
    }
  }
  
  m_EntryPlane->SetCenter(m_EntryPoint[0], m_EntryPoint[1], m_EntryPoint[2]);
  m_EntryPlane->SetNormal(m_Normal[0], m_Normal[1], m_Normal[2]);

  m_Transform->Identity();
  m_Transform->Translate(m_EntryPoint[0], m_EntryPoint[1], m_EntryPoint[2]);
  m_Transform->Scale(256.0, 256.0, 256.0);
  m_Transform->Translate(-m_EntryPoint[0], -m_EntryPoint[1], -m_EntryPoint[2]);

  m_EntryMarker.SetCenter(entry[0], entry[1], entry[2]);
  m_TargetMarker.SetCenter(target[0], target[1], target[2]);
  m_EntryMarker.VisibilityOn();
  m_TargetMarker.VisibilityOn();
}

void IGMTTargetViewer::UpdateTargetPoint(double *target)
{
//  memcpy(m_UpdatedTargetPoint, target, 3 * sizeof(double));
  for (unsigned int i = 0; i < 3; i++)
  {
    m_UpdatedTargetPoint[i] = target[i];
  }

  float x, y, z;

  x = m_UpdatedTargetPoint[0];
  y = m_UpdatedTargetPoint[1];
  z = m_UpdatedTargetPoint[2];
  m_Renderer->WorldToView(x, y, z);
  m_TargetMarker.SetCenter(m_UpdatedTargetPoint[0], m_UpdatedTargetPoint[1], m_UpdatedTargetPoint[2]);
  m_TargetCrossLine.SetCenter(x * m_Renderer->GetSize()[0] / 2, y * m_Renderer->GetSize()[1] / 2);
}

void IGMTTargetViewer::TranslateTargetPoint(double offx, double offy, double offz)
{

  m_UpdatedTargetPoint[0] = m_TargetPoint[0] + offx;
  m_UpdatedTargetPoint[1] = m_TargetPoint[1] + offy;
  m_UpdatedTargetPoint[2] = m_TargetPoint[2] + offz;

  float x, y, z;

  x = m_UpdatedTargetPoint[0];
  y = m_UpdatedTargetPoint[1];
  z = m_UpdatedTargetPoint[2];
  m_Renderer->WorldToView(x, y, z);
  m_TargetCrossLine.SetCenter(x * m_Renderer->GetSize()[0] / 2, y * m_Renderer->GetSize()[1] / 2);
  m_TargetMarker.SetCenter(m_UpdatedTargetPoint[0], m_UpdatedTargetPoint[1], m_UpdatedTargetPoint[2]);
}

void IGMTTargetViewer::SetProbePosition(double *tip, double *hip)
{
  float tippos[3], hippos[3];
   
  for (unsigned int i = 0; i < 3; i++)
  {
    tippos[i] = tip[i];
    hippos[i] = hip[i];
    m_TipPoint[i] = tip[i];
    m_HipPoint[i] = hip[i];
  }  

  m_Probe.SetTipAndDirection(tip, hip);
  m_Probe.VisiblityOn();

  m_Renderer->WorldToView(tippos[0], tippos[1], tippos[2]);
  m_ProbeTipCrossLine.SetCenter(tippos[0] * m_Renderer->GetSize()[0] / 2, tippos[1] * m_Renderer->GetSize()[1] / 2);
  m_Renderer->WorldToView(hippos[0], hippos[1], hippos[2]);
  m_ProbeHipCrossLine.SetCenter(hippos[0] * m_Renderer->GetSize()[0] / 2, hippos[1] * m_Renderer->GetSize()[1] / 2);

  UpdateDepth();
  UpdateAcc();
}

void IGMTTargetViewer::SetupCamera()
{
  double nor[3];

  for (unsigned int i = 0; i < 3; i++)
  {
    nor[i] = m_TargetPoint[i] - m_EntryPoint[i];
  }

  m_Camera->SetFocalPoint(m_EntryPoint);

  m_Camera->SetPosition(m_EntryPoint[0] - 100 * nor[0], m_EntryPoint[1] - 100 * nor[1], m_EntryPoint[2] - 100 * nor[2]);

  m_Camera->SetViewUp(1, 0, 0);

  m_Camera->SetParallelScale(32);

  m_Camera->ParallelProjectionOn();

  m_Renderer->SetActiveCamera(m_Camera);

  float x, y, z;
  
  x = m_EntryPoint[0];
  y = m_EntryPoint[1];
  z = m_EntryPoint[2];
  m_Renderer->WorldToView(x, y, z);
  m_EntryCrossLine.SetCenter(x * m_Renderer->GetSize()[0] / 2, y * m_Renderer->GetSize()[1] / 2);

  x = m_TargetPoint[0];
  y = m_TargetPoint[1];
  z = m_TargetPoint[2];
  m_Renderer->WorldToView(x, y, z);
  m_TargetCrossLine.SetCenter(x * m_Renderer->GetSize()[0] / 2, y * m_Renderer->GetSize()[1] / 2);
}

double IGMTTargetViewer::GetTargetDistance(double *point)
{
  double dx, dy, dz, dis;

  dx = (point[0] - m_UpdatedTargetPoint[0]) * m_Spacing[0];
  dy = (point[1] - m_UpdatedTargetPoint[1]) * m_Spacing[1];
  dz = (point[2] - m_UpdatedTargetPoint[2]) * m_Spacing[2];

  dis = sqrt(dx * dx + dy * dy + dz * dz);

  if (dx * m_Normal[0] + dy * m_Normal[1] + dz * m_Normal[2] > 0)
  {
    return dis;
  }
  else
  {
    return -dis;
  }
}

double IGMTTargetViewer::GetTargetAcc(double *tip, double *hip)
{
  unsigned int i;
  double nor[3], probedir[3];
  double acc, dis;

  for (i = 0; i < 3; i++)
  {
    nor[i] = m_UpdatedTargetPoint[i] - m_EntryPoint[i];
    probedir[i] = tip[i] - hip[i];
  }

  this->NormalizeVector(nor);
  this->NormalizeVector(probedir);

  dis = 0;
  for (i = 0; i < 3; i++)
  {
    dis += nor[i] * probedir[i];
  }
  
  acc = acos(fabs(dis));

  return acc;
}

void IGMTTargetViewer::BuildDefaultLUT1(vtkLookupTable *lut)
{
  unsigned int i;

  lut->SetNumberOfColors(100);
  lut->Build();

  for (i = 0; i < 67; i++)
  {
    lut->SetTableValue(i, 1.0, 1.0, 0.0, 1.0);
  }
  for (i = 67; i < 100; i++)
  {
    lut->SetTableValue(i, 1.0, 0.0, 0.0, 1.0);
  }
}

void IGMTTargetViewer::BuildDefaultLUT2(vtkLookupTable *lut)
{
  unsigned int i;

  lut->SetNumberOfColors(100);
  lut->Build();

  for (i = 0; i < 100; i++)
  {
    lut->SetTableValue(i, 1.0, 1.0, 0.0, 1.0);
  }  
}

void IGMTTargetViewer::UpdateDepth()
{
  unsigned int i, pos;
  double dis;
  char mes[128];

  dis = this->GetTargetDistance(m_TipPoint);
  if (dis < 66.0 && dis > -33.0)
  {
    pos = (unsigned int)(66 - dis);
  
    this->BuildDefaultLUT1(m_DepthLUT);
    
    for (i = 0; i <= pos; i++)
    {
      m_DepthLUT->SetTableValue(i, 0.0, 0.0, 1.0, 1.0);
    }
  }  

  sprintf(mes, "%.2fmm", dis);
  m_DepthBarActor->SetTitle((const char*)mes);
}

void IGMTTargetViewer::UpdateAcc()
{
  unsigned int i;
  double acc;
  char mes[128];

  acc = this->GetTargetAcc(m_TipPoint, m_HipPoint);

  this->BuildDefaultLUT2(m_AccLUT);
    
  for (i = 0; i <= (1.58 - acc) * 100 / 1.58; i++)
  {
    m_AccLUT->SetTableValue(i, 0.0, 0.0, 1.0, 1.0);
  }

  sprintf(mes, "%.0f", acc * 90 / 1.58);
  m_AccBarActor->SetTitle((const char*)mes);
}

void IGMTTargetViewer::NormalizeVector(double *vec)
{
  double dis = 0.0;

  for (unsigned int i = 0; i < 3; i++)
  {
    dis += vec[i] * vec[i];
  }  

  dis = sqrt(dis);

  if (dis > 0.000001)
  {
    for (unsigned int i = 0; i < 3; i++)
    {
      vec[i] /= dis;
    }
  }
}

void IGMTTargetViewer::SetSpacing(double *spacing)
{
  this->SetSpacing(spacing[0], spacing[1], spacing[2]);
}

void IGMTTargetViewer::SetSpacing(double s1, double s2, double s3)
{
  m_Spacing[0] = s1;
  m_Spacing[1] = s2;
  m_Spacing[2] = s3;
}

}





