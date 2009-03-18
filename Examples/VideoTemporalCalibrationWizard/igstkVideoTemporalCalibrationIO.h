/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkVideoTemporalCalibrationIO.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkVideoTemporalCalibrationIO_h
#define __igstkVideoTemporalCalibrationIO_h


#include "igstkVideoTemporalCalibration.h"

namespace igstk
{

/** \class VideoTemporalCalibrationIO
* 
* \brief Reader/Writer class for VideoTemporalCalibration
*
* Sample format:
---------------------------------------------------------------------
# Fiducial points
98.4887   -152.976  -181
-1.89214  -148.996  -191
-59.2006  -190.563  -191
--------------------------------------------------------------------
*/

class VideoTemporalCalibrationIO
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

  VideoTemporalCalibration * GetVideoTemporalCalibration()
    {
    return m_VideoTemporalCalibration;
    }

  void SetVideoTemporalCalibration(VideoTemporalCalibration * plan)
    {
    m_VideoTemporalCalibration = plan;
    }


  VideoTemporalCalibrationIO();
  virtual ~VideoTemporalCalibrationIO();

private:
  VideoTemporalCalibration         * m_VideoTemporalCalibration;
  std::string                      m_FileName;

};

} // end of namespace
#endif
