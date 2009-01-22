/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkAuroraConfigurationXMLFileReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkAuroraConfigurationXMLFileReader_h
#define __igstkAuroraConfigurationXMLFileReader_h

#include <itkXMLFile.h>
#include "igstkSerialCommunicatingTrackerConfigurationXMLFileReader.h"


namespace igstk
{
/**
 * \class AuroraConfigurationXMLFileReader 
 *
 * \brief This class is used to read the xml 
 *        configuration file for NDI's Aurora tracker.
 *
 *        This class is used to read the xml 
 *        configuration file for NDI's Aurora tracker.
 */
class AuroraConfigurationXMLFileReader : 
   public SerialCommunicatingTrackerConfigurationXMLFileReader 
{
public:

  //standard typedefs
  typedef AuroraConfigurationXMLFileReader                        Self;
  typedef SerialCommunicatingTrackerConfigurationXMLFileReader    Superclass;
  typedef itk::SmartPointer<Self>                                 Pointer;

           //run-time type information (and related methods)
  itkTypeMacro( AuroraConfigurationXMLFileReader, 
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
  AuroraConfigurationXMLFileReader() : 
  SerialCommunicatingTrackerConfigurationXMLFileReader(),
  m_HaveCurrentControlBoxPort( false ),
  m_HaveCurrentControlBoxChannel( false ) { }

  virtual ~AuroraConfigurationXMLFileReader() { }

  void ProcessSromFile() throw ( FileFormatException );

  void ProcessControlBoxPort() throw ( FileFormatException );

  void ProcessControlBoxChannel() throw ( FileFormatException );

  virtual void ProcessToolData() throw ( FileFormatException );

  virtual double GetMaximalRefreshRate();

  virtual std::string GetSystemType();

  std::string   m_CurrentSromFileName;
  unsigned int  m_CurrentControlBoxPort;
  unsigned int  m_CurrentControlBoxChannel;

  bool          m_HaveCurrentControlBoxPort;
  bool          m_HaveCurrentControlBoxChannel;

private:
  AuroraConfigurationXMLFileReader( 
    const AuroraConfigurationXMLFileReader & other );


  const AuroraConfigurationXMLFileReader & operator=( 
    const AuroraConfigurationXMLFileReader & right );
};


}
#endif //__igstkAuroraConfigurationXMLFileReader_h
