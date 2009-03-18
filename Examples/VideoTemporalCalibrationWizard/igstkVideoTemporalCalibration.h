/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoTemporalCalibration.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkVideoTemporalCalibration_h
#define __igstkVideoTemporalCalibration_h

#include "igstkTransform.h"


namespace igstk
{

/** \class VideoTemporalCalibration
* 
* \brief Defines 
*
*/

class VideoTemporalCalibration
{

public:

  std::vector< igstk::Transform >      m_PointerTransforms;
  std::vector< igstk::Transform >      m_ProbeTransforms;

  VideoTemporalCalibration();
  virtual ~VideoTemporalCalibration();

};


} // end of namespace
#endif
