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
      return FALSE;
    }
    std::getline( planFile, line ); // Second line, entry point
    if ( sscanf( line.c_str(), "%f %f %f", &p[0], &p[1], &p[2] ) != 3 )
    {
      std::cerr << "Incorrect file format!\n";
      planFile.close();
      return FALSE;
    }
    m_TreatmentPlan->EntryPoint[0] = p[0];
    m_TreatmentPlan->EntryPoint[1] = p[1];
    m_TreatmentPlan->EntryPoint[2] = p[2];

    std::getline( planFile, line ); // Third line, description, starts with #
    if ( line[0] != '#')
    {
      std::cerr << "Incorrect file format!\n";
      planFile.close();
      return FALSE;
    }
    std::getline( planFile, line ); // Fourth line, target point
    if ( sscanf( line.c_str(), "%f %f %f", &p[0], &p[1], &p[2] ) != 3 )
    {
      std::cerr << "Incorrect file format!\n";
      planFile.close();
      return FALSE;
    }
    m_TreatmentPlan->TargetPoint[0] = p[0];
    m_TreatmentPlan->TargetPoint[1] = p[1];
    m_TreatmentPlan->TargetPoint[2] = p[2];

    std::getline( planFile, line ); // Fifth line, description, starts with #
    if ( line[0] != '#')
    {
      std::cerr << "Incorrect file format!\n";
      planFile.close();
      return FALSE;
    }
    m_TreatmentPlan->FiducialPoints.clear();
    while ( std::getline( planFile, line ) )
    {
      if (sscanf( line.c_str(), "%f %f %f", &p[0], &p[1], &p[2]) !=3 )
      {
        std::cerr << "Incorrect file format!\n";
        planFile.close();
        return FALSE;
      }
      TreatmentPlan::PointType fp;
      fp[0] = p[0];
      fp[1] = p[1];
      fp[2] = p[2];
      m_TreatmentPlan->FiducialPoints.push_back( fp );
    }
    if (m_TreatmentPlan->FiducialPoints.size()<3)
    {
      std::cerr << "At least 3 fiducial points needed!\n";
      planFile.close();
      return FALSE;
    }
    planFile.close();
    return TRUE;
  }
  else
  {
    return FALSE;
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
    p = m_TreatmentPlan->EntryPoint;
    planFile << p[0] << "\t" << p[1] << "\t" << p[2] << "\n";
    
    planFile << "# Target point\n"; // Third line, description
    p = m_TreatmentPlan->TargetPoint;
    planFile << p[0] << "\t" << p[1] << "\t" << p[2] << "\n";

    planFile << "# Fiducial points\n"; // Fifth line, description
    for ( int i=0; i<m_TreatmentPlan->FiducialPoints.size(); i++)
    {
      p = m_TreatmentPlan->FiducialPoints[i];
      planFile << p[0] << "\t" << p[1] << "\t" << p[2] << "\n";
    }
    planFile.close();
    return TRUE;
  }
  else
  {
    return FALSE;
  }  
}
/** Destructor */
TreatmentPlanIO::~TreatmentPlanIO()
{

}
} // end of namespace
