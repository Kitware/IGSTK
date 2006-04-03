/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerPort.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTrackerPort_h
#define __igstkTrackerPort_h

#include <vector>

#include "igstkTrackerTool.h"

namespace igstk
{
/** \class TrackerPort
    \brief Generic implementation of the Tracker Port.

    This class provides a generic implementation of a port of
    a tracker. This may contain hardware specific details of 
    the tracker port, and a vector of all tracker tools
    connected to the port.
*/
class TrackerPort : public Object
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( TrackerPort, Object )

public:

  /** Internal types */
  typedef igstk::TrackerTool                TrackerToolType;
  typedef TrackerToolType::Pointer          TrackerToolPointer;
  typedef TrackerToolType::ConstPointer     TrackerToolConstPointer;
  typedef std::vector< TrackerToolPointer > TrackerToolVectorType;

  /** The "AddTool" method adds a tool to the port. */
  void AddTool( TrackerToolType * tool );

  /** The "ClearTools" clears all the tools. */
  void ClearTools( void );

  /** The "GetNumberOfTools" method returns number of tools. */ 
  unsigned int GetNumberOfTools( void ) const;

  /** Get the pointer to a specific tool. Const and non-const versions of the
   * same method */
  TrackerToolType * GetTool( unsigned int );
  const TrackerToolType * GetTool( unsigned int ) const;
  

protected:

  TrackerPort(void);

  ~TrackerPort(void);

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 


private:

  /** Vector of all tools on the port */
  TrackerToolVectorType       m_Tools;

};


std::ostream& operator<<(std::ostream& os, const TrackerPort& o);


}
#endif //__igstk_TrackerPort_h_
