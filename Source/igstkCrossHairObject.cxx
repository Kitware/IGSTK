/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCrossHairObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkCrossHairObject.h"

namespace igstk
{ 

/** Constructor */
CrossHairObject::CrossHairObject():m_StateMachine(this)
{
  m_CrossHairSpatialObject = CrossHairSpatialObjectType::New();
  this->RequestSetInternalSpatialObject( m_CrossHairSpatialObject );  

  m_ToolSpatialObject = NULL; 

  //tool spatial object check flag
  m_ToolSpatialObjectSet  = false;

  // List of states
  igstkAddStateMacro( Initial );
  igstkAddStateMacro( ToolSpatialObjectSet );
  igstkAddStateMacro( ValidBoundingBoxSet );
  igstkAddStateMacro( AttemptingToSetBoundingBox );  
  igstkAddStateMacro( AttemptingToSetMousePosition );
  igstkAddStateMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem );
  
  // List of state machine inputs

  igstkAddInputMacro( SetBoundingBox );
  igstkAddInputMacro( ValidBoundingBox );
  igstkAddInputMacro( InValidBoundingBox );
  igstkAddInputMacro( ValidToolSpatialObject );
  igstkAddInputMacro( InValidToolSpatialObject );
  igstkAddInputMacro( SetMousePosition );
  igstkAddInputMacro( ValidMousePosition );
  igstkAddInputMacro( InValidMousePosition );
  igstkAddInputMacro( GetToolTransformWRTImageCoordinateSystem );
  igstkAddInputMacro( ToolTransformWRTImageCoordinateSystem );
  igstkAddInputMacro( GetCrossHairPosition );

  // From Initial

  igstkAddTransitionMacro( Initial, SetBoundingBox, 
                           AttemptingToSetBoundingBox, SetBoundingBox);

  // From AttemptingToSetBoundingbox

  igstkAddTransitionMacro( AttemptingToSetBoundingBox, ValidBoundingBox,
                           ValidBoundingBoxSet,  SetBoundingBox ); 

  igstkAddTransitionMacro( AttemptingToSetBoundingBox, InValidBoundingBox,
                           Initial,  ReportInvalidBoundingBox );

  igstkAddTransitionMacro( AttemptingToSetBoundingBox, SetMousePosition, 
                           AttemptingToSetMousePosition, AttemptSetMousePosition);

  // From AttemptingToSetMousePosition

  igstkAddTransitionMacro( AttemptingToSetMousePosition, ValidMousePosition,
                           ValidBoundingBoxSet,  SetMousePosition ); 

  igstkAddTransitionMacro( AttemptingToSetMousePosition, InValidMousePosition,
                           ValidBoundingBoxSet,  ReportInvalidMousePosition );

  // From ValidBoundingBoxSet

  igstkAddTransitionMacro( ValidBoundingBoxSet, SetMousePosition,
                           AttemptingToSetMousePosition, AttemptSetMousePosition );

  igstkAddTransitionMacro( ValidBoundingBoxSet, ValidToolSpatialObject,
                           ToolSpatialObjectSet, SetToolSpatialObject );

  igstkAddTransitionMacro( ValidBoundingBoxSet, InValidToolSpatialObject,
                           ValidBoundingBoxSet, ReportInvalidToolSpatialObject );

  igstkAddTransitionMacro( ValidBoundingBoxSet, InValidToolSpatialObject,
                           ValidBoundingBoxSet, ReportInvalidToolSpatialObject );

  igstkAddTransitionMacro( ValidBoundingBoxSet, GetCrossHairPosition,
                           ValidBoundingBoxSet, GetCrossHairPosition );

  // From ToolSpatialObjectSet

  igstkAddTransitionMacro( ToolSpatialObjectSet, GetToolTransformWRTImageCoordinateSystem,
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                                         RequestGetToolTransformWRTImageCoordinateSystem );

  igstkAddTransitionMacro( ToolSpatialObjectSet, ToolTransformWRTImageCoordinateSystem,
                           ToolSpatialObjectSet, ReportInvalidRequest );  

  igstkAddTransitionMacro( ToolSpatialObjectSet, GetCrossHairPosition,
                           ToolSpatialObjectSet, GetCrossHairPosition );

  // From AttemptingToGetToolTransformWRTImageCoordinateSystem

  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                                            ToolTransformWRTImageCoordinateSystem,
                           ToolSpatialObjectSet,
                                            ReceiveToolTransformWRTImageCoordinateSystem );

  igstkSetInitialStateMacro( Initial );
  this->m_StateMachine.SetReadyToRun();
} 

/** Destructor */
CrossHairObject::~CrossHairObject()  
{
}

/** Report invalid request */
void 
CrossHairObject
::ReportInvalidRequestProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::CrossHairObject::ReportInvalidRequestProcessing called...\n");

  this->InvokeEvent( InvalidRequestErrorEvent() );
}

/** Request compute reslicing plane */
void
CrossHairObject
::RequestGetCrossHairPosition( )
{
  igstkLogMacro( DEBUG,"igstk::CrossHairObject\
                       ::RequestGetCrossHairPosition called...\n");

  igstkPushInputMacro( GetCrossHairPosition );
  m_StateMachine.ProcessInputs();

}

void 
CrossHairObject
::RequestSetMousePosition( PointType point )
{  
  igstkLogMacro( DEBUG,"igstk::CrossHairObject\
                       ::RequestSetMousePosition called...\n");

  m_MousePositionToBeSet[0] = point[0];
  m_MousePositionToBeSet[1] = point[1];
  m_MousePositionToBeSet[2] = point[2];

  m_StateMachine.PushInput( m_SetMousePositionInput );

  m_StateMachine.ProcessInputs();
}

/** Receive the tool spatial object transform with respect to the supplied reference
 * coordinate system using a transduction macro */
void
CrossHairObject
::ReceiveToolTransformWRTImageCoordinateSystemProcessing()
{
  igstkLogMacro( DEBUG,"igstk::CrossHairObject\
                       ::ReceiveToolTransformWRTImageCoordinateSystemProcessing called...\n");

//  igstkLogMacro( DEBUG, "ReceiveToolTransformWRTImageCoordinateSystemProcessing " 
//                 << this->m_ToolTransformWRTImageCoordinateSystem );

  this->m_ToolTransformWRTImageCoordinateSystem =
    this->m_ToolTransformWRTImageCoordinateSystemInputToBeSet.GetTransform();
}

void 
CrossHairObject
::ReportInvalidToolSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::CrossHairObject\
                       ::ReportInvalidToolSpatialObjectProcessing called...\n");
}

void 
CrossHairObject
::ReportInvalidBoundingBoxProcessing( )
{
  igstkLogMacro( DEBUG,"igstk::CrossHairObject\
                       ::ReportInvalidBoundingBoxProcessing called...\n");
}

void 
CrossHairObject
::ReportInvalidMousePositionProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::CrossHairObject\
                       ::ReportInvalidMousePositionProcessing called...\n");
}

void 
CrossHairObject
::AttemptSetMousePositionProcessing()
{

  igstkLogMacro( DEBUG, "igstk::CrossHairObject\
                        ::AttemptSetMousePositionProcessing called...\n");

    bool validPosition = false; 

    const BoundingBoxType::BoundsArrayType &bounds = m_BoundingBox->GetBounds();


    if( m_MousePositionToBeSet[0] < bounds[0] || 
        m_MousePositionToBeSet[1] > bounds[1] ||
        m_MousePositionToBeSet[2] < bounds[2] || 
        m_MousePositionToBeSet[3] > bounds[3] ||
        m_MousePositionToBeSet[4] < bounds[4] || 
        m_MousePositionToBeSet[5] > bounds[5] )
    {
        validPosition = true;
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

void 
CrossHairObject
::SetMousePositionProcessing()
{
  igstkLogMacro( DEBUG, "igstk::CrossHairObject\
                        ::SetMousePositionProcessing called...\n");

  m_MousePosition[0] = m_MousePositionToBeSet[0];
  m_MousePosition[1] = m_MousePositionToBeSet[1];
  m_MousePosition[2] = m_MousePositionToBeSet[2];

  m_Position[0] = m_MousePosition[0];
  m_Position[1] = m_MousePosition[1];
  m_Position[2] = m_MousePosition[2];

  //turn on the flag
  m_MousePositionSetFlag = true;
}

void
CrossHairObject
::RequestSetBoundingBox( const BoundingBoxType* boundingBox )
{
  igstkLogMacro( DEBUG,"igstk::CrossHairObject\
                       ::RequestSetBoundingBox called...\n");

  m_BoundingBoxToBeSet = const_cast< BoundingBoxType *>(boundingBox);

  m_StateMachine.PushInput( m_SetBoundingBoxInput );

  m_StateMachine.ProcessInputs();
}

void
CrossHairObject
::AttemptSetBoundingBoxProcessing()
{

 igstkLogMacro( DEBUG, "igstk::CrossHairObject\
                        ::AttemptSetBoundingBoxProcessing called...\n");

 if( !m_BoundingBoxToBeSet )
  {
  m_StateMachine.PushInput( m_InValidBoundingBoxInput );
  }
 else
  {
  m_StateMachine.PushInput( m_ValidBoundingBoxInput );
  }

 m_StateMachine.ProcessInputs();
}


 /** Return a given dimension */
double
CrossHairObject
::GetBoundingBoxDimensionByIndex(unsigned int index)
{

  if(index >= 6)
    {
    return NULL;
    }

  const BoundingBoxType::BoundsArrayType &bounds = m_BoundingBox->GetBounds();

  return bounds[index];
}

void 
CrossHairObject
::RequestSetToolSpatialObject( const ToolSpatialObjectType * toolSpatialObject )
{  
  igstkLogMacro( DEBUG,"igstk::CrossHairObject\
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
CrossHairObject
::SetToolSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageReslicePlaneSpatialObject\
                       ::SetToolSpatialObjectProcessing called...\n");

  m_ToolSpatialObject = m_ToolSpatialObjectToBeSet;
  this->ObserveToolTransformWRTImageCoordinateSystemInput( this->m_ToolSpatialObject );
  m_ToolSpatialObjectSet = true;
}


void 
CrossHairObject
::SetBoundingBoxProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::CrossHairObject\
                       ::SetBoundingBoxProcessing called...\n");

  m_BoundingBox = m_BoundingBoxToBeSet;
}

void
CrossHairObject
::RequestUpdateToolTransformWRTImageCoordinateSystem()
{
  igstkLogMacro( DEBUG,
     "igstk::CrossHairObject::RequestUpdateToolTransformWRTImageCoordinateSystem called ...\n");
  
  igstkPushInputMacro( GetToolTransformWRTImageCoordinateSystem );
  this->m_StateMachine.ProcessInputs();
}

/** Request to receive the tool transform WRT image coordinate system */
void
CrossHairObject
::RequestGetToolTransformWRTImageCoordinateSystemProcessing()
{
  igstkLogMacro( DEBUG,
     "igstk::CrossHairObject::RequestGetToolTransformWRTImageCoordinateSystemProcessing called ...\n");

  typedef igstk::Friends::CoordinateSystemHelper 
                          CoordinateSystemHelperType;

  const CoordinateSystem* ImageSpatialObjectCoordinateSystem = 
    CoordinateSystemHelperType::GetCoordinateSystem( this );

  CoordinateSystem * ImageSpatialObjectCoordinateSystemNC =
          const_cast< CoordinateSystem *>(ImageSpatialObjectCoordinateSystem);

  m_ToolSpatialObject->RequestComputeTransformTo( ImageSpatialObjectCoordinateSystemNC ); 
}

/** Check if tool spatial object is set to drive the reslicing*/
bool
CrossHairObject
::IsToolSpatialObjectSet()
{
  return this->m_ToolSpatialObjectSet;
}

/** Compute reslicing plane */
void
CrossHairObject
::GetCrossHairPositionProcessing()
{
  //Update the tool transform if tool spatial object provided
  if ( m_ToolSpatialObject ) 
    {
    this->RequestUpdateToolTransformWRTImageCoordinateSystem();
    const VectorType &vec = m_ToolTransformWRTImageCoordinateSystem.GetTranslation();
    m_Position[0] = vec[0];
    m_Position[1] = vec[1];
    m_Position[2] = vec[2];
    }

  PointEvent positionEvent;
  positionEvent.Set( m_Position );
  this->InvokeEvent( positionEvent );
}


/** Print object information */
void 
CrossHairObject
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
//  os << "Size = " << m_Size << std::endl;
}


} // end namespace igstk
