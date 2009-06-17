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

#include "vtkPlaneSource.h"
#include "vtkPlane.h"
#include "vtkMath.h"
#include "vtkTransform.h"

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
  igstkAddInputMacro( GetReslicingPlaneParameters );
  igstkAddInputMacro( GetToolTransformWRTImageCoordinateSystem );
  igstkAddInputMacro( ToolTransformWRTImageCoordinateSystem );
  igstkAddInputMacro( ComputeReslicePlane );


  // List of state machine transitions

  //From Initial
  igstkAddTransitionMacro( Initial, ValidReslicingMode, 
                           ReslicingModeSet, SetReslicingMode );

  igstkAddTransitionMacro( Initial, InValidReslicingMode, 
                           Initial, ReportInvalidReslicingMode );

  igstkAddTransitionMacro( Initial, SetBoundingBoxProviderSpatialObject, 
                           Initial, ReportInvalidRequest);

  igstkAddTransitionMacro( Initial, ValidOrientationType, 
                           Initial, ReportInvalidRequest);

  igstkAddTransitionMacro( Initial, InValidOrientationType, 
                           Initial, ReportInvalidRequest);  

  igstkAddTransitionMacro( Initial, ValidBoundingBoxProviderSpatialObject, 
                           Initial, ReportInvalidRequest);  

  igstkAddTransitionMacro( Initial, InValidBoundingBoxProviderSpatialObject, 
                           Initial, ReportInvalidRequest);

  igstkAddTransitionMacro( Initial, ValidToolSpatialObject, 
                           Initial, ReportInvalidRequest);  

  igstkAddTransitionMacro( Initial, InValidToolSpatialObject, 
                           Initial, ReportInvalidRequest);  

  igstkAddTransitionMacro( Initial, SetCursorPosition, 
                           Initial, ReportInvalidRequest);

  igstkAddTransitionMacro( Initial, ValidCursorPosition, 
                           Initial, ReportInvalidRequest);

  igstkAddTransitionMacro( Initial, InValidCursorPosition, 
                           Initial, ReportInvalidRequest);

  igstkAddTransitionMacro( Initial, GetToolPosition, 
                           Initial, ReportInvalidRequest);

  igstkAddTransitionMacro( Initial, GetReslicingPlaneParameters, 
                           Initial, ReportInvalidRequest);

  igstkAddTransitionMacro( Initial, GetToolTransformWRTImageCoordinateSystem, 
                           Initial, ReportInvalidRequest);

  igstkAddTransitionMacro( Initial, ToolTransformWRTImageCoordinateSystem, 
                           Initial, ReportInvalidRequest);

  igstkAddTransitionMacro( Initial, ComputeReslicePlane, 
                           Initial, ReportInvalidRequest);

  //From ReslicingModeSet
  igstkAddTransitionMacro( ReslicingModeSet, ValidOrientationType, 
                           OrientationTypeSet, SetOrientationType );

  igstkAddTransitionMacro( ReslicingModeSet, InValidOrientationType, 
                           ReslicingModeSet, ReportInvalidOrientationType);

  igstkAddTransitionMacro( ReslicingModeSet, SetCursorPosition, 
                           ReslicingModeSet, ReportInvalidRequest );  

  igstkAddTransitionMacro( ReslicingModeSet, 
                           SetBoundingBoxProviderSpatialObject,
                           ReslicingModeSet, 
                           ReportInvalidRequest);

  igstkAddTransitionMacro( ReslicingModeSet, 
                           ValidBoundingBoxProviderSpatialObject, 
                           ReslicingModeSet, 
                           ReportInvalidRequest);  

  igstkAddTransitionMacro( ReslicingModeSet, 
                           InValidBoundingBoxProviderSpatialObject, 
                           ReslicingModeSet, ReportInvalidRequest);

  igstkAddTransitionMacro( ReslicingModeSet, ValidToolSpatialObject, 
                           ReslicingModeSet, ReportInvalidRequest);  

  igstkAddTransitionMacro( ReslicingModeSet, InValidToolSpatialObject, 
                           ReslicingModeSet, ReportInvalidRequest);  

  igstkAddTransitionMacro( ReslicingModeSet, ValidCursorPosition, 
                           ReslicingModeSet, ReportInvalidRequest);

  igstkAddTransitionMacro( ReslicingModeSet, InValidCursorPosition, 
                           ReslicingModeSet, ReportInvalidRequest);

  igstkAddTransitionMacro( ReslicingModeSet, GetToolPosition, 
                           ReslicingModeSet, ReportInvalidRequest);

  igstkAddTransitionMacro( ReslicingModeSet, GetReslicingPlaneParameters, 
                           ReslicingModeSet, ReportInvalidRequest);

  igstkAddTransitionMacro( ReslicingModeSet, 
                           GetToolTransformWRTImageCoordinateSystem, 
                           ReslicingModeSet, ReportInvalidRequest);

  igstkAddTransitionMacro( ReslicingModeSet, 
                           ToolTransformWRTImageCoordinateSystem, 
                           ReslicingModeSet, ReportInvalidRequest);

  igstkAddTransitionMacro( ReslicingModeSet, ComputeReslicePlane, 
                           ReslicingModeSet, ReportInvalidRequest);

  //From OrientationTypeSet
  igstkAddTransitionMacro( OrientationTypeSet, 
                           SetBoundingBoxProviderSpatialObject,
                           AttemptingToSetBoundingBoxProviderSpatialObject, 
                           AttemptSetBoundingBoxProviderSpatialObject );

  igstkAddTransitionMacro( OrientationTypeSet, SetCursorPosition, 
                           OrientationTypeSet, ReportInvalidRequest );

  igstkAddTransitionMacro( OrientationTypeSet, ValidOrientationType, 
                           OrientationTypeSet, SetOrientationType );

  igstkAddTransitionMacro( OrientationTypeSet, InValidOrientationType, 
                           OrientationTypeSet, ReportInvalidOrientationType);

  igstkAddTransitionMacro( OrientationTypeSet, 
                           ValidBoundingBoxProviderSpatialObject, 
                           OrientationTypeSet, ReportInvalidRequest);  

  igstkAddTransitionMacro( OrientationTypeSet, 
                           InValidBoundingBoxProviderSpatialObject, 
                           OrientationTypeSet, ReportInvalidRequest);

  igstkAddTransitionMacro( OrientationTypeSet, ValidToolSpatialObject, 
                           OrientationTypeSet, ReportInvalidRequest);  

  igstkAddTransitionMacro( OrientationTypeSet, InValidToolSpatialObject, 
                           OrientationTypeSet, ReportInvalidRequest);  

  igstkAddTransitionMacro( OrientationTypeSet, ValidCursorPosition, 
                           OrientationTypeSet, ReportInvalidRequest);

  igstkAddTransitionMacro( OrientationTypeSet, InValidCursorPosition, 
                           OrientationTypeSet, ReportInvalidRequest);

  igstkAddTransitionMacro( OrientationTypeSet, GetToolPosition, 
                           OrientationTypeSet, ReportInvalidRequest);

  igstkAddTransitionMacro( OrientationTypeSet, GetReslicingPlaneParameters, 
                           OrientationTypeSet, ReportInvalidRequest);

  igstkAddTransitionMacro( OrientationTypeSet, 
                           GetToolTransformWRTImageCoordinateSystem, 
                           OrientationTypeSet, ReportInvalidRequest);

  igstkAddTransitionMacro( OrientationTypeSet, 
                           ToolTransformWRTImageCoordinateSystem, 
                           OrientationTypeSet, ReportInvalidRequest);

  igstkAddTransitionMacro( OrientationTypeSet, ComputeReslicePlane, 
                           OrientationTypeSet, ReportInvalidRequest);


  // From AttemptingToSetBoundingBoxProviderSpatialObject
  igstkAddTransitionMacro( AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ValidBoundingBoxProviderSpatialObject,
                           BoundingBoxProviderSpatialObjectSet,  
                           SetBoundingBoxProviderSpatialObject ); 

  igstkAddTransitionMacro( AttemptingToSetBoundingBoxProviderSpatialObject, 
                           InValidBoundingBoxProviderSpatialObject,
                           OrientationTypeSet,  
                           ReportInvalidBoundingBoxProviderSpatialObject );

  igstkAddTransitionMacro( AttemptingToSetBoundingBoxProviderSpatialObject, 
                           SetBoundingBoxProviderSpatialObject,
                           AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToSetBoundingBoxProviderSpatialObject, 
                           SetCursorPosition, 
                           AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ValidOrientationType, 
                           AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToSetBoundingBoxProviderSpatialObject, 
                           InValidOrientationType, 
                           AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ValidBoundingBoxProviderSpatialObject, 
                           AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ReportInvalidRequest);  

  igstkAddTransitionMacro( AttemptingToSetBoundingBoxProviderSpatialObject, 
                           InValidBoundingBoxProviderSpatialObject, 
                           AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ValidToolSpatialObject, 
                           AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ReportInvalidRequest);  

  igstkAddTransitionMacro( AttemptingToSetBoundingBoxProviderSpatialObject, 
                           InValidToolSpatialObject, 
                           AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ReportInvalidRequest);  

  igstkAddTransitionMacro( AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ValidCursorPosition, 
                           AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToSetBoundingBoxProviderSpatialObject, 
                           InValidCursorPosition, 
                           AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToSetBoundingBoxProviderSpatialObject, 
                           GetToolPosition, 
                           AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToSetBoundingBoxProviderSpatialObject, 
                           GetReslicingPlaneParameters, 
                           AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToSetBoundingBoxProviderSpatialObject, 
                           GetToolTransformWRTImageCoordinateSystem, 
                           AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ToolTransformWRTImageCoordinateSystem, 
                           AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ComputeReslicePlane, 
                           AttemptingToSetBoundingBoxProviderSpatialObject, 
                           ReportInvalidRequest);

  //From BoundingBoxProviderSpatialObjectSet
  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, 
                           ValidOrientationType,
                           BoundingBoxProviderSpatialObjectSet, 
                           SetOrientationType );

  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, 
                           InValidOrientationType,
                           BoundingBoxProviderSpatialObjectSet, 
                           ReportInvalidOrientationType );

  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, 
                           ValidReslicingMode, 
                           BoundingBoxProviderSpatialObjectSet, 
                           SetReslicingMode );

  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, 
                           InValidReslicingMode, 
                           BoundingBoxProviderSpatialObjectSet, 
                           ReportInvalidReslicingMode );

  //  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, 
  //                           ValidCursorPosition,
  //                           BoundingBoxProviderSpatialObjectSet,  
  //                           SetCursorPosition ); 

  //  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, 
  //                           InValidCursorPosition,
  //                           BoundingBoxProviderSpatialObjectSet,  
  //                           ReportInvalidCursorPosition );

  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, 
                           ValidToolSpatialObject,
                           BoundingBoxProviderSpatialObjectSet, 
                           SetToolSpatialObject );

  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, 
                           InValidToolSpatialObject,
                           BoundingBoxProviderSpatialObjectSet, 
                           ReportInvalidToolSpatialObject );  

  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, 
                           SetCursorPosition,
                           AttemptingToSetCursorPosition, 
                           AttemptSetCursorPosition );  

  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, 
                           ComputeReslicePlane,
                           BoundingBoxProviderSpatialObjectSet, 
                           ComputeReslicePlane );

  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, 
                           GetToolPosition,
                           BoundingBoxProviderSpatialObjectSet, 
                           ReportToolPosition );

  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, 
                           GetReslicingPlaneParameters,
                           BoundingBoxProviderSpatialObjectSet, 
                           ReportReslicingPlaneParameters );

  igstkAddTransitionMacro( BoundingBoxProviderSpatialObjectSet, 
                           GetToolTransformWRTImageCoordinateSystem,
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           RequestGetToolTransformWRTImageCoordinateSystem );

  // From AttemptingToSetCursorPosition
  igstkAddTransitionMacro( AttemptingToSetCursorPosition, 
                           ValidCursorPosition,
                           BoundingBoxProviderSpatialObjectSet, 
                           SetCursorPosition ); 

  igstkAddTransitionMacro( AttemptingToSetCursorPosition, 
                           InValidCursorPosition,
                           BoundingBoxProviderSpatialObjectSet, 
                           ReportInvalidCursorPosition );

  igstkAddTransitionMacro( AttemptingToSetCursorPosition, 
                           ValidBoundingBoxProviderSpatialObject,
                           AttemptingToSetCursorPosition, 
                           ReportInvalidRequest ); 

  igstkAddTransitionMacro( AttemptingToSetCursorPosition, 
                           InValidBoundingBoxProviderSpatialObject,
                           AttemptingToSetCursorPosition, 
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToSetCursorPosition, 
                           SetBoundingBoxProviderSpatialObject,
                           AttemptingToSetCursorPosition, 
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToSetCursorPosition, SetCursorPosition, 
                           AttemptingToSetCursorPosition, 
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToSetCursorPosition, ValidOrientationType, 
                           AttemptingToSetCursorPosition, 
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingToSetCursorPosition, 
                           InValidOrientationType, 
                           AttemptingToSetCursorPosition, ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToSetCursorPosition, 
                           ValidBoundingBoxProviderSpatialObject, 
                           AttemptingToSetCursorPosition, ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToSetCursorPosition, 
                           InValidBoundingBoxProviderSpatialObject, 
                           AttemptingToSetCursorPosition, ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToSetCursorPosition, 
                           ValidToolSpatialObject, 
                           AttemptingToSetCursorPosition, ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToSetCursorPosition, 
                           InValidToolSpatialObject, 
                           AttemptingToSetCursorPosition, ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToSetCursorPosition, GetToolPosition, 
                           AttemptingToSetCursorPosition, ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToSetCursorPosition, 
                           GetReslicingPlaneParameters, 
                           AttemptingToSetCursorPosition, ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToSetCursorPosition,
                           GetToolTransformWRTImageCoordinateSystem, 
                           AttemptingToSetCursorPosition, ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToSetCursorPosition, 
                           ToolTransformWRTImageCoordinateSystem, 
                           AttemptingToSetCursorPosition, ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToSetCursorPosition, ComputeReslicePlane, 
                           AttemptingToSetCursorPosition, ReportInvalidRequest);


  // From AttemptingToGetToolTransformWRTImageCoordinateSystem
  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ToolTransformWRTImageCoordinateSystem,
                           BoundingBoxProviderSpatialObjectSet,
                           ReceiveToolTransformWRTImageCoordinateSystem );

  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ValidReslicingMode, 
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           SetReslicingMode );

  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           InValidReslicingMode, 
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ReportInvalidReslicingMode );

  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           SetBoundingBoxProviderSpatialObject, 
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ValidOrientationType, 
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           InValidOrientationType, 
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ReportInvalidRequest);  

  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ValidBoundingBoxProviderSpatialObject, 
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ReportInvalidRequest);  

  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           InValidBoundingBoxProviderSpatialObject, 
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ValidToolSpatialObject, 
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ReportInvalidRequest);  

  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           InValidToolSpatialObject, 
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ReportInvalidRequest);  

  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           SetCursorPosition, 
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ValidCursorPosition, 
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           InValidCursorPosition, 
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           GetToolPosition, 
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           GetReslicingPlaneParameters, 
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           GetToolTransformWRTImageCoordinateSystem, 
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ReportInvalidRequest);

  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ComputeReslicePlane, 
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                           ReportInvalidRequest);

  igstkSetInitialStateMacro( Initial );
  this->m_StateMachine.SetReadyToRun();
} 

/** Destructor */
ReslicerPlaneSpatialObject
::~ReslicerPlaneSpatialObject()  
{  

}

void 
ReslicerPlaneSpatialObject
::RequestSetReslicingMode( ReslicingMode reslicingMode )
{  
  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
                       ::RequestSetReslicingMode called...\n");

  bool validReslicingMode = false;

  if( reslicingMode == Orthogonal )
    {
     validReslicingMode = true;
    }

  if( reslicingMode == OffOrthogonal )
    {
     validReslicingMode = true;
    }

  if( reslicingMode == Oblique )
    {
     validReslicingMode = true;
    }

  if ( validReslicingMode )
    {
    m_ReslicingModeToBeSet = reslicingMode;
    m_StateMachine.PushInput( m_ValidReslicingModeInput );
    }
  else
    {
    m_StateMachine.PushInput( m_InValidReslicingModeInput );
    }

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
::RequestGetReslicingPlaneParameters() 
{
  igstkLogMacro( DEBUG, "igstk::ReslicerPlaneSpatialObject\
                        ::RequestGetReslicingPlaneParameters called...\n");
 
  igstkPushInputMacro( GetReslicingPlaneParameters );
  m_StateMachine.ProcessInputs();
}

void
ReslicerPlaneSpatialObject
::ReportToolPositionProcessing() 
{
  igstkLogMacro( DEBUG, "igstk::ReslicerPlaneSpatialObject\
                        ::ReportToolPositionProcessing called...\n");

  ToolTipPositionEvent event;
  event.Set( m_ToolPosition );
  this->InvokeEvent( event );
}

ReslicerPlaneSpatialObject::ReslicingMode 
ReslicerPlaneSpatialObject
::GetReslicingMode() const
{ 
  igstkLogMacro( DEBUG, "igstk::ReslicerPlaneSpatialObject\
                        ::GetReslicingMode called...\n");

  return m_ReslicingMode; 
}

ReslicerPlaneSpatialObject::OrientationType
ReslicerPlaneSpatialObject
::GetOrientationType() const
{ 
  igstkLogMacro( DEBUG, "igstk::ReslicerPlaneSpatialObject\
                        ::GetOrientationType called...\n");

  return m_OrientationType; 
}

void
ReslicerPlaneSpatialObject
::ReportReslicingPlaneParametersProcessing() 
{
  igstkLogMacro( DEBUG, "igstk::ReslicerPlaneSpatialObject\
                       ::ReportReslicingPlaneParametersProcessing called...\n");

  ReslicerPlaneCenterEvent pcEvent;
  pcEvent.Set( m_PlaneCenter );
  this->InvokeEvent( pcEvent );

  ReslicerPlaneNormalEvent pnEvent;
  pnEvent.Set( m_PlaneNormal );
  this->InvokeEvent( pnEvent );

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
      if( m_CursorPositionToBeSet[2] >= m_Bounds[4] && 
          m_CursorPositionToBeSet[2] <= m_Bounds[5] )
        {
        validPosition = true;
        }
        break;
    case Sagittal:
      if( m_CursorPositionToBeSet[0] >= m_Bounds[0] && 
          m_CursorPositionToBeSet[0] <= m_Bounds[1] )
        {
        validPosition = true;
        }
      break;
    case Coronal:
      if( m_CursorPositionToBeSet[1] >= m_Bounds[2] && 
          m_CursorPositionToBeSet[1] <= m_Bounds[3] )
        {
        validPosition = true;
        }
      break;
    default:
      {
      validPosition = false;
      }
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

void
ReslicerPlaneSpatialObject
::RequestSetBoundingBoxProviderSpatialObject( 
                     const BoundingBoxProviderSpatialObjectType* spatialObject )
{
  igstkLogMacro( DEBUG,"igstk::ReslicerPlaneSpatialObject\
                    ::RequestSetBoundingBoxProviderSpatialObject called...\n");

  m_BoundingBoxProviderSpatialObjectToBeSet = 
             const_cast< BoundingBoxProviderSpatialObjectType* >(spatialObject);

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

  m_BoundingBoxProviderSpatialObject= m_BoundingBoxProviderSpatialObjectToBeSet;

  // get the bounding box from the reference spatial object
  BoundingBoxObserver::Pointer boundingBoxObserver = BoundingBoxObserver::New();
  boundingBoxObserver->Reset();

  unsigned long boundingBoxObserverID = 
  m_BoundingBoxProviderSpatialObject->AddObserver( 
                       BoundingBoxProviderSpatialObjectType::BoundingBoxEvent(),
                                                          boundingBoxObserver );
  m_BoundingBoxProviderSpatialObject->RequestGetBounds();

  if( !boundingBoxObserver->GotBoundingBox() ) 
  return;

  m_BoundingBox = boundingBoxObserver->GetBoundingBox();

  if ( m_BoundingBox.IsNull() )
  return;

  m_BoundingBoxProviderSpatialObject->RemoveObserver( boundingBoxObserverID );

  const BoundingBoxType::BoundsArrayType &bounds = m_BoundingBox->GetBounds();

  m_Bounds[0] = bounds[0];
  m_Bounds[1] = bounds[1];
  m_Bounds[2] = bounds[2];
  m_Bounds[3] = bounds[3];
  m_Bounds[4] = bounds[4];
  m_Bounds[5] = bounds[5];

  for ( unsigned int i = 0; i <= 4; i += 2 ) // reverse bounds if necessary
    {
    if ( m_Bounds[i] > m_Bounds[i+1] )
      {
      double t = m_Bounds[i+1];
      m_Bounds[i+1] = m_Bounds[i];
      m_Bounds[i] = t;
      }
    }

  // we start in the middle of the bounding box
  m_ToolPosition[0] = 0.5*(m_Bounds[0] + m_Bounds[1]);
  m_ToolPosition[1] = 0.5*(m_Bounds[2] + m_Bounds[3]);
  m_ToolPosition[2] = 0.5*(m_Bounds[4] + m_Bounds[5]);
  
  m_PlaneCenter[0] = m_ToolPosition[0];
  m_PlaneCenter[1] = m_ToolPosition[1];
  m_PlaneCenter[2] = m_ToolPosition[2];

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

  m_ToolSpatialObjectToBeSet = const_cast< ToolSpatialObjectType *>(
                                                             toolSpatialObject);

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
  this->ObserveToolTransformWRTImageCoordinateSystemInput( 
                                                    this->m_ToolSpatialObject );
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

/** Request to receive the tool transform WRT reference spatial object 
 * coordinate system */
void
ReslicerPlaneSpatialObject
::RequestGetToolTransformWRTImageCoordinateSystemProcessing()
{
  igstkLogMacro( DEBUG,
                 "igstk::ReslicerPlaneSpatialObject::\
       RequestGetToolTransformWRTImageCoordinateSystemProcessing called ...\n");

  typedef igstk::Friends::CoordinateSystemHelper     CoordinateSystemHelperType;

  const CoordinateSystem* ImageSpatialObjectCoordinateSystem = 
    CoordinateSystemHelperType::GetCoordinateSystem( 
                                           m_BoundingBoxProviderSpatialObject );

  CoordinateSystem * ImageSpatialObjectCoordinateSystemNC =
          const_cast< CoordinateSystem *>(ImageSpatialObjectCoordinateSystem);

  m_ToolSpatialObject->RequestComputeTransformTo( 
                                         ImageSpatialObjectCoordinateSystemNC );
}

/** Receive the tool spatial object transform WRT reference sapatial object
 * coordinate system using a transduction macro */
void
ReslicerPlaneSpatialObject
::ReceiveToolTransformWRTImageCoordinateSystemProcessing()
{
  igstkLogMacro( DEBUG, "ReceiveToolTransformWRTImageCoordinateSystemProcessing"
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
    VectorType   translation;
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

        m_PlaneCenter[0] = 0.5*(m_Bounds[0]+m_Bounds[1]);
        m_PlaneCenter[1] = 0.5*(m_Bounds[2]+m_Bounds[3]);

        break;
        }

      case Sagittal:
        {
        m_PlaneNormal[0] = 1.0;
        m_PlaneNormal[1] = 0.0;
        m_PlaneNormal[2] = 0.0;

        m_PlaneCenter[1] = 0.5*(m_Bounds[2]+m_Bounds[3]);
        m_PlaneCenter[2] = 0.5*(m_Bounds[4]+m_Bounds[5]);

        break;
        }

      case Coronal:
        {
        m_PlaneNormal[0] = 0.0;
        m_PlaneNormal[1] = 1.0;
        m_PlaneNormal[2] = 0.0;

        m_PlaneCenter[0] = 0.5*(m_Bounds[0]+m_Bounds[1]);
        m_PlaneCenter[2] = 0.5*(m_Bounds[4]+m_Bounds[5]);

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
    // Otherwise, use the cursor postion and image bounds to set the center
    switch( m_OrientationType )
      {
      case Axial:
        {
        m_PlaneNormal[0] = 0.0;
        m_PlaneNormal[1] = 0.0;
        m_PlaneNormal[2] = 1.0;

        m_PlaneCenter[0] = 0.5*(m_Bounds[0] + m_Bounds[1]);
        m_PlaneCenter[1] = 0.5*(m_Bounds[2] + m_Bounds[3]);
        
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

        m_PlaneCenter[1] = 0.5*(m_Bounds[2] + m_Bounds[3]);
        m_PlaneCenter[2] = 0.5*(m_Bounds[4] + m_Bounds[5]);

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

        m_PlaneCenter[0] = 0.5*(m_Bounds[0] + m_Bounds[1]);
        m_PlaneCenter[2] = 0.5*(m_Bounds[4] + m_Bounds[5]);

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

  /* Calculate the tool's long axis vector.
  * we make the assumption that the tool's long axis is on the -x axis
  * with the tip in (0,0,0)
  */
  const VersorType& rotation = 
                          m_ToolTransformWRTImageCoordinateSystem.GetRotation();
  // auxiliary vecs
  VectorType v1, v2, vn;
  vn.Fill(0.0);
  vn[0] = 1;
  vn = rotation.Transform(vn);

  const VectorType& translation = 
                       m_ToolTransformWRTImageCoordinateSystem.GetTranslation();

  m_ToolPosition[0] = translation[0];
  m_ToolPosition[1] = translation[1];
  m_ToolPosition[2] = translation[2];

  m_PlaneCenter[0] = translation[0];
  m_PlaneCenter[1] = translation[1];
  m_PlaneCenter[2] = translation[2];

  // get any normal vector to the tool's long axis
  // fixme: we could make use of the 6DOF by converting the
  // orthonormal base (1,0,0) (0,1,0) (0,0,1) but it's too noisy ...
  v1[0] = -vn[1];
  v1[1] = vn[0];
  v1[2] = 0;

  // get a second normal vector
  v2 = itk::CrossProduct( v1, vn );

  switch( m_OrientationType )
    {
    case PlaneOrientationWithXAxesNormal:
      { 
      m_PlaneNormal = v1;
      break;
      }

    case PlaneOrientationWithYAxesNormal:
      {
      m_PlaneNormal = v2;
      break;
      }

    case PlaneOrientationWithZAxesNormal:
      {
      m_PlaneNormal = vn;
      break;
      }

    default:
      {
      std::cerr << "Invalid orientation" << std::endl;
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

 /* Calculate the tool's long axis vector.
  * we make the assumption that the tool's long axis is on the -x axis
  * with the tip in (0,0,0)
  */
  const VersorType& rotation = 
                          m_ToolTransformWRTImageCoordinateSystem.GetRotation();
  
  // auxiliary vecs
  VectorType v1, vn;
  vn.Fill(0.0);
  vn[0] = 1;
  vn = rotation.Transform(vn);

  const VectorType& translation = 
                       m_ToolTransformWRTImageCoordinateSystem.GetTranslation();

  m_ToolPosition[0] = translation[0];
  m_ToolPosition[1] = translation[1];
  m_ToolPosition[2] = translation[2];

  m_PlaneCenter[0] = translation[0];
  m_PlaneCenter[1] = translation[1];
  m_PlaneCenter[2] = translation[2];

  switch( m_OrientationType )
    {
    case OffAxial:
      {
      v1.Fill( 0.0 );
      v1[0] = 1;
    
      if ( fabs(v1*vn) < 1e-9 )
      {
      // FIXME: need to handle this situation too
      igstkLogMacro( DEBUG, "The two vectors are parallel \n");
      }

      vn = itk::CrossProduct( v1, vn );
      vn.Normalize();

      m_PlaneCenter[0] = 0.5*(m_Bounds[0]+m_Bounds[1]);

      m_PlaneNormal = vn;
      break;
      }

    case OffSagittal:
      {
                
      v1.Fill( 0.0 );
      v1[2] = 1;
    
      if ( fabs(v1*vn) < 1e-9 )
        {
        // FIXME: need to handle this situation too
        igstkLogMacro( DEBUG, "The two vectors are parallel \n");
        }

      vn = itk::CrossProduct( v1, vn );
      vn.Normalize();

      m_PlaneNormal = vn;

      m_PlaneCenter[2] = 0.5*(m_Bounds[4]+m_Bounds[5]);
      
      break;
      }

    case OffCoronal:
      {
      v1.Fill( 0.0 );
      v1[1] = 1;
    
      if ( fabs(v1*vn) < 1e-9 )
        {
        // FIXME: need to handle this situation too
        igstkLogMacro( DEBUG, "The two vectors are parallel \n");
        }

      vn = itk::CrossProduct( v1, vn );
      vn.Normalize();

      m_PlaneNormal = vn;

      m_PlaneCenter[1] = 0.5*(m_Bounds[2]+m_Bounds[3]);

      break;
      }

    default:
      {
         std::cerr << "Invalid orientation" << std::endl;
         break;
      }
    }
}

/** Report invalid reslicing mode */
void
ReslicerPlaneSpatialObject
::ReportInvalidReslicingModeProcessing( void )
{
  igstkLogMacro( WARNING, "igstk::ReslicerPlaneSpatialObject::\
                             ReportInvalidReslicingModeProcessing called...\n");
}

/** Report invalid orientation type */
void
ReslicerPlaneSpatialObject
::ReportInvalidOrientationTypeProcessing( void )
{
  igstkLogMacro( WARNING, "igstk::ReslicerPlaneSpatialObject::\
                           ReportInvalidOrientationTypeProcessing called...\n");
}

/** Report invalid request */
void 
ReslicerPlaneSpatialObject
::ReportInvalidRequestProcessing( void )
{
  igstkLogMacro( DEBUG, "igstk::ReslicerPlaneSpatialObject::\
                                   ReportInvalidRequestProcessing called...\n");

  this->InvokeEvent( InvalidRequestErrorEvent() );
}

/** Get tool transform WRT Image Coordinate System */
igstk::Transform
ReslicerPlaneSpatialObject
::GetToolTransform( ) const
{
  return this->m_ToolTransformWRTImageCoordinateSystem;
}

//todo: get tool position as an event
ReslicerPlaneSpatialObject::VectorType
ReslicerPlaneSpatialObject
::GetToolPosition() const
{
  return m_ToolPosition;
}

/** Check if tool spatial object is set to drive the reslicing */
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
  os << indent << "Plane normal" << std::endl;
  os << indent << m_PlaneNormal[0] << " " << m_PlaneNormal[1] << " " 
                                        << m_PlaneNormal[2] << " " << std::endl;
  os << indent << "Plane center" << std::endl;
  os << indent << m_PlaneCenter[0] << " " << m_PlaneCenter[1] << " " 
                                        << m_PlaneCenter[2] << " " << std::endl;
  os << indent << "Tool spatial object set?" << std::endl;
  os << indent << m_ToolSpatialObjectSet << std::endl;
  os << indent << "Tool position" << std::endl;
  os << indent << m_ToolPosition[0] << " " << m_ToolPosition[1] << " " 
                                       << m_ToolPosition[2] << " " << std::endl;
  os << indent << "Cursor position" << std::endl;
  os << indent << m_CursorPosition[0] << " " << m_CursorPosition[1] << " " 
                                     << m_CursorPosition[2] << " " << std::endl;
  os << indent << "Bounding box:" << std::endl;
  os << indent << "x: " << m_Bounds[0] << " " << m_Bounds[1] << std::endl;
  os << indent << "y: " << m_Bounds[2] << " " << m_Bounds[3] << std::endl;
  os << indent << "z: " << m_Bounds[4] << " " << m_Bounds[5] << std::endl;

  os << indent << "Reslicing mode:" << std::endl;

  if( m_ReslicingMode == Orthogonal )
    {
    os << indent << "Orthogonal" << std::endl;
    }
  if( m_ReslicingMode == OffOrthogonal )
    {
    os << indent << "OffOrthogonal" << std::endl;
    }
  if( m_ReslicingMode == Oblique )
    {
    os << indent << "Oblique" << std::endl;
    }

  os << indent << "Orientation type:" << std::endl;

  if( m_OrientationType == Axial )
    {
    os << indent << "Axial" << std::endl;
    }
  if( m_OrientationType == Sagittal )
    {
    os << indent << "Sagittal" << std::endl;
    }
  if( m_OrientationType == Coronal )
    {
    os << indent << "Coronal" << std::endl;
    }

  if( m_OrientationType == OffAxial )
    {
    os << indent << "OffAxial" << std::endl;
    }
  if( m_OrientationType == OffSagittal )
    {
    os << indent << "OffSagittal" << std::endl;
    }
  if( m_OrientationType == OffCoronal )
    {
    os << indent << "OffCoronal" << std::endl;
    }
  
  if( m_OrientationType == PlaneOrientationWithZAxesNormal )
    {
    os << indent << "PlaneOrientationWithZAxesNormal" << std::endl;
    }
  if( m_OrientationType == PlaneOrientationWithXAxesNormal )
    {
    os << indent << "PlaneOrientationWithXAxesNormal" << std::endl;
    }
  if( m_OrientationType == PlaneOrientationWithYAxesNormal )
    {
    os << indent << "PlaneOrientationWithYAxesNormal" << std::endl;
    }
}

} // end namespace igstk


#endif
