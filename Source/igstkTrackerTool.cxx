/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerTool.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTrackerTool.h"

namespace igstk
{

TrackerTool::TrackerTool(void)
{
  // Initialize the variables
  m_ValidityPeriod = 0.0; // current values are alreay invalid
  m_Transform.SetToIdentity( m_ValidityPeriod ); 
  m_Error = 0.0;
}

TrackerTool::~TrackerTool(void)
{
}

}

