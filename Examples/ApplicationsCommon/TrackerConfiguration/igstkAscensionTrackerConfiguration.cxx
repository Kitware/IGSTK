/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAscensionTrackerConfiguration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkAscensionTrackerConfiguration.h"

namespace igstk
{
               //maximal refresh rate for the Ascension system with short/mid
               //range transmitters
const double AscensionTrackerConfiguration::MAXIMAL_REFERESH_RATE = 115200;

               //maximal port number for the Ascension system
const unsigned AscensionTrackerConfiguration::MAXIMAL_PORT_NUMBER = 4;


AscensionTrackerConfiguration::AscensionTrackerConfiguration()  
{
  this->m_Frequency = this->MAXIMAL_REFERESH_RATE;
}


AscensionTrackerConfiguration::~AscensionTrackerConfiguration()
{
}


double
AscensionTrackerConfiguration::GetMaximalRefreshRate()
{
  return this->MAXIMAL_REFERESH_RATE;
}


void 
AscensionTrackerConfiguration::InternalAddTool( const
  TrackerToolConfiguration *tool, bool isReference )
{
  AddToolFailureEvent fe;
  const AscensionToolConfiguration *wiredTool = 
    dynamic_cast<const AscensionToolConfiguration *>( tool );
  

  if( wiredTool == NULL )
  {
    fe.Set( "Given tool configuration type not compatible with tracker type." );
    this->InvokeEvent( fe );
    return;
  }
 
  if( (wiredTool->GetPortNumber() > this->MAXIMAL_PORT_NUMBER) ||
      (wiredTool->GetPortNumber() < 1) )
  {
    fe.Set( "Specified physical port number is invalid." );
    this->InvokeEvent( fe );
    return;
  }
  
         //copy the tool and add it as a standard or dynamic reference tool
  AscensionToolConfiguration *newTool = 
    new AscensionToolConfiguration( *wiredTool );
  
  if( !isReference )
  {
    this->m_TrackerToolList.insert(std::pair<std::string, 
                                   TrackerToolConfiguration *>
      (newTool->GetToolName(), newTool) );
  }
  else
  {
    delete this->m_ReferenceTool;
    this->m_ReferenceTool = newTool; 
  }
  this->InvokeEvent( AddToolSuccessEvent() );
}


AscensionToolConfiguration::AscensionToolConfiguration() : m_PortNumber( 1 )
{
}


AscensionToolConfiguration::AscensionToolConfiguration( const 
  AscensionToolConfiguration &other ) : TrackerToolConfiguration( other ) 
{
  this->m_PortNumber = other.m_PortNumber;
}


AscensionToolConfiguration::~AscensionToolConfiguration()
{
}


std::string 
AscensionToolConfiguration::GetToolTypeAsString()
{
  return "AscensionToolConfiguration";
}

} // end of name space
