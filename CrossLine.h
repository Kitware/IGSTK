
#ifndef _CROSSLINE_H_
#define _CROSSLINE_H_

#include "vtkActor.h"
#include "vtkActor2D.h"
#include "vtkAssembly.h"
#include "vtkLineSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkRenderer.h"

namespace IGSTK
{

class CrossLine  
{
public:
	CrossLine();
	virtual ~CrossLine();
 
public:
	void SetColor(float r, float g, float b);
	void AddActorsToRenderer(vtkRenderer* renderer);
	void SetCenter(double x, double y);

  vtkLineSource* m_HLine;
	
	vtkPolyDataMapper2D* m_HLineMapper;

  vtkActor2D* m_HLineActor;

  vtkLineSource* m_VLine;
	
	vtkPolyDataMapper2D* m_VLineMapper;

  vtkActor2D* m_VLineActor;

};

}

#endif

