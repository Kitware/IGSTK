/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkTreatmentPlanIO.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __TreatmentPlanIO_h
#define __TreatmentPlanIO_h


#include "igstkTreatmentPlan.h"

namespace igstk
{

class TreatmentPlanIO
{

public:
  
  int RequestRead(  std::string fileName );
  int RequestWrite( std::string fileName );

  TreatmentPlanIO();
  virtual ~TreatmentPlanIO();

private:
  TreatmentPlan         * m_TreatmentPlan;

};

} // end of namespace
#endif
