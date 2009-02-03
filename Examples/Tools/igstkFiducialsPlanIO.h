/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkFiducialsPlanIO.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkFiducialsPlanIO_h
#define __igstkFiducialsPlanIO_h


#include "igstkFiducialsPlan.h"

namespace igstk
{

/** \class FiducialsPlanIO
* 
* \brief Reader/Writer class for FiducialsPlan
*
* Sample format:
---------------------------------------------------------------------
# Fiducial points
98.4887   -152.976  -181
-1.89214  -148.996  -191
-59.2006  -190.563  -191
--------------------------------------------------------------------
*/

class FiducialsPlanIO
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

  FiducialsPlan * GetFiducialsPlan()
    {
    return m_FiducialsPlan;
    }

  void SetFiducialsPlan(FiducialsPlan * plan)
    {
    m_FiducialsPlan = plan;
    }

  FiducialsPlanIO();
  virtual ~FiducialsPlanIO();

private:
  FiducialsPlan         * m_FiducialsPlan;
  std::string             m_FileName;

};

} // end of namespace
#endif
