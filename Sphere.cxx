#include "Sphere.h"
#include "vtkProperty.h"

Sphere::Sphere( void)
{
	m_Sphere      = vtkSphereSource::New();
	
	m_SphereActor = vtkActor::New();  
	
	m_SphereMapper = vtkPolyDataMapper::New();
	
	m_SphereMapper->SetInput( m_Sphere->GetOutput() );
	
	m_SphereActor->SetMapper( m_SphereMapper );
	
	this->SetRadius(10.0f); //2.0f
	
    m_SphereActor->GetProperty()->SetColor(0.5294, 0.8078, 0.9804);
    m_SphereActor->GetProperty()->SetSpecularColor(1, 1, 1);
    m_SphereActor->GetProperty()->SetSpecular(0.4);
    m_SphereActor->GetProperty()->SetSpecularPower(5);
    m_SphereActor->GetProperty()->SetAmbient(0.2);
    m_SphereActor->GetProperty()->SetDiffuse(0.8);
}

Sphere::~Sphere( void)
{
	if( m_Sphere )
	{
		m_Sphere->Delete();
	}   
	
	if( m_SphereActor )
	{
		m_SphereActor->Delete();
	}  
	
	if( m_SphereMapper )
	{
		m_SphereMapper->Delete();
	}   	
}


void Sphere::SetRadius( const double rad)
{
	m_Sphere->SetRadius( rad );	
}


Sphere &Sphere::operator=( const Sphere &src )
{
	this->SetRadius( src.m_Sphere->GetRadius());
	this->SetCenter( src.m_Sphere->GetCenter());
	return *this;
}


void Sphere::SetCenter( double pos[3] )
{
	m_Sphere->SetCenter( pos );
}

void Sphere::GetCenter( double pos[3] )
{
	m_Sphere->GetCenter( pos );
}

void Sphere::SetCenter( const double x, const double y, const double z )
{
	m_Sphere->SetCenter( x, y, z );
}



void Sphere::SetColor( const double r, const double g, const double b )
{
	m_SphereActor->GetProperty()->SetColor(r, g, b);
}



vtkActor* Sphere::GetVTKActorPointer( void )
{
	return m_SphereActor;
}