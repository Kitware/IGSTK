#include "ImageSliceViewer01.h"
#include "vtkImageData.h"

namespace ISIS
{
	ImageSliceViewer01::ImageSliceViewer01() 
	{
		m_Sphere      = vtkSphereSource::New();
		m_SphereActor = vtkActor::New();  
		
		vtkPolyDataMapper * sphereMapper = vtkPolyDataMapper::New();
		
		sphereMapper->SetInput( m_Sphere->GetOutput() );  
		m_SphereActor->SetMapper( sphereMapper );
		this->GetRenderer()->AddActor( m_SphereActor );
		
		m_Sphere->SetRadius(0.0f);
		
		sphereMapper->Delete();
		
	}
	

	ImageSliceViewer01::~ImageSliceViewer01() 
	{
		if( m_Sphere )
		{
			m_Sphere->Delete();
		}   
		
		if( m_SphereActor )
		{
			m_SphereActor->Delete();
		}  	
	}
	


	void ImageSliceViewer01::SelectPoint( int x, int y )
	{
		ImageSliceViewer::SelectPoint(x,y);
	}
	
	
	void ImageSliceViewer01::SelectPoint( float x, float y, float z )
	{
		if (!m_Actor->GetInput()) 
		{
			return;     // return, if no image is loaded yet.
		}

		ImageSliceViewer::SelectPoint(x,y,z);

		m_SphereActor->SetPosition( x, y, z );

		int dimensions[3] = { 100, 100, 100 };

		if ( m_Actor->GetInput() )
		{
			m_Actor->GetInput()->GetDimensions(dimensions);
			
			switch( m_Orientation )
			{
			case Sagittal:
				m_Sphere->SetRadius(dimensions[0]/75);
				break;
			case Coronal:
				m_Sphere->SetRadius(dimensions[1]/75);
				break;
			case Axial:
				m_Sphere->SetRadius(dimensions[2]/75);
				break;
			}
		}
	}

}



