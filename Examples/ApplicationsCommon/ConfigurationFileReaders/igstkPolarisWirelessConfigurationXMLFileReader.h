/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisWirelessConfigurationXMLFileReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkPolarisWirelessConfigurationXMLFileReader_h
#define __igstkPolarisWirelessConfigurationXMLFileReader_h

#include <itkXMLFile.h>
#include "igstkSerialCommunicatingTrackerConfigurationXMLFileReader.h"
#include "igstkPolarisTrackerConfiguration.h"

namespace igstk
{
  
/**
 * \class PolarisWirelessConfigurationXMLFileReader 
 *
 * \brief This class is a super class
 *        for all of the wireless polaris (NDI) trackers. 
 *
 *        This class is a super class
 *        for all of the wireless polaris (NDI) trackers.
 *        NOTE: Only its decendants are actually used.
 */
class PolarisWirelessConfigurationXMLFileReader : 
   public SerialCommunicatingTrackerConfigurationXMLFileReader 
{
public:

  //standard typedefs
  typedef PolarisWirelessConfigurationXMLFileReader               Self;
  typedef SerialCommunicatingTrackerConfigurationXMLFileReader    Superclass;
  typedef itk::SmartPointer<Self>                                 Pointer;

  //run-time type information (and related methods)
  itkTypeMacro( PolarisWirelessConfigurationXMLFileReader, 
                SerialCommunicatingTrackerConfigurationXMLFileReader );

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
  PolarisWirelessConfigurationXMLFileReader() : 
         SerialCommunicatingTrackerConfigurationXMLFileReader() { }
  virtual ~PolarisWirelessConfigurationXMLFileReader() { }

  void ProcessSromFile() throw ( FileFormatException );

  virtual void ProcessToolData() throw ( FileFormatException );

  virtual igstk::PolarisWirelessTrackerConfiguration::Pointer
    GetPolarisConfiguration()=0;

  std::string m_CurrentSromFileName;

private:
  PolarisWirelessConfigurationXMLFileReader( 
    const PolarisWirelessConfigurationXMLFileReader & other );


  const PolarisWirelessConfigurationXMLFileReader & operator=( 
    const PolarisWirelessConfigurationXMLFileReader & right );
};


}
#endif //__igstkPolarisWirelessConfigurationXMLFileReader_h
