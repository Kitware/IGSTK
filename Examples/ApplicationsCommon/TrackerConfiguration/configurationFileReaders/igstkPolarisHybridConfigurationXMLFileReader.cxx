#include <iostream>
#include <sstream>
#include <itksys/SystemTools.hxx>

#include "igstkPolarisHybridConfigurationXMLFileReader.h"
#include "igstkPolarisTrackerConfiguration.h"

namespace igstk
{


void 
PolarisHybridConfigurationXMLFileReader::StartElement( 
  const char * name, 
  const char **atts )
{
          //let the super class try to analyze the current tag
  Superclass::StartElement( name, atts );
                       
  if( itksys::SystemTools::Strucmp( name, "srom_file" ) == 0 ||
    itksys::SystemTools::Strucmp( name, "control_box_port" ) == 0 ) 
  {
    if( !m_ReadingToolConfiguration )
      throw FileFormatException( "Tag not nested correctly in xml structure." );
  }

}


void 
PolarisHybridConfigurationXMLFileReader::EndElement( 
  const char *name )
{
          //let the super class try to analyze the current tag
  Superclass::EndElement( name );
  
         
  if( m_ReadingToolConfiguration &&
    itksys::SystemTools::Strucmp( name, "srom_file" ) == 0 )
  {
    ProcessSromFile();
  }
  if( m_ReadingToolConfiguration &&
    itksys::SystemTools::Strucmp( name, "control_box_port" ) == 0 )
  {
    ProcessControlBoxPort();
    this->m_HaveCurrentControlBoxPort = true;  
  }
}


std::string 
PolarisHybridConfigurationXMLFileReader::GetSystemType()
{
  return "polaris hybrid";
}


double 
PolarisHybridConfigurationXMLFileReader::GetMaximalRefreshRate()
{
  igstk::PolarisHybridTrackerConfiguration trackerConfig;

  return trackerConfig.GetMaximalRefreshRate();
}


void 
PolarisHybridConfigurationXMLFileReader::ProcessSromFile() 
  throw ( FileFormatException )
{
  this->m_CurrentSromFileName = this->m_CurrentTagData;
}


void 
PolarisHybridConfigurationXMLFileReader::ProcessControlBoxPort() 
  throw ( FileFormatException )
{
  unsigned controlBoxPort;
  std::istringstream instr;  
  instr.str( this->m_CurrentTagData );
  instr>>controlBoxPort;

  if( !instr.eof() )
    throw FileFormatException( 
           "Error in control box port tag data, possibly non numeric values." );
  if( controlBoxPort<1 || controlBoxPort>12 )
    throw FileFormatException("Invalid value given for control box port."); 
  this->m_CurrentControlBoxPort = controlBoxPort;    
}


void 
PolarisHybridConfigurationXMLFileReader::ProcessToolData() 
  throw ( FileFormatException )
{
  if( this->m_CurrentToolName.empty() )
    throw FileFormatException( "\"name\" missing for one of the tools." );

  if( !this->m_HaveCurrentControlBoxPort && 
       this->m_CurrentSromFileName.empty() )
    throw FileFormatException( "Missing srom file and/or control box port." );

              //if the tool section does not have a "calibration_file" tag 
              //we assume the calibration is identity

  TrackerToolConfiguration *toolConfig;

                   //wireless tool
  if( !this->m_HaveCurrentControlBoxPort )
  {
    igstk::PolarisWirelessToolConfiguration *wirelessToolConfig = 
      new igstk::PolarisWirelessToolConfiguration();
      wirelessToolConfig->SetSROMFile( this->m_CurrentSromFileName );
      toolConfig = wirelessToolConfig;
  }
  else //wired tool
  {
    igstk::PolarisWiredToolConfiguration *wiredToolConfig = 
      new igstk::PolarisWiredToolConfiguration();
    wiredToolConfig->SetPortNumber( this->m_CurrentControlBoxPort );
                       //srom is optional
    if( !this->m_CurrentSromFileName.empty() )
      wiredToolConfig->SetSROMFile( this->m_CurrentSromFileName );      
    toolConfig = wiredToolConfig;
  }
  toolConfig->SetToolName( this->m_CurrentToolName );
  toolConfig->SetCalibrationTransform( this->m_CurrentToolCalibration );  
  if( this->m_CurrentToolIsReference )
    this->m_ReferenceTool = toolConfig;
  else
    this->m_TrackerToolList.push_back( toolConfig );

                 //reset all tool data to initial state
  this->m_HaveCurrentControlBoxPort = false;
  this->m_CurrentToolIsReference = false;
  this->m_CurrentSromFileName.clear();
  this->m_CurrentToolName.clear();
  this->m_CurrentToolCalibration.SetToIdentity( 
    igstk::TimeStamp::GetLongestPossibleTime() );
}


bool 
PolarisHybridConfigurationXMLFileReader::HaveConfigurationData()
{
        //check that we have a refresh rate for the tracking system, at least 
        //one tool, and all the serial communication settings
  return ( this->m_HaveRefreshRate &&
           this->m_TrackerToolList.size()!=0 &&
           this->m_HaveComPort && this->m_HaveBaudRate && 
           this->m_HaveDataBits && this->m_HaveParity && 
           this->m_HaveStopBits && this->m_HaveHandshake );
}


const igstk::TrackerConfiguration * 
PolarisHybridConfigurationXMLFileReader::GetTrackerConfigurationData()
throw ( FileFormatException )
{
  igstk::PolarisHybridTrackerConfiguration *trackerConfig = 
    GetPolarisConfiguration();
    
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
  for( ; it!=end; it++ )
  {
    trackerConfig->RequestAddTool( *it );
    if( failureObserver->GotAddToolFailure() )
    {
      delete trackerConfig;
      throw FileFormatException( failureObserver->GetAddToolFailure() );
    }
  }
  if( this->m_ReferenceTool != NULL )
  {
    trackerConfig->RequestAddReference( this->m_ReferenceTool );
    if( failureObserver->GotAddToolFailure() )
    {
      delete trackerConfig;
      throw FileFormatException( failureObserver->GetAddToolFailure() );
    }
  }
 
  return trackerConfig;
}

igstk::PolarisHybridTrackerConfiguration * 
PolarisHybridConfigurationXMLFileReader::GetPolarisConfiguration()
{
  return new igstk::PolarisHybridTrackerConfiguration();
}

} //namespace
