#ifndef __ISIS_ImageSliceViewer01_h__
#define __ISIS_ImageSliceViewer01_h__

#include "ImageSliceViewer.h"
#include "vtkSphereSource.h" 
#include "vtkPolyDataMapper.h" 

namespace ISIS
{
  
  class ImageSliceViewer01 : public ImageSliceViewer
  {
  public:

    ImageSliceViewer01();

    ~ImageSliceViewer01();

    virtual void SelectPoint( int x, int y );

    virtual void SelectPoint( float x, float y, float z );

    vtkActor          * m_SphereActor;

    vtkSphereSource   * m_Sphere;

 };
  
  
}  // end namespace ISIS

#endif


