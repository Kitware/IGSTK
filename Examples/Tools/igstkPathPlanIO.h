/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkPathPlanIO.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkPathPlanIO_h
#define __igstkPathPlanIO_h


#include "igstkPathPlan.h"

namespace igstk
{

/** \class PathPlanIO
* 
* \brief Reader/Writer class for PathPlan
*
* Sample format:
---------------------------------------------------------------------
# Entry point
0.820425  -143.635  -186
# Target point
54.268    -108.513  -191
# Fiducial points
98.4887   -152.976  -181
-1.89214  -148.996  -191
-59.2006  -190.563  -191
--------------------------------------------------------------------
*/

class PathPlanIO
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

  PathPlan * GetPathPlan()
    {
    return m_PathPlan;
    }

  void SetPathPlan(PathPlan * plan)
    {
    m_PathPlan = plan;
    }

  PathPlanIO();
  virtual ~PathPlanIO();

private:
  PathPlan         * m_PathPlan;
  std::string        m_FileName;

};

} // end of namespace
#endif
