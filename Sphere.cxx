#include "Sphere.h"
#include "vtkProperty.h"

Sphere::Sphere( void)
{
	m_Sphere = vtkSphereSource::New();
  m_Sphere->SetThetaResolution(36);
  m_Sphere->SetPhiResolution(36);
	
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

  m_CurRadius = m_Radius = rad;
}


Sphere &Sphere::operator=( const Sphere &src )
{
	this->SetRadius( src.m_Sphere->GetRadius());
  vtkFloatingPointType ftmp[3];
  src.m_Sphere->GetCenter( ftmp );
	this->SetCenter( ftmp[0], ftmp[1], ftmp[2] );
	return *this;
}


void Sphere::SetCenter( double pos[3] )
{
	m_Sphere->SetCenter( pos[0], pos[1], pos[2] );
}

void Sphere::GetCenter( double pos[3] )
{
  vtkFloatingPointType ftmp[3];
	m_Sphere->GetCenter( ftmp );
  for(unsigned int i=0; i<3; i++)
    {
      pos[i] = ftmp[i];
    }
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

void Sphere::SetOpacity(double op)
{
  m_SphereActor->GetProperty()->SetOpacity(op);
}

void Sphere::UpdateRadius()
{
  m_CurRadius += m_Radius / 6.0;

  if (m_CurRadius > m_Radius)
  {
    m_CurRadius = 0;
  }

  m_Sphere->SetRadius( m_CurRadius );	
}

void Sphere::VisibilityOn()
{
  m_SphereActor->VisibilityOn();
}

void Sphere::VisibilityOff()
{
  m_SphereActor->VisibilityOff();
}

void Sphere::AddPosition(float x, float y, float z)
{
  vtkFloatingPointType pos[3];

  m_Sphere->GetCenter(pos);
  m_Sphere->SetCenter(pos[0] + x, pos[1] + y, pos[2] + z);

}
