/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCompressedDVVideoImagerConfiguration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkCompressedDVVideoImagerConfiguration.h"

namespace igstk
{                  //highest referesh rate
const double CompressedDVVideoImagerConfiguration::MAXIMAL_REFERESH_RATE = 30;


CompressedDVVideoImagerConfiguration::CompressedDVVideoImagerConfiguration():
  m_BufferSize(50)
{
  this->m_Frequency = this->MAXIMAL_REFERESH_RATE;

}


CompressedDVVideoImagerConfiguration::~CompressedDVVideoImagerConfiguration()
{
}

double
CompressedDVVideoImagerConfiguration::GetMaximalRefreshRate()
{
  return this->MAXIMAL_REFERESH_RATE;
}

void
CompressedDVVideoImagerConfiguration::InternalAddTool( const
  VideoImagerToolConfiguration *tool )
{
  AddToolFailureEvent fe;

  const CompressedDVToolConfiguration *wirelessTool =
    dynamic_cast<const CompressedDVToolConfiguration *>( tool );

if( wirelessTool == NULL )
{
  fe.Set( "Given tool configuration type not compatible with tracker type." );
  this->InvokeEvent( fe );
  return;
}

  this->m_VideoImagerToolList.push_back( new CompressedDVToolConfiguration(
                                                              *wirelessTool ) );
  this->InvokeEvent( AddToolSuccessEvent() );
}

CompressedDVToolConfiguration::CompressedDVToolConfiguration()
{

}

CompressedDVToolConfiguration::CompressedDVToolConfiguration( const
  CompressedDVToolConfiguration &other ) : VideoImagerToolConfiguration( other )
{

}

CompressedDVToolConfiguration::~CompressedDVToolConfiguration()
{
}


std::string
CompressedDVToolConfiguration::GetToolTypeAsString()
{
  return "CompressedDVToolConfiguration";
}

} // end of name space
