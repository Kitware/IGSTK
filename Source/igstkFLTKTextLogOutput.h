/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    igstkFLTKTextLogOutput.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_FLTKTextLogOutput_h_
#define __igstk_FLTKTextLogOutput_h_

#include<iostream>
#include<string>
#include"itkMacro.h"
#include"itkObject.h"
#include"itkObjectFactory.h"
#include"itkSimpleFastMutexLock.h"
#include"itkLogOutput.h"
#include"FL/Fl_Text_Buffer.H"
#include"FL/Fl_Text_Display.H"


namespace igstk
{

/** \class FLTKTextLogOutput
    \brief Class FLTKTextLogOutput represents LogOutput for FLTK Fl_Text_Display.
    This class provides thread safety for logging on FLTK Fl_Text_Display.
*/

class FLTKTextLogOutput : public itk::LogOutput
{

public:

  typedef FLTKTextLogOutput Self;
  typedef itk::LogOutput  Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  typedef Fl_Text_Display  StreamType;
  typedef StreamType*     StreamPointerType;

  itkTypeMacro(FLTKTextLogOutput, itk::LogOutput);
  
  itkNewMacro(FLTKTextLogOutput);

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
  FLTKTextLogOutput();

  /** Destructor */
  virtual ~FLTKTextLogOutput();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  StreamPointerType m_Stream;

  itk::SimpleFastMutexLock m_Mutex;
};

}

#endif //__igstk_FLTKTextLogOutput_h_
