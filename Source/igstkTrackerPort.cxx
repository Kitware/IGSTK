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

TrackerPort::TrackerPort(void):m_StateMachine(this)
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



unsigned int 
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


/** Print object information */
void TrackerPort::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Number of tools: " << this->m_Tools.size() << std::endl;
  for(unsigned int i=0; i < m_Tools.size(); ++i )
  {
    if( this->m_Tools[i] )
    {
      os << indent << *(this->m_Tools[i]) << std::endl;
    }
  }
}


std::ostream& operator<<(std::ostream& os, const TrackerPort& o)
{
  o.Print(os, 0);
  return os;
}


}

