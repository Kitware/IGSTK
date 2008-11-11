/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageResliceSpatialObjectRepresentation.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkImageResliceSpatialObjectRepresentation_txx
#define __igstkImageResliceSpatialObjectRepresentation_txx

#include "igstkImageResliceSpatialObjectRepresentation.h"

#include "igstkEvents.h"

#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkLookupTable.h>
#include <vtkImageReslice.h>
#include <vtkTexture.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkPlaneSource.h>
#include <vtkCamera.h>
#include <vtkMath.h>

namespace igstk
{

/** Constructor */

template < class TImageSpatialObject >
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::ImageResliceSpatialObjectRepresentation():m_StateMachine(this)
{
  // We create the image spatial object
  m_ImageSpatialObject = NULL;

  this->RequestSetSpatialObject( m_ImageSpatialObject );

  // Create classes for displaying images
  m_ImageActor = vtkActor::New();
  this->AddActor( m_ImageActor );
 
  m_ImageData  = NULL;

  m_ImageReslice = vtkImageReslice::New();
  m_ImageReslice->SetOutputDimensionality(2);
  m_ImageReslice->SetOutputSpacing( 1, 1, 1 );
  m_ImageReslice->AutoCropOutputOn();  
  m_ImageReslice->SetOptimization( 1 );
  m_ImageReslice->SetInterpolationModeToNearestNeighbor();

  m_TextureMapper = vtkPolyDataMapper::New();
  m_Texture = vtkTexture::New();

  m_PlaneSource = vtkPlaneSource::New();

  m_LUT        = vtkLookupTable::New();
  m_ResliceTransform = vtkTransform::New();
  m_ResliceAxes = vtkMatrix4x4::New();
  m_Camera = vtkCamera::New();
  // Set default values for window and level
  m_Level = 52;
  m_Window = 542;
  m_CameraDistance = 1000;

  m_LUT->SetTableRange ( (m_Level - m_Window/2.0), (m_Level + m_Window/2.0) );
  m_LUT->SetSaturationRange( 0.0, 0.0 );
  m_LUT->SetHueRange( 0.0, 0.0 );
  m_LUT->SetValueRange( 0.0, 1.0 );  
  m_LUT->SetRampToLinear();
  m_LUT->Build();

  m_ImageReslice->SetBackgroundLevel( m_Level - m_Window/2.0 );

  m_Texture->MapColorScalarsThroughLookupTableOn();
  m_Texture->InterpolateOn();
  m_Texture->SetLookupTable( m_LUT );
  m_Texture->RepeatOff();

  m_VTKImageObserver = VTKImageObserver::New();
  m_VTKPlaneObserver = VTKPlaneObserver::New();
  m_ImageTransformObserver = ImageTransformObserver::New();

  igstkAddInputMacro( ValidImageSpatialObject );
  igstkAddInputMacro( InValidImageSpatialObject );
  igstkAddInputMacro( ValidReslicePlaneSpatialObject );
  igstkAddInputMacro( InValidReslicePlaneSpatialObject  );
  igstkAddInputMacro( ConnectVTKPipeline  );

  igstkAddStateMacro( Initial  );
  igstkAddStateMacro( ImageSpatialObjectSet );
  igstkAddStateMacro( ReslicePlaneSpatialObjectSet );

  //From Initial state
  igstkAddTransitionMacro( Initial, ValidImageSpatialObject, 
                           ImageSpatialObjectSet,  SetImageSpatialObject );

  igstkAddTransitionMacro( Initial, InValidImageSpatialObject, 
                           Initial, ReportInvalidImageSpatialObject );

  //From ImageSpatialObjectSet
  igstkAddTransitionMacro( ImageSpatialObjectSet, ValidReslicePlaneSpatialObject, 
                           ReslicePlaneSpatialObjectSet, SetReslicePlaneSpatialObject);

  igstkAddTransitionMacro( ImageSpatialObjectSet, InValidReslicePlaneSpatialObject, 
                           ImageSpatialObjectSet, ReportInvalidReslicePlaneSpatialObject );
 
  //From ReslicePlaneSpatialObjectSet
  igstkAddTransitionMacro( ReslicePlaneSpatialObjectSet, ConnectVTKPipeline,
                           ReslicePlaneSpatialObjectSet, ConnectVTKPipeline );
  
  igstkSetInitialStateMacro( Initial );
 
  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
template < class TImageSpatialObject >
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::~ImageResliceSpatialObjectRepresentation()  
{

  // This deletes also the m_ImageActor
  this->DeleteActors();

  if( m_Camera )
    {
    m_Camera->Delete();
    m_Camera = NULL;
    }

  if( m_ResliceAxes )
    {
    m_ResliceAxes->Delete();
    m_ResliceAxes = NULL;
    }

  if( m_ResliceTransform )
    {
    m_ResliceTransform->Delete();
    m_ResliceTransform = NULL;
    }
    
  if( m_LUT )
    {
    m_LUT->Delete();
    m_LUT = NULL;
    }
 
  if( m_ImageReslice )
    {
    m_ImageReslice->Delete();
    m_ImageReslice = NULL;
    }

  if( m_TextureMapper )
    {
    m_TextureMapper->Delete();
    m_TextureMapper = NULL;
    }

  if ( m_Texture )
    {
    m_Texture->Delete();
    m_Texture = NULL;
    }

  if ( m_PlaneSource )
    {
    m_PlaneSource->Delete();
    m_PlaneSource = NULL;
    }
}

/** Overloaded DeleteActor function */
template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::DeleteActors( )
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceSpatialObjectRepresentation\
                        ::DeleteActors called...\n");
   
  this->Superclass::DeleteActors();
  
  m_ImageActor = NULL;

}
 
/** Set the Image Spatial Object 
* // to do: get rid of the image spatial object. The required information
* // should be obtained from the ReslicePlaneSpatialObject
*/
template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetImageSpatialObject( const ImageSpatialObjectType * image )
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceSpatialObjectRepresentation\
                        ::RequestSetImageSpatialObject called...\n");
  
  m_ImageSpatialObjectToAdd = image;

  if( !m_ImageSpatialObjectToAdd )
    {
    m_StateMachine.PushInput( m_InValidImageSpatialObjectInput );
    }
  else 
    {
    m_StateMachine.PushInput( m_ValidImageSpatialObjectInput );
    }
  
  m_StateMachine.ProcessInputs();
}


template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::ReportInvalidImageSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageResliceSpatialObjectRepresentation\
                       ::ReportInvalidImageSpatialObjectProcessing called...\n"); 
}

template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::ReportInvalidReslicePlaneSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageResliceSpatialObjectRepresentation\
                       ::ReportInvalidReslicePlaneSpatialObjectProcessing called...\n");
}

template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetReslicePlaneSpatialObject( const ReslicerPlaneType *
reslicePlaneSpatialObject )
{  
  igstkLogMacro( DEBUG,"igstk::ImageResliceSpatialObjectRepresentation\
                       ::RequestSetReslicePlaneSpatialObject called...\n");

  m_ReslicePlaneSpatialObjectToBeSet = const_cast< ReslicerPlaneType* >(reslicePlaneSpatialObject);

  if( !m_ReslicePlaneSpatialObjectToBeSet )
    {
    m_StateMachine.PushInput( m_InValidReslicePlaneSpatialObjectInput );
    }
  else
    {
    m_StateMachine.PushInput( m_ValidReslicePlaneSpatialObjectInput );
    }

  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::SetReslicePlaneSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageResliceSpatialObjectRepresentation\
                       ::SetReslicePlaneSpatialObjectProcessing called...\n");

  m_ReslicePlaneSpatialObject = m_ReslicePlaneSpatialObjectToBeSet;

  m_ReslicePlaneSpatialObject->RequestComputeReslicingPlane();

  unsigned int obsID = 
      m_ReslicePlaneSpatialObject->AddObserver( VTKPlaneModifiedEvent(),
                                      m_VTKPlaneObserver );
  
  m_VTKPlaneObserver->Reset();

  m_ReslicePlaneSpatialObject->RequestGetVTKPlane();
  
  if( m_VTKPlaneObserver->GotVTKPlane() )
  {
    this->SetPlane( m_VTKPlaneObserver->GetVTKPlane() );
  }

  m_ReslicePlaneSpatialObject->RemoveObserver( obsID );
}

/** Verify time stamp of the attached tool*/
template < class TImageSpatialObject >
bool
ImageResliceSpatialObjectRepresentation < TImageSpatialObject >
::VerifyTimeStamp( ) const
{
  igstkLogMacro( DEBUG, 
    "igstk::ImageResliceSpatialObjectRepresentation::VerifyTimeStamp called...\n");

  if( this->m_ReslicePlaneSpatialObject.IsNull() )
    {
    return false;
    }

  /* if a tool spatial object is driving the reslicing, compare the 
     tool spatial object transform with the view render time*/
  if( this->m_ReslicePlaneSpatialObject->IsToolSpatialObjectSet())
    {
    if( this->GetRenderTimeStamp().GetExpirationTime() <
      this->m_ReslicePlaneSpatialObject->GetToolTransform().GetStartTime() ||
      this->GetRenderTimeStamp().GetStartTime() >
      this->m_ReslicePlaneSpatialObject->GetToolTransform().GetExpirationTime() )
      {
        // fixme
        double diff = 
          this->GetRenderTimeStamp().GetStartTime() - this->m_ReslicePlaneSpatialObject->GetToolTransform().GetExpirationTime();

        if (diff > 250 )
        {
          //std::cout << diff << std::endl;
          return false;
        }
        else
          return true;
      }
    else
      {
      return true;
      }
    }
  else
    {
    return true;
    }
}

template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::SetWindowLevel( double window, double level )
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceSpatialObjectRepresentation\
                        ::SetWindowLevel called...\n");

  m_Window = window;
  m_Level = level;

  m_LUT->SetTableRange ( (m_Level - m_Window/2.0), (m_Level + m_Window/2.0) );
}


template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::SetVisibility( bool visible )
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceSpatialObjectRepresentation\
                        ::SetVisibility called...\n");

  m_ImageActor->SetVisibility(visible);
}

/** Null Operation for a State Machine Transition */
template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::NoProcessing()
{
}

/** Set nhe Image Spatial Object */
template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::SetImageSpatialObjectProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceSpatialObjectRepresentation\
                         ::SetImageSpatialObjectProcessing called...\n");

  m_ImageSpatialObject = m_ImageSpatialObjectToAdd;

  this->RequestSetSpatialObject( m_ImageSpatialObject );
  
  unsigned int obsId = m_ImageSpatialObject->AddObserver( VTKImageModifiedEvent(), 
                                      m_VTKImageObserver );

  m_VTKImageObserver->Reset();

  m_ImageSpatialObject->RequestGetVTKImage();
  
  if( m_VTKImageObserver->GotVTKImage() )
    {
        this->SetImage( m_VTKImageObserver->GetVTKImage() );
        if( m_ImageData )
        {
            m_ImageData->Update();
            m_ImageData->GetSpacing( m_ImageSpacing );           
            m_ImageReslice->SetInput ( m_ImageData );
        }
    }

  m_ImageSpatialObject->RemoveObserver( obsId );
/*
  this->m_ImageTransformObserver->Reset();

  this->m_ImageSpatialObject->RequestGetImageTransform();

  if( this->m_ImageTransformObserver->GotImageTransform() ) 
    {
      const CoordinateSystemTransformToResult transformCarrier =
        this->m_ImageTransformObserver->GetImageTransform();

      igstk::Transform imageTransform = transformCarrier.GetTransform();

      // Image Actor takes care of the image origin position internally.
      this->m_ImageActor->SetPosition(0,0,0);

      vtkMatrix4x4 * imageTransformMatrix = vtkMatrix4x4::New();

      imageTransform.ExportTransform( *imageTransformMatrix );

      this->m_ImageActor->SetUserMatrix( imageTransformMatrix );
      imageTransformMatrix->Delete();
    }
*/
}


/** Print Self function */
template < class TImageSpatialObject >
void
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Plane origin";
  os << indent << this->m_PlaneSource->GetOrigin() << std::endl;
  os << indent << "Point 1 on the plane";
  os << indent << this->m_PlaneSource->GetPoint1() << std::endl;
  os << indent << "Point 2 on the plane";
  os << indent << this->m_PlaneSource->GetPoint2() << std::endl;
  os << indent << "Center of the plane";
  os << indent << this->m_PlaneSource->GetCenter() << std::endl;
}


/** Update the visual representation in response to changes in the geometric
 * object */
template < class TImageSpatialObject >
void
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceSpatialObjectRepresentation::\
                         UpdateRepresentationProcessing called...\n");

  m_ReslicePlaneSpatialObject->RequestComputeReslicingPlane();

  unsigned int obsID = 
      m_ReslicePlaneSpatialObject->AddObserver( VTKPlaneModifiedEvent(),
                                      m_VTKPlaneObserver );
  
  m_VTKPlaneObserver->Reset();

  m_ReslicePlaneSpatialObject->RequestGetVTKPlane();
  
  if( !m_VTKPlaneObserver->GotVTKPlane() )
    return;

  this->SetPlane( m_VTKPlaneObserver->GetVTKPlane() );

   m_ReslicePlaneSpatialObject->RemoveObserver( obsID );

  if( !m_PlaneSource )
    return;

  double planeAxis1[3];
  double planeAxis2[3];

  this->GetVector1(planeAxis1);
  this->GetVector2(planeAxis2);

  // The x,y dimensions of the plane
  //
  double planeSizeX = vtkMath::Normalize(planeAxis1);
  double planeSizeY = vtkMath::Normalize(planeAxis2);

  double normal[3];
  m_PlaneSource->GetNormal(normal);

  // Generate the slicing matrix
  //
  m_ResliceAxes->Identity();

  for (int i = 0; i < 3; i++ )
  {
      m_ResliceAxes->SetElement(0,i,planeAxis1[i]);
      m_ResliceAxes->SetElement(1,i,planeAxis2[i]);
      m_ResliceAxes->SetElement(2,i,normal[i]);
  }           

  double planeOrigin[4];
  m_PlaneSource->GetOrigin(planeOrigin);

  planeOrigin[3] = 1.0;
  double originXYZW[4];
  m_ResliceAxes->MultiplyPoint(planeOrigin, originXYZW);

  m_ResliceAxes->Transpose();
  double neworiginXYZW[4];
  m_ResliceAxes->MultiplyPoint(originXYZW, neworiginXYZW);

  m_ResliceAxes->SetElement(0,3,neworiginXYZW[0]);
  m_ResliceAxes->SetElement(1,3,neworiginXYZW[1]);
  m_ResliceAxes->SetElement(2,3,neworiginXYZW[2]);

  m_ImageReslice->SetResliceAxes(m_ResliceAxes);

 // m_ImageActor->SetUserMatrix(m_ResliceAxes);

  double spacingX = fabs(planeAxis1[0]*m_ImageSpacing[0])+\
                    fabs(planeAxis1[1]*m_ImageSpacing[1])+\
                    fabs(planeAxis1[2]*m_ImageSpacing[2]);

  double spacingY = fabs(planeAxis2[0]*m_ImageSpacing[0])+\
                    fabs(planeAxis2[1]*m_ImageSpacing[1])+\
                    fabs(planeAxis2[2]*m_ImageSpacing[2]);

  // Pad extent up to a power of two for efficient texture mapping

  // make sure we're working with valid values
  double realExtentX = ( spacingX == 0 ) ? 0 : planeSizeX / spacingX;

  int extentX;
  // Sanity check the input data:
  // * if realExtentX is too large, extentX will wrap
  // * if spacingX is 0, things will blow up.
  // * if realExtentX is naturally 0 or < 0, the padding will yield an
  //   extentX of 1, which is also not desirable if the input data is invalid.
  if (realExtentX > (VTK_INT_MAX >> 1) || realExtentX < 1)
  {
      std::cout << "Invalid X extent.  Perhaps the input data is empty?" << std::endl;
      extentX = 0;
  }
  else
  {
      extentX = 1;
      while (extentX < realExtentX)
      {
          extentX = extentX << 1;
      }
  }

  // make sure extentY doesn't wrap during padding
  double realExtentY = ( spacingY == 0 ) ? 0 : planeSizeY / spacingY;

  int extentY;
  if (realExtentY > (VTK_INT_MAX >> 1) || realExtentY < 1)
  {
      std::cout << "Invalid Y extent.  Perhaps the input data is empty?" << std::endl;
      extentY = 0;
  }
  else
  {
      extentY = 1;
      while (extentY < realExtentY)
      {
      extentY = extentY << 1;
      }
  }

  double outputSpacingX = planeSizeX/extentX;
  double outputSpacingY = planeSizeY/extentY;

  m_ImageReslice->SetOutputSpacing(outputSpacingX, outputSpacingY, 1);
  m_ImageReslice->SetOutputOrigin(0.5*outputSpacingX, 0.5*outputSpacingY, 0);
  m_ImageReslice->SetOutputExtent(0, extentX-1, 0, extentY-1, 0, 0);

  //Setting up the camera position

  double planeCenter[3];
  m_PlaneSource->GetCenter(planeCenter);

  double focalPoint[3];
  double position[3];
  for ( int i = 0; i<3; i++ )
  {
  focalPoint[i] = planeCenter[i];
  position[i] = planeCenter[i];
  }

  for ( int i = 0; i<3; i++ )
  {
  position[i] -= m_CameraDistance * normal[i];
  }

  m_Camera->SetViewUp( -m_ResliceAxes->GetElement(0,1), 
                       -m_ResliceAxes->GetElement(1,1), 
                       -m_ResliceAxes->GetElement(2,1) );

  m_Camera->SetFocalPoint( focalPoint );
  //camera->SetParallelScale( 0.8 );
  //camera->Zoom( 2 );
  m_Camera->SetPosition( position );
   

  VTKCameraModifiedEvent event;
  event.Set( m_Camera );
  this->InvokeEvent( event );  
}


/** Create the vtk Actors */
template < class TImageSpatialObject >
void
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::CreateActors()
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceSpatialObjectRepresentation\
                        ::CreateActors called...\n");

  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  m_ImageActor = vtkActor::New();

  this->AddActor( m_ImageActor );  

  igstkPushInputMacro( ConnectVTKPipeline );
  m_StateMachine.ProcessInputs(); 

}
 
template < class TImageSpatialObject >
void
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::SetImage( const vtkImageData * image )
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceSpatialObjectRepresentation\
                        ::SetImage called...\n");

  // This const_cast<> is needed here due to 
  // the lack of const-correctness in VTK 
  m_ImageData = const_cast< vtkImageData *>( image );
}

template < class TImageSpatialObject >
void
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::SetPlane( const vtkPlaneSource * plane )
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceSpatialObjectRepresentation\
                        ::SetPlane called...\n");

  // This const_cast<> is needed here due to 
  // the lack of const-correctness in VTK 
  m_PlaneSource = const_cast< vtkPlaneSource *>( plane );
}

template < class TImageSpatialObject >
void
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::ConnectVTKPipelineProcessing() 
{
  m_ImageReslice->SetInput( m_ImageData );
  m_Texture->SetInput( m_ImageReslice->GetOutput() );
  m_TextureMapper->SetInput( m_PlaneSource->GetOutput() );
  m_ImageActor->SetTexture( m_Texture );
  m_ImageActor->SetMapper( m_TextureMapper );
}

template < class TImageSpatialObject >
void
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::GetVector1(double v1[3])
{
  double* p1 = m_PlaneSource->GetPoint1();
  double* o =  m_PlaneSource->GetOrigin();
  v1[0] = p1[0] - o[0];
  v1[1] = p1[1] - o[1];
  v1[2] = p1[2] - o[2];
}

template < class TImageSpatialObject >
void
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::GetVector2(double v2[3])
{
  double* p2 = m_PlaneSource->GetPoint2();
  double* o =  m_PlaneSource->GetOrigin();
  v2[0] = p2[0] - o[0];
  v2[1] = p2[1] - o[1];
  v2[2] = p2[2] - o[2];
}

/** Create a copy of the current object representation */
template < class TImageSpatialObject >
typename ImageResliceSpatialObjectRepresentation< TImageSpatialObject >::Pointer
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::Copy() const
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceSpatialObjectRepresentation\
                        ::Copy called...\n");

  Pointer newOR = ImageResliceSpatialObjectRepresentation::New();
  newOR->SetColor( this->GetRed(),this->GetGreen(),this->GetBlue() );
  newOR->SetOpacity( this->GetOpacity() );
  newOR->RequestSetImageSpatialObject( m_ImageSpatialObject );
  newOR->RequestSetReslicePlaneSpatialObject( m_ReslicePlaneSpatialObject );

  return newOR;
}

} // end namespace igstk

#endif
