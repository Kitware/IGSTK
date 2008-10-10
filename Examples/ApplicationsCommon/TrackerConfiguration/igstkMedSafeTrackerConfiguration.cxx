/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMedSafeTrackerConfiguration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkMedSafeTrackerConfiguration.h"

namespace igstk
{
               //maximal refresh rate for the 3D Guidance MedSafe system possible
               //with the short/mid range transmitters
const double MedSafeTrackerConfiguration::MAXIMAL_REFERESH_RATE = 350;

const unsigned MedSafeTrackerConfiguration::MAXIMAL_PORT_NUMBER = 4;

MedSafeTrackerConfiguration::MedSafeTrackerConfiguration()  
{
  this->m_Frequency = this->MAXIMAL_REFERESH_RATE;
}


MedSafeTrackerConfiguration::~MedSafeTrackerConfiguration()
{
}


double
MedSafeTrackerConfiguration::GetMaximalRefreshRate()
{
  return this->MAXIMAL_REFERESH_RATE;
}


void 
MedSafeTrackerConfiguration::InternalAddTool( 
  const TrackerToolConfiguration *tool, bool isReference )
{
  AddToolFailureEvent fe;
  const MedSafeToolConfiguration *wiredTool = 
    dynamic_cast<const MedSafeToolConfiguration *>( tool );
  

  if( wiredTool == NULL )
  {
    fe.Set( "Given tool configuration type not compatible with tracker type." );
    this->InvokeEvent( fe );
    return;
  }
   //check for valid bird name???

  if( wiredTool->GetPortNumber()==0 ||
      wiredTool->GetPortNumber()> this->MAXIMAL_PORT_NUMBER )
  {
    fe.Set( "Specified physical port number is invalid." );
    this->InvokeEvent( fe );
    return;
  }
  
        //copy the tool and add it as a standard or dynamic reference tool
  MedSafeToolConfiguration *newTool = 
    new MedSafeToolConfiguration( *wiredTool );
  
  if( !isReference )
  {
    this->m_TrackerToolList.insert(std::pair<std::string, TrackerToolConfiguration *>
      (newTool->GetToolName(), newTool) );
  }
  else
  {
    delete this->m_ReferenceTool;
    this->m_ReferenceTool = newTool; 
  }
  this->InvokeEvent( AddToolSuccessEvent() );
}


MedSafeToolConfiguration::MedSafeToolConfiguration() : m_PortNumber( 1 )
{
}                                                                


MedSafeToolConfiguration::MedSafeToolConfiguration( const 
  MedSafeToolConfiguration &other ) : TrackerToolConfiguration( other ) 
{
  this->m_PortNumber = other.m_PortNumber;
}                                                                


MedSafeToolConfiguration::~MedSafeToolConfiguration()
{
}


std::string 
MedSafeToolConfiguration::GetToolTypeAsString()
{
  return "MedSafeToolConfiguration";
}

} // end of name space
