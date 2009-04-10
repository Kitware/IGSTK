/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkWebcamWinImagerConfiguration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkWebcamWinImagerConfiguration.h"

namespace igstk
{                  //highest referesh rate
const double WebcamWinImagerConfiguration::MAXIMAL_REFERESH_RATE = 30;


WebcamWinImagerConfiguration::WebcamWinImagerConfiguration():
  m_BufferSize(50)
{
  this->m_Frequency = this->MAXIMAL_REFERESH_RATE;

}


WebcamWinImagerConfiguration::~WebcamWinImagerConfiguration()
{
}

double
WebcamWinImagerConfiguration::GetMaximalRefreshRate()
{
  return this->MAXIMAL_REFERESH_RATE;
}

void
WebcamWinImagerConfiguration::InternalAddTool( const
  ImagerToolConfiguration *tool )
{
  AddToolFailureEvent fe;

  const WebcamWinToolConfiguration *wirelessTool =
    dynamic_cast<const WebcamWinToolConfiguration *>( tool );

  if( wirelessTool == NULL )
  {
    fe.Set( "Given tool configuration type not compatible with tracker type." );
    this->InvokeEvent( fe );
    return;
  }

  this->m_ImagerToolList.push_back( new WebcamWinToolConfiguration(
                                                              *wirelessTool ) );
  this->InvokeEvent( AddToolSuccessEvent() );
}


WebcamWinToolConfiguration::WebcamWinToolConfiguration()
{

}



WebcamWinToolConfiguration::WebcamWinToolConfiguration( const
  WebcamWinToolConfiguration &other ) : ImagerToolConfiguration( other )
{

}


WebcamWinToolConfiguration::~WebcamWinToolConfiguration()
{
}


std::string
WebcamWinToolConfiguration::GetToolTypeAsString()
{
  return "WebcamWinToolConfiguration";
}

} // end of name space
