#include <itksys/SystemTools.hxx>
#include <algorithm>
#include "igstkInterventionalPlanXMLFileReaderBase.h"

namespace igstk
{



int 
InterventionalPlanXMLFileReaderBase::CanReadFile(const char* name)
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
InterventionalPlanXMLFileReaderBase::StartElement( const char * name, 
                                                   const char **atts )
{
  this->m_CurrentTagData.clear();

  if( itksys::SystemTools::Strucmp( name, "interventional_plan" ) == 0 ) 
    {
    //there can only be one "precomputed_transform" per xml file
    if( HavePlan() )
      {
        throw FileFormatException(
        std::string( "The tag \"interventional_plan\" appears multiple times,\
        should only appear once." ) );
      }
    this->m_ReadingPlan = true;
    ProcessPlanAttributes( atts );
    }
}


void 
InterventionalPlanXMLFileReaderBase::EndElement( const char *name )
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
}


void 
InterventionalPlanXMLFileReaderBase::CharacterDataHandler( 
  const char *inData, 
  int inLength )
{
  this->m_CurrentTagData.append( inData, inLength );
}


bool 
InterventionalPlanXMLFileReaderBase::HavePlan()
{
  return this->m_HavePlan;
}


void 
InterventionalPlanXMLFileReaderBase::ProcessPlanAttributes( const char ** atts )
throw ( FileFormatException )
{
  bool haveSeriesUID, haveStudyUID, havePatientName;
  haveSeriesUID = haveStudyUID = havePatientName = false;

  //go over all the attribute-value pairs
  for( int i=0; atts[i] != NULL; i+=2 ) 
    {
    if( itksys::SystemTools::Strucmp( atts[i], "patient_name" ) == 0 ) 
      {
       this->m_PatientName = atts[i+1];
       havePatientName = true;
      }
    if( itksys::SystemTools::Strucmp( atts[i], "study_instance_UID" ) == 0 ) 
      {
       this->m_studyUID = atts[i+1];
       haveStudyUID = true;
      }
    if( itksys::SystemTools::Strucmp( atts[i], "series_instance_UID" ) == 0 ) 
      {
       this->m_seriesUID = atts[i+1];
       haveSeriesUID = true;
      }
    }
  if( !( haveSeriesUID && haveStudyUID && havePatientName ) )
    throw FileFormatException("Missing required plan attributes.");
}

} //namespace
