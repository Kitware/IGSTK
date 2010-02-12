/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkPolarisTrackerConfigurationIO.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkPolarisTrackerConfigurationIO.h"
#include "stdio.h"

namespace igstk
{

/** Constructor: Initializes all internal variables. */
PolarisTrackerConfigurationIO::PolarisTrackerConfigurationIO()
{
  m_PolarisTrackerConfiguration = new NDITrackerConfiguration;
  m_FileName      = "";
}

int PolarisTrackerConfigurationIO::RequestRead()
{

  std::ifstream planFile( m_FileName.c_str() );
  if ( planFile.is_open())
    {
    std::string line;

    std::getline( planFile, line ); // 1 line, description, starts with #
    std::getline( planFile, line ); // 2 line, serial port number
    int com_port;
    if ( sscanf( line.c_str(), "%d", &com_port ) == 1 )
    {
      switch( com_port ) 
      {
      case 0 : 
        m_PolarisTrackerConfiguration->m_COMPort = igstk::SerialCommunication::PortNumber0;
        break;
      case 1 : 
        m_PolarisTrackerConfiguration->m_COMPort = igstk::SerialCommunication::PortNumber1;
        break;
      case 2 : 
        m_PolarisTrackerConfiguration->m_COMPort = igstk::SerialCommunication::PortNumber2;      
        break;
      case 3 : 
        m_PolarisTrackerConfiguration->m_COMPort = igstk::SerialCommunication::PortNumber3;
        break;
      case 4 : 
        m_PolarisTrackerConfiguration->m_COMPort = igstk::SerialCommunication::PortNumber4;
        break;
      case 5 : 
        m_PolarisTrackerConfiguration->m_COMPort = igstk::SerialCommunication::PortNumber5;
        break;
      case 6 : 
        m_PolarisTrackerConfiguration->m_COMPort = igstk::SerialCommunication::PortNumber6;
        break;
      case 7 : 
        m_PolarisTrackerConfiguration->m_COMPort = igstk::SerialCommunication::PortNumber7;
        break;
      default:
        m_PolarisTrackerConfiguration->m_COMPort = igstk::SerialCommunication::PortNumber0;
      }
    }
    else
    {
      std::cerr << "Incorrect file format 0 !\n";
      planFile.close();
      return 0;
    }

    m_PolarisTrackerConfiguration->m_TrackerToolList.clear();

    for (unsigned int i =0; i<2; i++)
    { // Grasper tool
      NDITrackerToolConfiguration * tool = new NDITrackerToolConfiguration;
      std::getline( planFile, line ); // 3 line, description, starts with #
      std::getline( planFile, line ); // 4 line, tool SROM
      tool->m_SROMFile     = line;

      std::getline( planFile, line ); // 5 line, description, starts with #
      std::getline( planFile, line ); // 6 line, tip offset
      float x,y,z;
      if ( sscanf( line.c_str(), "%f\t%f\t%f", &x, &y, &z ) == 3 )
      {
        igstk::Transform toolCalibrationTransform;

        igstk::Transform::VectorType translation;
        translation[0] = x;   // Tip offset
        translation[1] = y;
        translation[2] = z;

        igstk::Transform::VersorType rotation;
        rotation.SetIdentity();
        toolCalibrationTransform.SetTranslationAndRotation( translation, rotation, 0.1, igstk::TimeStamp::GetLongestPossibleTime());

        tool->m_CalibrationTransform = toolCalibrationTransform;
      }
      else
      {
        std::cerr << "Incorrect file format" << i+1 <<"!\n";
        planFile.close();
        return 0;
      }

      tool->m_WiredTool    = 0;
      tool->m_IsReference  = 0;
      tool->m_HasSROM      = 1;
      m_PolarisTrackerConfiguration->m_TrackerToolList.push_back(tool);     
    }    

    planFile.close();
    return 1;
  }
  else
  {
    std::cerr << "Failed to open file\n";
    planFile.close();
    return 0;
  }
}

int PolarisTrackerConfigurationIO::RequestWrite()
{
  std::ofstream  planFile;
  planFile.open( m_FileName.c_str(), ios::trunc );
  if ( planFile.is_open())
    {
    std::string line;
    planFile << "# Serial Port Number\n"; // First line, description
    planFile <<  m_PolarisTrackerConfiguration->m_COMPort << "\n";
    
    //---
    planFile << "# Grasper SROM File\n"; // Third line, description
    planFile << m_PolarisTrackerConfiguration->m_TrackerToolList[0]->m_SROMFile << "\n";

    igstk::Transform::VectorType translation;
    translation = m_PolarisTrackerConfiguration->m_TrackerToolList[0]->m_CalibrationTransform.GetTranslation();
    planFile << "# Tip Offset\n"; // Fifth line, description
    planFile << translation[0] <<"\t"<< translation[1] <<"\t"<< translation[2] << "\n";

    //------

    planFile << "# Scope SROM File\n"; // Third line, description
    planFile << m_PolarisTrackerConfiguration->m_TrackerToolList[1]->m_SROMFile << "\n";

    translation = m_PolarisTrackerConfiguration->m_TrackerToolList[1]->m_CalibrationTransform.GetTranslation();
    planFile << "# Tip Offset\n"; // Fifth line, description
    planFile << translation[0] <<"\t"<< translation[1] <<"\t"<< translation[2] << "\n";

    //---

    planFile.close();
    return 1;
    }
  else
    {
    return 0;
    }  
}
/** Destructor */
PolarisTrackerConfigurationIO::~PolarisTrackerConfigurationIO()
{

}
} // end of namespace
