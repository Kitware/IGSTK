/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkArucoConfigurationXMLFileReader.cxx
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

#include "igstkArucoConfigurationXMLFileReader.h"
#include "igstkArucoTrackerConfiguration.h"

namespace igstk
{


void
ArucoConfigurationXMLFileReader::StartElement(
  const char * name,
  const char **atts )
{
  //let the super class try to analyze the current tag
  Superclass::StartElement( name, atts );
  if( itksys::SystemTools::Strucmp( name,
        "camera_calibration_file" ) == 0 ||
      itksys::SystemTools::Strucmp( name, "marker_size" ) == 0 ||
      itksys::SystemTools::Strucmp( name, "tool" ) == 0 )
    {
    if( !m_ReadingTrackerConfiguration )
      throw FileFormatException( "Tag not nested correctly in xml structure." );
    }
}


void
ArucoConfigurationXMLFileReader::EndElement(
  const char *name )
{
  //let the super class try to analyze the current tag
  Superclass::EndElement( name );

  if( m_ReadingTrackerConfiguration &&
      itksys::SystemTools::Strucmp( name, "camera_calibration_file" ) == 0 )
    {
    ProcessArucoCalibrationFile();
    }
  else if( m_ReadingTrackerConfiguration &&
           itksys::SystemTools::Strucmp( name, "marker_size" ) == 0 )
    {
    ProcessArucoMarkerSize();
    this->m_HaveMarkerSize = true;
    }
  else if( m_ReadingToolConfiguration &&
           itksys::SystemTools::Strucmp( name, "marker_ID" ) == 0 )
    {
    ProcessMarkerID();
    this->m_HaveCurrentMarkerID = true;
    }
}


std::string
ArucoConfigurationXMLFileReader::GetSystemType()
{
  return "aruco";
}


double
ArucoConfigurationXMLFileReader::GetMaximalRefreshRate()
{
  igstk::ArucoTrackerConfiguration::Pointer trackerConfig =
    igstk::ArucoTrackerConfiguration::New();

  return trackerConfig->GetMaximalRefreshRate();
}


void
ArucoConfigurationXMLFileReader::ProcessArucoCalibrationFile()
throw ( FileFormatException )
{
  if( !itksys::SystemTools::FileExists( this->m_CurrentTagData.c_str() ) ||
      itksys::SystemTools::FileIsDirectory( this->m_CurrentTagData.c_str()  ) )
    {
    throw FileFormatException(
      "Invalid string given as camera_calibration_file tag" );
    }
  this->m_ArucoCalibrationFile = this->m_CurrentTagData;
}


void
ArucoConfigurationXMLFileReader::ProcessArucoMarkerSize()
throw ( FileFormatException )
{
  double markerSize;
  std::istringstream instr;
  instr.str( this->m_CurrentTagData );
  instr>>markerSize;
  //check that we got to the end of the stream, (the string
  //m_CurrentTagData has no trailing white spaces, trimmed in the
  //EndElement method)
  if( !instr.eof() )
    {
    throw FileFormatException(
      "Error in marker size data, possibly non numeric values" );
    }
  this->m_MarkerSize = markerSize;
}


void
ArucoConfigurationXMLFileReader::ProcessMarkerID()
throw ( FileFormatException )
{
  unsigned int markerID;
  std::istringstream instr;
  instr.str( this->m_CurrentTagData );
  instr>>markerID;
  //check that we got to the end of the stream, (the string
  //m_CurrentTagData has no trailing white spaces, trimmed in the
  //EndElement method)
  if( !instr.eof() )
    {
    throw FileFormatException(
      "Error in marker ID data, possibly non numeric values" );
    }
  this->m_CurrentMarkerID = markerID;
}


void
ArucoConfigurationXMLFileReader::ProcessToolData()
throw ( FileFormatException )
{
  if( this->m_CurrentToolName.empty() )
    {
    throw FileFormatException( "\"name\" missing for one of the tools." );
    }
  if( !this->m_HaveCurrentMarkerID )
    {
    throw FileFormatException( "\"marker_ID\" missing for one of the tools." );
    }

  //if the tool section does not have a "calibration_file" tag
  //we assume the calibration is identity
  igstk::ArucoToolConfiguration *toolConfig =
      new igstk::ArucoToolConfiguration();

  toolConfig->SetToolName( this->m_CurrentToolName );
  toolConfig->SetMarkerID( this->m_CurrentMarkerID );
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
  this->m_HaveCurrentMarkerID = false;
  this->m_CurrentToolCalibration.SetToIdentity(
    igstk::TimeStamp::GetLongestPossibleTime() );
}


bool
ArucoConfigurationXMLFileReader::HaveConfigurationData()
{
  //check that we have a refresh rate for the tracking system, at least
  //one tool
  return ( this->m_HaveRefreshRate &&
           this->m_TrackerToolList.size() != 0 &&
           !this->m_ArucoCalibrationFile.empty() &&
           this->m_HaveMarkerSize );
}


const igstk::TrackerConfiguration::Pointer
ArucoConfigurationXMLFileReader::GetTrackerConfigurationData()
throw ( FileFormatException )
{
  igstk::ArucoTrackerConfiguration::Pointer trackerConfig =
    igstk::ArucoTrackerConfiguration::New();

  //this request is guaranteed to succeed as the refresh rate
  //is validated in the TrackerConfigurationXMLReaderBase
  trackerConfig->RequestSetFrequency( this->m_RefreshRate );

  trackerConfig->SetCameraCalibrationFile( this->m_ArucoCalibrationFile );
  trackerConfig->SetMarkerSize( this->m_MarkerSize );

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
