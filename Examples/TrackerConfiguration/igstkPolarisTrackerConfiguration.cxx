/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisTrackerConfiguration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkPolarisTrackerConfiguration.h"

namespace igstk
{
const double PolarisVicraTrackerConfiguration::MAXIMAL_REFERESH_RATE = 20;
const double PolarisSpectraTrackerConfiguration::MAXIMAL_REFERESH_RATE = 60;
const double PolarisHybridTrackerConfiguration::MAXIMAL_REFERESH_RATE = 60;
const unsigned PolarisHybridTrackerConfiguration::MAXIMAL_PORT_NUMBER = 11; 


void 
PolarisWirelessTrackerConfiguration::InternalAddTool( 
  const TrackerToolConfiguration *tool, bool isReference )
{
  AddToolFailureEvent fe;
  const PolarisWirelessToolConfiguration *wirelessTool = 
    dynamic_cast<const PolarisWirelessToolConfiguration *>( tool );

  if( wirelessTool == NULL )
    {
    fe.Set( "Given tool configuration type not compatible with tracker type." );
    this->InvokeEvent( fe );
    return;
    }
  if( wirelessTool->GetSROMFile().empty() )
    {
    fe.Set( "SROM file not specified for wireless tool." );
    this->InvokeEvent( fe );
    return;
    }
  if( !isReference )
    {
    this->m_TrackerToolList.insert(std::pair<std::string, 
                                   TrackerToolConfiguration *>
      (wirelessTool->GetToolName(), 
       new PolarisWirelessToolConfiguration( *wirelessTool )) );
    }
  else
    {
    delete this->m_ReferenceTool;
    this->m_ReferenceTool = new PolarisWirelessToolConfiguration( 
                                                              *wirelessTool );
    }
  this->InvokeEvent( AddToolSuccessEvent() );
}

PolarisVicraTrackerConfiguration::PolarisVicraTrackerConfiguration()
{  
  this->m_Frequency = MAXIMAL_REFERESH_RATE;
}


PolarisVicraTrackerConfiguration::~PolarisVicraTrackerConfiguration()
{

}


double
PolarisVicraTrackerConfiguration::GetMaximalRefreshRate()
{
  return this->MAXIMAL_REFERESH_RATE;
}


PolarisSpectraTrackerConfiguration::PolarisSpectraTrackerConfiguration()
{  
  this->m_Frequency = MAXIMAL_REFERESH_RATE;
}


PolarisSpectraTrackerConfiguration::~PolarisSpectraTrackerConfiguration()
{

}


double
PolarisSpectraTrackerConfiguration::GetMaximalRefreshRate()
{
  return this->MAXIMAL_REFERESH_RATE;
}


PolarisHybridTrackerConfiguration::PolarisHybridTrackerConfiguration()
{  
  this->m_Frequency = MAXIMAL_REFERESH_RATE;
}


PolarisHybridTrackerConfiguration::~PolarisHybridTrackerConfiguration()
{

}


double
PolarisHybridTrackerConfiguration::GetMaximalRefreshRate()
{
  return this->MAXIMAL_REFERESH_RATE;
}


void 
PolarisHybridTrackerConfiguration::InternalAddTool( const
  TrackerToolConfiguration *tool, bool isReference )
{
  AddToolFailureEvent fe;
  const PolarisWirelessToolConfiguration *wirelessTool = 
    dynamic_cast<const PolarisWirelessToolConfiguration *>( tool );
  const PolarisWiredToolConfiguration *wiredTool = 
    dynamic_cast<const PolarisWiredToolConfiguration *>( tool );
  

  if( wirelessTool == NULL  && wiredTool == NULL )
    {
    fe.Set( "Given tool configuration type not compatible with tracker type." );
    this->InvokeEvent( fe );
    return;
    }
  if( wirelessTool ) 
    {
    if( wirelessTool->GetSROMFile().empty() )
      {
      fe.Set( "SROM file not specified for wireless tool." );
      this->InvokeEvent( fe );
      return;
      }
    }
  else   //we have a wired tool
    {
    unsigned int newPortNumber = wiredTool->GetPortNumber();
    if( newPortNumber > this->MAXIMAL_PORT_NUMBER )
      {
      fe.Set( "Specified physical port number is invalid." );
      this->InvokeEvent( fe );
      return;
      }
    std::map<std::string, TrackerToolConfiguration *>::const_iterator it, end;
    it = this->m_TrackerToolList.begin();
    end = this->m_TrackerToolList.end();

    for(; it!=end; it++ )
      {
      const PolarisWiredToolConfiguration *currentTool = 
        dynamic_cast<const PolarisWiredToolConfiguration *>( it->second );
                   //rely on short circuit evaluation so that the second term
                   //is not evaluated if currentTool==NULL
      if( currentTool != NULL && currentTool->GetPortNumber() == newPortNumber )
        {
        fe.Set( "Multiple tools with same port are not allowed.");
        this->InvokeEvent( fe );
        return;
        }
      }
    }
  //copy the tool and add it as a standard or dynamic reference tool
  TrackerToolConfiguration *newTool;
  if(wirelessTool)
    {
    newTool = new PolarisWirelessToolConfiguration( *wirelessTool );
    }
  else
    {
    newTool = new PolarisWiredToolConfiguration( *wiredTool );
    }
  
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


PolarisWiredToolConfiguration::PolarisWiredToolConfiguration() :
                                                              m_PortNumber( 1 ),
                                                              m_SROMFile( "" )
{
}


PolarisWiredToolConfiguration::PolarisWiredToolConfiguration( const 
  PolarisWiredToolConfiguration &other ) : TrackerToolConfiguration( other )
{
  this->m_PortNumber = other.m_PortNumber;
  this->m_SROMFile = other.m_SROMFile;
}

PolarisWiredToolConfiguration::~PolarisWiredToolConfiguration()
{
}

std::string 
PolarisWiredToolConfiguration::GetToolTypeAsString()
{
  return "PolarisWiredToolConfiguration";
}

PolarisWirelessToolConfiguration::PolarisWirelessToolConfiguration() :
                                                              m_SROMFile( "" )
{
}

PolarisWirelessToolConfiguration::PolarisWirelessToolConfiguration( const 
PolarisWirelessToolConfiguration &other ) : TrackerToolConfiguration( other )
{
  this->m_SROMFile = other.m_SROMFile;
}

PolarisWirelessToolConfiguration::~PolarisWirelessToolConfiguration()
{
}

std::string 
PolarisWirelessToolConfiguration::GetToolTypeAsString()
{
  return "PolarisWirelessToolConfiguration";
}


} // end of name space
