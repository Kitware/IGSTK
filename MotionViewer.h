// MotionViewer.h: interface for the MotionViewer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __ISIS_MotionViewer_h__
#define __ISIS_MotionViewer_h__

#include "vtkActor2D.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkImageActor.h"
#include "vtkImageCanvasSource2D.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkTextMapper.h"

namespace ISIS
{

	class MotionViewer  
	{
	public:
		MotionViewer();
		
		virtual ~MotionViewer();
		
	public:
		void SetInteractor( vtkRenderWindowInteractor * interactor );
		
		void DrawBar();
		
		void Render();
		
//		void show();
		
		void UpdateMotion(float x);
		
		void SetupCamera( void );
		
	protected:

		int m_Width, m_Height;

		int m_BarHeight;

		int m_BarWidth;
		
		int m_XPos;
		
		vtkImageActor     * m_Actor;
		
		vtkRenderer       * m_Renderer;
		
		vtkCamera         * m_Camera;
		
		vtkRenderWindow   * m_RenderWindow;
		
		vtkRenderWindowInteractor * m_Interactor;
		
		vtkImageCanvasSource2D * ImageCanvas;

		vtkTextMapper * LabelMapper[6];
		
		vtkActor2D * LabelActor[6];
		
		vtkTextMapper * TextMapper[5];
		
		vtkActor2D * TextActor[5];		
	};	

}

#endif
