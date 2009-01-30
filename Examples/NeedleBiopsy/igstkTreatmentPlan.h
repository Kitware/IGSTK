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
#ifndef __igstkTreatmentPlan_h
#define __igstkTreatmentPlan_h

#include "igstkLandmark3DRegistration.h"
#include "igstkTrackerConfiguration.h"

namespace igstk
{

/** \class TreatmentPlan
* 
* \brief Defines a simple treatment plan for needle biopsy application
*
* This class define the treatment plan including: a entry point, a target 
* point, together these two define the biopsy trajectory, and a list of 
* fiducial points (in the image) for pair point landmark registration.
*
* These points can be defined prior to the surgery by going through 
* pre-operative images.
*/

class TreatmentPlan
{

public:

  typedef Landmark3DRegistration::LandmarkImagePointType      PointType;
  typedef Landmark3DRegistration::LandmarkPointContainerType  PointListType;

  PointType            m_EntryPoint;
  PointType            m_TargetPoint;
  PointListType        m_FiducialPoints;

  // To be added
  // Add the capability to save tracker configuration in plan
  std::vector< TrackerConfiguration * >      m_TrackerCofigurations;

  TreatmentPlan();
  virtual ~TreatmentPlan();

};


} // end of namespace
#endif
