/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMultipleOutput.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkMultipleOutput.h"

namespace igstk
{

MultipleOutput::MultipleOutput()
{
  m_Output.clear();
}


MultipleOutput::~MultipleOutput()
{
  this->Flush();
}


/** Adds an output stream to the MultipleOutput for writing. */
void 
MultipleOutput::AddOutputStream( StreamType & output )
{
  m_Output.insert( &output ); // insert the address
}


/** The Flush method flushes all the streams. */  
void 
MultipleOutput::Flush( void )
{
  ContainerType::iterator itr = m_Output.begin();
  ContainerType::iterator end = m_Output.end();
  while( itr != end )
    {
    (*itr)->flush(); 
    ++itr;
    }
}



}

