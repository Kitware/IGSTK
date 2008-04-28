/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkTrackerConfigurationGUIBase.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTrackerConfigurationGUIBase.h"

namespace igstk
{

/** Constructor: Initializes all internal variables. */
TrackerConfigurationGUIBase::TrackerConfigurationGUIBase()
{
  m_Reporter = ::itk::Object::New();
}

/** Destructor */
TrackerConfigurationGUIBase::~TrackerConfigurationGUIBase()
{
}

unsigned long TrackerConfigurationGUIBase::AddObserver( 
  const ::itk::EventObject & event, ::itk::Command * observer )
{
  return m_Reporter->AddObserver( event, observer );
}

void TrackerConfigurationGUIBase::RemoveObserver( unsigned long tag )
{
  m_Reporter->RemoveObserver( tag );
}

void TrackerConfigurationGUIBase::RemoveAllObservers()
{
  m_Reporter->RemoveAllObservers();
}

} // end of name space
