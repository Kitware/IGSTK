
#ifndef __ISIS_ImageSliceViewer_h__
#define __ISIS_ImageSliceViewer_h__

#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkImageActor.h"
#include "vtkRenderWindow.h"
#include "vtkCommand.h"
#include "vtkInteractorStyleImage.h"
#include "vtkImageReslice.h"
#include "vtkImageResample.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkImageMapToColors.h"
#include "vtkImageDataGeometryFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkPlaneSource.h"
#include "vtkTexture.h"
#include "vtkTransform.h" 
#include "vtkTransformPolyDataFilter.h"

#include "itkEventObject.h"
#include "itkCommand.h"

namespace ISIS
{
	/** \class ImageSlicerViewer
	    \brief This class visualizes a volume
      \warning
	    \sa everyone needs to look at doxygen tags
  */

class ImageSliceViewer
{
public:

  typedef enum
    { 
    Sagittal, 
    Coronal, 
    Axial 
    } 
    OrientationType;

  ImageSliceViewer( void );

  ImageSliceViewer( vtkRenderWindow* renWin );

  void Init( vtkRenderWindow* renWin );

  virtual ~ImageSliceViewer( void );

  void SetInput( vtkImageData * image );

  void SelectSlice( int slice );

  void SelectPoint( int x, int y);

  void MovePoint( int x, int y, bool firstmove);

  virtual void SetInteractor( vtkRenderWindowInteractor * interactor );

  void Render( void );

  void SetOrientation( OrientationType orientation );

  vtkRenderer *  GetRenderer( void );

  virtual void SelectPoint( double x, double y, double z );

  virtual void  GetSelectPoint(double data[3]); 
  
  unsigned long AddObserver( const itk::EventObject & event, itk::Command *);

  virtual void SetZoomFactor( double factor );

  void GetWindowLevelWidth(double& level, double& width);

  void SetWindowLevelWidth(double level, double width);

  vtkImageData* GetInput();

protected:

  virtual void SetupCamera( void );


public:
	virtual void SelectRightPoint(int x, int y);
	
  vtkImageActor     * m_ImageActor;

  vtkRenderer       * m_Renderer;

  vtkCamera         * m_Camera;

  vtkRenderWindow   * m_RenderWindow;

  OrientationType     m_Orientation;

  int                 m_SliceNum;

  vtkCommand        * m_InteractorObserver;

  itk::Object::Pointer   m_Notifier;

  vtkImageMapToWindowLevelColors  * m_ImageMap;

  vtkImageDataGeometryFilter      * m_GeoFilter;

  vtkPolyDataMapper   * m_Mapper;

  vtkActor            * m_Actor;

  vtkWindowLevelLookupTable   * m_LUT;

  vtkImageReslice     * m_ImageReslice;

  vtkPlaneSource      * m_Plane;

  vtkTexture          * m_Texture;

  vtkTransformPolyDataFilter  * m_TransformFilter;

  vtkTransform        * m_Transform;

  int                 m_ViewerMode;

  double              m_NearPlane;

  double              m_FarPlane;

  double              m_ZoomFactor;

  int                 m_Ext[6];

  vtkFloatingPointType            m_Origin[3];

  vtkFloatingPointType               m_Spacing[3];

  double              m_SelectPoint[3];

  double              m_DragPoint[2];

  double              m_Window[2];

  double              m_AxisX[3], m_AxisY[3], m_AxisZ[3];
};


}  // end namespace ISIS

#endif



