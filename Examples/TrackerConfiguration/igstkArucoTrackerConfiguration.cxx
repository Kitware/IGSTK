/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkArucoTrackerConfiguration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkArucoTrackerConfiguration.h"

namespace igstk
{                  //highest refresh rate for a web camera
const double ArucoTrackerConfiguration::MAXIMAL_REFERESH_RATE = 30;


ArucoTrackerConfiguration::ArucoTrackerConfiguration() :
m_CameraCalibrationFile(""),
m_MarkerSize(0)
{
  this->m_Frequency = this->MAXIMAL_REFERESH_RATE;
}


ArucoTrackerConfiguration::~ArucoTrackerConfiguration()
{
}

double
ArucoTrackerConfiguration::GetMaximalRefreshRate()
{
  return this->MAXIMAL_REFERESH_RATE;
}

void
ArucoTrackerConfiguration::InternalAddTool(
  const TrackerToolConfiguration *tool,
  bool isReference )
{
  AddToolFailureEvent fe;

  const ArucoToolConfiguration *wirelessTool =
    dynamic_cast<const ArucoToolConfiguration *>( tool );

  if( wirelessTool == NULL )
    {
    fe.Set( "Given tool configuration type not compatible with tracker type." );
    this->InvokeEvent( fe );
    return;
    }
  if( !isReference )
    {
    this->m_TrackerToolList.insert(std::pair<std::string,
                                   TrackerToolConfiguration *>
      (wirelessTool->GetToolName(),
       new ArucoToolConfiguration( *wirelessTool )) );
    }
  else
    {
    if (this->m_ReferenceTool)
      {
      delete this->m_ReferenceTool;
      }
    this->m_ReferenceTool = new ArucoToolConfiguration( *wirelessTool );
    }
  this->InvokeEvent( AddToolSuccessEvent() );
}

/*initialize with an invalid marker ID, IDs are in [0,1023]*/
ArucoToolConfiguration::ArucoToolConfiguration() : m_MarkerID( 1024 )
{
}


ArucoToolConfiguration::ArucoToolConfiguration( const
  ArucoToolConfiguration &other ) : TrackerToolConfiguration( other )
{
  this->m_MarkerID = other.m_MarkerID;
}


ArucoToolConfiguration::~ArucoToolConfiguration()
{
}


std::string
ArucoToolConfiguration::GetToolTypeAsString()
{
  return "ArucoToolConfiguration";
}

} // end of name space
