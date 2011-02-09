/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageResliceObjectRepresentation.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkImageResliceObjectRepresentation_txx
#define __igstkImageResliceObjectRepresentation_txx

#include "igstkImageResliceObjectRepresentation.h"

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
#include <vtkPlane.h>
#include <vtkPolyData.h>
#include <vtkOutlineFilter.h>
#include <vtkCamera.h>
#include <vtkMath.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>
#include <vtkImageMapToColors.h>
#include <vtkCutter.h>

namespace igstk
{

/** Constructor */

template < class TImageSpatialObject >
ImageResliceObjectRepresentation< TImageSpatialObject >
::ImageResliceObjectRepresentation():m_StateMachine(this)
{
  // We create the image spatial object
  m_ImageSpatialObject = NULL;
  this->RequestSetSpatialObject( m_ImageSpatialObject );

  m_PlaneProperty = NULL;
  m_PlaneSource = NULL;
  m_Plane = NULL;
  m_ResliceAxes = NULL;
  m_ImageReslicer = NULL;
  m_LookupTable = NULL;
  m_ColorMap = NULL;
  m_Texture = NULL;
  m_Box = NULL;
  m_Cutter = NULL;
  m_ImageData = NULL;

  //m_Edges->Delete();
  //m_EdgesTuber->Delete();

  // Create classes for displaying images
  m_ImageActor = vtkActor::New();

  m_PlaneProperty = vtkProperty::New();
  m_PlaneProperty->SetAmbient(1.0);
  m_PlaneProperty->SetAmbientColor(1.0,1.0,1.0);
  m_PlaneProperty->SetOpacity(1);

 // m_ImageActor->SetProperty( m_PlaneProperty );

  this->AddActor( m_ImageActor );
 
  // for dubugging purpose
  //m_SphereActor = vtkActor::New();
  //this->AddActor( m_SphereActor );

  // for dubugging purpose
  //m_EdgesActor = vtkActor::New();
  //this->AddActor( m_EdgesActor );

  m_RestrictPlaneToVolume    = 0;  
  m_TextureInterpolate       = 0;
  m_ResliceInterpolate       = VTK_NEAREST_RESLICE;

  m_FrameColor[0] = 1;
  m_FrameColor[1] = 1;
  m_FrameColor[2] = 0;

  m_PlaneSource = vtkPlaneSource::New();
  m_PlaneSource->SetXResolution(1);
  m_PlaneSource->SetYResolution(1);

  m_ColorMap = vtkImageMapToColors::New();

  m_ImageReslicer = vtkImageReslice::New();
  m_ImageReslicer->SetBackgroundLevel( 0 );
  m_ImageReslicer->TransformInputSamplingOff();
  m_ImageReslicer->SetOutputDimensionality(2);

  m_ResliceAxes        = vtkMatrix4x4::New();
  m_Texture            = vtkTexture::New();

  m_LookupTable        = vtkLookupTable::New();
  m_LookupTable->SetNumberOfColors( 256);
  m_LookupTable->SetHueRange( 0, 0);
  m_LookupTable->SetSaturationRange( 0, 0);
  m_LookupTable->SetValueRange( 0 ,1);
  m_LookupTable->SetAlphaRange( 1, 1);
  m_LookupTable->Build();

  // Define some default point coordinates
  //
  double bounds[6];
  bounds[0] = -0.5;
  bounds[1] =  0.5;
  bounds[2] = -0.5;
  bounds[3] =  0.5;
  bounds[4] = -0.5;
  bounds[5] =  0.5;

  double center[3];
  center[0] = (bounds[0] + bounds[1])/2.0;
  center[1] = (bounds[2] + bounds[3])/2.0;
  center[2] = (bounds[4] + bounds[5])/2.0;

  m_PlaneSource->SetOrigin(center[0],bounds[2],bounds[4]);
  m_PlaneSource->SetPoint1(center[0],bounds[3],bounds[4]);
  m_PlaneSource->SetPoint2(center[0],bounds[2],bounds[5]);

  this->UpdatePlane();

  m_Plane = vtkPlane::New();
  m_Plane->SetOrigin( m_PlaneSource->GetCenter() );
  m_Plane->SetNormal( m_PlaneSource->GetNormal() );

  m_Box = vtkImageData::New();
  m_Box->SetDimensions(2,2,2);

  m_Cutter = vtkCutter::New();
  m_Cutter->SetInput(m_Box);
  m_Cutter->SetCutFunction(m_Plane);

  //m_Edges->SetInput(m_Cutter->GetOutput());
  //m_EdgesTuber->SetInput(m_Edges->GetOutput());

  m_Level = 0;
  m_Window = 0;

  m_ImageReslicer->SetBackgroundLevel( m_Level - m_Window/2.0 );

  m_VTKImageObserver = VTKImageObserver::New();

  m_ReslicerPlaneCenterObserver = ReslicerPlaneCenterObserver::New();
  m_ReslicerPlaneNormalObserver = ReslicerPlaneNormalObserver::New();

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
  igstkAddTransitionMacro( ImageSpatialObjectSet,
                           ValidReslicePlaneSpatialObject,
                           ReslicePlaneSpatialObjectSet,
                           SetReslicePlaneSpatialObject);

  igstkAddTransitionMacro( ImageSpatialObjectSet,
                           InValidReslicePlaneSpatialObject,
                           ImageSpatialObjectSet,
                           ReportInvalidReslicePlaneSpatialObject );
 
  //From ReslicePlaneSpatialObjectSet
  igstkAddTransitionMacro( ReslicePlaneSpatialObjectSet, ConnectVTKPipeline,
                           ReslicePlaneSpatialObjectSet, ConnectVTKPipeline );
  
  igstkSetInitialStateMacro( Initial );
 
  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
template < class TImageSpatialObject >
ImageResliceObjectRepresentation< TImageSpatialObject >
::~ImageResliceObjectRepresentation()  
{

  this->DeleteActors();

  //m_EdgesProperty->Delete();
  if ( m_PlaneProperty != NULL )
    {
    m_PlaneProperty->Delete();
    m_PlaneProperty = NULL;
    }

  if ( m_PlaneSource != NULL )
    {
    m_PlaneSource->Delete();
    m_PlaneSource = NULL;
    }

  if ( m_Plane != NULL )
    {
    m_Plane->Delete();
    m_Plane = NULL;
    }

  if ( m_ResliceAxes != NULL )
    {
    m_ResliceAxes->Delete();
    m_ResliceAxes = NULL;
    }

  if ( m_ImageReslicer != NULL )
    {
    m_ImageReslicer->Delete();
    m_ImageReslicer = NULL;
    }

  if ( m_LookupTable )
    {
    m_LookupTable->Delete();
    m_LookupTable = NULL;
    }

  if ( m_ColorMap )
    {
    m_ColorMap->Delete();
    m_ColorMap = NULL;
    }

  if ( m_Texture )
    {
    m_Texture->Delete();
    m_Texture = NULL;
    }

  if ( m_Box )
    {
    m_Box->Delete();
    m_Box = NULL;
    }

  if ( m_Cutter )
    {
    m_Cutter->Delete();
    m_Cutter = NULL;
    }

  //m_Edges->Delete();
  //m_EdgesTuber->Delete();

  if ( m_ImageData )
    {
    m_ImageData = NULL;
    }

}


/** Overloaded DeleteActor function */
template < class TImageSpatialObject >
void 
ImageResliceObjectRepresentation< TImageSpatialObject >
::DeleteActors( )
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceObjectRepresentation\
                        ::DeleteActors called...\n");
   
  this->Superclass::DeleteActors();
  
  m_ImageActor = NULL;
  //m_SphereActor = NULL;
  //m_EdgesActor = NULL;

}
 
/** Set the Image Spatial Object 
  * // to do: get rid of the image spatial object. The required information
  * // should be obtained from the ReslicePlaneSpatialObject
  */
template < class TImageSpatialObject >
void 
ImageResliceObjectRepresentation< TImageSpatialObject >
::RequestSetImageSpatialObject( const ImageSpatialObjectType * image )
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceObjectRepresentation\
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
ImageResliceObjectRepresentation< TImageSpatialObject >
::ReportInvalidImageSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageResliceObjectRepresentation\
                      ::ReportInvalidImageSpatialObjectProcessing called...\n");
}

template < class TImageSpatialObject >
void 
ImageResliceObjectRepresentation< TImageSpatialObject >
::ReportInvalidReslicePlaneSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageResliceObjectRepresentation\
              ::ReportInvalidReslicePlaneSpatialObjectProcessing called...\n");
}

template < class TImageSpatialObject >
void 
ImageResliceObjectRepresentation< TImageSpatialObject >
::RequestSetReslicePlaneSpatialObject( const ReslicerPlaneType *
reslicePlaneSpatialObject )
{  
  igstkLogMacro( DEBUG,"igstk::ImageResliceObjectRepresentation\
                       ::RequestSetReslicePlaneSpatialObject called...\n");

  m_ReslicePlaneSpatialObjectToBeSet = 
                    const_cast< ReslicerPlaneType* >(reslicePlaneSpatialObject);

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
ImageResliceObjectRepresentation< TImageSpatialObject >
::SetReslicePlaneSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageResliceObjectRepresentation\
                       ::SetReslicePlaneSpatialObjectProcessing called...\n");

  m_ReslicePlaneSpatialObject = m_ReslicePlaneSpatialObjectToBeSet;

  m_ReslicePlaneSpatialObject->AddObserver( 
                                  ReslicerPlaneType::ReslicerPlaneCenterEvent(),
                                                m_ReslicerPlaneCenterObserver );

  m_ReslicePlaneSpatialObject->AddObserver( 
                                  ReslicerPlaneType::ReslicerPlaneNormalEvent(),
                                                m_ReslicerPlaneNormalObserver );

  m_ReslicePlaneSpatialObject->RequestComputeReslicingPlane();
}

/** Verify time stamp of the attached tool*/
template < class TImageSpatialObject >
bool
ImageResliceObjectRepresentation < TImageSpatialObject >
::VerifyTimeStamp( ) const
{
  igstkLogMacro( DEBUG, 
    "igstk::ImageResliceObjectRepresentation::VerifyTimeStamp called...\n");

  if( this->m_ReslicePlaneSpatialObject.IsNull() )
  return false;


  if( !this->m_ReslicePlaneSpatialObject->IsToolSpatialObjectSet() )
    return true;

  /* if a tool spatial object is driving the reslicing, compare the 
   *  tool spatial object transform with the view render time
   */
  if( this->GetRenderTimeStamp().GetExpirationTime() <
    this->m_ReslicePlaneSpatialObject->GetToolTransform().GetStartTime() ||
    this->GetRenderTimeStamp().GetStartTime() >
    this->m_ReslicePlaneSpatialObject->GetToolTransform().GetExpirationTime() )
    {
    // fixme
    double diff = 
      this->GetRenderTimeStamp().GetStartTime() - 
      m_ReslicePlaneSpatialObject->GetToolTransform().GetExpirationTime();

    if (diff > 450 )
      {
      //std::cout << diff << std::endl;
      return false;
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
ImageResliceObjectRepresentation< TImageSpatialObject >
::SetWindowLevel( double window, double level )
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceObjectRepresentation\
                        ::SetWindowLevel called...\n");

  m_Window = window;
  m_Level = level;

  m_LookupTable->SetTableRange ( (m_Level - m_Window/2.0), 
                                 (m_Level + m_Window/2.0) );
  m_LookupTable->Build();
}


/** Null Operation for a State Machine Transition */
template < class TImageSpatialObject >
void 
ImageResliceObjectRepresentation< TImageSpatialObject >
::NoProcessing()
{
}

/** Set nhe Image Spatial Object */
template < class TImageSpatialObject >
void 
ImageResliceObjectRepresentation< TImageSpatialObject >
::SetImageSpatialObjectProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceObjectRepresentation\
                         ::SetImageSpatialObjectProcessing called...\n");

  m_ImageSpatialObject = m_ImageSpatialObjectToAdd;

  this->RequestSetSpatialObject( m_ImageSpatialObject );
  
  unsigned int obsId = m_ImageSpatialObject->AddObserver( 
                                                        VTKImageModifiedEvent(),
                                                           m_VTKImageObserver );

  m_VTKImageObserver->Reset();

  m_ImageSpatialObject->RequestGetVTKImage();
  
  if( !m_VTKImageObserver->GotVTKImage() )
    {
    return;
    }

  this->SetImage( m_VTKImageObserver->GetVTKImage() );

  if( !m_ImageData )
    {
    return;
    }

  m_ImageSpatialObject->RemoveObserver( obsId );

  m_ImageData->UpdateInformation();
  
  double range[2];
  m_ImageData->GetScalarRange(range);

  m_LookupTable->SetTableRange(range[0],range[1]);
  m_LookupTable->Build();

  m_Window = range[1] - range[0];
  m_Level = 0.5*(range[0] + range[1]);
  
  if( fabs( m_Window ) < 0.001 )
    {
    m_Window = 0.001 * ( m_Window < 0.0 ? -1 : 1 );
    }

  if( fabs( m_Level ) < 0.001 )
    {
    m_Level = 0.001 * ( m_Level < 0.0 ? -1 : 1 );
    }

  this->SetWindowLevel( m_Window, m_Level );

  m_ColorMap->SetLookupTable(m_LookupTable);
  m_ColorMap->SetInput(m_ImageData);

  this->SetResliceInterpolate(m_ResliceInterpolate);

  m_ImageReslicer->SetInput(m_ColorMap->GetOutput());

  m_Texture->SetInput(m_ImageReslicer->GetOutput());
  m_Texture->SetInterpolate(m_TextureInterpolate);
 
  m_ImageData->GetWholeExtent(m_ImageExtent);

  m_ImageData->GetOrigin(m_ImageOrigin);

  m_ImageData->GetSpacing(m_ImageSpacing);

  // Prevent obscuring voxels by offsetting the plane geometry
  m_xbounds[0]= m_ImageOrigin[0] + m_ImageSpacing[0] * (m_ImageExtent[0] - 0.5);
  m_xbounds[1]= m_ImageOrigin[0] + m_ImageSpacing[0] * (m_ImageExtent[1] + 0.5);
  m_ybounds[0]= m_ImageOrigin[1] + m_ImageSpacing[1] * (m_ImageExtent[2] - 0.5);
  m_ybounds[1]= m_ImageOrigin[1] + m_ImageSpacing[1] * (m_ImageExtent[3] + 0.5);
  m_zbounds[0]= m_ImageOrigin[2] + m_ImageSpacing[2] * (m_ImageExtent[4] - 0.5);
  m_zbounds[1]= m_ImageOrigin[2] + m_ImageSpacing[2] * (m_ImageExtent[5] + 0.5);

  if ( m_ImageSpacing[0] < 0.0 )
    {
    double t = m_xbounds[0];
    m_xbounds[0] = m_xbounds[1];
    m_xbounds[1] = t;
    }

  if ( m_ImageSpacing[1] < 0.0 )
    {
    double t = m_ybounds[0];
    m_ybounds[0] = m_ybounds[1];
    m_ybounds[1] = t;
    }

  if ( m_ImageSpacing[2] < 0.0 )
    {
    double t = m_zbounds[0];
    m_zbounds[0] = m_zbounds[1];
    m_zbounds[1] = t;
    }

  m_Box->SetOrigin(m_xbounds[0],m_ybounds[0],m_zbounds[0]);
  m_Box->SetSpacing((m_xbounds[1]-m_xbounds[0]),
                    (m_ybounds[1]-m_ybounds[0]),
                    (m_zbounds[1]-m_zbounds[0]));

  m_Cutter->SetInput(m_Box);

  m_PlaneSource->SetOrigin(m_xbounds[0],m_ybounds[0],m_zbounds[0]);
  m_PlaneSource->SetPoint1(m_xbounds[1],m_ybounds[0],m_zbounds[0]);
  m_PlaneSource->SetPoint2(m_xbounds[0],m_ybounds[1],m_zbounds[0]);

  m_Plane->SetOrigin( m_PlaneSource->GetCenter() );
  m_Plane->SetNormal( m_PlaneSource->GetNormal() );

  this->UpdatePlane();
}


/** Print Self function */
template < class TImageSpatialObject >
void
ImageResliceObjectRepresentation< TImageSpatialObject >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Plane origin";
  os << indent << m_PlaneSource->GetOrigin() << std::endl;
  os << indent << "Point 1 on the plane";
  os << indent << m_PlaneSource->GetPoint1() << std::endl;
  os << indent << "Point 2 on the plane";
  os << indent << m_PlaneSource->GetPoint2() << std::endl;
  os << indent << "Center of the plane";
  os << indent << m_PlaneSource->GetCenter() << std::endl;
}


/** Update the visual representation in response to changes in the geometric
 * object */
template < class TImageSpatialObject >
void
ImageResliceObjectRepresentation< TImageSpatialObject >
::UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceObjectRepresentation::\
                         UpdateRepresentationProcessing called...\n");

  m_ReslicePlaneSpatialObject->RequestComputeReslicingPlane();

  m_ReslicerPlaneCenterObserver->Reset();
  m_ReslicerPlaneNormalObserver->Reset();  

  m_ReslicePlaneSpatialObject->RequestGetReslicingPlaneParameters();

  ReslicerPlaneType::VectorType reslicerPlaneCenter;

  if( m_ReslicerPlaneCenterObserver->GotReslicerPlaneCenter() )
    {
      reslicerPlaneCenter = 
                        m_ReslicerPlaneCenterObserver->GetReslicerPlaneCenter();
      m_Plane->SetOrigin( reslicerPlaneCenter[0], 
                          reslicerPlaneCenter[1], 
                          reslicerPlaneCenter[2] );
    }
  else
    return;

  ReslicerPlaneType::VectorType reslicerPlaneNormal;

  if( m_ReslicerPlaneNormalObserver->GotReslicerPlaneNormal() )
    {
      reslicerPlaneNormal = 
                        m_ReslicerPlaneNormalObserver->GetReslicerPlaneNormal();
      m_Plane->SetNormal( reslicerPlaneNormal[0], 
                          reslicerPlaneNormal[1], 
                          reslicerPlaneNormal[2] );
    }
  else
    return;

  m_Cutter->Update();

  vtkPolyData* outputPd = m_Cutter->GetOutput();

  // if we don't have any intersection return
  // fixme: make the representation invisible
  if (!outputPd)
    return;

  // get the center of the bounding box around the contour
  double* cutc = outputPd->GetCenter();

  double newcenter[3];
  // project it back onto the reslicing plane
  m_Plane->ProjectPoint(cutc, m_Plane->GetOrigin(), 
                                               m_Plane->GetNormal(), newcenter);

  //m_Sphere->SetCenter(newcenter[0], newcenter[1], newcenter[2] );
  
  m_PlaneSource->SetCenter( newcenter[0], newcenter[1], newcenter[2] );
  m_PlaneSource->SetNormal( reslicerPlaneNormal[0],
                            reslicerPlaneNormal[1],
                            reslicerPlaneNormal[2] );

  this->UpdatePlane();
}

/** Set the color */
template < class TImageSpatialObject >
void ImageResliceObjectRepresentation< TImageSpatialObject >
::SetFrameColor(
  ColorScalarType r, ColorScalarType g, ColorScalarType b)
{
  if( m_FrameColor[0] == r && 
      m_FrameColor[1] == g && 
      m_FrameColor[2] == b    )
    {
    return;
    }
  m_FrameColor[0] = r;
  m_FrameColor[1] = g;
  m_FrameColor[2] = b;
}

template < class TImageSpatialObject >
void ImageResliceObjectRepresentation< TImageSpatialObject >
::SetRestrictPlaneToVolume(int value)
{
  if ( m_RestrictPlaneToVolume == value )
    {
    return;
    }
    
  m_RestrictPlaneToVolume = value;

  this->UpdatePlane();
}
  
template < class TImageSpatialObject >
void ImageResliceObjectRepresentation< TImageSpatialObject >
::SetTextureInterpolate(int value)
{
  if ( m_TextureInterpolate == value )
    {
    return;
    }
    
  if (!m_Texture)
    {
    return;
    }
    
  m_TextureInterpolate = value;

  m_Texture->SetInterpolate(m_TextureInterpolate);
}
  
/** Get the red color component */
template < class TImageSpatialObject >
ObjectRepresentation::ColorScalarType
ImageResliceObjectRepresentation< TImageSpatialObject >::GetFrameRed() const
{
  return this->m_FrameColor[0];
}

/** Get the green color component */ 
template < class TImageSpatialObject >
ObjectRepresentation::ColorScalarType
ImageResliceObjectRepresentation< TImageSpatialObject >::GetFrameGreen() const
{
  return this->m_FrameColor[1];
}

/** Get the blue color component */
template < class TImageSpatialObject >
ObjectRepresentation::ColorScalarType
ImageResliceObjectRepresentation< TImageSpatialObject >::GetFrameBlue() const  
{
  return this->m_FrameColor[2];
}

/** Create the vtk Actors */
template < class TImageSpatialObject >
void
ImageResliceObjectRepresentation< TImageSpatialObject >
::CreateActors()
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceObjectRepresentation\
                        ::CreateActors called...\n");

  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  m_ImageActor = vtkActor::New();
 // m_ImageActor->SetProperty(m_PlaneProperty);
  this->AddActor( m_ImageActor );  

  // for dubugging purpose
  //m_SphereActor = vtkActor::New();  
  //m_EdgesActor = vtkActor::New();
  //this->AddActor( m_EdgesActor );
  //this->AddActor( m_SphereActor );


  igstkPushInputMacro( ConnectVTKPipeline );
  m_StateMachine.ProcessInputs(); 

}
 
template < class TImageSpatialObject >
void
ImageResliceObjectRepresentation< TImageSpatialObject >
::SetImage( const vtkImageData * image )
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceObjectRepresentation\
                        ::SetImage called...\n");

  // This const_cast<> is needed here due to 
  // the lack of const-correctness in VTK 
  m_ImageData = const_cast< vtkImageData *>( image );
}


template < class TImageSpatialObject >
void
ImageResliceObjectRepresentation< TImageSpatialObject >
::ConnectVTKPipelineProcessing() 
{
  this->SetResliceInterpolate(m_ResliceInterpolate);

  m_ColorMap->SetLookupTable(m_LookupTable);
  m_ColorMap->SetOutputFormatToRGBA();
  m_ColorMap->PassAlphaToOutputOn();

  vtkPolyDataMapper* texturePlaneMapper = vtkPolyDataMapper::New();
  texturePlaneMapper->SetInput(
    vtkPolyData::SafeDownCast(m_PlaneSource->GetOutput()));

  m_Texture->SetQualityTo32Bit();
  m_Texture->MapColorScalarsThroughLookupTableOff();
  m_Texture->SetInterpolate(m_TextureInterpolate);
  m_Texture->RepeatOff();

  m_ImageActor->SetMapper(texturePlaneMapper);
  m_ImageActor->SetTexture(m_Texture);
  
  m_ImageActor->PickableOff();
  // m_ImageActor->SetProperty(m_PlaneProperty);
  
  texturePlaneMapper->Delete();
}

template < class TImageSpatialObject >
void
ImageResliceObjectRepresentation< TImageSpatialObject >
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
ImageResliceObjectRepresentation< TImageSpatialObject >
::GetVector2(double v2[3])
{
  double* p2 = m_PlaneSource->GetPoint2();
  double* o =  m_PlaneSource->GetOrigin();
  v2[0] = p2[0] - o[0];
  v2[1] = p2[1] - o[1];
  v2[2] = p2[2] - o[2];
}

template < class TImageSpatialObject >
void ImageResliceObjectRepresentation< TImageSpatialObject >
::UpdatePlane()
{
  if ( !m_ImageReslicer ||
       !(m_ImageData = vtkImageData::SafeDownCast(m_ImageReslicer->GetInput())))
    {
    return;
    }

  // Calculate appropriate pixel spacing for the reslicing
  //
  int i;

  for (i = 0; i < 3; i++)
    {
    if (m_ImageExtent[2*i] > m_ImageExtent[2*i + 1])
      {
      igstkLogMacro( DEBUG, "igstk::ImageResliceObjectRepresentation\
            invalid extent!\n");
      return;
      }
    }

  if ( m_RestrictPlaneToVolume )
    {
    double bounds[] = 
                {m_ImageOrigin[0] + m_ImageSpacing[0]*m_ImageExtent[0], //xmin
                 m_ImageOrigin[0] + m_ImageSpacing[0]*m_ImageExtent[1], //xmax
                 m_ImageOrigin[1] + m_ImageSpacing[1]*m_ImageExtent[2], //ymin
                 m_ImageOrigin[1] + m_ImageSpacing[1]*m_ImageExtent[3], //ymax
                 m_ImageOrigin[2] + m_ImageSpacing[2]*m_ImageExtent[4], //zmin
                 m_ImageOrigin[2] + m_ImageSpacing[2]*m_ImageExtent[5]};//zmax

    for ( i = 0; i <= 4; i += 2 ) // reverse bounds if necessary
      {
      if ( bounds[i] > bounds[i+1] )
        {
        double t = bounds[i+1];
        bounds[i+1] = bounds[i];
        bounds[i] = t;
        }
      }

    double abs_normal[3];
    m_PlaneSource->GetNormal(abs_normal);
    double planeCenter[3];
    m_PlaneSource->GetCenter(planeCenter);
    double nmax = 0.0;
    int k = 0;
    for ( i = 0; i < 3; i++ )
      {
      abs_normal[i] = fabs(abs_normal[i]);
      if ( abs_normal[i]>nmax )
        {
        nmax = abs_normal[i];
        k = i;
        }
      }

    // Force the plane to lie within the true image bounds along its normal
    //
    if ( planeCenter[k] > bounds[2*k+1] )
      {
      planeCenter[k] = bounds[2*k+1];
      }
    else if ( planeCenter[k] < bounds[2*k] )
      {
      planeCenter[k] = bounds[2*k];
      }

    m_PlaneSource->SetCenter(planeCenter);
    }

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
  for ( i = 0; i < 3; i++ )
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

  if ( m_ReslicePlaneSpatialObject.IsNotNull() )
    {
    // update the current reslicer transformation in the reslicer plane spatial 
    // object
    igstk::Transform updatedTransform;
    updatedTransform.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime());
    updatedTransform.ImportTransform( *m_ResliceAxes );

    m_ReslicePlaneSpatialObject->RequestUpdateTransformToParent( 
                                                             updatedTransform );
    }

  double neworiginXYZW[4];
  m_ResliceAxes->MultiplyPoint(originXYZW, neworiginXYZW);

  m_ResliceAxes->SetElement(0,3,neworiginXYZW[0]);
  m_ResliceAxes->SetElement(1,3,neworiginXYZW[1]);
  m_ResliceAxes->SetElement(2,3,neworiginXYZW[2]);

  m_ImageReslicer->SetResliceAxes(m_ResliceAxes);

  double spacingX = fabs(planeAxis1[0]*m_ImageSpacing[0])+
                   fabs(planeAxis1[1]*m_ImageSpacing[1])+
                   fabs(planeAxis1[2]*m_ImageSpacing[2]);

  double spacingY = fabs(planeAxis2[0]*m_ImageSpacing[0])+
                   fabs(planeAxis2[1]*m_ImageSpacing[1])+
                   fabs(planeAxis2[2]*m_ImageSpacing[2]);


  // Pad extent up to a power of two for efficient texture mapping

  // make sure we're working with valid values
  double realExtentX = ( spacingX == 0 ) ? VTK_INT_MAX : planeSizeX / spacingX;

  int extentX;
  // Sanity check the input data:
  // * if realExtentX is too large, extentX will wrap
  // * if spacingX is 0, things will blow up.
  if (realExtentX > (VTK_INT_MAX >> 1))
    {
    igstkLogMacro( DEBUG, "igstk::ImageResliceObjectRepresentation\
                   invalid X extent: " << realExtentX << " \n");
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
  double realExtentY = ( spacingY == 0 ) ? VTK_INT_MAX : planeSizeY / spacingY;

  int extentY;
  if (realExtentY > (VTK_INT_MAX >> 1))
    {
    igstkLogMacro( DEBUG, "igstk::ImageResliceObjectRepresentation\
                   invalid Y extent: " << realExtentY << " \n");
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

  double outputSpacingX = (planeSizeX == 0) ? 1.0 : planeSizeX/extentX;
  double outputSpacingY = (planeSizeY == 0) ? 1.0 : planeSizeY/extentY;
  m_ImageReslicer->SetOutputSpacing(outputSpacingX, outputSpacingY, 1);
  m_ImageReslicer->SetOutputOrigin(0.5*outputSpacingX, 0.5*outputSpacingY, 0);
  m_ImageReslicer->SetOutputExtent(0, extentX-1, 0, extentY-1, 0, 0);

}

template < class TImageSpatialObject >
void ImageResliceObjectRepresentation< TImageSpatialObject >
::SetResliceInterpolate(int value)
{
  if ( m_ResliceInterpolate == value )
    {
    return;
    }
    
  m_ResliceInterpolate = value;

  if ( !m_ImageReslicer )
    {
    return;
    }
  
  if ( value == VTK_NEAREST_RESLICE )
    {
    m_ImageReslicer->SetInterpolationModeToNearestNeighbor();
    }
  else if ( value == VTK_LINEAR_RESLICE)
    {
    m_ImageReslicer->SetInterpolationModeToLinear();
    }
  else
    {
    m_ImageReslicer->SetInterpolationModeToCubic();
    }
    
  m_Texture->SetInterpolate(m_TextureInterpolate);
}

/** Create a copy of the current object representation */
template < class TImageSpatialObject >
typename ImageResliceObjectRepresentation< TImageSpatialObject >::Pointer
ImageResliceObjectRepresentation< TImageSpatialObject >
::Copy() const
{
  igstkLogMacro( DEBUG, "igstk::ImageResliceObjectRepresentation\
                        ::Copy called...\n");

  Pointer newOR = ImageResliceObjectRepresentation::New();
  newOR->SetColor( this->GetRed(),this->GetGreen(),this->GetBlue() );
  newOR->SetFrameColor( this->GetFrameRed(),
                        this->GetFrameGreen(),
                        this->GetFrameBlue() );
  newOR->SetOpacity( this->GetOpacity() );
  newOR->RequestSetImageSpatialObject( m_ImageSpatialObject );
  newOR->RequestSetReslicePlaneSpatialObject( m_ReslicePlaneSpatialObject );

  return newOR;
}

} // end namespace igstk

#endif
