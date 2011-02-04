/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerDataLoggerConfigurationXMLFileReader.cxx
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
#include <algorithm>

#include "igstkTrackerDataLoggerConfigurationXMLFileReader.h"

namespace igstk
{

int 
TrackerDataLoggerConfigurationXMLFileReader::CanReadFile(const char* name)
{
  //check that we have a valid file name, it is not a directory, and the 
  //file exists (has length greater than zero)
  if(itksys::SystemTools::FileLength( name ) == 0 ||
     itksys::SystemTools::FileIsDirectory( name ))
    {
    return 0;
    }
  return 1;
}


void 
TrackerDataLoggerConfigurationXMLFileReader::StartElement( const char * name, 
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
      std::string( "The tag \"tracking_system\" appears multiple times, \
        should only appear once." ) );
      }
    this->m_ReadingTrackerConfigurationLogger = true;
    }
  else if( itksys::SystemTools::Strucmp( name, "tool" ) == 0 )
    {
    if( !m_ReadingTrackerConfigurationLogger )
      throw FileFormatException( "Tag not nested correctly in xml structure." );
    if( m_ReadingToolConfiguration )
      throw FileFormatException( "Self nested \"tool\" tag not allowed." );
    else
      {
      ProcessToolAttributes( atts );
      this->m_ReadingToolConfiguration = true;
      }
    }
  else if( itksys::SystemTools::Strucmp( name, "name" ) == 0 ||
           itksys::SystemTools::Strucmp( name, "save_to" ) == 0 )
    {
    if( !m_ReadingToolConfiguration )
      {
      throw FileFormatException( "Tag not nested correctly in xml structure." );
      }
    }
}


void 
TrackerDataLoggerConfigurationXMLFileReader::EndElement( const char *name )
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
    this->m_ReadingTrackerConfigurationLogger = false;
                 //see that we got all the data, perhaps replace with a more
                //specific message with regard to the missing tag(s)
    if( !HaveConfigurationData() )
      {
      throw FileFormatException( std::string(
        "Missing information in xml file." ) );
      }
    return;
    }
  
  if( m_ReadingToolConfiguration &&
      itksys::SystemTools::Strucmp( name, "name" ) == 0 )
    {
    ProcessToolName();
    }

  if( m_ReadingToolConfiguration &&
      itksys::SystemTools::Strucmp( name, "save_to" ) == 0 )
    {
    ProcessToolOutputFile();
    }

  if( itksys::SystemTools::Strucmp( name, "tool" ) == 0 )
    {
    this->m_ReadingToolConfiguration = false;
    ProcessToolData();
    }
}


void 
TrackerDataLoggerConfigurationXMLFileReader::CharacterDataHandler( 
const char *inData, int inLength )
{
  this->m_CurrentTagData.append( inData, inLength );
}


void 
TrackerDataLoggerConfigurationXMLFileReader::ProcessToolName() 
throw ( FileFormatException )
{
  //see DEVICE_NAME http://www.na-mic.org/Wiki/index.php//Protocol
  const unsigned int DEVICE_NAME_MAX_LENGTH = 20;

  if( this->m_CurrentTagData.size() > DEVICE_NAME_MAX_LENGTH )
    {
    std::ostringstream outStr; 
    outStr<<"Device name ("<<this->m_CurrentTagData<<") too long, ";
    outStr<<" maximal length is "<<DEVICE_NAME_MAX_LENGTH;
    throw FileFormatException( outStr.str() );
    }
  this->m_CurrentToolName = this->m_CurrentTagData;
}


void 
TrackerDataLoggerConfigurationXMLFileReader::ProcessToolAttributes( 
const char **atts )
{
  //by default the tool isn't a reference
  this->m_CurrentToolIsReference = false;

  //go over all the attribute-value pairs
  for( int i=0; atts[i] != NULL; i+=2 ) 
    {
    if( itksys::SystemTools::Strucmp( atts[i], "usage" ) == 0 &&
        itksys::SystemTools::Strucmp( atts[i+1], "reference" ) == 0 )
        this->m_CurrentToolIsReference = true;
    }
}


void 
TrackerDataLoggerConfigurationXMLFileReader::ProcessToolOutputFile() 
throw ( FileFormatException )
{
  FileNameDataType outputFileData = this->m_CurrentTagData;
  //only add if not a duplicate outputFile, this is a naive test that can be
  //bypassed easily by the user. It is here to prevent copy paste errors in the
  //xml file.
  std::vector<FileNameDataType>::iterator it, end;
  it = this->m_CurrentOutputFileNames.begin();
  end = this->m_CurrentOutputFileNames.end();
  for(; it!=end; it++)
    {
    if( (*it) == outputFileData )
      {
      return;
      }
    }
  this->m_CurrentOutputFileNames.push_back( outputFileData ); 
}


void 
TrackerDataLoggerConfigurationXMLFileReader::ProcessToolData() 
throw ( FileFormatException )
{
  if( this->m_CurrentToolName.empty() )
    {
    throw FileFormatException( "\"name\" tag missing for one of the tools." );
    }
  if( this->m_CurrentOutputFileNames.empty()  && !this->m_CurrentToolIsReference )
    { 
    throw FileFormatException( "\"save_to\" tag missing for one of the tools." );
    }
  LoggerDataType::const_iterator it, end;
  it = this->m_ToolNamesAndOutputFileNames.begin();
  end = this->m_ToolNamesAndOutputFileNames.end();

  //check that both the name and outputFile data are unique, don't send
  //two sets of tool info to the same output file.
  for(; it !=end; it++ )
    {
    if( it->first == this->m_CurrentToolName )
      {
      throw FileFormatException( "Non unique tool name given." );
      }
    std::vector<FileNameDataType>::const_iterator it1, it2, end1, end2;
    it1 = this->m_CurrentOutputFileNames.begin();
    end1 = this->m_CurrentOutputFileNames.end();
    for(; it1 != end1; it1++ )
      {
      it2 = it->second.begin();
      end2 = it->second.end();
      for(; it2 != end2; it2++ )
        {
          if ( (*it1).compare(*it2)  == 0 )
          {
          std::ostringstream msg;
          msg<<"Tools '"<<it->first<<"' and '"<<this->m_CurrentToolName;
          msg<<"' send data to the same output file.";
          throw FileFormatException( msg.str() );
          }
        }
      }
    }
  this->m_ToolNamesAndOutputFileNames.insert( 
    std::pair<std::string, std::vector<FileNameDataType> >
    ( this->m_CurrentToolName, this->m_CurrentOutputFileNames ) );
  this->m_CurrentOutputFileNames.clear();
  this->m_CurrentToolName.clear();
}


bool 
TrackerDataLoggerConfigurationXMLFileReader::HaveConfigurationData()
{
  //check that we finished reading the configuration and that we have at 
  //least one tool with output file defined. 
  return ( !this->m_ReadingTrackerConfigurationLogger &&
            this->m_ToolNamesAndOutputFileNames.size() != 0 );
}

void 
TrackerDataLoggerConfigurationXMLFileReader::
  GetToolConfigurationData(
  LoggerDataType &toolNamesAndOutputFiles )
{
  toolNamesAndOutputFiles.clear();
  toolNamesAndOutputFiles.insert( this->m_ToolNamesAndOutputFileNames.begin(), 
                                  this->m_ToolNamesAndOutputFileNames.end() );
}

} //namespace
