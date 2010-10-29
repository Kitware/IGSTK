/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFLTKTextBufferLogOutput.cxx
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
#include "igstkFLTKTextBufferLogOutput.h"

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters in the
// debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
// Disabling warning C4355: 'this' : used in base member initializer list
#pragma warning ( disable : 4355 )
#endif


namespace igstk
{

/** Constructor */
FLTKTextBufferLogOutput::FLTKTextBufferLogOutput():m_StateMachine(this)
{
  this->m_Stream = 0;
}

/** Destructor */
FLTKTextBufferLogOutput::~FLTKTextBufferLogOutput()
{
  free( m_Stream->text() );
}


/** Set an output stream */
void FLTKTextBufferLogOutput::SetStream(StreamType &Stream)
{
  this->m_Stream = &Stream;
}


/** Flush a buffer */
void FLTKTextBufferLogOutput::Flush()
{
  // Flushing is immediately done. (we don't need)
}


/** Write to a buffer */
void FLTKTextBufferLogOutput::Write(double timestamp)
{
  FLTKTextBufferLogOutput::m_Mutex.Lock();
  if( this->m_Stream )
    {
    std::ostringstream ostr;
    ostr.precision(30);
    ostr << timestamp;
    this->m_Stream->append( ostr.str().c_str() );
    }
  FLTKTextBufferLogOutput::m_Mutex.Unlock();
}


/** Write to a buffer */
void FLTKTextBufferLogOutput::Write(std::string const &content)
{
  FLTKTextBufferLogOutput::m_Mutex.Lock();
  if( this->m_Stream )
    {
    this->m_Stream->append( content.c_str() );
    }
  FLTKTextBufferLogOutput::m_Mutex.Unlock();
}


/** Write to a buffer */
void FLTKTextBufferLogOutput
::Write(std::string const &content,double timestamp)
{
  FLTKTextBufferLogOutput::m_Mutex.Lock();
  if( this->m_Stream )
    {
    std::ostringstream ostr;
    ostr.precision(30);
    ostr << timestamp << "  :  " << content;
    this->m_Stream->append( ostr.str().c_str() );
    }
  FLTKTextBufferLogOutput::m_Mutex.Unlock();
}


/** Print Self function */
void FLTKTextBufferLogOutput
::PrintSelf( std::ostream& os, itk::Indent indent ) const
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
