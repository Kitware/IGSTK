/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImagerConfiguration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkImagerConfiguration.h"

namespace igstk
{
  
  
ImagerConfiguration::ImagerConfiguration() : m_ReferenceTool( NULL )
{
}


ImagerConfiguration::~ImagerConfiguration()
{
  std::vector< ImagerToolConfiguration * >::iterator it;
  std::vector< ImagerToolConfiguration * >::iterator endIt = 
    this->m_ImagerToolList.end();

  for(it=this->m_ImagerToolList.begin(); it!=endIt; it++)
  {    
    delete (*it);
  }
  this->m_ImagerToolList.clear();
  if ( m_ReferenceTool )
  {
      delete this->m_ReferenceTool;
  }
}


void
ImagerConfiguration::RequestSetFrequency( double frequency )
{
  if( frequency<=0 || frequency>GetMaximalRefreshRate() )
  {
    std::ostringstream msg;
    msg.str("");
    msg<<"Invalid tracker frequency specified.";
    msg<< "Valid values are in [0,"<<GetMaximalRefreshRate()<<"].";

    FrequencySetFailureEvent evt;
    evt.Set( msg.str() );
    this->InvokeEvent( evt );
  }
  else
  {
    this->m_Frequency = frequency;
     this->InvokeEvent( FrequencySetSuccessEvent() );
  }
}


void 
ImagerConfiguration::RequestAddTool( const ImagerToolConfiguration *tool )
{
  InternalAddTool( tool, false );
}



ImagerToolConfiguration::ImagerToolConfiguration() 
{
 
};


ImagerToolConfiguration::ImagerToolConfiguration(const 
  ImagerToolConfiguration &other)
{
  this->m_ToolUniqueIdentifier = other.m_ToolUniqueIdentifier;
  this->m_FrameDimensions[0] = other.m_FrameDimensions[0];
  this->m_FrameDimensions[1] = other.m_FrameDimensions[1];
  this->m_FrameDimensions[2] = other.m_FrameDimensions[2];
  this->m_PixelDepth = other.m_PixelDepth;
}

ImagerToolConfiguration::~ImagerToolConfiguration()
{
}

void 
ImagerToolConfiguration::GetFrameDimensions(unsigned int* dims) const
{
  dims[0] = m_FrameDimensions[0];
  dims[1] = m_FrameDimensions[1];
  dims[2] = m_FrameDimensions[2];
}

void 
ImagerToolConfiguration::SetFrameDimensions(unsigned int* dims)
{
  this->m_FrameDimensions[0] = dims[0];
  this->m_FrameDimensions[1] = dims[1];
  this->m_FrameDimensions[2] = dims[2];
}

void  
ImagerToolConfiguration::SetPixelDepth (unsigned int value)
{
  m_PixelDepth = value;
}
  
unsigned int 
ImagerToolConfiguration::GetPixelDepth ( void ) const
{
  return this->m_PixelDepth;
}

SocketCommunicatingImagerConfiguration::SocketCommunicatingImagerConfiguration()
{
  m_SocketPort = 0;
}


SocketCommunicatingImagerConfiguration::~SocketCommunicatingImagerConfiguration()
{
}


} // end of name space
