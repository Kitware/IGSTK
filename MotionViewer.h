// MotionViewer.h: interface for the MotionViewer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __ISIS_MotionViewer_h__
#define __ISIS_MotionViewer_h__

#include "vtkActor2D.h"
#include "vtkTextMapper.h"
#include "vtkImageCanvasSource2D.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkImageActor.h"
#include "vtkRenderWindow.h"
#include "vtkCommand.h"

class MotionViewer  
{
public:
	int w, h;

	MotionViewer();
	virtual ~MotionViewer();
	
public:
	void SetInteractor( vtkRenderWindowInteractor * interactor );
	
	void DrawBar();
	
	void Render();
	
	void show();
	
	void UpdateMotion(float x);

	void SetupCamera( void );
	
protected:

	vtkImageActor     * m_Actor;
	
	vtkRenderer       * m_Renderer;
	
	vtkCamera         * m_Camera;
	
	vtkRenderWindow   * m_RenderWindow;

	vtkRenderWindowInteractor * m_Interactor;
	
	vtkImageCanvasSource2D * ImageCanvas;
	vtkTextMapper * LabelMapper[11];
	vtkActor2D * LabelActor[11];
	vtkTextMapper * TextMapper[5];
	vtkActor2D * TextActor[5];
	
	int BarHeight;
	int BarWidth;
	int XPos;
	
};

#endif
