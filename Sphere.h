#ifndef __ISIS_Sphere__h__
#define __ISIS_Sphere__h__

#include "vtkActor.h"
#include "vtkSphereSource.h" 
#include "vtkPolyDataMapper.h" 

class Sphere
{
public:

	Sphere( void );

	~Sphere( void );

	vtkActor* GetVTKActorPointer( void );

	void SetRadius( const double rad );

	void SetCenter( double pos[3] );

	void SetCenter( const double x, const double y, const double z );

	void GetCenter( double pos[3] );

	void SetColor( const double r, const double g, const double b );

	Sphere &operator=( const Sphere &src );

protected:

	bool				m_ClipOn;

	vtkActor			* m_SphereActor;
	
    vtkSphereSource		* m_Sphere;
	
	vtkPolyDataMapper	* m_SphereMapper;
};

#endif
