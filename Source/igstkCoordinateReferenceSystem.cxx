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

namespace igstk
{ 

/** Constructor */
CoordinateReferenceSystem
::CoordinateReferenceSystem():m_StateMachine(this)
{

  /** Self loop for FindLowestCommonAncestor 
      Will prevent destructor from being called if
      m_Parent is never set.
  */
  this->m_Parent = this; 

  /** Default transform is identity */
  this->m_TransformToParent.SetToIdentity( 
                                    TimeStamp::GetLongestPossibleTime() );

  /**
    State machine stuff
   */ 

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

  // ---------------
  // Transitions
  // ---------------

  // Should be |state| * |input| transitions...
  // i.e. 4 * 9 = 36

  /** Transitions from Initialized */
  igstkAddTransitionMacro( Initialized, ValidParent, ParentSet  , SetTransformAndParent           );
  igstkAddTransitionMacro( Initialized, NullParent , Initialized, SetTransformAndParentNullParent );
  igstkAddTransitionMacro( Initialized, ThisParent , Initialized, SetTransformAndParentThisParent );
  igstkAddTransitionMacro( Initialized, ParentCausesCycle, Initialized, SetTransformAndParentCycle );
  igstkAddTransitionMacro( Initialized, NullCoordinateReferenceSystem , Initialized, ComputeTransformToNullTarget );
  igstkAddTransitionMacro( Initialized, ThisCoordinateReferenceSystem , Initialized, ComputeTransformToThisTarget );
  igstkAddTransitionMacro( Initialized, ValidCoordinateReferenceSystem, AttemptingComputeTransformToInInitialized, ComputeTransformToValidTarget );
  /** Invalid inputs to Initialized : AncestorFound and Disconnected inputs */
  igstkAddTransitionMacro( Initialized, AncestorFound, Initialized, InvalidRequest );
  igstkAddTransitionMacro( Initialized, Disconnected , Initialized, InvalidRequest );

  /** Transitions from ParentSet */
  igstkAddTransitionMacro( ParentSet  , NullParent , ParentSet  , SetTransformAndParentNullParent );
  igstkAddTransitionMacro( ParentSet  , ThisParent , ParentSet  , SetTransformAndParentThisParent );
  igstkAddTransitionMacro( ParentSet  , ValidParent, ParentSet  , SetTransformAndParent           );
  igstkAddTransitionMacro( ParentSet  , ParentCausesCycle, ParentSet, SetTransformAndParentCycle  );
  igstkAddTransitionMacro( ParentSet  , NullCoordinateReferenceSystem  , ParentSet  , ComputeTransformToNullTarget );
  igstkAddTransitionMacro( ParentSet  , ThisCoordinateReferenceSystem  , ParentSet  , ComputeTransformToThisTarget );
  igstkAddTransitionMacro( ParentSet  , ValidCoordinateReferenceSystem , AttemptingComputeTransformTo  , ComputeTransformToValidTarget);
  /** Invalid inputs to ParentSet : AncestorFound and Disconnected inputs */
  igstkAddTransitionMacro( ParentSet  , AncestorFound, ParentSet, InvalidRequest );
  igstkAddTransitionMacro( ParentSet  , Disconnected , ParentSet, InvalidRequest );

  /** Transitions from AttemptingComputeTransformToInInitialized */
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized, Disconnected, Initialized, ComputeTransformToDisconnected);
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized, AncestorFound, Initialized, ComputeTransformToAncestorFound);
  /** Invalid inputs to AttemptingComputeTransformToInitialized :
       NullParent, ThisParent, ValidParent, ParentCausesCycle, NullCoordinateReferenceSystem
       ThisCoordinateReferenceSystem, ValidCoordinateReferenceSystem */
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized, NullParent, AttemptingComputeTransformToInInitialized, InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized, ThisParent, AttemptingComputeTransformToInInitialized, InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized, ValidParent, AttemptingComputeTransformToInInitialized, InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized, ParentCausesCycle, AttemptingComputeTransformToInInitialized, InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized, NullCoordinateReferenceSystem, AttemptingComputeTransformToInInitialized, InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized, ThisCoordinateReferenceSystem, AttemptingComputeTransformToInInitialized, InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformToInInitialized, ValidCoordinateReferenceSystem, AttemptingComputeTransformToInInitialized, InvalidRequest );

  /** Transitions from AttemptingComputeTransformTo */
  igstkAddTransitionMacro( AttemptingComputeTransformTo, Disconnected  , ParentSet  , ComputeTransformToDisconnected );
  igstkAddTransitionMacro( AttemptingComputeTransformTo, AncestorFound , ParentSet  , ComputeTransformToAncestorFound );
  /** Invalid inputs to AttemptingComputeTransformTo :
       NullParent, ThisParent, ValidParent, ParentCausesCycle, NullCoordinateReferenceSystem
       ThisCoordinateReferenceSystem, ValidCoordinateReferenceSystem */
  igstkAddTransitionMacro( AttemptingComputeTransformTo, NullParent, AttemptingComputeTransformTo, InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformTo, ThisParent, AttemptingComputeTransformTo, InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformTo, ValidParent, AttemptingComputeTransformTo, InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformTo, ParentCausesCycle, AttemptingComputeTransformTo, InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformTo, NullCoordinateReferenceSystem, AttemptingComputeTransformTo, InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformTo, ThisCoordinateReferenceSystem, AttemptingComputeTransformTo, InvalidRequest );
  igstkAddTransitionMacro( AttemptingComputeTransformTo, ValidCoordinateReferenceSystem, AttemptingComputeTransformTo, InvalidRequest );

  m_ReportTiming = false;

  igstkSetInitialStateMacro( Initialized );
  m_StateMachine.SetReadyToRun();
} 

/** Destructor */
CoordinateReferenceSystem
::~CoordinateReferenceSystem()  
{
  // Break references
  m_Parent = NULL;
  m_RequestSetTransformAndParentParent = NULL;
  m_ComputeTransformToTarget = NULL;
  m_LowestCommonAncestor = NULL;

  if (this->m_ReportTiming == true)
    {
    std::cout << "============================================" << std::endl;
    std::cout << "Lowest common ancestor timer - ";
    std::cout << this->m_LowestCommonAncestorTimer.GetNumberOfStarts() 
              << " starts ";
    std::cout << this->m_LowestCommonAncestorTimer.GetNumberOfStops() 
              << " stops ";
    std::cout << this->m_LowestCommonAncestorTimer.GetMeanTime() * 1000.0 
              << " msec." << std::endl;

    std::cout << "Compute transform to timer - ";
    std::cout << this->m_ComputeTransformToTimer.GetNumberOfStarts() 
              << " starts ";
    std::cout << this->m_ComputeTransformToTimer.GetNumberOfStops() 
              << " stops ";
    std::cout << this->m_ComputeTransformToTimer.GetMeanTime() * 1000.0 
              << " msec." << std::endl;

    std::cout << "Request compute transform to timer - ";
    std::cout << this->m_RequestComputeTransformToTimer.GetNumberOfStarts() 
              << " starts ";
    std::cout << this->m_RequestComputeTransformToTimer.GetNumberOfStops() 
              << " stops ";
    std::cout << this->m_RequestComputeTransformToTimer.GetMeanTime() * 1000.0 
              << " msec." << std::endl;

    std::cout << "--------------------------------------------" << std::endl;
    }
}


/** Print object information */
void CoordinateReferenceSystem::PrintSelf( 
  std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Parent = " << this->m_Parent.GetPointer() << std::endl;
  os << indent << "Transform to parent = " 
     << this->m_TransformToParent << std::endl;
}

void CoordinateReferenceSystem
::RequestComputeTransformTo(const CoordinateReferenceSystem* target)
{
  this->m_RequestComputeTransformToTimer.Start();

  if (NULL == target)
    {
    igstkPushInputMacro( NullCoordinateReferenceSystem );
    m_StateMachine.ProcessInputs();
    this->m_RequestComputeTransformToTimer.Stop();
    return;
    }
  if (target == this)
    {
    igstkPushInputMacro( ThisCoordinateReferenceSystem );
    m_StateMachine.ProcessInputs();
    this->m_RequestComputeTransformToTimer.Stop();
    return;
    }

  this->m_ComputeTransformToTarget = target;

  igstkPushInputMacro( ValidCoordinateReferenceSystem );
  m_StateMachine.ProcessInputs();
  this->m_RequestComputeTransformToTimer.Stop();

  // Break the reference once we're done with it...
  this->m_ComputeTransformToTarget = NULL;

  return;
}

void CoordinateReferenceSystem
::ComputeTransformToNullTargetProcessing()
{
 // ERROR
 CoordinateReferenceSystemTransformToErrorResult payload;
 payload.Initialize( this, m_ComputeTransformToTarget );

 CoordinateReferenceSystemTransformToNullTargetEvent event;
 event.Set( payload );

 this->InvokeEvent( event );
}

void CoordinateReferenceSystem
::ComputeTransformToThisTargetProcessing()
{
 // Should return identity transform
 // What time limit?
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
  if (this->m_ComputeTransformToTarget.IsNull()) //( NULL == this->m_ComputeTransformToTarget )
    {
    igstkPushInputMacro( Disconnected );
    m_StateMachine.ProcessInputs();
    return;
    }

  if (this->m_ComputeTransformToTarget == this) //( this == this->m_ComputeTransformToTarget )
    {
    this->m_ComputedTransform = igstk::Transform();
    this->m_ComputedTransform.SetToIdentity(igstk::TimeStamp::GetLongestPossibleTime());
    igstkPushInputMacro( AncestorFound );
    m_StateMachine.ProcessInputs();
    return;
    }

  this->FindLowestCommonAncestor( this->m_ComputeTransformToTarget );
}

void CoordinateReferenceSystem
::ComputeTransformToDisconnectedProcessing()
{
 // ERROR
 CoordinateReferenceSystemTransformToErrorResult payload;
 payload.Initialize(this, this->m_ComputeTransformToTarget);

 CoordinateReferenceSystemTransformToDisconnectedEvent event;
 event.Set( payload );

 this->InvokeEvent ( event );
}

void CoordinateReferenceSystem
::ComputeTransformToAncestorFoundProcessing()
{
  this->m_ComputeTransformToTimer.Start();

  igstkLogMacro(DEBUG, this->GetName() << " and "
                        << m_ComputeTransformToTarget->GetName()
                        << " have lowest common ancestor : "
                        << this->m_LowestCommonAncestor->GetName()
                        << "\n" );

  Transform thisToAncestor   = 
            this->ComputeTransformTo( m_LowestCommonAncestor );

  Transform targetToAncestor = 
    m_ComputeTransformToTarget->ComputeTransformTo( m_LowestCommonAncestor );

  Transform result = Transform::TransformCompose(
                                              targetToAncestor.GetInverse(),
                                              thisToAncestor);

  this->m_ComputeTransformToTimer.Stop();

  // Create event
  CoordinateReferenceSystemTransformToResult payload;
  payload.Initialize(result, this, m_ComputeTransformToTarget);

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
    //
    // Check order of composition - switched order 10/30/07
    //
    Transform result = Transform::TransformCompose( 
      this->m_Parent->ComputeTransformTo(ancestor),
      this->m_TransformToParent);
    return result;
    }
}

void CoordinateReferenceSystem
::RequestSetTransformAndParent(Transform t, const CoordinateReferenceSystem* parent)
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
    // Do something smart -- i.e. push an error input...
    // 
    this->m_RequestSetTransformAndParentParent = parent;

    igstkPushInputMacro( ParentCausesCycle );
    m_StateMachine.ProcessInputs();

    // Break reference when we're done with it...
    this->m_RequestSetTransformAndParentParent = NULL;
    return;    
    }
  else
    {
    this->m_RequestSetTransformAndParentTransform = t;
    this->m_RequestSetTransformAndParentParent = parent;

    igstkPushInputMacro( ValidParent );
    m_StateMachine.ProcessInputs();

    // Break reference when we're done with it...
    this->m_RequestSetTransformAndParentParent = NULL;
    return;
    }
}

void CoordinateReferenceSystem
::SetTransformAndParentProcessing()
{
  this->m_Parent = this->m_RequestSetTransformAndParentParent;
  this->m_TransformToParent = this->m_RequestSetTransformAndParentTransform;
}

void CoordinateReferenceSystem
::SetTransformAndParentNullParentProcessing()
{
  // ERROR
  CoordinateReferenceSystemNullParentEvent event;
  this->InvokeEvent( event );
}

void CoordinateReferenceSystem
::SetTransformAndParentThisParentProcessing()
{
  // ERROR
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

  this->m_LowestCommonAncestorTimer.Start();

  CoordinateReferenceSystemConstPointer aSmart = this;
  CoordinateReferenceSystemConstPointer bSmart = B;

  if ( aSmart == bSmart )
    {
    this->m_LowestCommonAncestor = aSmart;
    this->m_LowestCommonAncestorTimer.Stop();
    igstkPushInputMacro( AncestorFound );
    m_StateMachine.ProcessInputs();
    // Break reference when we're done with it.
    this->m_LowestCommonAncestor = NULL;
    return;
    }
  if ( NULL == bSmart )
    {
    // Error 
    this->m_LowestCommonAncestorTimer.Stop();
    igstkPushInputMacro( Disconnected );
    m_StateMachine.ProcessInputs();
    return;
    }

  CoordinateReferenceSystemConstPointer aTemp; 
  CoordinateReferenceSystemConstPointer bTemp;
  CoordinateReferenceSystemConstPointer aTempPrev = NULL;
  CoordinateReferenceSystemConstPointer bTempPrev = NULL;

  for(bTemp = bSmart; 
      (bTemp != NULL && bTempPrev != bTemp); 
      bTemp = bTemp->m_Parent)
    {
    aTempPrev = NULL;
    for(aTemp = aSmart; 
        (aTemp != NULL && aTempPrev != aTemp); 
        aTemp = aTemp->m_Parent)
      {
      if (aTemp == bTemp)
        {
        this->m_LowestCommonAncestor = aTemp;
        this->m_LowestCommonAncestorTimer.Stop();
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

  // Error 
  this->m_LowestCommonAncestorTimer.Stop();
  igstkPushInputMacro( Disconnected );
  m_StateMachine.ProcessInputs();
  return;
}

bool
CoordinateReferenceSystem
::CanReach(const CoordinateReferenceSystem* target) const
{
  /** target is reachable since we're "target" */
  if (target == this)
    {
    return true;
    }
  /** Self loop at the top of the tree */
  else if (this->m_Parent == this)
    {
    return false;
    }
  /** If we're not "target", can our parent
      reach "target"? */
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
  event.Set( this->m_RequestSetTransformAndParentParent );

  this->InvokeEvent( event );
}

void 
CoordinateReferenceSystem
::InvalidRequestProcessing()
{

}

} // end namespace igstk

