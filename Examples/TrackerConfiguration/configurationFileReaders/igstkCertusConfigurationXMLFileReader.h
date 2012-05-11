/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkCertusConfigurationXMLFileReader.h,v $
  Language:  C++
  Date:      $Date: 2012-01-27 17:48:03 $
  Version:   $Revision: 1.1 $

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCertusConfigurationXMLFileReader_h
#define __igstkCertusConfigurationXMLFileReader_h

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
class CertusConfigurationXMLFileReader : 
   public TrackerConfigurationXMLFileReaderBase 
{
public:

  //standard typedefs
  typedef CertusConfigurationXMLFileReader          Self;
  typedef TrackerConfigurationXMLFileReaderBase     Superclass;
  typedef itk::SmartPointer<Self>                   Pointer;

  //run-time type information (and related methods)
  itkTypeMacro( CertusConfigurationXMLFileReader, 
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
  CertusConfigurationXMLFileReader() : TrackerConfigurationXMLFileReaderBase()
  { }
  virtual ~CertusConfigurationXMLFileReader() {}

  void ProcessCertusSetupFile() throw ( FileFormatException );

  void ProcessNumberOfRigidBodies() throw ( FileFormatException );

  virtual void ProcessToolData() throw ( FileFormatException );

  void ProcessRigidBodyName() throw ( FileFormatException );

  void ProcessStartMarker() throw ( FileFormatException );

  void ProcessNumberOfMarkers() throw ( FileFormatException );


  
  std::string m_SetupFile;
  std::string m_NumberOfRigidBodies;

  std::string m_CurrentRigidBodyName;
  std::string m_CurrentStartMarker;
  std::string m_CurrentNumberOfMarkers;
  
private:
  CertusConfigurationXMLFileReader( 
    const CertusConfigurationXMLFileReader & other );


  const CertusConfigurationXMLFileReader & operator=( 
    const CertusConfigurationXMLFileReader & right );
};


}
#endif //__igstkCertusConfigurationXMLFileReader_h
