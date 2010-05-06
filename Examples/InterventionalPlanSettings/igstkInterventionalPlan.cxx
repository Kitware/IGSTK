#include "igstkInterventionalPlan.h"
#include <typeinfo>

namespace igstk
{


InterventionalPlan::InterventionalPlan(const std::string &patientName,
                                       const std::string &studyID,
                                       const std::string &seriesID)
{
  this->m_PatientName = patientName;
  this->m_StudyID = studyID;
  this->m_SeriesID = seriesID;
}


InterventionalPlan::InterventionalPlan( const InterventionalPlan &other)
{
  this->m_PatientName = other.m_PatientName;
  this->m_StudyID = other.m_StudyID;
  this->m_SeriesID = other.m_SeriesID;
}


std::string 
InterventionalPlan::GetPatientName()
{
  return this->m_PatientName;
}

  
std::string 
InterventionalPlan::GetStudyID()
{
  return this->m_StudyID;
}


std::string 
InterventionalPlan::GetSeriesID()
{
  return this->m_SeriesID;
}


void 
InterventionalPlan::Print( std::ostream& os, itk::Indent indent ) const
{
  this->PrintHeader( os, indent ); 
  this->PrintSelf( os, indent.GetNextIndent() );
  this->PrintTrailer( os, indent );
}


void 
InterventionalPlan::PrintHeader( std::ostream& os, itk::Indent indent ) const
{
  os << indent << "Interventional Plan" << " (" << this << ")\n";
  os << indent << "RTTI typeinfo: " << typeid( *this ).name() << std::endl;
}


/**
 * Define a default print trailer for all objects.
 */
void 
InterventionalPlan::PrintTrailer( std::ostream& itkNotUsed(os), 
                                  itk::Indent itkNotUsed(indent) ) const
{
}


void 
InterventionalPlan::PrintSelf( std::ostream& os, itk::Indent indent ) const
{ 
  if( !this->m_PatientName.empty() )
    os << indent << "Patient name: "<<this->m_PatientName << std::endl;
  if( !this->m_StudyID.empty() )
    os << indent << "Study ID: "<<this->m_StudyID << std::endl;
  if( !this->m_SeriesID.empty() )
    os << indent << "Series ID: "<<this->m_SeriesID << std::endl;
}


std::ostream& operator<<( std::ostream& os, const InterventionalPlan& o )
{
  o.Print(os, 0);
  return os;
}



} // end namespace igstk
