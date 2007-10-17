/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCommunication.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkCommunication.h"
#include "igstkMacros.h"


#ifdef _MSC_VER
// Disabling warning C4355: 'this' : used in base member initializer list
#pragma warning ( disable : 4355 )
#endif

namespace igstk
{

Communication::Communication( void ):m_StateMachine(this)
{
  m_TimeoutPeriod = 5000;
  m_ReadTerminationCharacter = 255;
  m_UseReadTerminationCharacter = false;
}

Communication::~Communication( void )
{
}

Communication::ResultType Communication::OpenCommunication( void )
{ 
  igstkLogMacro( FATAL, " Communication::OpenCommunication() "
            << " method is not implemented yet.\n" );

  return SUCCESS;
}

Communication::ResultType Communication::CloseCommunication( void )
{
  igstkLogMacro( FATAL, " Communication::CloseCommunication() "
            << " method is not implemented yet.\n" );
 
 return SUCCESS;
}

void Communication::PrintSelf(std::ostream &os, itk::Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  
  os << indent << "TimeoutPeriod: " << m_TimeoutPeriod 
     << std::endl;
  os << indent << "ReadTerminationCharacter: " << "<"
     << (static_cast<unsigned int>(m_ReadTerminationCharacter) & 0xFF)
     << ">" << std::endl;
  os << indent << "UseReadTerminationCharacter: "
     << m_UseReadTerminationCharacter 
     << std::endl;
}

}
