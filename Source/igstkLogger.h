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

namespace igstk
{
/** \class Logger
    \brief Class Logger is meant for logging information during a run. 
    This class has been derived from MultipleOutput class. 
*/

class Logger : public igstk::MultipleOutput
{
public:

    typedef enum 
    { 
        FATAL=0, CRITICAL, WARNING, DEBUG, NOTSET 
    } PriorityLevelType;
   
    /** Constructor */
    Logger( void );


    igstkSetMacro( PriorityLevel, PriorityLevelType );

    igstkGetMacro( PriorityLevel, PriorityLevelType );

private:

    PriorityLevelType    m_PriorityLevel;

};

}

#endif //__igstk_Logger_h_
