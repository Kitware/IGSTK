#include "VolumeViewer.h"
#include "vtkInteractorStyleImage.h"

namespace ISIS
{


VolumeViewer
::VolumeViewer()
{
  // Create the renderer, render window, and interactor

  m_Renderer = vtkRenderer::New();

  m_RenderWindow = vtkRenderWindow::New();
    m_RenderWindow->AddRenderer(m_Renderer);
    m_RenderWindow->SetDesiredUpdateRate( 25 );

  // Create transfer functions for mapping scalar value to opacity
  opacityTransferFunction = vtkPiecewiseFunction::New();
    opacityTransferFunction->AddPoint( 0, 0.0 );
    opacityTransferFunction->AddPoint( 79, 0.1 );
    opacityTransferFunction->AddPoint( 80, 0.8 );
    opacityTransferFunction->AddPoint( 110, 0.8 );
    opacityTransferFunction->AddPoint( 112, 0.3 );
    opacityTransferFunction->AddPoint( 254, 0.3 );
    opacityTransferFunction->AddPoint( 255, 0.8 );

  // Create transfer functions for mapping scalar value to grey scale 
//  colorTransferFunction = vtkPiecewiseFunction::New();
//    colorTransferFunction->AddSegment(0, 0.0, 255, 1.0);  
	

  // Create transfer functions for mapping scalar value to color 
  colorTransferFunction = vtkColorTransferFunction::New();
    colorTransferFunction->AddRGBPoint(  0.0, 0.0, 0.0, 0.0);  
    colorTransferFunction->AddRGBPoint( 64.0, 1.0, 0.0, 0.0);  
    colorTransferFunction->AddRGBPoint(128.0, 0.0, 0.0, 1.0);  
    colorTransferFunction->AddRGBPoint(192.0, 0.0, 1.0, 0.0);  
    colorTransferFunction->AddRGBPoint(255.0, 0.0, 0.2, 0.0);  
 
  // Create properties, mappers, volume actors, and ray cast function
  volumeProperty = vtkVolumeProperty::New();
    volumeProperty->SetColor( colorTransferFunction );
    volumeProperty->SetScalarOpacity( opacityTransferFunction );
//    volumeProperty->SetInterpolationTypeToLinear();
//    volumeProperty->ShadeOn();
/*
  // 1. Create a ray function - this is a iso surface function
  isoFunction = vtkVolumeRayCastIsosurfaceFunction::New();
    isoFunction->SetIsoValue(25);
    

  // Create the volume mapper and set the ray function and scalar input
  volumeMapper = vtkVolumeTextureMapper2D::New();
*/

  // 2. Create a ray function - this is a compositing ray function
  compositeFunction = vtkVolumeRayCastCompositeFunction::New();

  // Create the volume mapper and set the ray function and scalar input
  volumeMapper = vtkVolumeRayCastMapper::New();
    volumeMapper->SetVolumeRayCastFunction(compositeFunction);
/*
    // 3. Create a ray function - this is a maximum intensity projection function
  // Create mip ray functions
  mipFunction = vtkVolumeRayCastMIPFunction::New();
  //mipFunction->SetMaximizeMethodToScalarValue();
  mipFunction->SetMaximizeMethodToOpacity();

  // Create the volume mapper and set the ray function and scalar input
  volumeMapper = vtkVolumeRayCastMapper::New();
    volumeMapper->SetVolumeRayCastFunction(mipFunction);
*/
	
  // Create the volume and set the mapper and property
  volume = vtkVolume::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);

  // Add this volume to the renderer and get a closer look
  m_Renderer->AddProp(volume);
  m_Renderer->GetActiveCamera()->Azimuth(50.0);
  m_Renderer->GetActiveCamera()->Dolly(1.60);
  m_Renderer->ResetCameraClippingRange();

  m_Renderer->GetActiveCamera()->Zoom(2.0); 

  m_Renderer->GetActiveCamera()->SetViewUp (0,  0,  1 ); 

  m_Renderer->SetBackground( 0, 0, 0 );
}

VolumeViewer
::~VolumeViewer()
{
  // Clean up
  m_Renderer->Delete();
  m_RenderWindow->Delete();
  opacityTransferFunction->Delete();
  colorTransferFunction->Delete();
  volumeProperty->Delete();
//  isoFunction->Delete();
  volumeMapper->Delete();
  volume->Delete();
}

void 
VolumeViewer
::SetInput( vtkImageData * image )
{
    volumeMapper->SetInput(image);

    
  float spacing[3];
  float origin[3];
  int   dimensions[3];
  image->GetSpacing(spacing);
  image->GetOrigin(origin);
  image->GetDimensions(dimensions);
  float focalPoint[3];
  for ( unsigned int cc = 0; cc < 3; cc++)
    {
    focalPoint[cc] = origin[cc] + ( spacing[cc] * dimensions[cc] ) / 2.0;
     }
  m_Renderer->GetActiveCamera()->SetFocalPoint ( focalPoint );
  focalPoint[1] += 1000.0;
  m_Renderer->GetActiveCamera()->SetPosition ( focalPoint );
}

void 
VolumeViewer
::SetInteractor( vtkRenderWindowInteractor * interactor )
{
  m_RenderWindow->SetInteractor( interactor );
//  vtkInteractorStyleImage * interactorStyle = vtkInteractorStyleImage::New();
//  interactor->SetInteractorStyle( interactorStyle );
//  interactorStyle->Delete();

  interactor->SetDesiredUpdateRate(5.0);
  interactor->SetStillUpdateRate(0.001);
  interactor->Initialize();
}

void 
VolumeViewer
::Render( void )
{
  m_RenderWindow->Render();
}
    




}
