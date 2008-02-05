/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    IPlanIO.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "IPlanIO.h"

namespace igstk
{

/** Constructor: Initializes all internal variables. */
IPlanIO::IPlanIO()
{
  m_EntryPoint.Fill( 0.0 );
  m_TargetPoint.Fill( 0.0 );
  PointType p;
  p.Fill( 0.0 );
  for (int i=0; i<3; i++)
  {
    m_FiducialPoints.push_back( p );
  }    
}

int IPlanIO::RequestRead( std::string fileName )
{

  return EXIT_SUCCESS;
}

int IPlanIO::RequestWrite( std::string fileName )
{
  return EXIT_SUCCESS;
}
/** Destructor */
IPlanIO::~IPlanIO()
{

}
} // end of namespace
