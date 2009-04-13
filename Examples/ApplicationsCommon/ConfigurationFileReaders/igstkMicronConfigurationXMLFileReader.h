/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMicronConfigurationXMLFileReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkMicronConfigurationXMLFileReader_h
#define __igstkMicronConfigurationXMLFileReader_h

#include <itkXMLFile.h>
#include "igstkTrackerConfigurationXMLFileReaderBase.h"

namespace igstk
{
/**
 * \class MicronConfigurationXMLFileReader 
 *
 * \brief This class is used to read the xml
 *        configuration file for Claron Technology Inc. Micron tracker.
 *
 *        This class is used to read the xml
 *        configuration file for Claron Technology Inc. Micron tracker.
 */  
class MicronConfigurationXMLFileReader : 
   public TrackerConfigurationXMLFileReaderBase 
{
public:

  //standard typedefs
  typedef MicronConfigurationXMLFileReader          Self;
  typedef TrackerConfigurationXMLFileReaderBase     Superclass;
  typedef itk::SmartPointer<Self>                   Pointer;

  //run-time type information (and related methods)
  itkTypeMacro( MicronConfigurationXMLFileReader, 
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
  MicronConfigurationXMLFileReader() : TrackerConfigurationXMLFileReaderBase()
  { }
  virtual ~MicronConfigurationXMLFileReader() {}

  void ProcessMicronCalibrationDirectory() throw ( FileFormatException );

  void ProcessMicronInitializationFile() throw ( FileFormatException );

  void ProcessMicronTemplatesDirectory() throw ( FileFormatException );

  void ProcessMarkerName() throw ( FileFormatException );

  virtual void ProcessToolData() throw ( FileFormatException );

  std::string m_MicronCalibrationDirectory;
  std::string m_MicronInitializationFile;
  std::string m_MicronTemplatesDirectory;
  std::string m_CurrentMarkerName;

private:
  MicronConfigurationXMLFileReader( 
    const MicronConfigurationXMLFileReader & other );


  const MicronConfigurationXMLFileReader & operator=( 
    const MicronConfigurationXMLFileReader & right );
};


}
#endif //__igstkMicronConfigurationXMLFileReader_h
