#include "ImageSliceViewer.h"

#include "vtkImageData.h"
#include "vtkInteractorStyleImage.h"
#include "vtkLookupTable.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"

#include "InteractorObserver.h"

#include "itkPoint.h"

#include "ClickedPointEvent.h"

namespace ISIS
{
void
ImageSliceViewer
::Init( vtkRenderWindow* renWin )
{
  m_ViewerMode = 2;

  m_ImageActor    = vtkImageActor::New();

  m_Renderer      = vtkRenderer::New();

  m_RenderWindow  = renWin;

  m_Renderer->AddActor( m_ImageActor );

  m_GeoFilter = vtkImageDataGeometryFilter::New();

  m_Mapper = vtkPolyDataMapper::New();

  m_Actor = vtkActor::New();

  m_Renderer->AddActor( m_Actor );

  m_Camera = m_Renderer->GetActiveCamera();

  m_Camera->ParallelProjectionOn();

  m_RenderWindow->AddRenderer( m_Renderer );

  m_ImageMap = vtkImageMapToWindowLevelColors::New();

  m_LUT = vtkWindowLevelLookupTable::New();

  m_ImageReslice = vtkImageReslice::New();

  m_Plane = vtkPlaneSource::New();

  m_Texture = vtkTexture::New();

  m_Transform = vtkTransform::New();

  m_TransformFilter = vtkTransformPolyDataFilter::New();
  
  this->SetOrientation( Axial );

  InteractorObserver * observer = InteractorObserver::New();
  observer->SetImageSliceViewer( this );
  m_InteractorObserver = observer;

  m_Notifier      = itk::Object::New();

  m_NearPlane = 0.1;

  m_FarPlane  = 1000.0;

  m_ZoomFactor = 1.0;

  m_Renderer->SetBackground( 0, 0, 0 );

  m_Window[0] = -600;

  m_Window[1] = 1500;
}

ImageSliceViewer
::ImageSliceViewer()
{

  m_RenderWindow  = vtkRenderWindow::New();

  this->Init( m_RenderWindow );

}

ImageSliceViewer
::ImageSliceViewer( vtkRenderWindow* renWin )
{

  this->Init( renWin );

}



ImageSliceViewer
::~ImageSliceViewer()
{
  if( m_RenderWindow )
  {
    m_RenderWindow->Delete();
  }   

  if ( m_ImageMap )
  {
    m_ImageMap->Delete();
  }

  if (m_ImageActor)
  {
    m_ImageActor->Delete();
  }

  if( m_InteractorObserver )
  {
    m_InteractorObserver->Delete();
  }

  if (m_Renderer)
  {
    m_Renderer->Delete();
  }

  if (m_GeoFilter)
  {
    m_GeoFilter->Delete();
  }

  if (m_Mapper)
  {
    m_Mapper->Delete();
  }

  if (m_LUT)
  {
    m_LUT->Delete();
  }

  if (m_ImageReslice)
  {
    m_ImageReslice->Delete();
  }

  if (m_Plane)
  {
    m_Plane->Delete();
  }

  if (m_Texture)
  {
    m_Texture->Delete();
  }

  if (m_Transform)
  {
    m_Transform->Delete();
  }

  if (m_TransformFilter)
  {
    m_TransformFilter->Delete();
  }
}



void
ImageSliceViewer
::Render()
{
  m_Camera->SetClippingRange( 0.1, 100000 );
  m_RenderWindow->Render();
}



vtkRenderer *
ImageSliceViewer
::GetRenderer()
{
  return m_Renderer;
}

vtkImageData *
ImageSliceViewer
::GetInput()
{
  switch ( m_ViewerMode )
  {
  case 0: // ImageActor
    return m_ImageActor->GetInput();
  case 1: // Actor
    return m_GeoFilter->GetInput();
  case 2:
    return m_ImageReslice->GetInput();
  }
  return NULL;
}


void
ImageSliceViewer
::SetInput( vtkImageData * image )
{
  
  image->GetExtent( m_Ext );
  image->GetOrigin( m_Origin );
  image->GetSpacing( m_Spacing );

  switch ( m_ViewerMode )
  {
  case 0:
    m_ImageMap->SetInput( image );  
    m_ImageActor->SetInput( m_ImageMap->GetOutput() );
    m_ImageMap->UpdateWholeExtent();
    break;
  case 1:    
    m_GeoFilter->SetInput( image );
    m_Mapper->SetInput( m_GeoFilter->GetOutput() );
    m_Actor->SetMapper( m_Mapper );
    break;
  case 2:    
    m_ImageReslice->SetInput( image );
    m_ImageReslice->SetResliceAxesDirectionCosines( m_AxisX, m_AxisY, m_AxisZ );
    m_ImageReslice->SetResliceAxesOrigin( m_Origin[0], m_Origin[1], m_Origin[2] );
    m_ImageReslice->SetOutputDimensionality(2);
    m_Texture->SetInput( m_ImageReslice->GetOutput() );
    m_Transform->Identity();
    m_Transform->Translate( m_Origin[0], m_Origin[1], m_Origin[2] );
    m_Transform->Scale( m_Spacing[0] * (m_Ext[1] - m_Ext[0] + 1), m_Spacing[1] * (m_Ext[3] - m_Ext[2] + 1), m_Spacing[2] * (m_Ext[5] - m_Ext[4] + 1));
    m_Transform->Translate( 0.5, 0.5, 0 );
    m_Transform->RotateX( 90.0 );
    m_TransformFilter->SetTransform( m_Transform );
    m_TransformFilter->SetInput( m_Plane->GetOutput() );
    m_Mapper->SetInput( m_TransformFilter->GetOutput() );
    m_Actor->SetMapper( m_Mapper );
    m_Actor->SetTexture( m_Texture );
    break;
  }
  
  this->SetupCamera();
  this->SetWindowLevelWidth( m_Window[0], m_Window[1] );

}



void
ImageSliceViewer
::SetupCamera()
{

  vtkImageData * image = this->GetInput();

  if ( !image )
    {
    return;
    }

  vtkFloatingPointType spacing[3];
  vtkFloatingPointType origin[3];
  int   dimensions[3];

  image->UpdateData();

  image->GetSpacing(spacing);
  image->GetOrigin(origin);
  image->GetDimensions(dimensions);

  double focalPoint[3];
  double position[3];

  for ( unsigned int cc = 0; cc < 3; cc++)
    {
    focalPoint[cc] = origin[cc] + ( spacing[cc] * dimensions[cc] ) / 2.0;
    position[cc]   = focalPoint[cc];
    }

  int idx = 0;
  switch( m_Orientation )
    {
  case Sagittal:
      {
      idx = 0;
      m_Camera->SetViewUp (     0,  0,  -1 );
      break;
      }
  case Coronal:
      {
      idx = 1;
      m_Camera->SetViewUp (     0,  0,  -1 );
      break;
      }
  case Axial:
      {
      idx = 2;
      m_Camera->SetViewUp (     0,  -1,  0 );      
      break;
      }
    }

  const double distanceToFocalPoint = 1000;
  position[idx] += distanceToFocalPoint;

  m_Camera->SetPosition (   position );
  m_Camera->SetFocalPoint ( focalPoint );

#define myMAX(x,y) (((x)>(y))?(x):(y))  

   int d1 = (idx + 1) % 3;
   int d2 = (idx + 2) % 3;
 
  double max = myMAX( 
    spacing[d1] * dimensions[d1],
    spacing[d2] * dimensions[d2]);


  m_Camera->SetParallelScale( max / 2  * m_ZoomFactor );

}


void
  ImageSliceViewer
::SetZoomFactor( double factor )
{
  m_ZoomFactor = factor;
}



void
  ImageSliceViewer
::SetInteractor( vtkRenderWindowInteractor * interactor )
{
  m_RenderWindow->SetInteractor( interactor );

  vtkInteractorStyleImage * interactorStyle = vtkInteractorStyleImage::New();
  interactor->SetInteractorStyle( interactorStyle );
  interactorStyle->Delete();
  interactor->AddObserver( ::vtkCommand::LeftButtonPressEvent, m_InteractorObserver );
  interactor->AddObserver( ::vtkCommand::LeftButtonReleaseEvent, m_InteractorObserver );
  interactor->AddObserver( ::vtkCommand::MouseMoveEvent, m_InteractorObserver );
}





void
  ImageSliceViewer
::SetOrientation( OrientationType orientation )
{
  m_Orientation = orientation;
  switch ( m_Orientation )
  {
  case Axial:
    m_AxisX[0] = 1;
    m_AxisX[1] = 0;
    m_AxisX[2] = 0;
    m_AxisY[0] = 0;
    m_AxisY[1] = 1;
    m_AxisY[2] = 0;
    m_AxisZ[0] = 0;
    m_AxisZ[1] = 0;
    m_AxisZ[2] = 1;
    break;
  case Coronal:
    m_AxisX[0] = 1;
    m_AxisX[1] = 0;
    m_AxisX[2] = 0;
    m_AxisY[0] = 0;
    m_AxisY[1] = 0;
    m_AxisY[2] = 1;
    m_AxisZ[0] = 0;
    m_AxisZ[1] = 1;
    m_AxisZ[2] = 0;
    break;
  case Sagittal:
    m_AxisX[0] = 0;
    m_AxisX[1] = 0;
    m_AxisX[2] = 1;
    m_AxisY[0] = 0;
    m_AxisY[1] = 1;
    m_AxisY[2] = 0;
    m_AxisZ[0] = 1;
    m_AxisZ[1] = 0;
    m_AxisZ[2] = 0;
    break;
  }
  this->SetupCamera();
}




void
  ImageSliceViewer
::SelectSlice( int slice )
{
  if (!this->GetInput()) 
    {
    return;     // return, if no image is loaded yet.
    }

  int ext[6];
  this->GetInput()->GetExtent( ext );

  memcpy(ext, m_Ext, 6 * sizeof(int));

  switch( m_Orientation )
    {
  case Sagittal:
      if ((slice>=ext[0]) && (slice<=ext[1]))
      {
        ext[0] = slice;
        ext[1] = slice;
        m_SliceNum = slice;
      }
      break;
  case Coronal:
      if ((slice>=ext[2]) && (slice<=ext[3]))
      {
        ext[2] = slice;
        ext[3] = slice;
        m_SliceNum = slice;
      }
      break;
  case Axial:
      if ((slice>=ext[4]) && (slice<=ext[5]))
      {
        ext[4] = slice;
        ext[5] = slice;
        m_SliceNum = slice;
      }
      break;
    }

  switch ( m_ViewerMode )
  {
  case 0:
    m_ImageActor->SetDisplayExtent( ext );
    break;
  case 1:
    m_GeoFilter->SetExtent( ext );    
    break;
  case 2:
    m_ImageReslice->SetResliceAxesDirectionCosines( m_AxisX, m_AxisY, m_AxisZ);
    m_ImageReslice->SetResliceAxesOrigin( m_Origin[0] + ext[0] * m_Spacing[0], m_Origin[1] + ext[2] * m_Spacing[1], m_Origin[2] + ext[4] * m_Spacing[2] );
    m_ImageReslice->SetOutputDimensionality(2);
    m_Transform->Identity();
    m_Transform->Translate( m_Origin[0] + ext[0] * m_Spacing[0], m_Origin[1] + ext[2] * m_Spacing[1], m_Origin[2] + ext[4] * m_Spacing[2] );
    m_Transform->Scale( m_Spacing[0] * (m_Ext[1] - m_Ext[0] + 1), m_Spacing[1] * (m_Ext[3] - m_Ext[2] + 1), m_Spacing[2] * (m_Ext[5] - m_Ext[4] + 1));
    
    switch ( m_Orientation )
    {
    case Axial:
      m_Transform->Translate( 0.5, 0.5, 0 );
      break;
    case Coronal:
      m_Transform->Translate( 0.5, 0, 0.5 );
      m_Transform->RotateX( 90 );
      break;
    case Sagittal:
      m_Transform->Translate( 0, 0.5, 0.5 );
      m_Transform->RotateY( -90 );
      break;
    }
    m_TransformFilter->SetTransform( m_Transform );
    break;
  }
  
}

void
ImageSliceViewer
::MovePoint( int x, int y, bool firstmove )
{
  if (!this->GetInput()) 
  {
    return;
  }

  double dx, dy;

  if ( !firstmove )
  {

    dx = (double)(x - m_DragPoint[0]) / m_RenderWindow->GetSize()[0] * 1000;
    dy = (double)(y - m_DragPoint[1]) / m_RenderWindow->GetSize()[1] * 1000;

    m_Window[0] += dx;
    m_Window[1] += dy;

    m_Notifier->InvokeEvent( UpdateWindowLevelWidthEvent() );

  }

  m_DragPoint[0] = x;
  m_DragPoint[1] = y;  

}

void
ImageSliceViewer
::GetWindowLevelWidth(double& level, double& width)
{
  level = m_Window[0];
  width = m_Window[1];
}

void
ImageSliceViewer
::SetWindowLevelWidth(double level, double width)
{
  m_Window[0] = level;
  m_Window[1] = width;

  switch ( m_ViewerMode )
  {
  case 0:
    m_ImageMap->SetLevel( level );
    m_ImageMap->SetWindow( width );
    break;
  case 1:
    m_LUT = vtkWindowLevelLookupTable::New();
    m_LUT->SetLevel( level );
    m_LUT->SetWindow( width );
    m_LUT->Build();
    m_Mapper->SetLookupTable( m_LUT );
    m_Mapper->Update();
    break;
  case 2:
    m_LUT = vtkWindowLevelLookupTable::New();
    m_LUT->SetLevel( level );
    m_LUT->SetWindow( width );
    m_LUT->Build();
    m_Texture->SetLookupTable( m_LUT );
    break;
  }  
}

void
ImageSliceViewer
::SelectPoint( int x, int y )
{
  if (!this->GetInput()) 
    {
    return;     // return, if no image is loaded yet.
    }

  m_DragPoint[0] = x;
  m_DragPoint[1] = y;
 
  // Invert the y coordinate (vtk uses opposite y as FLTK)
  int* winsize = m_RenderWindow->GetSize();
  y = winsize[1] - y;

  // Convert display point to world point
  double wpoint[4];
  const double z = m_SliceNum / ( m_FarPlane - m_NearPlane );
  m_Renderer->SetDisplayPoint( x, y, 0 );
  m_Renderer->DisplayToWorld();
  m_Renderer->GetWorldPoint( wpoint );

  // Fix camera Z coorinate to match the current slice
  vtkFloatingPointType spacing[3]={1,1,1};
  vtkFloatingPointType origin[3] ={0,0,0};

  int dimensions[3] = { 100, 100, 100 };
  if ( this->GetInput() )
    {
    this->GetInput()->GetSpacing(spacing);
    this->GetInput()->GetOrigin(origin);
    this->GetInput()->GetDimensions(dimensions);
    }

  int idx = 0;
  switch( m_Orientation )
    {
  case Sagittal:
      {
      idx = 0;
      break;
      }
  case Coronal:
      {
      idx = 1;
      break;
      }
  case Axial:
      {
      idx = 2;
      break;
      }
    }
  double realz = m_SliceNum * spacing[idx] + origin[idx];
  wpoint[idx] = realz;

  // At this point we have 3D position in the variable wpoint
  this->SelectPoint(wpoint[0], wpoint[1], wpoint[2]);

  m_Notifier->InvokeEvent( ClickedPointEvent() );  
}

void  
ImageSliceViewer::SelectPoint( double x, double y, double z )
{
  m_SelectPoint[0] = x;
  m_SelectPoint[1] = y;
  m_SelectPoint[2] = z;
}

void 
ImageSliceViewer::GetSelectPoint(double data[3])
{
  for(int i=0; i<3; i++)
  {
    data[i] = m_SelectPoint[i];
  }
}

unsigned long 
ImageSliceViewer::AddObserver( const itk::EventObject & event, itk::Command * command)
{
  return m_Notifier->AddObserver( event, command );
}

void ImageSliceViewer::SelectRightPoint(int x, int y)
{

}

}


