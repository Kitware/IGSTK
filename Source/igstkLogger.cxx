/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLogger.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkLogger.h"

namespace igstk
{

Logger::Logger()
{
  m_PriorityLevel = Logger::NOTSET;
}

Logger::~Logger()
{
  m_Output.Flush();
}


/** Adds an output stream to the MultipleOutput for writing. */
void 
Logger::AddOutputStream( StreamType & output )
{
  // delegates to MultipleOutput
  m_Output.AddOutputStream( output ); 
}


MultipleOutput & 
Logger::GetMultipleOutput()
{
  return m_Output;
}


}

