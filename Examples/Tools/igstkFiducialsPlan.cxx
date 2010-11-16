/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkFiducialsPlan.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkFiducialsPlan.h"

namespace igstk
{

  /** Constructor: Initializes all internal variables. */
  FiducialsPlan::FiducialsPlan()
  {
    PointType p;
    p.Fill( 0.0 );
    for (unsigned int i=0; i<4; i++)
      {
      m_FiducialPoints.push_back( p );
      }
  }

  /** Destructor */
  FiducialsPlan::~FiducialsPlan()
  {

  }
} // end of namespace
