/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerPort.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTrackerPort.h"

namespace igstk
{

TrackerPort::TrackerPort(void)
{
  this->ClearTools();
}


TrackerPort::~TrackerPort(void)
{
  this->ClearTools();
}


void 
TrackerPort
::AddTool( TrackerToolType * tool)
{
  TrackerToolPointer toolPtr = tool;
  m_Tools.push_back( toolPtr );
}



void 
TrackerPort
::ClearTools( void )
{
  m_Tools.clear();
}



int 
TrackerPort
::GetNumberOfTools( void ) const
{
  return m_Tools.size();
}


const TrackerTool *
TrackerPort
::GetTool( unsigned int toolNumber ) const
{
  if( toolNumber >= m_Tools.size() )
    {
    return 0;  // FIXME  a STATE MACHINE SHOULD PREVENT THIS RISK
    }
  return m_Tools[toolNumber];
}


TrackerTool *
TrackerPort
::GetTool( unsigned int toolNumber ) 
{
  if( toolNumber >= m_Tools.size() )
    {
    return 0;  // FIXME  a STATE MACHINE SHOULD PREVENT THIS RISK
    }
  return m_Tools[toolNumber];
}



}

