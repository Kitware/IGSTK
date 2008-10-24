#include <iostream>
#include <sstream>
#include <itksys/SystemTools.hxx>
#include <algorithm>

#include "igstkOIGTLinkTrackerConfigurationXMLFileReader.h"

namespace igstk
{

int 
OIGTLinkTrackerConfigurationXMLFileReader::CanReadFile(const char* name)
{
       //check that we have a valid file name, it is not a directory, and the 
       //file exists (has length greater than zero)
  if(itksys::SystemTools::FileLength( name ) == 0 ||
     itksys::SystemTools::FileIsDirectory( name ))
      return 0;
  return 1;
}


void 
OIGTLinkTrackerConfigurationXMLFileReader::StartElement( const char * name, 
                                                     const char **atts )
{
  this->m_CurrentTagData.clear();

              //check for the root of the xml tree
  if( itksys::SystemTools::Strucmp( name, "tracking_system" ) == 0 ) 
  {
                   //there can only be one "tracking_system" per xml file                  
    if( HaveConfigurationData() )
    {
      throw FileFormatException(
      std::string( "The tag \"tracking_system\" appears multiple times, should only appear once." ) );
    }
    this->m_ReadingTrackerConfiguration = true;
  }
  else if( itksys::SystemTools::Strucmp( name, "send_to_ip" ) == 0 ) 
  {
    if( !m_ReadingTrackerConfiguration )
      throw FileFormatException( "\"send_to_ip\" tag not nested in \"tracking_system\" tag." );
  }
  else if( itksys::SystemTools::Strucmp( name, "tool" ) == 0 )
  {
    if( !m_ReadingTrackerConfiguration )
      throw FileFormatException( "Tag not nested correctly in xml structure." );
    if( m_ReadingToolConfiguration )
      throw FileFormatException( "Self nested \"tool\" tag not allowed." );
    else
    {
      this->m_ReadingToolConfiguration = true;
    }
  }
  else if( itksys::SystemTools::Strucmp( name, "name" ) == 0 ||
           itksys::SystemTools::Strucmp( name, "ip_port" ) == 0 )
  {
    if( !m_ReadingToolConfiguration )
      throw FileFormatException( "Tag not nested correctly in xml structure." );
  }
}


void 
OIGTLinkTrackerConfigurationXMLFileReader::EndElement( const char *name )
{
                         //trim the tag's data string and check that it
                         //isn't empty (empty tags are not allowed)
  size_t startpos = this->m_CurrentTagData.find_first_not_of(" \t\n\r");
  size_t endpos = this->m_CurrentTagData.find_last_not_of(" \t\n\r");   
  if( startpos == std::string::npos || endpos == std::string::npos )
  {
    this->m_CurrentTagData.clear();
  }
  else
  {
    this->m_CurrentTagData = this->m_CurrentTagData.substr( startpos, 
                                                            endpos-startpos+1 );
  }
           //replace all newlines ('\n') and carriage returns ('\r') with space
  std::replace( this->m_CurrentTagData.begin(), 
                this->m_CurrentTagData.end(), 
                '\n', ' ' );
  std::replace( this->m_CurrentTagData.begin(), 
                this->m_CurrentTagData.end(), 
                '\r', ' ' );
                    
  if( this->m_CurrentTagData.empty() )
  {
    std::ostringstream outStr; 
    outStr<<"Empty tag ("<<name<<") not allowed in tracker settings";
    throw FileFormatException( outStr.str() );
  }

  if( itksys::SystemTools::Strucmp( name, "tracking_system" ) == 0 )
  {
    this->m_ReadingTrackerConfiguration = false;
                 //see that we got all the data, perhaps replace with a more
                //specific message with regard to the missing tag(s)
    if( !HaveConfigurationData() )
    {
      throw FileFormatException( std::string( "Missing tag(s) in xml file." ) );
    }
  }            
  if( m_ReadingTrackerConfiguration &&
      itksys::SystemTools::Strucmp( name, "send_to_ip" ) == 0 )
  {
    ProcessHostName();
  }
  if( m_ReadingToolConfiguration &&
      itksys::SystemTools::Strucmp( name, "name" ) == 0 )
  {
    ProcessToolName();
  }
  if( m_ReadingToolConfiguration &&
      itksys::SystemTools::Strucmp( name, "ip_port" ) == 0 )
  {
    ProcessToolIPPort();
    this->m_HaveCurrentToolIPPort = true;
  }
 if( itksys::SystemTools::Strucmp( name, "tool" ) == 0 )
 {
  this->m_ReadingToolConfiguration = false;
  ProcessToolData();
 }
}


void 
OIGTLinkTrackerConfigurationXMLFileReader::CharacterDataHandler( 
  const char *inData, 
  int inLength )
{
  this->m_CurrentTagData.append( inData, inLength );
}


void 
OIGTLinkTrackerConfigurationXMLFileReader::ProcessHostName() 
  throw ( FileFormatException )
{
  this->m_HostName = this->m_CurrentTagData;
}


void 
OIGTLinkTrackerConfigurationXMLFileReader::ProcessToolName() 
  throw ( FileFormatException )
{
  this->m_CurrentToolName = this->m_CurrentTagData;
}


void 
OIGTLinkTrackerConfigurationXMLFileReader::ProcessToolIPPort() 
  throw ( FileFormatException )
{
  unsigned ipPort;
  std::istringstream instr;  
  instr.str( this->m_CurrentTagData );
  instr>>ipPort;
            //check that we got to the end of the stream, (the string 
            //m_CurrentTagData has no trailing white spaces, trimmed in the 
            //EndElement method)
  if( !instr.eof() )
    throw FileFormatException( 
    "Error in tool's ip port, possibly non numeric values" );
  this->m_CurrentToolIPPort = ipPort;
}


void 
OIGTLinkTrackerConfigurationXMLFileReader::ProcessToolData() 
  throw ( FileFormatException )
{
  if( this->m_CurrentToolName.empty() )
    throw FileFormatException( "\"name\" missing for one of the tools." );
  if( !this->m_HaveCurrentToolIPPort )
    throw FileFormatException( "\"ip_port\" missing for one of the tools." );

  std::map<std::string, unsigned int>::const_iterator it, end;
  it = this->m_ToolNamesAndPorts.begin();
  end = this->m_ToolNamesAndPorts.end();

           //check that both the name and ip port are unique
  for( ; it!=end; it++ )
  {
    if( it->first == this->m_CurrentToolName ||
        it->second == this->m_CurrentToolIPPort )
        throw FileFormatException( "Non unique tool name or ip port given." );
  }
  this->m_ToolNamesAndPorts.insert( 
    std::pair<std::string, unsigned int>( this->m_CurrentToolName,
                                          this->m_CurrentToolIPPort ) );
  this->m_HaveCurrentToolIPPort = false;
  this->m_CurrentToolName.clear();
}


bool 
OIGTLinkTrackerConfigurationXMLFileReader::HaveConfigurationData()
{
        //check that we have a host name for the OIGTLink, and at least 
        //one tool, 
  return ( !this->m_HostName.empty() &&
           this->m_ToolNamesAndPorts.size()!=0 );
}

void 
OIGTLinkTrackerConfigurationXMLFileReader::GetOIGTLinkToolConfigurationData( 
    std::map<std::string, unsigned int> &toolNamesAndPorts )
{
  toolNamesAndPorts.clear();
  toolNamesAndPorts.insert( this->m_ToolNamesAndPorts.begin(), 
                            this->m_ToolNamesAndPorts.end() );
}


std::string 
OIGTLinkTrackerConfigurationXMLFileReader::GetOIGTLinkHostName()
{
  return this->m_HostName;
}


} //namespace
