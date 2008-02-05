/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    IPlanIO.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __IPlanIO_h
#define __IPlanIO_h

#include "igstkLandmark3DRegistration.h"

namespace igstk
{

class IPlanIO
{

public:
  
  typedef Landmark3DRegistration::LandmarkImagePointType      PointType;
  typedef Landmark3DRegistration::LandmarkPointContainerType  PointListType;

  int RequestRead(  std::string fileName );
  int RequestWrite( std::string fileName );

  PointType            m_EntryPoint;
  PointType            m_TargetPoint;
  PointListType        m_FiducialPoints;

  IPlanIO();
  virtual ~IPlanIO();

};

} // end of namespace
#endif
