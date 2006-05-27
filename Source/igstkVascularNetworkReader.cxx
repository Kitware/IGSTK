/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVascularNetworkReader.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkVascularNetworkReader.h"

namespace igstk
{ 

/** Constructor */
VascularNetworkReader::VascularNetworkReader():m_StateMachine(this)
{ 
  m_VascularNetwork = VascularNetworkType::New();

  igstkAddInputMacro( ValidVascularNetworkObject );
  igstkAddInputMacro( NullVascularNetworkObject  );

  igstkAddInputMacro( GetVascularNetwork );

  igstkAddStateMacro( NullVascularNetworkObject );
  igstkAddStateMacro( ValidVascularNetworkObject );

  igstkAddTransitionMacro( NullVascularNetworkObject, 
                           NullVascularNetworkObject, 
                           NullVascularNetworkObject,  No );

  igstkAddTransitionMacro( NullVascularNetworkObject,
                           GetVascularNetwork,
                           NullVascularNetworkObject,
                           No );

  igstkAddTransitionMacro( NullVascularNetworkObject,
                           ValidVascularNetworkObject,
                           ValidVascularNetworkObject,
                           No );

  igstkAddTransitionMacro( ValidVascularNetworkObject,
                           GetVascularNetwork,
                           ValidVascularNetworkObject,
                           ReportVascularNetwork );

  igstkSetInitialStateMacro( NullVascularNetworkObject );

  m_StateMachine.SetReadyToRun();

} 

/** Destructor */
VascularNetworkReader::~VascularNetworkReader()  
{
}

/** No Processing */
void VascularNetworkReader::NoProcessing()  
{
}


void VascularNetworkReader::RequestGetVascularNetwork()
{
  igstkLogMacro( DEBUG, 
       "igstk::VascularNetworkReader::RequestGetVascularNetwork called...\n");
  m_StateMachine.PushInput(m_GetVascularNetworkInput);
  m_StateMachine.ProcessInputs();
}

/** This function reports the vascular network */
void VascularNetworkReader::ReportVascularNetworkProcessing()
{
  VascularNetworkModifiedEvent  event;
  event.Set( this->m_VascularNetwork );
  this->InvokeEvent( event );
}

/** Read the spatialobject file */
void VascularNetworkReader::AttemptReadObjectProcessing()
{
  igstkLogMacro( DEBUG, "igstk::VascularNetworkReader::\
                        AttemptReadObject called...\n");

  Superclass::AttemptReadObjectProcessing();

  // Do the conversion
  GroupSpatialObjectType::Pointer m_GroupSpatialObject 
                                          = m_SpatialObjectReader->GetGroup();

  if(!m_GroupSpatialObject)
    {
    m_StateMachine.PushInput( m_NullVascularNetworkObjectInput );
    m_StateMachine.ProcessInputs();
    return;
    }

  GroupSpatialObjectType::ChildrenListType * children 
                                   = m_GroupSpatialObject->GetChildren(99999);
  GroupSpatialObjectType::ChildrenListType::const_iterator it 
                                                          = children->begin();

  while(it != children->end())
    {
    if(!strcmp((*it)->GetTypeName(),"VesselTubeSpatialObject"))
      {
      VesselTubeSpatialObjectType * vesselTube = 
          dynamic_cast< VesselTubeSpatialObjectType * >( it->GetPointer() );
      
      if(vesselTube)
        {
        // Create a new VascularNetworkObject
        VesselObjectType::Pointer vessel = VesselObjectType::New();
        vessel->SetVesselSpatialObject(vesselTube);

        // For the moment no hierarchy
        m_VascularNetwork->RequestAddObject(vessel);
        }
      }
    it++;
    }
  delete children;

  m_StateMachine.PushInput( m_ValidVascularNetworkObjectInput );
  m_StateMachine.ProcessInputs();
}


/** Print Self function */
void VascularNetworkReader
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << "VascularNetwork = " << m_VascularNetwork.GetPointer() << std::endl;
}

} // end namespace igstk
