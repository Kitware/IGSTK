/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkUltrasoundCalibrationIO.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkUltrasoundCalibrationIO_h
#define __igstkUltrasoundCalibrationIO_h


#include "igstkUltrasoundCalibration.h"

namespace igstk
{

/** \class UltrasoundCalibrationIO
* 
* \brief Reader/Writer class for UltrasoundCalibration
*
* Sample format:
---------------------------------------------------------------------
# Fiducial points
98.4887   -152.976  -181
-1.89214  -148.996  -191
-59.2006  -190.563  -191
--------------------------------------------------------------------
*/

class UltrasoundCalibrationIO
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

  int RequestWrite();

  UltrasoundCalibration * GetUltrasoundCalibration()
    {
    return m_UltrasoundCalibration;
    }

  void SetUltrasoundCalibration(UltrasoundCalibration * plan)
    {
    m_UltrasoundCalibration = plan;
    }


  UltrasoundCalibrationIO();
  virtual ~UltrasoundCalibrationIO();

private:
  UltrasoundCalibration         * m_UltrasoundCalibration;
  std::string                   m_FileName;

};

} // end of namespace
#endif
