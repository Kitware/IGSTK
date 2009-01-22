/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerConfigurationXMLFileReaderBase.cxx
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

#include "igstkTrackerConfigurationXMLFileReaderBase.h"
#include "igstkRigidTransformXMLFileReader.h"

namespace igstk
{

int 
TrackerConfigurationXMLFileReaderBase::CanReadFile(const char* name)
{
  //check that we have a valid file name, it is not a directory, and the 
  //file exists
  if(itksys::SystemTools::FileLength( name ) == 0 ||
     !itksys::SystemTools::FileExists( name ) ||
     itksys::SystemTools::FileIsDirectory( name ))
    {
    return 0;
    }
  return 1;
}


void 
TrackerConfigurationXMLFileReaderBase::StartElement( const char * name, 
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
        std::string( 
          "The tag \"tracking_system\" appears multiple times, should only appear once." 
          ) );
      }
    this->m_ReadingTrackerConfiguration = true;
    ProcessTrackingSystemAttributes( atts );
    }
  else if( itksys::SystemTools::Strucmp( name, "refresh_rate" ) == 0 ) 
    {
    if( !m_ReadingTrackerConfiguration )
      {
      throw FileFormatException( 
        "\"refresh_rate\" tag not nested in \"tracking_system\" tag." );
      }
    }
  else if( itksys::SystemTools::Strucmp( name, "tool" ) == 0 )
    {
    if( !m_ReadingTrackerConfiguration )
      {
      throw FileFormatException( "Tag not nested correctly in xml structure." );
      }
    if( m_ReadingToolConfiguration )
      {
      throw FileFormatException( "Self nested \"tool\" tag not allowed." );
      }
    else
      {
      this->m_ReadingToolConfiguration = true;
      ProcessToolAttributes( atts );
      }
    }
  else if( itksys::SystemTools::Strucmp( name, "name" ) == 0 ||
           itksys::SystemTools::Strucmp( name, "calibration_file" ) == 0 )
    {
    if( !m_ReadingToolConfiguration )
      {
      throw FileFormatException( "Tag not nested correctly in xml structure." );
      }
    }
}


void 
TrackerConfigurationXMLFileReaderBase::EndElement( const char *name )
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
      itksys::SystemTools::Strucmp( name, "refresh_rate" ) == 0 )
    {
    ProcessRefreshRate();
    this->m_HaveRefreshRate = true;
    }
  if( m_ReadingToolConfiguration &&
      itksys::SystemTools::Strucmp( name, "name" ) == 0 )
    {
    ProcessToolName();
    }
  if( m_ReadingToolConfiguration &&
      itksys::SystemTools::Strucmp( name, "calibration_file" ) == 0 )
    {
    ProcessToolCalibration();
    }
 if( itksys::SystemTools::Strucmp( name, "tool" ) == 0 )
   {
   this->m_ReadingToolConfiguration = false;
   ProcessToolData();
   }
}


void 
TrackerConfigurationXMLFileReaderBase::CharacterDataHandler( 
const char *inData, int inLength )
{
  this->m_CurrentTagData.append( inData, inLength );
}


void 
TrackerConfigurationXMLFileReaderBase::ProcessTrackingSystemAttributes( 
  const char **atts ) throw ( 
    FileFormatException, UnexpectedTrackerTypeException )
{
  bool systemTypeFound = false;

  //go over all the attribute-value pairs
  for( int i=0; atts[i] != NULL; i+=2 ) 
    {
    if( itksys::SystemTools::Strucmp( atts[i], "type" ) == 0 )
      {
      if( itksys::SystemTools::Strucmp( atts[i+1], 
            GetSystemType().c_str() ) != 0 )
        {
        throw UnexpectedTrackerTypeException();
        }
      else
        {
        systemTypeFound = true;
        }
      }
    }
  if( !systemTypeFound )
    {
    throw FileFormatException( 
      "\"type\" attribute missing in tracking_system tag." );
    }
}


void 
TrackerConfigurationXMLFileReaderBase::ProcessRefreshRate() 
throw ( FileFormatException )
{
  double refreshRate;
  std::istringstream instr;  
  instr.str( this->m_CurrentTagData );
  instr>>refreshRate;
  //check that we got to the end of the stream, (the string 
  //m_CurrentTagData has no trailing white spaces, trimmed in the 
  //EndElement method)
  if( !instr.eof() )
    {
    throw FileFormatException( 
      "Error in refresh rate data, possibly non numeric values" );
    }

  if( refreshRate<=0 || refreshRate>GetMaximalRefreshRate() )
    {
    std::ostringstream msg;
    msg.str("");
    msg<<"Invalid tracker frequency specified ("<<refreshRate<<").";
    msg<< " Valid values are in [0,"<<GetMaximalRefreshRate()<<"].";
    throw FileFormatException( msg.str() );
    }
  this->m_RefreshRate = refreshRate;
}


void 
TrackerConfigurationXMLFileReaderBase::ProcessToolAttributes( 
const char **atts ) throw ( FileFormatException )
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
  if( this->m_CurrentToolIsReference && this->m_ReferenceTool != NULL )
    {
    throw FileFormatException( "Multiple tools defined as reference." );
    }
}


void 
TrackerConfigurationXMLFileReaderBase::ProcessToolName() 
throw ( FileFormatException )
{
  this->m_CurrentToolName = this->m_CurrentTagData;
}


void 
TrackerConfigurationXMLFileReaderBase::ProcessToolCalibration() 
throw ( FileFormatException )
{
  std::ostringstream msg;

  //check that the calibration file name is valid
  if(itksys::SystemTools::FileLength( this->m_CurrentTagData.c_str() ) == 0 ||
    !itksys::SystemTools::FileExists( this->m_CurrentTagData.c_str() ) ||
    itksys::SystemTools::FileIsDirectory( this->m_CurrentTagData.c_str() ) )
    {
    msg<<"Calibration file ("<<this->m_CurrentTagData;
    msg<<") does not exist or is a directory";
    throw FileFormatException( msg.str() );
    }
  //try to read the file
  igstk::RigidTransformXMLFileReader::Pointer xmlReader = 
    igstk::RigidTransformXMLFileReader::New();

  xmlReader->SetFilename( this->m_CurrentTagData );
  //may cause exceptions which the user should take care of
  xmlReader->GenerateOutputInformation();
  //xml reading is successful (no exception generation) even 
  //when the file is empty, need to check that the data was
  //actually read
  if( !xmlReader->HaveTransformData() )
    {
    msg<<"Calibration file ("<<this->m_CurrentTagData;
    msg<<") doesn't contain transformation data";  
    throw FileFormatException( msg.str() );
    }
  igstk::PrecomputedTransformData::Pointer calibrationData = 
    xmlReader->GetTransformData();

  RequestTransformObserver::Pointer transformObserver = 
    RequestTransformObserver::New();
  calibrationData->AddObserver( 
    igstk::PrecomputedTransformData::TransformTypeEvent(), 
                                transformObserver );
  calibrationData->RequestTransform();
  igstk::Transform *transform = 
    static_cast<igstk::Transform *>(transformObserver->GetRequestTransform());

  //load the tools calibration data
  this->m_CurrentToolCalibration = *transform;
}

} //namespace
