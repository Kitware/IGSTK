/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisHybridConfigurationXMLFileReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkPolarisHybridConfigurationXMLFileReader_h
#define __igstkPolarisHybridConfigurationXMLFileReader_h

#include <itkXMLFile.h>
#include "igstkSerialCommunicatingTrackerConfigurationXMLFileReader.h"
#include "igstkPolarisTrackerConfiguration.h"

namespace igstk
{
/**
 * \class PolarisHybridConfigurationXMLFileReader 
 *
 * \brief This class is used to read the 
 *        xml configuration file for NDI's old hybrid Polaris tracker.
 *
 *        This class is used to read the 
 *        xml configuration file for NDI's old hybrid Polaris tracker.
 */  
class PolarisHybridConfigurationXMLFileReader : 
   public SerialCommunicatingTrackerConfigurationXMLFileReader 
{
public:

  //standard typedefs
  typedef PolarisHybridConfigurationXMLFileReader                  Self;
  typedef SerialCommunicatingTrackerConfigurationXMLFileReader     Superclass;
  typedef itk::SmartPointer<Self>                                  Pointer;

           //run-time type information (and related methods)
  itkTypeMacro( PolarisHybridConfigurationXMLFileReader, 
                SerialCommunicatingTrackerConfigurationXMLFileReader );

         //method for creation through the object factory
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
  PolarisHybridConfigurationXMLFileReader() : 
    SerialCommunicatingTrackerConfigurationXMLFileReader() 
    {m_HaveCurrentControlBoxPort = false;}

  virtual ~PolarisHybridConfigurationXMLFileReader() { }

  void ProcessSromFile() throw ( FileFormatException );

  void 
  ProcessControlBoxPort() throw ( FileFormatException );

  virtual void ProcessToolData() throw ( FileFormatException );

  virtual igstk::PolarisHybridTrackerConfiguration::Pointer
    GetPolarisConfiguration();

  std::string    m_CurrentSromFileName;
  unsigned int   m_CurrentControlBoxPort;
  bool           m_HaveCurrentControlBoxPort;

private:
  PolarisHybridConfigurationXMLFileReader( 
    const PolarisHybridConfigurationXMLFileReader & other );


  const PolarisHybridConfigurationXMLFileReader & operator=( 
    const PolarisHybridConfigurationXMLFileReader & right );
};

}
#endif //__igstkPolarisHybridConfigurationXMLFileReader_h
