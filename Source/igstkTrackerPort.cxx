/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerPort.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTrackerPort.h"

namespace igstk
{

TrackerPort::TrackerPort(void)
{
    m_Tools.clear();
}

TrackerPort::~TrackerPort(void)
{
    m_Tools.clear();
}

}
