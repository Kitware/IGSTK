#include <itksys/SystemTools.hxx>
#include <algorithm>
#include "igstkInterventionalPlanXMLFileWriterBase.h"

namespace igstk
{

int 
InterventionalPlanXMLFileWriterBase::CanWriteFile( const char* name )
{
  //check that we have a valid file name, it is not a directory
  if( itksys::SystemTools::FileIsDirectory( name ) )
    {
    return 0;
    }
  return 1;
}


int 
InterventionalPlanXMLFileWriterBase::WriteFile()
{ //problem with the given file name 
  if( !CanWriteFile( this->m_Filename.c_str() ) )
    {
    return 0;
    }
  std::ofstream out;
  out.open( this->m_Filename.c_str() );
        //failed openning file
  if( out.fail() )
    {
    return 0;
    }
  out<<"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n\n\n\n";
  out<<"<interventional_plan patient_name=\""<<this->m_InputObject->GetPatientName()<<"\""<<"\n";
  out<<"\t\tstudy_instance_UID=\""<<this->m_InputObject->GetStudyID()<<"\""<<"\n";
  out<<"\t\tseries_instance_UID=\""<<this->m_InputObject->GetSeriesID()<<"\""<<">\n";
  WritePlan( out );
  out<<"</interventional_plan>\n";

  out.close();
  return 1;
}



} //namespace
