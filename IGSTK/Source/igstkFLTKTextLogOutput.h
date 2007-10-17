/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFLTKTextLogOutput.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkFLTKTextLogOutput_h
#define __igstkFLTKTextLogOutput_h

#include <iostream>
#include <string>

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkSimpleFastMutexLock.h"
#include "itkLogOutput.h"

#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>

#include "igstkMacros.h"
#include "igstkStateMachine.h"


namespace igstk
{

/** \class FLTKTextLogOutput
 *  \brief Class FLTKTextLogOutput represents LogOutput for 
 *  FLTK Fl_Text_Display.
 *  This class provides thread safety for logging on FLTK Fl_Text_Display.
 *
 *  \ingroup Logging
 */

class FLTKTextLogOutput : public ::itk::LogOutput
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( FLTKTextLogOutput, ::itk::LogOutput )

public:

  typedef Fl_Text_Display  StreamType;
  typedef StreamType*      StreamPointerType;

  /** Get an output stream pointer */
  igstkGetMacro(Stream, StreamPointerType);

  /** Set an output stream pointer */
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

  /** Stream pointer (pointer to a FLTK Fl_Text_Display) */
  StreamPointerType m_Stream;

  /** Mutex lock */
  itk::SimpleFastMutexLock m_Mutex;
};

} // end namespace igstk

#endif //__igstk_FLTKTextLogOutput_h_
