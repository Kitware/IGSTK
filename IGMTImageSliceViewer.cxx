
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
		m_SelectedPositionMarker.SetColor(1, 1, 0);
		m_SelectedPositionMarker.SetRadius( 2.0f );

		m_TargetPositionMarker.SetColor(0, 1, 0);
		m_TargetPositionMarker.SetRadius( 2.0f );

		m_ToolPositionMarker.SetColor(0.5294, 0.8078, 0.9804);
		m_ToolPositionMarker.SetRadius( 2.0f );
		m_ToolPositionMarker.SetLength( 70.0f);

		m_EntryToTargetPathMarker.SetColor(0.9804, 0.5294, 0.8078);
		m_EntryToTargetPathMarker.SetRadius( 1.0f );
		//m_EntryToTargetPathMarker.SetLength( 40.0f);

		m_EntryPositionMarker.SetColor(1, 0, 1);
		m_EntryPositionMarker.SetRadius( 2.0f );

//		m_Actor->GetProperty()->SetOpacity(0.5);

		this->GetRenderer()->AddActor( m_SelectedPositionMarker.GetVTKActorPointer() );
		this->GetRenderer()->AddActor( m_TargetPositionMarker.GetVTKActorPointer() );
		this->GetRenderer()->AddActor( m_EntryPositionMarker.GetVTKActorPointer() );
		this->GetRenderer()->AddActor( m_EntryToTargetPathMarker.GetVTKActorPointer() );
		this->GetRenderer()->AddActor( m_ToolPositionMarker.GetVTKActorPointer() );

		this->GetRenderer()->AddActor( m_ToolPositionMarker.GetVTKActorPointerHip() );

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

		this->DeActivateSelectedPosition();
		this->DeActivateTargetPosition();
		this->DeActivateEntryPosition();
		this->DeActivateToolPosition();
		this->DeActivateEntryToTargetPathMarker();
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
	
	void IGMTImageSliceViewer::ActivateToolPosition( void )
	{
		m_ToolPositionMarker.GetVTKActorPointer()->VisibilityOn();
	}

	
	void IGMTImageSliceViewer::DeActivateToolPosition( void )
	{
		m_ToolPositionMarker.GetVTKActorPointer()->VisibilityOff();
	}
	
	void IGMTImageSliceViewer::SetOrientation( OrientationType orientation )
	{
		ImageSliceViewer::SetOrientation( orientation );
		
		switch (orientation)
		{
		case Axial:
			m_LabelMapper->SetInput("Axial");
			break;
		case Coronal:
			m_LabelMapper->SetInput("Coronal");
			break;
		case Saggital:
			m_LabelMapper->SetInput("Saggittal");
			break;
		}		
	}
	
	void IGMTImageSliceViewer::MakeToolPositionMarkerVisible()
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
			m_ToolPositionMarker.AddPosition(0, 0, -ext[5]);
			break;
		case Coronal:
			m_ToolPositionMarker.AddPosition(0, -ext[3], 0);
			break;
		case Saggital:
			m_ToolPositionMarker.AddPosition(ext[1], 0, 0);
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
//		interactor->AddObserver( ::vtkCommand::MouseMoveEvent, m_InteractorObserver );
	}

}


