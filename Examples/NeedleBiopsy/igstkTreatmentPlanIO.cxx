/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkTreatmentPlanIO.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkTreatmentPlanIO.h"
#include "stdio.h"

namespace igstk
{

/** Constructor: Initializes all internal variables. */
TreatmentPlanIO::TreatmentPlanIO()
{
  m_TreatmentPlan = new TreatmentPlan;
  m_FileName      = "";
}

int TreatmentPlanIO::RequestRead()
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
    std::getline( planFile, line ); // Second line, entry point
    if ( sscanf( line.c_str(), "%f %f %f", &p[0], &p[1], &p[2] ) != 3 )
    {
      std::cerr << "Incorrect file format!\n";
      planFile.close();
      return 0;
    }
    m_TreatmentPlan->m_EntryPoint[0] = p[0];
    m_TreatmentPlan->m_EntryPoint[1] = p[1];
    m_TreatmentPlan->m_EntryPoint[2] = p[2];

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
    m_TreatmentPlan->m_TargetPoint[0] = p[0];
    m_TreatmentPlan->m_TargetPoint[1] = p[1];
    m_TreatmentPlan->m_TargetPoint[2] = p[2];

    std::getline( planFile, line ); // Fifth line, description, starts with #
    if ( line[0] != '#')
      {
      std::cerr << "Incorrect file format!\n";
      planFile.close();
      return 0;
      }
    m_TreatmentPlan->m_FiducialPoints.clear();
    while ( std::getline( planFile, line ) )
      {
      if( sscanf( line.c_str(), "%f %f %f", &p[0], &p[1], &p[2]) != 3 )
        {
        std::cerr << "Incorrect file format!\n";
        planFile.close();
        return 0;
        }
      TreatmentPlan::PointType fp;
      fp[0] = p[0];
      fp[1] = p[1];
      fp[2] = p[2];
      m_TreatmentPlan->m_FiducialPoints.push_back( fp );
      }
    if (m_TreatmentPlan->m_FiducialPoints.size()<3)
      {
      std::cerr << "At least 3 fiducial points needed!\n";
      planFile.close();
      return 0;
      }
    planFile.close();
    return 1;
    }
  else
    {
    return 0;
    }  
}

int TreatmentPlanIO::RequestWrite()
{
  std::ofstream  planFile;
  planFile.open( m_FileName.c_str(), ios::trunc );
  if ( planFile.is_open())
    {
    std::string line;
    TreatmentPlan::PointType p;
    planFile << "# Entry point\n"; // First line, description
    p = m_TreatmentPlan->m_EntryPoint;
    planFile << p[0] << "\t" << p[1] << "\t" << p[2] << "\n";
    
    planFile << "# Target point\n"; // Third line, description
    p = m_TreatmentPlan->m_TargetPoint;
    planFile << p[0] << "\t" << p[1] << "\t" << p[2] << "\n";

    planFile << "# Fiducial points\n"; // Fifth line, description
    for ( unsigned int i=0; i<m_TreatmentPlan->m_FiducialPoints.size(); i++)
      {
      p = m_TreatmentPlan->m_FiducialPoints[i];
      planFile << p[0] << "\t" << p[1] << "\t" << p[2] << "\n";
      }
    planFile.close();
    return 1;
    }
  else
    {
    return 0;
    }  
}
/** Destructor */
TreatmentPlanIO::~TreatmentPlanIO()
{

}
} // end of namespace
