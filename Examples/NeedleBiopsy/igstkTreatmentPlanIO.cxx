/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkTreatmentPlanIO.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTreatmentPlanIO.h"

namespace igstk
{

/** Constructor: Initializes all internal variables. */
TreatmentPlanIO::TreatmentPlanIO()
{
  m_TreatmentPlan = new TreatmentPlan;
}

int TreatmentPlanIO::RequestRead( std::string fileName )
{

  return EXIT_SUCCESS;
}

int TreatmentPlanIO::RequestWrite( std::string fileName )
{
  return EXIT_SUCCESS;
}
/** Destructor */
TreatmentPlanIO::~TreatmentPlanIO()
{

}
} // end of namespace
