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
#ifndef __igstkTreatmentPlanIO_h
#define __igstkTreatmentPlanIO_h


#include "igstkTreatmentPlan.h"

namespace igstk
{

class TreatmentPlanIO
{

public:
  
  std::string GetFileName()
    {
    return m_FileName;
    }

  void SetFileName( std::string fileName)
    {
    m_FileName = fileName;
    }

  int RequestRead();
  int RequestWrite();

  TreatmentPlan * GetTreatmentPlan()
    {
    return m_TreatmentPlan;
    }

  void SetTreatmentPlan(TreatmentPlan * plan)
    {
    m_TreatmentPlan = plan;
    }

  TreatmentPlanIO();
  virtual ~TreatmentPlanIO();

private:
  TreatmentPlan         * m_TreatmentPlan;
  std::string             m_FileName;

};

} // end of namespace
#endif
