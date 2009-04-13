/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMicronConfigurationXMLFileReader.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <itksys/SystemTools.hxx>

#include "igstkMicronConfigurationXMLFileReader.h"
#include "igstkMicronTrackerConfiguration.h"

namespace igstk
{


void 
MicronConfigurationXMLFileReader::StartElement( 
  const char * name, 
  const char **atts )
{
  //let the super class try to analyze the current tag
  Superclass::StartElement( name, atts );
  if( itksys::SystemTools::Strucmp( name, 
        "camera_calibration_directory" ) == 0 ||
      itksys::SystemTools::Strucmp( name, "initialization_file" ) == 0 ||
      itksys::SystemTools::Strucmp( name, "templates_directory" ) == 0 ||
      itksys::SystemTools::Strucmp( name, "tool" ) == 0 ) 
    {
    if( !m_ReadingTrackerConfiguration )
      throw FileFormatException( "Tag not nested correctly in xml structure." );
    }  
}


void 
MicronConfigurationXMLFileReader::EndElement( 
  const char *name )
{
  //let the super class try to analyze the current tag
  Superclass::EndElement( name );
  
         
  if( m_ReadingTrackerConfiguration &&
      itksys::SystemTools::Strucmp( name, "camera_calibration_directory" ) == 0 )
    {
    ProcessMicronCalibrationDirectory();
    }
  else if( m_ReadingTrackerConfiguration &&
           itksys::SystemTools::Strucmp( name, "initialization_file" ) == 0 )
    {
    ProcessMicronInitializationFile();
    }
  else if( m_ReadingTrackerConfiguration &&
           itksys::SystemTools::Strucmp( name, "templates_directory" ) == 0 )
    {
    ProcessMicronTemplatesDirectory();
    }
  else if( m_ReadingToolConfiguration &&
           itksys::SystemTools::Strucmp( name, "marker_name" ) == 0 )
    {
    ProcessMarkerName();
    }
}


std::string 
MicronConfigurationXMLFileReader::GetSystemType()
{
  return "micron";
}


double 
MicronConfigurationXMLFileReader::GetMaximalRefreshRate()
{
  igstk::MicronTrackerConfiguration::Pointer trackerConfig =
    igstk::MicronTrackerConfiguration::New();

  return trackerConfig->GetMaximalRefreshRate();
}


void 
MicronConfigurationXMLFileReader::ProcessMicronCalibrationDirectory() 
throw ( FileFormatException )
{
  if( !itksys::SystemTools::FileExists( this->m_CurrentTagData.c_str() ) ||
    !itksys::SystemTools::FileIsDirectory( this->m_CurrentTagData.c_str()  ) )
    {
    throw FileFormatException( 
      "Invalid string given as camera_calibration_directory tag" );
    }
  this->m_MicronCalibrationDirectory = this->m_CurrentTagData;
}


void 
MicronConfigurationXMLFileReader::ProcessMicronInitializationFile() 
throw ( FileFormatException )
{
  if( !itksys::SystemTools::FileExists( this->m_CurrentTagData.c_str() ) ||
      itksys::SystemTools::FileIsDirectory( this->m_CurrentTagData.c_str()  ) )
    {
    throw FileFormatException( 
      "Invalid string given as initialization_file tag" );
    }
  this->m_MicronInitializationFile = this->m_CurrentTagData;
}


void 
MicronConfigurationXMLFileReader::ProcessMicronTemplatesDirectory() 
throw ( FileFormatException )
{
  if( !itksys::SystemTools::FileExists( this->m_CurrentTagData.c_str() ) ||
      !itksys::SystemTools::FileIsDirectory( this->m_CurrentTagData.c_str()  ) )
    {
    throw FileFormatException( 
      "Invalid string given as templates_directory tag" );
    }
  this->m_MicronTemplatesDirectory = this->m_CurrentTagData;
}

void 
MicronConfigurationXMLFileReader::ProcessMarkerName() 
throw ( FileFormatException )
{
  this->m_CurrentMarkerName = this->m_CurrentTagData;
}


void 
MicronConfigurationXMLFileReader::ProcessToolData() 
throw ( FileFormatException )
{
  if( this->m_CurrentToolName.empty() )
    {
    throw FileFormatException( "\"name\" missing for one of the tools." );
    }
  if( this->m_CurrentMarkerName.empty() )
    {
    throw FileFormatException( "\"marker_name\" missing for one of the tools." );
    }

  //if the tool section does not have a "calibration_file" tag 
  //we assume the calibration is identity
  igstk::MicronToolConfiguration *toolConfig = 
      new igstk::MicronToolConfiguration();
  
  toolConfig->SetToolName( this->m_CurrentToolName );
  toolConfig->SetMarkerName( this->m_CurrentMarkerName );
  toolConfig->SetCalibrationTransform( this->m_CurrentToolCalibration );  
  if( this->m_CurrentToolIsReference )
    {
    this->m_ReferenceTool = toolConfig;
    }
  else
    {
    this->m_TrackerToolList.push_back( toolConfig );
    }

  //reset all tool data to initial state
  this->m_CurrentToolName.clear();
  this->m_CurrentMarkerName.clear();
  this->m_CurrentToolCalibration.SetToIdentity( 
    igstk::TimeStamp::GetLongestPossibleTime() );
}


bool 
MicronConfigurationXMLFileReader::HaveConfigurationData()
{
  //check that we have a refresh rate for the tracking system, at least 
  //one tool
  return ( this->m_HaveRefreshRate &&
           this->m_TrackerToolList.size() != 0 &&
           !this->m_MicronCalibrationDirectory.empty() &&
           !this->m_MicronInitializationFile.empty() &&
           !this->m_MicronTemplatesDirectory.empty() );
}


const igstk::TrackerConfiguration::Pointer
MicronConfigurationXMLFileReader::GetTrackerConfigurationData()
throw ( FileFormatException )
{
  igstk::MicronTrackerConfiguration::Pointer trackerConfig = 
    igstk::MicronTrackerConfiguration::New();
    
  //this request is guaranteed to succeed as the refresh rate
  //is validated in the TrackerConfigurationXMLReaderBase
  trackerConfig->RequestSetFrequency( this->m_RefreshRate );
  
  trackerConfig->SetCameraCalibrationFileDirectory( 
    this->m_MicronCalibrationDirectory );
  trackerConfig->SetInitializationFile( this->m_MicronInitializationFile );
  trackerConfig->SetTemplatesDirectory( this->m_MicronTemplatesDirectory );

  //add the tools
  std::vector<TrackerToolConfiguration *>::iterator it, end; 
  it = this->m_TrackerToolList.begin();
  end = this->m_TrackerToolList.end();  
  
  AddToolFailureObserver::Pointer failureObserver =
    AddToolFailureObserver::New();
  trackerConfig->AddObserver( igstk::TrackerConfiguration::AddToolFailureEvent(),
                              failureObserver );
  for(; it!=end; it++ )
    {
    trackerConfig->RequestAddTool( *it );
    if( failureObserver->GotAddToolFailure() )
      {
      throw FileFormatException( failureObserver->GetAddToolFailure() );
      }
    }
  if( this->m_ReferenceTool != NULL )
    {
    trackerConfig->RequestAddReference( this->m_ReferenceTool );
    if( failureObserver->GotAddToolFailure() )
      {
      throw FileFormatException( failureObserver->GetAddToolFailure() );
      }
    }
 
  //explicitly upcast to avoid the compiler warning
  igstk::TrackerConfiguration::Pointer genericConfig;
  genericConfig  = trackerConfig;
  return genericConfig;
}

} //namespace
