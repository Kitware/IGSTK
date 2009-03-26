/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTransformXMLFileReaderBase.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkTransformXMLFileReaderBase_h
#define __igstkTransformXMLFileReaderBase_h

#include <itkXMLFile.h>
#include "igstkPrecomputedTransformData.h"


namespace igstk
{
/** \class TransformXMLFileReaderBase
 * 
 *  \brief This class is an abstract reader for xml files containing a 
 *         precomputed transformation. The specific type of transformation is
 *         defined by the subclass.
 *
 *  This class is the base class for all the readers that load a precomputed
 *  transformation. 
 *
 *  The xml file format is as follows:
    @verbatim
    <precomputed_transform>
   
    <description>
       Claron bayonet probe calibration
    </description>
   
    <computation_date>
       Thursday July 4 12:00:00 1776
    </computation_date>
   
    <transformation estimation_error="0.5">
       your parameters go here
    </transformation>
     
   </precomputed_transform>
   @endverbatim
 *
 * 
 */
class TransformXMLFileReaderBase : public itk::XMLReaderBase 
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
  typedef TransformXMLFileReaderBase    Self;
  typedef itk::XMLReaderBase            Superclass;
  typedef itk::SmartPointer<Self>       Pointer;

           //run-time type information (and related methods)
  itkTypeMacro( TransformXMLFileReaderBase, itk::XMLReaderBase );


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
   * Method for checking if the transformation data has been read. When the xml 
   * file is empty or doesn't contain the relevant tags the read operation is 
   * successful, but we don't have the information we need.
   */
  bool HaveTransformData();

  /**
   * Return a pointer to the object containing the transform data.
   */
  igstk::PrecomputedTransformData::Pointer GetTransformData(); 

protected:
          //this is the constructor that is called by the factory to 
         //create a new object
  TransformXMLFileReaderBase() {
    m_ReadingTransformData = false;
    m_HaveDescription = false;
    m_HaveDate = false;
    m_HaveTransform = false;
    m_HaveError = false;
    m_ReadingTagData = false;
    m_Transform = NULL;
    }
  virtual ~TransformXMLFileReaderBase() { delete m_Transform; }

  void ProcessDescription() 
  throw ( FileFormatException );

  void ProcessDate() 
  throw ( FileFormatException );
 
  virtual void ProcessTransformation() 
  throw (FileFormatException ) = 0;

  void ProcessTransformationAttributes( const char ** atts )
  throw ( FileFormatException );


  bool m_ReadingTransformData;
  bool m_ReadingTagData;
  bool m_HaveDescription;
  bool m_HaveDate;
  bool m_HaveTransform;
  bool m_HaveError;

  
  std::string m_CurrentTagData;

  std::string                               m_Description;
  PrecomputedTransformData::DateType        m_Date;
  PrecomputedTransformData::ErrorType       m_EstimationError;
  PrecomputedTransformData::TransformType*  m_Transform; 

private:
  TransformXMLFileReaderBase( 
    const TransformXMLFileReaderBase & other );


  const TransformXMLFileReaderBase & operator=( 
    const TransformXMLFileReaderBase & right );
};


}
#endif //__igstkTransformXMLFileReaderBase_h
