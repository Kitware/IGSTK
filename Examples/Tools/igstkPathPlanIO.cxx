/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkPathPlanIO.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkPathPlanIO.h"
#include <stdio.h>

namespace igstk
{

/** Constructor: Initializes all internal variables. */
PathPlanIO::PathPlanIO()
{
  m_PathPlan = new PathPlan;
  m_FileName      = "";
}

int PathPlanIO::RequestRead()
{

  std::ifstream planFile( m_FileName.c_str() );
  if ( planFile.is_open())
    {
    std::string line;
    float p[3];

  std::getline( planFile, line ); // First line, description, starts with #
    if ( line[0] != '#')
    {
      std::cerr << "Incorrect file format!\n";
      planFile.close();
      return 0;
    }
    std::getline( planFile, line ); // Second line, target point
    if ( sscanf( line.c_str(), "%f %f %f", &p[0], &p[1], &p[2] ) != 3 )
    {
      std::cerr << "Incorrect file format!\n";
      planFile.close();
      return 0;
    }
    m_PathPlan->m_EntryPoint[0] = p[0];
    m_PathPlan->m_EntryPoint[1] = p[1];
    m_PathPlan->m_EntryPoint[2] = p[2];

    std::getline( planFile, line ); // Third line, description, starts with #
    if ( line[0] != '#')
    {
      std::cerr << "Incorrect file format!\n";
      planFile.close();
      return 0;
    }
    std::getline( planFile, line ); // Fourth line, target point
    if ( sscanf( line.c_str(), "%f %f %f", &p[0], &p[1], &p[2] ) != 3 )
    {
      std::cerr << "Incorrect file format!\n";
      planFile.close();
      return 0;
    }
    m_PathPlan->m_TargetPoint[0] = p[0];
    m_PathPlan->m_TargetPoint[1] = p[1];
    m_PathPlan->m_TargetPoint[2] = p[2];

    planFile.close();
    return 1;
    }
  else
    {
    return 0;
    }  
}

int PathPlanIO::RequestWrite()
{
  std::ofstream  planFile;
  planFile.open( m_FileName.c_str(), ios::trunc );
  if ( planFile.is_open())
    {
    std::string line;
    PathPlan::PointType p;
    planFile << "# Entry point\n"; // First line, description
    p = m_PathPlan->m_EntryPoint;
    planFile << p[0] << "\t" << p[1] << "\t" << p[2] << "\n";
    
    planFile << "# Target point\n"; // Third line, description
    p = m_PathPlan->m_TargetPoint;
    planFile << p[0] << "\t" << p[1] << "\t" << p[2] << "\n";

    planFile.close();
    return 1;
    }
  else
    {
    return 0;
    }  
}
/** Destructor */
PathPlanIO::~PathPlanIO()
{

}
} // end of namespace
