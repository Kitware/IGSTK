
#include "IGMTVolumeViewer.h"

#include "vtkCamera.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"

namespace IGSTK
{

IGMTVolumeViewer
::IGMTVolumeViewer()
{
	m_Renderer = vtkRenderer::New();
	m_Renderer->SetBackground(0, 0, 0);

	m_RenderWindow = vtkRenderWindow::New();
	m_RenderWindow->AddRenderer(m_Renderer);
  m_RenderWindow->SetDesiredUpdateRate( 25 );

  m_ResampleRateRayCastLOD = 0.5;

  m_ResampleRateTex2DLOD = 0.25;

  m_ResampleRate = 0.5;

  m_ImageResampleRayCast = vtkImageResample::New();
 
  m_ImageResampleTex2D = vtkImageResample::New();

  m_ImageResampleVPro = vtkImageResample::New();

  this->SetResampleRate(0.5);

  m_Composite = vtkVolumeRayCastCompositeFunction::New();
	m_IsoSurface = vtkVolumeRayCastIsosurfaceFunction::New(); 
	m_IsoSurface->IsoValue = 80;
	m_MIP = vtkVolumeRayCastMIPFunction::New();

	m_RayCastMapper = vtkVolumeRayCastMapper::New();
	m_RayCastMapper->SetVolumeRayCastFunction(m_Composite);
  m_RayCastMapper->AutoAdjustSampleDistancesOn();

  m_Tex2DMapper = vtkVolumeTextureMapper2D::New();
  
  m_VProMapper = vtkVolumeProMapper::New();
  m_VProMapper->SetBlendModeToComposite();

  m_OpacityFunction = vtkPiecewiseFunction::New();
  m_OpacityFunction->AddPoint(20, 0.0);
  m_OpacityFunction->AddPoint(255, 0.2);

	m_ColorFunction = vtkColorTransferFunction::New();
	m_ColorFunction->AddRGBPoint(0, 0, 0, 0);
	m_ColorFunction->AddRGBPoint(64, 1, 0, 0);
	m_ColorFunction->AddRGBPoint(128, 0, 0, 1);
	m_ColorFunction->AddRGBPoint(192, 0, 1, 0);
	m_ColorFunction->AddRGBPoint(255, 0, 0.2, 0);

  m_GradientFunction = vtkPiecewiseFunction::New();
  m_GradientFunction->AddPoint(0, 0.0);
  m_GradientFunction->AddPoint(3, 0.0);
  m_GradientFunction->AddPoint(6, 1.0);
  m_GradientFunction->AddPoint(255, 1.0);

	m_Property = vtkVolumeProperty::New();
	m_Property->SetScalarOpacity(m_OpacityFunction);
	m_Property->SetColor(m_ColorFunction);
//  m_Property->SetGradientOpacity(m_GradientFunction);
  m_Property->ShadeOff();
  m_Property->SetDiffuse(0.7);
  m_Property->SetAmbient(0.01);
  m_Property->SetSpecular(0.5);
  m_Property->SetSpecularPower(70.0);

	m_Volume = vtkVolume::New();
	m_Volume->SetProperty(m_Property);
  m_Volume->SetMapper(m_RayCastMapper);
  m_Volume->VisibilityOff();
	m_Renderer->AddProp(m_Volume);

  m_LOD = vtkLODProp3D::New();
  m_LOD->AddLOD(m_RayCastMapper, m_Property, 0.0);
  m_LOD->AddLOD(m_Tex2DMapper, m_Property, 0.0);
  m_LOD->VisibilityOff();
  m_Renderer->AddProp(m_LOD);

  m_ImageResampleMarchingCubes = vtkImageResample::New();
  m_ImageResampleMarchingCubes->SetAxisMagnificationFactor(0, 0.3);
  m_ImageResampleMarchingCubes->SetAxisMagnificationFactor(1, 0.3);
  m_ImageResampleMarchingCubes->SetAxisMagnificationFactor(2, 0.3);

  m_MarchingCubes = vtkMarchingCubes::New();
  m_MarchingCubes->SetInput(m_ImageResampleMarchingCubes->GetOutput());
  m_MarchingCubes->SetValue(0, 80);

  m_DecimatePro = vtkDecimatePro::New();
  m_DecimatePro->SetInput(m_MarchingCubes->GetOutput());
  m_DecimatePro->SetTargetReduction(0.3);
  m_DecimatePro->PreserveTopologyOn();

  m_SmoothFilter = vtkSmoothPolyDataFilter::New();
  m_SmoothFilter->SetInput(m_DecimatePro->GetOutput());
  m_SmoothFilter->SetNumberOfIterations(50);

  m_ContourMapper = vtkPolyDataMapper::New();
  m_ContourMapper->SetInput(m_SmoothFilter->GetOutput());

	m_ContourActor = vtkActor::New();
  m_ContourActor->SetMapper(m_ContourMapper);
  m_ContourActor->GetProperty()->SetColor(0.8000, 0.0078, 0.0804);
  m_ContourActor->GetProperty()->SetSpecularColor(1, 1, 1);
  m_ContourActor->GetProperty()->SetSpecular(0.4);
  m_ContourActor->GetProperty()->SetSpecularPower(5);
  m_ContourActor->GetProperty()->SetAmbient(0.2);
  m_ContourActor->GetProperty()->SetDiffuse(0.8);
  m_ContourActor->VisibilityOff();
	m_Renderer->AddActor(m_ContourActor);

	m_Outline = vtkOutlineFilter::New();

	m_OutlineMapper = vtkPolyDataMapper::New();

	m_OutlineActor = vtkActor::New();
	m_Renderer->AddActor(m_OutlineActor);

  m_Probe.SetColor( 0.5294, 0.8078, 0.9804);
  m_Probe.SetRadius( 2.0f );
  m_Probe.SetLength( 70.0f);

  m_Probe.GetVTKActorPointer()->VisibilityOff();
  m_Renderer->AddActor( m_Probe.GetVTKActorPointer() );

	m_Interactor = NULL;
	m_ImageData = NULL;

  m_DesiredTriNum = 30000;
  m_CellNum = 1;

  this->SetMCEndMethod(IGMTVolumeViewer::MCEndFunc, this);
}

IGMTVolumeViewer
::~IGMTVolumeViewer()
{
	if (m_Renderer)
	{
		m_Renderer->Delete();
	}

	if (m_RenderWindow)
	{
		m_RenderWindow->Delete();
	}

  if (m_ImageResampleRayCast)
  {
    m_ImageResampleRayCast->Delete();
  }

  if (m_ImageResampleTex2D)
  {
    m_ImageResampleTex2D->Delete();
  }

  if (m_ImageResampleVPro)
  {
    m_ImageResampleVPro->Delete();
  }

	if (m_Composite)
	{
		m_Composite->Delete();
	}

	if (m_IsoSurface)
	{
		m_IsoSurface->Delete();
	}

	if (m_MIP)
	{
		m_MIP->Delete();
	}

	if (m_RayCastMapper)
	{
		m_RayCastMapper->Delete();
	}

  if (m_VProMapper)
	{
		m_VProMapper->Delete();
	}

  if (m_Tex2DMapper)
  {
    m_Tex2DMapper->Delete();
  }

 	if (m_OpacityFunction)
	{
		m_OpacityFunction->Delete();
	}

	if (m_ColorFunction)
	{
		m_ColorFunction->Delete();
	}

  if (m_GradientFunction)
	{
		m_GradientFunction->Delete();
	}

	if (m_Property)
	{
		m_Property->Delete();
	}

	if (m_Volume)
	{
    m_Volume->Delete();	  
	}

  if (m_LOD)
  {
    m_LOD->Delete();
  }

  if (m_Outline)
  {
    m_Outline->Delete();
  }

  if (m_OutlineMapper)
  {
    m_OutlineMapper->Delete();
  }

  if (m_OutlineActor)
  {
    m_OutlineActor->Delete();
  }

  if (m_ImageResampleMarchingCubes)
  {
    m_ImageResampleMarchingCubes->Delete();
  }

  if (m_MarchingCubes)
  {
    m_MarchingCubes->Delete();
  }

  if (m_DecimatePro)
  {
    m_DecimatePro->Delete();
  }

  if (m_SmoothFilter)
  {
    m_SmoothFilter->Delete();
  }

  if (m_ContourMapper)
  {
    m_ContourMapper->Delete();
  }

  if (m_ContourActor)
  {
    m_ContourActor->Delete();
  }
}

void 
IGMTVolumeViewer
::SetInput(vtkImageData * imagedata)
{
	m_ImageData = imagedata;
	
	m_ImageData->GetExtent(m_Ext);
	m_ImageData->GetDimensions(m_Dim);
	m_ImageData->GetSpacing(m_Spacing);

  m_ImageResampleTex2D->SetInput(m_ImageData);
 
  m_ImageResampleRayCast->SetInput(m_ImageData);

  m_ImageResampleVPro->SetInput(m_ImageData);

	m_RayCastMapper->SetInput(m_ImageResampleRayCast->GetOutput());

  m_Tex2DMapper->SetInput(m_ImageResampleTex2D->GetOutput());
  
  m_VProMapper->SetInput(m_ImageResampleVPro->GetOutput());

  m_ImageResampleMarchingCubes->SetInput(m_ImageData);
//  m_MarchingCubes->SetInput(m_ImageData);
//  m_DecimatePro->SetInput(m_MarchingCubes->GetOutput());
//  m_ContourActor->VisibilityOn();

  m_Outline->SetInput(m_ImageData);
	m_OutlineMapper->SetInput(m_Outline->GetOutput());
	m_OutlineActor->SetMapper(m_OutlineMapper);

	m_Renderer->SetBackground(0, 0, 1);
}

void 
IGMTVolumeViewer
::SetInteractor(vtkRenderWindowInteractor *interactor)
{
	m_Interactor = interactor;
  m_Interactor->SetRenderWindow(m_RenderWindow);
	m_RenderWindow->SetInteractor(m_Interactor);
}

void 
IGMTVolumeViewer
::Render()
{
	m_Renderer->ResetCamera();
	m_RenderWindow->Render();
}

void 
IGMTVolumeViewer
::SetThreshold(int thres)
{
  m_IsoSurface->IsoValue = thres;
  m_MarchingCubes->SetValue(0, thres);
}

void 
IGMTVolumeViewer
::SetRenderingMethod(int rm)
{
  switch (rm)
  {
  case 0: // composite
    m_RayCastMapper->SetVolumeRayCastFunction(m_Composite);
    m_VProMapper->SetBlendModeToComposite();
        
    m_Property->SetScalarOpacity(m_OpacityFunction);
	  m_Property->SetColor(m_ColorFunction);
    m_Property->SetGradientOpacity(NULL);
    m_Property->ShadeOff();
    break;
  case 1: // iso surface
    m_RayCastMapper->SetVolumeRayCastFunction(m_IsoSurface);
    
    m_Property->SetScalarOpacity(m_OpacityFunction);
	  m_Property->SetColor(m_ColorFunction);
    m_Property->SetGradientOpacity(m_GradientFunction);
    m_Property->ShadeOn();
    break;
  case 2: // max ip
    m_RayCastMapper->SetVolumeRayCastFunction(m_MIP);
    m_VProMapper->SetBlendModeToMaximumIntensity();
    
    m_Property->SetScalarOpacity(NULL);
	  m_Property->SetColor((vtkColorTransferFunction*)NULL);
    m_Property->SetGradientOpacity(NULL);
    m_Property->ShadeOff();
    break;
  case 3: // min ip
    m_VProMapper->SetBlendModeToMinimumIntensity();
    
    m_Property->SetScalarOpacity(NULL);
	  m_Property->SetColor((vtkColorTransferFunction*)NULL);
    m_Property->SetGradientOpacity(NULL);
    m_Property->ShadeOff();
    break;
  }
}

void 
IGMTVolumeViewer
::ShowFrame(int s)
{
  if (s == 1)
  {
    m_OutlineActor->VisibilityOn();
  }
  else
  {
    m_OutlineActor->VisibilityOff();
  }
}

void 
IGMTVolumeViewer
::ShowProbe(int s)
{
  if (s == 1)
  {
    m_Probe.GetVTKActorPointer()->VisibilityOn();
  }
  else
  {
    m_Probe.GetVTKActorPointer()->VisibilityOff();
  }
}

int 
IGMTVolumeViewer
::GetProbeVisibility()
{
  return m_Probe.GetVTKActorPointer()->GetVisibility();
}

void IGMTVolumeViewer::SetResampleRate(double r)
{
  m_ResampleRate = r;

  m_ImageResampleRayCast->SetAxisMagnificationFactor(0, r);
  m_ImageResampleRayCast->SetAxisMagnificationFactor(1, r);
  m_ImageResampleRayCast->SetAxisMagnificationFactor(2, r);

  m_ImageResampleTex2D->SetAxisMagnificationFactor(0, r);
  m_ImageResampleTex2D->SetAxisMagnificationFactor(1, r);
  m_ImageResampleTex2D->SetAxisMagnificationFactor(2, r);

  m_ImageResampleVPro->SetAxisMagnificationFactor(0, r);
  m_ImageResampleVPro->SetAxisMagnificationFactor(1, r);
  m_ImageResampleVPro->SetAxisMagnificationFactor(2, r);
}

void IGMTVolumeViewer::SetResampleRateLOD(double r1, double r2)
{
  m_ImageResampleRayCast->SetAxisMagnificationFactor(0, r1);
  m_ImageResampleRayCast->SetAxisMagnificationFactor(1, r1);
  m_ImageResampleRayCast->SetAxisMagnificationFactor(2, r1);

  m_ImageResampleTex2D->SetAxisMagnificationFactor(0, r2);
  m_ImageResampleTex2D->SetAxisMagnificationFactor(1, r2);
  m_ImageResampleTex2D->SetAxisMagnificationFactor(2, r2);
}

void 
IGMTVolumeViewer
::SetRenderingCategory(int rc)
{
  switch (rc)
  {
  case 0: // raycast
    this->SetResampleRate(m_ResampleRate);
    m_Renderer->GetActiveCamera()->ParallelProjectionOff();

    m_LOD->VisibilityOff();
    m_ContourActor->VisibilityOff();

    m_Volume->SetMapper(m_RayCastMapper);
    m_Volume->VisibilityOn();    
    break;
  case 1: // tex 2d
    this->SetResampleRate(m_ResampleRate);
    m_Renderer->GetActiveCamera()->ParallelProjectionOff();

    m_LOD->VisibilityOff();
    m_ContourActor->VisibilityOff();

    m_Volume->SetMapper(m_Tex2DMapper);
    m_Volume->VisibilityOn();
    break;
  case 2: // lod
    this->SetResampleRateLOD(m_ResampleRateRayCastLOD, m_ResampleRateTex2DLOD);
    m_Renderer->GetActiveCamera()->ParallelProjectionOff();

    m_Volume->VisibilityOff();
    m_ContourActor->VisibilityOff();

    m_LOD->VisibilityOn();
    break;
  case 3: // vol pro
    this->SetResampleRate(m_ResampleRate);
    m_Renderer->GetActiveCamera()->ParallelProjectionOn();
    
    m_LOD->VisibilityOff();
    m_ContourActor->VisibilityOff();

    m_Volume->SetMapper(m_VProMapper);
    m_Volume->VisibilityOn();    
    break;
  case 4: // mc
    m_Renderer->GetActiveCamera()->ParallelProjectionOff();

    m_Volume->VisibilityOff();
    m_LOD->VisibilityOff();

    m_ContourActor->VisibilityOn();
  }
}

void 
IGMTVolumeViewer
::SetProbeTipAndDirection(double *tip, double *hip)
{
  m_Probe.SetTipAndDirection(tip, hip);
  m_Probe.UpdateTipBubbleRadius();
}

void 
IGMTVolumeViewer
::SetMCProgressMethod(void (*f)(void *), void* arg)
{
//  THIS METHOD HAS BEEN DEPRECATED
//  m_MarchingCubes->SetProgressMethod(f, arg);
}

void 
IGMTVolumeViewer
::SetMCEndMethod(void (*f)(void *), void* arg)
{
//  THIS METHOD HAS BEEN DEPRECATED
//  m_MarchingCubes->SetEndMethod(f, arg);
}

void 
IGMTVolumeViewer
::SetDecProgressMethod(void (*f)(void *), void* arg)
{
//  THIS METHOD HAS BEEN DEPRECATED
//  m_DecimatePro->SetProgressMethod(f, arg);
}

void 
IGMTVolumeViewer
::SetSmoothProgressMethod(void (*f)(void *), void* arg)
{
//  THIS METHOD HAS BEEN DEPRECATED
//  m_SmoothFilter->SetProgressMethod(f, arg);
}

void 
IGMTVolumeViewer
::MCEndFunc(void* arg)
{
  float ratio;
  IGMTVolumeViewer* viewer = (IGMTVolumeViewer*)arg;

  viewer->m_CellNum = viewer->m_MarchingCubes->GetOutput()->GetNumberOfCells();
  ratio = (float)viewer->m_DesiredTriNum / viewer->m_CellNum;
  ratio = (ratio > 1.0f) ? 1.0f : ratio;

  viewer->m_DecimatePro->SetTargetReduction(ratio);
}

void IGMTVolumeViewer::SetDesiredTriNum(int num)
{
  float ratio;

  m_DesiredTriNum = num;
  ratio = (float)m_DesiredTriNum / m_CellNum;
  ratio = (ratio > 1.0f) ? 1.0f : ratio;
  m_DecimatePro->SetTargetReduction(ratio);
}

}






