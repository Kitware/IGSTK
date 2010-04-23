#ifndef __igstkInterventionalPlanXMLFileReaderBase_h
#define __igstkInterventionalPlanXMLFileReaderBase_h

#include <itkXMLFile.h>
#include "igstkInterventionalPlan.h"


namespace igstk
{
/** \class InterventionalPlanXMLFileReaderBase
 * 
 *  \brief This class is an abstract reader for xml files containing an 
 *         intreventional plan. The specific type of plan is
 *         defined by the subclass.
 *
 *  This class is the base class for all the readers that load an interventional
 *  plan. 
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
 */
class InterventionalPlanXMLFileReaderBase : public itk::XMLReaderBase 
{
public:
  /**
   * \class This is the exception that is thrown if there is a problem with the
   * file format (does not follow the expected format).
   */
  class FileFormatException : public std::exception
    {
    public:
    /**
     * Construct an exception with a specifc message.
     */
    FileFormatException(const std::string &errorMessage) 
      {
      this->m_ErrorMessage = errorMessage;
      }

    /**
     * Virtual destructor.
     */
    virtual ~FileFormatException() throw() {}

    /**
     * Get the error message.
     */
    virtual const char* what() const throw() 
      {
      return m_ErrorMessage.c_str();
      }
    private:
    std::string m_ErrorMessage;
    };

  //standard typedefs
  typedef InterventionalPlanXMLFileReaderBase    Self;
  typedef itk::XMLReaderBase                     Superclass;
  typedef itk::SmartPointer<Self>                Pointer;
  typedef itk::SmartPointer < const Self >       ConstPointer;

           //run-time type information (and related methods)
  itkTypeMacro( InterventionalPlanXMLFileReaderBase, itk::XMLReaderBase );

  /**
   * Check that the given name isn't a zero length string, that the file exists,
   * and that it isn't a directory.
   */
  virtual int CanReadFile(const char* name);

  /**
   * Method called when a new xml tag start is encountered.
   */
  virtual void StartElement( const char * name, const char **atts ); 

  /**
   * Method called when an xml tag end is encountered.
   */
  virtual void EndElement( const char *name ); 

  /**
   * Method for handling the data inside an xml tag.
   */
  virtual void CharacterDataHandler( const char *inData, int inLength );

  /**
   * Method for checking if the plan has been read. When the xml 
   * file is empty or doesn't contain the relevant tags the read operation is 
   * successful, but we don't have the information we need.
   */
  bool HavePlan();

  /**
   * Return a pointer to the plan. If no plan has been read then return NULL.
   * The caller is responsible for releasing this memory via delete.
   */
  virtual igstk::InterventionalPlan * GetPlan() = 0;

protected:
          //this is the constructor that is called by the factory to 
         //create a new object
  InterventionalPlanXMLFileReaderBase() : m_HavePlan(false), m_ReadingPlan(false) {}
  virtual ~InterventionalPlanXMLFileReaderBase() {}

  void ProcessPlanAttributes( const char ** atts )
  throw ( FileFormatException );
  
  std::string m_CurrentTagData;
  bool        m_ReadingPlan;

  std::string m_PatientName;
  std::string m_seriesUID;
  std::string m_studyUID;
  bool        m_HavePlan;
  

private:
  InterventionalPlanXMLFileReaderBase( 
    const InterventionalPlanXMLFileReaderBase & other );


  const InterventionalPlanXMLFileReaderBase & operator=( 
    const InterventionalPlanXMLFileReaderBase & right );
};


}
#endif //__igstkInterventionalPlanXMLFileReaderBase_h
