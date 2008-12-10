/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkFiducialsPlanIO.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkFiducialsPlanIO.h"
#include "stdio.h"

namespace igstk
{

/** Constructor: Initializes all internal variables. */
FiducialsPlanIO::FiducialsPlanIO()
{
  m_FiducialsPlan = new FiducialsPlan;
  m_FileName      = "";
}

int FiducialsPlanIO::RequestRead()
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
    m_FiducialsPlan->m_FiducialPoints.clear();
    while ( std::getline( planFile, line ) )
      {
      if( sscanf( line.c_str(), "%f %f %f", &p[0], &p[1], &p[2]) != 3 )
        {
        std::cerr << "Incorrect file format!\n";
        planFile.close();
        return 0;
        }
      FiducialsPlan::PointType fp;
      fp[0] = p[0];
      fp[1] = p[1];
      fp[2] = p[2];
      m_FiducialsPlan->m_FiducialPoints.push_back( fp );
      }
    if (m_FiducialsPlan->m_FiducialPoints.size()<3)
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

int FiducialsPlanIO::RequestWrite()
{
  std::ofstream  planFile;
  planFile.open( m_FileName.c_str(), ios::trunc );
  if ( planFile.is_open())
    {
    std::string line;
    FiducialsPlan::PointType p;

    planFile << "# Fiducial points\n"; // First line, description
    for ( unsigned int i=0; i<m_FiducialsPlan->m_FiducialPoints.size(); i++)
      {
      p = m_FiducialsPlan->m_FiducialPoints[i];
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
FiducialsPlanIO::~FiducialsPlanIO()
{

}
} // end of namespace
