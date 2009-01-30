/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateSystemDelegator.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkCoordinateSystemDelegator.h"

/* Methods for new coordinate system classes that support the creation of scene
 * graphs. */

namespace igstk
{

CoordinateSystemDelegator
::CoordinateSystemDelegator() : m_StateMachine(this)
{
  m_CoordinateSystemObserver = CoordinateSystemObserverType::New();
  m_CoordinateSystemObserver->SetCallbackFunction(
    this, &Self::ObserverCallback);

  m_CoordinateSystem = CoordinateSystem::New();
 
  /** Add observer for coordinate reference system events */
  m_CoordinateSystem->AddObserver( 
    CoordinateSystemTransformToNullTargetEvent()
    , m_CoordinateSystemObserver );

  m_CoordinateSystem->AddObserver( 
    CoordinateSystemTransformToDisconnectedEvent()
    , m_CoordinateSystemObserver );

  m_CoordinateSystem->AddObserver( 
    CoordinateSystemNullParentEvent()
    , m_CoordinateSystemObserver );

  m_CoordinateSystem->AddObserver( 
    CoordinateSystemThisParentEvent()
    , m_CoordinateSystemObserver );

  m_CoordinateSystem->AddObserver( 
    CoordinateSystemParentCycleEvent()
    , m_CoordinateSystemObserver );

  m_CoordinateSystem->AddObserver( 
    CoordinateSystemTransformToEvent()
    , m_CoordinateSystemObserver );

  m_CoordinateSystem->AddObserver( 
    CoordinateSystemSetTransformEvent()
    , m_CoordinateSystemObserver );
  
  std::stringstream tempStream;
  tempStream << this->GetNameOfClass() << " 0x";
  tempStream << static_cast<void*>(this);
  std::string name = tempStream.str();
  m_CoordinateSystem->SetName( name.c_str() );
  m_CoordinateSystem->SetType( name.c_str() );

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

CoordinateSystemDelegator::
~CoordinateSystemDelegator()
{

}

void 
CoordinateSystemDelegator::
ObserverCallback(const ::itk::EventObject & eventvar)
{
  /** Re-invoke the event */
  this->InvokeEvent( eventvar );
}

const CoordinateSystem * 
CoordinateSystemDelegator::GetCoordinateSystem() const
{
  return m_CoordinateSystem;
}

void
CoordinateSystemDelegator
::NullTargetProcessing()
{
  igstkLogMacro( WARNING, "Null target!" );  
}

void
CoordinateSystemDelegator
::NullParentProcessing()
{
  igstkLogMacro( WARNING, "Null parent!" );
}

void 
CoordinateSystemDelegator
::RequestGetTransformToParent()
{
  /** Handle returns with event observer */
  this->m_CoordinateSystem->RequestGetTransformToParent();
}

// Print object information
void 
CoordinateSystemDelegator
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Coordinate system : " 
     << m_CoordinateSystem << std::endl;
  os << indent << "Coordinate system observer : " 
     << m_CoordinateSystemObserver << std::endl;
}

void 
CoordinateSystemDelegator
::SetName (const char* _arg)
{ 
  this->m_CoordinateSystem->SetName( _arg );
} 
  
void
CoordinateSystemDelegator
::SetName (const std::string & _arg) 
{
  this->SetName( _arg.c_str() );
}

const char* 
CoordinateSystemDelegator
::GetName () const
{
  return this->m_CoordinateSystem->GetName();
}

void 
CoordinateSystemDelegator
::SetType (const char* _arg)
{ 
  this->m_CoordinateSystem->SetType( _arg );
} 
  
void
CoordinateSystemDelegator
::SetType (const std::string & _arg) 
{
  this->SetType( _arg.c_str() );
}

const char* 
CoordinateSystemDelegator
::GetType () const
{
  return this->m_CoordinateSystem->GetType();
}

bool
CoordinateSystemDelegator
::IsCoordinateSystem( const CoordinateSystemType* inCS) const
{
  // Pointer comparison -- we're not checking whether the 
  // coordinate systems are equivalent, but whether the
  // instances of the coordinate system are the same.
  return (inCS == this->m_CoordinateSystem); 
}


} // end igstk namespace
