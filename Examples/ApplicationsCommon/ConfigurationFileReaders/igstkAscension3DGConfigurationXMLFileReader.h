/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAscension3DGConfigurationXMLFileReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkAscension3DGConfigurationXMLFileReader_h
#define __igstkAscension3DGConfigurationXMLFileReader_h

#include <itkXMLFile.h>
#include "igstkTrackerConfigurationXMLFileReaderBase.h"

namespace igstk
{
/**
 * \class Ascension3DGConfigurationXMLFileReader 
 *
 * \brief This class is used to read the xml
 *        configuration file for Claron Technology Inc. Ascension3DG tracker.
 *
 *        This class is used to read the xml
 *        configuration file for Claron Technology Inc. Ascension3DG tracker.
 */  
class Ascension3DGConfigurationXMLFileReader : 
   public TrackerConfigurationXMLFileReaderBase 
{
public:

  //standard typedefs
  typedef Ascension3DGConfigurationXMLFileReader          Self;
  typedef TrackerConfigurationXMLFileReaderBase     Superclass;
  typedef itk::SmartPointer<Self>                   Pointer;

  //run-time type information (and related methods)
  itkTypeMacro( Ascension3DGConfigurationXMLFileReader, 
                TrackerConfigurationXMLFileReaderBase );

  itkNewMacro( Self );

  /**
   * Method called when a new xml tag start is encountered.
   */
  virtual void StartElement( const char * name, const char **atts ); 


  /**
   * Method called when an xml tag end is encountered.
   */
  virtual void EndElement( const char *name ); 


   /**
   * Method for checking if the configuration data has been read. When the xml 
   * file is empty or doesn't contain the relevant tags the read operation is 
   * successful, but we don't have the information we need.
   */
  virtual bool HaveConfigurationData(); 


  /**
   * Return a pointer to the object containing the configuration data.
   */
  virtual const igstk::TrackerConfiguration::Pointer 
    GetTrackerConfigurationData() throw ( FileFormatException );


protected:

  virtual double GetMaximalRefreshRate();
  virtual std::string GetSystemType();

  //this is the constructor that is called by the factory to 
  //create a new object
  Ascension3DGConfigurationXMLFileReader() : TrackerConfigurationXMLFileReaderBase()
  { }
  virtual ~Ascension3DGConfigurationXMLFileReader() {}
  
  void ProcessPortNumber() throw ( FileFormatException );

  virtual void ProcessToolData() throw ( FileFormatException );

  int m_CurrentPortNumber;

private:
  Ascension3DGConfigurationXMLFileReader( 
    const Ascension3DGConfigurationXMLFileReader & other );


  const Ascension3DGConfigurationXMLFileReader & operator=( 
    const Ascension3DGConfigurationXMLFileReader & right );
};


}
#endif //__igstkAscension3DGConfigurationXMLFileReader_h
