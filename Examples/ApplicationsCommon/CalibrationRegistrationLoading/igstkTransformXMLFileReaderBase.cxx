/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTransformXMLFileReaderBase.cxx
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
#include "igstkTransformXMLFileReaderBase.h"

namespace igstk
{

int 
TransformXMLFileReaderBase::CanReadFile(const char* name)
{
  //check that we have a valid file name, it is not a directory, and the 
  //file exists
  if(itksys::SystemTools::FileLength( name ) == 0 ||
     !itksys::SystemTools::FileExists( name ) ||
     itksys::SystemTools::FileIsDirectory( name ))
      return 0;
  return 1;
}


void 
TransformXMLFileReaderBase::StartElement( const char * name, 
                                          const char **atts )
{
  if( itksys::SystemTools::Strucmp( name, "precomputed_transform" ) == 0 ) 
    {
    //there can only be one "precomputed_transform" per xml file
    if( HaveTransformData() )
      {
        throw FileFormatException(
        std::string( "The tag \"precomputed_transform\" appears multiple times,\
        should only appear once." ) );
      }
    this->m_ReadingTransformData = true;
    }
  else if( itksys::SystemTools::Strucmp( name, "description" ) == 0      ||  
           itksys::SystemTools::Strucmp( name, "computation_date" ) == 0 ||
           itksys::SystemTools::Strucmp( name, "transformation" ) == 0 )
    {
    if( this->m_ReadingTagData )
      {
      throw FileFormatException( "Nested tags are not allowed." );
      }
    else
      {
      this->m_ReadingTagData = true;
      }
    //if this is the "transformation" tag then it can have an
    //"estimation_error" attribute, check for it here
    if( itksys::SystemTools::Strucmp( name, "transformation" ) == 0 )
      {
      ProcessTransformationAttributes( atts );
      }
    }
}


void 
TransformXMLFileReaderBase::EndElement( const char *name )
{
  //trim the tag's data string
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
  
  if( itksys::SystemTools::Strucmp( name, "precomputed_transform" ) == 0 )
    {
    this->m_ReadingTransformData = false;
    //see that we got all the data, perhaps replace with a more
    //specific message with regard to the missing tag(s)
    if( !HaveTransformData() )
      {
      throw FileFormatException( std::string( "Missing tag(s) in xml file." ) );
      }
    }    //tags are expected to be inside the "precomputed_transform" tag
  if( this->m_ReadingTransformData )
    {
    if( itksys::SystemTools::Strucmp( name, "description" ) == 0 )
      {
      ProcessDescription();
      this->m_HaveDescription = true;
      }
    else if( itksys::SystemTools::Strucmp( name, "computation_date" ) == 0 )
      {
      ProcessDate();
      this->m_HaveDate = true;
      }
    else if( itksys::SystemTools::Strucmp( name, "transformation" ) == 0 )
      {
      ProcessTransformation();
      this->m_HaveTransform = true;
      }
    //done processing, clear the current tag's data
    this->m_CurrentTagData.clear();
    this->m_ReadingTagData = false;
    }
}


void 
TransformXMLFileReaderBase::CharacterDataHandler( 
  const char *inData, 
  int inLength )
{
  this->m_CurrentTagData.append( inData, inLength );
}


bool 
TransformXMLFileReaderBase::HaveTransformData()
{
  return ( this->m_HaveDescription && this-> m_HaveDate && 
           this->m_HaveTransform  && this->m_HaveError );
}


igstk::PrecomputedTransformData::Pointer 
TransformXMLFileReaderBase::GetTransformData()
{
  igstk::PrecomputedTransformData::Pointer transformData =
    igstk::PrecomputedTransformData::New();
  
  transformData->RequestInitialize( this->m_Transform,
                                    this->m_Date,
                                    this->m_Description,
                                    this->m_EstimationError );

  return transformData;
}


void 
TransformXMLFileReaderBase::ProcessDescription() 
throw ( FileFormatException )
{
  if( this->m_CurrentTagData.empty() )
    {
    throw FileFormatException("Transformation description is missing.");
    }
  else
    {
    this->m_Description = this->m_CurrentTagData;
    }
}


void 
TransformXMLFileReaderBase::ProcessDate() 
throw ( FileFormatException )
{
  if( this->m_CurrentTagData.empty() )
    {
    throw FileFormatException("Computation date is missing.");
    }
  else
    {
    this->m_Date = this->m_CurrentTagData;
    }
}


void 
TransformXMLFileReaderBase::ProcessTransformationAttributes( 
const char ** atts )
throw ( FileFormatException )
{
  //go over all the attribute-value pairs
  for( int i=0; atts[i] != NULL; i+=2 ) 
    {
    if( itksys::SystemTools::Strucmp( atts[i], "estimation_error" ) == 0 ) 
      {
      PrecomputedTransformData::ErrorType tmp;
      std::istringstream instr;  
      instr.str( atts[i+1] );
      instr>>tmp; 
      //check that we got to the end of the stream, we expect a single value
      //here (assumes that the string err has no trailing white spaces)
      if(!instr.eof())
        {
        throw FileFormatException("Estimation error format is invalid.");
        }
      else
        {
        this->m_EstimationError = tmp;
        }
      this->m_HaveError = true;
      }
    }
}

} //namespace
