/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAscension3DGTrackerConfiguration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkAscension3DGTrackerConfiguration.h"

namespace igstk
{                  //highest referesh rate for the Ascension3DG tracker family (Sx60)
const double Ascension3DGTrackerConfiguration::MAXIMAL_REFERESH_RATE = 60;


Ascension3DGTrackerConfiguration::Ascension3DGTrackerConfiguration()
{
  this->m_Frequency = this->MAXIMAL_REFERESH_RATE;
}


Ascension3DGTrackerConfiguration::~Ascension3DGTrackerConfiguration()
{
}

double
Ascension3DGTrackerConfiguration::GetMaximalRefreshRate()
{
  return this->MAXIMAL_REFERESH_RATE;
}

void 
Ascension3DGTrackerConfiguration::InternalAddTool( 
  const TrackerToolConfiguration *tool, 
  bool isReference )
{
  AddToolFailureEvent fe;

  const Ascension3DGToolConfiguration *wiredTool = 
    dynamic_cast<const Ascension3DGToolConfiguration *>( tool );

  if( wiredTool == NULL )
    {
    fe.Set( "Given tool configuration type not compatible with tracker type." );
    this->InvokeEvent( fe );
    return;
    }
  if( !isReference )
    {
    this->m_TrackerToolList.insert(std::pair<std::string, 
                                   TrackerToolConfiguration *>
      (wiredTool->GetToolName(), 
       new Ascension3DGToolConfiguration( *wiredTool )) );
    }
  else
    {
    if (this->m_ReferenceTool)
      {
      delete this->m_ReferenceTool;
      }
    this->m_ReferenceTool = new Ascension3DGToolConfiguration( *wiredTool );
    }
  this->InvokeEvent( AddToolSuccessEvent() );
}

Ascension3DGToolConfiguration::Ascension3DGToolConfiguration() 
{
  this->m_PortNumber = -1;
}

Ascension3DGToolConfiguration::~Ascension3DGToolConfiguration() 
{
}


std::string 
Ascension3DGToolConfiguration::GetToolTypeAsString()
{
  return "Ascension3DGToolConfiguration";
}

} // end of name space
