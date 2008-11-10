/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkReslicerPlaneSpatialObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkReslicerPlaneSpatialObject_cxx
#define __igstkReslicerPlaneSpatialObject_cxx

#include "igstkReslicerPlaneSpatialObject.h"
#include "igstkEvents.h"

#include "vtkMatrix4x4.h"
#include "vtkPlaneSource.h"
#include "vtkMath.h"
#include "vtkTransform.h"


static const double dist0 = 200;

namespace igstk
{ 

/** Constructor */
ReslicerPlaneSpatialObject
::ReslicerPlaneSpatialObject():m_StateMachine(this)
{
  //Default reslicing mode
  m_ReslicingMode = Orthogonal;

  //Default orientation type
  m_OrientationType = Axial;

  m_BoundingBoxProviderSpatialObject = NULL;
  m_ToolSpatialObject = NULL; 

  //Create vtk plane 
  m_PlaneSource = vtkPlaneSource::New();
  m_PlaneSource->SetOrigin(0,0,0);
  m_PlaneSource->SetCenter(0,0,0);
  m_PlaneSource->SetNormal(1,0,0);

  //Create reslice axes matrix
  m_ResliceAxes = vtkMatrix4x4::New();

  //Create reslice transform
  //m_ResliceTransform = vtkTransform::New();
  //m_ResliceTransform->Identity();

  // Create the observer to VTK image events 
//  m_VTKImageObserver = VTKImageObserver::New();
  //m_ImageTransformObserver = ImageTransformObserver::New();

  //tool spatial object check flag
  m_ToolSpatialObjectSet  = false;

  //Cursor position 
  m_CursorPosition[0] = 0;
  m_CursorPosition[1] = 0;
  m_CursorPosition[2] = 0;

  //tool position 
  m_ToolPosition[0] = 0;
  m_ToolPosition[1] = 0;
  m_ToolPosition[2] = 0;

  m_CursorPositionSetFlag = false;

  //List of states
  igstkAddStateMacro( Initial );
  igstkAddStateMacro( ReslicingModeSet );
  igstkAddStateMacro( OrientationTypeSet );
  igstkAddStateMacro( BoundingBoxProviderSpatialObjectSet );
  igstkAddStateMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem );
  igstkAddStateMacro( AttemptingToSetCursorPosition );
  igstkAddStateMacro( AttemptingToSetBoundingBoxProviderSpatialObject );

  // List of state machine inputs
  igstkAddInputMacro( SetBoundingBoxProviderSpatialObject );
  igstkAddInputMacro( ValidReslicingMode );
  igstkAddInputMacro( InValidReslicingMode );
  igstkAddInputMacro( ValidOrientationType );
  igstkAddInputMacro( InValidOrientationType );
  igstkAddInputMacro( ValidBoundingBoxProviderSpatialObject );
  igstkAddInputMacro( InValidBoundingBoxProviderSpatialObject );
  igstkAddInputMacro( ValidToolSpatialObject );
  igstkAddInputMacro( InValidToolSpatialObject );  
  igstkAddInputMacro( SetCursorPosition );
  igstkAddInputMacro( ValidCursorPosition );
  igstkAddInputMacro( InValidCursorPosition );
  igstkAddInputMacro( GetToolPosition );
  igstkAddInputMacro( GetVTKPlane );
  igstkAddInputMacro( GetToolTransformWRTImageCoordinateSystem );
  igstkAddInputMacro( ToolTransformWRTImageCoordinateSystem );
  igstkAddInputMacro( ComputeReslicePlane );

  // List of state machine transitions

  //From Initial
  igstkAddTransitionMacro( Initial, ValidReslicingMode, 
                           ReslicingModeSet, SetReslicingMode );

  igstkAddTransitionMacro( Initial, InValidReslicingMode, 
                           Initial, ReportInvalidReslicingMode );

  igstkAddTransitionMacro( Initial, ValidOrientationType, 
                           Initial, ReportInvalidRequest);

  igstkAddTransitionMacro( Initial, InValidOrientationType, 
                           Initial, ReportInvalidRequest);  

  igstkAddTransitionMacro( Initial, SetCursorPosition, 
                           Initial, ReportInvalidRequest);

  igstkAddTransitionMacro( Initial, SetBoundingBoxProviderSpatialObject, 
                           Initial, ReportInvalidRequest);

  //From ReslicingModeSet
  igstkAddTransitionMacro( ReslicingModeSet, ValidOrientationType, 
                           OrientationTypeSet, SetOrientationType );

  igstkAddTransitionMacro( ReslicingModeSet, InValidOrientationType, 
                           ReslicingModeSet, ReportInvalidOrientationType);

  igstkAddTransitionMacro( ReslicingModeSet, SetCursorPosition, 
                           ReslicingModeSet, ReportInvalidRequest );  

  //From OrientationTypeSet
  igstkAddTransitionMacro( OrientationTypeSet, SetBoundingBoxProviderSpatialObject,
                           AttemptingToSetBoundingBoxProviderSpatialObject, AttemptSetBoundingBoxProviderSpatialObject );

  igstkAddTransitionMacro( OrientationTypeSet, SetCursorPosition, 
                           OrientationTypeSet, ReportInvalidRequest );


  // From AttemptingToSetBoundingBoxProviderSpatialObject
  igstkAddTransitionMacro( AttemptingToSetBoundingBoxProviderSpatialObject, ValidBoundingBoxProviderSpatialObject,
                           BoundingBoxProviderSpatialObjectSet,  SetBoundingBoxProviderSpatialObject ); 

  igstkAddTransitionMacro( AttemptingToSetBoundingBoxProviderSpatialObject, InValidBoundingBoxProviderSpatialObject,
                           OrientationTypeSet,  ReportInvalidBoundingBoxProviderSpatialObject );


  //From BoundingBoxProviderSpatialObjectSet
  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, ValidOrientationType,
                           BoundingBoxProviderSpatialObjectSet, SetOrientationType );

  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, InValidOrientationType,
                           BoundingBoxProviderSpatialObjectSet, ReportInvalidOrientationType );

  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, ValidReslicingMode, 
                           BoundingBoxProviderSpatialObjectSet, SetReslicingMode );

  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, InValidReslicingMode, 
                           BoundingBoxProviderSpatialObjectSet, ReportInvalidReslicingMode );

  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, ValidToolSpatialObject,
                           BoundingBoxProviderSpatialObjectSet, SetToolSpatialObject );

  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, InValidToolSpatialObject,
                           BoundingBoxProviderSpatialObjectSet, ReportInvalidToolSpatialObject );  

  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, SetCursorPosition,
                           AttemptingToSetCursorPosition, AttemptSetCursorPosition );  

  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, ComputeReslicePlane,
                           BoundingBoxProviderSpatialObjectSet, ComputeReslicePlane );

  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, GetToolPosition,
                           BoundingBoxProviderSpatialObjectSet, ReportToolPosition );

  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, GetVTKPlane,
                           BoundingBoxProviderSpatialObjectSet, ReportVTKPlane );

  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, GetToolTransformWRTImageCoordinateSystem,
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                                             RequestGetToolTransformWRTImageCoordinateSystem );

  // From AttemptingToSetCursorPosition
  igstkAddTransitionMacro( AttemptingToSetCursorPosition, ValidCursorPosition,
                           BoundingBoxProviderSpatialObjectSet,  SetCursorPosition ); 

  igstkAddTransitionMacro( AttemptingToSetCursorPosition, InValidCursorPosition,
                           BoundingBoxProviderSpatialObjectSet,  ReportInvalidCursorPosition );

  // From AttemptingToGetToolTransformWRTImageCoordinateSystem
  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem, ToolTransformWRTImageCoordinateSystem,
                           BoundingBoxProviderSpatialObjectSet, ReceiveToolTransformWRTImageCoordinateSystem );

  igstkSetInitialStateMacro( Initial );
  this->m_StateMachine.SetReadyToRun();
} 

/** Destructor */
ReslicerPlaneSpatialObject
::~ReslicerPlaneSpatialObject()  
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

void 
ReslicerPlaneSpatialObject
::RequestSetReslicingMode( ReslicingMode reslicingMode )
{  
  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
                       ::RequestSetReslicingMode called...\n");

  m_ReslicingModeToBeSet = reslicingMode;
  m_StateMachine.PushInput( m_ValidReslicingModeInput );

  //FIXME: Check conditions for InValidReslicing mode
  m_StateMachine.ProcessInputs();
}

/** Null Operation for a State Machine Transition */
void 
ReslicerPlaneSpatialObject
::NoProcessing()
{
}

void
ReslicerPlaneSpatialObject
::RequestGetToolPosition() 
{
  igstkLogMacro( DEBUG, "igstk::ReslicerPlaneSpatialObject\
                        ::RequestGetToolPosition called...\n");
 
  igstkPushInputMacro( GetToolPosition );
  m_StateMachine.ProcessInputs();
}

void
ReslicerPlaneSpatialObject
::RequestGetVTKPlane() 
{
  igstkLogMacro( DEBUG, "igstk::ReslicerPlaneSpatialObject\
                        ::RequestGetVTKPlane called...\n");
 
  igstkPushInputMacro( GetVTKPlane );
  m_StateMachine.ProcessInputs();
}

void
ReslicerPlaneSpatialObject
::ReportToolPositionProcessing() 
{
  igstkLogMacro( DEBUG, "igstk::ReslicerPlaneSpatialObject\
                        ::ReportToolPositionProcessing called...\n");

  EventHelperType::PointType position;
   
  position[0] = m_ToolPosition[0];
  position[1] = m_ToolPosition[1];
  position[2] = m_ToolPosition[2];

  PointEvent event;
  event.Set( position );
  this->InvokeEvent( event );
}

void
ReslicerPlaneSpatialObject
::ReportVTKPlaneProcessing() 
{
  igstkLogMacro( DEBUG, "igstk::ReslicerPlaneSpatialObject\
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

  // Force the plane to lie within the true bounds along its normal
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

void 
ReslicerPlaneSpatialObject
::RequestSetCursorPosition( const double *point )
{  
  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
                       ::RequestSetCursorPosition called...\n");

  m_CursorPositionToBeSet[0] = point[0];
  m_CursorPositionToBeSet[1] = point[1];
  m_CursorPositionToBeSet[2] = point[2];

  m_StateMachine.PushInput( m_SetCursorPositionInput );

  m_StateMachine.ProcessInputs();
}

void 
ReslicerPlaneSpatialObject
::AttemptSetCursorPositionProcessing()
{

  igstkLogMacro( DEBUG, "igstk::ReslicerPlaneSpatialObject\
                        ::AttemptSetCursorPositionProcessing called...\n");

    bool validPosition = false; 

    switch( m_OrientationType )
      {
      case Axial:
        if( m_CursorPositionToBeSet[2] >= m_ImageBounds[4] && 
            m_CursorPositionToBeSet[2] <= m_ImageBounds[5] )
          {
          validPosition = true;
          }
          break;
      case Sagittal:
        if( m_CursorPositionToBeSet[0] >= m_ImageBounds[0] && 
            m_CursorPositionToBeSet[0] <= m_ImageBounds[1] )
          {
          validPosition = true;
          }
        break;
      case Coronal:
        if( m_CursorPositionToBeSet[1] >= m_ImageBounds[2] && 
            m_CursorPositionToBeSet[1] <= m_ImageBounds[3] )
          {
          validPosition = true;
          }
        break;
      }

    if( validPosition )
      {
      igstkPushInputMacro( ValidCursorPosition );
      }
    else
      {
      igstkPushInputMacro( InValidCursorPosition );
      }

    m_StateMachine.ProcessInputs();
}

void 
ReslicerPlaneSpatialObject
::SetCursorPositionProcessing()
{
  igstkLogMacro( DEBUG, "igstk::ReslicerPlaneSpatialObject\
                        ::SetCursorPosition called...\n");

  m_CursorPosition[0] = m_CursorPositionToBeSet[0];
  m_CursorPosition[1] = m_CursorPositionToBeSet[1];
  m_CursorPosition[2] = m_CursorPositionToBeSet[2];

  m_ToolPosition[0] = m_CursorPosition[0];
  m_ToolPosition[1] = m_CursorPosition[1];
  m_ToolPosition[2] = m_CursorPosition[2];

  //turn on the flag
  m_CursorPositionSetFlag = true;
}

void 
ReslicerPlaneSpatialObject
::SetReslicingModeProcessing()
{
  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
                       ::SetReslicingModeProcessing called...\n");
  m_ReslicingMode = m_ReslicingModeToBeSet;
}

void 
ReslicerPlaneSpatialObject
::RequestSetOrientationType( OrientationType orientationType )
{  
  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
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

void 
ReslicerPlaneSpatialObject
::SetOrientationTypeProcessing()
{
  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
                       ::SetOrientationTypeProcessing called...\n");
  m_OrientationType = m_OrientationTypeToBeSet;
}

//void 
//ReslicerPlaneSpatialObject
//::RequestSetBoundingBoxProviderSpatialObject( const BoundingBoxProviderSpatialObjectType * BoundingBoxProviderSpatialObject )
//{  
//  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
//                       ::RequestSetBoundingBoxProviderSpatialObject called...\n");
//
//  m_BoundingBoxProviderSpatialObjectToBeSet = BoundingBoxProviderSpatialObject;
//
//  if( !m_BoundingBoxProviderSpatialObjectToBeSet )
//    {
//    m_StateMachine.PushInput( m_InValidBoundingBoxProviderSpatialObjectInput );
//    }
//  else
//    {
//    m_StateMachine.PushInput( m_ValidBoundingBoxProviderSpatialObjectInput );
//    }
//
//  m_StateMachine.ProcessInputs();
//}

void
ReslicerPlaneSpatialObject
::RequestSetBoundingBoxProviderSpatialObject( const BoundingBoxProviderSpatialObjectType* spatialObject )
{
  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
                       ::RequestSetBoundingBoxProviderSpatialObject called...\n");

  m_BoundingBoxProviderSpatialObjectToBeSet = const_cast< BoundingBoxProviderSpatialObjectType* >(spatialObject);

  m_StateMachine.PushInput( m_SetBoundingBoxProviderSpatialObjectInput );

  m_StateMachine.ProcessInputs();
}

void
ReslicerPlaneSpatialObject
::AttemptSetBoundingBoxProviderSpatialObjectProcessing( )
{
  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
                       ::AttemptSetBoundingBoxProviderSpatialObject called...\n");

  if( !m_BoundingBoxProviderSpatialObjectToBeSet )
    {
    m_StateMachine.PushInput( m_InValidBoundingBoxProviderSpatialObjectInput );
    }
  else
    {
    m_StateMachine.PushInput( m_ValidBoundingBoxProviderSpatialObjectInput );
    }

  m_StateMachine.ProcessInputs();
}

void 
ReslicerPlaneSpatialObject
::SetBoundingBoxProviderSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
                       ::SetBoundingBoxProviderSpatialObjectProcessing called...\n");

  m_BoundingBoxProviderSpatialObject = m_BoundingBoxProviderSpatialObjectToBeSet;

  // get the bounding box from the reference spatial object
  BoundingBoxObserver::Pointer  boundingBoxObserver = BoundingBoxObserver::New();
  boundingBoxObserver->Reset();

  unsigned long boundingBoxObserverID = 
  m_BoundingBoxProviderSpatialObject->AddObserver( BoundingBoxProviderSpatialObjectType::BoundingBoxEvent(), boundingBoxObserver );
  m_BoundingBoxProviderSpatialObject->RequestGetBounds();

  if( boundingBoxObserver->GotBoundingBox() ) 
  {
    m_BoundingBox = boundingBoxObserver->GetBoundingBox();

    const BoundingBoxType::BoundsArrayType &bounds = m_BoundingBox->GetBounds();

    m_ImageBounds[0] = bounds[0];
    m_ImageBounds[1] = bounds[1];
    m_ImageBounds[2] = bounds[2];
    m_ImageBounds[3] = bounds[3];
    m_ImageBounds[4] = bounds[4];
    m_ImageBounds[5] = bounds[5];

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
    m_BoundsCenters.push_back(auxvec);
    auxnorm[0] = 1;
    auxnorm[1] = 0;
    auxnorm[2] = 0;
    m_BoundsNormals.push_back(auxnorm);

    auxvec.Fill(0);
    auxvec[0] = m_ImageBounds[1];
    auxvec[1] = 0.5 * ( m_ImageBounds[2] + m_ImageBounds[3] );
    auxvec[2] = 0.5 * ( m_ImageBounds[4] + m_ImageBounds[5] );
    m_BoundsCenters.push_back(auxvec);
    auxnorm[0] = -1;
    auxnorm[1] = 0;
    auxnorm[2] = 0;
    m_BoundsNormals.push_back(auxnorm);

    auxvec.Fill(0);
    auxvec[0] = 0.5 * ( m_ImageBounds[0] + m_ImageBounds[1] );
    auxvec[1] = m_ImageBounds[2];
    auxvec[2] = 0.5 * ( m_ImageBounds[4] + m_ImageBounds[5] );
    m_BoundsCenters.push_back(auxvec);
    auxnorm[0] = 0;
    auxnorm[1] = 1;
    auxnorm[2] = 0;
    m_BoundsNormals.push_back(auxnorm);

    auxvec.Fill(0);
    auxvec[0] = 0.5 * ( m_ImageBounds[0] + m_ImageBounds[1] );
    auxvec[1] = m_ImageBounds[3];
    auxvec[2] = 0.5 * ( m_ImageBounds[4] + m_ImageBounds[5] );
    m_BoundsCenters.push_back(auxvec);
    auxnorm[0] = 0;
    auxnorm[1] = -1;
    auxnorm[2] = 0;
    m_BoundsNormals.push_back(auxnorm);

    auxvec.Fill(0);
    auxvec[0] = 0.5 * ( m_ImageBounds[0] + m_ImageBounds[1] );
    auxvec[1] = 0.5 * ( m_ImageBounds[2] + m_ImageBounds[3] );
    auxvec[2] = m_ImageBounds[4];
    m_BoundsCenters.push_back(auxvec);
    auxnorm[0] = 0;
    auxnorm[1] = 0;
    auxnorm[2] = 1;
    m_BoundsNormals.push_back(auxnorm);

    auxvec.Fill(0);
    auxvec[0] = 0.5 * ( m_ImageBounds[0] + m_ImageBounds[1] );
    auxvec[1] = 0.5 * ( m_ImageBounds[2] + m_ImageBounds[3] );
    auxvec[2] = m_ImageBounds[5];
    m_BoundsCenters.push_back(auxvec);
    auxnorm[0] = 0;
    auxnorm[1] = 0;
    auxnorm[2] = -1;
    m_BoundsNormals.push_back(auxnorm);

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

     m_PlaneSource->SetNormal(m_PlaneNormal[0],m_PlaneNormal[1],m_PlaneNormal[2]);
     m_PlaneSource->SetOrigin(m_PlaneOrigin[0],m_PlaneOrigin[1],m_PlaneOrigin[2]);
     m_PlaneSource->SetPoint1(m_PlanePoint1[0],m_PlanePoint1[1],m_PlanePoint1[2]);
     m_PlaneSource->SetPoint2(m_PlanePoint2[0],m_PlanePoint2[1],m_PlanePoint2[2]);
  }
}

void 
ReslicerPlaneSpatialObject
::ReportInvalidBoundingBoxProviderSpatialObjectProcessing( )
{
  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
                       ::ReportInvalidBoundingBoxProviderSpatialObjectProcessing called...\n");
}

void 
ReslicerPlaneSpatialObject
::ReportInvalidCursorPositionProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
                       ::ReportInvalidCursorPositionProcessing called...\n");
}

void 
ReslicerPlaneSpatialObject
::RequestSetToolSpatialObject( const ToolSpatialObjectType * toolSpatialObject )
{  
  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
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

void 
ReslicerPlaneSpatialObject
::SetToolSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
                       ::SetToolSpatialObjectProcessing called...\n");

  m_ToolSpatialObject = m_ToolSpatialObjectToBeSet;
  this->ObserveToolTransformWRTImageCoordinateSystemInput( this->m_ToolSpatialObject );
  m_ToolSpatialObjectSet = true;
}

void 
ReslicerPlaneSpatialObject
::ReportInvalidToolSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
                       ::ReportInvalidToolSpatialObjectProcessing called...\n");
}

void
ReslicerPlaneSpatialObject
::RequestUpdateToolTransformWRTImageCoordinateSystem()
{
  igstkLogMacro( DEBUG,
                 "igstk::ReslicerPlaneSpatialObject::\
                 RequestUpdateToolTransformWRTImageCoordinateSystem called ...\n");
  
  igstkPushInputMacro( GetToolTransformWRTImageCoordinateSystem );
  this->m_StateMachine.ProcessInputs();
}

/** Request to receive the tool transform WRT reference spatial object coordinate system */
void
ReslicerPlaneSpatialObject
::RequestGetToolTransformWRTImageCoordinateSystemProcessing()
{
  igstkLogMacro( DEBUG,
                 "igstk::ReslicerPlaneSpatialObject::\
                 RequestGetToolTransformWRTImageCoordinateSystemProcessing called ...\n");

  typedef igstk::Friends::CoordinateSystemHelper 
                          CoordinateSystemHelperType;

  const CoordinateSystem* ImageSpatialObjectCoordinateSystem = 
    CoordinateSystemHelperType::GetCoordinateSystem( m_BoundingBoxProviderSpatialObject );

  CoordinateSystem * ImageSpatialObjectCoordinateSystemNC =
          const_cast< CoordinateSystem *>(ImageSpatialObjectCoordinateSystem);

  m_ToolSpatialObject->RequestComputeTransformTo( ImageSpatialObjectCoordinateSystemNC ); 
}

/** Receive the tool spatial object transform WRT reference sapatial object
 * coordinate system using a transduction macro */
void
ReslicerPlaneSpatialObject
::ReceiveToolTransformWRTImageCoordinateSystemProcessing()
{
  igstkLogMacro( DEBUG, "ReceiveToolTransformWRTImageCoordinateSystemProcessing " 
                 << this->m_ToolTransformWRTImageCoordinateSystem );

  this->m_ToolTransformWRTImageCoordinateSystem =
    this->m_ToolTransformWRTImageCoordinateSystemInputToBeSet.GetTransform();
}

/** Request compute reslicing plane */
void
ReslicerPlaneSpatialObject
::RequestComputeReslicingPlane( )
{
  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
                       ::RequestComputeReslicingPlane called...\n");

  igstkPushInputMacro( ComputeReslicePlane );
  m_StateMachine.ProcessInputs();

}

/** Compute reslicing plane */
void
ReslicerPlaneSpatialObject
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
void
ReslicerPlaneSpatialObject
::ComputeOrthogonalReslicingPlane( )
{
  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
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
        // Otherwise, use the slice number or Cursor postion and image bounds to set the center
        switch( m_OrientationType )
        {
            case Axial:
            {
              m_PlaneNormal[0] = 0.0;
              m_PlaneNormal[1] = 0.0;
              m_PlaneNormal[2] = 1.0;

              m_PlaneCenter[0] = 0.5*(m_ImageBounds[0] + m_ImageBounds[1]);
              m_PlaneCenter[1] = 0.5*(m_ImageBounds[2] + m_ImageBounds[3]);
              
              if ( m_CursorPositionSetFlag )
              {
              m_PlaneCenter[2] = m_CursorPosition[2];
              m_ToolPosition[2] = m_CursorPosition[2];
              m_CursorPositionSetFlag = false;
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

              if ( m_CursorPositionSetFlag )
              {
                m_PlaneCenter[0] = m_CursorPosition[0];
                m_ToolPosition[0] = m_CursorPosition[0];
                m_CursorPositionSetFlag = false;
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

              if ( m_CursorPositionSetFlag )
              {
                  m_PlaneCenter[1] = m_CursorPosition[1];
                  m_ToolPosition[1] = m_CursorPosition[1];
                  m_CursorPositionSetFlag = false;
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
void
ReslicerPlaneSpatialObject
::ComputeObliqueReslicingPlane( )
{
 igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
                       ::ComputeObliqueReslicingPlane called...\n");

  /* Calculate the tool's long axis vector */
 
  igstk::Transform::VectorType   probeVector;
  probeVector.Fill(0.0);
  // we make the assumption that the tool's long spatial object is on the -x axis
  // with the tip in (0,0,0)
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
void
ReslicerPlaneSpatialObject
::ComputeOffOrthogonalReslicingPlane( )
{
  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
                       ::ComputeOffOrthogonalReslicingPlane called...\n");

  /* Calculate the tool's long axis vector */
  igstk::Transform::VectorType   probeVector;
  probeVector.Fill(0.0);
  // fixme: the tool´s long axis direction changes depending on the spatial object definition 
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
* pv: is the tool´s long axis vector
* pi: is the plane index in the bounding box
*/
double
ReslicerPlaneSpatialObject
::GetDistanceToPlane(VectorType p, VectorType pv, unsigned int pi)
{
  double u = 0.0;

  // first check that p does not lay on the plane pi
  if ( fabs( (p - m_BoundsCenters[pi])*m_BoundsNormals[pi] ) < 1e-9 )
    return u;

  u = (m_BoundsNormals[pi]*m_BoundsCenters[pi] - m_BoundsCenters[pi]*p)/
    (m_BoundsNormals[pi]*pv);

  return u;
}

/** Get reslicing plane equation */
//vtkPlaneSource *
//ReslicerPlaneSpatialObject
//::GetReslicingPlane()
//{
//  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
//                       ::GetReslicingPlane called...\n");
//  return m_PlaneSource;
//}

///** Request Get reslicing axes matrix */
//vtkMatrix4x4 *
//ReslicerPlaneSpatialObject
//::GetResliceAxes()
//{
//  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
//                       ::GetResliceAxes called...\n");
//  return m_ResliceAxes;
//}

/** Request Get reslicing transform*/
//vtkTransform *
//ReslicerPlaneSpatialObject
//::GetResliceTransform()
//{
//  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
//                       ::GetResliceTransform called...\n");
//  return m_ResliceTransform;
//}

/** Report invalid reslicing mode */
void
ReslicerPlaneSpatialObject
::ReportInvalidReslicingModeProcessing( void )
{
  igstkLogMacro( WARNING, 
    "igstk::ReslicerPlaneSpatialObject::ReportInvalidReslicingModeProcessing called...\n");
}

/** Report invalid orientation type */
void
ReslicerPlaneSpatialObject
::ReportInvalidOrientationTypeProcessing( void )
{
  igstkLogMacro( WARNING, 
    "igstk::ReslicerPlaneSpatialObject::ReportInvalidOrientationTypeProcessing called...\n");
}

/** Report invalid request */
void 
ReslicerPlaneSpatialObject
::ReportInvalidRequestProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::ReslicerPlaneSpatialObject::ReportInvalidRequestProcessing called...\n");

  this->InvokeEvent( InvalidRequestErrorEvent() );
}

/** Get tool transform WRT Image Coordinate System*/
igstk::Transform
ReslicerPlaneSpatialObject
::GetToolTransform( ) const
{
  return this->m_ToolTransformWRTImageCoordinateSystem;
}

/** Check if tool spatial object is set to drive the reslicing*/
bool
ReslicerPlaneSpatialObject
::IsToolSpatialObjectSet( ) 
{
  return this->m_ToolSpatialObjectSet;
}

/** Print object information */
void
ReslicerPlaneSpatialObject
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace igstk


#endif
