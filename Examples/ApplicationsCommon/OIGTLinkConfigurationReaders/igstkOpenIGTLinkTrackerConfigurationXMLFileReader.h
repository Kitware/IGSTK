/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkOpenIGTLinkTrackerConfigurationXMLFileReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkOpenIGTLinkTrackerConfigurationXMLFileReader_h
#define __igstkOpenIGTLinkTrackerConfigurationXMLFileReader_h

#include <itkXMLFile.h>

namespace igstk
{
/** \class OpenIGTLinkTrackerConfigurationXMLFileReader
 * 
 *  \brief This class is a reader for xml files containing a 
 *         tracker configuration with open IGT link settings. 
 *
 *         This class is a reader for xml files containing a 
 *         tracker configuration with open IGT link settings.
 */
class OpenIGTLinkTrackerConfigurationXMLFileReader : public itk::XMLReaderBase 
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
  typedef OpenIGTLinkTrackerConfigurationXMLFileReader    Self;
  typedef itk::XMLReaderBase                              Superclass;
  typedef itk::SmartPointer<Self>                         Pointer;

          //run-time type information (and related methods)
  itkTypeMacro( OpenIGTLinkTrackerConfigurationXMLFileReader,
                itk::XMLReaderBase );

          //method for creation through the object factory
  itkNewMacro( Self );

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

  typedef std::pair<std::string, unsigned int> ConnectionDataType;
  typedef std::map<std::string, std::vector<ConnectionDataType> >
    OpenIGTLinkDataType;

  /**
   * Get the map between tool names and ip ports.
   */
  void GetOpenIGTLinkToolConfigurationData( OpenIGTLinkDataType 
    &toolNamesAndConnections );
    
  bool HaveConfigurationData();

protected:
          //this is the constructor that is called by the factory to 
         //create a new object
  OpenIGTLinkTrackerConfigurationXMLFileReader() : 
    m_ReadingTrackerConfiguration( false ),
    m_ReadingToolConfiguration( false )
    {}
  virtual ~OpenIGTLinkTrackerConfigurationXMLFileReader() {}

  void ProcessToolName() 
  throw ( FileFormatException );

  void ProcessToolAttributes( const char **atts );

  void ProcessToolConnection() 
  throw ( FileFormatException );
  
  void ProcessToolData()
  throw ( FileFormatException );

  bool m_ReadingTrackerConfiguration;
  bool m_ReadingToolConfiguration;
  
  bool                            m_CurrentToolIsReference;
  std::string                     m_CurrentTagData;
  std::string                     m_CurrentToolName;
  std::vector<ConnectionDataType> m_CurrentConnections;
  
  OpenIGTLinkDataType m_ToolNamesAndConnections;

private:
  OpenIGTLinkTrackerConfigurationXMLFileReader( 
    const OpenIGTLinkTrackerConfigurationXMLFileReader & other );


  const OpenIGTLinkTrackerConfigurationXMLFileReader & operator=( 
    const OpenIGTLinkTrackerConfigurationXMLFileReader & right );
};


}
#endif //__igstkOpenIGTLinkTrackerConfigurationXMLFileReader_h
