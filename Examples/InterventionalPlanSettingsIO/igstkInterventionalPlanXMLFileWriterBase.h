#ifndef __igstkInterventionalPlanXMLFileWriterBase_h
#define __igstkInterventionalPlanXMLFileWriterBase_h

#include <itkXMLFile.h>
#include "igstkInterventionalPlan.h"


namespace igstk
{
/** \class InterventionalPlanXMLFileWriterBase
 * 
 *  \brief This class is an abstract writer for xml files containing an 
 *         interventional plan. The specific type of plan is
 *         defined by the subclass.
 *
 *  This class is the base class for all the writers that output an 
 *  interventional plan.
 *
 *  The xml file format is as follows:
    @verbatim
    <InterventionalPlan PatientName="Liver24" 
                        StudyInstanceUID="1.2.840.113696.563627.530.570770.20071104103124" 
                        SeriesInstanceUID="1.3.12.2.1107.5.1.4.24515.4.0.12150429059722404">
    </InterventionalPlan>
   @endverbatim
 *
 *
 * 
 */
class InterventionalPlanXMLFileWriterBase :
         public itk::XMLWriterBase<InterventionalPlan> 
{
public:
  
  //standard typedefs
  typedef InterventionalPlanXMLFileWriterBase                     Self;
  typedef itk::XMLWriterBase<InterventionalPlan>                  Superclass;
  typedef itk::SmartPointer<Self>                                 Pointer;

           //run-time type information (and related methods)
  itkTypeMacro( InterventionalPlanXMLFileWriterBase<InterventionalPlan>, 
                itk::XMLWriterBase );

  //Calling program must check that we have a valid file name, it is not a directory
  virtual int CanWriteFile( const char* name );

  //write the file, if failed return 0 otherwise 1, should be boolean but the itk
  //base class method signature is pure virtual with int as the return value
  virtual int WriteFile();

protected:

  InterventionalPlanXMLFileWriterBase() {}
  virtual ~InterventionalPlanXMLFileWriterBase() {}

  //write the actual plan. on failure returns 0, otherwise return 1
  virtual int WritePlan( std::ofstream &out ) = 0;
 
private:

  InterventionalPlanXMLFileWriterBase( 
    const InterventionalPlanXMLFileWriterBase & other );


  const InterventionalPlanXMLFileWriterBase & operator=( 
    const InterventionalPlanXMLFileWriterBase & right );
};


}
#endif //__igstkInterventionalPlanXMLFileWriterBase_h
