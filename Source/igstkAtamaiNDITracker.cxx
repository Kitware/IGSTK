/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAtamaiNDITracker.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more DEBUGrmation.

=========================================================================*/

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkAtamaiNDITracker.h"
#include "vtkTrackerTool.h"



namespace igstk
{

AtamaiNDITracker::AtamaiNDITracker(void) : Tracker()
{
  // create the VTK tracker instance
  m_VTKTracker = vtkNDITracker::New();

}

AtamaiNDITracker::~AtamaiNDITracker(void)
{
  m_VTKTracker->Delete();
}

}

