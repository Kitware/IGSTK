#ifndef __ISIS_TubeWrappedLine__h__
#define __ISIS_TubeWrappedLine__h__

#include "vtkLineSource.h"
#include "vtkTubeFilter.h"
#include "vtkPolyDataMapper.h" 
#include "vtkActor.h"
#include "vtkAssembly.h"

#include "Sphere.h"

class TubeWrappedLine
{
public:
//<<<<<<< TubeWrappedLine.h
	void TipBubbleVisibilityOn();
  void TipBubbleVisibilityOff();
	void UpdateTipBubbleRadius();
	void VisibilityOff();
	void VisiblityOn();
//	void AddPosition(float x, float y, float z);
//	void AddPosition(float* pos);
//=======
	void AddPosition(const double x, const double y, const double z);
	void AddPosition(double* pos);
//>>>>>>> 1.2

	TubeWrappedLine( void );

	~TubeWrappedLine( void );

	vtkProp* GetVTKActorPointer( void );

	void SetEnds( double tip[3], double end[3] );

	// SetTipAndDirection:: The end point is used to get the direction only.
	// The actual endpoint is computed as per the m_Length defined for the line.

	void SetTipAndDirection( double tip[3], double end[3] );

	void SetColor( double r, double g, double b );

	void SetRadius( double rad );

	void SetLength( double length );

	double GetRadius( void );

	double GetLength( void );

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

//<<<<<<< TubeWrappedLine.h
  vtkAssembly      * m_ProbeActor;

  Sphere  m_TipBubble;

//	float				m_Length;		// in the measurement unit of the data (mm)
//=======
	double				m_Length;		// in the measurement unit of the data (mm)
//>>>>>>> 1.2

};

#endif
