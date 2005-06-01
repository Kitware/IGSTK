/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkFLTKTextBufferLogOutput.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include<iostream>
#include<stdio.h>
#include"itkMacro.h"
#include"igstkFLTKTextBufferLogOutput.h"


namespace igstk
{

/** Constructor */
FLTKTextBufferLogOutput::FLTKTextBufferLogOutput()
{
  this->m_Stream = 0;
}

/** Destructor */
FLTKTextBufferLogOutput::~FLTKTextBufferLogOutput()
{
}


/** Set file stream */
void FLTKTextBufferLogOutput::SetStream(StreamType &Stream)
{
  this->m_Stream = &Stream;
}


/** flush a buffer */
void FLTKTextBufferLogOutput::Flush()
{
//  FLTKTextBufferLogOutput::m_Mutex.Lock();
//  FLTKTextBufferLogOutput::m_Mutex.Unlock();
}


/** Write to a buffer */
void FLTKTextBufferLogOutput::Write(double timestamp)
{
  FLTKTextBufferLogOutput::m_Mutex.Lock();
  if( this->m_Stream )
  {
    itk::OStringStream ostr;
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
void FLTKTextBufferLogOutput::Write(std::string const &content, double timestamp)
{
  FLTKTextBufferLogOutput::m_Mutex.Lock();
  if( this->m_Stream )
  {
    itk::OStringStream ostr;
    ostr << timestamp << "  :  " << content;
    this->m_Stream->append( ostr.str().c_str() );
  }
  FLTKTextBufferLogOutput::m_Mutex.Unlock();
}


/** Print Self function */
void FLTKTextBufferLogOutput::PrintSelf( std::ostream& os, itk::Indent indent ) const
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


}

