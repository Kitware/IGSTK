/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraTrackerConfiguration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkAuroraTrackerConfiguration.h"

namespace igstk
{
const double AuroraTrackerConfiguration::MAXIMAL_REFERESH_RATE = 40;
const unsigned AuroraTrackerConfiguration::MAXIMAL_PORT_NUMBER = 4;
const unsigned AuroraTrackerConfiguration::MAXIMAL_CHANNEL_NUMBER = 1;

AuroraTrackerConfiguration::AuroraTrackerConfiguration()
{
  this->m_Frequency = this->MAXIMAL_REFERESH_RATE;
}


AuroraTrackerConfiguration::~AuroraTrackerConfiguration()
{

}

double
AuroraTrackerConfiguration::GetMaximalRefreshRate()
{
  return this->MAXIMAL_REFERESH_RATE;
}


void 
AuroraTrackerConfiguration::InternalAddTool( 
  const TrackerToolConfiguration *tool, bool isReference )
{
  AddToolFailureEvent fe;
  const AuroraToolConfiguration *wiredTool = 
    dynamic_cast<const AuroraToolConfiguration *>( tool );
  

  unsigned int newPortNumber = wiredTool->GetPortNumber();
  unsigned int newChannelNumber = wiredTool->GetChannelNumber();

  if( wiredTool == NULL )
    {
    fe.Set( "Given tool configuration type not compatible with tracker type." );
    this->InvokeEvent( fe );
    return;
    }
  if( newPortNumber > this->MAXIMAL_PORT_NUMBER )
    {
    fe.Set( "Specified physical port number is invalid." );
    this->InvokeEvent( fe );
    return;
    }
  if( newChannelNumber > this->MAXIMAL_CHANNEL_NUMBER )
    {
    fe.Set( "Specified channel number is invalid." );
    this->InvokeEvent( fe );
    return;
    }

  std::map<std::string, TrackerToolConfiguration *>::const_iterator it, end;
  it = this->m_TrackerToolList.begin();
  end = this->m_TrackerToolList.end();
  for(; it!=end; it++ )
    {
    const AuroraToolConfiguration *currentTool = 
      static_cast<const AuroraToolConfiguration *>( it->second );
    if( currentTool->GetPortNumber() == newPortNumber &&
      currentTool->GetChannelNumber() == newChannelNumber )
      {
      fe.Set( "Multiple tools with same port and channel are not allowed.");
      this->InvokeEvent( fe );
      return;
      }
    }
  //copy the tool and add it as a standard or dynamic reference tool
  AuroraToolConfiguration *newTool = new AuroraToolConfiguration( *wiredTool );
  
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


AuroraToolConfiguration::AuroraToolConfiguration()
{
  m_PortNumber = 1;
  m_ChannelNumber = 0;
  m_SROMFile = "";
  m_Is5DOF = false;
}


AuroraToolConfiguration::AuroraToolConfiguration( const 
  AuroraToolConfiguration &other ) : TrackerToolConfiguration( other )
{
  this->m_PortNumber = other.m_PortNumber;
  this->m_ChannelNumber = other.m_ChannelNumber;
  this->m_SROMFile = other.m_SROMFile;
  this->m_Is5DOF = other.m_Is5DOF;
}


AuroraToolConfiguration::~AuroraToolConfiguration()
{
}


void 
AuroraToolConfiguration::SetChannelNumber( unsigned channelNumber )
{
  this->m_ChannelNumber = channelNumber;
  this->m_Is5DOF = true;
}


std::string 
AuroraToolConfiguration::GetToolTypeAsString()
{
  return "AuroraToolConfiguration";
}

} // end of name space
