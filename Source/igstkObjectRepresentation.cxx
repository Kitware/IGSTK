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
ObjectRepresentation::ObjectRepresentation():m_StateMachine(this),m_Logger(NULL)
{
  m_Color[0] = 1.0;
  m_Color[1] = 1.0;
  m_Color[2] = 1.0;
  m_Opacity = 1.0;
  m_SpatialObject = NULL;
  m_LastMTime = 0;

  igstkAddInputMacro( ValidSpatialObject );
  igstkAddInputMacro( NullSpatialObject  );
  igstkAddInputMacro( ValidUpdatePosition );
  igstkAddInputMacro( ExpiredUpdatePosition );
  igstkAddInputMacro( UpdateRepresentation );
  igstkAddInputMacro( ValidTimeStamp );
  igstkAddInputMacro( InvalidTimeStamp );
  igstkAddInputMacro( SpatialObjectTransform );
  igstkAddInputMacro( RequestUpdatePosition );

  igstkAddStateMacro( NullSpatialObject );
  igstkAddStateMacro( ValidSpatialObject );
  igstkAddStateMacro( ValidTimeStamp );
  igstkAddStateMacro( InvalidTimeStamp );
  igstkAddStateMacro( AttemptingUpdatePosition );

  igstkAddTransitionMacro( NullSpatialObject, NullSpatialObject, NullSpatialObject,  No );
  igstkAddTransitionMacro( NullSpatialObject, ValidSpatialObject, ValidSpatialObject,  SetSpatialObject );
  igstkAddTransitionMacro( NullSpatialObject, ValidUpdatePosition, NullSpatialObject,  ReportInvalidRequest );
  igstkAddTransitionMacro( NullSpatialObject, ExpiredUpdatePosition, NullSpatialObject,  ReportInvalidRequest );
  igstkAddTransitionMacro( NullSpatialObject, UpdateRepresentation, NullSpatialObject,  No );
  igstkAddTransitionMacro( NullSpatialObject, ValidTimeStamp, NullSpatialObject,  ReportInvalidRequest );
  igstkAddTransitionMacro( NullSpatialObject, InvalidTimeStamp, NullSpatialObject,  ReportInvalidRequest );
  igstkAddTransitionMacro( NullSpatialObject, RequestUpdatePosition, NullSpatialObject,  ReportInvalidRequest );
  igstkAddTransitionMacro( NullSpatialObject, SpatialObjectTransform, NullSpatialObject,  ReportInvalidRequest );

  igstkAddTransitionMacro( ValidSpatialObject, ValidUpdatePosition, ValidTimeStamp,  UpdatePosition );
  igstkAddTransitionMacro( ValidSpatialObject, ExpiredUpdatePosition, InvalidTimeStamp,  UpdatePosition );
  igstkAddTransitionMacro( ValidSpatialObject, NullSpatialObject, NullSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidSpatialObject, ValidSpatialObject, ValidSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidSpatialObject, UpdateRepresentation, ValidSpatialObject,  UpdateRepresentation );
  igstkAddTransitionMacro( ValidSpatialObject, ValidTimeStamp, ValidTimeStamp,  MakeObjectsVisible );
  igstkAddTransitionMacro( ValidSpatialObject, InvalidTimeStamp, InvalidTimeStamp,  MakeObjectsInvisible );
  igstkAddTransitionMacro( ValidSpatialObject, RequestUpdatePosition, AttemptingUpdatePosition,  RequestUpdatePosition );
  igstkAddTransitionMacro( ValidSpatialObject, SpatialObjectTransform, ValidSpatialObject,  ReceiveSpatialObjectTransform );

  igstkAddTransitionMacro( ValidTimeStamp, ValidUpdatePosition, ValidTimeStamp,  UpdatePosition );
  igstkAddTransitionMacro( ValidTimeStamp, ExpiredUpdatePosition, InvalidTimeStamp,  UpdatePosition );
  igstkAddTransitionMacro( ValidTimeStamp, NullSpatialObject, NullSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidTimeStamp, ValidSpatialObject, ValidSpatialObject,  No ); 
  igstkAddTransitionMacro( ValidTimeStamp, UpdateRepresentation, ValidTimeStamp,  UpdateRepresentation );
  igstkAddTransitionMacro( ValidTimeStamp, ValidTimeStamp, ValidTimeStamp,  No );
  igstkAddTransitionMacro( ValidTimeStamp, InvalidTimeStamp, InvalidTimeStamp,  MakeObjectsInvisible );
  igstkAddTransitionMacro( ValidTimeStamp, RequestUpdatePosition, AttemptingUpdatePosition,  RequestUpdatePosition );
  igstkAddTransitionMacro( ValidTimeStamp, SpatialObjectTransform, ValidSpatialObject,  ReceiveSpatialObjectTransform );

  igstkAddTransitionMacro( InvalidTimeStamp, ValidUpdatePosition, ValidTimeStamp,  UpdatePosition );
  igstkAddTransitionMacro( InvalidTimeStamp, ExpiredUpdatePosition, InvalidTimeStamp,  UpdatePosition );
  igstkAddTransitionMacro( InvalidTimeStamp, NullSpatialObject, NullSpatialObject,  No ); 
  igstkAddTransitionMacro( InvalidTimeStamp, ValidSpatialObject, ValidSpatialObject,  No ); 
  igstkAddTransitionMacro( InvalidTimeStamp, UpdateRepresentation, ValidSpatialObject,  UpdateRepresentation );
  igstkAddTransitionMacro( InvalidTimeStamp, ValidTimeStamp, ValidTimeStamp,  MakeObjectsVisible );
  igstkAddTransitionMacro( InvalidTimeStamp, InvalidTimeStamp, InvalidTimeStamp,  No );
  igstkAddTransitionMacro( InvalidTimeStamp, RequestUpdatePosition, AttemptingUpdatePosition,  RequestUpdatePosition );
  igstkAddTransitionMacro( InvalidTimeStamp, SpatialObjectTransform, ValidSpatialObject,  ReceiveSpatialObjectTransform );

  igstkAddTransitionMacro( AttemptingUpdatePosition, ValidUpdatePosition, AttemptingUpdatePosition,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingUpdatePosition, ExpiredUpdatePosition, AttemptingUpdatePosition,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingUpdatePosition, NullSpatialObject, NullSpatialObject,  No ); 
  igstkAddTransitionMacro( AttemptingUpdatePosition, ValidSpatialObject, ValidSpatialObject,  No ); 
  igstkAddTransitionMacro( AttemptingUpdatePosition, UpdateRepresentation, ValidSpatialObject,  UpdateRepresentation );
  igstkAddTransitionMacro( AttemptingUpdatePosition, ValidTimeStamp, AttemptingUpdatePosition,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingUpdatePosition, InvalidTimeStamp, AttemptingUpdatePosition,  ReportInvalidRequest );
  igstkAddTransitionMacro( AttemptingUpdatePosition, RequestUpdatePosition, AttemptingUpdatePosition,  RequestUpdatePosition );
  igstkAddTransitionMacro( AttemptingUpdatePosition, SpatialObjectTransform, ValidSpatialObject,  ReceiveSpatialObjectTransform );



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

/** Has the object been modified */
bool ObjectRepresentation::IsModified() const
{
  if( m_LastMTime < this->GetMTime() )
    {
    return true;
    }
  return false;
}


/** Set the Spatial Object */
void ObjectRepresentation::RequestSetSpatialObject( const SpatialObjectType * spatialObject )
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
  this->ObserveTransformModifiedEvent( m_SpatialObject );

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
  this->Modified();
}


/** Request Update the object representation (i.e vtkActors). */
void ObjectRepresentation::RequestUpdateRepresentation( const TimeStamp & time )
{
  m_TimeToRender = time; 
  igstkPushInputMacro( UpdateRepresentation );
  m_StateMachine.ProcessInputs();
}


/** Request Update the object position (i.e vtkActors). */
void ObjectRepresentation::RequestUpdatePosition( const TimeStamp & time )
{
  m_TimeToRender = time; 
  igstkPushInputMacro( RequestUpdatePosition );
  m_StateMachine.ProcessInputs();
}


/** Process the request for updating the transform from the SpatialObject. */
void ObjectRepresentation::RequestUpdatePositionProcessing()
{
  m_SpatialObject->RequestGetTransform();  // The response should be sent back in an event
}


/** Receive the Transform from the SpatialObject via a transduction macro.
 *  Once the transform is received, the validity time is verified. */
void ObjectRepresentation::ReceiveSpatialObjectTransformProcessing()
{
  m_SpatialObjectTransform = m_SpatialObjectTransformToBeSet;

  if( m_TimeToRender.GetExpirationTime() < m_SpatialObjectTransform.GetStartTime() ||
      m_TimeToRender.GetStartTime()      > m_SpatialObjectTransform.GetExpirationTime() )
    {
    igstkPushInputMacro( ExpiredUpdatePosition );
    m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidUpdatePosition );
    m_StateMachine.ProcessInputs();
    }
}

/** Null operation for a State Machine transition */
void ObjectRepresentation::NoProcessing()
{
}

/** Update the object representation (i.e vtkActors). */
void ObjectRepresentation::UpdatePositionProcessing()
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

  // Update the modified time
  m_LastMTime = this->GetMTime();
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
void ObjectRepresentation::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Color: " << m_Color[0] << " : " << m_Color[1] << " : " << m_Color[2] << std::endl;
  os << indent << "Opacity: " << m_Opacity << std::endl;
}

} // end namespace igstk

