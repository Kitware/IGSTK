/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageReslicePlaneSpatialObject.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkImageReslicePlaneSpatialObject_txx
#define __igstkImageReslicePlaneSpatialObject_txx

#include "igstkImageReslicePlaneSpatialObject.h"
#include "igstkEvents.h"

#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkPlaneSource.h"
#include "vtkMath.h"
#include "vtkTransform.h"


static const double dist0 = 200;

namespace igstk
{ 

/** Constructor */
template < class TImageSpatialObject >
ImageReslicePlaneSpatialObject< TImageSpatialObject>
::ImageReslicePlaneSpatialObject():m_StateMachine(this)
{
  //Default reslicing mode
  m_ReslicingMode = Orthogonal;

  //Default orientation type
  m_OrientationType = Axial;

  m_ImageData = NULL;
  m_ImageSpatialObject = NULL;
  m_ToolSpatialObject = NULL; 

  //Create vtk plane 
  m_PlaneSource = vtkPlaneSource::New();
  m_PlaneSource->SetOrigin(0,0,0);
  m_PlaneSource->SetCenter(0,0,0);

  //Create reslice axes matrix
  m_ResliceAxes = vtkMatrix4x4::New();

  //Create reslice transform
  //m_ResliceTransform = vtkTransform::New();
  //m_ResliceTransform->Identity();

  // Create the observer to VTK image events 
  m_VTKImageObserver = VTKImageObserver::New();
  //m_ImageTransformObserver = ImageTransformObserver::New();

  //slice number
  m_SliceNumber = 0;
  m_SliceNumberSetFlag = false;

  //tool spatial object check flag
  m_ToolSpatialObjectSet  = false;

  //mouse position 
  m_MousePosition[0] = 0;
  m_MousePosition[1] = 0;
  m_MousePosition[2] = 0;

  //tool position 
  m_ToolPosition[0] = 0;
  m_ToolPosition[1] = 0;
  m_ToolPosition[2] = 0;

  m_MousePositionSetFlag = false;

  //List of states
  igstkAddStateMacro( Initial );
  igstkAddStateMacro( ReslicingModeSet );
  igstkAddStateMacro( OrientationTypeSet );
  igstkAddStateMacro( ImageSpatialObjectSet );
  igstkAddStateMacro( ToolSpatialObjectSet );
  igstkAddStateMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem );
  igstkAddStateMacro( AttemptingToSetSliceNumber );
  igstkAddStateMacro( ValidSliceNumberSet);
  igstkAddStateMacro( AttemptingToSetMousePosition );
  igstkAddStateMacro( ValidMousePositionSet);

  // List of state machine inputs
  igstkAddInputMacro( ValidReslicingMode );
  igstkAddInputMacro( InValidReslicingMode );
  igstkAddInputMacro( ValidOrientationType );
  igstkAddInputMacro( InValidOrientationType );
  igstkAddInputMacro( ValidImageSpatialObject );
  igstkAddInputMacro( InValidImageSpatialObject );
  igstkAddInputMacro( ValidToolSpatialObject );
  igstkAddInputMacro( InValidToolSpatialObject );
  igstkAddInputMacro( SetSliceNumber );
  igstkAddInputMacro( ValidSliceNumber );
  igstkAddInputMacro( InValidSliceNumber );
  igstkAddInputMacro( SetMousePosition );
  igstkAddInputMacro( ValidMousePosition );
  igstkAddInputMacro( InValidMousePosition );
  igstkAddInputMacro( GetSliceNumberBounds );
  igstkAddInputMacro( GetToolPosition );
  igstkAddInputMacro( GetImageBounds );
  igstkAddInputMacro( GetVTKPlane );
  igstkAddInputMacro( GetToolTransformWRTImageCoordinateSystem );
  igstkAddInputMacro( ToolTransformWRTImageCoordinateSystem );
  igstkAddInputMacro( ComputeReslicePlane );

  // List of state machine transitions

  //From Initial
  igstkAddTransitionMacro( Initial, ValidReslicingMode, ReslicingModeSet, SetReslicingMode );
  igstkAddTransitionMacro( Initial, InValidReslicingMode, Initial, ReportInvalidReslicingMode );
  igstkAddTransitionMacro( Initial, ValidOrientationType, Initial, ReportInvalidRequest);
  igstkAddTransitionMacro( Initial, InValidOrientationType, Initial, ReportInvalidRequest);
  igstkAddTransitionMacro( Initial, SetSliceNumber, Initial, ReportInvalidRequest);
  igstkAddTransitionMacro( Initial, SetMousePosition, Initial, ReportInvalidRequest);

  //From ReslicingModeSet
  igstkAddTransitionMacro( ReslicingModeSet, ValidOrientationType, OrientationTypeSet, SetOrientationType );
  igstkAddTransitionMacro( ReslicingModeSet, InValidOrientationType, ReslicingModeSet, ReportInvalidOrientationType);
  igstkAddTransitionMacro( ReslicingModeSet, SetSliceNumber, ReslicingModeSet, ReportInvalidRequest );  
  igstkAddTransitionMacro( ReslicingModeSet, SetMousePosition, ReslicingModeSet, ReportInvalidRequest );  

  //From OrientationTypeSet
  igstkAddTransitionMacro( OrientationTypeSet, ValidImageSpatialObject,
                           ImageSpatialObjectSet, SetImageSpatialObject );

  igstkAddTransitionMacro( OrientationTypeSet, InValidImageSpatialObject, 
                           OrientationTypeSet, ReportInvalidImageSpatialObject );

  igstkAddTransitionMacro( OrientationTypeSet, SetSliceNumber,
                           OrientationTypeSet, ReportInvalidRequest );  

  igstkAddTransitionMacro( OrientationTypeSet, SetMousePosition,
                           OrientationTypeSet, ReportInvalidRequest );  


  //From ImageSpatialObjectSet
  igstkAddTransitionMacro( ImageSpatialObjectSet,
                           ValidOrientationType,
                           ImageSpatialObjectSet,
                           SetOrientationType );

  igstkAddTransitionMacro( ImageSpatialObjectSet,
                           InValidOrientationType,
                           ImageSpatialObjectSet,
                           ReportInvalidOrientationType );

  igstkAddTransitionMacro( ImageSpatialObjectSet, 
                           ValidToolSpatialObject,
                           ToolSpatialObjectSet, 
                           SetToolSpatialObject );

  igstkAddTransitionMacro( ImageSpatialObjectSet,
                           InValidToolSpatialObject,
                           ImageSpatialObjectSet,
                           ReportInvalidToolSpatialObject );
  
  igstkAddTransitionMacro( ImageSpatialObjectSet, SetSliceNumber,
                           AttemptingToSetSliceNumber, AttemptSetSliceNumber );  

  igstkAddTransitionMacro( ImageSpatialObjectSet, SetMousePosition,
                           AttemptingToSetMousePosition, AttemptSetMousePosition );  

  igstkAddTransitionMacro( ImageSpatialObjectSet, GetSliceNumberBounds,
                           ImageSpatialObjectSet, ReportSliceNumberBounds );

  igstkAddTransitionMacro( ImageSpatialObjectSet, GetImageBounds,
                           ImageSpatialObjectSet, ReportImageBounds );

  igstkAddTransitionMacro( ImageSpatialObjectSet, GetToolPosition,
                           ImageSpatialObjectSet, ReportToolPosition );

  igstkAddTransitionMacro( ImageSpatialObjectSet, GetVTKPlane,
                           ImageSpatialObjectSet, ReportVTKPlane );

  // From AttemptingToSetSliceNumber
  igstkAddTransitionMacro( AttemptingToSetSliceNumber, ValidSliceNumber,
                           ValidSliceNumberSet,  SetSliceNumber ); 

  igstkAddTransitionMacro( AttemptingToSetSliceNumber, InValidSliceNumber,
                           ImageSpatialObjectSet,  ReportInvalidSliceNumber ); 

  // From AttemptingToSetMousePosition
  igstkAddTransitionMacro( AttemptingToSetMousePosition, ValidMousePosition,
                           ValidMousePositionSet,  SetMousePosition ); 

  igstkAddTransitionMacro( AttemptingToSetMousePosition, InValidMousePosition,
                           ImageSpatialObjectSet,  ReportInvalidMousePosition ); 


  // From ValidSliceNumberSet
  igstkAddTransitionMacro( ValidSliceNumberSet,
                           ComputeReslicePlane,
                           ValidSliceNumberSet,
                           ComputeReslicePlane );

  igstkAddTransitionMacro( ValidSliceNumberSet,
                           SetSliceNumber,
                           AttemptingToSetSliceNumber,
                           AttemptSetSliceNumber );

  igstkAddTransitionMacro( ValidSliceNumberSet,
                           SetMousePosition,
                           AttemptingToSetMousePosition,
                           AttemptSetMousePosition );

  igstkAddTransitionMacro( ValidSliceNumberSet,
                           ValidOrientationType,
                           ValidSliceNumberSet,
                           SetOrientationType );

  igstkAddTransitionMacro( ValidSliceNumberSet,
                           InValidOrientationType,
                           ValidSliceNumberSet,
                           ReportInvalidOrientationType );

  igstkAddTransitionMacro( ValidSliceNumberSet,
                           GetToolTransformWRTImageCoordinateSystem,
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           RequestGetToolTransformWRTImageCoordinateSystem );

  igstkAddTransitionMacro( ValidSliceNumberSet, 
                           ValidToolSpatialObject,
                           ToolSpatialObjectSet, 
                           SetToolSpatialObject );

  igstkAddTransitionMacro( ValidSliceNumberSet,
                           InValidToolSpatialObject,
                           ValidSliceNumberSet,
                           ReportInvalidToolSpatialObject );

  igstkAddTransitionMacro( ValidSliceNumberSet, GetImageBounds,
                           ValidSliceNumberSet, ReportImageBounds );

  igstkAddTransitionMacro( ValidSliceNumberSet, GetToolPosition,
                           ValidSliceNumberSet, ReportToolPosition );

  igstkAddTransitionMacro( ValidSliceNumberSet, GetVTKPlane,
                           ValidSliceNumberSet, ReportVTKPlane );

  // From ValidMousePositionSet
  igstkAddTransitionMacro( ValidMousePositionSet,
                           ComputeReslicePlane,
                           ValidMousePositionSet,
                           ComputeReslicePlane );

  igstkAddTransitionMacro( ValidMousePositionSet,
                           SetSliceNumber,
                           AttemptingToSetSliceNumber,
                           AttemptSetSliceNumber );

  igstkAddTransitionMacro( ValidMousePositionSet,
                           SetMousePosition,
                           AttemptingToSetMousePosition,
                           AttemptSetMousePosition );

  igstkAddTransitionMacro( ValidMousePositionSet,
                           ValidOrientationType,
                           ValidMousePositionSet,
                           SetOrientationType );

  igstkAddTransitionMacro( ValidMousePositionSet,
                           InValidOrientationType,
                           ValidMousePositionSet,
                           ReportInvalidOrientationType );

  igstkAddTransitionMacro( ValidMousePositionSet,
                           GetToolTransformWRTImageCoordinateSystem,
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           RequestGetToolTransformWRTImageCoordinateSystem );

  igstkAddTransitionMacro( ValidMousePositionSet, GetImageBounds,
                           ValidMousePositionSet, ReportImageBounds );

  igstkAddTransitionMacro( ValidMousePositionSet, GetToolPosition,
                           ValidMousePositionSet, ReportToolPosition );

  igstkAddTransitionMacro( ValidMousePositionSet, GetVTKPlane,
                           ValidMousePositionSet, ReportVTKPlane );

  igstkAddTransitionMacro( ValidMousePositionSet, 
                           ValidToolSpatialObject,
                           ToolSpatialObjectSet, 
                           SetToolSpatialObject );

  igstkAddTransitionMacro( ValidMousePositionSet,
                           InValidToolSpatialObject,
                           ValidMousePositionSet,
                           ReportInvalidToolSpatialObject );

  //From ToolSpatialObjectSet

  igstkAddTransitionMacro( ToolSpatialObjectSet, 
                           ValidReslicingMode, 
                           ToolSpatialObjectSet, 
                           SetReslicingMode );

  igstkAddTransitionMacro( ToolSpatialObjectSet,
                           ValidOrientationType,
                           ToolSpatialObjectSet,
                           SetOrientationType );

  igstkAddTransitionMacro( ToolSpatialObjectSet,
                           InValidOrientationType,
                           ToolSpatialObjectSet,
                           ReportInvalidOrientationType );

  igstkAddTransitionMacro( ToolSpatialObjectSet,
                           GetToolTransformWRTImageCoordinateSystem,
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           RequestGetToolTransformWRTImageCoordinateSystem );

  igstkAddTransitionMacro( ToolSpatialObjectSet,
                           ComputeReslicePlane,
                           ToolSpatialObjectSet,
                           ComputeReslicePlane );

  igstkAddTransitionMacro( ToolSpatialObjectSet, SetSliceNumber,
                           AttemptingToSetSliceNumber, AttemptSetSliceNumber );  

  igstkAddTransitionMacro( ToolSpatialObjectSet, SetMousePosition,
                           AttemptingToSetMousePosition, AttemptSetMousePosition );  

  igstkAddTransitionMacro( ToolSpatialObjectSet, GetSliceNumberBounds,
                           ToolSpatialObjectSet, ReportSliceNumberBounds );

  igstkAddTransitionMacro( ToolSpatialObjectSet, GetImageBounds,
                           ToolSpatialObjectSet, ReportImageBounds );

  igstkAddTransitionMacro( ToolSpatialObjectSet, GetToolPosition,
                           ToolSpatialObjectSet, ReportToolPosition );

  igstkAddTransitionMacro( ToolSpatialObjectSet, GetVTKPlane,
                           ToolSpatialObjectSet, ReportVTKPlane );

  // From AttemptingToGetToolTransformWRTImageCoordinateSystem
  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ToolTransformWRTImageCoordinateSystem,
                           ToolSpatialObjectSet,
                           ReceiveToolTransformWRTImageCoordinateSystem );
 
  igstkSetInitialStateMacro( Initial );
  this->m_StateMachine.SetReadyToRun();
} 

/** Destructor */
template < class TImageSpatialObject >
ImageReslicePlaneSpatialObject<TImageSpatialObject>::~ImageReslicePlaneSpatialObject()  
{
  //if( ! m_ResliceAxes )
  //  {
  //  m_ResliceAxes->Delete();
  //  m_ResliceAxes = NULL;
  //  }

  //if( ! m_ResliceTransform )
  //  {
  //  m_ResliceTransform->Delete();
  //  m_ResliceTransform = NULL;
  //  }
  

  if( ! m_PlaneSource )
    {
    m_PlaneSource->Delete();
    m_PlaneSource = NULL;
    }
}


template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject<TImageSpatialObject>
::RequestSetReslicingMode( ReslicingMode reslicingMode )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::RequestSetReslicingMode called...\n");

  m_ReslicingModeToBeSet = reslicingMode;
  m_StateMachine.PushInput( m_ValidReslicingModeInput );

  //FIXME: Check conditions for InValidReslicing mode
  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject<TImageSpatialObject>
::RequestSetSliceNumber( SliceNumberType sliceNumber )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::RequestSetSliceNumber called...\n");

  m_SliceNumberToBeSet = sliceNumber;
  m_StateMachine.PushInput( m_SetSliceNumberInput );

  m_StateMachine.ProcessInputs();
}

/** Null Operation for a State Machine Transition */
template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::NoProcessing()
{
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::AttemptSetSliceNumberProcessing()
{

  igstkLogMacro( DEBUG, "igstk::ImageReslicePlaneSpatialObject\
                        ::AttemptSetSliceNumberProcessing called...\n");
  if( m_ImageData )
    {

    SliceNumberType minSlice = 0;
    SliceNumberType maxSlice = 0;
    
    switch( m_OrientationType )
      {
      case Axial:
        minSlice = m_ImageExtent[4];
        maxSlice = m_ImageExtent[5];
        break;
      case Sagittal:
        minSlice = m_ImageExtent[0];
        maxSlice = m_ImageExtent[1];
        break;
      case Coronal:
        minSlice = m_ImageExtent[2];
        maxSlice = m_ImageExtent[3];
        break;
      }

    if( m_SliceNumberToBeSet >= minSlice && m_SliceNumberToBeSet <= maxSlice )
      {
      igstkPushInputMacro( ValidSliceNumber );
      }
    else
      {
      igstkPushInputMacro( InValidSliceNumber );
      }

    m_StateMachine.ProcessInputs();
    }
}


template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::RequestGetImageBounds() 
{
  igstkLogMacro( DEBUG, "igstk::ImageReslicePlaneSpatialObject\
                        ::RequestGetImageBounds called...\n");
 
  igstkPushInputMacro( GetImageBounds );
  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::RequestGetToolPosition() 
{
  igstkLogMacro( DEBUG, "igstk::ImageReslicePlaneSpatialObject\
                        ::RequestGetToolPosition called...\n");
 
  igstkPushInputMacro( GetToolPosition );
  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::RequestGetVTKPlane() 
{
  igstkLogMacro( DEBUG, "igstk::ImageReslicePlaneSpatialObject\
                        ::RequestGetVTKPlane called...\n");
 
  igstkPushInputMacro( GetVTKPlane );
  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ReportToolPositionProcessing() 
{
  igstkLogMacro( DEBUG, "igstk::ImageReslicePlaneSpatialObject\
                        ::ReportToolPositionProcessing called...\n");

  EventHelperType::PointType position;
   
  position[0] = m_ToolPosition[0];
  position[1] = m_ToolPosition[1];
  position[2] = m_ToolPosition[2];

  PointEvent event;
  event.Set( position );
  this->InvokeEvent( event );
}

template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ReportImageBoundsProcessing() 
{
  igstkLogMacro( DEBUG, "igstk::ImageReslicePlaneSpatialObject\
                        ::ReportImageBoundsProcessing called...\n");
 
  m_ImageData->Update();

  EventHelperType::ImageBoundsType bounds;

  bounds.xmin = m_ImageBounds[0];
  bounds.xmax = m_ImageBounds[1];
  bounds.ymin = m_ImageBounds[2];
  bounds.ymax = m_ImageBounds[3];
  bounds.zmin = m_ImageBounds[4];
  bounds.zmax = m_ImageBounds[5];

  ImageBoundsEvent event;
  event.Set( bounds );
  this->InvokeEvent( event );
}

template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ReportVTKPlaneProcessing() 
{
  igstkLogMacro( DEBUG, "igstk::ImageReslicePlaneSpatialObject\
                        ::ReportVTKPlaneProcessing called...\n");

  double abs_normal[3];
  m_PlaneSource->GetNormal(abs_normal);

  double nmax = 0.0;
  int k = 0;

  for (int i = 0; i < 3; i++ )
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
  if ( m_PlaneCenter[k] > m_ImageBounds[2*k+1] )
  {
     m_PlaneCenter[k] = m_ImageBounds[2*k+1];
  }
  else if ( m_PlaneCenter[k] < m_ImageBounds[2*k] )
  {
     m_PlaneCenter[k] = m_ImageBounds[2*k];
  }

  m_PlaneSource->SetOrigin(m_PlaneOrigin[0],m_PlaneOrigin[1],m_PlaneOrigin[2]);
  m_PlaneSource->SetPoint1(m_PlanePoint1[0],m_PlanePoint1[1],m_PlanePoint1[2]);
  m_PlaneSource->SetPoint2(m_PlanePoint2[0],m_PlanePoint2[1],m_PlanePoint2[2]);
  m_PlaneSource->SetNormal(m_PlaneNormal[0],m_PlaneNormal[1],m_PlaneNormal[2]);
  m_PlaneSource->SetCenter(m_PlaneCenter[0],m_PlaneCenter[1],m_PlaneCenter[2]);

  VTKPlaneModifiedEvent event;
  event.Set( m_PlaneSource );
  this->InvokeEvent( event );
}


template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::RequestGetSliceNumberBounds() 
{
  igstkLogMacro( DEBUG, "igstk::ImageReslicePlaneSpatialObject\
                        ::RequestGetSliceNumberBounds called...\n");
 
  igstkPushInputMacro( GetSliceNumberBounds );
  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ReportSliceNumberBoundsProcessing() 
{
  igstkLogMacro( DEBUG, "igstk::ImageReslicePlaneSpatialObject\
                        ::ReportSliceNumberBoundsProcessing called...\n");

  EventHelperType::IntegerBoundsType bounds;

  switch( m_OrientationType )
    {
    case Axial:
      {
      bounds.minimum = m_ImageExtent[4];
      bounds.maximum = m_ImageExtent[5];
      AxialSliceBoundsEvent event;
      event.Set( bounds );
      this->InvokeEvent( event );
      break;
      }
    case Sagittal:
      {
      bounds.minimum = m_ImageExtent[0];
      bounds.maximum = m_ImageExtent[1];
      SagittalSliceBoundsEvent event;
      event.Set( bounds );
      this->InvokeEvent( event );
      break;
      }
    case Coronal:
      {
      bounds.minimum = m_ImageExtent[2];
      bounds.maximum = m_ImageExtent[3];
      CoronalSliceBoundsEvent event;
      event.Set( bounds );
      this->InvokeEvent( event );
      break;
      }
    }
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::SetSliceNumberProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ImageReslicePlaneSpatialObject\
                        ::SetSliceNumber called...\n");

  m_SliceNumber = m_SliceNumberToBeSet;
  
  //turn on the flag
  m_SliceNumberSetFlag = true;
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject<TImageSpatialObject>
::RequestSetMousePosition( PointType point )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::RequestSetMousePosition called...\n");

  m_MousePositionToBeSet[0] = point[0];
  m_MousePositionToBeSet[1] = point[1];
  m_MousePositionToBeSet[2] = point[2];

  m_StateMachine.PushInput( m_SetMousePositionInput );

  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::AttemptSetMousePositionProcessing()
{

  igstkLogMacro( DEBUG, "igstk::ImageReslicePlaneSpatialObject\
                        ::AttemptSetMousePositionProcessing called...\n");

    bool validPosition = false; 

    switch( m_OrientationType )
      {
      case Axial:
        if( m_MousePositionToBeSet[2] >= m_ImageBounds[4] && 
            m_MousePositionToBeSet[2] <= m_ImageBounds[5] )
          {
          validPosition = true;
          }
          break;
      case Sagittal:
        if( m_MousePositionToBeSet[0] >= m_ImageBounds[0] && 
            m_MousePositionToBeSet[0] <= m_ImageBounds[1] )
          {
          validPosition = true;
          }
        break;
      case Coronal:
        if( m_MousePositionToBeSet[1] >= m_ImageBounds[2] && 
            m_MousePositionToBeSet[1] <= m_ImageBounds[3] )
          {
          validPosition = true;
          }
        break;
      }

    if( validPosition )
      {
      igstkPushInputMacro( ValidMousePosition );
      }
    else
      {
      igstkPushInputMacro( InValidMousePosition );
      }

    m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::SetMousePositionProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ImageReslicePlaneSpatialObject\
                        ::SetMousePosition called...\n");

  m_MousePosition[0] = m_MousePositionToBeSet[0];
  m_MousePosition[1] = m_MousePositionToBeSet[1];
  m_MousePosition[2] = m_MousePositionToBeSet[2];

  m_ToolPosition[0] = m_MousePosition[0];
  m_ToolPosition[1] = m_MousePosition[1];
  m_ToolPosition[2] = m_MousePosition[2];

  //turn on the flag
  m_MousePositionSetFlag = true;
}



template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject<TImageSpatialObject>
::SetReslicingModeProcessing()
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::SetReslicingModeProcessing called...\n");
  m_ReslicingMode = m_ReslicingModeToBeSet;
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject<TImageSpatialObject>
::RequestSetOrientationType( OrientationType orientationType )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::RequestSetOrientationType called...\n");

  m_OrientationTypeToBeSet = orientationType;

  bool validOrientation = true;

  if( m_ReslicingMode == Orthogonal )
    {
    if( m_OrientationTypeToBeSet == OffAxial ||
         m_OrientationTypeToBeSet == OffCoronal ||
         m_OrientationTypeToBeSet == OffSagittal  ||
         m_OrientationTypeToBeSet == PlaneOrientationWithZAxesNormal ||
         m_OrientationTypeToBeSet == PlaneOrientationWithXAxesNormal ||
         m_OrientationTypeToBeSet == PlaneOrientationWithYAxesNormal )
      {
      validOrientation = false;
      }
    }

  if( m_ReslicingMode == Oblique )
    {
    if( m_OrientationTypeToBeSet  == OffAxial ||
         m_OrientationTypeToBeSet == OffCoronal ||
         m_OrientationTypeToBeSet == OffSagittal  ||
         m_OrientationTypeToBeSet == Axial ||
         m_OrientationTypeToBeSet == Sagittal ||
         m_OrientationTypeToBeSet == Coronal )
      {
      validOrientation = false;
      }
    }

  if( m_ReslicingMode == OffOrthogonal )
    {
    if( m_OrientationTypeToBeSet  == PlaneOrientationWithZAxesNormal ||
         m_OrientationTypeToBeSet == PlaneOrientationWithXAxesNormal ||
         m_OrientationTypeToBeSet == PlaneOrientationWithYAxesNormal  ||
         m_OrientationTypeToBeSet == Axial ||
         m_OrientationTypeToBeSet == Sagittal ||
         m_OrientationTypeToBeSet == Coronal )
      {
      validOrientation = false;
      }
    }
       
  if ( validOrientation )
    {
    m_StateMachine.PushInput( m_ValidOrientationTypeInput );
    }
  else
    {
    m_StateMachine.PushInput( m_InValidOrientationTypeInput );
    }

  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::SetOrientationTypeProcessing()
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::SetOrientationTypeProcessing called...\n");
  m_OrientationType = m_OrientationTypeToBeSet;
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::RequestSetImageSpatialObject( const ImageSpatialObjectType * imageSpatialObject )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::RequestSetImageSpatialObject called...\n");

  m_ImageSpatialObjectToBeSet = imageSpatialObject;

  if( !m_ImageSpatialObjectToBeSet )
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
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::SetImageSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::SetImageSpatialObjectProcessing called...\n");

  m_ImageSpatialObject = m_ImageSpatialObjectToBeSet;

  m_ImageSpatialObject->AddObserver( VTKImageModifiedEvent(), 
                                     m_VTKImageObserver );

  this->m_VTKImageObserver->Reset();

  this->m_ImageSpatialObject->RequestGetVTKImage();

  if( this->m_VTKImageObserver->GotVTKImage() ) 
    {
      m_ImageData = this->m_VTKImageObserver->GetVTKImage();

      // get image parameters
      m_ImageData->GetDimensions( m_ImageDimension );
      m_ImageData->GetOrigin( m_ImageOrigin );
      m_ImageData->GetSpacing( m_ImageSpacing );
      m_ImageData->GetWholeExtent( m_ImageExtent );

      m_ImageBounds[0] = m_ImageOrigin[0] + m_ImageSpacing[0]*m_ImageExtent[0]; //xmin
      m_ImageBounds[1] = m_ImageOrigin[0] + m_ImageSpacing[0]*m_ImageExtent[1]; //xmax
      m_ImageBounds[2] = m_ImageOrigin[1] + m_ImageSpacing[1]*m_ImageExtent[2]; //ymin
      m_ImageBounds[3] = m_ImageOrigin[1] + m_ImageSpacing[1]*m_ImageExtent[3]; //ymax
      m_ImageBounds[4] = m_ImageOrigin[2] + m_ImageSpacing[2]*m_ImageExtent[4]; //zmin
      m_ImageBounds[5] = m_ImageOrigin[2] + m_ImageSpacing[2]*m_ImageExtent[5]; //zmax

      for ( unsigned int i = 0; i <= 4; i += 2 ) // reverse bounds if necessary
      {
        if ( m_ImageBounds[i] > m_ImageBounds[i+1] )
        {
          double t = m_ImageBounds[i+1];
          m_ImageBounds[i+1] = m_ImageBounds[i];
          m_ImageBounds[i] = t;
        }
      }

      VectorType auxvec;
      VectorType auxnorm;

      auxvec.Fill(0);
      auxvec[0] = m_ImageBounds[0];
      auxvec[1] = 0.5 * ( m_ImageBounds[2] + m_ImageBounds[3] );
      auxvec[2] = 0.5 * ( m_ImageBounds[4] + m_ImageBounds[5] );
      m_ImageBoundsCenters.push_back(auxvec);
      auxnorm[0] = 1;
      auxnorm[1] = 0;
      auxnorm[2] = 0;
      m_ImageBoundsNormals.push_back(auxnorm);

      auxvec.Fill(0);
      auxvec[0] = m_ImageBounds[1];
      auxvec[1] = 0.5 * ( m_ImageBounds[2] + m_ImageBounds[3] );
      auxvec[2] = 0.5 * ( m_ImageBounds[4] + m_ImageBounds[5] );
      m_ImageBoundsCenters.push_back(auxvec);
      auxnorm[0] = -1;
      auxnorm[1] = 0;
      auxnorm[2] = 0;
      m_ImageBoundsNormals.push_back(auxnorm);

      auxvec.Fill(0);
      auxvec[0] = 0.5 * ( m_ImageBounds[0] + m_ImageBounds[1] );
      auxvec[1] = m_ImageBounds[2];
      auxvec[2] = 0.5 * ( m_ImageBounds[4] + m_ImageBounds[5] );
      m_ImageBoundsCenters.push_back(auxvec);
      auxnorm[0] = 0;
      auxnorm[1] = 1;
      auxnorm[2] = 0;
      m_ImageBoundsNormals.push_back(auxnorm);

      auxvec.Fill(0);
      auxvec[0] = 0.5 * ( m_ImageBounds[0] + m_ImageBounds[1] );
      auxvec[1] = m_ImageBounds[3];
      auxvec[2] = 0.5 * ( m_ImageBounds[4] + m_ImageBounds[5] );
      m_ImageBoundsCenters.push_back(auxvec);
      auxnorm[0] = 0;
      auxnorm[1] = -1;
      auxnorm[2] = 0;
      m_ImageBoundsNormals.push_back(auxnorm);

      auxvec.Fill(0);
      auxvec[0] = 0.5 * ( m_ImageBounds[0] + m_ImageBounds[1] );
      auxvec[1] = 0.5 * ( m_ImageBounds[2] + m_ImageBounds[3] );
      auxvec[2] = m_ImageBounds[4];
      m_ImageBoundsCenters.push_back(auxvec);
      auxnorm[0] = 0;
      auxnorm[1] = 0;
      auxnorm[2] = 1;
      m_ImageBoundsNormals.push_back(auxnorm);

      auxvec.Fill(0);
      auxvec[0] = 0.5 * ( m_ImageBounds[0] + m_ImageBounds[1] );
      auxvec[1] = 0.5 * ( m_ImageBounds[2] + m_ImageBounds[3] );
      auxvec[2] = m_ImageBounds[5];
      m_ImageBoundsCenters.push_back(auxvec);
      auxnorm[0] = 0;
      auxnorm[1] = 0;
      auxnorm[2] = -1;
      m_ImageBoundsNormals.push_back(auxnorm);

      m_ToolPosition[0] = 0.5*(m_ImageBounds[0] + m_ImageBounds[1]);
      m_ToolPosition[1] = 0.5*(m_ImageBounds[2] + m_ImageBounds[3]);
      m_ToolPosition[2] = 0.5*(m_ImageBounds[4] + m_ImageBounds[5]);

      switch ( this->GetOrientationType() )
      {
        case Axial:
        case OffAxial:
            m_PlaneOrigin[0] = m_ImageBounds[0];
            m_PlaneOrigin[1] = m_ImageBounds[2];
            m_PlaneOrigin[2] = m_ImageBounds[4];

            m_PlanePoint1[0] = m_ImageBounds[1];
            m_PlanePoint1[1] = m_ImageBounds[2];
            m_PlanePoint1[2] = m_ImageBounds[4];

            m_PlanePoint2[0] = m_ImageBounds[0];
            m_PlanePoint2[1] = m_ImageBounds[3];
            m_PlanePoint2[2] = m_ImageBounds[4];

            m_PlaneNormal[0] = 0;
            m_PlaneNormal[1] = 0;
            m_PlaneNormal[2] = 1;

            break;
        case Sagittal:
        case OffSagittal:
            m_PlaneOrigin[0] = m_ImageBounds[0];
            m_PlaneOrigin[1] = m_ImageBounds[2];
            m_PlaneOrigin[2] = m_ImageBounds[4];

            m_PlanePoint1[0] = m_ImageBounds[0];
            m_PlanePoint1[1] = m_ImageBounds[3];
            m_PlanePoint1[2] = m_ImageBounds[4];

            m_PlanePoint2[0] = m_ImageBounds[0];
            m_PlanePoint2[1] = m_ImageBounds[2];
            m_PlanePoint2[2] = m_ImageBounds[5];

            m_PlaneNormal[0] = 0;
            m_PlaneNormal[1] = 1;
            m_PlaneNormal[2] = 0;

            break;
        case Coronal:
        case OffCoronal:
            m_PlaneOrigin[0] = m_ImageBounds[0];
            m_PlaneOrigin[1] = m_ImageBounds[2];
            m_PlaneOrigin[2] = m_ImageBounds[4];

            m_PlanePoint1[0] = m_ImageBounds[0];
            m_PlanePoint1[1] = m_ImageBounds[2];
            m_PlanePoint1[2] = m_ImageBounds[5];

            m_PlanePoint2[0] = m_ImageBounds[1];
            m_PlanePoint2[1] = m_ImageBounds[2];
            m_PlanePoint2[2] = m_ImageBounds[4];

            m_PlaneNormal[0] = 1;
            m_PlaneNormal[1] = 0;
            m_PlaneNormal[2] = 0;

            break;
        default: // set axial extension as default
            m_PlaneOrigin[0] = m_ImageBounds[0];
            m_PlaneOrigin[1] = m_ImageBounds[2];
            m_PlaneOrigin[2] = m_ImageBounds[4];

            m_PlanePoint1[0] = m_ImageBounds[1];
            m_PlanePoint1[1] = m_ImageBounds[2];
            m_PlanePoint1[2] = m_ImageBounds[4];

            m_PlanePoint2[0] = m_ImageBounds[0];
            m_PlanePoint2[1] = m_ImageBounds[3];
            m_PlanePoint2[2] = m_ImageBounds[4];

            m_PlaneNormal[0] = 0;
            m_PlaneNormal[1] = 0;
            m_PlaneNormal[2] = 1;

            break;
      }

    //   m_PlaneSource->SetNormal(m_PlaneNormal[0],m_PlaneNormal[1],m_PlaneNormal[2]);
       m_PlaneSource->SetOrigin(m_PlaneOrigin[0],m_PlaneOrigin[1],m_PlaneOrigin[2]);
       m_PlaneSource->SetPoint1(m_PlanePoint1[0],m_PlanePoint1[1],m_PlanePoint1[2]);
       m_PlaneSource->SetPoint2(m_PlanePoint2[0],m_PlanePoint2[1],m_PlanePoint2[2]);
    }
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ReportInvalidImageSpatialObjectProcessing( )
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::ReportInvalidImageSpatialObjectProcessing called...\n");
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ReportInvalidSliceNumberProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::ReportInvalidSliceNumberProcessing called...\n");
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ReportInvalidMousePositionProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::ReportInvalidMousePositionProcessing called...\n");
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::RequestSetToolSpatialObject( const ToolSpatialObjectType * toolSpatialObject )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::RequestSetToolSpatialObject called...\n");

  m_ToolSpatialObjectToBeSet = const_cast< ToolSpatialObjectType *>(toolSpatialObject);

  if( !m_ToolSpatialObjectToBeSet )
    {
    m_StateMachine.PushInput( m_InValidToolSpatialObjectInput );
    }
  else
    {
    m_StateMachine.PushInput( m_ValidToolSpatialObjectInput );
    }

  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::SetToolSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::SetToolSpatialObjectProcessing called...\n");

  m_ToolSpatialObject = m_ToolSpatialObjectToBeSet;
  this->ObserveToolTransformWRTImageCoordinateSystemInput( this->m_ToolSpatialObject );
  m_ToolSpatialObjectSet = true;
}

template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ReportInvalidToolSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::ReportInvalidToolSpatialObjectProcessing called...\n");
}

template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::RequestUpdateToolTransformWRTImageCoordinateSystem()
{
  igstkLogMacro( DEBUG,
                 "igstk::ImageReslicePlaneSpatialObject::\
                 RequestUpdateToolTransformWRTImageCoordinateSystem called ...\n");
  
  igstkPushInputMacro( GetToolTransformWRTImageCoordinateSystem );
  this->m_StateMachine.ProcessInputs();
}

/** Request to receive the tool transform WRT image coordinate system */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::RequestGetToolTransformWRTImageCoordinateSystemProcessing()
{
  igstkLogMacro( DEBUG,
                 "igstk::ImageReslicePlaneSpatialObject::\
                 RequestGetToolTransformWRTImageCoordinateSystemProcessing called ...\n");

  typedef igstk::Friends::CoordinateSystemHelper 
                          CoordinateSystemHelperType;

  const CoordinateSystem* ImageSpatialObjectCoordinateSystem = 
    CoordinateSystemHelperType::GetCoordinateSystem( m_ImageSpatialObject );

  CoordinateSystem * ImageSpatialObjectCoordinateSystemNC =
          const_cast< CoordinateSystem *>(ImageSpatialObjectCoordinateSystem);

  m_ToolSpatialObject->RequestComputeTransformTo( ImageSpatialObjectCoordinateSystemNC ); 
}

/** Receive the tool spatial object transform with respect to the Image
 * coordinate system using a transduction macro */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ReceiveToolTransformWRTImageCoordinateSystemProcessing()
{
  igstkLogMacro( DEBUG, "ReceiveToolTransformWRTImageCoordinateSystemProcessing " 
                 << this->m_ToolTransformWRTImageCoordinateSystem );

  this->m_ToolTransformWRTImageCoordinateSystem =
    this->m_ToolTransformWRTImageCoordinateSystemInputToBeSet.GetTransform();
}

/** Request compute reslicing plane */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::RequestComputeReslicingPlane( )
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::RequestComputeReslicingPlane called...\n");

  igstkPushInputMacro( ComputeReslicePlane );
  m_StateMachine.ProcessInputs();

}

/** Compute reslicing plane */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ComputeReslicePlaneProcessing()
{
  //Update the tool transform if tool spatial object provided
  if ( m_ToolSpatialObject ) 
    {
    this->RequestUpdateToolTransformWRTImageCoordinateSystem();
    }

  switch( m_ReslicingMode )
    {
    case Orthogonal:
      {
      this->ComputeOrthogonalReslicingPlane();
      break;
      }
    case Oblique:
      {
      this->ComputeObliqueReslicingPlane();
      break;
      }
    case OffOrthogonal:
      {
      this->ComputeOffOrthogonalReslicingPlane();
      break;
      }
    default:
      break;
    }  
} 

/**Compute orthgonal reslicing plane */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ComputeOrthogonalReslicingPlane( )
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::ComputeOrthogonalReslicingPlane called...\n");

  //If a tool spatial object is set (automatic reslicing) , then the 
  //the plane center will be set to the tool postion in 3D space.
  if( m_ToolSpatialObject )
    {
        Transform::VectorType   translation;
        translation = m_ToolTransformWRTImageCoordinateSystem.GetTranslation();

        m_ToolPosition[0] = translation[0];
        m_ToolPosition[1] = translation[1];
        m_ToolPosition[2] = translation[2];

        m_PlaneCenter[0] = translation[0];
        m_PlaneCenter[1] = translation[1];
        m_PlaneCenter[2] = translation[2];

        switch( m_OrientationType )
        {
            case Axial:
            {
              m_PlaneNormal[0] = 0.0;
              m_PlaneNormal[1] = 0.0;
              m_PlaneNormal[2] = 1.0;

              m_PlaneCenter[0] = 0.5*(m_ImageBounds[0]+m_ImageBounds[1]);
              m_PlaneCenter[1] = 0.5*(m_ImageBounds[2]+m_ImageBounds[3]);

              m_PlaneOrigin[0] = m_ImageBounds[0];
              m_PlaneOrigin[1] = m_ImageBounds[2];
              m_PlaneOrigin[2] = m_ImageBounds[4];

              m_PlanePoint1[0] = m_ImageBounds[1];
              m_PlanePoint1[1] = m_ImageBounds[2];
              m_PlanePoint1[2] = m_ImageBounds[4];

              m_PlanePoint2[0] = m_ImageBounds[0];
              m_PlanePoint2[1] = m_ImageBounds[3];
              m_PlanePoint2[2] = m_ImageBounds[4];

              break;
            }

            case Sagittal:
            {
              m_PlaneNormal[0] = 1.0;
              m_PlaneNormal[1] = 0.0;
              m_PlaneNormal[2] = 0.0;

              m_PlaneCenter[1] = 0.5*(m_ImageBounds[2]+m_ImageBounds[3]);
              m_PlaneCenter[2] = 0.5*(m_ImageBounds[4]+m_ImageBounds[5]);

              m_PlaneOrigin[0] = m_ImageBounds[0];
              m_PlaneOrigin[1] = m_ImageBounds[2];
              m_PlaneOrigin[2] = m_ImageBounds[4];

              m_PlanePoint1[0] = m_ImageBounds[0];
              m_PlanePoint1[1] = m_ImageBounds[3];
              m_PlanePoint1[2] = m_ImageBounds[4];

              m_PlanePoint2[0] = m_ImageBounds[0];
              m_PlanePoint2[1] = m_ImageBounds[2];
              m_PlanePoint2[2] = m_ImageBounds[5];
              break;
            }

            case Coronal:
            {
              m_PlaneNormal[0] = 0.0;
              m_PlaneNormal[1] = 1.0;
              m_PlaneNormal[2] = 0.0;

              m_PlaneCenter[0] = 0.5*(m_ImageBounds[0]+m_ImageBounds[1]);
              m_PlaneCenter[2] = 0.5*(m_ImageBounds[4]+m_ImageBounds[5]);

              m_PlaneOrigin[0] = m_ImageBounds[0];
              m_PlaneOrigin[1] = m_ImageBounds[2];
              m_PlaneOrigin[2] = m_ImageBounds[4];

              m_PlanePoint1[0] = m_ImageBounds[0];
              m_PlanePoint1[1] = m_ImageBounds[2];
              m_PlanePoint1[2] = m_ImageBounds[5];

              m_PlanePoint2[0] = m_ImageBounds[1];
              m_PlanePoint2[1] = m_ImageBounds[2];
              m_PlanePoint2[2] = m_ImageBounds[4];

              break;
            }

            default:
            {
            std::cerr << "Invalid orientation type " << std::endl;
            break;
            }
       }
    }
  else
    {
        // Otherwise, use the slice number or mouse postion and image bounds to set the center
        switch( m_OrientationType )
        {
            case Axial:
            {
              m_PlaneNormal[0] = 0.0;
              m_PlaneNormal[1] = 0.0;
              m_PlaneNormal[2] = 1.0;

              m_PlaneCenter[0] = 0.5*(m_ImageBounds[0] + m_ImageBounds[1]);
              m_PlaneCenter[1] = 0.5*(m_ImageBounds[2] + m_ImageBounds[3]);

              if ( m_SliceNumberSetFlag )
              {
              m_PlaneCenter[2] = m_ImageOrigin[2] + m_ImageSpacing[2]*m_SliceNumber;
              m_ToolPosition[2] = m_PlaneCenter[2];
              m_SliceNumberSetFlag = false;
              }

              if ( m_MousePositionSetFlag )
              {
              m_PlaneCenter[2] = m_MousePosition[2];
              m_ToolPosition[2] = m_MousePosition[2];
              m_MousePositionSetFlag = false;
              }
              break; 
            }
            case Sagittal:
            {
              m_PlaneNormal[0] = 1.0;
              m_PlaneNormal[1] = 0.0;
              m_PlaneNormal[2] = 0.0;

              m_PlaneCenter[1] = 0.5*(m_ImageBounds[2] + m_ImageBounds[3]);
              m_PlaneCenter[2] = 0.5*(m_ImageBounds[4] + m_ImageBounds[5]);

              if( m_SliceNumberSetFlag )
              {
                m_PlaneCenter[0] = m_ImageOrigin[0] + m_ImageSpacing[0]*m_SliceNumber;
                m_ToolPosition[0] = m_PlaneCenter[0];
                m_SliceNumberSetFlag = false;
              }

              if ( m_MousePositionSetFlag )
              {
                m_PlaneCenter[0] = m_MousePosition[0];
                m_ToolPosition[0] = m_MousePosition[0];
                m_MousePositionSetFlag = false;
              }
              break;
            }
            case Coronal:
            {
              m_PlaneNormal[0] = 0.0;
              m_PlaneNormal[1] = 1.0;
              m_PlaneNormal[2] = 0.0;

              m_PlaneCenter[0] = 0.5*(m_ImageBounds[0] + m_ImageBounds[1]);
              m_PlaneCenter[2] = 0.5*(m_ImageBounds[4] + m_ImageBounds[5]);

              if( m_SliceNumberSetFlag )
              {
                  m_PlaneCenter[1] = m_ImageOrigin[1] + m_ImageSpacing[1]*m_SliceNumber;
                  m_ToolPosition[1] = m_PlaneCenter[1];
                  m_SliceNumberSetFlag = false;
              }

              if ( m_MousePositionSetFlag )
              {
                  m_PlaneCenter[1] = m_MousePosition[1];
                  m_ToolPosition[1] = m_MousePosition[1];
                  m_MousePositionSetFlag = false;
              }
              break;
            }
            default:
            {
                std::cerr << "Invalid orientaiton" << std::endl;
                break;
            }
      }
  }  
}

/**Compute oblique reslicing plane */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ComputeObliqueReslicingPlane( )
{
 igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::ComputeObliqueReslicingPlane called...\n");

  /* Calculate the tool's long axis vector */
 
  igstk::Transform::VectorType   probeVector;
  probeVector.Fill(0.0);
  // fixme: the tool�s long axis direction changes depending on the spatial object definition 
  // so, what we can do to always set it correctly?
  probeVector[0] = 1;

  probeVector = m_ToolTransformWRTImageCoordinateSystem.GetRotation().Transform(probeVector);
  m_PlaneCenter = m_ToolTransformWRTImageCoordinateSystem.GetTranslation();

  m_ToolPosition[0] = m_PlaneCenter[0];
  m_ToolPosition[1] = m_PlaneCenter[1];
  m_ToolPosition[2] = m_PlaneCenter[2];

  // auxiliary axes
  igstk::Transform::VectorType vx, vy, vn, v;

  m_PlaneCenter = m_ToolTransformWRTImageCoordinateSystem.GetTranslation();

  switch( m_OrientationType )
    {
    case PlaneOrientationWithXAxesNormal:
      { 
        vx.Fill( 0.0 );
        vx[1] = 1;
        vx = m_ToolTransformWRTImageCoordinateSystem.GetRotation().Transform(vx);        

        m_PlaneNormal = probeVector;        
        
        vy = itk::CrossProduct( vx, probeVector );

        m_PlaneOrigin = m_ToolPosition-vx*dist0-vy*dist0;
        m_PlanePoint1 = m_ToolPosition-vx*dist0+vy*dist0;
        m_PlanePoint2 = m_ToolPosition+vx*dist0-vy*dist0;

        break;
      }

    case PlaneOrientationWithYAxesNormal:
      {
        vx.Fill( 0.0 );
        vx[1] = 1;
        vx = m_ToolTransformWRTImageCoordinateSystem.GetRotation().Transform(vx);

        m_PlaneNormal = vx;

        vy = itk::CrossProduct( vx, probeVector );

        m_PlaneOrigin = m_ToolPosition-probeVector*dist0;
        m_PlanePoint1 = m_ToolPosition-probeVector*dist0-vx*dist0-vy*dist0;
        m_PlanePoint2 = m_ToolPosition-probeVector*dist0+vx*dist0-vy*dist0;
        
        break;
      }

    case PlaneOrientationWithZAxesNormal:
      {
        vx.Fill( 0.0 );
        vx[2] = 1;
        vx = m_ToolTransformWRTImageCoordinateSystem.GetRotation().Transform(vx);

        m_PlaneNormal = vx;

        vy = itk::CrossProduct( vx, probeVector );

        
        m_PlaneOrigin = m_ToolPosition-probeVector*dist0;
        m_PlanePoint1 = m_ToolPosition-probeVector*dist0-vx*dist0-vy*dist0;
        m_PlanePoint2 = m_ToolPosition-probeVector*dist0+vx*dist0-vy*dist0;

        break;
      }
    }
}

/**Compute off-orthgonal reslicing plane */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ComputeOffOrthogonalReslicingPlane( )
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::ComputeOffOrthogonalReslicingPlane called...\n");

  /* Calculate the tool's long axis vector */
  igstk::Transform::VectorType   probeVector;
  probeVector.Fill(0.0);
  // fixme: the tool�s long axis direction changes depending on the spatial object definition 
  // so, what we can do to always set it correctly?
  probeVector[0] = 1;

  probeVector = m_ToolTransformWRTImageCoordinateSystem.GetRotation().Transform(probeVector);
  m_PlaneCenter = m_ToolTransformWRTImageCoordinateSystem.GetTranslation();

  m_ToolPosition[0] = m_PlaneCenter[0];
  m_ToolPosition[1] = m_PlaneCenter[1];
  m_ToolPosition[2] = m_PlaneCenter[2];

  VectorType vx, vy, vn, v;

  switch( m_OrientationType )
    {
      case OffAxial:
      {
        vx.Fill( 0.0 );
        vx[0] = 1;
      
        if ( fabs(vx*probeVector) < 1e-9 )
        {
        // FIXME: need to handle this situation too
        igstkLogMacro( DEBUG, "The two vectors are parallel \n");
        }

        vn = itk::CrossProduct( vx, probeVector );
        vn.Normalize();

        m_PlaneCenter[0] = 0.5*(m_ImageBounds[0]+m_ImageBounds[1]); 

        m_PlaneNormal = vn;
        break;
      }

      case OffSagittal:
      {
                
        vx.Fill( 0.0 );
        vx[2] = 1;
      
        if ( fabs(vx*probeVector) < 1e-9 )
        {
        // FIXME: need to handle this situation too
        igstkLogMacro( DEBUG, "The two vectors are parallel \n");
        }

        vn = itk::CrossProduct( vx, probeVector );
        vn.Normalize();

        m_PlaneNormal = vn;

        vy = itk::CrossProduct( vn, vx );
        vy.Normalize();

//        m_PlaneCenter[2] = 0.5*(m_ImageBounds[4]+m_ImageBounds[5]); 

        m_PlaneOrigin = m_ToolPosition-probeVector*dist0;
        m_PlanePoint1 = m_ToolPosition-probeVector*dist0-vx*dist0-vy*dist0;
        m_PlanePoint2 = m_ToolPosition-probeVector*dist0+vx*dist0-vy*dist0;
        
        break;
      }

    case OffCoronal:
      {
        vx.Fill( 0.0 );
        vx[1] = 1;
      
        if ( fabs(vx*probeVector) < 1e-9 )
        {
        // FIXME: need to handle this situation too
        igstkLogMacro( DEBUG, "The two vectors are parallel \n");
        }

        vn = itk::CrossProduct( vx, probeVector );
        vn.Normalize();

        m_PlaneNormal = vn;

        vy = itk::CrossProduct( vn, vx );
        vy.Normalize();

//        m_PlaneCenter[2] = 0.5*(m_ImageBounds[4]+m_ImageBounds[5]); 

        m_PlaneOrigin = m_ToolPosition-probeVector*dist0;
        m_PlanePoint1 = m_ToolPosition-probeVector*dist0-vx*dist0-vy*dist0;
        m_PlanePoint2 = m_ToolPosition-probeVector*dist0+vx*dist0-vy*dist0;

        break;
      }      
    }
}

/** Get distance to closest plane in the image bounding box 
* work in progress ...
* p: is the tool position
* pv: is the tool�s long axis vector
* pi: is the plane index in the bounding box
*/
template < class TImageSpatialObject >
double
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::GetDistanceToPlane(VectorType p, VectorType pv, unsigned int pi)
{
  double u = 0.0;

  // first check that p does not lay on the plane pi
  if ( fabs( (p - m_ImageBoundsCenters[pi])*m_ImageBoundsNormals[pi] ) < 1e-9 )
    return u;

  u = (m_ImageBoundsNormals[pi]*m_ImageBoundsCenters[pi] - m_ImageBoundsCenters[pi]*p)/
    (m_ImageBoundsNormals[pi]*pv);

  return u;
}

/** Get reslicing plane equation */
template < class TImageSpatialObject >
vtkPlaneSource *
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::GetReslicingPlane()
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::GetReslicingPlane called...\n");
  return m_PlaneSource;
}

///** Request Get reslicing axes matrix */
//template < class TImageSpatialObject >
//vtkMatrix4x4 *
//ImageReslicePlaneSpatialObject< TImageSpatialObject >
//::GetResliceAxes()
//{
//  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
//                       ::GetResliceAxes called...\n");
//  return m_ResliceAxes;
//}

/** Request Get reslicing transform*/
//template < class TImageSpatialObject >
//vtkTransform *
//ImageReslicePlaneSpatialObject< TImageSpatialObject >
//::GetResliceTransform()
//{
//  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
//                       ::GetResliceTransform called...\n");
//  return m_ResliceTransform;
//}

/** Report invalid reslicing mode */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ReportInvalidReslicingModeProcessing( void )
{
  igstkLogMacro( WARNING, 
    "igstk::ImageReslicePlaneSpatialObject::ReportInvalidReslicingModeProcessing called...\n");
}

/** Report invalid orientation type */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ReportInvalidOrientationTypeProcessing( void )
{
  igstkLogMacro( WARNING, 
    "igstk::ImageReslicePlaneSpatialObject::ReportInvalidOrientationTypeProcessing called...\n");
}

/** Report invalid request */
template < class TImageSpatialObject >
void 
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ReportInvalidRequestProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::ImageReslicePlaneSpatialObject::ReportInvalidRequestProcessing called...\n");

  this->InvokeEvent( InvalidRequestErrorEvent() );
}

/** Get tool transform WRT Image Coordinate System*/
template < class TImageSpatialObject >
igstk::Transform
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::GetToolTransform( ) const
{
  return this->m_ToolTransformWRTImageCoordinateSystem;
}

/** Check if tool spatial object is set to drive the reslicing*/
template < class TImageSpatialObject >
bool
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::IsToolSpatialObjectSet( ) 
{
  return this->m_ToolSpatialObjectSet;
}

/** Print object information */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace igstk


#endif
