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

#ifndef __igstk_TrackerPort_h_
#define __igstk_TrackerPort_h_

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

class Tracker;

class TrackerPort
{
public:

    typedef igstk::TrackerTool             TrackerToolType;
    typedef std::vector< TrackerToolType > TrackerToolVectorType;

    FriendClassMacro( Tracker );

    TrackerPort(void);

    ~TrackerPort(void);

    /** The "AddTool" method adds a tool to the port. */
    void AddTool( const TrackerToolType& tool);

    /** The "ClearTools" clears all the tools. */
    void ClearTools( void );

    /** The "GetNumberOfTools" method returns number of tools. */ 
    inline int GetNumberOfTools( void )
    {
      return m_Tools.size();
    }

private:

    /** Vector of all tools on the port */
    TrackerToolVectorType       m_Tools;
};

}
#endif //__igstk_TrackerPort_h_
