/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkCertusConfigurationXMLFileReader.cxx,v $
  Language:  C++
  Date:      $Date: 2012-01-27 18:20:00 $
  Version:   $Revision: 1.1 $

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <itksys/SystemTools.hxx>

#include "igstkCertusConfigurationXMLFileReader.h"
#include "igstkCertusTrackerConfiguration.h"

namespace igstk
{


void 
CertusConfigurationXMLFileReader::StartElement( 
  const char * name, 
  const char **atts )
{
  //let the super class try to analyze the current tag
  Superclass::StartElement( name, atts );
  if( itksys::SystemTools::Strucmp( name, "setup_file" ) == 0 ||
      itksys::SystemTools::Strucmp( name, "rigid_body_cfg_file" ) == 0 ) 
    {
    if( !m_ReadingTrackerConfiguration )
      throw FileFormatException( "Tag not nested correctly in xml structure." );
    }  
}


void 
CertusConfigurationXMLFileReader::EndElement( 
  const char *name )
{
  //let the super class try to analyze the current tag
  Superclass::EndElement( name );
  
         
  if( m_ReadingTrackerConfiguration &&
      itksys::SystemTools::Strucmp( name, "setup_file" ) == 0 )
    {
    ProcessCertusSetupFile();
    }
  else if( m_ReadingTrackerConfiguration &&
           itksys::SystemTools::Strucmp( name, "number_of_rigid_bodies" ) == 0 )
    {
    ProcessNumberOfRigidBodies();
    }
  else if( m_ReadingToolConfiguration &&
           itksys::SystemTools::Strucmp( name, "rigid_body_name" ) == 0 )
    {
    ProcessRigidBodyName();
    }
  else if( m_ReadingToolConfiguration &&
           itksys::SystemTools::Strucmp( name, "start_marker" ) == 0 )
    {
    ProcessStartMarker();
    }
  else if( m_ReadingToolConfiguration &&
           itksys::SystemTools::Strucmp( name, "number_of_markers" ) == 0 )
    {
    ProcessNumberOfMarkers();
    }
}


std::string 
CertusConfigurationXMLFileReader::GetSystemType()
{
  return "certus";
}


double 
CertusConfigurationXMLFileReader::GetMaximalRefreshRate()
{
  igstk::CertusTrackerConfiguration::Pointer trackerConfig =
    igstk::CertusTrackerConfiguration::New();

  return trackerConfig->GetMaximalRefreshRate();
}


void 
CertusConfigurationXMLFileReader::ProcessCertusSetupFile() 
throw ( FileFormatException )
{
  if( !itksys::SystemTools::FileExists( this->m_CurrentTagData.c_str() ) ||
    itksys::SystemTools::FileIsDirectory( this->m_CurrentTagData.c_str()  ) )
    {
    throw FileFormatException( 
      "Invalid string given as camera_calibration_directory tag" );
    }
  this->m_SetupFile = this->m_CurrentTagData;
}

void 
CertusConfigurationXMLFileReader::ProcessNumberOfRigidBodies() 
throw ( FileFormatException )
{
  this->m_NumberOfRigidBodies = this->m_CurrentTagData;
}

void 
CertusConfigurationXMLFileReader::ProcessRigidBodyName() 
throw ( FileFormatException )
{
  this->m_CurrentRigidBodyName = this->m_CurrentTagData;
}

void 
CertusConfigurationXMLFileReader::ProcessStartMarker() 
throw ( FileFormatException )
{
  this->m_CurrentStartMarker = this->m_CurrentTagData;
}

void 
CertusConfigurationXMLFileReader::ProcessNumberOfMarkers() 
throw ( FileFormatException )
{
  this->m_CurrentNumberOfMarkers = this->m_CurrentTagData;
}




void 
CertusConfigurationXMLFileReader::ProcessToolData() 
throw ( FileFormatException )
{
  if( this->m_CurrentToolName.empty() )	
    {
    throw FileFormatException( "\"name\" missing for one of the tools." );
    }
  if( this->m_CurrentRigidBodyName.empty() )	
    {
    throw FileFormatException( "\"Rigid body name\" missing for one of the tools." );
    }
  if( this->m_CurrentStartMarker.empty() )	
    {
    throw FileFormatException( "\"Start marker\" missing for one of the tools." );
    }
  if( this->m_CurrentNumberOfMarkers.empty() )	
    {
    throw FileFormatException( "\"Number of markers\" missing for one of the tools." );
    }
  
  igstk::CertusToolConfiguration *toolConfig = 
      new igstk::CertusToolConfiguration();

  toolConfig->SetToolName( this->m_CurrentToolName );
  toolConfig->SetRigidBodyName( this->m_CurrentRigidBodyName );
  toolConfig->SetStartMarker( this->m_CurrentStartMarker );
  toolConfig->SetNumberOfMarkers( this->m_CurrentNumberOfMarkers );
  this->m_TrackerToolList.push_back( toolConfig );

 //reset all tool data to initial state
  this->m_CurrentRigidBodyName.clear();
  this->m_CurrentStartMarker.clear();
  this->m_CurrentNumberOfMarkers.clear();
 
  this->m_CurrentToolCalibration.SetToIdentity( igstk::TimeStamp::GetLongestPossibleTime() );

}


bool 
CertusConfigurationXMLFileReader::HaveConfigurationData()
{
  //check that we have a refresh rate for the tracking system, at least 
  //one tool
  return ( this->m_HaveRefreshRate &&
           !this->m_SetupFile.empty() &&
           !this->m_NumberOfRigidBodies.empty() );
}


const igstk::TrackerConfiguration::Pointer
CertusConfigurationXMLFileReader::GetTrackerConfigurationData()
throw ( FileFormatException )
{
  igstk::CertusTrackerConfiguration::Pointer trackerConfig = 
    igstk::CertusTrackerConfiguration::New();
    
  //this request is guaranteed to succeed as the refresh rate
  //is validated in the TrackerConfigurationXMLReaderBase
  trackerConfig->RequestSetFrequency( this->m_RefreshRate );
  
  trackerConfig->SetSetupFile( this->m_SetupFile );
  trackerConfig->SetNumberOfRigidBodies( this->m_NumberOfRigidBodies );
  
  //
  ////add the tools
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
