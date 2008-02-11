/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerToolTest.cxx
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
//  Warning about: identifier was truncated to '255' characters
//  in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include <fstream>

#include "igstkTrackerTool.h"

namespace igstk
{
class TestingTrackerTool : public igstk::TrackerTool
{
public:
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( TestingTrackerTool, TrackerTool )

protected:
  TestingTrackerTool():m_StateMachine(this)
    {
    }
  ~TestingTrackerTool()
    {
    }

  /** Check if the tracker tool is configured or not. This method should
   *  be implemented in the derived classes*/
  virtual bool CheckIfTrackerToolIsConfigured( ) const { return true; }
};
}

int igstkTrackerToolTest( int, char * [] )
{
  igstk::RealTimeClock::Initialize();

  typedef igstk::TestingTrackerTool             TrackerToolType;
  typedef TrackerToolType::TransformType        TransformType;

  TrackerToolType::Pointer trackerTool = TrackerToolType::New();

  trackerTool->RequestConfigure();

  std::cout << trackerTool << std::endl;

  return EXIT_SUCCESS;
}
