/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerConfigurationXMLFileReaderBase.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTrackerConfigurationXMLFileReaderBase_h
#define __igstkTrackerConfigurationXMLFileReaderBase_h

#include <itkXMLFile.h>
#include "igstkTrackerConfiguration.h"
#include "igstkPrecomputedTransformData.h"

namespace igstk
{
/** \class TrackerConfigurationXMLFileReaderBase
 * 
 *  \brief This class is an abstract reader for xml files containing a 
 *         tracker configuration. 
 *
 *         This class is an abstract reader for xml files containing a 
 *         tracker configuration. The specific type of tracker is
 *         defined by the subclass.
 */
class TrackerConfigurationXMLFileReaderBase : public itk::XMLReaderBase 
{
public:

  class UnexpectedTrackerTypeException : public std::exception
  {
  public:
    UnexpectedTrackerTypeException() throw() { };
    virtual ~UnexpectedTrackerTypeException() throw() { };
  };

  /**
   * \class FileFormatException This is the exception that is thrown if there 
   *        is a problem with the file format (does not follow the expected 
   *        format).
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
  typedef TrackerConfigurationXMLFileReaderBase    Self;
  typedef itk::XMLReaderBase                       Superclass;
  typedef itk::SmartPointer<Self>                  Pointer;
  typedef itk::SmartPointer < const Self >         ConstPointer;

  //run-time type information (and related methods)
  itkTypeMacro( TrackerConfigurationXMLFileReaderBase, itk::XMLReaderBase );

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
   * Method for checking if the configuration data has been read. When the xml 
   * file is empty or doesn't contain the relevant tags the read operation is 
   * successful, but we don't have the information we need.
   */
  virtual bool HaveConfigurationData() = 0; 

  /**
   * Return a pointer to the object containing the configuration data.
   */
  virtual const igstk::TrackerConfiguration::Pointer GetTrackerConfigurationData()
    throw ( FileFormatException ) = 0;

protected:
  //this is the constructor that is called by the factory to 
  //create a new object
  TrackerConfigurationXMLFileReaderBase()
    {
    m_ReadingTrackerConfiguration = false;
    m_ReadingToolConfiguration = false;
    m_HaveRefreshRate = false;
    m_ReferenceTool = NULL;
    m_CurrentToolIsReference = false;
    m_CurrentToolCalibration.SetToIdentity( 
    igstk::TimeStamp::GetLongestPossibleTime() );
    }
  virtual ~TrackerConfigurationXMLFileReaderBase() 
    {
    std::vector<TrackerToolConfiguration *>::iterator it = 
      this->m_TrackerToolList.begin();
    std::vector<TrackerToolConfiguration *>::iterator end =  
      this->m_TrackerToolList.end();
    for(; it!=end; it++)
      {
      delete (*it);
      }
    delete m_ReferenceTool;
    }

  igstkObserverMacro( AddToolFailure, 
                      igstk::TrackerConfiguration::AddToolFailureEvent, 
                      std::string );
  
  typedef igstk::PrecomputedTransformData::TransformType* TransformTypePointer;
  igstkObserverMacro( RequestTransform, 
                      igstk::PrecomputedTransformData::TransformTypeEvent, 
                      TransformTypePointer);

  void ProcessTrackingSystemAttributes( const char **atts )
    throw ( FileFormatException, UnexpectedTrackerTypeException );

  void ProcessRefreshRate() throw ( FileFormatException );

  void ProcessToolAttributes( const char **atts ) throw ( FileFormatException );

  void ProcessToolName() throw ( FileFormatException );

  void ProcessToolCalibration() throw ( FileFormatException );
   
  virtual void ProcessToolData() throw ( FileFormatException ) = 0;
  
  virtual double GetMaximalRefreshRate()=0;

  virtual std::string GetSystemType()=0;

  bool m_ReadingTrackerConfiguration;
  bool m_ReadingToolConfiguration;
  bool m_HaveRefreshRate;

  std::string m_CurrentTagData;

  igstk::Transform   m_CurrentToolCalibration;
  std::string        m_CurrentToolName;
  bool               m_CurrentToolIsReference;

  double                                    m_RefreshRate; 
  std::vector<TrackerToolConfiguration *>   m_TrackerToolList;   
  TrackerToolConfiguration *                m_ReferenceTool; 

private:
  TrackerConfigurationXMLFileReaderBase( 
    const TrackerConfigurationXMLFileReaderBase & other );


  const TrackerConfigurationXMLFileReaderBase & operator=( 
    const TrackerConfigurationXMLFileReaderBase & right );
};


}
#endif //__igstkTrackerConfigurationXMLFileReaderBase_h
