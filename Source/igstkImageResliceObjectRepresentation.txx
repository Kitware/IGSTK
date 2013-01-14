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

#include <vtkImageData.h>
#include <vtkTransform.h>
#include <vtkPlaneSource.h>
#include <vtkPlane.h>
#include <vtkCamera.h>
#include <vtkMath.h>
#include <vtkImageMapToColors.h>
#include <vtkImageResliceMapper.h>
#include <vtkImageProperty.h>
#include <vtkImageSlice.h>

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

  m_PlaneSource = NULL;
  m_Plane = NULL;
  m_ImageData = NULL;

  // Create classes for displaying images
  m_ImageSlice = vtkImageSlice::New();
 
  // Setting of vtkImageProperty
	m_ImageProperty = vtkImageProperty::New();
	m_ImageProperty->SetColorWindow(m_Window);
	m_ImageProperty->SetColorLevel(m_Level);
	m_ImageProperty->SetAmbient(1.0);
  m_ImageProperty->SetBacking(1.0);
  m_ImageProperty->SetBackingColor(1.0,0.0,0.0);
	m_ImageProperty->SetOpacity(1.0);
	m_ImageProperty->SetInterpolationTypeToLinear();

  this->AddActor( m_ImageSlice );

  m_ResliceInterpolate       = VTK_NEAREST_RESLICE;

  m_PlaneSource = vtkPlaneSource::New();
  m_PlaneSource->SetXResolution(1);
  m_PlaneSource->SetYResolution(1);

  // Define some default point coordinates
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

  m_Plane = vtkPlane::New();
  m_Plane->SetOrigin( m_PlaneSource->GetCenter() );
  m_Plane->SetNormal( m_PlaneSource->GetNormal() );

  m_Level = 0;
  m_Window = 0;

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

  if ( m_ImageSlice != NULL )
    {
    m_ImageSlice->Delete();
    m_ImageSlice = NULL;
    }

  if ( m_ImageProperty != NULL )
    {
    m_ImageProperty->Delete();
    m_ImageProperty = NULL;
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
  
  m_ImageSlice = NULL;
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

  typedef igstk::ReslicerPlaneSpatialObject::OrientationType OrientationType;
  OrientationType orientation
                        = m_ReslicePlaneSpatialObject->GetOrientationType();
  switch(orientation)
  {
    case ReslicerPlaneType::Axial:
      {
        m_PlaneSource->SetPoint1(m_xbounds[1],m_ybounds[0],m_zbounds[0]);
        m_PlaneSource->SetPoint2(m_xbounds[0],m_ybounds[1],m_zbounds[0]);
        break;
      }
    case ReslicerPlaneType::OffAxial:
      {
        m_PlaneSource->SetPoint1(m_xbounds[1],m_ybounds[0],m_zbounds[0]);
        m_PlaneSource->SetPoint2(m_xbounds[0],m_ybounds[1],m_zbounds[0]);
        break;
      }
    case ReslicerPlaneType::Coronal:
      {
        m_PlaneSource->SetPoint1(m_xbounds[1],m_ybounds[0],m_zbounds[0]);
        m_PlaneSource->SetPoint2(m_xbounds[0],m_ybounds[0],m_zbounds[1]);
        break;
      }
    case ReslicerPlaneType::OffCoronal:
      {
        m_PlaneSource->SetPoint1(m_xbounds[1],m_ybounds[0],m_zbounds[0]);
        m_PlaneSource->SetPoint2(m_xbounds[0],m_ybounds[0],m_zbounds[1]);
        break;
      }
    case ReslicerPlaneType::Sagittal:
      {
        m_PlaneSource->SetPoint1(m_xbounds[0],m_ybounds[1],m_zbounds[0]);
        m_PlaneSource->SetPoint2(m_xbounds[0],m_ybounds[0],m_zbounds[1]);
        break;
      }
    case ReslicerPlaneType::OffSagittal:
      {
        m_PlaneSource->SetPoint1(m_xbounds[0],m_ybounds[1],m_zbounds[0]);
        m_PlaneSource->SetPoint2(m_xbounds[0],m_ybounds[0],m_zbounds[1]);
        break;
      }
    case ReslicerPlaneType::PlaneOrientationWithXAxesNormal:
    case ReslicerPlaneType::PlaneOrientationWithYAxesNormal:
    case ReslicerPlaneType::PlaneOrientationWithZAxesNormal:
      {
        m_PlaneSource->SetPoint1(m_xbounds[1],m_ybounds[0],m_zbounds[0]);
        m_PlaneSource->SetPoint2(m_xbounds[0],m_ybounds[1],m_zbounds[0]);
        break;
      }
    default:
      {
        std::cerr << "Invalid orientation" << std::endl;
        break;
      }
  }
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

  m_ImageProperty->SetColorWindow(m_Window);
	m_ImageProperty->SetColorLevel(m_Level);
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

  this->SetResliceInterpolate(m_ResliceInterpolate);

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

  m_PlaneSource->SetOrigin(m_xbounds[0],m_ybounds[0],m_zbounds[0]);

  m_Plane->SetOrigin( m_PlaneSource->GetCenter() );
  m_Plane->SetNormal( m_PlaneSource->GetNormal() );
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

  m_ImageResliceMapper->SetSlicePlane(m_Plane);
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

  m_ImageSlice = vtkImageSlice::New();
  this->AddActor( m_ImageSlice );

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

  // Setting of vtkImageResliceMapper
	m_ImageResliceMapper = vtkImageResliceMapper::New();
  m_ImageResliceMapper->SetInput(m_ImageData);
  m_ImageResliceMapper->SetSlicePlane(m_Plane);
  m_ImageResliceMapper->BorderOn();

	m_ImageSlice->SetMapper(m_ImageResliceMapper);
	m_ImageSlice->SetProperty(m_ImageProperty);
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
::SetResliceInterpolate(int value)
{
  if ( m_ResliceInterpolate == value )
    {
    return;
    }
    
  m_ResliceInterpolate = value;

  if ( m_ImageSlice )
    {
    return;
    }
  
  if ( value == VTK_NEAREST_RESLICE )
    {
      m_ImageProperty->SetInterpolationTypeToNearest();
    }
  else if ( value == VTK_LINEAR_RESLICE)
    {
      m_ImageProperty->SetInterpolationTypeToLinear();
    }
  else
    {
      m_ImageProperty->SetInterpolationTypeToCubic();
    }
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
  newOR->SetOpacity( this->GetOpacity() );
  newOR->RequestSetImageSpatialObject( m_ImageSpatialObject );
  newOR->RequestSetReslicePlaneSpatialObject( m_ReslicePlaneSpatialObject );

  return newOR;
}

} // end namespace igstk

#endif
