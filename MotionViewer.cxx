// MotionViewer.cpp: implementation of the MotionViewer class.
//
//////////////////////////////////////////////////////////////////////

#include "MotionViewer.h"

#include "vtkProperty2D.h"
#include "vtkTextProperty.h"
#include "vtkInteractorStyleImage.h"

namespace ISIS
{
	
	static const char * strText[] = 
	{
		"RESPIRATORY DISTANCE (cm)",
			"                                   \n                             \n                ",
			"H\nE\nA\nD",
			"F\nO\nO\nT",
			"HOME",
			NULL
	};
	
	MotionViewer::MotionViewer()
	{
		m_Width = 512;  
		m_Height = 512;
		m_BarWidth = m_Width * 0.8;
		m_BarHeight = m_Height * 0.1;
		
		m_Actor         = vtkImageActor::New();
		m_Renderer      = vtkRenderer::New();
		m_RenderWindow  = vtkRenderWindow::New();
		
		m_Camera = m_Renderer->GetActiveCamera();
		m_Camera->ParallelProjectionOn();
		
		m_RenderWindow->AddRenderer( m_Renderer );		
		
		ImageCanvas = vtkImageCanvasSource2D::New();
		ImageCanvas->SetNumberOfScalarComponents(3);
		ImageCanvas->SetScalarType(VTK_UNSIGNED_CHAR);
		ImageCanvas->SetExtent(0, m_Width, 0, m_Height, 0, 0);
		
		ImageCanvas->SetDrawColor(0, 0, 0);
		ImageCanvas->FillBox(0, m_Width, 0, m_Height);
		
		float home = (m_Width - m_BarWidth) / 2.0 + 0.2 * m_BarWidth;
		m_XPos = home;
		
		DrawBar();
		
		ImageCanvas->SetDrawColor(0, 255, 0);
		m_Actor->SetInput(ImageCanvas->GetOutput());
		
		m_Renderer->AddProp( m_Actor );
		
		for (int i = 0; i < 5; i ++)
		{
			this->TextMapper[i] = vtkTextMapper::New();
			this->TextMapper[i]->SetInput(strText[i]);
			this->TextMapper[i]->GetTextProperty()->SetFontSize(12);
			this->TextMapper[i]->GetTextProperty()->SetFontFamilyToArial();
			this->TextMapper[i]->GetTextProperty()->BoldOn();
			this->TextMapper[i]->GetTextProperty()->ItalicOff();
			this->TextMapper[i]->GetTextProperty()->ShadowOff();
			this->TextMapper[i]->GetTextProperty()->SetJustificationToCentered();
			this->TextMapper[i]->GetTextProperty()->SetVerticalJustificationToCentered();
			this->TextMapper[i]->GetTextProperty()->SetLineSpacing(0.6);
			
			this->TextActor[i] = vtkActor2D::New();
			this->TextActor[i]->SetMapper(this->TextMapper[i]);
			this->TextActor[i]->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
			this->TextActor[i]->GetProperty()->SetColor(0, 1, 0);
			
			m_Renderer->AddActor2D(this->TextActor[i]);
		}
		
		this->TextMapper[1]->GetTextProperty()->SetFontSize(10);
		this->TextActor[0]->GetPositionCoordinate()->SetValue(0.5, 0.35);
		this->TextActor[1]->GetPositionCoordinate()->SetValue(0.7, 0.1);
		this->TextActor[2]->GetPositionCoordinate()->SetValue(0.05, 0.5);
		this->TextActor[3]->GetPositionCoordinate()->SetValue(0.95, 0.5);
		this->TextActor[4]->GetPositionCoordinate()->SetValue(0.26, 0.65);
		
		int lh = this->m_BarHeight / 4;
		this->ImageCanvas->DrawSegment(home, 0.6 * m_Height, home, 0.6 * m_Height - lh);
		
		char strLabel[10];
		
		for (i = 0; i < 6; i ++)
		{
			itoa(1 - i, strLabel, 10);
			//	sprintf(strLable, "%1.1f", 
			this->LabelMapper[i] = vtkTextMapper::New();
			this->LabelMapper[i]->SetInput(strLabel);
			this->LabelMapper[i]->GetTextProperty()->SetFontSize(11);
			this->LabelMapper[i]->GetTextProperty()->SetFontFamilyToArial();
			this->LabelMapper[i]->GetTextProperty()->BoldOn();
			this->LabelMapper[i]->GetTextProperty()->ItalicOff();
			this->LabelMapper[i]->GetTextProperty()->ShadowOff();
			this->LabelMapper[i]->GetTextProperty()->SetJustificationToCentered();
			this->LabelMapper[i]->GetTextProperty()->SetVerticalJustificationToCentered();
			
			this->LabelActor[i] = vtkActor2D::New();
			this->LabelActor[i]->SetMapper(this->LabelMapper[i]);
			this->LabelActor[i]->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
			this->LabelActor[i]->GetPositionCoordinate()->SetValue(((m_Width - this->m_BarWidth) / 2.0 + i * this->m_BarWidth / 5.0) / (float)m_Width, 0.43);
			this->LabelActor[i]->GetProperty()->SetColor(0, 1, 0);
			
			m_Renderer->AddActor2D(this->LabelActor[i]);
		}
		
	}
	
	MotionViewer::~MotionViewer()
	{
		this->ImageCanvas->Delete();
		for (int i = 0; i < 5; i ++)
		{
			this->TextActor[i]->Delete();
			this->TextMapper[i]->Delete();
		}
		
		for (i = 0; i < 6; i ++)
		{
			this->LabelActor[i]->Delete();
			this->LabelMapper[i]->Delete();
		}	
	}
	
	void MotionViewer::SetInteractor( vtkRenderWindowInteractor * interactor )
	{
		m_RenderWindow->SetInteractor( interactor );
		
		m_Interactor = interactor;
		
		vtkInteractorStyleImage * interactorStyle = vtkInteractorStyleImage::New();
		interactor->SetInteractorStyle( interactorStyle );
		interactorStyle->Delete();
	}
	
	void MotionViewer::Render()
	{
		this->DrawBar();
		m_RenderWindow->Render();
	}
/*	
	void MotionViewer::show()
	{
		this->Render();
	}
*/	
	void MotionViewer::UpdateMotion(float x)
	{
		float home = (m_Width - this->m_BarWidth) / 2.0 + 0.2 * this->m_BarWidth;
		this->m_XPos = home + (x / 5.0) * (this->m_BarWidth / 10.0);
		this->Render();
	}
	
	void MotionViewer::DrawBar()
	{
		this->ImageCanvas->SetDrawColor(0.0 * 255, 0.7490 * 255, 1.0 * 255);
		int xmin = (m_Width - this->m_BarWidth) / 2;
		int xmax = xmin + this->m_BarWidth;
		int ymin = (m_Height - this->m_BarHeight) / 2;
		int ymax = ymin + this->m_BarHeight;
		this->ImageCanvas->FillBox(xmin, xmax, ymin, ymax);
		
		int dx = this->m_BarHeight / 5;
		int dy = this->m_BarHeight * 9 / 20;
		this->ImageCanvas->SetDrawColor(255, 0, 0);
		int x0 = this->m_XPos - dx;
		int x1 = this->m_XPos + dx;
		int y0 = 0.5 * this->m_Height - dy;
		int y1 = 0.5 * this->m_Height + dy;
		if (x0 < xmin)
			x0 = xmin;
		if (x0 > xmax)
			x0 = xmax;
		if (x1 < xmin)
			x1 = xmin;
		if (x1 > xmax)
			x1 = xmax;
		this->ImageCanvas->FillBox(x0, x1, y0, y1);
		
		int lh = this->m_BarHeight / 4;
		float step = (xmax - xmin) / 10.0; 
		this->ImageCanvas->SetDrawColor(0.1843 * 255, 0.3098 * 255, 0.3098 * 255);
		for (float xc = xmin + step; xc < xmax; xc += step)
		{
			this->ImageCanvas->DrawSegment(xc, ymin, xc, ymin + lh);
			this->ImageCanvas->DrawSegment(xc, m_Height / 2 - lh /2, xc, m_Height / 2 + lh / 2);
			this->ImageCanvas->DrawSegment(xc, ymax, xc, ymax - lh);
		}
	}
	
	void MotionViewer::SetupCamera()
	{
		
		vtkImageData * image = m_Actor->GetInput();
		
		if ( !image )
		{
			return;
		}
		
		float spacing[3];
		float origin[3];
		int   dimensions[3];
		
		image->GetSpacing(spacing);
		image->GetOrigin(origin);
		image->GetDimensions(dimensions);
		
		spacing[0] = spacing[1] = spacing[2] = 1;
		dimensions[0] = m_Width;
		dimensions[1] = m_Height;
		dimensions[2] = 0;
		origin[0] = origin[1] = origin[2] = 0;
		
		float focalPoint[3];
		float position[3];
		
		for ( unsigned int cc = 0; cc < 3; cc++)
		{
			focalPoint[cc] = origin[cc] + ( spacing[cc] * dimensions[cc] ) / 2.0;
			position[cc]   = focalPoint[cc];
		}
		
		const float distanceToFocalPoint = 1000;
		int idx = 2;
		
		position[idx] -= distanceToFocalPoint;
		m_Camera->SetViewUp ( 0,  -1,  0 );
		
		m_Camera->SetPosition (   position );
		m_Camera->SetFocalPoint ( focalPoint );
		
		
#define myMAX(x,y) (((x)>(y))?(x):(y))  
		
		int d1 = (idx + 1) % 3;
		int d2 = (idx + 2) % 3;
		
		float max = myMAX( 
			spacing[d1] * dimensions[d1],
			spacing[d2] * dimensions[d2]);
		
		
		m_Camera->SetParallelScale( max / 2 );
		
	}
	
}