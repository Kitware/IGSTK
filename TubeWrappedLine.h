#ifndef __ISIS_TubeWrappedLine__h__
#define __ISIS_TubeWrappedLine__h__

#include "vtkLineSource.h"
#include "vtkTubeFilter.h"
#include "vtkPolyDataMapper.h" 
#include "vtkActor.h"

class TubeWrappedLine
{
public:
	void AddPosition(float x, float y, float z);
	void AddPosition(float* pos);

	TubeWrappedLine( void );

	~TubeWrappedLine( void );

	vtkActor* GetVTKActorPointer( void );

	vtkActor* GetVTKActorPointerHip( void );

	void SetEnds( float tip[3], float end[3] );

	// SetTipAndDirection:: The end point is used to get the direction only.
	// The actual endpoint is computed as per the m_Length defined for the line.

	void SetTipAndDirection( float tip[3], float end[3] );

	void SetColor( float r, float g, float b );

	void SetRadius( float rad );

	void SetLength( float length );

	float GetRadius( void );

	float GetLength( void );

//	TubeWrappedLine &operator=( const TubeWrappedLine &src );

//protected:

	vtkLineSource		* m_LineSource;

	vtkTubeFilter		* m_TubeFilter;

	vtkPolyDataMapper	* m_LineMapper;

	vtkActor			* m_LineActor;
	
	vtkLineSource		* m_LineSourceHip;

	vtkTubeFilter		* m_TubeFilterHip;

	vtkPolyDataMapper	* m_LineMapperHip;

	vtkActor			* m_LineActorHip;

	float				m_Length;		// in the measurement unit of the data (mm)

};

#endif
