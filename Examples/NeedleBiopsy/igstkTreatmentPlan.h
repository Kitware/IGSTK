/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkTreatmentPlan.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __TreatmentPlan_h
#define __TreatmentPlan_h

#include "igstkLandmark3DRegistration.h"
#include "igstkTrackerConfiguration.h"

namespace igstk
{

class TreatmentPlan
{

public:

  typedef Landmark3DRegistration::LandmarkImagePointType      PointType;
  typedef Landmark3DRegistration::LandmarkPointContainerType  PointListType;

  PointType            m_EntryPoint;
  PointType            m_TargetPoint;
  PointListType        m_FiducialPoints;

  std::vector< TrackerConfiguration * >             m_TrackerCofigurations;

  TreatmentPlan();
  virtual ~TreatmentPlan();

};


} // end of namespace
#endif
