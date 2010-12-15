/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageSliceRepresentationPlus.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkImageSliceRepresentationPlus_txx
#define __igstkImageSliceRepresentationPlus_txx


#include "igstkImageSliceRepresentationPlus.h"

#include "vtkImageData.h"
#include "igstkEvents.h"
#include "vtkWindowLevelLookupTable.h"

namespace igstk
{

/** Constructor */
template < class TBGImageSO,  class TFGImageSO  >
ImageSliceRepresentationPlus < TBGImageSO,  TFGImageSO  >
::ImageSliceRepresentationPlus():m_StateMachine(this)
{
  m_Orientation = Axial;
  m_SurgeonPosition  =PatientLeft;

  this->SetPositionLocking(true);

  // We create the image spatial object
  m_BGImageSO = NULL;
  m_FGImageSO = NULL;
  m_BGImageData  = NULL;
  m_FGImageData  = NULL;
  this->RequestSetSpatialObject( m_BGImageSO );

  m_Transform.SetToIdentity( 1000 );
  m_CalibrationTransform.SetToIdentity( 1000 );
  m_ResliceTransform.SetToIdentity( 1000 );

  m_SliceSize      = 250;
  m_CameraDistance = 1000;
  m_BGOpacity      = 1.0;
  m_VirtualTip     = 0.0;
  m_NeedleVector.Fill(0.0);
  m_PathVector.Fill(0.0);

  // Create classes for displaying images
  m_BGActor = vtkImageActor::New();
  m_BGActor->SetOpacity( m_BGOpacity );
  this->AddActor( m_BGActor );

  m_FGActor = vtkImageActor::New();
  m_FGActor->SetOpacity( 1-m_BGOpacity );
  this->AddActor( m_FGActor );

  m_ResliceAxes = vtkMatrix4x4::New();
  m_ResliceAxes->Identity();
  m_Camera      = vtkCamera::New();

  // Observer for vtkImageData
  m_VTKImageObserver  = VTKImageObserver::New();
  m_TransformObserver = igstk::TransformObserver::New();

  // Set default values for window and level
  m_BGImageMapper = vtkImageMapToColors::New();
  //m_BGImageMapper->SetOutputFormatToRGBA();
  m_BGImageMapper->SetOutputFormatToLuminance();
  //m_BGImageMapper->PassAlphaToOutputOn();
  //m_BGImageLUT = vtkWindowLevelLookupTable::New();
  vtkWindowLevelLookupTable * lut = vtkWindowLevelLookupTable::New();
  lut->SetLevel( 0 );
  lut->SetWindow( 1000 );
  lut->Build();
  m_BGImageLUT = lut;
  m_BGImageMapper->SetLookupTable( m_BGImageLUT );

  m_FGImageMapper = vtkImageMapToColors::New();
  m_FGImageMapper->SetOutputFormatToRGBA();
  //m_FGImageMapper->SetOutputFormatToLuminance();
  //m_FGImageMapper->PassAlphaToOutputOn();
  vtkWindowLevelLookupTable * lut2 = vtkWindowLevelLookupTable::New();
  lut2->SetLevel( 0 );
  lut2->SetWindow( 1000 );
  lut2->SetMinimumTableValue(0,0,0,0);
  lut2->SetMaximumTableValue(1,0,0,1);
  m_FGImageLUT = lut2;
  m_FGImageMapper->SetLookupTable( m_FGImageLUT );

  // Image reslice
  m_BGImageReslice = vtkImageReslice::New();
  m_BGImageReslice->SetResliceAxes( m_ResliceAxes );
  m_BGImageReslice->SetInterpolationModeToLinear();
  m_BGImageReslice->SetBackgroundColor( 128.0, 128.0, 128.0, 0 );
  m_BGImageReslice->AutoCropOutputOn();
  m_BGImageReslice->SetOptimization( 1 );
  m_BGImageReslice->SetOutputSpacing( 1, 1, 1 );
  m_BGImageReslice->SetOutputDimensionality( 2 );
  m_BGImageReslice->SetOutputOrigin( -m_SliceSize/2, -m_SliceSize/2, 0 );
  m_BGImageReslice->SetOutputExtent( 1, int(m_SliceSize+0.5),
                                   1, int(m_SliceSize+0.5),
                                   0, 0 );

  m_FGImageReslice = vtkImageReslice::New();
  m_FGImageReslice->SetResliceAxes( m_ResliceAxes );
  m_FGImageReslice->SetInterpolationModeToLinear();
  m_FGImageReslice->SetBackgroundColor( 128.0, 128.0, 128.0, 0 );
  m_FGImageReslice->AutoCropOutputOn();
  m_FGImageReslice->SetOptimization( 1 );
  m_FGImageReslice->SetOutputSpacing( 1, 1, 1 );
  m_FGImageReslice->SetOutputDimensionality( 2 );
  m_FGImageReslice->SetOutputOrigin( -m_SliceSize/2, -m_SliceSize/2, 0 );
  m_FGImageReslice->SetOutputExtent( 1, int(m_SliceSize+0.5),
                                   1, int(m_SliceSize+0.5),
                                   0, 0 );

}

/** Destructor */

template < class TBGImageSO,  class TFGImageSO  >
ImageSliceRepresentationPlus < TBGImageSO,  TFGImageSO  >
::~ImageSliceRepresentationPlus()
{
  // This deletes also the m_BGActor
  this->DeleteActors();

  // Cleanup vtk classes
}


/** Overloaded DeleteActor function */
template < class TBGImageSO,  class TFGImageSO  >
void
ImageSliceRepresentationPlus < TBGImageSO,  TFGImageSO  >
::DeleteActors( )
{
  igstkLogMacro( DEBUG, "igstk::ImageSliceRepresentationPlus\
                        ::DeleteActors called...\n");

  this->Superclass::DeleteActors();

}

/** Set the Image Spatial Object */
template < class TBGImageSO,  class TFGImageSO  >
void
ImageSliceRepresentationPlus < TBGImageSO,  TFGImageSO  >
::RequestSetBGImageSO( BGImageSOType * image )
{
  m_BGImageSO = image;
  if(m_BGImageSO)
    {
    this->RequestSetSpatialObject( m_BGImageSO ); // Call supoer class

    m_BGImageSO->AddObserver( VTKImageModifiedEvent(),
                                      m_VTKImageObserver );
    m_VTKImageObserver->Reset();

    m_TransformObserver->ObserveTransformEventsFrom( m_BGImageSO );
    m_TransformObserver->Clear();

    m_BGImageSO->RequestGetVTKImage();

    if( m_VTKImageObserver->GotVTKImage() )
      {
      m_BGImageData = const_cast< vtkImageData *>
                                          ( m_VTKImageObserver->GetVTKImage() );
      m_BGImageData->Update();
      m_BGImageData->GetExtent( m_Extent );
      m_BGImageData->GetDimensions( m_Dim );
      m_BGImageData->GetOrigin( m_Origin );
      m_BGImageData->GetSpacing( m_Spacing );
      double center[3];
      m_BGImageData->GetCenter( center );
      for(unsigned int i=0; i<3; i++)
        {
        m_ImageCenter[i] = center[i];
        m_Size[i] = m_Dim[i] * m_Spacing[i];
        }
      }      
    }
}

template < class TBGImageSO,  class TFGImageSO  >
void
ImageSliceRepresentationPlus < TBGImageSO,  TFGImageSO  >
::GetExtent(int & min, int & max)
{
  if(m_BGImageData)
    {
    m_BGImageData->GetExtent( m_Extent );
    }

  switch( m_Orientation )
    {
    case Axial:
      min = m_Extent[4];
      max = m_Extent[5];
      break;
    case Sagittal:
      min = m_Extent[0];
      max = m_Extent[1];
      break;
    case Coronal:
      min = m_Extent[2];
      max = m_Extent[3];
      break;
      default:  // Invalid query under other orientations
      min = -1;
      max = -1;
    }
}

/** Set the Image Spatial Object */
template < class TBGImageSO,  class TFGImageSO  >
void
ImageSliceRepresentationPlus < TBGImageSO,  TFGImageSO  >
::RequestSetFGImageSO( FGImageSOType * image )
{
  m_FGImageSO = image;
  if(m_FGImageSO)
    {
    m_FGImageSO->AddObserver( VTKImageModifiedEvent(),
                                      m_VTKImageObserver );
    m_VTKImageObserver->Reset();

    m_FGImageSO->RequestGetVTKImage();

    if( m_VTKImageObserver->GotVTKImage() )
      {
      m_FGImageData = const_cast< vtkImageData *>
                                          ( m_VTKImageObserver->GetVTKImage() );
      m_FGImageData->Update();
      }
    }
}


template < class TBGImageSO,  class TFGImageSO  >
void
ImageSliceRepresentationPlus < TBGImageSO,  TFGImageSO  >
::SetBGImageMapper( vtkImageMapToColors * mapper )
{
  m_BGImageMapper = mapper;
}

template < class TBGImageSO,  class TFGImageSO  >
void
ImageSliceRepresentationPlus < TBGImageSO,  TFGImageSO  >
::SetFGImageMapper( vtkImageMapToColors * mapper )
{
  m_FGImageMapper = mapper;
}

template < class TBGImageSO,  class TFGImageSO  >
void
ImageSliceRepresentationPlus < TBGImageSO,  TFGImageSO  >
::SetBGImageLUT( vtkLookupTable * lut )
{
  m_BGImageLUT = lut;
  m_BGImageMapper->SetLookupTable( m_BGImageLUT );
}

template < class TBGImageSO,  class TFGImageSO  >
void
ImageSliceRepresentationPlus < TBGImageSO,  TFGImageSO  >
::SetFGImageLUT( vtkLookupTable * lut )
{
  m_FGImageLUT = lut;
  m_FGImageMapper->SetLookupTable( m_FGImageLUT );
}

// set the reslice transform matrix
template < class TBGImageSO,  class TFGImageSO  >
void
ImageSliceRepresentationPlus < TBGImageSO,  TFGImageSO  >
::SetResliceTransform( igstk::Transform t )
{
  m_ResliceTransform = t;
  m_Transform = igstk::Transform::TransformCompose(m_ResliceTransform, m_CalibrationTransform);
  m_ResliceCenter = m_Transform.GetTranslation();
  m_NeedleVector = m_Transform.GetRotation().Transform(m_PrincipalAxes);
  m_NeedleVector += m_Transform.GetTranslation();
}

template < class TBGImageSO,  class TFGImageSO  >
void
ImageSliceRepresentationPlus < TBGImageSO,  TFGImageSO  >
::SetCalibrationTransform( igstk::Transform t )
{
  m_ResliceTransform = t;
  m_Transform = igstk::Transform::TransformCompose(m_ResliceTransform, m_CalibrationTransform);
  m_ResliceCenter = m_Transform.GetTranslation();
  m_NeedleVector = m_Transform.GetRotation().Transform(m_PrincipalAxes);
  m_NeedleVector += m_Transform.GetTranslation();
}

template < class TBGImageSO,  class TFGImageSO  >
void
ImageSliceRepresentationPlus < TBGImageSO,  TFGImageSO  >
::SetPrincipalAxes( igstk::Transform::VectorType pAxes )
{
  m_PrincipalAxes = pAxes;
  m_ResliceCenter = m_Transform.GetTranslation();
  m_NeedleVector = m_Transform.GetRotation().Transform(m_PrincipalAxes);
  m_NeedleVector += m_Transform.GetTranslation();
}

template < class TBGImageSO,  class TFGImageSO  >
void
ImageSliceRepresentationPlus < TBGImageSO,  TFGImageSO  >
::SetBGOpacity( double o )
{
  m_BGOpacity = o;
  if(m_FGImageSO)
    {
    m_BGActor->SetOpacity( m_BGOpacity );
    m_FGActor->SetOpacity( 1 - m_BGOpacity );
    }
  else
    {
    m_BGActor->SetOpacity( 1 );
    }
}

template < class TBGImageSO,  class TFGImageSO  >
void
ImageSliceRepresentationPlus < TBGImageSO,  TFGImageSO  >
::ComputeOrthogonalResliceAxes( )
{
  itk::Vector< double, 3 >       vx, vy, vz, vn;
  vtkMatrix4x4 * t = vtkMatrix4x4::New();
  igstk::Transform::VectorType center;
  center = m_ImageCenter;

  m_Transform.ExportTransform( * t );
  
  if(( m_Orientation == Axial) || (m_Orientation == Perpendicular) )
    {  
    for ( int i = 0; i < 3; i++ )
      {
      vx[i] = t->GetElement(i, 0); // +X
      vy[i] = t->GetElement(i, 1); // +Y
      vn[i] = t->GetElement(i, 2); // +Z
      }
    center[2] = m_ResliceCenter[2];
    m_SizeX = m_Size[0];
    m_SizeY = m_Size[1];
    }
  else if( m_Orientation == Sagittal)
    {
     for ( int i = 0; i < 3; i++ )
      {
      vx[i] = - t->GetElement(i, 1); // -Y
      vy[i] = t->GetElement(i, 2);   // +Z
      vn[i] = - t->GetElement(i, 0); // -X
      }
    center[0] = m_ResliceCenter[0];
    m_SizeX = m_Size[1];
    m_SizeY = m_Size[2];
    }
  else if( m_Orientation == Coronal)
    {
    for ( int i = 0; i < 3; i++ )
      {
      vx[i] = t->GetElement(i, 0);   // +X
      vy[i] = t->GetElement(i, 2);   // +Z
      vn[i] = - t->GetElement(i, 1); // -Y
      }
    center[1] = m_ResliceCenter[1];
    m_SizeX = m_Size[0];
    m_SizeY = m_Size[2];
    }
 
  t->Delete();
  vx.Normalize();
  vy.Normalize();
  vn.Normalize();

  m_ResliceAxes->Identity();
  for ( int i = 0; i < 3; i++ )
    {
    m_ResliceAxes->SetElement(i, 0, vx[i] );
    m_ResliceAxes->SetElement(i, 1, vy[i] );
    m_ResliceAxes->SetElement(i, 2, vn[i] );
    m_ResliceAxes->SetElement(i, 3, center[i]);
    }

}

template < class TBGImageSO,  class TFGImageSO  >
void
ImageSliceRepresentationPlus < TBGImageSO,  TFGImageSO  >
::ComputeOffOrthogonalResliceAxes( )
{
  itk::Vector< double, 3 >       vx, vy, vn, v;
  vtkMatrix4x4 * t = vtkMatrix4x4::New();;
  m_Transform.ExportTransform( * t );
 
    if( m_Orientation == OffAxial)
    {
    // Calculate the reslice axes
    vx.Fill( 0.0 );
    vx[0] = 1;
    
    vy = m_NeedleVector;
    
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

    }
  else if( m_Orientation == OffSagittal)
    {
    // Calculate the reslice axes
    vy.Fill( 0.0 );
    vy[2] = -1;

    vx = m_NeedleVector;

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
  }
  else if ( m_Orientation == HybridPerpendicular || m_Orientation == OffPerpendicular)
    {
        // Calculate the reslice axes
    vx.Fill( 0.0 );
    if( m_SurgeonPosition == PatientLeft )
      {
      vx[2] = -1;
      }
    else if ( m_SurgeonPosition == PatientRight )
      {
      vx[2] = 1;
      }
    if (m_Orientation == OffPerpendicular)
      {
      vn = m_NeedleVector;
      }
    else
      {
      vn = m_PathVector;
      }

    if ( fabs(vx*vn) < 1e-9 )
      {
      // FIXME: need to handle this situation too
      igstkLogMacro( DEBUG, "The two vectors are parrelell \n");
      }

    vy = itk::CrossProduct( vn, vx );
    vx = itk::CrossProduct( vy, vn );

    v.Fill( 0.0 );
    //v[2] = -1;
    if( m_SurgeonPosition ==PatientLeft )
      {
      v[2] = -1;
      }
    else if ( m_SurgeonPosition == PatientRight)
      {
      v[2] = 1;
      }
    if (vx*v<0)
      {
      vx *= -1;
      }

    v.Fill(0.0);
    //v[1] = 1;

    if( m_SurgeonPosition ==PatientLeft )
      {
      v[0] = -1;
      }
    else if ( m_SurgeonPosition == PatientRight)
      {
      v[0] = 1;
      }

    if (vy*v<0)
      {
      vy *= -1;
      }
    }
  
  t->Delete();
  vx.Normalize();
  vy.Normalize();
  vn.Normalize();

  m_ResliceAxes->Identity();
  for ( int i = 0; i < 3; i++ )
    {
    m_ResliceAxes->SetElement(i, 0, vx[i] );
    m_ResliceAxes->SetElement(i, 1, vy[i] );
    m_ResliceAxes->SetElement(i, 2, vn[i] );
    m_ResliceAxes->SetElement(i, 3, m_ResliceCenter[i]);
    }

  //Setting up the camera position
  double focalPoint[3];
  double position[3];
  for ( int i = 0; i<3; i++ )
    {
    focalPoint[i] = m_ResliceCenter[i];
    position[i] = m_ResliceCenter[i];
    }

  for ( int i = 0; i<3; i++ )
    {
    position[i] -= m_CameraDistance * vn[i];
    }

  m_Camera->SetViewUp( -vy[0], -vy[1], -vy[2] );
  m_Camera->SetFocalPoint( focalPoint );
  //m_Camera->SetParallelScale( 0.8 );
  //m_Camera->SetZoomFactor( 2 );
  m_Camera->SetPosition( position );

}

template < class TBGImageSO,  class TFGImageSO  >
vtkCamera *
ImageSliceRepresentationPlus < TBGImageSO,  TFGImageSO  >
::RequestReslice( )
{
  igstkLogMacro( DEBUG,
           "igstk::ImageSliceRepresentationPlus::ResliceProcessing called...\n");

  if( ( m_Orientation == Axial) ||
      ( m_Orientation == Sagittal) ||
      ( m_Orientation == Coronal) ||
      ( m_Orientation == Perpendicular)
    )
    {
    this->ComputeOrthogonalResliceAxes();
    }
    else
    {
    this->ComputeOffOrthogonalResliceAxes();
    }

  //m_BGImageReslice->SetInput ( m_BGImageData );
  m_BGImageReslice->SetResliceAxes( m_ResliceAxes );
  m_BGImageReslice->SetOutputOrigin( -m_SizeX/2, -m_SizeY/2, 0 );
  m_BGImageReslice->SetOutputExtent( 1, int(m_SizeX+0.5),
                                   1, int(m_SizeY+0.5),
                                   0, 0 );
  m_BGActor->SetUserMatrix(m_ResliceAxes);

  if(m_FGImageSO)
    {
    m_TransformObserver->Clear();
    m_BGImageSO->RequestComputeTransformTo(m_FGImageSO);
    if (m_TransformObserver->GotTransform())
      {
      vtkMatrix4x4 * reslice = vtkMatrix4x4::New();
      m_TransformObserver->GetTransform().ExportTransform(* reslice);
      vtkMatrix4x4::Multiply4x4(reslice, m_ResliceAxes, reslice);
      m_FGImageReslice->SetResliceAxes( reslice );
      m_FGImageReslice->SetOutputOrigin( -m_SizeX/2, -m_SizeY/2, 0 );
      m_FGImageReslice->SetOutputExtent( 1, int(m_SizeX+0.5),
                                   1, int(m_SizeY+0.5),
                                   0, 0 );
      m_FGActor->SetUserMatrix(m_ResliceAxes);
      }
    }


  return m_Camera;

}


/** Print Self function */
template < class TBGImageSO,  class TFGImageSO  >
void
ImageSliceRepresentationPlus < TBGImageSO,  TFGImageSO  >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << m_Orientation << "\n";
  os << indent << m_Transform << "\n";
}


/** Update the visual representation in response to changes in the geometric
 * object */
template < class TBGImageSO,  class TFGImageSO  >
void
ImageSliceRepresentationPlus < TBGImageSO,  TFGImageSO  >
::UpdateRepresentationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ImageSliceRepresentationPlus::\
                         UpdateRepresentationProcessing called...\n");
  //if( m_BGImageReslice->GetOutput() )
  //  {
    //ADDCODE
  //  m_MapColors->SetInput( m_BGImageReslice->GetOutput() );
  //  }
}


/** Create the vtk Actors */
template < class TBGImageSO,  class TFGImageSO  >
void
ImageSliceRepresentationPlus < TBGImageSO,  TFGImageSO  >
::CreateActors()
{
  igstkLogMacro( DEBUG, "igstk::ImageSliceRepresentationPlus\
                        ::CreateActors called...\n");

  // to avoid duplicates we clean the previous actors
  this->DeleteActors();

  m_BGImageReslice->SetInput ( m_BGImageData );
  m_BGImageReslice->SetResliceAxes( m_ResliceAxes );
  m_BGImageMapper->SetLookupTable( m_BGImageLUT );
  m_BGImageMapper->SetInput( m_BGImageReslice->GetOutput() );
  m_BGActor = vtkImageActor::New();
  m_BGActor->SetInput( m_BGImageMapper->GetOutput() );
  m_BGActor->SetUserMatrix( m_ResliceAxes );
  m_BGActor->SetOpacity( m_BGOpacity );
  this->AddActor( m_BGActor );

  if(m_FGImageSO)
    {
    m_FGImageReslice->SetInput ( m_FGImageData );
    m_FGImageReslice->SetResliceAxes( m_ResliceAxes );
    m_FGImageMapper->SetLookupTable( m_FGImageLUT );
    m_FGImageMapper->SetInput( m_FGImageReslice->GetOutput() );
    m_FGActor = vtkImageActor::New();
    m_FGActor->SetInput( m_FGImageMapper->GetOutput() );
    m_FGActor->SetUserMatrix( m_ResliceAxes );
    m_FGActor->SetOpacity( 1-m_BGOpacity );
    this->AddActor( m_FGActor );
    }

  //this->RequestReslice();
}

/** Print Self function */
template < class TBGImageSO,  class TFGImageSO  >
void
ImageSliceRepresentationPlus < TBGImageSO,  TFGImageSO  >
::SetSliceSize( double size )
{
  m_SizeX = size;
  m_SizeY = size;
  m_SliceSize = size;
}


} // end namespace igstk

#endif
