/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkOpenIGTLinkVideoImagerConfiguration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTerasonVideoImagerConfiguration.h"

namespace igstk
{                  //highest referesh rate
const double TerasonVideoImagerConfiguration::MAXIMAL_REFERESH_RATE = 30;


TerasonVideoImagerConfiguration::TerasonVideoImagerConfiguration():
m_BufferSize(50)
{
  this->m_Frequency = this->MAXIMAL_REFERESH_RATE;

}


TerasonVideoImagerConfiguration::~TerasonVideoImagerConfiguration()
{
}

double
TerasonVideoImagerConfiguration::GetMaximalRefreshRate()
{
  return this->MAXIMAL_REFERESH_RATE;
}

void
TerasonVideoImagerConfiguration::InternalAddTool( const
  VideoImagerToolConfiguration *tool )
{
  AddToolFailureEvent fe;

  const TerasonToolConfiguration *wirelessTool =
    dynamic_cast<const TerasonToolConfiguration *>( tool );

  if( wirelessTool == NULL )
  {
    fe.Set( "Given tool configuration type not compatible with tracker type." );
    this->InvokeEvent( fe );
    return;
  }
  this->m_VideoImagerToolList.push_back( new TerasonToolConfiguration(
                                                              *wirelessTool ) );
  this->InvokeEvent( AddToolSuccessEvent() );
}

TerasonToolConfiguration::TerasonToolConfiguration()
{

}

TerasonToolConfiguration::TerasonToolConfiguration( const
  TerasonToolConfiguration &other ) : VideoImagerToolConfiguration( other )
{

}

TerasonToolConfiguration::~TerasonToolConfiguration()
{
}

std::string
TerasonToolConfiguration::GetToolTypeAsString()
{
  return "TerasonToolConfiguration";
}

} // end of name space
