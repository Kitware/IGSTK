/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateSystem.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkCoordinateSystem.h"
#include "igstkCoordinateSystemTransformToResult.h"
#include "igstkCoordinateSystemTransformToErrorResult.h"
#include "igstkCoordinateSystemSetTransformResult.h"

namespace igstk
{ 

// Constructor
CoordinateSystem
::CoordinateSystem():m_StateMachine(this)
{

  // Using a self loop for FindLowestCommonAncestor 
  // will prevent the destructor from being called if
  // m_Parent is never set. Instead, we set the 
  // parent to be NULL.
  this->m_Parent = NULL; 

  // Default transform is identity.
  this->m_TransformToParent.SetToIdentity( 
                                    TimeStamp::GetLongestPossibleTime() );

  //
  // State machine configuration
  // 

  // ---------------
  // States
  // ---------------

  // Initial state
  igstkAddStateMacro( Initialized );

  igstkAddStateMacro( ParentSet   );
  // States for RequestComputeTransformTo
  igstkAddStateMacro( AttemptingComputeTransformTo );
  igstkAddStateMacro( AttemptingComputeTransformToInInitialized );

  // ---------------
  // Inputs
  // ---------------
  igstkAddInputMacro( NullCoordinateSystem  );
  igstkAddInputMacro( ThisCoordinateSystem  );
  igstkAddInputMacro( ValidCoordinateSystem );

  // RequestSetTransformAndParent
  igstkAddInputMacro( NullParent                     ); 
  igstkAddInputMacro( ThisParent                     );
  igstkAddInputMacro( ValidParent                    );

  igstkAddInputMacro( AncestorFound                  );
  igstkAddInputMacro( Disconnected                   );
  igstkAddInputMacro( ParentCausesCycle              );

  // RequestUpdateTransformToParent
  igstkAddInputMacro( UpdateTransformToParent        );

  // RequestDetach
  igstkAddInputMacro( DetachFromParent               );

  // ---------------
  // Transitions
  // ---------------

  // Should be |state| * |input| transitions...
  // i.e. 4 * 10 = 40

  /* Transitions from Initialized */
  igstkAddTransitionMacro( Initialized, ValidParent,
                           ParentSet  , SetTransformAndParent           );
  igstkAddTransitionMacro( Initialized, NullParent ,
                           Initialized, SetTransformAndParentNullParent );
  igstkAddTransitionMacro( Initialized, ThisParent ,
                           Initialized, SetTransformAndParentThisParent );
  igstkAddTransitionMacro( Initialized, ParentCausesCycle,
                           Initialized, SetTransformAndParentCycle );
  igstkAddTransitionMacro( Initialized, NullCoordinateSystem ,
                           Initialized, ComputeTransformToNullTarget );
  igstkAddTransitionMacro( Initialized, ThisCoordinateSystem ,
                           Initialized, ComputeTransformToThisTarget );
  igstkAddTransitionMacro( Initialized, ValidCoordinateSystem,
                           AttemptingComputeTransformToInInitialized,
                           ComputeTransformToValidTarget );
  igstkAddTransitionMacro( Initialized, DetachFromParent,
                           Initialized, DoNothing );
  /* Invalid inputs to Initialized : AncestorFound and Disconnected inputs */
  igstkAddTransitionMacro( Initialized, AncestorFound,
                           Initialized, InvalidRequest );
  igstkAddTransitionMacro( Initialized, Disconnected ,
                           Initialized, InvalidRequest );
  igstkAddTransitionMacro( Initialized, UpdateTransformToParent ,
                           Initialized, InvalidRequest );

  /* Transitions from ParentSet */
  igstkAddTransitionMacro( ParentSet  , NullParent ,
                           ParentSet  , SetTransformAndParentNullParent );
  igstkAddTransitionMacro( ParentSet  , ThisParent ,
                           ParentSet  , SetTransformAndParentThisParent );
  igstkAddTransitionMacro( ParentSet  , ValidParent,
                           ParentSet  , SetTransformAndParent           );
  igstkAddTransitionMacro( ParentSet  , ParentCausesCycle,
                           ParentSet  , SetTransformAndParentCycle  );
  igstkAddTransitionMacro( ParentSet  , NullCoordinateSystem ,
                           ParentSet  , ComputeTransformToNullTarget );
  igstkAddTransitionMacro( ParentSet  , ThisCoordinateSystem ,
                           ParentSet  , ComputeTransformToThisTarget );
  igstkAddTransitionMacro( ParentSet  , ValidCoordinateSystem ,
                           AttemptingComputeTransformTo, 
                           ComputeTransformToValidTarget);
  igstkAddTransitionMacro( ParentSet  , DetachFromParent,
                           Initialized, DetachFromParent );
  /* Invalid inputs to ParentSet : AncestorFound and Disconnected inputs */
  igstkAddTransitionMacro( ParentSet  , AncestorFound,
                           ParentSet, InvalidRequest );
  igstkAddTransitionMacro( ParentSet  , Disconnected,
                           ParentSet, InvalidRequest );
  igstkAddTransitionMacro( ParentSet, UpdateTransformToParent ,
                           ParentSet, UpdateTransformToParent );

  /* Transitions from AttemptingComputeTransformToInInitialized */
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized,
                           Disconnected,
                           Initialized,
                           ComputeTransformToDisconnected);
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized,
                           AncestorFound,
                           Initialized,
                           ComputeTransformToAncestorFound);
  /* Invalid inputs to AttemptingComputeTransformToInitialized :
  *   NullParent, ThisParent, ValidParent, ParentCausesCycle, 
  *   NullCoordinateSystem
  *   ThisCoordinateSystem, 
  *   ValidCoordinateSystem,
  *   DetachFromParent
  */
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized,
                           NullParent,
                           AttemptingComputeTransformToInInitialized,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized,
                           ThisParent,
                           AttemptingComputeTransformToInInitialized,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized,
                           ValidParent,
                           AttemptingComputeTransformToInInitialized,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized,
                           ParentCausesCycle,
                           AttemptingComputeTransformToInInitialized,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized,
                           NullCoordinateSystem,
                           AttemptingComputeTransformToInInitialized,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized,
                           ThisCoordinateSystem,
                           AttemptingComputeTransformToInInitialized,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized,
                           ValidCoordinateSystem,
                           AttemptingComputeTransformToInInitialized,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized,
                           DetachFromParent,
                           AttemptingComputeTransformToInInitialized,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized, 
                           UpdateTransformToParent ,
                           AttemptingComputeTransformToInInitialized, 
                           InvalidRequest );

  /* Transitions from AttemptingComputeTransformTo */
  igstkAddTransitionMacro( AttemptingComputeTransformTo,
                           Disconnected,
                           ParentSet,
                           ComputeTransformToDisconnected );
  igstkAddTransitionMacro( AttemptingComputeTransformTo,
                           AncestorFound,
                           ParentSet,
                           ComputeTransformToAncestorFound );
  /* Invalid inputs to AttemptingComputeTransformTo :
  *   NullParent, 
  *   ThisParent, 
  *   ValidParent, 
  *   ParentCausesCycle, 
  *   NullCoordinateSystem
  *   ThisCoordinateSystem, 
  *   ValidCoordinateSystem,
  *   DetachFromParent
  */
  igstkAddTransitionMacro( AttemptingComputeTransformTo,
                           NullParent,
                           AttemptingComputeTransformTo,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformTo,
                           ThisParent,
                           AttemptingComputeTransformTo,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformTo,
                           ValidParent,
                           AttemptingComputeTransformTo,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformTo,
                           ParentCausesCycle,
                           AttemptingComputeTransformTo,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformTo,
                           NullCoordinateSystem,
                           AttemptingComputeTransformTo,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformTo,
                           ThisCoordinateSystem,
                           AttemptingComputeTransformTo,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformTo,
                           ValidCoordinateSystem,
                           AttemptingComputeTransformTo,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformTo,
                           DetachFromParent,
                           AttemptingComputeTransformTo,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformTo, 
                           UpdateTransformToParent ,
                           AttemptingComputeTransformTo, 
                           InvalidRequest );

  igstkSetInitialStateMacro( Initialized );
  m_StateMachine.SetReadyToRun();
} 

// Destructor
CoordinateSystem
::~CoordinateSystem()  
{
  // Break references
  m_Parent = NULL;
  m_ParentFromRequestSetTransformAndParent = NULL;
  m_TargetFromRequestComputeTransformTo = NULL;
  m_LowestCommonAncestor = NULL;
}


// Print object information
void CoordinateSystem::PrintSelf( 
  std::ostream& os, itk::Indent indent ) const
{
  os << indent << "COORDINATE SYSTEM NAME = " << this->m_Name << std::endl;
  Superclass::PrintSelf(os, indent);

  os << indent << "Transform to parent = " << std::endl;
  this->m_TransformToParent.Print( os, indent );

  os << indent << "Transform from RequestSetTransformAndParent (temporary) = ";
  os << std::endl; 
  this->m_TransformFromRequestSetTransformAndParent.Print( os, indent );

  os << indent << "Parent from RequestSetTransformAndParent (temporary) = ";
  if ( this->m_ParentFromRequestSetTransformAndParent.IsNotNull() )
    {
    os << std::endl;
    this->m_ParentFromRequestSetTransformAndParent->PrintSelf( os, indent );
    }
  else
    {
    os << " NULL " << std::endl;
    }

  os << indent << "Target from RequestComputeTransformTo (temporary) = ";
  if ( this->m_TargetFromRequestComputeTransformTo.IsNotNull() )
    {
    os << std::endl;
    this->m_TargetFromRequestComputeTransformTo->PrintSelf( os, indent );
    }
  else
    {
    os << " NULL" << std::endl;
    }

  os << indent << "Lowest common ancestor (temporary) = ";
  if ( this->m_LowestCommonAncestor.IsNotNull() )
    {
    os << std::endl;
    this->m_LowestCommonAncestor->PrintSelf( os, indent );
    }
  else
    {
    os << " NULL" << std::endl;
    }

  os << indent << "COORDINATE SYSTEM PARENT = ";
  if ( this->m_Parent.IsNotNull() )
    {
    os << std::endl;
    this->m_Parent->PrintSelf( os, indent.GetNextIndent() );
    // os << indent.GetNextIndent() << this->m_Parent;
    }
  else
    {
    os << "NULL" << std::endl;
    }

}

void CoordinateSystem
::RequestComputeTransformTo(const CoordinateSystem* target)
{
  if (NULL == target)
    {
    igstkPushInputMacro( NullCoordinateSystem );
    m_StateMachine.ProcessInputs();
    return;
    }
  if (target == this)
    {
    igstkPushInputMacro( ThisCoordinateSystem );
    m_StateMachine.ProcessInputs();
    return;
    }

  this->m_TargetFromRequestComputeTransformTo = target;

  igstkPushInputMacro( ValidCoordinateSystem );
  m_StateMachine.ProcessInputs();

  // Break the reference once we're done with it...
  this->m_TargetFromRequestComputeTransformTo = NULL;

  return;
}

void CoordinateSystem
::ComputeTransformToNullTargetProcessing()
{
  // ERROR - Can't compute transform to a NULL target.
  CoordinateSystemTransformToErrorResult payload;
  payload.Initialize( this, m_TargetFromRequestComputeTransformTo );

  CoordinateSystemTransformToNullTargetEvent event;
  event.Set( payload );

  this->InvokeEvent( event );
}

void CoordinateSystem
::ComputeTransformToThisTargetProcessing()
{
  // We have been asked for the transform to ourself --
  // return identity.
  igstk::Transform transform;
  transform.SetToIdentity( TimeStamp::GetLongestPossibleTime() );

  CoordinateSystemTransformToResult payload;
  payload.Initialize(transform, this, this);

  CoordinateSystemTransformToEvent event;
  event.Set(payload);

  this->InvokeEvent( event );
}


void CoordinateSystem
::ComputeTransformToValidTargetProcessing()
{
  if (this->m_TargetFromRequestComputeTransformTo.IsNull())
    {
    igstkPushInputMacro( Disconnected );
    m_StateMachine.ProcessInputs();
    return;
    }

  this->FindLowestCommonAncestor( this->m_TargetFromRequestComputeTransformTo );
}

void CoordinateSystem
::ComputeTransformToDisconnectedProcessing()
{
  // ERROR - Disconnected from the target of ComputeTransformTo
  CoordinateSystemTransformToErrorResult payload;
  payload.Initialize(this, this->m_TargetFromRequestComputeTransformTo);

  CoordinateSystemTransformToDisconnectedEvent event;
  event.Set( payload );

  this->InvokeEvent ( event );
}

void CoordinateSystem
::ComputeTransformToAncestorFoundProcessing()
{
  igstkLogMacro(DEBUG, "0x" << this
                        << " [" << this->GetName() 
                        << "] " 
                        << " and "
                        << "0x" 
                        << m_TargetFromRequestComputeTransformTo.GetPointer()
                        << " [" 
                        << m_TargetFromRequestComputeTransformTo->GetName()
                        << "] " 
                        << " have lowest common ancestor : "
                        << "0x" << this->m_LowestCommonAncestor.GetPointer()
                        << " [" << this->m_LowestCommonAncestor->GetName()
                        << "] "
                        << "\n" );

  Transform thisToAncestor   = 
            this->ComputeTransformTo( m_LowestCommonAncestor );

  Transform targetToAncestor = 
    m_TargetFromRequestComputeTransformTo
                              ->ComputeTransformTo( m_LowestCommonAncestor );

  Transform result = Transform::TransformCompose(
                                              targetToAncestor.GetInverse(),
                                              thisToAncestor);

  // Create event
  CoordinateSystemTransformToResult payload;
  payload.Initialize(result, 
                     this,
                     m_TargetFromRequestComputeTransformTo,
                     m_LowestCommonAncestor);

  CoordinateSystemTransformToEvent event;
  event.Set( payload );

  this->InvokeEvent( event );
}

Transform
CoordinateSystem::
ComputeTransformTo(const CoordinateSystem* ancestor) const
{
  if ( ancestor == this )
    {
    Transform identity;
    identity.SetToIdentity(igstk::TimeStamp::GetLongestPossibleTime());
    return identity;
    }
  else
    {
    Transform result = Transform::TransformCompose( 
      this->m_Parent->ComputeTransformTo(ancestor),
      this->m_TransformToParent);
    return result;
    }
}

void CoordinateSystem
::RequestSetTransformAndParent(const Transform & transform, 
                               const CoordinateSystem* parent)
{
  if (NULL == parent)
    {
    igstkPushInputMacro( NullParent );
    m_StateMachine.ProcessInputs();
    return;
    }

  //
  // Pointer comparison
  //
  if (parent == this)
    {
    igstkPushInputMacro( ThisParent );
    m_StateMachine.ProcessInputs();
    return;
    }

  if (parent->CanReach( this ) == true)
    {
    // 
    // Setting this parent will cause a cycle in the coordinate
    // system graph -- push an error input...
    // 
    this->m_ParentFromRequestSetTransformAndParent = parent;

    igstkPushInputMacro( ParentCausesCycle );
    m_StateMachine.ProcessInputs();

    // Break reference when we're done with it...
    this->m_ParentFromRequestSetTransformAndParent = NULL;
    return;
    }
  else
    {
    this->RequestDetachFromParent();
    this->m_TransformFromRequestSetTransformAndParent = transform;
    this->m_ParentFromRequestSetTransformAndParent = parent;

    igstkPushInputMacro( ValidParent );
    m_StateMachine.ProcessInputs();

    // Break reference when we're done with it...
    this->m_ParentFromRequestSetTransformAndParent = NULL;
    return;
    }
}

void CoordinateSystem
::SetTransformAndParentProcessing()
{
  this->m_Parent = this->m_ParentFromRequestSetTransformAndParent;
  this->m_TransformToParent = this->m_TransformFromRequestSetTransformAndParent;
  
  CoordinateSystemSetTransformResult payload;
  
  const CoordinateSystem * parentCoordinateSystem =
      Friends::CoordinateSystemHelper::GetCoordinateSystem( m_Parent );

  payload.Initialize(this->m_TransformToParent,parentCoordinateSystem ,
                     this,
                     true);

  CoordinateSystemSetTransformEvent event;
  event.Set( payload );

  this->InvokeEvent( event );
}

void CoordinateSystem
::RequestUpdateTransformToParent(const Transform & transform)
{
  this->m_TransformFromRequestSetTransformAndParent = transform;
  igstkPushInputMacro( UpdateTransformToParent );
  m_StateMachine.ProcessInputs();
}

void CoordinateSystem
::UpdateTransformToParentProcessing()
{  
  this->m_TransformToParent = this->m_TransformFromRequestSetTransformAndParent;
}

void CoordinateSystem
::SetTransformAndParentNullParentProcessing()
{
  // ERROR - Can't set parent to NULL.
  CoordinateSystemNullParentEvent event;
  this->InvokeEvent( event );
}

void CoordinateSystem
::SetTransformAndParentThisParentProcessing()
{
  // ERROR - Can't set our parent to this.
  CoordinateSystemThisParentEvent event;
  this->InvokeEvent( event );
}

void 
CoordinateSystem
::FindLowestCommonAncestor( const CoordinateSystem* B)
{
  //
  // A simple algorithm is to find a path to a root for each 
  // coordinate system and then find the earliest path intersection. We 
  // do this by walking up one path and comparing it against every other
  // node in the other path.
  // 
  typedef const CoordinateSystem* CoordinateSystemConstPointer;

  CoordinateSystemConstPointer aSmart = this;
  CoordinateSystemConstPointer bSmart = B;

  if( aSmart == bSmart )
    {
    this->m_LowestCommonAncestor = aSmart;
    // Push the AncestorFound input. We should be in an 
    // attempting state (AttemptingComputeTransformToInInitialized or 
    // AttemptingComputeTransformTo) as a result of RequestComputeTransformTo
    // This input allows us to return to our previous state.
    igstkPushInputMacro( AncestorFound );
    m_StateMachine.ProcessInputs();
    // Break reference when we're done with it.
    this->m_LowestCommonAncestor = NULL;
    return;
    }

  if( NULL == bSmart )
    {
    // Error - target for FindLowestCommonAncestor is NULL.
    // Push the Disconnected input. We should be in an 
    // attempting state (AttemptingComputeTransformToInInitialized or 
    // AttemptingComputeTransformTo) as a result of RequestComputeTransformTo
    // This input allows us to return to our previous state.
    igstkPushInputMacro( Disconnected );
    m_StateMachine.ProcessInputs();
    return;
    }

  CoordinateSystemConstPointer aTemp; 
  CoordinateSystemConstPointer bTemp;

  for(bTemp = bSmart;
      bTemp != NULL;
      bTemp = bTemp->m_Parent)
    {
    for(aTemp = aSmart; 
        aTemp != NULL;
        aTemp = aTemp->m_Parent)
      {
      if (aTemp == bTemp)
        {
        this->m_LowestCommonAncestor = aTemp;
        // Push the AncestorFound input. We should be in an 
        // attempting state (AttemptingComputeTransformToInInitialized or 
        // AttemptingComputeTransformTo) as a result of 
        // RequestComputeTransformTo. This input allows us to return to 
        // our previous state.
        igstkPushInputMacro( AncestorFound );
        m_StateMachine.ProcessInputs();
        // Break reference when we're done with it.
        this->m_LowestCommonAncestor = NULL;
        return;
        }
      }
    }

  // Error - can't find a lowest common ancestor. Must
  // be disconnected.
  // Push the Disconnected input. We should be in an 
  // attempting state (AttemptingComputeTransformToInInitialized or 
  // AttemptingComputeTransformTo) as a result of RequestComputeTransformTo
  // This input allows us to return to our previous state.
  //
  igstkPushInputMacro( Disconnected );
  m_StateMachine.ProcessInputs();
  return;
}

bool
CoordinateSystem
::CanReach(const CoordinateSystem* target) const
{
  // target is reachable since we're "target"
  if (target == this)
    {
    return true;
    }
  // The top of the tree 
  else if (this->m_Parent.IsNull())
    {
    return false;
    }
  // If we're not "target", can our parent
  //  reach "target"? Note, if parent was 
  //  NULL, we'd be executing the branch 
  //  above. 
  // 
  else
    {
    return this->m_Parent->CanReach(target);
    }
}

void
CoordinateSystem
::SetTransformAndParentCycleProcessing()
{
  CoordinateSystemParentCycleEvent event;
  event.Set( this->m_ParentFromRequestSetTransformAndParent );

  this->InvokeEvent( event );
}

void 
CoordinateSystem
::InvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "Invalid request made to the State Machine" );
}

void 
CoordinateSystem
::RequestGetTransformToParent()
{
  this->RequestComputeTransformTo(this->m_Parent);
}

const CoordinateSystem* 
CoordinateSystem
::GetCoordinateSystem() const
{
  return this;
}

void 
CoordinateSystem
::RequestDetachFromParent()
{
  igstkPushInputMacro( DetachFromParent );
  m_StateMachine.ProcessInputs();
}

void
CoordinateSystem
::DoNothingProcessing()
{
  // Purposely does nothing.
}

void
CoordinateSystem
::DetachFromParentProcessing()
{
  //
  // The values of m_Parent and m_Transform
  // must match the constructor settings.
  //

  //Preparing the event to be sent.
  const CoordinateSystem * parentCoordinateSystem =
      Friends::CoordinateSystemHelper::GetCoordinateSystem( m_Parent );
  CoordinateSystemSetTransformResult payload;

  payload.Initialize(this->m_TransformToParent,parentCoordinateSystem , 
                     this, 
                     false);

  CoordinateSystemSetTransformEvent event;
  event.Set( payload );
  
  // We set the parent to be NULL.
  this->m_Parent = NULL; 

  // Default transform is identity.
  this->m_TransformToParent.SetToIdentity( 
                                    TimeStamp::GetLongestPossibleTime() );
  this->InvokeEvent( event );
}

} // end namespace igstk
