/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkFLTKTextLogOutput.cxx
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
#include"igstkFLTKTextLogOutput.h"


namespace igstk
{

/** Constructor */
FLTKTextLogOutput::FLTKTextLogOutput()
{
  this->m_Stream = 0;
}

/** Destructor */
FLTKTextLogOutput::~FLTKTextLogOutput()
{
}


/** Set file stream */
void FLTKTextLogOutput::SetStream(StreamType &Stream)
{
  this->m_Stream = &Stream;
}


/** flush a buffer */
void FLTKTextLogOutput::Flush()
{
//  FLTKTextLogOutput::m_Mutex.Lock();
//  FLTKTextLogOutput::m_Mutex.Unlock();
}


/** Write to a buffer */
void FLTKTextLogOutput::Write(double timestamp)
{
  FLTKTextLogOutput::m_Mutex.Lock();
  if( this->m_Stream  &&  this->m_Stream->buffer() )
  {
    itk::OStringStream ostr;
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
    itk::OStringStream ostr;
    ostr << timestamp << "  :  " << content;
    this->m_Stream->insert( ostr.str().c_str() );
    this->m_Stream->scroll(this->m_Stream->insert_position(), 0);
  }
  FLTKTextLogOutput::m_Mutex.Unlock();
}


}

