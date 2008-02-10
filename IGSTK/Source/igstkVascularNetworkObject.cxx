/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVascularNetworkObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkVascularNetworkObject.h"

namespace igstk
{ 

/** Constructor */
VascularNetworkObject::VascularNetworkObject():m_StateMachine(this)
{
  // Observe the superclass
  this->ObserveVesselReceivedInput( this );
  this->ObserveSpatialObjectNotAvailableEvent( this );

  igstkAddInputMacro( GetVessel );
  igstkAddInputMacro( VesselReceived );
  igstkAddInputMacro( VesselNotFound );

  igstkAddStateMacro( Init );
  igstkAddStateMacro( AttemtingToGetVessel );

  igstkAddTransitionMacro( Init, 
                           GetVessel, 
                           AttemtingToGetVessel, SearchForVessel );

  igstkAddTransitionMacro( AttemtingToGetVessel, 
                           VesselReceived, 
                           Init, ReportVessel );

  igstkAddTransitionMacro( AttemtingToGetVessel, 
                           VesselNotFound, 
                           Init, ReportVesselNotFound );

  igstkSetInitialStateMacro( Init );

  m_StateMachine.SetReadyToRun();

  m_VesselId = 0;
}

/** This function reports the vessel */
void VascularNetworkObject::SearchForVesselProcessing()
{
  // This request goes to the parent, the GroupObject
  // and it will trigger a callback in the Observer.
  this->Superclass::RequestGetChild( m_VesselId );
}

/** This function reports the vessel */
void VascularNetworkObject::ReportVesselProcessing()
{
  VesselObjectType * vessel = 
    static_cast< VesselObjectType * >( m_VesselReceivedInputToBeSet );

  VesselObjectModifiedEvent  event;
  event.Set( const_cast<VesselObjectType*>(vessel)  );
  this->InvokeEvent( event );
}


/** Destructor */
VascularNetworkObject::~VascularNetworkObject()  
{
}

/** Request to get a vessel */
void VascularNetworkObject::RequestGetVessel( unsigned long vesselId )
{
  igstkLogMacro( DEBUG, 
              "igstk::VascularNetworkObject::RequestGetVessel called...\n");
  
  m_VesselId = vesselId;
  igstkPushInputMacro( GetVessel );
  m_StateMachine.ProcessInputs();
}

/** Return the number of Vessels associated with this network */
unsigned long VascularNetworkObject
::GetNumberOfVessels() const
{
  return this->Superclass::GetNumberOfChildren();
}

/** Request adding a child to the group */
void VascularNetworkObject::RequestAddVessel( 
  const Transform & transform, VesselObjectType * vessel )
{
  this->Superclass::RequestAddChild( transform, vessel );
}

/** Report that an invalid or suspicious operation has been requested.
 * This may mean that an error condition has arised in one of the
 * componenta that interact with this SpatialObject. */
void VascularNetworkObject::ReportInvalidRequestProcessing()
{
  igstkLogMacro( WARNING, "Invalid request made to the State Machine" );
}

/** Print object information */
void VascularNetworkObject
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

/** This function reports the vessel */
void VascularNetworkObject::ReportVesselNotFoundProcessing()
{
  VesselObjectNotAvailableEvent  event;
  this->InvokeEvent( event );
}

} // end namespace igstk
