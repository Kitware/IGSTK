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

	void SetRadius( float rad );

	void SetCenter( float pos[3] );

	void SetCenter( float x, float y, float z );

	void GetCenter( float pos[3] );

	void SetColor( float r, float g, float b );

	Sphere &operator=( const Sphere &src );

protected:

	bool				m_ClipOn;

	vtkActor			* m_SphereActor;
	
    vtkSphereSource		* m_Sphere;
	
	vtkPolyDataMapper	* m_SphereMapper;
};

#endif
