
#ifndef _IGSTK_IGMTVolumeViewer_H_
#define _IGSTK_IGMTVolumeViewer_H_

#include "vtkActor.h"
#include "vtkAssembly.h"
#include "vtkColorTransferFunction.h"
#include "vtkDecimatePro.h"
#include "vtkImageData.h"
#include "vtkImageResample.h"
#include "vtkMarchingCubes.h"
#include "vtkLODProp3D.h"
#include "vtkOutlineFilter.h"
#include "vtkPiecewiseFunction.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkVolume.h"
#include "vtkVolumeProMapper.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkVolumeRayCastIsosurfaceFunction.h"
#include "vtkVolumeRayCastMapper.h"
#include "vtkVolumeRayCastMIPFunction.h"
#include "vtkVolumeTextureMapper2D.h"

#include "TubeWrappedLine.h"

namespace IGSTK
{

class IGMTVolumeViewer  
{
public:
  static void MCEndFunc(void* arg);
	void SetThreshold(int thres);
	void Render();
	
  void SetInteractor( vtkRenderWindowInteractor * interactor );
	
  void SetInput( vtkImageData * image );
	
  IGMTVolumeViewer();

  IGMTVolumeViewer( vtkRenderWindow* renWin );

  void Init( vtkRenderWindow* renWin );
	
  virtual ~IGMTVolumeViewer();

public:
	void SetDesiredTriNum(int num);
	void SetMCProgressMethod(void(*f)(void *), void* arg);
  void SetMCEndMethod(void(*f)(void *), void* arg);
  void SetDecProgressMethod(void(*f)(void *), void* arg);
  void SetSmoothProgressMethod(void(*f)(void *), void* arg);
	void SetProbeTipAndDirection(double* tip, double* hip);
	void SetRenderingCategory(int rc);
	void SetResampleRate(double r);
  void SetResampleRateLOD(double r1, double r2);

	int GetProbeVisibility();
	void ShowProbe(int s);
	void ShowFrame(int s);
	void SetRenderingMethod(int rm);

  vtkAssembly * GetRoot()
  { return m_Root; };

public:
  vtkMarchingCubes* GetMarchingCubes() 
  { return m_MarchingCubes; };

  vtkRenderWindow* GetRenderWindow()
  { return m_RenderWindow; };

  vtkDecimatePro* GetDecimatePro()
  { return m_DecimatePro; };

  vtkSmoothPolyDataFilter* GetSmoothFiler()
  { return m_SmoothFilter; };

private:

  vtkAssembly* m_Root;

  vtkAssembly* m_ProbeRoot;

  vtkRenderer* m_Renderer;

	vtkRenderWindow* m_RenderWindow;
	
  vtkRenderWindowInteractor* m_Interactor;

  vtkLODProp3D* m_LOD;

	vtkImageData* m_ImageData;

  double m_ResampleRateRayCastLOD;

  double m_ResampleRateTex2DLOD;

  double m_ResampleRate;

  vtkImageResample* m_ImageResampleRayCast;
  
  vtkImageResample* m_ImageResampleTex2D;
  
  vtkImageResample* m_ImageResampleVPro;

  vtkVolumeRayCastCompositeFunction* m_Composite;
	
  vtkVolumeRayCastIsosurfaceFunction* m_IsoSurface; 
	
  vtkVolumeRayCastMIPFunction* m_MIP;
	
  vtkVolumeRayCastMapper* m_RayCastMapper;

  vtkVolumeProMapper* m_VProMapper;

  vtkVolumeTextureMapper2D* m_Tex2DMapper;

  vtkPiecewiseFunction* m_OpacityFunction;

  vtkColorTransferFunction* m_ColorFunction;
	
  vtkPiecewiseFunction* m_GradientFunction;

  vtkVolumeProperty* m_Property;
	
  vtkVolume* m_Volume;

 	vtkOutlineFilter* m_Outline;
	
  vtkPolyDataMapper* m_OutlineMapper;
	
  vtkActor* m_OutlineActor;

  TubeWrappedLine		m_Probe;

  vtkImageResample* m_ImageResampleMarchingCubes;

  vtkMarchingCubes* m_MarchingCubes;

  vtkDecimatePro* m_DecimatePro;

  vtkSmoothPolyDataFilter* m_SmoothFilter;

  vtkPolyDataMapper* m_ContourMapper;

  vtkActor* m_ContourActor;

  int m_Ext[6];
	
  int m_Dim[3];
	
  vtkFloatingPointType m_Spacing[3];

  int m_DesiredTriNum;

  int m_CellNum;

//  void(* m_MCProgressFunc)(void *);

};

}

#endif 
