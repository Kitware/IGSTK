/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkUltrasoundCalibration.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkUltrasoundCalibration_h
#define __igstkUltrasoundCalibration_h

#include "igstkTransform.h"


namespace igstk
{

/** \class UltrasoundCalibration
* 
* \brief Defines 
*
*/

class UltrasoundCalibration
{

public:

  std::vector< igstk::Transform >      m_PointerTransforms;
  std::vector< igstk::Transform >      m_ProbeTransforms;

  UltrasoundCalibration();
  virtual ~UltrasoundCalibration();

};


} // end of namespace
#endif
