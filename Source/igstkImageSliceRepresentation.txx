/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageSliceRepresentation.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkImageSliceRepresentation_txx
#define __igstkImageSliceRepresentation_txx


#include "igstkImageSliceRepresentation.h"

#include "vtkImageData.h"
#include "igstkEvents.h"

namespace igstk
{

/** Constructor */
template < class TImageSpatialObject >
ImageSliceRepresentation < TImageSpatialObject >
::ImageSliceRepresentation():m_StateMachine(this)
{
  m_SliceOrientation = Axial;
  m_SurgeonPosition  = Left;

  // We create the image spatial object
  m_ImageSpatialObject = NULL;
  m_ImageData  = NULL;
  this->RequestSetSpatialObject( m_ImageSpatialObject );

  m_ProbeTransform.SetToIdentity( 1000 );

  m_SliceSize      = 250;
  m_CameraDistance = 1000;
  m_ActorOpacity   = 0.7;

  // Set default orientation
  this->RequestSetSliceOrientation( Axial );

  // Create classes for displaying images
  m_Actor = vtkImageActor::New();
  m_Actor->SetOpacity( m_ActorOpacity );
  this->AddActor( m_Actor );

  // Set default values for window and level
  m_Level = 0;
  m_Window = 2000;
  m_MapColors  = vtkImageMapToWindowLevelColors::New();
  m_MapColors->SetWindow( m_Window );
  m_MapColors->SetLevel( m_Level );

  // Image reslice
  m_ImageReslice = vtkImageReslice::New();
  m_ImageReslice->SetBackgroundColor( 128.0, 128.0, 128.0, 0 );
  m_ImageReslice->AutoCropOutputOn();
  m_ImageReslice->SetOptimization( 1 );
  m_ImageReslice->SetOutputSpacing( 1, 1, 1 );
  m_ImageReslice->SetOutputDimensionality( 2 );

  m_ResliceAxes = vtkMatrix4x4::New();

  // Observer for vtkImageData
  m_VTKImageObserver = VTKImageObserver::New();

}

/** Destructor */

template < class TImageSpatialObject >
ImageSliceRepresentation < TImageSpatialObject >
::~ImageSliceRepresentation()
{
  // This deletes also the m_Actor
  this->DeleteActors();

}


template < class TImageSpatialObject >
void
ImageSliceRepresentation < TImageSpatialObject >
::RequestSetSliceOrientation( SliceOrientationType orientation )
{
  m_SliceOrientation = orientation; //FIXME

  switch ( m_SliceOrientation )
    {
    case Axial:
    case OffAxial:
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
      m_AxisY[2] = 1;   //??
      m_AxisZ[0] = 0;
      m_AxisZ[1] = -1;   //??
      m_AxisZ[2] = 0;
    break;
    case Sagittal:
    case OffSagittal:
      m_AxisX[0] = 0;
      m_AxisX[1] = 0;   //??
      m_AxisX[2] = 1;
      m_AxisY[0] = 0;
      m_AxisY[1] = 1;
      m_AxisY[2] = 0;
      m_AxisZ[0] = -1;
      m_AxisZ[1] = 0;
      m_AxisZ[2] = 0;
    break;
    }

  //ADDCODE
  // this->SetupCamera();
}

/** Overloaded DeleteActor function */
template < class TImageSpatialObject >
void
ImageSliceRepresentation < TImageSpatialObject >
::DeleteActors( )
{
  igstkLogMacro( DEBUG, "igstk::ImageSliceRepresentation\
                        ::DeleteActors called...\n");

  this->Superclass::DeleteActors();

  m_Actor = NULL;

}

/** Set the Image Spatial Object */
template < class TImageSpatialObject >
void
ImageSliceRepresentation < TImageSpatialObject >
::RequestSetImageSpatialObject( const ImageSpatialObjectType * image )
{
  m_ImageSpatialObject = image;
  this->SetImageSpatialObjectProcessing(); //FIXME
}

/** Set nhe Image Spatial Object */
template < class TImageSpatialObject >
void
ImageSliceRepresentation < TImageSpatialObject >
::SetImageSpatialObjectProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ImageSliceRepresentation\
                         ::SetImageSpatialObjectProcessing called...\n");

  m_ImageSpatialObject->AddObserver( VTKImageModifiedEvent(),
                                      m_VTKImageObserver );

  //
  // Here we get a VTK image data from the private method of the
  // ImageSpatialObject and stores it in the representation .
  //
  m_VTKImageObserver->Reset();

  m_ImageSpatialObject->RequestGetVTKImage();

  if( m_VTKImageObserver->GotVTKImage() )
    {
    m_ImageData = const_cast< vtkImageData *>
                                        ( m_VTKImageObserver->GetVTKImage() );
    m_ImageData->Update();
    }

  //ADDCODE
}

template < class TImageSpatialObject >
void
ImageSliceRepresentation < TImageSpatialObject >
::RequestSetProbeTransform( TransformType trasform )
{
  m_ProbeTransform = trasform;
  igstk::Transform::VectorType   translation, probeVector;

  probeVector[0] = 0;
  probeVector[1] = 0;
  probeVector[2] = -1;
  probeVector = m_ProbeTransform.GetRotation().Transform(probeVector);
  translation = m_ProbeTransform.GetTranslation();

  for (unsigned int i=0; i<3; i++)
    {
    m_ProbeVector[i]   = probeVector[i];
    m_ProbePosition[i] = translation[i];
    }
}

template < class TImageSpatialObject >
void
ImageSliceRepresentation < TImageSpatialObject >
::RequestSetProbeVectorAndPosition(double * probeVector,
                                   double * probePosition )
{
  for (unsigned int i=0; i<3; i++)
    {
    m_ProbeVector[i]   = probeVector[i];
    m_ProbePosition[i] = probePosition[i];
    }

}

template < class TImageSpatialObject >
vtkCamera *
ImageSliceRepresentation < TImageSpatialObject >
::RequestReslice( )
{
  igstkLogMacro( DEBUG,
           "igstk::ImageSliceRepresentation::ResliceProcessing called...\n");

  itk::Vector< double, 3 >       vx, vy, vn, v;

  if( m_SliceOrientation == OffAxial)
    {
    // Calculate the reslice axes
    vx.Fill( 0.0 );
    vx[0] = 1;

    vy[0] = m_ProbeVector[0];
    vy[1] = m_ProbeVector[1];
    vy[2] = m_ProbeVector[2];

    if ( fabs(vx*vy) < 1e-9 )
      {
      // FIXME: need to handle this situation too
      igstkLogMacro( DEBUG, "The two vectors are parrelell \n");
      }

    vn = itk::CrossProduct( vx, vy );
    vy = itk::CrossProduct( vn, vx );

    v.Fill( 0.0 );
    v[0] = 1;
    if (vx*v<0)
    {
      vx *= -1;
    }

    v.Fill(0.0);
    v[1] = 1;
    if (vy*v<0)
    {
      vy *= -1;
    }

    v.Fill( 0.0 );
    v[2] = 1;
    if (vn*v<0)
    {
      vn *= -1;
    }

    vx.Normalize();
    vy.Normalize();
    vn.Normalize();

    }
  else if ( m_SliceOrientation == Perpendicular)
    {
        // Calculate the reslice axes
    vx.Fill( 0.0 );
    if( m_SurgeonPosition == Left )
      {
      vx[2] = -1;
      }
    else if ( m_SurgeonPosition == Right )
      {
      vx[2] = 1;
      }

    vn[0] = m_ProbeVector[0];
    vn[1] = m_ProbeVector[1];
    vn[2] = m_ProbeVector[2];

    if ( fabs(vx*vn) < 1e-9 )
      {
      // FIXME: need to handle this situation too
      igstkLogMacro( DEBUG, "The two vectors are parrelell \n");
      }

    vy = itk::CrossProduct( vn, vx );
    vx = itk::CrossProduct( vy, vn );

    v.Fill( 0.0 );
    //v[2] = -1;
    if( m_SurgeonPosition == Left )
      {
      v[2] = -1;
      }
    else if ( m_SurgeonPosition == Right )
      {
      v[2] = 1;
      }
    if (vx*v<0)
    {
      vx *= -1;
    }

    v.Fill(0.0);
    //v[1] = 1;

    if( m_SurgeonPosition == Left )
      {
      v[0] = -1;
      }
    else if ( m_SurgeonPosition == Right )
      {
      v[0] = 1;
      }

    if (vy*v<0)
    {
      vy *= -1;
    }

    vx.Normalize();
    vy.Normalize();
    vn.Normalize();

    }
  else if( m_SliceOrientation == OffSagittal)
    {
    // Calculate the reslice axes
    vy.Fill( 0.0 );
    vy[2] = -1;

    vx[0] = m_ProbeVector[0];
    vx[1] = m_ProbeVector[1];
    vx[2] = m_ProbeVector[2];

    if ( fabs(vx*vy) < 1e-9 )
      {
      // FIXME: need to handle this situation too
      igstkLogMacro( DEBUG, "The two vectors are parrelell \n");
      }

    vn = itk::CrossProduct( vx, vy );
    vx = itk::CrossProduct( vy, vn );

    v.Fill( 0.0 );
    v[1] = 1;
    if (vx*v<0)
    {
      vx *= -1;
    }

    v.Fill(0.0);
    v[0] = -1;
    if (vn*v<0)
    {
      vn *= -1;
    }

    vx.Normalize();
    vy.Normalize();
    vn.Normalize();

    }
  else if( m_SliceOrientation == Axial)
    {
    vx.Fill( 0 );
    vy.Fill( 0 );
    vn.Fill( 0 );
    vx[0] = 1;
    vy[1] = 1;
    vn[2] = 1;
    }

  // set the reslice axes
  m_ResliceAxes->Identity();
  for ( int i = 0; i < 3; i++ )
    {
    m_ResliceAxes->SetElement(i, 0, vx[i] );
    m_ResliceAxes->SetElement(i, 1, vy[i] );
    m_ResliceAxes->SetElement(i, 2, vn[i] );
    m_ResliceAxes->SetElement(i, 3, m_ProbePosition[i]);
    }

  m_ImageReslice->SetResliceAxes( m_ResliceAxes );
  m_ImageReslice->SetOutputSpacing( 1,1,1 );
  m_ImageReslice->SetOutputOrigin( -m_SliceSize/2, -m_SliceSize/2, 0 );
  m_ImageReslice->SetOutputExtent( 1, int(m_SliceSize+0.5),
                                   1, int(m_SliceSize+0.5),
                                   0, 0 );

  m_Actor->SetUserMatrix(m_ResliceAxes);

  //Setting up the camera position
  double focalPoint[3];
  double position[3];
  for ( int i = 0; i<3; i++ )
    {
    focalPoint[i] = m_ProbePosition[i];
    position[i] = m_ProbePosition[i];
    }

  for ( int i = 0; i<3; i++ )
    {
    position[i] -= m_CameraDistance * vn[i];
    }

  vtkCamera   * camera = vtkCamera::New();
  camera->SetViewUp( -vy[0], -vy[1], -vy[2] );
  camera->SetFocalPoint( focalPoint );
  //camera->SetParallelScale( 0.8 );
  //camera->Zoom( 2 );
  camera->SetPosition( position );

  return camera;

  //m_ImageReslice->Print( std::cout );
  //m_ImageReslice->GetOutput()->Print( std::cout );
}


template < class TImageSpatialObject >
void
ImageSliceRepresentation < TImageSpatialObject >
::SetWindowLevel( double window, double level )
{
  igstkLogMacro( DEBUG, "igstk::ImageSliceRepresentation\
                        ::SetWindowLevel called...\n");

  m_Window = window;
  m_Level = level;

  m_MapColors->SetWindow( m_Window );
  m_MapColors->SetLevel( m_Level );

  //ADDCODE
  // Update plane
}


/** Print Self function */
template < class TImageSpatialObject >
void
ImageSliceRepresentation < TImageSpatialObject >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << m_SliceOrientation << "\n";
  os << indent << m_ProbeTransform << "\n";
}


/** Update the visual representation in response to changes in the geometric
 * object */
template < class TImageSpatialObject >
void
ImageSliceRepresentation < TImageSpatialObject >
::UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ImageSliceRepresentation::\
                         UpdateRepresentationProcessing called...\n");
  if( m_ImageReslice->GetOutput() )
    {
    //ADDCODE
    //m_MapColors->SetInput( m_ImageReslice->GetOutput() );
    }
}


/** Create the vtk Actors */
template < class TImageSpatialObject >
void
ImageSliceRepresentation < TImageSpatialObject >
::CreateActors()
{
  igstkLogMacro( DEBUG, "igstk::ImageSliceRepresentation\
                        ::CreateActors called...\n");

  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  m_ImageData->GetExtent( m_Ext );
  m_ImageData->GetDimensions( m_Dim );
  m_ImageData->GetOrigin( m_Origin );
  m_ImageData->GetSpacing( m_Spacing );
  m_ImageData->GetCenter( m_FocalPoint );

  m_ResliceAxes->Identity();
  for ( int i = 0; i < 3; i++ )
    {
    m_ResliceAxes->SetElement(i, 0, m_AxisX[i] );
    m_ResliceAxes->SetElement(i, 1, m_AxisY[i] );
    m_ResliceAxes->SetElement(i, 2, m_AxisZ[i] );
    m_ResliceAxes->SetElement(i, 3, m_FocalPoint[i]);
    }

  m_ImageReslice->SetInput ( m_ImageData );
  m_ImageReslice->SetResliceAxes( m_ResliceAxes );
  m_ImageReslice->SetOutputDimensionality(2);
  //m_ImageReslice->SetInterpolationModeToNearestNeighbor();
  m_ImageReslice->SetInterpolationModeToLinear();
  //m_ImageReslice->SetInterpolationModeToCubic();
  m_ImageReslice->SetOutputSpacing( 1, 1, 1);
  m_ImageReslice->SetOutputOrigin( -m_SliceSize/2, -m_SliceSize/2, 0 );
  m_ImageReslice->SetOutputExtent( 1, int(m_SliceSize+0.5),
                                   1, int(m_SliceSize+0.5),
                                   0, 0 );

  //m_MapColors->SetOutputFormatToRGBA();
  //m_MapColors->SetOutputFormatToLuminance();
  //m_MapColors->PassAlphaToOutputOn();
  m_MapColors->SetInput( m_ImageReslice->GetOutput() );

  m_Actor = vtkImageActor::New();
  m_Actor->SetInput( m_MapColors->GetOutput() );
  m_Actor->SetUserMatrix( m_ResliceAxes );
  m_Actor->SetOpacity( m_ActorOpacity );
  this->AddActor( m_Actor );
}

template < class TImageSpatialObject >
void
ImageSliceRepresentation < TImageSpatialObject >
::ConnectVTKPipelineProcessing()
{
  /*
  m_ImageReslice->SetInput ( m_ImageData );
  //m_ReslicedImageData = m_ImageReslice->GetOutput();
  m_MapColors->SetInput( m_ImageReslice->GetOutput() );
  m_Actor->SetInput( m_MapColors->GetOutput() );
  m_Actor->InterpolateOn();
  */
}

} // end namespace igstk

#endif
