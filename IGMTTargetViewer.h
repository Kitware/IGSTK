#ifndef _IGSTK_IGMTTargetViewer_H_
#define _IGSTK_IGMTTargetViewer_H_

#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkLineSource.h"
#include "vtkLookUpTable.h"
#include "vtkPlaneSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkScalarBarActor.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

#include "CrossLine.h"
#include "Sphere.h"
#include "TubeWrappedLine.h"

namespace IGSTK
{

class IGMTTargetViewer  
{
public:
	double GetTargetDistance(double* point);

  double GetTargetAcc(double* tip, double* hip);
  
  double m_EntryPoint[3];

  double m_TargetPoint[3];

  double m_UpdatedTargetPoint[3];

  double m_TipPoint[3];

  double m_HipPoint[3];

  double m_Normal[3];

  double m_Spacing[3];

	void SetupCamera();

	void SetProbePosition(double* tip, double* hip);
	
  void UpdateTargetPoint(double* target);

  void TranslateTargetPoint(double x, double y, double z);
	
  void SetEntryTargetPoint(double* entry, double* target);
	
  void SetTargetPoint(double* target);
	
  void SetEntryPoint(double* entry);
	
  void SetInteractor(vtkRenderWindowInteractor* interactor);
	
  void Render();
	
  IGMTTargetViewer();
	
  virtual ~IGMTTargetViewer();

public:
	void SetSpacing(double* spacing);
  void SetSpacing(double s1, double s2, double s3);
	void NormalizeVector(double* vec);
	void UpdateDepth();

  void UpdateAcc();

	void BuildDefaultLUT1(vtkLookupTable* lut);

  void BuildDefaultLUT2(vtkLookupTable* lut);

  vtkRenderer* m_Renderer;

	vtkRenderWindow* m_RenderWindow;
	
  vtkRenderWindowInteractor* m_Interactor;

  vtkCamera* m_Camera;

  CrossLine m_TargetCrossLine;

  CrossLine m_EntryCrossLine;

  CrossLine m_ProbeTipCrossLine;

  CrossLine m_ProbeHipCrossLine;

  TubeWrappedLine		m_Probe;

  Sphere m_EntryMarker;

  Sphere m_TargetMarker;

  vtkPlaneSource* m_EntryPlane;

  vtkPolyDataMapper* m_PlaneMapper;

  vtkActor*  m_PlaneActor;

  vtkTransform *m_Transform;

  vtkTransformPolyDataFilter* m_TransformFilter;

  vtkLineSource *m_EntryCrossLineH, *m_EntryCrossLineV;

  vtkLineSource *m_TargetCrossLineH, *m_TargetCrossLineV;

  vtkPolyDataMapper *m_EntryCrossLineHMapper, *m_EntryCrossLineVMapper;

  vtkPolyDataMapper *m_TargetCrossLineHMapper, *m_TargetCrossLineVMapper;

  vtkActor *m_EntryCrossLineHActor, *m_EntryCrossLineVActor;

  vtkActor *m_TargetCrossLineHActor, *m_TargetCrossLineVActor;

  vtkScalarBarActor* m_DepthBarActor;

  vtkLookupTable* m_DepthLUT;

  vtkScalarBarActor* m_AccBarActor;

  vtkLookupTable* m_AccLUT;

};

}

#endif 
