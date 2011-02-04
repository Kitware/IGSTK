/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAscension3DGConfigurationXMLFileReader.cxx
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

#include "igstkAscension3DGConfigurationXMLFileReader.h"
#include "igstkAscension3DGTrackerConfiguration.h"

namespace igstk
{


void 
Ascension3DGConfigurationXMLFileReader::StartElement( 
  const char * name, 
  const char **atts )
{

  //let the super class try to analyze the current tag
  Superclass::StartElement( name, atts );
  if( itksys::SystemTools::Strucmp( name, 
        "control_box_port" ) == 0 ) 
    {
    if( !m_ReadingTrackerConfiguration )
      throw FileFormatException( "Tag not nested correctly in xml structure." );
    }  
}


void 
Ascension3DGConfigurationXMLFileReader::EndElement( 
  const char *name )
{
  //let the super class try to analyze the current tag
  Superclass::EndElement( name );
  
         
  if( m_ReadingTrackerConfiguration &&
      itksys::SystemTools::Strucmp( name, "control_box_port" ) == 0 )
    {
    ProcessPortNumber();
    }
}


std::string 
Ascension3DGConfigurationXMLFileReader::GetSystemType()
{
  return "Ascension3DG";
}


double 
Ascension3DGConfigurationXMLFileReader::GetMaximalRefreshRate()
{
  igstk::Ascension3DGTrackerConfiguration::Pointer trackerConfig =
    igstk::Ascension3DGTrackerConfiguration::New();

  return trackerConfig->GetMaximalRefreshRate();
}

void 
Ascension3DGConfigurationXMLFileReader::ProcessPortNumber() 
throw ( FileFormatException )
{
  int controlBoxPort;
  std::istringstream instr;  
  instr.str( this->m_CurrentTagData );
  instr>>controlBoxPort;

  this->m_CurrentPortNumber = controlBoxPort;
}


void 
Ascension3DGConfigurationXMLFileReader::ProcessToolData() 
throw ( FileFormatException )
{
  if( this->m_CurrentToolName.empty() )
    {
    throw FileFormatException( "\"name\" missing for one of the tools." );
    }
  
  //if the tool section does not have a "calibration_file" tag 
  //we assume the calibration is identity
  igstk::Ascension3DGToolConfiguration *toolConfig = 
      new igstk::Ascension3DGToolConfiguration();
  
  toolConfig->SetToolName( this->m_CurrentToolName );
  toolConfig->SetPortNumber( this->m_CurrentPortNumber );
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
  this->m_CurrentPortNumber = -1;
  this->m_CurrentToolCalibration.SetToIdentity( 
    igstk::TimeStamp::GetLongestPossibleTime() );
}


bool 
Ascension3DGConfigurationXMLFileReader::HaveConfigurationData()
{
  //check that we have a refresh rate for the tracking system, at least 
  //one tool
  return ( this->m_HaveRefreshRate &&
           this->m_TrackerToolList.size() != 0 );
}


const igstk::TrackerConfiguration::Pointer
Ascension3DGConfigurationXMLFileReader::GetTrackerConfigurationData()
throw ( FileFormatException )
{
  igstk::Ascension3DGTrackerConfiguration::Pointer trackerConfig = 
    igstk::Ascension3DGTrackerConfiguration::New();
    
  //this request is guaranteed to succeed as the refresh rate
  //is validated in the TrackerConfigurationXMLReaderBase
  trackerConfig->RequestSetFrequency( this->m_RefreshRate );
  
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
