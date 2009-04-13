/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAscensionConfigurationXMLFileReader.cxx
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

#include "igstkAscensionConfigurationXMLFileReader.h"
#include "igstkAscensionTrackerConfiguration.h"

namespace igstk
{


void 
AscensionConfigurationXMLFileReader::StartElement( 
  const char * name, 
  const char **atts )
{
  //let the super class try to analyze the current tag
  Superclass::StartElement( name, atts );
                          
  if( itksys::SystemTools::Strucmp( name, "srom_file" ) == 0 ||
      itksys::SystemTools::Strucmp( name, "bird_port" ) == 0 ) 
    {
    if( !m_ReadingToolConfiguration )
      throw FileFormatException( "Tag not nested correctly in xml structure." );
    }

}


void 
AscensionConfigurationXMLFileReader::EndElement( 
  const char *name )
{
  //let the super class try to analyze the current tag
  Superclass::EndElement( name );
           
  if( m_ReadingToolConfiguration &&
      itksys::SystemTools::Strucmp( name, "bird_port" ) == 0 )
    {
    ProcessBirdPort();
    this->m_HaveCurrentBirdPort = true;  
    }
}

void 
AscensionConfigurationXMLFileReader::ProcessBirdPort() 
throw ( FileFormatException )
{
  unsigned birdPort;
  std::istringstream instr;  
  instr.str( this->m_CurrentTagData );
  instr>>birdPort;

  if( !instr.eof() )
    {
    throw FileFormatException( 
           "Error in bird port tag data, possibly non numeric values." );
    }
  if( birdPort<1 || birdPort>4 )
    {
    throw FileFormatException("Invalid value given for bird port."); 
    }
  this->m_CurrentBirdPort = birdPort;
}


void 
AscensionConfigurationXMLFileReader::ProcessToolData() 
  throw ( FileFormatException )
{
  if( this->m_CurrentToolName.empty() )
    throw FileFormatException( "\"name\" missing for one of the tools." );
  if( !this->m_HaveCurrentBirdPort )
    throw FileFormatException( "\"bird_port\" missing for one of the tools." );

  //if the tool section does not have a "calibration_file" tag 
  //we assume the calibration is identity
  igstk::AscensionToolConfiguration *toolConfig = 
    new igstk::AscensionToolConfiguration();

  toolConfig->SetToolName( this->m_CurrentToolName );
  toolConfig->SetCalibrationTransform( this->m_CurrentToolCalibration );
  toolConfig->SetPortNumber( this->m_CurrentBirdPort );
    
  if( this->m_CurrentToolIsReference )
    {
    this->m_ReferenceTool = toolConfig;
    }
  else
    {
    this->m_TrackerToolList.push_back( toolConfig );
    }

  //reset all tool data to initial state
  this->m_CurrentToolIsReference = false;
  this->m_CurrentToolName.clear();
  this->m_CurrentToolCalibration.SetToIdentity( 
    igstk::TimeStamp::GetLongestPossibleTime() );
  this->m_HaveCurrentBirdPort = false;
}


bool 
AscensionConfigurationXMLFileReader::HaveConfigurationData()
{
  //check that we have a refresh rate for the tracking system, at least 
  //one tool, and all the serial communication settings
  return ( this->m_HaveRefreshRate &&
           this->m_TrackerToolList.size() != 0 &&
           this->m_HaveComPort && this->m_HaveBaudRate && 
           this->m_HaveDataBits && this->m_HaveParity && 
           this->m_HaveStopBits && this->m_HaveHandshake );
}


const igstk::TrackerConfiguration::Pointer 
AscensionConfigurationXMLFileReader::GetTrackerConfigurationData()
throw ( FileFormatException )
{
  igstk::AscensionTrackerConfiguration::Pointer trackerConfig = 
    AscensionTrackerConfiguration::New();
        
  //this request is guaranteed to succeed as the refresh rate
  //is validated in the TrackerConfigurationXMLReaderBase
  trackerConfig->RequestSetFrequency( this->m_RefreshRate );
  //the communication settings requests are guaranteed to
  //succeed as they are validated in the 
  //SerialCommunicatingTrackerConfigurationXMLFileReader
  trackerConfig->RequestSetCOMPort( this->m_COMPort );
  trackerConfig->RequestSetBaudRate( this->m_BaudRate );
  trackerConfig->RequestSetDataBits( this->m_DataBits );
  trackerConfig->RequestSetParity( this->m_Parity );
  trackerConfig->RequestSetStopBits( this->m_StopBits );
  trackerConfig->RequestSetHandshake( this->m_Handshake );

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
  igstk::TrackerConfiguration::Pointer genericTrackerConfig;
  genericTrackerConfig = trackerConfig;
  return genericTrackerConfig;
}


std::string 
AscensionConfigurationXMLFileReader::GetSystemType()
{
  return "ascension";
}


double 
AscensionConfigurationXMLFileReader::GetMaximalRefreshRate()
{
  igstk::AscensionTrackerConfiguration::Pointer trackerConfig =
    igstk::AscensionTrackerConfiguration::New();

  return trackerConfig->GetMaximalRefreshRate();
}


} //namespace
