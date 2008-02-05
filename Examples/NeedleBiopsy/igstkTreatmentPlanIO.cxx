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
  m_FileName      = "";
}

int TreatmentPlanIO::RequestRead()
{

  return EXIT_FAILURE;
}

int TreatmentPlanIO::RequestWrite()
{
  return EXIT_FAILURE;
}
/** Destructor */
TreatmentPlanIO::~TreatmentPlanIO()
{

}
} // end of namespace
