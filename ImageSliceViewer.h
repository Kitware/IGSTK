
#ifndef __ISIS_ImageSliceViewer_h__
#define __ISIS_ImageSliceViewer_h__

#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkImageActor.h"
#include "vtkRenderWindow.h"
#include "vtkCommand.h"
#include "vtkInteractorStyleImage.h"

#include "itkEventObject.h"
#include "itkCommand.h"

namespace ISIS
{

class ImageSliceViewer
{
public:

  typedef enum
    { 
    Saggital, 
    Coronal, 
    Axial 
    } 
    OrientationType;

  ImageSliceViewer( void );

  virtual ~ImageSliceViewer( void );

  void SetInput( vtkImageData * image );

  void SelectSlice( int slice );

  void SelectPoint( int x, int y);

  virtual void SetInteractor( vtkRenderWindowInteractor * interactor );

  void Render( void );

  void SetOrientation( OrientationType orientation );

  vtkRenderer *  GetRenderer( void );

  virtual void SelectPoint( float x, float y, float z );

  virtual void  GetSelectPoint(float data[3]); 
  
  unsigned long AddObserver( const itk::EventObject & event, itk::Command *);

  virtual void SetZoomFactor( double factor );

protected:

  void SetupCamera( void );


public:

  vtkImageActor     * m_Actor;

  vtkRenderer       * m_Renderer;

  vtkCamera         * m_Camera;

  vtkRenderWindow   * m_RenderWindow;

  OrientationType     m_Orientation;

  int                 m_SliceNum;

  vtkCommand        * m_InteractorObserver;

  itk::Object::Pointer   m_Notifier;

  double              m_NearPlane;

  double              m_FarPlane;

  double              m_ZoomFactor;

  float               m_SelectPoint[3];
};


}  // end namespace ISIS

#endif



