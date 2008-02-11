/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkGroupObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkGroupObject.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
GroupObject::GroupObject():m_StateMachine(this)
{
  // Create the Group Spatial Object
  m_GroupSpatialObject = GroupSpatialObjectType::New();

  this->RequestSetInternalSpatialObject( m_GroupSpatialObject );

  igstkAddInputMacro( AddChildValid );
  igstkAddInputMacro( AddChildInvalid );
  igstkAddInputMacro( GetChildValid );
  igstkAddInputMacro( GetChildInvalid );

  igstkAddStateMacro( EmptyGroup );
  igstkAddStateMacro( NonEmptyGroup );

  igstkAddTransitionMacro( EmptyGroup, 
                           AddChildValid, 
                           NonEmptyGroup,  AddChild );

  igstkAddTransitionMacro( EmptyGroup, 
                           AddChildInvalid, 
                           EmptyGroup,  ReportInvalidRequest );

  igstkAddTransitionMacro( EmptyGroup, 
                           GetChildValid, 
                           EmptyGroup,  ReportNoChildAvailable );

  igstkAddTransitionMacro( EmptyGroup, 
                           GetChildInvalid,
                           EmptyGroup,  ReportNoChildAvailable );

  igstkAddTransitionMacro( NonEmptyGroup, 
                           AddChildValid, 
                           NonEmptyGroup,  AddChild );

  igstkAddTransitionMacro( NonEmptyGroup, 
                           AddChildInvalid, 
                           NonEmptyGroup,  ReportInvalidRequest );

  igstkAddTransitionMacro( NonEmptyGroup, 
                           GetChildValid, 
                           NonEmptyGroup,  GetChild );

  igstkAddTransitionMacro( NonEmptyGroup, 
                           GetChildInvalid, 
                           NonEmptyGroup,  ReportNoChildAvailable );

  igstkSetInitialStateMacro( EmptyGroup );

  m_StateMachine.SetReadyToRun();

} 

/** Destructor */
GroupObject::~GroupObject()  
{
}

/** Return the number of objects in the group */
unsigned long GroupObject::GetNumberOfChildren() const
{
  return m_ChildrenArray.size();
}

/** Request adding a child to the group */
void GroupObject::RequestAddChild( 
  const Transform & transform, SpatialObject * child )
{
  m_ChildToAdd = child;
  m_TransformToAdd = transform;
  if( m_ChildToAdd == NULL )
    {
    igstkPushInputMacro( AddChildInvalid );
    m_StateMachine.ProcessInputs();
    return;
    }
  ChildrenContainerType::const_iterator childItr = m_ChildrenArray.begin();
  while( childItr != m_ChildrenArray.end() )
    {
    if( childItr->GetPointer() == m_ChildToAdd )
      {
      igstkPushInputMacro( AddChildInvalid );
      m_StateMachine.ProcessInputs();
      return;
      }
    ++childItr;
    }

  igstkPushInputMacro( AddChildValid );
  m_StateMachine.ProcessInputs();
}

/** Request to add a child to the group */
void GroupObject::RequestGetChild( unsigned long childID )
{
  m_ChildIdToGet = childID;
  if( childID >= m_ChildrenArray.size() )
    {
    igstkPushInputMacro( GetChildInvalid );
    }
  else
    {
    igstkPushInputMacro( GetChildValid );
    }
  m_StateMachine.ProcessInputs();
}

/** Add a child to the group */
void GroupObject::AddChildProcessing()
{
  m_ChildrenArray.push_back( m_ChildToAdd );
  m_ChildToAdd->RequestSetTransformAndParent( m_TransformToAdd, this );
}

/** Return a child from the group */
void GroupObject::GetChildProcessing()
{
  SpatialObjectModifiedEvent event;
  event.Set( m_ChildrenArray[ m_ChildIdToGet ] );
  this->InvokeEvent( event );
}

/** Return a child from the group */
void GroupObject::ReportNoChildAvailableProcessing()
{
  SpatialObjectNotAvailableEvent event;
  this->InvokeEvent( event );
}

/** Null operation for a State Machine transition */
void GroupObject::NoProcessing()
{
}

/** Report that an invalid or suspicious operation has been requested.
 * This may mean that an error condition has arisen in one of the
 * components that interact with this SpatialObject. */
void
GroupObject::ReportInvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "Invalid request made to the State Machine" );
}

/** Print object information */
void GroupObject::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  if( this->m_GroupSpatialObject )
    {
    os << indent << this->m_GroupSpatialObject << std::endl;
    }
}


} // end namespace igstk
