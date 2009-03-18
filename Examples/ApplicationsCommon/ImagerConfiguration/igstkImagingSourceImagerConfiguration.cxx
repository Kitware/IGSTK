/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImagingSourceImagerConfiguration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkImagingSourceImagerConfiguration.h"

namespace igstk
{                  //highest referesh rate 
const double ImagingSourceImagerConfiguration::MAXIMAL_REFERESH_RATE = 30;


ImagingSourceImagerConfiguration::ImagingSourceImagerConfiguration():
  m_BufferSize(50)
{
  this->m_Frequency = this->MAXIMAL_REFERESH_RATE;

}


ImagingSourceImagerConfiguration::~ImagingSourceImagerConfiguration()
{
}

double
ImagingSourceImagerConfiguration::GetMaximalRefreshRate()
{
  return this->MAXIMAL_REFERESH_RATE;
}

void 
ImagingSourceImagerConfiguration::InternalAddTool( const
  ImagerToolConfiguration *tool )
{
  AddToolFailureEvent fe;

  const ImagingSourceToolConfiguration *wirelessTool = 
    dynamic_cast<const ImagingSourceToolConfiguration *>( tool );

  if( wirelessTool == NULL )
  {
    fe.Set( "Given tool configuration type not compatible with tracker type." );
    this->InvokeEvent( fe );
    return;
  }

  this->m_ImagerToolList.push_back( new ImagingSourceToolConfiguration( 
                                                              *wirelessTool ) );
  this->InvokeEvent( AddToolSuccessEvent() );
}


ImagingSourceToolConfiguration::ImagingSourceToolConfiguration()
{

}



ImagingSourceToolConfiguration::ImagingSourceToolConfiguration( const 
  ImagingSourceToolConfiguration &other ) : ImagerToolConfiguration( other )
{

}


ImagingSourceToolConfiguration::~ImagingSourceToolConfiguration() 
{
}


std::string 
ImagingSourceToolConfiguration::GetToolTypeAsString()
{
  return "ImagingSourceToolConfiguration";
}

} // end of name space
