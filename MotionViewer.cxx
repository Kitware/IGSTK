// MotionViewer.cpp: implementation of the MotionViewer class.
//
//////////////////////////////////////////////////////////////////////

#include "MotionViewer.h"

#include "vtkProperty2D.h"
#include "vtkTextProperty.h"
#include "vtkInteractorStyleImage.h"

#include <stdlib.h>

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
		m_BarWidth = static_cast<int>( m_Width * 0.8 );
		m_BarHeight = static_cast<int>( m_Height * 0.1 );
		
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
		m_XPos = static_cast<int>( home ); 
		
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
    
		this->ImageCanvas->DrawSegment( 
                static_cast<int>( home ),  
                static_cast<int>( 0.6 * m_Height),
                static_cast<int>( home),
                static_cast<int>( 0.6 * m_Height - lh) );
		
		char strLabel[10];
		
		for (unsigned int ii = 0; ii < 6; ii ++)
		{
			sprintf(strLabel, "%02d", 1-ii); 
			this->LabelMapper[ii] = vtkTextMapper::New();
			this->LabelMapper[ii]->SetInput(strLabel);
			this->LabelMapper[ii]->GetTextProperty()->SetFontSize(11);
			this->LabelMapper[ii]->GetTextProperty()->SetFontFamilyToArial();
			this->LabelMapper[ii]->GetTextProperty()->BoldOn();
			this->LabelMapper[ii]->GetTextProperty()->ItalicOff();
			this->LabelMapper[ii]->GetTextProperty()->ShadowOff();
			this->LabelMapper[ii]->GetTextProperty()->SetJustificationToCentered();
			this->LabelMapper[ii]->GetTextProperty()->SetVerticalJustificationToCentered();
			
			this->LabelActor[ii] = vtkActor2D::New();
			this->LabelActor[ii]->SetMapper(this->LabelMapper[ii]);
			this->LabelActor[ii]->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
			this->LabelActor[ii]->GetPositionCoordinate()->SetValue(((m_Width - this->m_BarWidth) / 2.0 + ii * this->m_BarWidth / 5.0) / (float)m_Width, 0.43);
			this->LabelActor[ii]->GetProperty()->SetColor(0, 1, 0);
			
			m_Renderer->AddActor2D(this->LabelActor[ii]);
		}
		
	}
	
	MotionViewer::~MotionViewer()
	{
		this->ImageCanvas->Delete();
		for (unsigned int i = 0; i < 5; i ++)
		{
			this->TextActor[i]->Delete();
			this->TextMapper[i]->Delete();
		}
		
		for (unsigned int j = 0; j < 6; j ++)
		{
			this->LabelActor[j]->Delete();
			this->LabelMapper[j]->Delete();
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
		this->m_XPos = static_cast<int>( home + (x / 5.0) * (this->m_BarWidth / 10.0) );
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
    
		int y0 = static_cast<int>( 0.5 * this->m_Height - dy );
		int y1 = static_cast<int>( 0.5 * this->m_Height + dy );
    
		if (x0 < xmin)
      {
			x0 = xmin;
      }
		if (x0 > xmax)
      {
			x0 = xmax;
      }
		if (x1 < xmin)
      {
			x1 = xmin;
      }
		if (x1 > xmax)
      {
			x1 = xmax;
      }
    
		this->ImageCanvas->FillBox(x0, x1, y0, y1);
		
		int lh = this->m_BarHeight / 4;
		float step = (xmax - xmin) / 10.0; 
    
		this->ImageCanvas->SetDrawColor(0.1843 * 255, 0.3098 * 255, 0.3098 * 255);

		for (float xc = xmin + step; xc < xmax; xc += step)
		{
      const int ixc = static_cast<int>( xc );
			this->ImageCanvas->DrawSegment(ixc, ymin, ixc, ymin + lh);
			this->ImageCanvas->DrawSegment(ixc, m_Height / 2 - lh /2, ixc, m_Height / 2 + lh / 2);
			this->ImageCanvas->DrawSegment(ixc, ymax, ixc, ymax - lh);
		}
	}
	
	void MotionViewer::SetupCamera()
	{
		
		vtkImageData * image = m_Actor->GetInput();
		
		if ( !image )
		{
			return;
		}
		
		vtkFloatingPointType spacing[3];
		vtkFloatingPointType origin[3];
		int   dimensions[3];
		
		image->GetSpacing(spacing);
		image->GetOrigin(origin);
		image->GetDimensions(dimensions);
		
		spacing[0] = spacing[1] = spacing[2] = 1;
		dimensions[0] = m_Width;
		dimensions[1] = m_Height;
		dimensions[2] = 0;
		origin[0] = origin[1] = origin[2] = 0;
		
		double focalPoint[3];
		double position[3];
		
		for ( unsigned int cc = 0; cc < 3; cc++)
		{
			focalPoint[cc] = origin[cc] + ( spacing[cc] * dimensions[cc] ) / 2.0;
			position[cc]   = focalPoint[cc];
		}
		
		const double distanceToFocalPoint = 1000;
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
