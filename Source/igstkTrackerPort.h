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

class TrackerPort
{
public:

    TrackerPort(void);

    ~TrackerPort(void);

private:

protected:

    /** Vector of all tools on the port */
    std::vector<igstk::TrackerTool>    m_Tools;
};

}
#endif //__igstk_TrackerPort_h_
