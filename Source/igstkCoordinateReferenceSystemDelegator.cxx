/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateReferenceSystemDelegator.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkCoordinateReferenceSystemDelegator.h"

/* Methods for new coordinate system classes that support the creation of scene
 * graphs. */

namespace igstk
{

CoordinateReferenceSystemDelegator
::CoordinateReferenceSystemDelegator() : m_StateMachine(this)
{
  m_CoordinateReferenceSystemObserver = CoordinateSystemObserverType::New();
  m_CoordinateReferenceSystemObserver->SetCallbackFunction(
    this, &Self::ObserverCallback);

  m_CoordinateReferenceSystem = CoordinateReferenceSystem::New();
 
  /** Add observer for coordinate reference system events */
  m_CoordinateReferenceSystem->AddObserver( 
    CoordinateReferenceSystemTransformToNullTargetEvent()
    , m_CoordinateReferenceSystemObserver );

  m_CoordinateReferenceSystem->AddObserver( 
    CoordinateReferenceSystemTransformToDisconnectedEvent()
    , m_CoordinateReferenceSystemObserver );

  m_CoordinateReferenceSystem->AddObserver( 
    CoordinateReferenceSystemNullParentEvent()
    , m_CoordinateReferenceSystemObserver );

  m_CoordinateReferenceSystem->AddObserver( 
    CoordinateReferenceSystemThisParentEvent()
    , m_CoordinateReferenceSystemObserver );

  m_CoordinateReferenceSystem->AddObserver( 
    CoordinateReferenceSystemParentCycleEvent()
    , m_CoordinateReferenceSystemObserver );

  m_CoordinateReferenceSystem->AddObserver( 
    CoordinateReferenceSystemTransformToEvent()
    , m_CoordinateReferenceSystemObserver );

  std::stringstream tempStream;
  tempStream << this->GetNameOfClass() << " 0x";
  tempStream << static_cast<void*>(this);
  std::string name = tempStream.str();
  m_CoordinateReferenceSystem->SetName( name.c_str() );

  igstkAddStateMacro( Idle );

  igstkAddInputMacro( NullParent );
  igstkAddInputMacro( NullTarget );

  igstkAddTransitionMacro( Idle, NullParent,
                           Idle, NullParent );
  igstkAddTransitionMacro( Idle, NullTarget,
                           Idle, NullTarget );

  igstkSetInitialStateMacro( Idle );
  m_StateMachine.SetReadyToRun();
}

CoordinateReferenceSystemDelegator::
~CoordinateReferenceSystemDelegator()
{

}

void 
CoordinateReferenceSystemDelegator::
ObserverCallback(const ::itk::EventObject & eventvar)
{
  /** Re-invoke the event */
  this->InvokeEvent( eventvar );
}

const CoordinateReferenceSystem * 
CoordinateReferenceSystemDelegator::GetCoordinateReferenceSystem() const
{
  return m_CoordinateReferenceSystem;
}

void
CoordinateReferenceSystemDelegator
::NullTargetProcessing()
{
  igstkLogMacro( WARNING, "Null target!" );  
}

void
CoordinateReferenceSystemDelegator
::NullParentProcessing()
{
  igstkLogMacro( WARNING, "Null parent!" );
}

void 
CoordinateReferenceSystemDelegator
::RequestGetTransformToParent()
{
  /** Handle returns with event observer */
  this->m_CoordinateReferenceSystem->RequestGetTransformToParent();
}

// Print object information
void 
CoordinateReferenceSystemDelegator
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Coordinate system : " 
     << m_CoordinateReferenceSystem << std::endl;
  os << indent << "Coordinate system observer : " 
     << m_CoordinateReferenceSystemObserver << std::endl;
}

void 
CoordinateReferenceSystemDelegator
::SetName (const char* _arg)
{ 
  this->m_CoordinateReferenceSystem->SetName( _arg );
} 
  
void
CoordinateReferenceSystemDelegator
::SetName (const std::string & _arg) 
{
  this->SetName( _arg.c_str() );
}

const char* 
CoordinateReferenceSystemDelegator
::GetName () const
{
  return this->m_CoordinateReferenceSystem->GetName();
}

bool
CoordinateReferenceSystemDelegator
::IsCoordinateSystem( const CoordinateSystemType* inCS) const
{
  // Pointer comparison -- we're not checking whether the 
  // coordinate systems are equivalent, but whether the
  // instances of the coordinate system are the same.
  return (inCS == this->m_CoordinateReferenceSystem); 
}


} // end igstk namespace
