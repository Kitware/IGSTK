/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkFLTKTextBufferLogOutput.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_FLTKTextBufferLogOutput_h_
#define __igstk_FLTKTextBufferLogOutput_h_

#include<iostream>
#include<string>
#include"itkMacro.h"
#include"itkObject.h"
#include"itkObjectFactory.h"
#include"itkSimpleFastMutexLock.h"
#include"itkLogOutput.h"
#include"FL/Fl_Text_Buffer.h"


namespace igstk
{

/** \class FLTKTextBufferLogOutput
    \brief Class FLTKTextBufferLogOutput represents a standard stream output stream.
    This class provides thread safety for the standard stream output stream.
*/

class FLTKTextBufferLogOutput : public itk::LogOutput
{

public:

  typedef FLTKTextBufferLogOutput Self;
  typedef itk::LogOutput  Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  typedef Fl_Text_Buffer  StreamType;
  typedef StreamType*     StreamPointerType;

  itkTypeMacro(FLTKTextBufferLogOutput, itk::LogOutput);
  
  itkNewMacro(FLTKTextBufferLogOutput);

  itkGetMacro(Stream, StreamPointerType);

  /** Set a standard stream pointer */
  void SetStream(StreamType &Stream);
  
  /** flush a buffer */
  virtual void Flush();

  /** Write to multiple outputs */
  virtual void Write(double timestamp);

  /** Write to a buffer */
  virtual void Write(std::string const &content);

  /** Write to a buffer */
  virtual void Write(std::string const &content, double timestamp);

protected:
  /** Constructor */
  FLTKTextBufferLogOutput();

  /** Destructor */
  virtual ~FLTKTextBufferLogOutput();

private:

  StreamPointerType m_Stream;

  itk::SimpleFastMutexLock m_Mutex;
};

}

#endif //__igstk_FLTKTextBufferLogOutput_h_
