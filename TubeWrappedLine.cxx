#include "TubeWrappedLine.h"
#include "vtkProperty.h"
#include <math.h>

TubeWrappedLine::TubeWrappedLine( void)
{
	m_LineSource = vtkLineSource::New();
	m_LineSource->SetResolution( 25 );

	m_TubeFilter = vtkTubeFilter::New();
	m_TubeFilter->SetInput( m_LineSource->GetOutput() );
	m_TubeFilter->SetNumberOfSides(5);
	m_TubeFilter->SetVaryRadiusToVaryRadiusByScalar();
	m_TubeFilter->SetRadius(5.0f); //2.0f
	m_TubeFilter->SetCapping(1);

	m_LineMapper = vtkPolyDataMapper::New();
	m_LineMapper->SetInput( m_TubeFilter->GetOutput() ); 

	m_LineActor = vtkActor::New();
	m_LineActor->SetMapper( m_LineMapper );
	m_LineActor->GetProperty()->SetDiffuseColor( 0.2, 1, 1 );

    m_LineActor->GetProperty()->SetColor(0.5294, 0.8078, 0.9804);
    m_LineActor->GetProperty()->SetSpecularColor(1, 1, 1);
    m_LineActor->GetProperty()->SetSpecular(0.4);
    m_LineActor->GetProperty()->SetSpecularPower(5);
    m_LineActor->GetProperty()->SetAmbient(0.2);
    m_LineActor->GetProperty()->SetDiffuse(0.8);

	m_LineSourceHip = vtkLineSource::New();
	m_LineSourceHip->SetResolution( 25 );

	m_TubeFilterHip = vtkTubeFilter::New();
	m_TubeFilterHip->SetInput( m_LineSourceHip->GetOutput() );
	m_TubeFilterHip->SetNumberOfSides(36);
	m_TubeFilterHip->SetVaryRadiusToVaryRadiusByScalar();
	m_TubeFilterHip->SetRadius(3.5f); //2.0f
	m_TubeFilterHip->SetCapping(1);

	m_LineMapperHip = vtkPolyDataMapper::New();
	m_LineMapperHip->SetInput( m_TubeFilterHip->GetOutput() ); 

	m_LineActorHip = vtkActor::New();
	m_LineActorHip->SetMapper( m_LineMapperHip );
	m_LineActorHip->GetProperty()->SetDiffuseColor( 0.2, 1, 1 );

    m_LineActorHip->GetProperty()->SetColor(1.000, 0.032, 0.798);
    m_LineActorHip->GetProperty()->SetSpecularColor(1, 1, 1);
    m_LineActorHip->GetProperty()->SetSpecular(0.4);
    m_LineActorHip->GetProperty()->SetSpecularPower(5);
    m_LineActorHip->GetProperty()->SetAmbient(0.2);
    m_LineActorHip->GetProperty()->SetDiffuse(0.8);
	m_LineActorHip->GetProperty()->SetOpacity(0.35);
  
	m_Length = 100.0f;	
}

TubeWrappedLine::~TubeWrappedLine( void)
{
	if( m_LineSource ) m_LineSource->Delete();
	
	if( m_TubeFilter ) m_TubeFilter->Delete();  
	
	if( m_LineMapper ) m_LineMapper->Delete();

	if( m_LineActor ) m_LineActor->Delete();  	
	
	if( m_LineSourceHip ) m_LineSourceHip->Delete();
	
	if( m_TubeFilterHip ) m_TubeFilterHip->Delete();  
	
	if( m_LineMapperHip ) m_LineMapperHip->Delete();

	if( m_LineActorHip ) m_LineActorHip->Delete(); 
}


void TubeWrappedLine::SetEnds( double tip[3], double end[3] )
{
	m_LineSource->SetPoint1( tip ); 
	m_LineSource->SetPoint2( end );
	double diff[3];
	diff[0] = end[0] - tip[0];
	diff[1] = end[1] - tip[1];
	diff[2] = end[2] - tip[2];
	m_Length = sqrt(diff[0]*diff[0] + diff[1]*diff[1] + diff[2]*diff[2]);
}

void TubeWrappedLine::SetTipAndDirection( double tip[3], double end[3] )
{
	m_LineSource->SetPoint1( tip ); 
	double direction[3];
	direction[0] = end[0] - tip[0];
	direction[1] = end[1] - tip[1];
	direction[2] = end[2] - tip[2];
	double length = sqrt(direction[0]*direction[0]+direction[1]*direction[1]+direction[2]*direction[2]);
	direction[0] /= length;
	direction[1] /= length;
	direction[2] /= length;
	double otherEnd[3];
	otherEnd[0] = tip[0] + m_Length * direction[0];
	otherEnd[1] = tip[1] + m_Length * direction[1];
	otherEnd[2] = tip[2] + m_Length * direction[2];
	m_LineSource->SetPoint2( otherEnd );

	m_LineSourceHip->SetPoint1( otherEnd );
	otherEnd[0] -= m_Length * direction[0] / 6.0f;
	otherEnd[1] -= m_Length * direction[1] / 6.0f;
	otherEnd[2] -= m_Length * direction[2] / 6.0f;
	m_LineSourceHip->SetPoint2( otherEnd );
}


void TubeWrappedLine::SetColor( double r, double g, double b )
{
	m_LineActor->GetProperty()->SetColor(r, g, b);
}


vtkActor* TubeWrappedLine::GetVTKActorPointer( void )
{
	return m_LineActor;
}

vtkActor* TubeWrappedLine::GetVTKActorPointerHip( void )
{
	return m_LineActorHip;
}

void TubeWrappedLine::SetRadius( double rad )
{
	m_TubeFilter->SetRadius( rad );
}


double TubeWrappedLine::GetRadius( void )
{
	return m_TubeFilter->GetRadius();
}

void TubeWrappedLine::SetLength( double len )
{
	m_Length = len;
}

double TubeWrappedLine::GetLength( void )
{
	return m_Length;
}

void TubeWrappedLine::AddPosition(double *pos)
{
	AddPosition(pos[0], pos[1], pos[2]);	
}

void TubeWrappedLine::AddPosition(double x, double y, double z)
{
	double point[3];

	m_LineSource->GetPoint1(point);
	m_LineSource->SetPoint1(point[0] + x, point[1] + y, point[2] + z);

	m_LineSource->GetPoint2(point);
	m_LineSource->SetPoint2(point[0] + x, point[1] + y, point[2] + z);

	m_LineSourceHip->GetPoint1(point);
	m_LineSourceHip->SetPoint1(point[0] + x, point[1] + y, point[2] + z);

	m_LineSourceHip->GetPoint2(point);
	m_LineSourceHip->SetPoint2(point[0] + x, point[1] + y, point[2] + z);

}
