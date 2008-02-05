/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkTrackerConfigurationGUI.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __TrackerConfigurationGUI_h
#define __TrackerConfigurationGUI_h

#include "igstkTrackerConfiguration.h"

namespace igstk
{

class TrackerConfigurationGUI
{

public:
  
  typedef TrackerConfiguration::TrackerType TrackerType;

  TrackerConfigurationGUI( TrackerType trackerType);

  TrackerType GetTrackerType() { return m_TrackerType; };

  virtual ~TrackerConfigurationGUI();

private:
  
  TrackerType            m_TrackerType;

};

} // end of name space
#endif
