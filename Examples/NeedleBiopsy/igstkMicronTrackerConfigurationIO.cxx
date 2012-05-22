/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkMicronTrackerConfigurationIO.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkMicronTrackerConfigurationIO.h"
#include "stdio.h"

namespace igstk
{

/** Constructor: Initializes all internal variables. */
MicronTrackerConfigurationIO::MicronTrackerConfigurationIO()
{
  m_MicronTrackerConfiguration = new MicronTrackerConfiguration;
  m_FileName      = "";
}

int MicronTrackerConfigurationIO::RequestRead()
{

  std::ifstream planFile( m_FileName.c_str() );
  if ( planFile.is_open())
    {
    std::string line;

    std::getline( planFile, line ); // First line, description, starts with #
    std::getline( planFile, line ); // Second line, calibration file
    m_MicronTrackerConfiguration->m_CameraCalibrationFileDirectory = line;

    std::getline( planFile, line ); // Third line, description, starts with #
    std::getline( planFile, line ); // Fourth line, ini file
    m_MicronTrackerConfiguration->m_InitializationFile = line;

    std::getline( planFile, line ); // Fifth line, description, starts with #
    std::getline( planFile, line ); // Sixth line, marker dir
    m_MicronTrackerConfiguration->m_TemplatesDirectory = line;
    
    m_MicronTrackerConfiguration->m_TrackerToolList.clear();
    std::getline( planFile, line ); // Seventh line, description, starts with #
    std::getline( planFile, line ); // Eighth line, marker name
    MicronTrackerToolConfiguration * tool = new MicronTrackerToolConfiguration;
    tool->m_MarkerName = line;
    tool->m_IsReference = 0;
    m_MicronTrackerConfiguration->m_TrackerToolList.push_back(tool);

    std::getline( planFile, line ); // Nineth line, description, starts with #
    std::getline( planFile, line ); // 10th line, use reference tool?
    int use_ref_tool;
    if ( sscanf( line.c_str(), "%d", &use_ref_tool ) == 1 )
      {
      if (use_ref_tool)
        {
        std::getline( planFile, line ); // 11 line, description, starts with #
        std::getline( planFile, line ); // 12 line, marker name
        MicronTrackerToolConfiguration * micronTool = new MicronTrackerToolConfiguration;
        micronTool->m_MarkerName = line;
        micronTool->m_IsReference = 1;
        m_MicronTrackerConfiguration->m_TrackerToolList.push_back(micronTool);
        }
      }
    else
      {
      std::cerr << "Incorrect file format!\n";
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

int MicronTrackerConfigurationIO::RequestWrite()
{
  std::ofstream  planFile;
  planFile.open( m_FileName.c_str(), ios::trunc );
  if ( planFile.is_open())
    {
    std::string line;
    planFile << "# Calibration file dir\n"; // First line, description
    planFile <<  m_MicronTrackerConfiguration->m_CameraCalibrationFileDirectory << "\n";
    
    planFile << "# Ini file\n"; // Third line, description
    planFile << m_MicronTrackerConfiguration->m_InitializationFile << "\n";

    planFile << "# Marker dir\n"; // Fifth line, description
    planFile << m_MicronTrackerConfiguration->m_TemplatesDirectory << "\n";

    planFile << "# Tracker tool marker name\n"; // Fifth line, description
    planFile << m_MicronTrackerConfiguration->m_TrackerToolList[0]->m_MarkerName << "\n";

    if ( m_MicronTrackerConfiguration->m_TrackerToolList.size() == 2)
      {
      planFile << "# Use reference tool?\n"; // Fifth line, description
      planFile << "1\n";
      planFile << "# Reference tool marker name\n"; // Fifth line, description
      planFile << m_MicronTrackerConfiguration->m_TrackerToolList[1]->m_MarkerName << "\n";
      }
    else
      {
      planFile << "# Use reference tool?\n"; // Fifth line, description
      planFile << "0\n";
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
MicronTrackerConfigurationIO::~MicronTrackerConfigurationIO()
{

}
} // end of namespace
