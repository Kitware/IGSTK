#ifndef __igstkInterventionalPlan_h
#define __igstkInterventionalPlan_h

#include <string>
#include "itkIndent.h"

namespace igstk 
{
 
/** \class InterventionalPlan 
 *  \brief A base class representing an interventional plan.
 * 
 *  Interventional plans are specified in image-space. As such, a plan is always
 *  associated with a specific image. In our context this is done using three
 *  DICOM tags:
 *        Study Instance UID: (0020,000D)  
 *        Series Instance UID (0020:000E)  
 *        PatientName         (0010,0010)
 *
 * The study and series numbers identify the image and the patient name is used
 * for HCI purposes. Most often the application developer will validate that the
 * image used for navigation is the same one used for planning.
 *
 **/

class InterventionalPlan
{
public:

  /** Constructor and destructor */
  InterventionalPlan(const std::string &patientName,
                     const std::string &studyID,
                     const std::string &seriesID);
  InterventionalPlan( const InterventionalPlan &other);
  virtual ~InterventionalPlan() {}

  std::string GetPatientName();
  std::string GetStudyID();
  std::string GetSeriesID();

  /** Method for printing the member variables of this class to an ostream */
  void Print( std::ostream& os, itk::Indent indent ) const;

protected:

  void PrintHeader( std::ostream& os, itk::Indent indent ) const;

  void PrintTrailer( std::ostream& itkNotUsed(os), 
                     itk::Indent itkNotUsed(indent) ) const;

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:
  /** identifiers that associate a plan with a specific image */   
  std::string m_PatientName;
  std::string m_StudyID;
  std::string m_SeriesID;

};

std::ostream& operator<<( std::ostream& os, const igstk::InterventionalPlan& o );
}

#endif
