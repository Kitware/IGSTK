
#include "IGMTImageSliceViewer.h"
#include "vtkImageData.h"
#include "vtkProperty.h"
#include "vtkProperty2D.h"
#include "vtkTextProperty.h"
#include <math.h>

#include "vtkRenderWindowInteractor.h"

namespace ISIS
{
	IGMTImageSliceViewer::IGMTImageSliceViewer()
	{
    unsigned int i;

		m_SelectedPositionMarker.SetColor(1, 1, 0);
		m_SelectedPositionMarker.SetRadius( 2.0f );

		m_TargetPositionMarker.SetColor(0, 1, 0);
		m_TargetPositionMarker.SetRadius( 2.0f );

//    m_TipPositionMarker.SetColor(1, 1, 0);
//		m_TipPositionMarker.SetRadius( 16.0f );
//    m_TipPositionMarker.SetOpacity( 0.3f );

    for (i = 0; i < 4; i++)
    {
      m_ProbeMarker[i].SetColor(0.5294, 0.8078, 0.9804);
      m_ProbeMarker[i].SetRadius( 2.0f );
      m_ProbeMarker[i].SetLength( 70.0f);
    }  

		m_EntryToTargetPathMarker.SetColor(0.9804, 0.5294, 0.8078);
		m_EntryToTargetPathMarker.SetRadius( 1.0f );

		m_EntryPositionMarker.SetColor(1, 0, 1);
		m_EntryPositionMarker.SetRadius( 2.0f );

		this->GetRenderer()->AddActor( m_SelectedPositionMarker.GetVTKActorPointer() );
		this->GetRenderer()->AddActor( m_TargetPositionMarker.GetVTKActorPointer() );
		this->GetRenderer()->AddActor( m_EntryPositionMarker.GetVTKActorPointer() );
		this->GetRenderer()->AddActor( m_EntryToTargetPathMarker.GetVTKActorPointer() );

//    this->GetRenderer()->AddActor( m_TipPositionMarker.GetVTKActorPointer() );

    for (i = 0; i < 4; i++)
    {
      this->GetRenderer()->AddActor( m_ProbeMarker[i].GetVTKActorPointer() );
    }

		m_LabelMapper = vtkTextMapper::New();
		m_LabelMapper->SetInput("");
		m_LabelMapper->GetTextProperty()->SetFontSize(12);
		m_LabelMapper->GetTextProperty()->SetFontFamilyToArial();
		m_LabelMapper->GetTextProperty()->BoldOn();
		m_LabelMapper->GetTextProperty()->ItalicOff();
		m_LabelMapper->GetTextProperty()->ShadowOff();
		m_LabelMapper->GetTextProperty()->SetJustificationToCentered();
		m_LabelMapper->GetTextProperty()->SetVerticalJustificationToCentered();
		m_LabelMapper->GetTextProperty()->SetLineSpacing(0.6);

		m_LabelActor = vtkActor2D::New();
		m_LabelActor->SetMapper(m_LabelMapper);
		m_LabelActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
		m_LabelActor->GetProperty()->SetColor(1, 1, 0);
		m_LabelActor->GetPositionCoordinate()->SetValue(0.5, 0.1);
		this->GetRenderer()->AddActor(m_LabelActor);

    m_pAnnotation = new IGSTK::Annotation();
    m_pAnnotation->AddActorTo(this->GetRenderer());
    this->GetRenderer()->AddActor(m_pAnnotation->GetAnnotationActor());

		this->DeActivateSelectedPosition();
		this->DeActivateTargetPosition();
		this->DeActivateEntryPosition();
		this->DeActivateEntryToTargetPathMarker();    

//    this->DeActivateTipPosition();
    
    for (i = 0; i < 4; i++)
    {
      this->DeActivateProbe(i);
    }

    for (i = 0; i < 3; i++)
    {
      m_RightClickedPoint[i] = 0;
    }

    m_RightClickedPointValid = false;

	}
	
	IGMTImageSliceViewer::~IGMTImageSliceViewer()
	{
		if (m_LabelMapper)
		{
			m_LabelMapper->Delete();
		}

		if (m_LabelActor)
		{
			m_LabelActor->Delete();
		}

    if (m_pAnnotation)
    {
      delete m_pAnnotation;
    }
	}
	
	void IGMTImageSliceViewer::SelectPoint( int x, int y )
	{
		ImageSliceViewer::SelectPoint(x,y);
	}
	
	
	void IGMTImageSliceViewer::SelectPoint( float x, float y, float z )
	{
		ImageSliceViewer::SelectPoint(x,y,z);
		m_SelectedPositionMarker.SetCenter( x, y, z );
	}
	
	void IGMTImageSliceViewer::ActivateSelectedPosition( void )
	{
		m_SelectedPositionMarker.GetVTKActorPointer()->VisibilityOn();
	}

	
	void IGMTImageSliceViewer::DeActivateSelectedPosition( void )
	{
		m_SelectedPositionMarker.GetVTKActorPointer()->VisibilityOff();
	}

	
	void IGMTImageSliceViewer::ActivateTargetPosition( void )
	{
		m_TargetPositionMarker.GetVTKActorPointer()->VisibilityOn();
	}


	void IGMTImageSliceViewer::DeActivateTargetPosition( void )
	{
		m_TargetPositionMarker.GetVTKActorPointer()->VisibilityOff();
	}
/*
  void IGMTImageSliceViewer::ActivateTipPosition( void )
	{
		m_TipPositionMarker.GetVTKActorPointer()->VisibilityOn();
	}
*/

/*	void IGMTImageSliceViewer::DeActivateTipPosition( void )
	{
		m_TipPositionMarker.GetVTKActorPointer()->VisibilityOff();
	}
*/

	void IGMTImageSliceViewer::ActivateEntryPosition( void )
	{
		m_EntryPositionMarker.GetVTKActorPointer()->VisibilityOn();
	}

	
	void IGMTImageSliceViewer::DeActivateEntryPosition( void )
	{
		m_EntryPositionMarker.GetVTKActorPointer()->VisibilityOff();
	}
	
	void IGMTImageSliceViewer::ActivateEntryToTargetPathMarker( void )
	{
		double entryPoint[3], targetPoint[3];
		m_EntryPositionMarker.GetCenter( entryPoint );
		m_TargetPositionMarker.GetCenter( targetPoint );
		//float length = sqrt((entryPoint[0]-targetPoint[0])*(entryPoint[0]-targetPoint[0])
		//	+(entryPoint[1]-targetPoint[1])*(entryPoint[1]-targetPoint[1])
		//	+(entryPoint[2]-targetPoint[2])*(entryPoint[2]-targetPoint[2]) );
		//m_EntryToTargetPathMarker.SetLength( length );
		m_EntryToTargetPathMarker.SetEnds( entryPoint, targetPoint );
		m_EntryToTargetPathMarker.GetVTKActorPointer()->VisibilityOn();
	}

	
	void IGMTImageSliceViewer::DeActivateEntryToTargetPathMarker( void )
	{
		m_EntryToTargetPathMarker.GetVTKActorPointer()->VisibilityOff();
	}
/*	
	void IGMTImageSliceViewer::ActivateToolPosition( void )
	{
 		m_ToolPositionMarker.GetVTKActorPointer()->VisibilityOn();
 	}

  void IGMTImageSliceViewer::DeActivateToolPosition( void )
	{
 		m_ToolPositionMarker.GetVTKActorPointer()->VisibilityOff();
 	}
*/
  void IGMTImageSliceViewer::ActivateProbe( int i )
  {
    m_ProbeMarker[i].GetVTKActorPointer()->VisibilityOn();
  }
  
	void IGMTImageSliceViewer::DeActivateProbe( int i )
	{
    m_ProbeMarker[i].GetVTKActorPointer()->VisibilityOff();
	}
	
	void IGMTImageSliceViewer::SetOrientation( OrientationType orientation )
	{
		ImageSliceViewer::SetOrientation( orientation );
		
		switch (orientation)
		{
		case Axial:
			m_LabelMapper->SetInput("Axial");
      m_pAnnotation->SetImageOrientation(0);
			break;
		case Coronal:
			m_LabelMapper->SetInput("Coronal");
      m_pAnnotation->SetImageOrientation(1);
			break;
		case Sagittal:
			m_LabelMapper->SetInput("Sagittal");
      m_pAnnotation->SetImageOrientation(2);
			break;
		}		
	}
	
	void IGMTImageSliceViewer::MakeToolPositionMarkerVisible()
	{
    int i;

		vtkImageData* pImageData = m_Actor->GetInput();

		if (!pImageData)
		{
			return;
		}

		int ext[6];

		pImageData->GetExtent(ext);

		switch 	(m_Orientation)
		{
		case Axial:
//			m_ToolPositionMarker.AddPosition(0, 0, ext[5]);
      for (i = 0; i < 4; i++)
      {
        m_ProbeMarker[i].AddPosition(0, 0, -ext[5]);
      }
			break;
		case Coronal:
//			m_ToolPositionMarker.AddPosition(0, -ext[3], 0);
      for (i = 0; i < 4; i++)
      {
        m_ProbeMarker[i].AddPosition(0, -ext[3], 0);
      }
			break;
		case Sagittal:
//			m_ToolPositionMarker.AddPosition(ext[1], 0, 0);
      for (i = 0; i < 4; i++)
      {
        m_ProbeMarker[i].AddPosition(ext[1], 0, 0);
      }
			break;
		}
	}

  void IGMTImageSliceViewer::MakeEntryToTargetPathVisible()
	{
 		vtkImageData* pImageData = m_Actor->GetInput();

		if (!pImageData)
		{
			return;
		}

		int ext[6];

		pImageData->GetExtent(ext);

		switch 	(m_Orientation)
		{
		case Axial:
//			m_ToolPositionMarker.AddPosition(0, 0, ext[5]);
//      for (i = 0; i < 4; i++)
//      {
//        m_ProbeMarker[i].AddPosition(0, 0, -ext[5]);
//      }
      m_EntryPositionMarker.AddPosition(0, 0, -ext[5]);
      m_TargetPositionMarker.AddPosition(0, 0, -ext[5]);
      m_EntryToTargetPathMarker.AddPosition(0, 0, -ext[5]);
			break;
		case Coronal:
//			m_ToolPositionMarker.AddPosition(0, -ext[3], 0);
//      for (i = 0; i < 4; i++)
//      {
//        m_ProbeMarker[i].AddPosition(0, -ext[3], 0);
//      }
      m_EntryPositionMarker.AddPosition(0, -ext[3], 0);
      m_TargetPositionMarker.AddPosition(0, -ext[3], 0);
      m_EntryToTargetPathMarker.AddPosition(0, ext[3], 0);
			break;
		case Sagittal:
//			m_ToolPositionMarker.AddPosition(ext[1], 0, 0);
//      for (i = 0; i < 4; i++)
//      {
//        m_ProbeMarker[i].AddPosition(ext[1], 0, 0);
//      }
      m_EntryPositionMarker.AddPosition(ext[1], 0, 0);
      m_TargetPositionMarker.AddPosition(ext[1], 0, 0);
      m_EntryToTargetPathMarker.AddPosition(ext[1], 0, 0);
			break;
		}
	}

	void IGMTImageSliceViewer::SetInteractor( vtkRenderWindowInteractor * interactor )
	{
		m_RenderWindow->SetInteractor( interactor );
		
		vtkInteractorStyleImage * interactorStyle = vtkInteractorStyleImage::New();
		interactor->SetInteractorStyle( interactorStyle );
		interactorStyle->Delete();
		interactor->AddObserver( ::vtkCommand::LeftButtonPressEvent, m_InteractorObserver );
		interactor->AddObserver( ::vtkCommand::LeftButtonReleaseEvent, m_InteractorObserver );
	}

	void IGMTImageSliceViewer::SetupCamera()
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
		
		float focalPoint[3];
		float position[3];
		
		for ( unsigned int cc = 0; cc < 3; cc++)
		{
			focalPoint[cc] = origin[cc] + ( spacing[cc] * dimensions[cc] ) / 2.0;
			position[cc]   = focalPoint[cc];
		}
		
		int idx = 0;
		const float distanceToFocalPoint = 1000;
		
		switch( m_Orientation )
		{
		case Sagittal:
			{
				idx = 0;
				m_Camera->SetViewUp (     0,  0,  1 );
				position[idx] += distanceToFocalPoint;
				break;
			}
		case Coronal:
			{
				idx = 1;
				m_Camera->SetViewUp (     0,  0,  1 );
				position[idx] -= distanceToFocalPoint;
				break;
			}
		case Axial:
			{
				idx = 2;
				m_Camera->SetViewUp (     0,  -1,  0 );
				position[idx] -= distanceToFocalPoint;
				break;
			}
		}
		
		
		
		m_Camera->SetPosition (   position );
		m_Camera->SetFocalPoint ( focalPoint );
		
#define myMAX(x,y) (((x)>(y))?(x):(y))  
		
		int d1 = (idx + 1) % 3;
		int d2 = (idx + 2) % 3;
		
		float max = myMAX( 
			spacing[d1] * dimensions[d1],
			spacing[d2] * dimensions[d2]);
		
		
		m_Camera->SetParallelScale( max / 2  * m_ZoomFactor );
	}

  void IGMTImageSliceViewer::SetProbeTipAndDirection(int i, float tip[], float end[])
  {
    m_ProbeMarker[i].SetTipAndDirection(tip, end);

    m_ProbeMarker[i].UpdateTipBubbleRadius();

//    m_TipPositionMarker.SetCenter(tip);
  }

  void IGMTImageSliceViewer::ShowAnnotation(int s)
  {
    m_pAnnotation->ShowInfo(s);
  }
/*
  void IGMTImageSliceViewer::UpdateTipRadius()
  {
    m_TipPositionMarker.UpdateRadius();
  }*/

  void IGMTImageSliceViewer::GetImagePosition(int x, int y, float *pos)
{
    if (!m_Actor->GetInput()) 
    {
    return;     // return, if no image is loaded yet.
    }

 
  // Invert the y coordinate (vtk uses opposite y as FLTK)
  int* winsize = m_RenderWindow->GetSize();
  y = winsize[1] - y;

  // Convert display point to world point
  float wpoint[4];
  const double z = m_SliceNum / ( m_FarPlane - m_NearPlane );
  m_Renderer->SetDisplayPoint( x, y, 0 );
  m_Renderer->DisplayToWorld();
  m_Renderer->GetWorldPoint( wpoint );

  // Fix camera Z coorinate to match the current slice
  float spacing[3]={1,1,1};
  float origin[3] ={0,0,0};
  int dimensions[3] = { 100, 100, 100 };
  if ( m_Actor->GetInput() )
    {
    m_Actor->GetInput()->GetSpacing(spacing);
    m_Actor->GetInput()->GetOrigin(origin);
    m_Actor->GetInput()->GetDimensions(dimensions);
    }

  int idx = 0;
  switch( m_Orientation )
    {
  case Sagittal:
      {
      idx = 0;
      break;
      }
  case Coronal:
      {
      idx = 1;
      break;
      }
  case Axial:
      {
      idx = 2;
      break;
      }
    }
  float realz = m_SliceNum * spacing[idx] + origin[idx];
  wpoint[idx] = realz;

  for (int i = 0; i < 3; i++)
  {
    pos[i] = wpoint[i];
  }

  // At this point we have 3D position in the variable wpoint
//  this->SelectPoint(wpoint[0], wpoint[1], wpoint[2]);

//  m_Notifier->InvokeEvent( ClickedPointEvent() );
}

  void IGMTImageSliceViewer::SelectRightPoint(int x, int y)
  {
    if (!m_Actor->GetInput()) 
    {
      return;     // return, if no image is loaded yet.
    }
    
    
    // Invert the y coordinate (vtk uses opposite y as FLTK)
    int* winsize = m_RenderWindow->GetSize();
    y = winsize[1] - y;
    
    // Convert display point to world point
    float wpoint[4];
    const double z = m_SliceNum / ( m_FarPlane - m_NearPlane );
    m_Renderer->SetDisplayPoint( x, y, 0 );
    m_Renderer->DisplayToWorld();
    m_Renderer->GetWorldPoint( wpoint );
    
    // Fix camera Z coorinate to match the current slice
    float spacing[3]={1,1,1};
    float origin[3] ={0,0,0};
    int dimensions[3] = { 100, 100, 100 };
    if ( m_Actor->GetInput() )
    {
      m_Actor->GetInput()->GetSpacing(spacing);
      m_Actor->GetInput()->GetOrigin(origin);
      m_Actor->GetInput()->GetDimensions(dimensions);
    }
    
    int idx = 0;
    switch( m_Orientation )
    {
    case Sagittal:
      {
        idx = 0;
        break;
      }
    case Coronal:
      {
        idx = 1;
        break;
      }
    case Axial:
      {
        idx = 2;
        break;
      }
    }
    float realz = m_SliceNum * spacing[idx] + origin[idx];
    wpoint[idx] = realz;
  
    m_RightClickedPoint[0] = wpoint[0];
    m_RightClickedPoint[1] = wpoint[1];
    m_RightClickedPoint[2] = wpoint[2];

    m_RightClickedPointValid = true;
 
  }
  
  void IGMTImageSliceViewer::GetRightClickedPoint(float point[])
  {
    if (m_RightClickedPointValid)
    {
    for (unsigned int i = 0; i < 3; i++)
    {
      point[i] = m_RightClickedPoint[i];
    }
    m_RightClickedPointValid = false;
    }
  }

}







