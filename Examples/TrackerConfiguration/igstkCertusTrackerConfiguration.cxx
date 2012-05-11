/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkCertusTrackerConfiguration.cxx,v $
  Language:  C++
  Date:      $Date: 2012-01-27 18:15:04 $
  Version:   $Revision: 1.1 $

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkCertusTrackerConfiguration.h"

namespace igstk
{                  
const double CertusTrackerConfiguration::MAXIMAL_REFERESH_RATE = 250;


CertusTrackerConfiguration::CertusTrackerConfiguration() :
m_SetupFile(""),
m_NumberOfRigidBodies("")
{
  this->m_Frequency = this->MAXIMAL_REFERESH_RATE;
}


CertusTrackerConfiguration::~CertusTrackerConfiguration()
{
}

double
CertusTrackerConfiguration::GetMaximalRefreshRate()
{
  return this->MAXIMAL_REFERESH_RATE;
}

void 
CertusTrackerConfiguration::InternalAddTool( 
  const TrackerToolConfiguration *tool, 
  bool isReference )
{
	
  AddToolFailureEvent fe;

  const CertusToolConfiguration *newTool = 
    dynamic_cast<const CertusToolConfiguration *>( tool );

  if( newTool == NULL )
    {
		fe.Set( "Given tool configuration type not compatible with tracker type." );
		this->InvokeEvent( fe );
    return;
    }

   //copy the tool and add it as a standard or dynamic reference tool
  CertusToolConfiguration *addTool = new CertusToolConfiguration( *newTool );
 
  if( !isReference )
    {
    this->m_TrackerToolList.insert(std::pair<std::string,TrackerToolConfiguration *> 
		(newTool->GetToolName(),addTool) );
    }
  else
    {
    if (this->m_ReferenceTool)
      {
      delete this->m_ReferenceTool;
      }
    this->m_ReferenceTool = new CertusToolConfiguration( *newTool );
    }
  this->InvokeEvent( AddToolSuccessEvent() );
}

CertusToolConfiguration::CertusToolConfiguration() : m_RigidBodyName( "" )
{
}

CertusToolConfiguration::CertusToolConfiguration( const 
  CertusToolConfiguration &other ) : TrackerToolConfiguration( other )
{
  this->m_RigidBodyName = other.m_RigidBodyName;
  this->m_StartMarker = other.m_StartMarker;
  this->m_NumberOfMarkers = other.m_NumberOfMarkers;
  

}


CertusToolConfiguration::~CertusToolConfiguration() 
{
}


std::string 
CertusToolConfiguration::GetToolTypeAsString()
{
  return "CertusToolConfiguration";
}

} // end of name space
