/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkWebcamWinVideoImagerConfiguration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkWebcamWinVideoImagerConfiguration.h"

namespace igstk
{                  //highest referesh rate
const double WebcamWinVideoImagerConfiguration::MAXIMAL_REFERESH_RATE = 30;


WebcamWinVideoImagerConfiguration::WebcamWinVideoImagerConfiguration():
m_BufferSize(50)
{
  this->m_Frequency = this->MAXIMAL_REFERESH_RATE;

}


WebcamWinVideoImagerConfiguration::~WebcamWinVideoImagerConfiguration()
{
}

double
WebcamWinVideoImagerConfiguration::GetMaximalRefreshRate()
{
  return this->MAXIMAL_REFERESH_RATE;
}

void
WebcamWinVideoImagerConfiguration::InternalAddTool( const
  VideoImagerToolConfiguration *tool )
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

  this->m_VideoImagerToolList.push_back( new WebcamWinToolConfiguration(
                                                              *wirelessTool ) );
  this->InvokeEvent( AddToolSuccessEvent() );
}

WebcamWinToolConfiguration::WebcamWinToolConfiguration()
{

}

WebcamWinToolConfiguration::WebcamWinToolConfiguration( const
  WebcamWinToolConfiguration &other ) : VideoImagerToolConfiguration( other )
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
