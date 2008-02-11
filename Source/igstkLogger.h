/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLogger.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkLogger_h
#define __igstkLogger_h

#include "itkLogger.h"
#include "igstkMacros.h"
#include "itkObject.h"

namespace igstk
{
/** \class Logger
 *
 *  \brief Class used for handling logging tasks.
 *
 *  This class derives from the ITK Logger. Its
 *  purpose is two-fold. First, it specifies the default
 *  priority level of messages. Second, it implements the
 *  ShouldBuildMessage API which enables lazy evaluation of
 *  messages passed to a logger through the logging macros.
 */
class Logger : public ::itk::Logger
{
public:
  /** General Typedefs. */
  typedef Logger                            Self;
  typedef ::itk::Logger                     Superclass;
  typedef ::itk::SmartPointer< Self >       Pointer;
  typedef ::itk::SmartPointer< const Self > ConstPointer;

  /** We do not use macros for New(), GetNameOfClass()
   *  since igstkLogger.h is included in
   *  igstkMacros.h
   */
  virtual const char* GetNameOfClass() const
    {
    return "Logger";
    }

  /** Makes a new Logger and returns a smart pointer to it. */
  static Pointer New(void)
    {
    Pointer smartPtr;
    Self *rawPtr = new Self;
    smartPtr = rawPtr;
    rawPtr->UnRegister();
    return smartPtr;
    }

  /** Returns true if the priority level of the message will cause
   *  the message to be written to a log.
   */
  virtual bool ShouldBuildMessage(PriorityLevelType message_level);

protected:

  /** Constructor */
  Logger();

  /** Destructor */
  virtual ~Logger();

}; // Logger

} // namespace igstk

#endif // __igstkLogger_h
