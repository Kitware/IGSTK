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
    std::cout << "Read file\n";
    std::string line;
    TreatmentPlan::PointType p;

    std::getline( planFile, line ); // First line, description
    std::getline( planFile, line ); // Second line, entry point
    sscanf( line.c_str(), "%f %f %f", &p[0], &p[1], &p[2] );
    m_TreatmentPlan->EntryPoint = p;

    std::getline( planFile, line ); // Third line, description
    std::getline( planFile, line ); // Fourth line, target point
    sscanf( line.c_str(), "%f %f %f", &p[0], &p[1], &p[2] );
    m_TreatmentPlan->TargetPoint = p;

    std::getline( planFile, line ); // Fifth line, description
    unsigned int count = 0;
    m_TreatmentPlan->FiducialPoints.clear();
    while ( std::getline( planFile, line ) )
    {
      sscanf( line.c_str(), "%f %f %f", &p[0], &p[1], &p[2]);
      m_TreatmentPlan->FiducialPoints.push_back( p );
    }
    planFile.close();
    return EXIT_SUCCESS;
  }
  else
  {
    return EXIT_FAILURE;
  }  
}

int TreatmentPlanIO::RequestWrite()
{
  std::ofstream  planFile;
  planFile.open( m_FileName.c_str(), ios::trunc );
  if ( planFile.is_open())
  {
    std::cout << "Write file: Open\n";
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
    return EXIT_SUCCESS;
  }
  else
  {
    return EXIT_FAILURE;
  }  
}
/** Destructor */
TreatmentPlanIO::~TreatmentPlanIO()
{

}
} // end of namespace
