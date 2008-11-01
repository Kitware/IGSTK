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
  igstkAddStateMacro( ReferenceSpatialObjectSet );
  igstkAddStateMacro( AttemptingToSetReferenceSpatialObject );  
  igstkAddStateMacro( AttemptingToSetCursorPosition );
  igstkAddStateMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem );
  
  // List of state machine inputs
  igstkAddInputMacro( SetReferenceSpatialObject );
  igstkAddInputMacro( ValidReferenceSpatialObject );
  igstkAddInputMacro( InValidReferenceSpatialObject );
  igstkAddInputMacro( ValidToolSpatialObject );
  igstkAddInputMacro( InValidToolSpatialObject );
  igstkAddInputMacro( SetCursorPosition );
  igstkAddInputMacro( ValidCursorPosition );
  igstkAddInputMacro( InValidCursorPosition );
  igstkAddInputMacro( GetToolTransformWRTImageCoordinateSystem );
  igstkAddInputMacro( ToolTransformWRTImageCoordinateSystem );
  igstkAddInputMacro( GetCrossHairPosition );

  // From Initial

  igstkAddTransitionMacro( Initial, SetReferenceSpatialObject, 
                           AttemptingToSetReferenceSpatialObject, AttemptSetReferenceSpatialObject);

  // From AttemptingToSetReferenceSpatialObject

  igstkAddTransitionMacro( AttemptingToSetReferenceSpatialObject, ValidReferenceSpatialObject,
                           ReferenceSpatialObjectSet,  SetReferenceSpatialObject ); 

  igstkAddTransitionMacro( AttemptingToSetReferenceSpatialObject, InValidReferenceSpatialObject,
                           Initial,  ReportInvalidReferenceSpatialObject );

  igstkAddTransitionMacro( AttemptingToSetReferenceSpatialObject, SetCursorPosition, 
                           AttemptingToSetCursorPosition, AttemptSetCursorPosition);

  // From ValidReferenceSpatialObjectSet

  igstkAddTransitionMacro( ReferenceSpatialObjectSet, SetCursorPosition,
                           AttemptingToSetCursorPosition, AttemptSetCursorPosition );

  igstkAddTransitionMacro( ReferenceSpatialObjectSet, ValidToolSpatialObject,
                           ReferenceSpatialObjectSet, SetToolSpatialObject );

  igstkAddTransitionMacro( ReferenceSpatialObjectSet, InValidToolSpatialObject,
                           ReferenceSpatialObjectSet, ReportInvalidToolSpatialObject );

  igstkAddTransitionMacro( ReferenceSpatialObjectSet, InValidToolSpatialObject,
                           ReferenceSpatialObjectSet, ReportInvalidToolSpatialObject );

  igstkAddTransitionMacro( ReferenceSpatialObjectSet, GetCrossHairPosition,
                           ReferenceSpatialObjectSet, GetCrossHairPosition );

  igstkAddTransitionMacro( ReferenceSpatialObjectSet, GetToolTransformWRTImageCoordinateSystem,
                           AttemptingToGetToolTransformWRTImageCoordinateSystem,
                                         RequestGetToolTransformWRTImageCoordinateSystem );

  igstkAddTransitionMacro( ReferenceSpatialObjectSet, ToolTransformWRTImageCoordinateSystem,
                           ReferenceSpatialObjectSet, ReportInvalidRequest );  

  // From AttemptingToSetCursorPosition

  igstkAddTransitionMacro( AttemptingToSetCursorPosition, ValidCursorPosition,
                           ReferenceSpatialObjectSet,  SetCursorPosition ); 

  igstkAddTransitionMacro( AttemptingToSetCursorPosition, InValidCursorPosition,
                           ReferenceSpatialObjectSet,  ReportInvalidCursorPosition );

  // From AttemptingToGetToolTransformWRTImageCoordinateSystem

  igstkAddTransitionMacro( AttemptingToGetToolTransformWRTImageCoordinateSystem,
                                            ToolTransformWRTImageCoordinateSystem,
                           ReferenceSpatialObjectSet,
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
::RequestSetCursorPosition( PointType point )
{  
  igstkLogMacro( DEBUG,"igstk::CrossHairObject\
                       ::RequestSetCursorPosition called...\n");

  m_CursorPositionToBeSet[0] = point[0];
  m_CursorPositionToBeSet[1] = point[1];
  m_CursorPositionToBeSet[2] = point[2];

  m_StateMachine.PushInput( m_SetCursorPositionInput );

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
::ReportInvalidReferenceSpatialObjectProcessing( )
{
  igstkLogMacro( DEBUG,"igstk::CrossHairObject\
                       ::ReportInvalidReferenceSpatialObjectProcessing called...\n");
}

void 
CrossHairObject
::ReportInvalidCursorPositionProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::CrossHairObject\
                       ::ReportInvalidCursorPositionProcessing called...\n");
}

void 
CrossHairObject
::AttemptSetCursorPositionProcessing()
{

  igstkLogMacro( DEBUG, "igstk::CrossHairObject\
                        ::AttemptSetCursorPositionProcessing called...\n");

    bool validPosition = false; 

    const BoundingBoxType::BoundsArrayType &bounds = m_BoundingBox->GetBounds();


    if( m_CursorPositionToBeSet[0] < bounds[0] || 
        m_CursorPositionToBeSet[1] > bounds[1] ||
        m_CursorPositionToBeSet[2] < bounds[2] || 
        m_CursorPositionToBeSet[3] > bounds[3] ||
        m_CursorPositionToBeSet[4] < bounds[4] || 
        m_CursorPositionToBeSet[5] > bounds[5] )
    {
        validPosition = true;
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
CrossHairObject
::SetCursorPositionProcessing()
{
  igstkLogMacro( DEBUG, "igstk::CrossHairObject\
                        ::SetCursorPositionProcessing called...\n");

  m_CursorPosition[0] = m_CursorPositionToBeSet[0];
  m_CursorPosition[1] = m_CursorPositionToBeSet[1];
  m_CursorPosition[2] = m_CursorPositionToBeSet[2];

  m_Position[0] = m_CursorPosition[0];
  m_Position[1] = m_CursorPosition[1];
  m_Position[2] = m_CursorPosition[2];

  //turn on the flag
  m_CursorPositionSetFlag = true;
}

//void
//CrossHairObject
//::RequestSetBoundingBox( const BoundingBoxType* boundingBox )
//{
//  igstkLogMacro( DEBUG,"igstk::CrossHairObject\
//                       ::RequestSetBoundingBox called...\n");
//
//  m_BoundingBoxToBeSet = const_cast< BoundingBoxType *>(boundingBox);
//
//  m_StateMachine.PushInput( m_SetBoundingBoxInput );
//
//  m_StateMachine.ProcessInputs();
//}

void
CrossHairObject
::RequestSetReferenceSpatialObject( const SpatialObjectType* spatialObject )
{
  igstkLogMacro( DEBUG,"igstk::CrossHairObject\
                       ::RequestSetReferenceSpatialObject called...\n");

  m_ReferenceSpatialObjectToBeSet = const_cast< SpatialObjectType* >(spatialObject);

  m_StateMachine.PushInput( m_SetReferenceSpatialObjectInput );

  m_StateMachine.ProcessInputs();
}


void
CrossHairObject
::AttemptSetReferenceSpatialObjectProcessing( )
{
  igstkLogMacro( DEBUG,"igstk::CrossHairObject\
                       ::AttemptSetReferenceSpatialObject called...\n");

  if( !m_ReferenceSpatialObjectToBeSet )
    {
    m_StateMachine.PushInput( m_InValidReferenceSpatialObjectInput );
    }
  else
    {
    m_StateMachine.PushInput( m_ValidReferenceSpatialObjectInput );
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
    return 0;
    }

  const BoundingBoxType::BoundsArrayType &bounds = m_BoundingBox->GetBounds();

  return bounds[index];
}

void 
CrossHairObject
::RequestSetToolSpatialObject( const SpatialObjectType * spatialObject )
{  
  igstkLogMacro( DEBUG,"igstk::CrossHairObject\
                       ::RequestSetToolSpatialObject called...\n");

  m_ToolSpatialObjectToBeSet = const_cast< SpatialObjectType *>(spatialObject);

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
::SetReferenceSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::CrossHairObject\
                       ::SetReferenceSpatialObjectProcessing called...\n");

  m_ReferenceSpatialObject = m_ReferenceSpatialObjectToBeSet;

  // get the bounding box from the reference spatial object
  BoundingBoxObserver::Pointer  boundingBoxObserver = BoundingBoxObserver::New();
  boundingBoxObserver->Reset();
  unsigned long boundingBoxObserverID = 
  m_ReferenceSpatialObject->AddObserver( SpatialObjectType::BoundingBoxEvent(), boundingBoxObserver );
  m_ReferenceSpatialObject->RequestGetBounds();

  if( boundingBoxObserver->GotBoundingBox() ) 
  {
    //SpatialObjectType::BoundingBoxType::ConstPointer bbox = boundingBoxObserver->GetBoundingBox();
    m_BoundingBox = boundingBoxObserver->GetBoundingBox();
  }
}

/** Get tool transform WRT Image Coordinate System*/
igstk::Transform
CrossHairObject
::GetToolTransform( ) const
{
  return this->m_ToolTransformWRTImageCoordinateSystem;
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
    CoordinateSystemHelperType::GetCoordinateSystem( m_ReferenceSpatialObject );

  CoordinateSystem * ImageSpatialObjectCoordinateSystemNC =
          const_cast< CoordinateSystem *>(ImageSpatialObjectCoordinateSystem);

  if ( m_ToolSpatialObjectSet )
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
