/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTerasonImagerConfiguration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTerasonImagerConfiguration.h"

namespace igstk
{                  //highest referesh rate 
const double TerasonImagerConfiguration::MAXIMAL_REFERESH_RATE = 30;


TerasonImagerConfiguration::TerasonImagerConfiguration():
m_BufferSize(50)
{
  this->m_Frequency = this->MAXIMAL_REFERESH_RATE;

}


TerasonImagerConfiguration::~TerasonImagerConfiguration()
{
}

double
TerasonImagerConfiguration::GetMaximalRefreshRate()
{
  return this->MAXIMAL_REFERESH_RATE;
}

void 
TerasonImagerConfiguration::InternalAddTool( const
  ImagerToolConfiguration *tool )
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
  this->m_ImagerToolList.push_back( new TerasonToolConfiguration( 
                                                              *wirelessTool ) );
  this->InvokeEvent( AddToolSuccessEvent() );
}

TerasonToolConfiguration::TerasonToolConfiguration()
{

}

TerasonToolConfiguration::TerasonToolConfiguration( const 
  TerasonToolConfiguration &other ) : ImagerToolConfiguration( other )
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
