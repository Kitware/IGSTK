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

//#include "itkVersor.h"

namespace igstk
{ 

/** Constructor */
template < class TImageSpatialObject >
ImageReslicePlaneSpatialObject< TImageSpatialObject>::ImageReslicePlaneSpatialObject():m_StateMachine(this)
{
  //Default reslicing mode
  m_ReslicingMode = Orthogonal;

  //Default orientation type
  m_OrientationType = Axial;

  m_ImageData = NULL;
  m_ImageSpatialObject = NULL;
  m_ToolSpatialObject = NULL; 

  //Create vtk plane 
  m_ImageReslicePlane = vtkPlaneSource::New();
  m_ImageReslicePlane->SetOrigin(0,0,0);
  m_ImageReslicePlane->SetCenter(0,0,0);

  //Create reslice axes matrix
  m_ResliceAxes = vtkMatrix4x4::New();

  //Create reslice transform
  //m_ResliceTransform = vtkTransform::New();
  //m_ResliceTransform->Identity();

  // Create the observer to VTK image events 
  m_VTKImageObserver = VTKImageObserver::New();
  m_ImageTransformObserver = ImageTransformObserver::New();

  //slice number
  m_SliceNumber = 0;
  m_SliceNumberSetFlag = false;

  //tool spatial object check flag
  m_ToolSpatialObjectSet  = false;

  //mouse position 
  m_MousePosition[0] = 0;
  m_MousePosition[1] = 0;
  m_MousePosition[2] = 0;
  m_MousePostionSetFlag = false;

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

  // From ValidMousePositionSet
  igstkAddTransitionMacro( ValidMousePositionSet,
                           ComputeReslicePlane,
                           ValidMousePositionSet,
                           ComputeReslicePlane );

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

  //From ToolSpatialObjectSet
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

  // From AttemptingToGetToolTransformWRTImageCoordinateSystem
  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ToolTransformWRTImageCoordinateSystem,
                           ToolSpatialObjectSet,
                           ReceiveToolTransformWRTImageCoordinateSystem );

  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem, GetSliceNumberBounds,
                           AttemptingToGetToolTransformWRTImageCoordinateSystem, ReportSliceNumberBounds );
 
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
  

  if( ! m_ImageReslicePlane )
    {
    m_ImageReslicePlane->Delete();
    m_ImageReslicePlane = NULL;
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
    
    int ext[6];

    m_ImageData->Update();
    m_ImageData->GetExtent( ext );

    switch( m_OrientationType )
      {
      case Axial:
        minSlice = ext[4];
        maxSlice = ext[5];
        break;
      case Sagittal:
        minSlice = ext[0];
        maxSlice = ext[1];
        break;
      case Coronal:
        minSlice = ext[2];
        maxSlice = ext[3];
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
 
  m_ImageData->Update();

  int ext[6];
  m_ImageData->GetExtent( ext );

  EventHelperType::IntegerBoundsType bounds;

  switch( m_OrientationType )
    {
    case Axial:
      {
      bounds.minimum = ext[4];
      bounds.maximum = ext[5];
      AxialSliceBoundsEvent event;
      event.Set( bounds );
      this->InvokeEvent( event );
      break;
      }
    case Sagittal:
      {
      bounds.minimum = ext[0];
      bounds.maximum = ext[1];
      SagittalSliceBoundsEvent event;
      event.Set( bounds );
      this->InvokeEvent( event );
      break;
      }
    case Coronal:
      {
      bounds.minimum = ext[2];
      bounds.maximum = ext[3];
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
::RequestSetMousePosition( double mousePosition[3] )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::RequestSetMousePosition called...\n");

  m_MousePositionToBeSet[0] = mousePosition[0];
  m_MousePositionToBeSet[1] = mousePosition[1];
  m_MousePositionToBeSet[2] = mousePosition[2];

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
  if( m_ImageData )
    {

    m_ImageData->Update();

    int ext[6];
    m_ImageData->GetExtent( ext );

    double imageSpacing[3];
    m_ImageData->GetSpacing( imageSpacing );

    double imageOrigin[3];
    m_ImageData->GetOrigin( imageOrigin );

    int imageExtent[6];
    m_ImageData->GetWholeExtent( imageExtent );

    double bounds[] = { imageOrigin[0] + imageSpacing[0]*imageExtent[0], //xmin
                        imageOrigin[0] + imageSpacing[0]*imageExtent[1], //xmax
                        imageOrigin[1] + imageSpacing[1]*imageExtent[2], //ymin
                        imageOrigin[1] + imageSpacing[1]*imageExtent[3], //ymax
                        imageOrigin[2] + imageSpacing[2]*imageExtent[4], //zmin
                        imageOrigin[2] + imageSpacing[2]*imageExtent[5]};//zmax

    for ( unsigned int i = 0; i <= 4; i += 2 ) // reverse bounds if necessary
        {
        if ( bounds[i] > bounds[i+1] )
          {
          double t = bounds[i+1];
          bounds[i+1] = bounds[i];
          bounds[i] = t;
          }
        }

    bool validPosition = false; 

    switch( m_OrientationType )
      {
      case Axial:
        if( m_MousePositionToBeSet[2] >= bounds[4] && 
            m_MousePositionToBeSet[2] <= bounds[5] )
          {
          validPosition = true;
          }
          break;
      case Sagittal:
        if( m_MousePositionToBeSet[0] >= bounds[0] && 
            m_MousePositionToBeSet[0] <= bounds[1] )
          {
          validPosition = true;
          }
        break;
      case Coronal:
        if( m_MousePositionToBeSet[1] >= bounds[2] && 
            m_MousePositionToBeSet[1] <= bounds[3] )
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

  //turn on the flag
  m_MousePostionSetFlag = true;
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
    if( m_OrientationTypeToBeSet == Perpendicular ||
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
    if( m_OrientationTypeToBeSet  == Perpendicular ||
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

  m_ImageSpatialObject->AddObserver( CoordinateSystemTransformToEvent(), 
                                     m_ImageTransformObserver );

  this->m_VTKImageObserver->Reset();

  this->m_ImageSpatialObject->RequestGetVTKImage();

  if( this->m_VTKImageObserver->GotVTKImage() ) 
    {
      m_ImageData = this->m_VTKImageObserver->GetVTKImage();
      m_ImageData->GetDimensions(m_ImageDimension);
      m_ImageData->GetOrigin(m_ImageOrigin);
      m_ImageData->GetSpacing(m_ImageSpacing);
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

  Transform::VectorType translation =
    this->m_ToolTransformWRTImageCoordinateSystem.GetTranslation();

  //Set the validity time stamp of the image spatial object transform to 
  //validity time of the tool spatial object.

//  Transform::TimePeriodType startTime = 
//         this->m_ToolTransformWRTImageCoordinateSystem.GetStartTime();

//  Transform::TimePeriodType expirationTime =
//         this->m_ToolTransformWRTImageCoordinateSystem.GetExpirationTime();
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
      this->ComputeOffOrthgonalReslicingPlane();
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
    m_PlaneCenter[0] = translation[0];
    m_PlaneCenter[1] = translation[1];
    m_PlaneCenter[2] = translation[2];
    }
  else
    {
    // Otherwise, use the slice number or mouse postion and image bounds to set the center
    switch( m_OrientationType )
    {
    case Axial:
      {
      m_PlaneCenter[0] = 0.5*(m_ImageBounds[0] + m_ImageBounds[1]);
      m_PlaneCenter[1] = 0.5*(m_ImageBounds[2] + m_ImageBounds[3]);
     
      if( m_SliceNumberSetFlag )
        {
        m_PlaneCenter[2] = m_ImageOrigin[2] + m_ImageSpacing[2]*m_SliceNumber;
        }
      else
        {
        m_PlaneCenter[2] = m_MousePosition[2]; 
        }
      break; 
      }
    case Sagittal:
      {
      if( m_SliceNumberSetFlag )
        {
        m_PlaneCenter[0] = m_ImageOrigin[0] + m_ImageSpacing[0]*m_SliceNumber;
        }
      else
        {
        m_PlaneCenter[0] = m_MousePosition[0];
        }

      m_PlaneCenter[1] = 0.5*(m_ImageBounds[2] + m_ImageBounds[3]);
      m_PlaneCenter[2] = 0.5*(m_ImageBounds[4] + m_ImageBounds[5]);
      break;
      }
    case Coronal:
      {
      m_PlaneCenter[0] = 0.5*(m_ImageBounds[0] + m_ImageBounds[1]);

      if( m_SliceNumberSetFlag )
        {
        m_PlaneCenter[1] = m_ImageOrigin[1] + m_ImageSpacing[1]*m_SliceNumber;
        }
      else
        {
        m_PlaneCenter[1] = m_MousePosition[1];
        }
      m_PlaneCenter[2] = 0.5*(m_ImageBounds[4] + m_ImageBounds[5]);
      break;
      }
    default:
      {
      std::cerr << "Invalid orientaiton" << std::endl;
      break;
      }
    }
  }
  
  switch( m_OrientationType )
    {
    case Axial:
      {
      m_PlaneNormal[0] = 0.0;
      m_PlaneNormal[1] = 0.0;
      m_PlaneNormal[2] = 1.0;

      m_PlaneCenter[0] = 0.5*(m_ImageBounds[0]+m_ImageBounds[1]);
      m_PlaneCenter[1] = 0.5*(m_ImageBounds[2]+m_ImageBounds[3]);

      break;
      }
    case Coronal:
      {
      m_PlaneNormal[0] = 0.0;
      m_PlaneNormal[1] = 1.0;
      m_PlaneNormal[2] = 0.0;

      m_PlaneCenter[0] = 0.5*(m_ImageBounds[0]+m_ImageBounds[1]);
      m_PlaneCenter[2] = 0.5*(m_ImageBounds[4]+m_ImageBounds[5]);
 
      break;
      }
    case Sagittal:
      {
      m_PlaneNormal[0] = 1.0;
      m_PlaneNormal[1] = 0.0;
      m_PlaneNormal[2] = 0.0;

  /*    origin[0] = planeCenter[0];
      origin[1] = bounds[2];
      origin[2] = bounds[4];*/

      m_PlaneCenter[1] = 0.5*(m_ImageBounds[2]+m_ImageBounds[3]);
      m_PlaneCenter[2] = 0.5*(m_ImageBounds[4]+m_ImageBounds[5]);

      break;
      }
    default:
      {
      std::cerr << "Invalid orientation type " << std::endl;
      break;
      }
    }

  m_ImageReslicePlane->SetCenter( m_PlaneCenter[0],
                                  m_PlaneCenter[1],
                                  m_PlaneCenter[2] );
 
  m_ImageReslicePlane->SetNormal( m_PlaneNormal[0],
                                  m_PlaneNormal[1],
                                  m_PlaneNormal[2] );
}

/**Compute oblique reslicing plane */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ComputeObliqueReslicingPlane( )
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::ComputeObliqueReslicingPlane called...\n");

  //For Oblique reslicing, a 6DOF tool should be used.
  if( !m_ToolSpatialObject )
    {
    std::cerr << "Tool spatial object is not specified. For Oblique \
                  reslicing mode,a tool spatialobject is required." << std::endl;
    return;
    }

  /* Compute input image bounds */
  double imageSpacing[3];
  m_ImageData->GetSpacing( imageSpacing );

  double imageOrigin[3];
  m_ImageData->GetOrigin( imageOrigin );

  int imageExtent[6];
  m_ImageData->GetWholeExtent( imageExtent );
 
  double bounds[] = {  imageOrigin[0] + imageSpacing[0]*imageExtent[0], //xmin
                       imageOrigin[0] + imageSpacing[0]*imageExtent[1], //xmax
                       imageOrigin[1] + imageSpacing[1]*imageExtent[2], //ymin
                       imageOrigin[1] + imageSpacing[1]*imageExtent[3], //ymax
                       imageOrigin[2] + imageSpacing[2]*imageExtent[4], //zmin
                       imageOrigin[2] + imageSpacing[2]*imageExtent[5]
                    };//zmax

  for ( unsigned int i = 0; i <= 4; i += 2 ) // reverse bounds if necessary
  {
    if ( bounds[i] > bounds[i+1] )
    {
      double t = bounds[i+1];
      bounds[i+1] = bounds[i];
      bounds[i] = t;
    }
  }

  double planeCenter[3];
  Transform::VectorType  translation;

  translation = 
    m_ToolTransformWRTImageCoordinateSystem.GetTranslation();

  planeCenter[0] = translation[0];
  planeCenter[1] = translation[1];
  planeCenter[2] = translation[2];

  double planeNormal[3];

  //Generate the orientation transforms
  Transform orientationTransform;
  translation.Fill(0.0);
  const double transformUncertainty = 1.0;
  orientationTransform.SetTranslation(
                          translation,
                          transformUncertainty,
                          ::igstk::TimeStamp::GetLongestPossibleTime() );

  Transform::VersorType  rotation;

  ::itk::Versor<double>::MatrixType   orientationMatrix;

  switch( m_OrientationType )
  {
  case PlaneOrientationWithZAxesNormal:
    {

      planeNormal[0] = 0.0;
      planeNormal[1] = 0.0;
      planeNormal[2] = 1.0;

      planeCenter[0] = 0.5*(bounds[0]+bounds[1]);
      planeCenter[1] = 0.5*(bounds[2]+bounds[3]);


    //equivalent to the matrix we use for Axial
    //set the versors

    orientationMatrix[0][0] = 0.0;
    orientationMatrix[1][0] = 1.0;
    orientationMatrix[2][0] = 0.0;

    orientationMatrix[0][1] = -1.0;
    orientationMatrix[1][1] = 0.0;
    orientationMatrix[2][1] = 0.0;

    orientationMatrix[0][2] = 0.0;
    orientationMatrix[1][2] = 0.0;
    orientationMatrix[2][2] = 1.0;

 
    rotation.Set( orientationMatrix );
    }
    break;
  case PlaneOrientationWithYAxesNormal:
    {

      planeNormal[0] = 0.0;
      planeNormal[1] = 1.0;
      planeNormal[2] = 0.0;

      planeCenter[0] = 0.5*(bounds[0]+bounds[1]);
      planeCenter[2] = 0.5*(bounds[4]+bounds[5]);

      //equivalent to the matrix we use for Coronal
      orientationMatrix[0][0] = 0.0;
      orientationMatrix[1][0] = 0.0;
      orientationMatrix[2][0] = -1.0;

      orientationMatrix[0][1] = 1.0;
      orientationMatrix[1][1] = 0.0;
      orientationMatrix[2][1] = 0.0;

      orientationMatrix[0][2] = 0.0;
      orientationMatrix[1][2] = 1.0;
      orientationMatrix[2][2] = 0.0;

      rotation.Set( orientationMatrix );
    }
    break;
  case PlaneOrientationWithXAxesNormal:
    {

      planeNormal[0] = 1.0;
      planeNormal[1] = 0.0;
      planeNormal[2] = 0.0;

      planeCenter[1] = 0.5*(bounds[2]+bounds[3]);
      planeCenter[2] = 0.5*(bounds[4]+bounds[5]);

    //equivalent to the matrix we use for Sagittal

      orientationMatrix[0][0] = 0.0;
      orientationMatrix[1][0] = 1.0;
      orientationMatrix[2][0] = 0.0;

      orientationMatrix[0][1] = 0.0;
      orientationMatrix[1][1] = 0.0;
      orientationMatrix[2][1] = -1.0;

      orientationMatrix[0][2] = -1.0;
      orientationMatrix[1][2] = 0.0;
      orientationMatrix[2][2] = 0.0;

    rotation.Set( orientationMatrix );

    }
    break;
  default:
    break;
  }
 
  orientationTransform.SetRotation(
                          rotation,
                          transformUncertainty,
                          ::igstk::TimeStamp::GetLongestPossibleTime() );

  Transform combinedTransform;
  combinedTransform = Transform::TransformCompose( orientationTransform, m_ToolTransformWRTImageCoordinateSystem );

  Transform::VectorType combinedTranslation = 
                            combinedTransform.GetTranslation();
  Transform::VersorType combinedRotation =
                            combinedTransform.GetRotation();

 ::itk::Versor<double>::MatrixType combinedRotationMatrix 
                          = combinedRotation.GetMatrix(); 
 
//  m_ResliceAxes->Identity();

  //for ( unsigned int i = 0; i < 3; i++ )
  //   {
  //   m_ResliceAxes->SetElement(i,0,combinedRotationMatrix[i][0]);
  //   m_ResliceAxes->SetElement(i,1,combinedRotationMatrix[i][1]);
  //   m_ResliceAxes->SetElement(i,2,combinedRotationMatrix[i][2]);
  //   m_ResliceAxes->SetElement(i,3,combinedTranslation[i]);
  //   }
 
  m_ImageReslicePlane->SetCenter( planeCenter[0],
                                  planeCenter[1],
                                  planeCenter[2] );
 
  m_ImageReslicePlane->SetNormal( planeNormal[0],
                                  planeNormal[1],
                                  planeNormal[2] );

  /* 
  std::cout.precision(3);
  std::cout << "ResliceAxes matrix: \n" 
            << "(" << m_ResliceAxes->GetElement(0,0) << ","
            << m_ResliceAxes->GetElement(0,1) << ","
            << m_ResliceAxes->GetElement(0,2) << ","
            << m_ResliceAxes->GetElement(0,3) << "\n"
            << m_ResliceAxes->GetElement(1,0) << ","
            << m_ResliceAxes->GetElement(1,1) << ","
            << m_ResliceAxes->GetElement(1,2) << ","
            << m_ResliceAxes->GetElement(1,3) << "\n"
            << m_ResliceAxes->GetElement(2,0) << ","
            << m_ResliceAxes->GetElement(2,1) << ","
            << m_ResliceAxes->GetElement(2,2) << ","
            << m_ResliceAxes->GetElement(2,3) << "\n"
            << m_ResliceAxes->GetElement(3,0) << ","
            << m_ResliceAxes->GetElement(3,1) << ","
            << m_ResliceAxes->GetElement(3,2) << ","
            << m_ResliceAxes->GetElement(3,3) << ")" << std::endl;
  */
}

/**Compute off-orthgonal reslicing plane */
template < class TImageSpatialObject >
void
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::ComputeOffOrthgonalReslicingPlane( )
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::ComputeOffOrthgonalReslicingPlane called...\n");

  /* Calculate the tool's long axis vector */
 
  igstk::Transform::VectorType   probeVector;
  probeVector.Fill(0.0);
  // fixme: the tool´s long axis direction changes deppending on the spatial object definition 
  // so, what we can do to always set it correctly?
  probeVector[0] = 1;

  probeVector = m_ToolTransformWRTImageCoordinateSystem.GetRotation().Transform(probeVector);
  m_PlaneCenter = m_ToolTransformWRTImageCoordinateSystem.GetTranslation();

  // auxiliary axes
  igstk::Transform::VectorType axes1, axes2;

  m_PlaneCenter = m_ToolTransformWRTImageCoordinateSystem.GetTranslation();

  switch( m_OrientationType )
    {
    case OffCoronal:
      {      

      //m_PlaneCenter[0] = 0.5*(m_ImageBounds[0] + m_ImageBounds[1]);
      //m_PlaneCenter[1] = 0.5*(m_ImageBounds[2] + m_ImageBounds[3]);
        m_PlaneCenter[2] = 0.5*(m_ImageBounds[4] + m_ImageBounds[5]);
        axes2.Fill( 0 );
        axes2[2] = 1;
        m_PlaneNormal = itk::CrossProduct( probeVector, axes2 );
        break;
      }

    case OffSagittal:
      {
        //m_PlaneCenter[0] = 0.5*(m_ImageBounds[0] + m_ImageBounds[1]);
        m_PlaneCenter[1] = 0.5*(m_ImageBounds[2] + m_ImageBounds[3]);
        //m_PlaneCenter[2] = 0.5*(m_ImageBounds[4] + m_ImageBounds[5]);
        axes2.Fill( 0 );
        axes2[1] = 1;
        m_PlaneNormal = itk::CrossProduct( probeVector, axes2 );
        break;
      }

    case Perpendicular:
      {
        m_PlaneNormal = probeVector;
        break;
      }
    }

  m_ImageReslicePlane->SetCenter( m_PlaneCenter[0],
                                  m_PlaneCenter[1],
                                  m_PlaneCenter[2]);

  m_ImageReslicePlane->SetNormal( m_PlaneNormal[0],
                                  m_PlaneNormal[1],
                                  m_PlaneNormal[2]);
  
}

/** Get reslcing plane equation */
template < class TImageSpatialObject >
vtkPlaneSource *
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::GetReslicingPlane()
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::GetReslicingPlane called...\n");
  return m_ImageReslicePlane;
}

/** Request Get reslicing axes matrix */
template < class TImageSpatialObject >
vtkMatrix4x4 *
ImageReslicePlaneSpatialObject< TImageSpatialObject >
::GetResliceAxes()
{
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::GetResliceAxes called...\n");
  return m_ResliceAxes;
}

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
