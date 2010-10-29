/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFLTKTextLogOutput.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <stdio.h>
#include <itkMacro.h>
#include "igstkFLTKTextLogOutput.h"


#if defined(_MSC_VER)
// Disabling warning C4355: 'this' : used in base member initializer list
#pragma warning ( disable : 4355 )
// Warning about: identifier was truncated to '255' characters in the
// debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif


namespace igstk
{

/** Constructor */
FLTKTextLogOutput::FLTKTextLogOutput():m_StateMachine(this)
{
  this->m_Stream = 0;
}

/** Destructor */
FLTKTextLogOutput::~FLTKTextLogOutput()
{
  free( m_Stream->buffer()->text() );
}


/** Set an output stream */
void FLTKTextLogOutput::SetStream(StreamType &Stream)
{
  this->m_Stream = &Stream;
}


/** Flush a buffer */
void FLTKTextLogOutput::Flush()
{
  // Flushing is immediately done. (we don't need)
}


/** Write to a buffer */
void FLTKTextLogOutput::Write(double timestamp)
{
  FLTKTextLogOutput::m_Mutex.Lock();
  if( this->m_Stream  &&  this->m_Stream->buffer() )
    {
    std::ostringstream ostr;
    ostr.precision(30);
    ostr << timestamp;
    this->m_Stream->insert( ostr.str().c_str() );
    }
  FLTKTextLogOutput::m_Mutex.Unlock();
}


/** Write to a buffer */
void FLTKTextLogOutput::Write(std::string const &content)
{
  FLTKTextLogOutput::m_Mutex.Lock();
  if( this->m_Stream  &&  this->m_Stream->buffer() )
    {
    this->m_Stream->insert( content.c_str() );
    }
  FLTKTextLogOutput::m_Mutex.Unlock();
}


/** Write to a buffer */
void FLTKTextLogOutput::Write(std::string const &content, double timestamp)
{
  FLTKTextLogOutput::m_Mutex.Lock();
  if( this->m_Stream  &&  this->m_Stream->buffer() )
    {
    std::ostringstream ostr;
    ostr.precision(30);
    ostr << timestamp << "  :  " << content;
    this->m_Stream->insert( ostr.str().c_str() );
    this->m_Stream->scroll(this->m_Stream->insert_position(), 0);
    }
  FLTKTextLogOutput::m_Mutex.Unlock();
}


/** PrintSelf function */
void FLTKTextLogOutput::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  if( this->m_Stream )
    {
    os << indent << "Stream is available" << std::endl;
    }
  else
    {
    os << indent << "Stream is NULL" << std::endl;
    }
}

} // end namespace igstk
