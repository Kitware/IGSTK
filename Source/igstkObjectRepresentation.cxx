/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkObjectRepresentation.h" 
#include "vtkMatrix4x4.h"
#include "igstkEvents.h"
#include "vtkActor.h"
#include "vtkProp3D.h"
#include "vtkProperty.h"

namespace igstk 
{ 

/** Constructor */
ObjectRepresentation::ObjectRepresentation():m_StateMachine(this)
{
  m_Color[0] = 1.0;
  m_Color[1] = 1.0;
  m_Color[2] = 1.0;
  m_Opacity = 1.0;
  m_SpatialObject = NULL;

  igstkAddInputMacro( ValidSpatialObject );
  igstkAddInputMacro( NullSpatialObject  );
  igstkAddInputMacro( UpdateActorsPosition );
  igstkAddInputMacro( UpdateRepresentation );
  igstkAddInputMacro( ValidTimeStamp );
  igstkAddInputMacro( InvalidTimeStamp );
  igstkAddInputMacro( SpatialObjectTransform );
  igstkAddInputMacro( RequestUpdatePosition );

  igstkAddStateMacro( NullSpatialObject );
  igstkAddStateMacro( ValidSpatialObjectAndVisible );
  igstkAddStateMacro( ValidSpatialObjectAndInvisible );
  igstkAddStateMacro( ValidTimeStamp );
  igstkAddStateMacro( InvalidTimeStamp );
  igstkAddStateMacro( AttemptingUpdatePositionAndVisible );
  igstkAddStateMacro( AttemptingUpdatePositionAndInvisible );

  igstkAddTransitionMacro( NullSpatialObject, NullSpatialObject, NullSpatialObject,  No );
  igstkAddTransitionMacro( NullSpatialObject, ValidSpatialObject, ValidSpatialObjectAndInvisible,  SetSpatialObject );
  igstkAddTransitionMacro( NullSpatialObject, UpdateActorsPosition, NullSpatialObject,  ReportInvalidRequest );
  igstkAddTransitionMacro( NullSpatialObject, UpdateRepresentation, NullSpatialObject,  No );
  igstkAddTransitionMacro( NullSpatialObject, ValidTimeStamp, NullSpatialObject,  ReportInvalidRequest );
  igstkAddTransitionMacro( NullSpatialObject, InvalidTimeStamp, NullSpatialObject,  ReportInvalidRequest );
  igstkAddTransitionMacro( NullSpatialObject, RequestUpdatePosition, NullSpatialObject,  ReportInvalidRequest );
  igstkAddTransitionMacro( NullSpatialObject, SpatialObjectTransform, NullSpatialObject,  ReportInvalidRequest );

  igstkAddTransitionMacro( ValidSpatialObjectAndVisible, NullSpatialObject, NullSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidSpatialObjectAndVisible, UpdateActorsPosition, ValidTimeStamp,  UpdateActorsPosition );
  igstkAddTransitionMacro( ValidSpatialObjectAndVisible, ValidSpatialObject, ValidSpatialObjectAndInvisible,  No ); 
  igstkAddTransitionMacro( ValidSpatialObjectAndVisible, UpdateRepresentation, ValidSpatialObjectAndVisible,  UpdateRepresentation );
  igstkAddTransitionMacro( ValidSpatialObjectAndVisible, ValidTimeStamp, ValidTimeStamp,  No );
  igstkAddTransitionMacro( ValidSpatialObjectAndVisible, InvalidTimeStamp, InvalidTimeStamp,  MakeObjectsInvisible );
  igstkAddTransitionMacro( ValidSpatialObjectAndVisible, RequestUpdatePosition, AttemptingUpdatePositionAndInvisible,  RequestUpdatePosition );
  igstkAddTransitionMacro( ValidSpatialObjectAndVisible, SpatialObjectTransform, ValidSpatialObjectAndVisible,  ReceiveSpatialObjectTransform );

  igstkAddTransitionMacro( ValidSpatialObjectAndInvisible, NullSpatialObject, NullSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidSpatialObjectAndInvisible, UpdateActorsPosition, ValidTimeStamp,  UpdateActorsPosition );
  igstkAddTransitionMacro( ValidSpatialObjectAndInvisible, ValidSpatialObject, ValidSpatialObjectAndInvisible,  No ); 
  igstkAddTransitionMacro( ValidSpatialObjectAndInvisible, UpdateRepresentation, ValidSpatialObjectAndInvisible,  UpdateRepresentation );
  igstkAddTransitionMacro( ValidSpatialObjectAndInvisible, ValidTimeStamp, ValidTimeStamp,  MakeObjectsVisible );
  igstkAddTransitionMacro( ValidSpatialObjectAndInvisible, InvalidTimeStamp, InvalidTimeStamp,  No );
  igstkAddTransitionMacro( ValidSpatialObjectAndInvisible, RequestUpdatePosition, AttemptingUpdatePositionAndInvisible,  RequestUpdatePosition );
  igstkAddTransitionMacro( ValidSpatialObjectAndInvisible, SpatialObjectTransform, ValidSpatialObjectAndInvisible,  ReceiveSpatialObjectTransform );

  igstkAddTransitionMacro( ValidTimeStamp, NullSpatialObject, NullSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidTimeStamp, UpdateActorsPosition, ValidTimeStamp,  UpdateActorsPosition );
  igstkAddTransitionMacro( ValidTimeStamp, ValidSpatialObject, ValidSpatialObjectAndInvisible,  No ); 
  igstkAddTransitionMacro( ValidTimeStamp, UpdateRepresentation, ValidTimeStamp,  UpdateRepresentation );
  igstkAddTransitionMacro( ValidTimeStamp, ValidTimeStamp, ValidTimeStamp,  No );
  igstkAddTransitionMacro( ValidTimeStamp, InvalidTimeStamp, InvalidTimeStamp,  MakeObjectsInvisible );
  igstkAddTransitionMacro( ValidTimeStamp, RequestUpdatePosition, AttemptingUpdatePositionAndVisible,  RequestUpdatePosition );
  igstkAddTransitionMacro( ValidTimeStamp, SpatialObjectTransform, InvalidTimeStamp,  ReceiveSpatialObjectTransform );

  igstkAddTransitionMacro( InvalidTimeStamp, NullSpatialObject, NullSpatialObject,  No ); 
  igstkAddTransitionMacro( InvalidTimeStamp, UpdateActorsPosition, InvalidTimeStamp,  UpdateActorsPosition );
  igstkAddTransitionMacro( InvalidTimeStamp, ValidSpatialObject, ValidSpatialObjectAndInvisible,  No ); 
  igstkAddTransitionMacro( InvalidTimeStamp, UpdateRepresentation, ValidSpatialObjectAndInvisible,  UpdateRepresentation );
  igstkAddTransitionMacro( InvalidTimeStamp, ValidTimeStamp, ValidTimeStamp,  MakeObjectsVisible );
  igstkAddTransitionMacro( InvalidTimeStamp, InvalidTimeStamp, InvalidTimeStamp,  No );
  igstkAddTransitionMacro( InvalidTimeStamp, RequestUpdatePosition, AttemptingUpdatePositionAndInvisible,  RequestUpdatePosition );
  igstkAddTransitionMacro( InvalidTimeStamp, SpatialObjectTransform, InvalidTimeStamp,  ReceiveSpatialObjectTransform );

  igstkAddTransitionMacro( AttemptingUpdatePositionAndInvisible, NullSpatialObject, NullSpatialObject,  No ); 
  igstkAddTransitionMacro( AttemptingUpdatePositionAndInvisible, UpdateActorsPosition, AttemptingUpdatePositionAndInvisible,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingUpdatePositionAndInvisible, ValidSpatialObject, ValidSpatialObjectAndInvisible,  No ); 
  igstkAddTransitionMacro( AttemptingUpdatePositionAndInvisible, UpdateRepresentation, ValidSpatialObjectAndInvisible,  UpdateRepresentation );
  igstkAddTransitionMacro( AttemptingUpdatePositionAndInvisible, ValidTimeStamp, AttemptingUpdatePositionAndInvisible,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingUpdatePositionAndInvisible, InvalidTimeStamp, AttemptingUpdatePositionAndInvisible,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingUpdatePositionAndInvisible, RequestUpdatePosition, AttemptingUpdatePositionAndInvisible,  RequestUpdatePosition );
  igstkAddTransitionMacro( AttemptingUpdatePositionAndInvisible, SpatialObjectTransform, InvalidTimeStamp,  ReceiveSpatialObjectTransform );

  igstkAddTransitionMacro( AttemptingUpdatePositionAndVisible, NullSpatialObject, NullSpatialObject,  No ); 
  igstkAddTransitionMacro( AttemptingUpdatePositionAndVisible, UpdateActorsPosition, AttemptingUpdatePositionAndVisible,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingUpdatePositionAndVisible, ValidSpatialObject, ValidSpatialObjectAndInvisible,  No ); 
  igstkAddTransitionMacro( AttemptingUpdatePositionAndVisible, UpdateRepresentation, AttemptingUpdatePositionAndVisible,  No );
  igstkAddTransitionMacro( AttemptingUpdatePositionAndVisible, ValidTimeStamp, AttemptingUpdatePositionAndVisible,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingUpdatePositionAndVisible, InvalidTimeStamp, AttemptingUpdatePositionAndVisible,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingUpdatePositionAndVisible, RequestUpdatePosition, AttemptingUpdatePositionAndVisible,  RequestUpdatePosition );
  igstkAddTransitionMacro( AttemptingUpdatePositionAndVisible, SpatialObjectTransform, ValidSpatialObjectAndVisible,  ReceiveSpatialObjectTransform );

  igstkSetInitialStateMacro( NullSpatialObject );

  m_StateMachine.SetReadyToRun();

} 

/** Destructor */
ObjectRepresentation::~ObjectRepresentation()  
{ 
  // This must be invoked in order to prevent Memory Leaks.
  this->DeleteActors();
}

/** Add an actor to the actors list */
void ObjectRepresentation::AddActor( vtkProp * actor )
{
  m_Actors.push_back( actor );
}

/** Empty the list of actors */
void ObjectRepresentation::DeleteActors()
{
  ActorsListType::iterator it = m_Actors.begin();
  while(it != m_Actors.end())
    {
    (*it)->Delete();
    it++;
    }

  // Reset the list of actors
  m_Actors.clear();
}


/** Set the Spatial Object */
void ObjectRepresentation
::RequestSetSpatialObject( const SpatialObjectType * spatialObject )
{
  // This const_cast is done because the ObjectRepresentation class invoke
  // Request methods in the SpatialObject, and those methods modify the state
  // of its internal StateMachine. It is however desirable to keep the outside
  // API of this class refering to a const object.
  m_SpatialObjectToAdd = const_cast< SpatialObjectType *>( spatialObject );
  if( !m_SpatialObjectToAdd )
    {
    igstkPushInputMacro( NullSpatialObject );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidSpatialObject );
    m_StateMachine.ProcessInputs();
    }

}


/** Set the Spatial Object */
void ObjectRepresentation::SetSpatialObjectProcessing()
{
  m_SpatialObject = m_SpatialObjectToAdd;
  this->ObserveSpatialObjectTransformInput( m_SpatialObject );
}

/** Set the color */
void ObjectRepresentation::SetColor(float r, float g, float b)
{
  if(m_Color[0] == r && m_Color[1] == g && m_Color[2] == b)
    {
    return;
    }
  m_Color[0] = r;
  m_Color[1] = g;
  m_Color[2] = b;

  // Update all the actors
  ActorsListType::iterator it = m_Actors.begin();
  while(it != m_Actors.end())
    {
    vtkActor * va = static_cast<vtkActor*>(*it);
    va->GetProperty()->SetColor(m_Color[0], m_Color[1], m_Color[2]); 
    it++;
    }
}

/** Set the opacity */
void ObjectRepresentation::SetOpacity(float alpha)
{
  if(m_Opacity == alpha)
    {
    return;
    }
  m_Opacity = alpha;

  // Update all the actors
  ActorsListType::iterator it = m_Actors.begin();
  while(it != m_Actors.end())
    {
    vtkActor * va = static_cast<vtkActor*>(*it);
    va->GetProperty()->SetOpacity(m_Opacity); 
    it++;
    }
}

/** Request Update the object representation (i.e vtkActors). */
void ObjectRepresentation::RequestUpdateRepresentation( const TimeStamp & time )
{
  igstkLogMacro( DEBUG, "RequestUpdateRepresentation at time" 
                          << time );
  m_TimeToRender = time; 
  igstkPushInputMacro( UpdateRepresentation );
  m_StateMachine.ProcessInputs();
}


/** Request Update the object position (i.e vtkActors). */
void ObjectRepresentation::RequestUpdatePosition( const TimeStamp & time )
{
  igstkLogMacro( DEBUG, "RequestUpdatePosition at time" 
                          << time );
  m_TimeToRender = time; 
  igstkPushInputMacro( RequestUpdatePosition );
  m_StateMachine.ProcessInputs();
}


/** Process the request for updating the transform from the SpatialObject. */
void ObjectRepresentation::RequestUpdatePositionProcessing()
{
  igstkLogMacro( DEBUG, "RequestUpdatePositionProcessing called ...."); 
  // The response should be sent back in an event
  m_SpatialObject->RequestGetTransform();  
}


/** Receive the Transform from the SpatialObject via a transduction macro. */
void ObjectRepresentation::ReceiveSpatialObjectTransformProcessing()
{
  m_SpatialObjectTransform = m_SpatialObjectTransformInputToBeSet;

  igstkPushInputMacro( UpdateActorsPosition );
  m_StateMachine.ProcessInputs();
}

/** Null operation for a State Machine transition */
void ObjectRepresentation::NoProcessing()
{
}


/** Update the object representation (i.e vtkActors). */
void ObjectRepresentation::UpdateActorsPositionProcessing()
{
  vtkMatrix4x4* vtkMatrix = vtkMatrix4x4::New();

  m_SpatialObjectTransform.ExportTransform( *vtkMatrix );

  // Update all the actors
  ActorsListType::iterator it = m_Actors.begin();
  while( it != m_Actors.end() )
    {  
    vtkProp3D::SafeDownCast(*it)->SetUserMatrix(vtkMatrix);
    it++;
    }

  vtkMatrix->Delete();
}


/** Update the object representation (i.e vtkActors). 
 *  It checks the transform expiration time. */
void ObjectRepresentation::RequestVerifyTimeStamp()
{

  if( m_TimeToRender.GetExpirationTime() < m_SpatialObjectTransform.GetStartTime() ||
      m_TimeToRender.GetStartTime()      > m_SpatialObjectTransform.GetExpirationTime() )
    {
    igstkPushInputMacro( InvalidTimeStamp );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidTimeStamp );
    m_StateMachine.ProcessInputs();
    }
}


/** Make Objects Invisible. This method is called when the Transform time stamp
 * has expired with respect to the requested rendering time. */
void ObjectRepresentation::MakeObjectsInvisibleProcessing()
{
  igstkLogMacro( WARNING, "MakeObjectsInvisibleProcessing at " 
                          << m_TimeToRender );
  
  ActorsListType::iterator it = m_Actors.begin();
  while(it != m_Actors.end())
    {
    (*it)->VisibilityOff();
    it++;
    }
}


/** Make Objects Visible. This method is called when the Transform time stamp
 * is valid with respect to the requested rendering time. */
void ObjectRepresentation::MakeObjectsVisibleProcessing()
{
  igstkLogMacro( WARNING, "MakeObjectsVisibleProcessing at "
                          << m_TimeToRender );

  ActorsListType::iterator it = m_Actors.begin();
  while(it != m_Actors.end())
    {
    (*it)->VisibilityOn();
    it++;
    }
}


/** Report an invalid request made to the State Machine */
void ObjectRepresentation::ReportInvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "Invalid request in ObjectRepresentation");
}


/** Print Self function */
void ObjectRepresentation
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Color: " << m_Color[0] << " : ";
  os << m_Color[1] << " : " << m_Color[2] << std::endl;
  os << indent << "Opacity: " << m_Opacity << std::endl;
}

} // end namespace igstk
