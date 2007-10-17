/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFlockOfBirdsTrackerTool.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
//Warning about: identifier was truncated to '255' characters 
//in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkFlockOfBirdsTrackerTool.h"

namespace igstk
{

/** Constructor (initializes FlockOfBirds-specific tool values) */
FlockOfBirdsTrackerTool::FlockOfBirdsTrackerTool():m_StateMachine(this)
{
}

/** Destructor */
FlockOfBirdsTrackerTool::~FlockOfBirdsTrackerTool()
{
}

/** Print Self function */
void FlockOfBirdsTrackerTool::PrintSelf( std::ostream& os, 
                                         itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


}
