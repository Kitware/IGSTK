/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateReferenceSystem.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkCoordinateReferenceSystem.h"
#include "igstkCoordinateSystemTransformToResult.h"
#include "igstkCoordinateSystemTransformToErrorResult.h"

namespace igstk
{ 

// Constructor
CoordinateReferenceSystem
::CoordinateReferenceSystem():m_StateMachine(this)
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
  igstkAddInputMacro( NullCoordinateReferenceSystem  );
  igstkAddInputMacro( ThisCoordinateReferenceSystem  );
  igstkAddInputMacro( ValidCoordinateReferenceSystem );

  // RequestSetTransformAndParent
  igstkAddInputMacro( NullParent                     ); 
  igstkAddInputMacro( ThisParent                     );
  igstkAddInputMacro( ValidParent                    );

  igstkAddInputMacro( AncestorFound                  );
  igstkAddInputMacro( Disconnected                   );
  igstkAddInputMacro( ParentCausesCycle              );

  // RequestDetach
  igstkAddInputMacro( DetachFromParent               );

  // ---------------
  // Transitions
  // ---------------

  // Should be |state| * |input| transitions...
  // i.e. 4 * 9 = 36

  /* Transitions from Initialized */
  igstkAddTransitionMacro( Initialized, ValidParent,
                           ParentSet  , SetTransformAndParent           );
  igstkAddTransitionMacro( Initialized, NullParent ,
                           Initialized, SetTransformAndParentNullParent );
  igstkAddTransitionMacro( Initialized, ThisParent ,
                           Initialized, SetTransformAndParentThisParent );
  igstkAddTransitionMacro( Initialized, ParentCausesCycle,
                           Initialized, SetTransformAndParentCycle );
  igstkAddTransitionMacro( Initialized, NullCoordinateReferenceSystem ,
                           Initialized, ComputeTransformToNullTarget );
  igstkAddTransitionMacro( Initialized, ThisCoordinateReferenceSystem ,
                           Initialized, ComputeTransformToThisTarget );
  igstkAddTransitionMacro( Initialized, ValidCoordinateReferenceSystem,
                           AttemptingComputeTransformToInInitialized,
                           ComputeTransformToValidTarget );
  igstkAddTransitionMacro( Initialized, DetachFromParent,
                           Initialized, DoNothing );
  /* Invalid inputs to Initialized : AncestorFound and Disconnected inputs */
  igstkAddTransitionMacro( Initialized, AncestorFound,
                           Initialized, InvalidRequest );
  igstkAddTransitionMacro( Initialized, Disconnected ,
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
  igstkAddTransitionMacro( ParentSet  , NullCoordinateReferenceSystem ,
                           ParentSet  , ComputeTransformToNullTarget );
  igstkAddTransitionMacro( ParentSet  , ThisCoordinateReferenceSystem ,
                           ParentSet  , ComputeTransformToThisTarget );
  igstkAddTransitionMacro( ParentSet  , ValidCoordinateReferenceSystem ,
                           AttemptingComputeTransformTo, 
                           ComputeTransformToValidTarget);
  igstkAddTransitionMacro( ParentSet  , DetachFromParent,
                           Initialized, DetachFromParent );
  /* Invalid inputs to ParentSet : AncestorFound and Disconnected inputs */
  igstkAddTransitionMacro( ParentSet  , AncestorFound,
                           ParentSet, InvalidRequest );
  igstkAddTransitionMacro( ParentSet  , Disconnected,
                           ParentSet, InvalidRequest );

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
  *   NullCoordinateReferenceSystem
  *   ThisCoordinateReferenceSystem, 
  *   ValidCoordinateReferenceSystem,
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
                           NullCoordinateReferenceSystem,
                           AttemptingComputeTransformToInInitialized,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized,
                           ThisCoordinateReferenceSystem,
                           AttemptingComputeTransformToInInitialized,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized,
                           ValidCoordinateReferenceSystem,
                           AttemptingComputeTransformToInInitialized,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized,
                           DetachFromParent,
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
  *   NullCoordinateReferenceSystem
  *   ThisCoordinateReferenceSystem, 
  *   ValidCoordinateReferenceSystem,
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
                           NullCoordinateReferenceSystem,
                           AttemptingComputeTransformTo,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformTo,
                           ThisCoordinateReferenceSystem,
                           AttemptingComputeTransformTo,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformTo,
                           ValidCoordinateReferenceSystem,
                           AttemptingComputeTransformTo,
                           InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformTo,
                           DetachFromParent,
                           AttemptingComputeTransformTo,
                           InvalidRequest );

  igstkSetInitialStateMacro( Initialized );
  m_StateMachine.SetReadyToRun();
} 

// Destructor
CoordinateReferenceSystem
::~CoordinateReferenceSystem()  
{
  // Break references
  m_Parent = NULL;
  m_ParentFromRequestSetTransformAndParent = NULL;
  m_TargetFromRequestComputeTransformTo = NULL;
  m_LowestCommonAncestor = NULL;
}


// Print object information
void CoordinateReferenceSystem::PrintSelf( 
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

void CoordinateReferenceSystem
::RequestComputeTransformTo(const CoordinateReferenceSystem* target)
{
  if (NULL == target)
    {
    igstkPushInputMacro( NullCoordinateReferenceSystem );
    m_StateMachine.ProcessInputs();
    return;
    }
  if (target == this)
    {
    igstkPushInputMacro( ThisCoordinateReferenceSystem );
    m_StateMachine.ProcessInputs();
    return;
    }

  this->m_TargetFromRequestComputeTransformTo = target;

  igstkPushInputMacro( ValidCoordinateReferenceSystem );
  m_StateMachine.ProcessInputs();

  // Break the reference once we're done with it...
  this->m_TargetFromRequestComputeTransformTo = NULL;

  return;
}

void CoordinateReferenceSystem
::ComputeTransformToNullTargetProcessing()
{
  // ERROR - Can't compute transform to a NULL target.
  CoordinateReferenceSystemTransformToErrorResult payload;
  payload.Initialize( this, m_TargetFromRequestComputeTransformTo );

  CoordinateReferenceSystemTransformToNullTargetEvent event;
  event.Set( payload );

  this->InvokeEvent( event );
}

void CoordinateReferenceSystem
::ComputeTransformToThisTargetProcessing()
{
  // We have been asked for the transform to ourself --
  // return identity.
  igstk::Transform transform;
  transform.SetToIdentity( TimeStamp::GetLongestPossibleTime() );

  CoordinateReferenceSystemTransformToResult payload;
  payload.Initialize(transform, this, this);

  CoordinateReferenceSystemTransformToEvent event;
  event.Set(payload);

  this->InvokeEvent( event );
}


void CoordinateReferenceSystem
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

void CoordinateReferenceSystem
::ComputeTransformToDisconnectedProcessing()
{
  // ERROR - Disconnected from the target of ComputeTransformTo
  CoordinateReferenceSystemTransformToErrorResult payload;
  payload.Initialize(this, this->m_TargetFromRequestComputeTransformTo);

  CoordinateReferenceSystemTransformToDisconnectedEvent event;
  event.Set( payload );

  this->InvokeEvent ( event );
}

void CoordinateReferenceSystem
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
  CoordinateReferenceSystemTransformToResult payload;
  payload.Initialize(result, this, m_TargetFromRequestComputeTransformTo);

  CoordinateReferenceSystemTransformToEvent event;
  event.Set( payload );

  this->InvokeEvent( event );
}

Transform
CoordinateReferenceSystem::
ComputeTransformTo(const CoordinateReferenceSystem* ancestor) const
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

void CoordinateReferenceSystem
::RequestSetTransformAndParent(const Transform & transform, 
                               const CoordinateReferenceSystem* parent)
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
    this->m_TransformFromRequestSetTransformAndParent = transform;
    this->m_ParentFromRequestSetTransformAndParent = parent;

    igstkPushInputMacro( ValidParent );
    m_StateMachine.ProcessInputs();

    // Break reference when we're done with it...
    this->m_ParentFromRequestSetTransformAndParent = NULL;
    return;
    }
}

void CoordinateReferenceSystem
::SetTransformAndParentProcessing()
{
  this->m_Parent = this->m_ParentFromRequestSetTransformAndParent;
  this->m_TransformToParent = this->m_TransformFromRequestSetTransformAndParent;
}

void CoordinateReferenceSystem
::SetTransformAndParentNullParentProcessing()
{
  // ERROR - Can't set parent to NULL.
  CoordinateReferenceSystemNullParentEvent event;
  this->InvokeEvent( event );
}

void CoordinateReferenceSystem
::SetTransformAndParentThisParentProcessing()
{
  // ERROR - Can't set our parent to this.
  CoordinateReferenceSystemThisParentEvent event;
  this->InvokeEvent( event );
}

void 
CoordinateReferenceSystem
::FindLowestCommonAncestor( const CoordinateReferenceSystem* B)
{
  //
  // A simple algorithm is to find a path to a root for each 
  // coordinate system and then find the earliest path intersection. We 
  // do this by walking up one path and comparing it against every other
  // node in the other path.
  // 
  typedef const CoordinateReferenceSystem* 
                                      CoordinateReferenceSystemConstPointer;

  CoordinateReferenceSystemConstPointer aSmart = this;
  CoordinateReferenceSystemConstPointer bSmart = B;

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

  CoordinateReferenceSystemConstPointer aTemp; 
  CoordinateReferenceSystemConstPointer bTemp;
  CoordinateReferenceSystemConstPointer aTempPrev = NULL;
  CoordinateReferenceSystemConstPointer bTempPrev = NULL;

  for(bTemp = bSmart;
      bTemp != NULL;
      bTemp = bTemp->m_Parent)
    {
    aTempPrev = NULL;
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
      aTempPrev = aTemp;
      bTempPrev = bTemp;
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
CoordinateReferenceSystem
::CanReach(const CoordinateReferenceSystem* target) const
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
CoordinateReferenceSystem
::SetTransformAndParentCycleProcessing()
{
  CoordinateReferenceSystemParentCycleEvent event;
  event.Set( this->m_ParentFromRequestSetTransformAndParent );

  this->InvokeEvent( event );
}

void 
CoordinateReferenceSystem
::InvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "Invalid request made to the State Machine" );
}

void 
CoordinateReferenceSystem
::RequestGetTransformToParent()
{
  this->RequestComputeTransformTo(this->m_Parent);
}

const CoordinateReferenceSystem* 
CoordinateReferenceSystem
::GetCoordinateReferenceSystem() const
{
  return this;
}

void 
CoordinateReferenceSystem
::RequestDetach()
{
  igstkPushInputMacro( DetachFromParent );
  m_StateMachine.ProcessInputs();
}

void
CoordinateReferenceSystem
::DoNothingProcessing()
{
  // Purposely does nothing.
}

void
CoordinateReferenceSystem
::DetachFromParentProcessing()
{
  //
  // The values of m_Parent and m_Transform
  // must match the constructor settings.
  //

  // We set the parent to be NULL.
  this->m_Parent = NULL; 

  // Default transform is identity.
  this->m_TransformToParent.SetToIdentity( 
                                    TimeStamp::GetLongestPossibleTime() );
}

} // end namespace igstk
