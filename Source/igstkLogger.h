/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLogger.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstk_Logger_h_
#define __igstk_Logger_h_

#include "igstkMacros.h"
#include "igstkMultipleOutput.h"
#include "itkObject.h"


namespace igstk
{
/** \class Logger
    \brief Class Logger is meant for logging information during a run. 
    This class has been derived from MultipleOutput class. 
*/

class Logger : public itk::Object
{
public:

  typedef Logger Self; 
  typedef itk::Object Superclass; 
  typedef itk::SmartPointer< Self > Pointer; 
  typedef itk::SmartPointer< const Self > ConstPointer; 

  /** Method for defining the name of the class */ 
  igstkTypeMacro(Logger, Object); 

  /** Method for creation through the object factory */ 
  igstkNewMacro(Self); 

  typedef  MultipleOutput::StreamType   StreamType;

  /** Definition of types of messages. These codes will be used to regulate the
   * level of detail of messages reported to the final outputs */
    typedef enum 
    { 
      FATAL=0, 
      CRITICAL, 
      WARNING, 
      DEBUG, 
      NOTSET=1000 
    } PriorityLevelType;
   
  /** Set the priority level for the current logger. Only messages that have
   * priorities equal or greater than the one set here will be posted to the
   * current outputs */
  igstkSetMacro( PriorityLevel, PriorityLevelType );

  /** Get the priority level for the current logger. Only messages that have
   * priorities equal or greater than the one set here will be posted to the
   * current outputs */
  igstkGetMacro( PriorityLevel, PriorityLevelType );

  /** Registers another output stream with the multiple output. */
  void AddOutputStream( StreamType & output );

  /** Exposes the Multiple Output Object so messages can be sent out to it */
  MultipleOutput & GetMultipleOutput();


protected:

  /** Constructor */
  Logger();

  /** Destructor */
  virtual ~Logger();

private:

  PriorityLevelType    m_PriorityLevel;
  
  MultipleOutput       m_Output;

};

}

#endif //__igstk_Logger_h_
