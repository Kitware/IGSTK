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


/** Convenience macro for adding Inputs to the State Machine */
#define igstkAddInputMacro2( statemachine, inputname ) \
    this->m_##statemachine.AddInput( this->m_##inputname##Input,  \
                                   #inputname"Input" );
/** Convenience macro for adding States to the State Machine */
#define igstkAddStateMacro2( statemachine, statename ) \
    this->m_##statemachine.AddState( this->m_##statename##State,\
                                   #statename"State" );

/** Convenience macro for adding Transitions to the State Machine */
#define igstkAddTransitionMacro2(statemachine,state1,input,state2,action )\
    this->m_##statemachine.AddTransition( this->m_##state1##State,   \
                                        this->m_##input##Input,    \
                                        this->m_##state2##State,   \
                                      & Self::action##Processing );

namespace igstk
{

/** Constructor */
ObjectRepresentation::ObjectRepresentation():
  m_StateMachine(this),m_VisibilityStateMachine(this)
{
  this->m_Color[0] = 1.0;
  this->m_Color[1] = 1.0;
  this->m_Color[2] = 1.0;
  this->m_Opacity = 1.0;
  this->m_SpatialObject = NULL;

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
                           ReceiveTransformNotAvailable );

  igstkSetInitialStateMacro( NullSpatialObject );

  this->m_StateMachine.SetReadyToRun();


  // Configure the auxiliary State Machine that controls the Visibility of the
  // objects.
  igstkAddInputMacro2( VisibilityStateMachine, ValidTimeStamp );
  igstkAddInputMacro2( VisibilityStateMachine, InvalidTimeStamp );
  igstkAddInputMacro2( VisibilityStateMachine, SetActorVisibility );

  igstkAddStateMacro2( VisibilityStateMachine, Invisible );
  igstkAddStateMacro2( VisibilityStateMachine, Visible );

  igstkAddTransitionMacro2( VisibilityStateMachine,
                            Invisible, InvalidTimeStamp,
                            Invisible, No);

  igstkAddTransitionMacro2( VisibilityStateMachine,
                            Invisible, ValidTimeStamp,
                            Visible, MakeObjectsVisible);

  igstkAddTransitionMacro2( VisibilityStateMachine,
                            Visible, InvalidTimeStamp,
                            Invisible, MakeObjectsInvisible );

  igstkAddTransitionMacro2( VisibilityStateMachine,
                            Visible, ValidTimeStamp,
                            Visible, No );

  igstkAddTransitionMacro2( VisibilityStateMachine,
                            Visible, SetActorVisibility,
                            Visible, SetActorVisible );

  igstkAddTransitionMacro2( VisibilityStateMachine,
                            Invisible, SetActorVisibility,
                            Invisible, SetActorInvisible );

  this->m_VisibilitySetActor = NULL;

  this->m_VisibilityStateMachine.SelectInitialState( this->m_InvisibleState );
  this->m_VisibilityStateMachine.SetReadyToRun();
}

/** Destructor */
ObjectRepresentation::~ObjectRepresentation()
{
  // This must be invoked in order to prevent Memory Leaks.
  this->DeleteActors();
}

/** Get the red color component */
ObjectRepresentation::ColorScalarType
ObjectRepresentation::GetRed() const
{
  return this->m_Color[0];
}

/** Get the green color component */ 
ObjectRepresentation::ColorScalarType
ObjectRepresentation::GetGreen() const
{
  return this->m_Color[1];
}
/** Get the blue color component */
ObjectRepresentation::ColorScalarType
ObjectRepresentation::GetBlue() const  
{
  return this->m_Color[2];
}

/** Add an actor to the actors list */
void ObjectRepresentation::AddActor( vtkProp * actor )
{
  // Initialize objects based on the visibility state machine.
  this->RequestSetActorVisibility( actor );
  this->m_Actors.push_back( actor );
}

/** Empty the list of actors */
void ObjectRepresentation::DeleteActors()
{
  ActorsListType::iterator it = this->m_Actors.begin();
  while(it != this->m_Actors.end())
    {
    (*it)->Delete();
    it++;
    }

  // Reset the list of actors
  this->m_Actors.clear();
}


/** Set the Spatial Object */
void ObjectRepresentation
::RequestSetSpatialObject( const SpatialObject * spatialObject )
{
  // This const_cast is done because the ObjectRepresentation class invoke
  // Request methods in the SpatialObject, and those methods modify the state
  // of its internal StateMachine. It is however desirable to keep the outside
  // API of this class refering to a const object.
  this->m_SpatialObjectToAdd = const_cast< SpatialObject *>( spatialObject );
  if( !this->m_SpatialObjectToAdd )
    {
    igstkPushInputMacro( NullSpatialObject );
    this->m_StateMachine.ProcessInputs();
    }
  else
    {
    igstkPushInputMacro( ValidSpatialObject );
    this->m_StateMachine.ProcessInputs();
    }

}


/** Set the Spatial Object */
void ObjectRepresentation::SetSpatialObjectProcessing()
{
  this->m_SpatialObject = this->m_SpatialObjectToAdd;
  this->ObserveSpatialObjectTransformInput( this->m_SpatialObject );
}

/** Set the color */
void ObjectRepresentation::SetColor(
  ColorScalarType r, ColorScalarType g, ColorScalarType b)
{
  if( this->m_Color[0] == r && 
      this->m_Color[1] == g && 
      this->m_Color[2] == b    )
    {
    return;
    }
  this->m_Color[0] = r;
  this->m_Color[1] = g;
  this->m_Color[2] = b;

  // Update all the actors
  ActorsListType::iterator it = this->m_Actors.begin();
  while(it != this->m_Actors.end())
    {
    vtkActor * va = dynamic_cast< vtkActor * >( *it );
    if( va != NULL )
      {
      va->GetProperty()->SetColor(
        this->m_Color[0], this->m_Color[1], this->m_Color[2] );
      }
    it++;
    }
}

/** Set the opacity */
void ObjectRepresentation::SetOpacity(OpacityType alpha)
{
  if( this->m_Opacity == alpha )
    {
    return;
    }
  this->m_Opacity = alpha;

  // Update all the actors
  ActorsListType::iterator it = this->m_Actors.begin();
  while(it != this->m_Actors.end())
    {
    vtkActor * va = dynamic_cast< vtkActor * >( *it );
    if( va )
      {
      va->GetProperty()->SetOpacity(this->m_Opacity);
      }
    it++;
    }
}


/** Request Update the object representation (i.e vtkActors). */
void ObjectRepresentation::RequestUpdateRepresentation( 
                                        const TimeStamp & time, 
                                        const CoordinateSystem* cs )
{
  igstkLogMacro( DEBUG, "RequestUpdateRepresentation at time"
                          << time );
  this->m_TimeToRender = time;
  this->m_TargetCoordinateSystem = cs;
  igstkPushInputMacro( UpdateRepresentation );
  this->m_StateMachine.ProcessInputs();
  this->m_TargetCoordinateSystem = NULL; // Break reference.
}

/** Process the request for updating the transform from the SpatialObject. */
void ObjectRepresentation::RequestGetTransformProcessing()
{
  igstkLogMacro( DEBUG, "RequestGetTransformProcessing called ....");

  // The response to this request is part of the internal dialog between the
  // ObjectRepresentation and the SpatialObject. There is no need to report the
  // answer outside of the ObjectRepresentation.
  this->m_SpatialObject->RequestComputeTransformTo( 
    this->m_TargetCoordinateSystem );

}


/** Receive the Transform from the SpatialObject via a transduction macro. */
void ObjectRepresentation::ReceiveSpatialObjectTransformProcessing()
{
  this->m_SpatialObjectTransform = 
    this->m_SpatialObjectTransformInputToBeSet.GetTransform();

  igstkLogMacro( DEBUG, 
    "Received SpatialObject Transform " << this->m_SpatialObjectTransform );

  vtkMatrix4x4* vtkMatrix = vtkMatrix4x4::New();

  this->m_SpatialObjectTransform.ExportTransform( *vtkMatrix );

  // Update all the actors
  ActorsListType::iterator it = this->m_Actors.begin();
  while( it != this->m_Actors.end() )
    {
    vtkProp3D::SafeDownCast(*it)->SetUserMatrix(vtkMatrix);
    it++;
    }

  vtkMatrix->Delete();

  this->RequestVerifyTimeStampAndUpdateVisibility();
}

/** Receive No Transform Available message from the SpatialObject via a
 * transduction macro. */
void ObjectRepresentation::ReceiveTransformNotAvailableProcessing()
{
  // No new transform for us to use.
  //
  // Check if the old one has not expired.
  //
  this->RequestVerifyTimeStampAndUpdateVisibility();
}

/** This method checks the time stamp of the transform
   *  and modifies the visibility of the objects accordingly. */
void ObjectRepresentation::RequestVerifyTimeStampAndUpdateVisibility()
{
  if( ! this->VerifyTimeStamp() )
    {
    this->m_VisibilityStateMachine.PushInput( this->m_InvalidTimeStampInput );
    this->m_VisibilityStateMachine.ProcessInputs();
    }
  else
    {
    this->m_VisibilityStateMachine.PushInput( this->m_ValidTimeStampInput );
    this->m_VisibilityStateMachine.ProcessInputs();
    }

  this->UpdateRepresentationProcessing();
}

/** Verify time stamp */
bool ObjectRepresentation::VerifyTimeStamp() const
{
  if( this->m_TimeToRender.GetExpirationTime() <
    this->m_SpatialObjectTransform.GetStartTime() ||
    this->m_TimeToRender.GetStartTime() >
    this->m_SpatialObjectTransform.GetExpirationTime() )
    {
    return false;
    }
  else
    {
    return true;
    }
}
 
/** Null operation for a State Machine transition */
void ObjectRepresentation::NoProcessing()
{
}


/** Make Objects Invisible. This method is called when the Transform time stamp
 * has expired with respect to the requested rendering time. */
void ObjectRepresentation::MakeObjectsInvisibleProcessing()
{
  igstkLogMacro( WARNING, 
    "MakeObjectsInvisibleProcessing at " << this->m_TimeToRender );

  this->InvokeEvent( TransformExpiredErrorEvent() );

  ActorsListType::iterator it = this->m_Actors.begin();
  while(it != this->m_Actors.end())
    {
    (*it)->VisibilityOff();
    it++;
    }
}


/** Make Objects Visible. This method is called when the Transform time stamp
 * is valid with respect to the requested rendering time. */
void ObjectRepresentation::MakeObjectsVisibleProcessing()
{
  igstkLogMacro( WARNING, 
    "MakeObjectsVisibleProcessing at " << this->m_TimeToRender );
  
  ActorsListType::iterator it = this->m_Actors.begin();
  while(it != this->m_Actors.end())
    {
    (*it)->VisibilityOn();
    it++;
    }
}


/** Report an invalid request made to the State Machine */
void ObjectRepresentation::ReportInvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "Invalid request in ObjectRepresentation\n");
  InvalidRequestErrorEvent event;
  this->InvokeEvent( event );
}


/** Print Self function */
void ObjectRepresentation
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Color: " << this->m_Color[0] << " : ";
  os << this->m_Color[1] << " : " << this->m_Color[2] << std::endl;
  os << indent << "Opacity: " << this->m_Opacity << std::endl;
}

void ObjectRepresentation
::RequestSetActorVisibility( vtkProp* p )
{
  this->m_VisibilitySetActor = p;

  this->m_VisibilityStateMachine.PushInput( this->m_SetActorVisibilityInput );
  this->m_VisibilityStateMachine.ProcessInputs();
}

void ObjectRepresentation
::SetActorVisibleProcessing()
{
  this->m_VisibilitySetActor->VisibilityOn();
}

void ObjectRepresentation
::SetActorInvisibleProcessing()
{
  this->m_VisibilitySetActor->VisibilityOff();
}

TimeStamp ObjectRepresentation
::GetRenderTimeStamp() const
{
  return this->m_TimeToRender;
}

} // end namespace igstk
