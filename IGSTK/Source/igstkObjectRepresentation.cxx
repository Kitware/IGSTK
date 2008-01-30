/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkObjectRepresentation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

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
ObjectRepresentation::ObjectRepresentation():m_StateMachine(this),m_VisibilityStateMachine(this)
{
  m_Color[0] = 1.0;
  m_Color[1] = 1.0;
  m_Color[2] = 1.0;
  m_Opacity = 1.0;
  m_SpatialObject = NULL;

  igstkAddInputMacro( ValidSpatialObject );
  igstkAddInputMacro( NullSpatialObject  );
  igstkAddInputMacro( UpdateRepresentation );
  igstkAddInputMacro( SpatialObjectTransform );
  igstkAddInputMacro( TransformNotAvailable );

  igstkAddStateMacro( NullSpatialObject  );
  igstkAddStateMacro( ValidSpatialObject );
  igstkAddStateMacro( AttemptingGetTransform );

  igstkAddTransitionMacro( NullSpatialObject, 
                           NullSpatialObject,
                           NullSpatialObject,
                           No );

  igstkAddTransitionMacro( NullSpatialObject,
                           ValidSpatialObject,
                           ValidSpatialObject,
                           SetSpatialObject );

  igstkAddTransitionMacro( NullSpatialObject,
                           UpdateRepresentation,
                           NullSpatialObject,
                           No );

  igstkAddTransitionMacro( NullSpatialObject,
                           SpatialObjectTransform,
                           NullSpatialObject,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( NullSpatialObject,
                           TransformNotAvailable,
                           NullSpatialObject,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ValidSpatialObject,
                           NullSpatialObject,
                           NullSpatialObject,
                           No );

  igstkAddTransitionMacro( ValidSpatialObject,
                           ValidSpatialObject,
                           ValidSpatialObject,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ValidSpatialObject,
                           UpdateRepresentation,
                           AttemptingGetTransform, 
                           RequestGetTransform );

  igstkAddTransitionMacro( ValidSpatialObject,
                           SpatialObjectTransform,
                           ValidSpatialObject,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( ValidSpatialObject,
                           TransformNotAvailable,
                           ValidSpatialObject,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingGetTransform,
                           NullSpatialObject,
                           AttemptingGetTransform,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingGetTransform,
                           ValidSpatialObject,
                           AttemptingGetTransform,
                           ReportInvalidRequest );

  igstkAddTransitionMacro( AttemptingGetTransform,
                           UpdateRepresentation,
                           AttemptingGetTransform,
                           No );

  igstkAddTransitionMacro( AttemptingGetTransform,
                           SpatialObjectTransform,
                           ValidSpatialObject,
                           ReceiveSpatialObjectTransform );

  igstkAddTransitionMacro( AttemptingGetTransform,
                           TransformNotAvailable,
                           ValidSpatialObject,
                           ReceiveSpatialObjectTransform );

  igstkSetInitialStateMacro( NullSpatialObject );

  m_StateMachine.SetReadyToRun();


  // Configure the auxiliary State Machine that controls the Visibility of the
  // objects.
  m_VisibilityStateMachine.AddInput( this->m_ValidTimeStampInput, "ValidTimeStampInput" );
  m_VisibilityStateMachine.AddInput( this->m_InvalidTimeStampInput, "InvalidTimeStampInput" );
  m_VisibilityStateMachine.AddInput( this->m_SetActorVisibilityInput, "SetActorVisibilityInput" );

  m_VisibilityStateMachine.AddState( this->m_InvisibleState, "InvisibleState" );
  m_VisibilityStateMachine.AddState( this->m_VisibleState, "VisibleState" );

  m_VisibilityStateMachine.AddTransition( this->m_InvisibleState,
                                          this->m_InvalidTimeStampInput,
                                          this->m_InvisibleState,
                                        & Self::NoProcessing );

  m_VisibilityStateMachine.AddTransition( this->m_InvisibleState,
                                          this->m_ValidTimeStampInput,
                                          this->m_VisibleState,
                                        & Self::MakeObjectsVisibleProcessing );

  m_VisibilityStateMachine.AddTransition( this->m_VisibleState,
                                          this->m_InvalidTimeStampInput,
                                          this->m_InvisibleState,
                                        & Self::MakeObjectsInvisibleProcessing );

  m_VisibilityStateMachine.AddTransition( this->m_VisibleState,
                                          this->m_ValidTimeStampInput,
                                          this->m_VisibleState,
                                        & Self::NoProcessing );

  m_VisibilityStateMachine.AddTransition( this->m_VisibleState,
                                          this->m_SetActorVisibilityInput,
                                          this->m_VisibleState,
                                        & Self::SetActorVisibleProcessing );

  m_VisibilityStateMachine.AddTransition( this->m_InvisibleState,
                                          this->m_SetActorVisibilityInput,
                                          this->m_InvisibleState,
                                        & Self::SetActorInvisibleProcessing );

  m_VisibilitySetActor = NULL;

  m_VisibilityStateMachine.SelectInitialState( this->m_InvisibleState );
  m_VisibilityStateMachine.SetReadyToRun();
}

/** Destructor */
ObjectRepresentation::~ObjectRepresentation()
{
  // This must be invoked in order to prevent Memory Leaks.
  this->DeleteActors();
}

/** Get the red color component */
float ObjectRepresentation::GetRed() const
{
  return m_Color[0];
}

/** Get the green color component */ 
float ObjectRepresentation::GetGreen() const
{
  return m_Color[1];
}
/** Get the blue color component */
float ObjectRepresentation::GetBlue() const  
{
  return m_Color[2];
}

/** Add an actor to the actors list */
void ObjectRepresentation::AddActor( vtkProp * actor )
{
  // Initialize objects based on the visibility state machine.
  this->RequestSetActorVisibility( actor );
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
    vtkActor * va = dynamic_cast< vtkActor * >( *it );
    if( va != NULL )
      {
      va->GetProperty()->SetColor(m_Color[0], m_Color[1], m_Color[2]);
      }
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
void ObjectRepresentation::RequestUpdateRepresentation( 
                                        const TimeStamp & time, 
                                        const CoordinateReferenceSystem* cs )
{
  igstkLogMacro( DEBUG, "RequestUpdateRepresentation at time"
                          << time );
  m_TimeToRender = time;
  m_TargetCoordinateSystem = cs;
  igstkPushInputMacro( UpdateRepresentation );
  m_StateMachine.ProcessInputs();
  m_TargetCoordinateSystem = NULL; // Break reference.
}

/** Process the request for updating the transform from the SpatialObject. */
void ObjectRepresentation::RequestGetTransformProcessing()
{
  igstkLogMacro( DEBUG, "RequestUpdatePositionProcessing called ....");
  // The response should be sent back in an event

  m_SpatialObject->RequestComputeTransformTo( this->m_TargetCoordinateSystem );

}


/** Receive the Transform from the SpatialObject via a transduction macro. */
void ObjectRepresentation::ReceiveSpatialObjectTransformProcessing()
{
  m_SpatialObjectTransform = m_SpatialObjectTransformInputToBeSet.GetTransform();

  igstkLogMacro( DEBUG, "Received SpatialObject Transform " << m_SpatialObjectTransform );

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

  this->RequestVerifyTimeStampAndUpdateVisibility();
}

/** Receive No Transform Availabe message from the SpatialObject via a transduction macro. */
void ObjectRepresentation::ReceiveTransformNotAvailableProcessing()
{
  // No new transform for us to use.
  //
  // Check if the old one has not expired.
  //
  this->RequestVerifyTimeStampAndUpdateVisibility();
}

/** Receive No Transform Availabe message from the SpatialObject via a transduction macro. */
void ObjectRepresentation::RequestVerifyTimeStampAndUpdateVisibility()
{
  if( m_TimeToRender.GetExpirationTime() <
    m_SpatialObjectTransform.GetStartTime() ||
    m_TimeToRender.GetStartTime() >
    m_SpatialObjectTransform.GetExpirationTime() )
    {
    m_VisibilityStateMachine.PushInput( m_InvalidTimeStampInput );
    m_VisibilityStateMachine.ProcessInputs();
    }
  else
    {
    m_VisibilityStateMachine.PushInput( m_ValidTimeStampInput );
    m_VisibilityStateMachine.ProcessInputs();
    }

  this->UpdateRepresentationProcessing();
}


/** Null operation for a State Machine transition */
void ObjectRepresentation::NoProcessing()
{
}



/** Make Objects Invisible. This method is called when the Transform time stamp
 * has expired with respect to the requested rendering time. */
void ObjectRepresentation::MakeObjectsInvisibleProcessing()
{
  igstkLogMacro( WARNING, "MakeObjectsInvisibleProcessing at " << m_TimeToRender );

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
  igstkLogMacro( WARNING, "MakeObjectsVisibleProcessing at " << m_TimeToRender );
  
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

void ObjectRepresentation
::RequestSetActorVisibility( vtkProp* p )
{
  m_VisibilitySetActor = p;

  m_VisibilityStateMachine.PushInput( m_SetActorVisibilityInput );
  m_VisibilityStateMachine.ProcessInputs();
}

void ObjectRepresentation
::SetActorVisibleProcessing()
{
  m_VisibilitySetActor->VisibilityOn();
}

void ObjectRepresentation
::SetActorInvisibleProcessing()
{
  m_VisibilitySetActor->VisibilityOff();
}

} // end namespace igstk
