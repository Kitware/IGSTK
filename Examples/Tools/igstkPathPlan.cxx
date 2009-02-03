/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPathPlan.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkPathPlan.h"

namespace igstk
{

/** Constructor: Initializes all internal variables. */
PathPlan::PathPlan()
{
  m_EntryPoint.Fill( 0.0 );
  m_TargetPoint.Fill( 10.0 );
  PointType p;
  p.Fill( 0.0 );

  //m_TrackerCofigurations.clear();
  //TrackerConfiguration * config = new TrackerConfiguration;
  //m_TrackerCofigurations.push_back( config );
}

/** Destructor */
PathPlan::~PathPlan()
{

}
} // end of namespace
