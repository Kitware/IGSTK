/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    TrackerCondigurationGUI.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "TrackerConfigurationGUI.h"

/** Constructor: Initializes all internal variables. */
TrackerConfigurationGUI::TrackerConfigurationGUI( TrackerType trackerType)
{
  m_TrackerType = trackerType;
    
}

/** Destructor */
TrackerConfigurationGUI::~TrackerConfigurationGUI()
{

}

