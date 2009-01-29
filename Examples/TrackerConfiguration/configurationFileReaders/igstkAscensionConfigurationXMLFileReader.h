/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAscensionConfigurationXMLFileReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkAscensionConfigurationXMLFileReader_h
#define __igstkAscensionConfigurationXMLFileReader_h

#include <itkXMLFile.h>
#include "igstkSerialCommunicatingTrackerConfigurationXMLFileReader.h"


namespace igstk
{
/**
 * \class AscensionConfigurationXMLFileReader 
 *
 * \brief This class is used to read the xml
 *        configuration file for Ascension's FOB tracker.
 *
 *        This class is used to read the xml
 *        configuration file for Ascension's FOB tracker.
 */
class AscensionConfigurationXMLFileReader : 
   public SerialCommunicatingTrackerConfigurationXMLFileReader 
{
public:

  //standard typedefs
  typedef AscensionConfigurationXMLFileReader                        Self;
  typedef SerialCommunicatingTrackerConfigurationXMLFileReader       Superclass;
  typedef itk::SmartPointer<Self>                                    Pointer;

           //run-time type information (and related methods)
  itkTypeMacro( AscensionConfigurationXMLFileReader, 
                SerialCommunicatingTrackerConfigurationXMLFileReader );
             
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
          //this is the constructor that is called by the factory to 
         //create a new object
  AscensionConfigurationXMLFileReader() : 
  SerialCommunicatingTrackerConfigurationXMLFileReader(),
  m_HaveCurrentBirdPort( false ) { }

  virtual ~AscensionConfigurationXMLFileReader() { }

  void ProcessBirdPort() throw ( FileFormatException );

  virtual void ProcessToolData() throw ( FileFormatException );

  virtual double GetMaximalRefreshRate();

  virtual std::string GetSystemType();

  unsigned int  m_CurrentBirdPort;

  bool          m_HaveCurrentBirdPort;

private:
  AscensionConfigurationXMLFileReader( 
    const AscensionConfigurationXMLFileReader & other );


  const AscensionConfigurationXMLFileReader & operator=( 
    const AscensionConfigurationXMLFileReader & right );
};


}
#endif //__igstkAscensionConfigurationXMLFileReader_h
